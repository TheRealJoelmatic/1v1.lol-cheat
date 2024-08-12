#pragma once
#include <cstdint>
namespace sdk{
	bool init();
	void getplayers();
	void cheatLoop();

	inline bool initil2cpp = false;

	namespace offsets {
		inline uintptr_t getmaincamera = 0;
		inline uintptr_t WorldToScreenPoint = 0;
		inline uintptr_t setFOV = 0;

		inline uintptr_t playerController = 0;

		inline uintptr_t set_lockState = 0; //this is for the mouse
		inline uintptr_t set_visible = 0; //this is for the mouse

		inline uintptr_t get_IsMine = 0;

		//expolits

		inline uintptr_t SetCurrentAmmoAmountO = 0;

		namespace assemblys {
			inline uintptr_t gameAssembly = 0;
			inline uintptr_t UnityPlayer = 0;
		}
	}

	namespace system {
		inline float screenWidth = 1920;
		inline float screenHeight = 1080;
	}
}