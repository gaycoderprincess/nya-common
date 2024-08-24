#include "enet/enet.h"

namespace NyaNet {
	const uint32_t nMaxPossiblePlayers = 256;

	typedef ENetPeer* NyaNetHandle;
	typedef void (*NyaStatusUpdateCallback) (bool connected, NyaNetHandle handle);
	typedef void (*NyaPacketCallback) (void* data, size_t len, NyaNetHandle handle);

	bool Init();
	bool IsConnected();
	bool IsClient();
	bool IsServer();
	void SetMinTimeout(int value);
	void SetMaxTimeout(int value);
	void SetMaxConnections(int value);
	void SetMaxChannels(int value);
	void QueuePacket(NyaNetHandle handle, void* data, size_t len, bool reliable);
	bool Host(uint16_t port, NyaStatusUpdateCallback statusUpdateCallback, NyaPacketCallback packetCallback);
	void Disconnect();
	bool Connect(uint32_t ip, uint16_t port, NyaStatusUpdateCallback statusUpdateCallback, NyaPacketCallback packetCallback);
	bool SERVER_IsDuplicate(NyaNetHandle handle1, NyaNetHandle handle2);
	void SERVER_DropClient(NyaNetHandle handle);
	void Process();
}