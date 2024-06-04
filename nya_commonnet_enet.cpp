#include "enet/enet.h"

#include <mutex>
#include <vector>
#include <thread>

#include "nya_commonnet_enet.h"

namespace NyaNet {
	bool bInited = false;

	bool Init() {
		if (bInited) return true;
		if (enet_initialize()) return false;
		atexit(enet_deinitialize);
		bInited = true;
		return true;
	}

	std::mutex mNetMutex;
	ENetHost* pLocalHost = nullptr;
	ENetPeer* pRemoteHost = nullptr;
	NyaStatusUpdateCallback pStatusUpdateCallback = nullptr;
	NyaPacketCallback pPacketCallback = nullptr;

	bool IsConnected() {
		return pLocalHost;
	}
	bool IsClient() {
		return pRemoteHost;
	}
	bool IsServer() {
		return pLocalHost && !pRemoteHost;
	}

	uint32_t nMinTimeout = 10000;
	uint32_t nMaxTimeout = 30000;
	uint32_t nMaxConnections = nMaxPossiblePlayers;
	uint32_t nMaxChannels = 2;

	void SetMinTimeout(int value) { nMinTimeout = value; }
	void SetMaxTimeout(int value) { nMaxTimeout = value; }
	void SetMaxConnections(int value) { nMaxConnections = value; }
	void SetMaxChannels(int value) { nMaxChannels = value; }

	struct tStatusUpdate {
		bool connected;
		NyaNetHandle handle = nullptr;
	};

	struct tNyaPacket {
		uint8_t* data = nullptr;
		size_t len = 0;
		bool reliable = false;
		NyaNetHandle handle = nullptr;
	};
	std::vector<tStatusUpdate> aReceivedStatusUpdates;
	std::vector<tNyaPacket> aReceivedPackets;
	std::mutex mReceivedDataMutex;

	std::vector<tNyaPacket> aPacketQueue;
	std::mutex mPacketQueueMutex;

	void QueuePacket(NyaNetHandle handle, void* data, size_t len, bool reliable) {
		mPacketQueueMutex.lock();
		tNyaPacket packet;
		packet.data = new uint8_t[len];
		memcpy(packet.data, data, len);
		packet.len = len;
		packet.reliable = reliable;
		packet.handle = handle;
		aPacketQueue.push_back(packet);
		mPacketQueueMutex.unlock();
	}

	void SendPacket(NyaNetHandle handle, void* data, size_t len, bool reliable) {
		if (!data) return;
		if (!len) return;

		ENetPacket* packet = enet_packet_create(data, len, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		if (handle) enet_peer_send(handle, 0, packet);
		else if (pRemoteHost) enet_peer_send(pRemoteHost, 0, packet);
		else enet_host_broadcast(pLocalHost, 0, packet);
		enet_host_flush(pLocalHost);
	}

	void ClearPacketQueue() {
		mPacketQueueMutex.lock();
		for (auto& packet : aPacketQueue) delete[] packet.data;
		aPacketQueue.clear();
		mPacketQueueMutex.unlock();
	}

	void ProcessPacketQueue() {
		mPacketQueueMutex.lock();
		std::vector<tNyaPacket> tmpQueue = aPacketQueue;
		aPacketQueue.clear();
		mPacketQueueMutex.unlock();
		for (auto& packet : tmpQueue) {
			SendPacket(packet.handle, packet.data, packet.len, packet.reliable);
			delete[] packet.data;
		}
	}

	void ServerLoop() {
		while (true) {
			mNetMutex.lock();

			if (!pLocalHost) {
				ClearPacketQueue();
				mNetMutex.unlock();
				return;
			}

			ProcessPacketQueue();

			ENetEvent event;
			while (enet_host_service(pLocalHost, &event, 0) > 0) {
				mNetMutex.unlock();
				switch (event.type) {
					case ENET_EVENT_TYPE_CONNECT:
						mReceivedDataMutex.lock();
						aReceivedStatusUpdates.push_back({true, event.peer});
						mReceivedDataMutex.unlock();
						event.peer->timeoutMinimum = nMinTimeout;
						event.peer->timeoutMaximum = nMaxTimeout;
						break;
					case ENET_EVENT_TYPE_RECEIVE: {
						mReceivedDataMutex.lock();
						tNyaPacket packet;
						auto data = new uint8_t[event.packet->dataLength];
						memcpy(data, event.packet->data, event.packet->dataLength);
						packet.data = data;
						packet.len = event.packet->dataLength;
						packet.handle = event.peer;
						aReceivedPackets.push_back(packet);
						mReceivedDataMutex.unlock();

						enet_packet_destroy(event.packet);
					} break;
					case ENET_EVENT_TYPE_DISCONNECT:
						mReceivedDataMutex.lock();
						aReceivedStatusUpdates.push_back({false, event.peer});
						mReceivedDataMutex.unlock();
						break;
					default:
						break;
				}
				mNetMutex.lock();
			}
			mNetMutex.unlock();
			Sleep(0);
		}
	}

	bool Host(uint16_t port, NyaStatusUpdateCallback statusUpdateCallback, NyaPacketCallback packetCallback) {
		if (pLocalHost) return false;
		if (!Init()) return false;

		mNetMutex.lock();
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;
		pLocalHost = enet_host_create(&address, nMaxConnections, nMaxChannels, 0, 0);
		mNetMutex.unlock();

		if (!pLocalHost) return false;

		pStatusUpdateCallback = statusUpdateCallback;
		pPacketCallback = packetCallback;
		std::thread(ServerLoop).detach();
		return true;
	}
	void SERVER_DropClient(NyaNetHandle handle) {
		if (!handle) return;

		mNetMutex.lock();
		enet_peer_reset(handle);
		mNetMutex.unlock();
	}
	void Disconnect() {
		if (!pLocalHost) return;

		mNetMutex.lock();
		if (pRemoteHost) {
			ENetEvent event;
			enet_peer_disconnect(pRemoteHost, 0);
			while (enet_host_service(pLocalHost, &event, 1000) > 0) {
				switch (event.type) {
					case ENET_EVENT_TYPE_RECEIVE:
						enet_packet_destroy(event.packet);
						break;
					case ENET_EVENT_TYPE_DISCONNECT:
						goto end;
					default:
						break;
				}
			}
			enet_peer_reset(pRemoteHost);
		}

	end:
		enet_host_destroy(pLocalHost);
		pLocalHost = nullptr;
		pRemoteHost = nullptr;
		mNetMutex.unlock();
	}
	void ClientLoop() {
		while (true) {
			mNetMutex.lock();

			if (!pLocalHost) {
				ClearPacketQueue();
				mNetMutex.unlock();
				return;
			}

			ProcessPacketQueue();

			ENetEvent event;
			while (enet_host_service(pLocalHost, &event, 0) > 0) {
				mNetMutex.unlock();
				switch (event.type) {
					case ENET_EVENT_TYPE_RECEIVE: {
						mReceivedDataMutex.lock();
						tNyaPacket packet;
						auto data = new uint8_t[event.packet->dataLength];
						memcpy(data, event.packet->data, event.packet->dataLength);
						packet.data = data;
						packet.len = event.packet->dataLength;
						packet.handle = pRemoteHost;
						aReceivedPackets.push_back(packet);
						mReceivedDataMutex.unlock();

						enet_packet_destroy(event.packet);
					} break;
					default:
						break;
				}
				mNetMutex.lock();
			}
			mNetMutex.unlock();
		}
	}
	bool Connect(uint32_t ip, uint16_t port, NyaStatusUpdateCallback statusUpdateCallback, NyaPacketCallback packetCallback) {
		if (pLocalHost) return false;
		if (!Init()) return false;

		mNetMutex.lock();
		pLocalHost = enet_host_create(nullptr, 1, nMaxChannels, 0, 0);
		mNetMutex.unlock();

		if (!pLocalHost) return false;

		mNetMutex.lock();
		ENetAddress address;
		address.host = ip;
		address.port = port;
		pRemoteHost = enet_host_connect(pLocalHost, &address, nMaxChannels, 0);
		mNetMutex.unlock();

		if (!pRemoteHost) return false;

		ENetEvent event;

		mNetMutex.lock();
		if (enet_host_service(pLocalHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
			event.peer->timeoutMinimum = nMinTimeout;
			event.peer->timeoutMaximum = nMaxTimeout;

			mNetMutex.unlock();
			statusUpdateCallback(true, nullptr);

			pStatusUpdateCallback = statusUpdateCallback;
			pPacketCallback = packetCallback;
			std::thread(ClientLoop).detach();
			return true;
		}

		mNetMutex.unlock();
		enet_peer_reset(pRemoteHost);
		pRemoteHost = nullptr;
		enet_host_destroy(pLocalHost);
		pLocalHost = nullptr;
		statusUpdateCallback(false, nullptr);
		return false;
	}

	void Process() {
		mReceivedDataMutex.lock();
		for (auto& update : aReceivedStatusUpdates) {
			pStatusUpdateCallback(update.connected, update.handle);
		}
		for (auto& packet : aReceivedPackets) {
			if (!packet.data) continue;
			pPacketCallback(packet.data, packet.len, packet.handle);
			delete[] packet.data;
		}
		aReceivedStatusUpdates.clear();
		aReceivedPackets.clear();
		mReceivedDataMutex.unlock();
	}
}