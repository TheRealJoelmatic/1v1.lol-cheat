#include <windows.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include "../modules/modules.h"

#include "config.h"
using namespace std;

std::filesystem::path GetDllDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::filesystem::path path(buffer);
    return path.parent_path(); // Get the directory containing the DLL
}


bool save_config(std::string filename) {
    std::filesystem::path dllDir = GetDllDirectory();

    std::filesystem::path jmSolutionsDir = dllDir / "JM solutions";
    if (!std::filesystem::exists(jmSolutionsDir)) {
        std::error_code ec;
        std::filesystem::create_directory(jmSolutionsDir, ec);
        if (ec) {
            MessageBoxA(nullptr, (LPCSTR)ec.message().c_str(), "Error", MB_ICONERROR);
            MessageBoxA(nullptr, (LPCSTR)jmSolutionsDir.c_str(), "Error", MB_ICONERROR);
        }
    }

    std::filesystem::path filePath = jmSolutionsDir / filename;

    ofstream outFile(filePath);

    if (!outFile) {
        MessageBoxA(nullptr, "Error opening file for writing!", "Error", MB_ICONERROR);
        return 1;
    }

    outFile << "menukeyBind: " << modules::menukeyBind << endl;
    outFile << "aimkeyBind: " << modules::aimkeyBind << endl;

    outFile << "aimbotEnabled: " << modules::aimbotEnabled << endl;
    outFile << "aimSpeed: " << modules::aimSpeed << endl;
    outFile << "aimSmoothing: " << modules::aimSmoothing << endl;
    outFile << "dontMoveDistance: " << modules::dontMoveDistance << endl;
    outFile << "aimbotModeValue: " << modules::aimbotModeValue << endl;

    outFile << "aimbotFovCircle: " << modules::aimbotFovCircle << endl;
    outFile << "aimbotFovCircleSize: " << modules::aimbotFovCircleSize << endl;

    outFile << "espEnabled: " << modules::espEnabled << endl;

    outFile << "espBoxEnabled: " << modules::espBoxEnabled << endl;
    outFile << "espSkeletonEnabled: " << modules::espSkeletonEnabled << endl;
    outFile << "espTracerEnabled: " << modules::espTracerEnabled << endl;

    outFile << "renderDowned: " << modules::renderDowned << endl;

    outFile << "crossHairEnabled: " << modules::crossHairEnabled << endl;
    outFile << "CrosshairSize: " << modules::CrosshairSize << endl;

    outFile << "fovChanger: " << modules::fovChanger << endl;
    outFile << "newFov: " << modules::newFov << endl;

    outFile << "aimbotTypeValue: " << modules::aimbotTypeValue << endl;
    outFile << "distanceDisplayEnabled: " << modules::distanceDisplayEnabled << endl;

    outFile << "aimbotFovCircleColor: " << modules::aimbotFovCircleColor.red << " " << modules::aimbotFovCircleColor.green << " " << modules::aimbotFovCircleColor.blue << endl;
    outFile << "crosshairColor: " << modules::crosshairColor.red << " " << modules::crosshairColor.green << " " << modules::crosshairColor.blue << endl;
    outFile << "esp: " << modules::esp.red << " " << modules::esp.green << " " << modules::esp.blue << endl;
    outFile << "bot: " << modules::bot.red << " " << modules::bot.green << " " << modules::bot.blue << endl;

    outFile.close();
    return true;
}

bool load_config(const std::string& filename) {
    std::filesystem::path dllDir = GetDllDirectory();
    std::filesystem::path jmSolutionsDir = dllDir / "JM solutions";
    std::filesystem::path filePath = jmSolutionsDir / filename;

    std::ifstream inFile(filePath);
    if (!inFile) {
        // Handle file not found or unable to open
        std::cerr << "Error opening file for reading: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string valueStr = line.substr(pos + 1);

        valueStr.erase(0, valueStr.find_first_not_of(" \t"));
        valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

        if (key == "menukeyBind")
            modules::menukeyBind = std::stoi(valueStr);
        else if (key == "aimkeyBind")
            modules::aimkeyBind = std::stoi(valueStr);
        else if (key == "aimbotEnabled")
            modules::aimbotEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "aimSpeed")
            modules::aimSpeed = std::stoi(valueStr);
        else if (key == "aimSmoothing")
            modules::aimSmoothing = std::stof(valueStr);
        else if (key == "dontMoveDistance")
            modules::dontMoveDistance = std::stof(valueStr);
        else if (key == "aimbotModeValue")
            modules::aimbotModeValue = std::stoi(valueStr);
        else if (key == "aimbotFovCircle")
            modules::aimbotFovCircle = (valueStr == "1" || valueStr == "true");
        else if (key == "aimbotFovCircleSize")
            modules::aimbotFovCircleSize = std::stof(valueStr);
        else if (key == "espEnabled")
            modules::espEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "espBoxEnabled")
            modules::espBoxEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "espSkeletonEnabled")
            modules::espSkeletonEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "espTracerEnabled")
            modules::espTracerEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "renderDowned")
            modules::renderDowned = (valueStr == "1" || valueStr == "true");
        else if (key == "crossHairEnabled")
            modules::crossHairEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "CrosshairSize")
            modules::CrosshairSize = std::stoi(valueStr);
        else if (key == "fovChanger")
            modules::fovChanger = std::stof(valueStr);
        else if (key == "newFov")
            modules::newFov = std::stof(valueStr);
        else if (key == "aimbotTypeValue")
			modules::aimbotTypeValue = std::stoi(valueStr);
        else if (key == "distanceDisplayEnabled")
			modules::distanceDisplayEnabled = (valueStr == "1" || valueStr == "true");
        else if (key == "aimbotFovCircleColor") {
            std::istringstream colorStream(valueStr);
            colorStream >> modules::aimbotFovCircleColor.red >> modules::aimbotFovCircleColor.green >> modules::aimbotFovCircleColor.blue;
        }
        else if (key == "crosshairColor") {
            std::istringstream colorStream(valueStr);
            colorStream >> modules::crosshairColor.red >> modules::crosshairColor.green >> modules::crosshairColor.blue;
        }
        else if (key == "esp") {
            std::istringstream colorStream(valueStr);
            colorStream >> modules::esp.red >> modules::esp.green >> modules::esp.blue;
        }
        else if (key == "bot") {
            std::istringstream colorStream(valueStr);
            colorStream >> modules::bot.red >> modules::bot.green >> modules::bot.blue;
        }
    }

    inFile.close();
    return true;
}