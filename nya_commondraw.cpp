#define STB_IMAGE_IMPLEMENTATION
#include "nya_commonbase.h"

void tNyaStringData::SetColor(const NyaDrawing::CNyaRGBA32* color) {
	this->r = color->r;
	this->g = color->g;
	this->b = color->b;
	this->a = color->a;
}

void tNyaStringData::SetColor(int r, int g, int b, int a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void tNyaStringData::SetOutlineColor(const NyaDrawing::CNyaRGBA32* color) {
	this->outliner = color->r;
	this->outlineg = color->g;
	this->outlineb = color->b;
	this->outlinea = color->a;
}

void tNyaStringData::SetOutlineColor(int r, int g, int b, int a) {
	this->outliner = r;
	this->outlineg = g;
	this->outlineb = b;
	this->outlinea = a;
}

namespace NyaDrawing {
	std::vector<CNyaDrawable*> aDrawList;

	const int nMaxDrawablesPerType = 2048;

	static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a) {
		return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
	}

	void ImageRotated(ImDrawList* draw_list, ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle, ImU32 col) {
		float cos_a = cosf(angle);
		float sin_a = sinf(angle);
		ImVec2 pos[4] = {
			center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
			center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
			center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
			center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)
		};
		ImVec2 uvs[4] = {
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
			ImVec2(0.0f, 1.0f)
		};

		draw_list->AddImageQuad(tex_id, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], col);
	}

	void CNyaRectangle::Draw() const {
		if (!a) return;

		CNyaRGBA32 rgb;
		rgb.r = r;
		rgb.g = g;
		rgb.b = b;
		rgb.a = a;
		ImVec2 v1;
		v1.x = x1 * nResX;
		v1.y = y1 * nResY;
		ImVec2 v2;
		v2.x = x2 * nResX;
		v2.y = y2 * nResY;

		auto drawList = ImGui::GetForegroundDrawList();
		if (texture) {
			if (rotation != 0.0) ImageRotated(drawList, texture, ImVec2((v1.x + v2.x) * 0.5, (v1.y + v2.y) * 0.5), ImVec2(v2.x - v1.x, v2.y - v1.y), rotation, rgb);
			else drawList->AddImageRounded(texture, v1, v2, ImVec2(0, 0), ImVec2(1, 1), rgb, rounding * nResY, 0);
		}
		else drawList->AddRectFilled(v1, v2, rgb, rounding * nResY, 0);
	}

	void CNyaTriangle::Draw() const {
		if (!a) return;

		CNyaRGBA32 rgb;
		rgb.r = r;
		rgb.g = g;
		rgb.b = b;
		rgb.a = a;
		ImVec2 v1;
		v1.x = x1 * nResX;
		v1.y = y1 * nResY;
		ImVec2 v2;
		v2.x = x2 * nResX;
		v2.y = y2 * nResY;
		ImVec2 v3;
		v3.x = x3 * nResX;
		v3.y = y3 * nResY;

		ImGui::GetForegroundDrawList()->AddTriangleFilled(v1, v2, v3, rgb);
	}

	void CNyaText::Draw() const {
		if (!string[0]) return;
		if (!data.a && !data.outlinea) return;

		CNyaRGBA32 rgb;
		rgb.r = data.r;
		rgb.g = data.g;
		rgb.b = data.b;
		rgb.a = data.a;
		ImVec2 v;
		v.x = data.x * nResX;
		v.y = data.y * nResY;

		auto font = ImGui::GetFont();
		float scale = data.size * nResY;
		if (data.XCenterAlign || data.XRightAlign || data.YCenterAlign || data.YBottomAlign) {
			auto textSize = font->CalcTextSizeA(scale, FLT_MAX, 0, string);
			if (data.XCenterAlign) v.x -= (textSize.x * 0.5);
			if (data.XRightAlign) v.x -= textSize.x;
			if (data.YCenterAlign) v.y -= (textSize.y * 0.5);
			if (data.YBottomAlign) v.y -= textSize.y;
		}

		ImVec4 clipRect;
		clipRect.x = data.clipMinX * nResX;
		clipRect.y = data.clipMinY * nResY;
		clipRect.z = data.clipMaxX * nResX;
		clipRect.w = data.clipMaxY * nResY;

		auto drawList = ImGui::GetForegroundDrawList();

		if (data.outlinea) {
			float outlinedistX = data.size * data.outlinedist * nResX * GetAspectRatioInv();
			float outlinedistY = data.size * data.outlinedist * nResY;

			CNyaRGBA32 orgb;
			orgb.b = data.outliner;
			orgb.g = data.outlineg;
			orgb.r = data.outlineb;
			orgb.a = data.outlinea;

			drawList->AddText(font, scale, ImVec2(v.x - outlinedistX, v.y), orgb, string, nullptr, 0.0f, &clipRect);
			drawList->AddText(font, scale, ImVec2(v.x + outlinedistX, v.y), orgb, string, nullptr, 0.0f, &clipRect);
			drawList->AddText(font, scale, ImVec2(v.x, v.y - outlinedistY), orgb, string, nullptr, 0.0f, &clipRect);
			drawList->AddText(font, scale, ImVec2(v.x, v.y + outlinedistY), orgb, string, nullptr, 0.0f, &clipRect);
		}
		drawList->AddText(font, scale, v, rgb, string, nullptr, 0.0f, &clipRect);
	}
	CNyaRectangle aRectangles[nMaxDrawablesPerType];
	CNyaTriangle aTriangles[nMaxDrawablesPerType];
	CNyaText aText[nMaxDrawablesPerType];
	int nNextRectangle = 0;
	int nNextTriangle = 0;
	int nNextText = 0;

	void DrawAll() {
#ifdef NYA_BACKEND_DX8
		ImGui_ImplDX8_NewFrame();
#endif
#ifdef NYA_BACKEND_DX11
		ImGui_ImplDX11_NewFrame();
#endif
		ImGui::NewFrame();

		for (auto& drawable : aDrawList) {
			if (!drawable->topLevel) drawable->Draw();
		}
		for (auto& drawable : aDrawList) {
			if (drawable->topLevel) drawable->Draw();
		}

		ImGui::Render();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = nResX;
		io.DisplaySize.y = nResY;

		ImDrawData* drawData = ImGui::GetDrawData();
		if (drawData->TotalVtxCount > 0) {
#ifdef NYA_BACKEND_DX8
			ImGui_ImplDX8_RenderDrawData(drawData);
#endif
#ifdef NYA_BACKEND_DX11
			ImGui_ImplDX11_RenderDrawData(drawData);
#endif
		}

		NyaDrawing::aDrawList.clear();
		nNextRectangle = 0;
		nNextTriangle = 0;
		nNextText = 0;
	}
}

float GetStringWidth(float size, const char* string) {
	if (!ImGui::GetFont()) return 1;

	float scale = size * nResY;
	return ImGui::GetFont()->CalcTextSizeA(scale, FLT_MAX, 0, string).x / (double)nResX;
}

float GetStringHeight(float size, const char* string) {
	if (!ImGui::GetFont()) return 1;

	float scale = size * nResY;
	return ImGui::GetFont()->CalcTextSizeA(scale, FLT_MAX, 0, string).y / (double)nResY;
}

bool DrawRectangle(float left, float right, float top, float bottom, NyaDrawing::CNyaRGBA32 rgb, float rounding, TEXTURE_TYPE* texture, float rotation) {
	if (NyaDrawing::nNextRectangle >= NyaDrawing::nMaxDrawablesPerType) return false;

	auto& tmp = NyaDrawing::aRectangles[NyaDrawing::nNextRectangle++];
	tmp.x1 = left;
	tmp.x2 = right;
	tmp.y1 = top;
	tmp.y2 = bottom;
	tmp.r = rgb.r;
	tmp.g = rgb.g;
	tmp.b = rgb.b;
	tmp.a = rgb.a;
	tmp.rounding = rounding;
	tmp.texture = texture;
	tmp.rotation = rotation;
	NyaDrawing::aDrawList.push_back(&tmp);

	if (!IsWindowInFocus()) return false;

	auto mouseX = GetMousePosX();
	auto mouseY = GetMousePosY();
	if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom) return true;
	return false;
}

void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, NyaDrawing::CNyaRGBA32 rgb) {
	if (NyaDrawing::nNextTriangle >= NyaDrawing::nMaxDrawablesPerType) return;

	auto& tmp = NyaDrawing::aTriangles[NyaDrawing::nNextTriangle++];
	tmp.x1 = x1;
	tmp.x2 = x2;
	tmp.x3 = x3;
	tmp.y1 = y1;
	tmp.y2 = y2;
	tmp.y3 = y3;
	tmp.r = rgb.r;
	tmp.g = rgb.g;
	tmp.b = rgb.b;
	tmp.a = rgb.a;
	NyaDrawing::aDrawList.push_back(&tmp);
}

void DrawString(tNyaStringData data, const char* string) {
	if (!string) return;
	if (NyaDrawing::nNextText >= NyaDrawing::nMaxDrawablesPerType) return;

	auto& tmp = NyaDrawing::aText[NyaDrawing::nNextText++];
	tmp.data = data;
	tmp.topLevel = data.topLevel;
	strcpy_s(tmp.string, string);
	NyaDrawing::aDrawList.push_back(&tmp);
}

double GetAspectRatioDefault() {
	return (double)nResX / (double)nResY;
}
double GetAspectRatioInv() {
	return 1.0 / GetAspectRatio();
}