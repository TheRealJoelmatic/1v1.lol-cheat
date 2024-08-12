#pragma once

#include <string>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")


#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx11.h"

struct RGB {
	int red, green, blue;
};

namespace renderer
{
	void drawLine(const ImVec2& a, const ImVec2& b, ImU32 color, bool shadow = true);
	void drawCross(const ImVec2& center, float size, ImU32 color, bool shadow = false);
	void drawBox(const ImVec2& centerPos, float width, float height, ImU32 color, bool shadow = true);
	void drawText(const ImVec2& a, ImU32 color, const std::string& text, bool shadow = true);
	void drawCircleOutline(const ImVec2& center, float radius, ImU32 color);
	void watermark();
}
