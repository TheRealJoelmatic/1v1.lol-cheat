#include "../ext/imgui/imgui.h"
#include "menu.h"
#include "../modules/modules.h"
#include <chrono>
#include "keys/keys.h"
#include "../config/config.h"

using namespace ImGui;

//
// buttons
//

bool waitingforkey1 = false;
bool waitingforkey2 = false;

void Hotkey(int& k, bool& waitingforkey, std::string name)
{
    std::chrono::milliseconds delay = std::chrono::milliseconds(200);
    static std::chrono::steady_clock::time_point lastKeyPressTime;

    if (!waitingforkey) {
        if (ImGui::Button(KeyNames[k]))
            waitingforkey = true;
    }
    else {
        ImGui::Button("...");
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        if (currentTime - lastKeyPressTime >= delay) {
            for (auto& Key : KeyCodes)
            {
                if (Key == VK_LBUTTON) {
                    continue;
                }
                if (name == "1" && Key == VK_RBUTTON) {
                    continue;
                }
                menu::isMenuOpen = true;
                if (GetAsyncKeyState(Key)) {
                    k = Key;
                    waitingforkey = false;
                    lastKeyPressTime = currentTime;
                    menu::isMenuOpen = true;
                    break;
                }
            }

        }
    }
}

void CenterText(const char* text)
{
    if (text == nullptr)
        return;

    ImGui::Spacing();
    ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (ImGui::CalcTextSize(text).x / 2));
    ImGui::Text(text);
    ImGui::Spacing();
}

void colorPicker(RGB& color) {
    ImGui::BeginChild("");

    int red = color.red;
    int green = color.green;
    int blue = color.blue;

    float sliderWidth = 100.0f;


    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderInt("Red", &red, 0, 255);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderInt("Green", &green, 0, 255);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(sliderWidth);
    ImGui::SliderInt("Blue", &blue, 0, 255);
    ImGui::PopItemWidth();

    color = { red,green,blue };

    ImGui::EndChild();
}

//
//menu
//
enum class Tab {
    Aimbot,
    ESP,
    Exploits,
    Customize,
    Config,
    Keybinds
};

int CustomizerPage = 0;

void menu::render() {
	if (!isMenuOpen)
		return;

    ImGui::SetNextWindowSize(ImVec2(1160, 626));
    ImGui::SetNextWindowSizeConstraints(ImVec2(1160, 626), ImVec2(1160, 626));

	static Tab currentTab = Tab::Aimbot; // Default to Aimbot tab

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Begin("1v1.lol - Matic (Made by @Joelmatic)", NULL, ImGuiWindowFlags_NoResize || ImGuiWindowFlags_NoCollapse);
    CenterText("1v1.lol - Matic (Made by @Joelmatic)");
    Separator();
	// Tab selection buttons
	if (ImGui::Button("Aimbot", ImVec2(185, 90)))
		currentTab = Tab::Aimbot;
	ImGui::SameLine();
	if (ImGui::Button("ESP", ImVec2(185, 90)))
		currentTab = Tab::ESP;
	ImGui::SameLine();
	if (ImGui::Button("Exploits", ImVec2(185, 90)))
		currentTab = Tab::Exploits;
    ImGui::SameLine();
    if (ImGui::Button("Config", ImVec2(185, 90)))
        currentTab = Tab::Config;
    ImGui::SameLine();
    if (ImGui::Button("Keybinds", ImVec2(185, 90)))
        currentTab = Tab::Keybinds;
	ImGui::SameLine();
	if (ImGui::Button("Customize", ImVec2(185, 90)))
		currentTab = Tab::Customize;

	ImGui::Separator();
	switch (currentTab) {
    case Tab::Aimbot:
        CenterText(" ");
        CenterText(" Aimbot ");
        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Enable Aimbot", &modules::aimbotEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Aims for you");

        if (ImGui::BeginCombo("##combo", modules::aimbotType[modules::aimbotTypeValue], 0)) {
            for (int i = 0; i < IM_ARRAYSIZE(modules::aimbotType); i++) {
                const bool isSelected = (modules::aimbotTypeValue == i);
                if (ImGui::Selectable(modules::aimbotType[i], isSelected))
                    modules::aimbotTypeValue = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        ImGui::Text("Aim mode");

        if (modules::aimbotModeValue == 0) {
            ImGui::SliderFloat(xor ("Dont Move Distance"), &modules::dontMoveDistance, 0.01f, 0.3f);
        }


        if (ImGui::BeginCombo("##comboForBone", modules::aimbotBone[modules::aimbotBoneModeValue], 0)) {
            for (int i = 0; i < IM_ARRAYSIZE(modules::aimbotBone); i++) {
                const bool isSelected = (modules::aimbotBoneModeValue == i);
                if (ImGui::Selectable(modules::aimbotBone[i], isSelected))
                    modules::aimbotBoneModeValue = i;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::Text("Target Bone");

        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::SliderFloat("Speed", &modules::aimSpeed, 2.0f, 1.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The higher the value, the smother the aim will feel but slower \n If aim is shaky increase the value");

        ImGui::SliderFloat("Smoothing", &modules::aimSmoothing, 0.2f, 0.8f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The lower the vaule the less smooth the aim will be.");

        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Show FOV", &modules::aimbotFovCircle);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Big Circle, were your aimbot will locks one");

        ImGui::SliderFloat("FOV Size", &modules::aimbotFovCircleSize, 0.0f, 10.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("How big you want the FOV Circle to be");

        break;
    case Tab::ESP:
        CenterText(" ");
        CenterText(" ESP ");
        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Enable ESP", &modules::espEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("See all the players on the map");


        //ImGui::Checkbox("Render Team", &modules::renderTeam);
        //if (ImGui::IsItemHovered())
        //    ImGui::SetTooltip("Show player that are knocked down");

        ImGui::Checkbox("Render Downed", &modules::renderDowned);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Show player that are knocked down");

        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Skeleton", &modules::espSkeletonEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("A box around players");

        ImGui::Checkbox("Tracers", &modules::espTracerEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("A line to players");

        ImGui::Checkbox("Distance", &modules::distanceDisplayEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("How far away the player is");

        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Crosshair", &modules::crossHairEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("A nice + in the middle of the screen");

        Separator();
        break;
    case Tab::Exploits:
        CenterText(" ");
        CenterText(" Exploits ");
        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Fov Changer", &modules::fovChanger);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Change your fov");

        ImGui::SliderFloat("Fov", &modules::newFov, 0.0f, 180.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Change your fov");

        break;
    case Tab::Keybinds:
        CenterText(" ");
        CenterText(" Keybinds ");
        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Text(" ");
        ImGui::Text("AimBot Keybind");
        ImGui::SameLine();
        Hotkey(modules::aimkeyBind, waitingforkey1, "");

        ImGui::Text("Menu Keybind");
        ImGui::SameLine();
        Hotkey(modules::menukeyBind, waitingforkey2, "1");

        break;
    case Tab::Config:
        CenterText(" ");
        CenterText(" Config ");
        CenterText(" ");
        Separator();
        CenterText(" ");
        CenterText("Note: Config 1 is automatically loaded when you inject");
        if (ImGui::Button("Load Config 1")) {
            load_config("config1.cfg");
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Config 1")) {
            save_config("config1.cfg");
        }

        CenterText(" ");
        Separator();
        CenterText(" ");

        if (ImGui::Button("Load Config 2")) {
            load_config("config2.cfg");
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Config 2")) {
            save_config("config2.cfg");
        }

        CenterText(" ");
        Separator();
        CenterText(" ");

        if (ImGui::Button("Load Config 3")) {
            load_config("config3.cfg");
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Config 3")) {
            save_config("config3.cfg");
        }

        CenterText(" ");
        Separator();
        CenterText(" ");

        if (ImGui::Button("Load Config 4")) {
            load_config("config4.cfg");
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Config 4")) {
            save_config("config4.cfg");
        }

        CenterText(" ");
        Separator();

        break;
    case Tab::Customize:
        CenterText(" ");
        CenterText(" Customize ");
        CenterText(" ");
        Separator();
        CenterText(" ");

        ImGui::Checkbox("Module List", &modules::moduleList);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("What Cheats your using");
        if (ImGui::Button("Regenerate Module Colors")) {
            modules::moduleColors.clear();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Get cool colors");

        CenterText(" ");
        Separator();

        if (ImGui::Button("Crosshair", ImVec2(185, 90)))
            CustomizerPage = 0;
        ImGui::SameLine();
        if (ImGui::Button("Fov Circle", ImVec2(185, 90)))
            CustomizerPage = 1;
        ImGui::SameLine();
        if (ImGui::Button("Enemies", ImVec2(185, 90)))
            CustomizerPage = 2;
        ImGui::SameLine();
        if (ImGui::Button("Bots", ImVec2(185, 90)))
           CustomizerPage = 3;

        Separator();
        if (CustomizerPage <= 0) {
            ImGui::Text(" ");
            ImGui::Text("Crosshair Color");
            ImGui::SameLine();

            float r = modules::crosshairColor.red / 255.0f;
            float g = modules::crosshairColor.green / 255.0f;
            float b = modules::crosshairColor.blue / 255.0f;

            ImGui::ColorButton("Preview", ImVec4(r, g, b, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);

            colorPicker(modules::crosshairColor);
        }
        else if (CustomizerPage == 1) {
            ImGui::Text(" ");
            ImGui::Text("Fov Circle Color");
            ImGui::SameLine();

            // Normalize color components
            float r = modules::aimbotFovCircleColor.red / 255.0f;
            float g = modules::aimbotFovCircleColor.green / 255.0f;
            float b = modules::aimbotFovCircleColor.blue / 255.0f;

            ImGui::ColorButton("Preview", ImVec4(r, g, b, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);

            colorPicker(modules::aimbotFovCircleColor);
        }
        else if (CustomizerPage == 2) {
            ImGui::Text(" ");
            ImGui::Text("Esp Color");
            ImGui::SameLine();

            // Normalize color components
            float r = modules::esp.red / 255.0f;
            float g = modules::esp.green / 255.0f;
            float b = modules::esp.blue / 255.0f;

            ImGui::ColorButton("Preview", ImVec4(r, g, b, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);

            colorPicker(modules::esp);
        }
        else if (CustomizerPage == 3) {
            ImGui::Text(" ");
            ImGui::Text("Bots Color");
            ImGui::SameLine();

            // Normalize color components
            float r = modules::bot.red / 255.0f;
            float g = modules::bot.green / 255.0f;
            float b = modules::bot.blue / 255.0f;
        
            ImGui::ColorButton("Preview", ImVec4(r, g, b, 1), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip);
        
            colorPicker(modules::bot);
        }
        Separator();
	}
	ImGui::End();
}