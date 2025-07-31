#include <cstdint>
#include <vector>

#include "imgui.h"

namespace NyaDrawing {
	class CNyaRGBA32;
}

// text data is too complex - a struct is way simpler here than having tons of different function params methinks
struct tNyaStringData {
	float x = 0;
	float y = 0;
	float size = 0.02;
	bool XRightAlign = false;
	bool XCenterAlign = false;
	bool YBottomAlign = false;
	bool YCenterAlign = true;
	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;
	uint8_t a = 255;
	int outliner = 0;
	int outlineg = 0;
	int outlineb = 0;
	int outlinea = 0;
	float outlinedist = 0.0075;
	float clipMinX = 0;
	float clipMinY = 0;
	float clipMaxX = 1;
	float clipMaxY = 1;
	bool topLevel = false;

	void SetColor(const NyaDrawing::CNyaRGBA32*);
	void SetColor(const NyaDrawing::CNyaRGBA32&);
	void SetColor(int r, int g, int b, int a);
	void SetOutlineColor(const NyaDrawing::CNyaRGBA32*);
	void SetOutlineColor(const NyaDrawing::CNyaRGBA32&);
	void SetOutlineColor(int r, int g, int b, int a);
};

namespace NyaDrawing {
	// helper rgb class for casting to int32
	class CNyaRGBA32 {
	public:
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;

		operator unsigned int() { return *(unsigned int*)this; }
	};

	class CNyaDrawable {
	public:
		bool topLevel = false;
		bool drawAfterImGui = false;

		virtual void Draw() const = 0;
	};

	class CNyaRectangle : public CNyaDrawable {
	public:
		float x1 = 0;
		float y1 = 0;
		float x2 = 0;
		float y2 = 0;
		float rounding = 0;
		float rotation = 0;
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;
		TEXTURE_TYPE* texture = nullptr;
		float uvx1 = 0;
		float uvy1 = 0;
		float uvx2 = 1;
		float uvy2 = 1;

		void Draw() const override;
	};

	class CNyaTriangle : public CNyaDrawable {
	public:
		float x1 = 0;
		float y1 = 0;
		float x2 = 0;
		float y2 = 0;
		float x3 = 0;
		float y3 = 0;
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;
		float clipMinX = 0;
		float clipMinY = 0;
		float clipMaxX = 1;
		float clipMaxY = 1;

		void Draw() const override;
	};

	class CNyaText : public CNyaDrawable {
	public:
		tNyaStringData data;
		char string[1024] = "";
		void(*drawFunc)(const tNyaStringData&, const std::string&) = nullptr;

		void Draw() const override;
	};

	class CNyaCallback : public CNyaDrawable {
	public:
		ImDrawCallback callback;

		void Draw() const override;
	};

	void DrawAll();
}

bool DrawRectangle(float left, float right, float top, float bottom, NyaDrawing::CNyaRGBA32 rgb, float rounding = 0, TEXTURE_TYPE* texture = nullptr, float rotation = 0, ImVec2 uvMin = {0,0}, ImVec2 uvMax = {1,1});
void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, NyaDrawing::CNyaRGBA32 rgb, float clipMinX = 0, float clipMinY = 0, float clipMaxX = 1, float clipMaxY = 1);
void DrawString(const tNyaStringData& data, const std::string& string, void(*drawFunc)(const tNyaStringData&, const std::string&) = nullptr);
void DrawCallback(ImDrawCallback data);
float GetStringWidth(float size, const char* string);
float GetStringHeight(float size, const char* string);
double GetAspectRatioDefault();
double GetAspectRatioInv();