#include <vector>
#include <random>
#include <unordered_map>

#include "../modules/modules.h"
#include "renderer.h"
#include "../ext/xor/xor_cc.hpp"
#include "../sdk/sdk.h"



void renderer::drawLine(const ImVec2& a, const ImVec2& b, ImU32 color, bool shadow)
{
    if ((a.x == 0 && a.y == 0) || (b.x == 0 && b.y == 0)) {
        return;
    }

    if (a.x > sdk::system::screenWidth || a.y > sdk::system::screenHeight) {
        return;
    }

    if (b.x > sdk::system::screenWidth || b.y > sdk::system::screenHeight) {
        return;
    }

	if (shadow)
		ImGui::GetForegroundDrawList()->AddLine(a, b, IM_COL32_BLACK, 3.f);

	ImGui::GetForegroundDrawList()->AddLine(a, b, color, 1.f);
}

void renderer::drawBox(const ImVec2& centerPos, float width, float height, ImU32 color, bool shadow) {

    if (centerPos.x > sdk::system::screenWidth || centerPos.y > sdk::system::screenHeight) {
        return;
    }

    ImVec2 topLeft = ImVec2(centerPos.x - width / 2, centerPos.y - height / 2);
    ImVec2 topRight = ImVec2(centerPos.x + width / 2, centerPos.y - height / 2);
    ImVec2 bottomLeft = ImVec2(centerPos.x - width / 2, centerPos.y + height / 2);
    ImVec2 bottomRight = ImVec2(centerPos.x + width / 2, centerPos.y + height / 2);

    renderer::drawLine(topLeft, topRight, color, shadow);
    renderer::drawLine(topRight, bottomRight, color, shadow);
    renderer::drawLine(bottomRight, bottomLeft, color, shadow);
    renderer::drawLine(bottomLeft, topLeft, color, shadow);
}

void renderer::drawCross(const ImVec2& center, float size, ImU32 color, bool shadow)
{
    float thickness = 2.0f;

    ImVec2 horizontalStart(center.x - size, center.y - thickness / 2.0f);
    ImVec2 horizontalEnd(center.x + size, center.y + thickness / 2.0f);
    ImVec2 verticalStart(center.x - thickness / 2.0f, center.y - size);
    ImVec2 verticalEnd(center.x + thickness / 2.0f, center.y + size);

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(horizontalStart, horizontalEnd, color);
    drawList->AddRectFilled(verticalStart, verticalEnd, color);
}

void renderer::drawCircleOutline(const ImVec2& center, float radius, ImU32 color) {
	ImGui::GetForegroundDrawList()->AddCircle(center, radius, color, 12, 2.0f);
}

void renderer::drawText(const ImVec2& a, ImU32 color, const std::string& text, bool shadow)
{
	if (shadow)
	{
		//ImGui::GetForegroundDrawList()->AddText(a + ImVec2(0.f, 1.f), IM_COL32_BLACK, text.c_str());
		//ImGui::GetForegroundDrawList()->AddText(a - ImVec2(0.f, 1.f), IM_COL32_BLACK, text.c_str());
		//ImGui::GetForegroundDrawList()->AddText(a - ImVec2(1.f, 0.f), IM_COL32_BLACK, text.c_str());
		//ImGui::GetForegroundDrawList()->AddText(a + ImVec2(1.f, 0.f), IM_COL32_BLACK, text.c_str());
	}

	ImGui::GetForegroundDrawList()->AddText(a, color, text.c_str());
}

void renderer::watermark() {

    modules::moduleNames.clear();
    modules::moduleNames.push_back(xor("MATIC - 1v1.lol"));

    if (modules::moduleList) {
        if (modules::aimbotEnabled) {
            std::string _text = xor ("Aimbot - ") + std::string(modules::aimbotType[modules::aimbotTypeValue]);
            modules::moduleNames.push_back(_text);
        }
        if (modules::espEnabled) {
            modules::moduleNames.push_back(xor("ESP"));
            if (modules::espSkeletonEnabled) {
                modules::moduleNames.push_back(xor ("Skeletons"));
            }
            if (modules::espTracerEnabled) {
                modules::moduleNames.push_back(xor ("Tracers"));
            }
        }

        if (modules::crossHairEnabled) {
            modules::moduleNames.push_back(xor("Crosshair"));
        }
        if (modules::fovChanger) {
			modules::moduleNames.push_back(xor("FOV Changer"));
        }
    }

    float spacing = 5.0f;
    float paddingX = 100.0f;
    float paddingY = 100.0f;
    float cornerRadius = 5.0f;
    ImU32 backgroundColor = IM_COL32(0, 0, 0, 200);

    float totalWidth = 0.0f;
    for (const auto& moduleName : modules::moduleNames) {
        totalWidth = max(totalWidth, ImGui::CalcTextSize(moduleName.c_str()).x);
    }

    float totalHeight = 0.0f;
    for (const auto& moduleName : modules::moduleNames) {
        totalHeight += ImGui::CalcTextSize(moduleName.c_str()).y + spacing;
    }

    ImVec2 startPosition = ImVec2(ImGui::GetIO().DisplaySize.x - totalWidth - paddingX, paddingY);

    ImVec2 backgroundTopLeft = ImVec2(startPosition.x - 10, startPosition.y - 10);
    ImVec2 backgroundBottomRight = ImVec2(startPosition.x + totalWidth + 10, startPosition.y + totalHeight + 10);
    ImGui::GetForegroundDrawList()->AddRectFilled(backgroundTopLeft, backgroundBottomRight, backgroundColor, cornerRadius);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (const auto& moduleName : modules::moduleNames) {
        if (modules::moduleColors.find(moduleName) == modules::moduleColors.end()) {
            modules::moduleColors[moduleName] = IM_COL32(dis(gen), dis(gen), dis(gen), 255);
        }

        ImVec2 textSize = ImGui::CalcTextSize(moduleName.c_str());
        drawText(startPosition, modules::moduleColors[moduleName], moduleName, true);
        startPosition.y += textSize.y + spacing;
    }
}