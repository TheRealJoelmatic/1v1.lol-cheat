#include <iostream>

#include "il2cpp.h"
#include "sdk.h"
#include "../ext/IL2CPP_Resolver/IL2CPP_Resolver.hpp"

#include "../ext/imgui/imgui.h"

#include "../ext/xor/xor_cc.hpp"
#include "../ext/logger/logger.h"

#include "../render/hooks.h"
#include "../render/kiero/kiero.h"
#include "../render/renderer.h"

#include "../modules/modules.h"
#include "../render/menu.h"

HWND hWnd;
std::vector <Unity::CComponent*> playerList;

struct Playerinfo {
	Unity::Vector3 position;
	float distance;
};


//
// External Functions
// 


void healPlayer(PlayerHealth_o player, int value) {
	void (*myFunction)(PlayerHealth_o, int) = reinterpret_cast<void(*)(PlayerHealth_o, int)>(reinterpret_cast<char*>(sdk::offsets::SetCurrentAmmoAmountO));
	myFunction(player, value);
}
//
// Until
// 

std::string clean_string(std::string string)
{
	std::vector<char> bytes(string.begin(), string.end());
	bytes.push_back('\0');
	std::list<char> chars;
	for (byte byte : bytes)
	{
		if (byte)
		{
			chars.push_back(byte);
		}
	}
	std::string clean(chars.begin(), chars.end());
	return clean;
}

auto worldToScreenPoint(float z, float x, float y, Unity::CCamera* CameraMain) {
	Unity::Vector3 world = { x, y, z };
	Unity::Vector3 screen = { 0, 0, 0 };
	if (!CameraMain) {
		return screen;
	}


	Unity::Vector3 buffer = CameraMain->CallMethodSafe<Unity::Vector3>(xor ("WorldToScreenPoint"), world, Unity::eye::mono); // Call the worldtoscren function using monoeye (2)

	if (buffer.x > sdk::system::screenWidth || buffer.y > sdk::system::screenHeight || buffer.x < 0 || buffer.y < 0 || buffer.z < 0) // check if point is on screen
	{
		return screen;
	}
	if (buffer.z > 0.0f) // Check if point is in view
	{
		screen = Unity::Vector3(buffer.x, sdk::system::screenHeight - buffer.y, buffer.z);
		return screen;
	}
	if (screen.x > 0 || screen.y > 0) // Check if point is in view
	{
		return screen;
	}
}

Unity::CCamera* find_main_camera()
{
	Unity::CCamera* (UNITY_CALLING_CONVENTION get_main)();
	return reinterpret_cast<decltype(get_main)>(sdk::offsets::getmaincamera)();
}

//sdk::offsets::get_IsMine
PlayerController_o* getLocalPlayer()
{
	PlayerController_o* (UNITY_CALLING_CONVENTION get_main)();
	return reinterpret_cast<decltype(get_main)>(sdk::offsets::get_IsMine)();
}
bool isLocalPlayer(Unity::CCamera* camera, Unity::CGameObject* gameObject) {

	return false;
}


Unity::CComponent* localPlayer = nullptr;
void sdk::getplayers() {

	SetThreadPriority(GetCurrentThread(), THREAD_BASE_PRIORITY_MAX);

	while (true) {
		try {
			if (!sdk::initil2cpp)
				continue;

			Sleep(2000);

			void* m_pPlayersThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());

			playerList.clear();

			Unity::il2cppArray<Unity::CComponent*>* list = Unity::Object::FindObjectsOfType<Unity::CComponent>("PlayerController");

			if (list == nullptr) {
				Sleep(4000);
				continue;
			}

			if (list->m_uMaxLength <= 0) {
				Sleep(4000);
				continue;
			}

			for (int i = 0; i < list->m_uMaxLength; i++) {

				if (!list->operator[](i)) {
					continue;
				}

				playerList.push_back(list->At(i));
			}

			list->RemoveAll();
			IL2CPP::Thread::Detach(m_pPlayersThread);
		}
		catch (...) {
		}
	}
}

//
// Cheats
//

//aimbot
void aimAtPos(float x, float y, float AimSpeed, float Smoothing)
{

	ImVec2 screenSize = ImGui::GetIO().DisplaySize;
	float TargetX = 0;
	float TargetY = 0;
	float ScreenCenterX = screenSize.x / 2.f;
	float ScreenCenterY = screenSize.y / 2.f;

	//X Axis
	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	//Y Axis
	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	// Apply Smoothing

	TargetX = TargetX * Smoothing * (1.0f - Smoothing);
	TargetY = TargetY * Smoothing * (1.0f - Smoothing);

	LPARAM lParam = MAKELPARAM(TargetX, TargetY);
	PostMessageA(hWnd, WM_MOUSEMOVE, 0, lParam);
	mouse_event(MOUSEEVENTF_MOVE, (DWORD)(TargetX), (DWORD)(TargetY), NULL, NULL);
}

void aimbot(std::vector <Playerinfo> players) {
  if (!(GetAsyncKeyState(modules::aimkeyBind) & 0x8000) || menu::isMenuOpen) {
    return;
  }

  const float screenCenterX = sdk::system::screenWidth / 2.f;
  const float screenCenterY = sdk::system::screenHeight / 2.f;

  if (modules::aimbotFovCircleSize == 0.0f) {
    return; // No need to search if FOV size is 0
  }

  Unity::Vector3 closestVec = {};
  float closestDist = FLT_MAX;

  if(modules::aimbotTypeValue == 0){
	  for (const auto& occ : players) {
		  const float xDistance = screenCenterX - occ.position.x;
		  const float yDistance = screenCenterY - occ.position.y;

		  // Use Manhattan distance for potential optimization (benchmark to see if it helps)
		  const float dist = fabsf(xDistance) + fabsf(yDistance);

		  if (dist <= modules::aimbotFovCircleSize * modules::aimbotFovCircleScaleSize) {
			  if (dist < closestDist) {
				  closestDist = dist;
				  closestVec = occ.position;
			  }
		  }
	  }
  }
  else {
	  std::vector <Playerinfo> playersInCircle = {};

	  for (const auto& occ : players)
	  {
		  const float xDistance = screenCenterX - occ.position.x;
		  const float yDistance = screenCenterY - occ.position.y;
		  const float dist = fabsf(xDistance) + fabsf(yDistance);

		  if (dist <= modules::aimbotFovCircleSize * modules::aimbotFovCircleScaleSize) {
			  {
				  playersInCircle.push_back(occ);
			  }
		  }
	  }
	  if (!playersInCircle.empty()) {
		  float smallest_distance = FLT_MAX;

		  size_t closest_index = 0;
		  for (size_t i = 0; i < playersInCircle.size(); ++i) {
			  if (playersInCircle[i].distance < smallest_distance) {
				  smallest_distance = playersInCircle[i].distance;
				  closest_index = i;
			  }
		  }

		  closestVec = playersInCircle[closest_index].position;
	  }
  }

  if (modules::aimbotTypeValue == 0) {
	  if (closestDist == FLT_MAX) {
		  return;
	  }
  }

  const bool withinDeadzone = (fabsf(closestVec.x - screenCenterX) <= 0.01f && fabsf(closestVec.y - screenCenterY) <= 0.01f);

    if (!withinDeadzone) {
      aimAtPos(closestVec.x, closestVec.y, modules::aimSpeed, modules::aimSmoothing);
    }
  
}

//cheat loop
void sdk::cheatLoop() {
	try {
		if (playerList.empty()) {
			return;
		}

		Unity::CCamera* camera = find_main_camera();
		if (camera == nullptr)
			return;

		auto localPlayerController = (Unity::CComponent*)getLocalPlayer();
		auto localPlayer = localPlayerController->GetGameObject();

		if (localPlayerController == nullptr || localPlayer == nullptr) {
			return;
		}
		                                                      
		auto localPlayerHealth = 
		std::vector <Playerinfo> playerPos;

		for (int i = 0; i < playerList.size(); i++) {

			//Unity::CComponent*
			auto component = playerList[i];
			if (component == nullptr)
				continue;

			//Unity::CGameObject*
			auto gameObject = component->GetGameObject();
			if (gameObject == nullptr)
				continue;

			if(localPlayer == gameObject)
				continue;

			bool isdead = component->CallMethodSafe<bool>("get_IsDead");
			bool isdowned = component->CallMethodSafe<bool>("get_IsDowned");

			if (isdead)
				continue;

			if (isdowned && !modules::renderDowned)
				continue;


			//Unity::CTransform* 
			auto transform = component->GetTransform();
			if (transform == nullptr)
				continue;

			auto animator = gameObject->GetComponent("Animator");
			if (animator == nullptr)
				continue;

			Unity::CTransform* bonetrans;

			if (modules::aimbotBoneModeValue == 0) {
				//head
				bonetrans = animator->CallMethodSafe<Unity::CTransform*>("GetBoneTransformInternal", 11);
			}
			else if (modules::aimbotBoneModeValue == 1) {
				bonetrans = animator->CallMethodSafe<Unity::CTransform*>("GetBoneTransformInternal", 10);
			}
			else if (modules::aimbotBoneModeValue == 2) {
				bonetrans = animator->CallMethodSafe<Unity::CTransform*>("GetBoneTransformInternal", 7);
			}
			else if (modules::aimbotBoneModeValue == 3) {
				bonetrans = animator->CallMethodSafe<Unity::CTransform*>("GetBoneTransformInternal", 0);
			}
			else {
				return;
			}

			if (bonetrans == nullptr)
				continue;


			//Unity::Vector3 
			auto GoPos = transform->GetPosition();
			if (GoPos.x == NULL || GoPos.y == NULL || GoPos.x == NULL)
				continue;

			//Unity::Vector3 
			auto pos = bonetrans->GetPosition();
			if (pos.x == NULL || pos.y == NULL || pos.x == NULL)
				continue;

			auto twoDPos = worldToScreenPoint(pos.z, pos.x, pos.y, camera);
			auto twoDGoPos = worldToScreenPoint(GoPos.z, GoPos.x, GoPos.y, camera);

			if (twoDPos.x <= 0 || twoDPos.y <= 0 || twoDPos.x == NULL || twoDPos.y == NULL) {
				continue;
			}
			if (twoDPos.x > sdk::system::screenWidth || twoDPos.y > sdk::system::screenHeight) {
				continue;
			}

			Unity::Vector3 Lpos = localPlayer->GetTransform()->GetPosition();

			float dx = GoPos.x - Lpos.x;
			float dy = GoPos.y - Lpos.y;
			float dz = GoPos.z - Lpos.z;

			float distance = sqrt(dx * dx + dy * dy + dz * dz);

			if (modules::espEnabled) {
				ImU32 color = IM_COL32(modules::esp.red, modules::esp.green, modules::esp.blue, 255);
				if (modules::espTracerEnabled) {

					renderer::drawLine({ twoDGoPos.x, twoDGoPos.y }, { sdk::system::screenWidth / 2, sdk::system::screenHeight }, color);
				}
				if (modules::distanceDisplayEnabled) {
					char distanceString[32] = {};
					sprintf_s(distanceString, xor ("Distance: %.1fm"), distance);

					renderer::drawText(ImVec2(twoDGoPos.x, twoDGoPos.y), color, distanceString, true);
				}

				if (modules::espSkeletonEnabled) {

					std::vector <Unity::Vector3> bones;
					bones.clear();

					for (int j = 0; j < 22; j++) {

						//Unity::CTransform*
						auto _bonetrans = animator->CallMethodSafe<Unity::CTransform*>("GetBoneTransformInternal", j);
						if (_bonetrans == nullptr)
							continue;
						
						//Unity::Vector3
						auto bonepos = _bonetrans->GetPosition();

						//Unity::Vector3
						auto twoDBonePos = worldToScreenPoint(bonepos.z, bonepos.x, bonepos.y, camera);

						//renderer::drawText({ twoDBonePos.x, twoDBonePos.y }, color, std::to_string(j));
						bones.push_back(twoDBonePos);
					}

					//left leg
					renderer::drawLine({bones[21].x, bones[21].y }, {bones[6].x, bones[6].y}, color);
					renderer::drawLine({ bones[6].x, bones[6].y }, { bones[4].x, bones[4].y }, color);
					renderer::drawLine({ bones[4].x, bones[4].y }, { bones[2].x, bones[2].y }, color);
					renderer::drawLine({ bones[2].x, bones[2].y }, { bones[0].x, bones[0].y }, color);

					//right leg
					renderer::drawLine({ bones[20].x, bones[20].y }, { bones[5].x, bones[5].y }, color);
					renderer::drawLine({ bones[5].x, bones[5].y }, { bones[3].x, bones[3].y }, color);
					renderer::drawLine({ bones[3].x, bones[3].y }, { bones[1].x, bones[1].y }, color);
					renderer::drawLine({ bones[1].x, bones[1].y }, { bones[0].x, bones[0].y }, color);

					//torso

					renderer::drawLine({ bones[0].x, bones[0].y }, { bones[7].x, bones[7].y }, color);
					renderer::drawLine({ bones[7].x, bones[7].y }, { bones[8].x, bones[8].y }, color);
					renderer::drawLine({ bones[8].x, bones[8].y }, { bones[9].x, bones[9].y }, color);
					renderer::drawLine({ bones[9].x, bones[9].y }, { bones[10].x, bones[10].y }, color);

					//neck and head
					//11 IS THE HEAD
					renderer::drawLine({ bones[10].x, bones[10].y }, { bones[11].x, bones[11].y }, color);

					//right arm

					renderer::drawLine({ bones[9].x, bones[9].y }, { bones[13].x, bones[13].y }, color);
					renderer::drawLine({ bones[13].x, bones[13].y }, { bones[15].x, bones[15].y }, color);
					renderer::drawLine({ bones[15].x, bones[15].y }, { bones[17].x, bones[17].y }, color);
					renderer::drawLine({ bones[17].x, bones[17].y }, { bones[19].x, bones[19].y }, color);

					//left arm

					renderer::drawLine({ bones[9].x, bones[9].y }, { bones[12].x, bones[12].y }, color);
					renderer::drawLine({ bones[12].x, bones[12].y }, { bones[14].x, bones[14].y }, color);
					renderer::drawLine({ bones[14].x, bones[14].y }, { bones[16].x, bones[16].y }, color);
					renderer::drawLine({ bones[16].x, bones[16].y }, { bones[18].x, bones[18].y }, color);
				}
			}

			playerPos.push_back({ twoDPos, distance });
		}

		if (playerPos.empty()) {
			return;
		}

		if (modules::aimbotEnabled) {
			aimbot(playerPos);
		}

		playerPos.clear();
		return;
	}
	catch (...) {
	}
}

//
// Init
//
bool sdk::init() {
	if (IL2CPP::Initialize()) {
		logger::print("IL2CPP initialized");
	}
	else {
		logger::error(xor ("IL2CPP failed to initialize"));
		return false;
	}

	logger::print("");

	hWnd = FindWindowA(NULL, "1v1_LOL");

	if (hWnd == NULL) {
		logger::error(xor ("1v1_LOL not found"));
		MessageBoxA(NULL, xor ("1v1_LOL not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	sdk::offsets::assemblys::gameAssembly = (uintptr_t)GetModuleHandleA(xor ("GameAssembly.dll"));
	if (sdk::offsets::assemblys::gameAssembly == 0) {
		logger::error(xor ("GameAssembly.dll not found"));
		MessageBoxA(NULL, xor ("GameAssembly.dll not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	logger::print("GameAssembly.dll found at: " + std::to_string(sdk::offsets::assemblys::gameAssembly));

	sdk::offsets::assemblys::UnityPlayer = (uintptr_t)GetModuleHandleA("UnityPlayer.dll");
	if (sdk::offsets::assemblys::UnityPlayer == 0) {
		logger::error(xor ("UnityPlayer.dll not found"));
		MessageBoxA(NULL, xor ("UnityPlayer.dll not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	logger::print(xor ("UnityPlayer.dll found at: ") + std::to_string(sdk::offsets::assemblys::UnityPlayer));

	logger::print("");

	//CAMERA

	Unity::il2cppClass* UnityEngineCamra = IL2CPP::Class::Find(xor ("UnityEngine.Camera"));
	sdk::offsets::getmaincamera = (uintptr_t)IL2CPP::Class::Utils::GetMethodPointer(UnityEngineCamra, xor ("get_main"));
	if (sdk::offsets::getmaincamera == 0) {
		logger::error(xor ("getmaincamera not found"));
		MessageBoxA(NULL, xor ("getmaincamera not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	logger::print("getmaincamera found at: " + std::to_string(sdk::offsets::getmaincamera));

	sdk::offsets::WorldToScreenPoint = (uintptr_t)IL2CPP::Class::Utils::GetMethodPointer(UnityEngineCamra, "WorldToScreenPoint");
	if (sdk::offsets::WorldToScreenPoint == 0) {
		logger::error(xor ("WorldToScreenPoint not found"));
		MessageBoxA(NULL, xor ("WorldToScreenPoint not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	logger::print("WorldToScreenPoint found at: " + std::to_string(sdk::offsets::WorldToScreenPoint));

	sdk::offsets::setFOV = (uintptr_t)IL2CPP::Class::Utils::GetMethodPointer(UnityEngineCamra, xor ("set_fieldOfView"));
	if (sdk::offsets::setFOV == 0) {
		logger::error(xor ("setFov not found"));
		MessageBoxA(NULL, xor ("setFov not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	logger::print("setFov found at: 0x" + std::to_string(sdk::offsets::setFOV));
	logger::print("");


	//player

	Unity::il2cppClass* PlayerController = IL2CPP::Class::Find(xor ("PlayerController"));
	sdk::offsets::get_IsMine = (uintptr_t)IL2CPP::Class::Utils::GetMethodPointer(PlayerController, xor ("get_Mine"));
	if (sdk::offsets::get_IsMine == 0) {
		logger::error(xor ("get_IsMine not found"));
		MessageBoxA(NULL, xor ("get_IsMine not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	logger::print("get_IsMine found at: " + std::to_string(sdk::offsets::get_IsMine));

	// Expilter

	Unity::il2cppClass* WeaponModel = IL2CPP::Class::Find(xor ("PlayerHealth"));
	sdk::offsets::SetCurrentAmmoAmountO = (uintptr_t)IL2CPP::Class::Utils::GetMethodPointer(WeaponModel, xor ("HealPlayer"));
	if (sdk::offsets::SetCurrentAmmoAmountO == 0) {
		logger::error(xor ("SetCurrentAmmoAmountO not found"));
		MessageBoxA(NULL, xor ("SetCurrentAmmoAmountO not found"), xor ("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	logger::print("SetCurrentAmmoAmountO found at: " + std::to_string(sdk::offsets::SetCurrentAmmoAmountO));

	Unity::Object::Initialize();

	logger::print("");
	logger::print(" --> SDK initialized");

	sdk::initil2cpp = true;
	return true;
}