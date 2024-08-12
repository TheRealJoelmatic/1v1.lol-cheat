#pragma once

#include <vector>
#include <unordered_map>
#include "../render/renderer.h"
#include "../ext/xor/xor_cc.hpp"

namespace modules {

	//keybinds
	inline int menukeyBind = 0x2D; //insert
	inline int aimkeyBind = 0x02; //rmb

	//menu
	inline std::unordered_map<std::string, ImU32> moduleColors;
	inline std::vector<std::string> moduleNames = {""};
	inline bool moduleList = true;

	//aimbot
	inline bool aimbotEnabled = false;
	inline float aimSpeed = 1.0f;
	inline float aimSmoothing = 0.5f;
	inline float dontMoveDistance = 1;

	inline const char* aimbotMode[] = { "Legit", "Rage" };
	inline int aimbotModeValue = 1;

	inline const char* aimbotType[] = { "Crosshair", "Distance" };
	inline int aimbotTypeValue = 1;

	inline const char* aimbotBone[] = { "Head", "Neck", "Chest", "Feet"};
	inline int aimbotBoneModeValue = 1;

	inline bool aimbotFovCircle = false;
	inline float aimbotFovCircleSize = 2.0f;
	inline float aimbotFovCircleScaleSize = 100.0f;

	//esp
	inline bool espEnabled = false;

	inline bool espBoxEnabled = false;
	inline bool espSkeletonEnabled = false;
	inline bool espTracerEnabled = false;

	inline bool distanceDisplayEnabled = false;

	inline bool renderDowned = false;
	inline bool renderTeam = false;

	inline bool crossHairEnabled = false;
	inline float CrosshairSize = 20.0f;

	//other
	inline bool fovChanger = false;
	inline float newFov = 90.0f;

	//colors
	inline RGB aimbotFovCircleColor = { 255, 255, 255 };
	inline RGB crosshairColor = { 255, 255, 255 };
	inline RGB esp = { 255, 0, 0 };
	inline RGB bot = { 255, 255, 0 };
}