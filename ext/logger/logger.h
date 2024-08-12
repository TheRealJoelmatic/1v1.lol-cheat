#pragma once
#ifndef LOOGER_H
#define LOOGER_H
#include <Windows.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace logger {
	//
	// Code stuff
	// 
	
	namespace console {
		inline HANDLE consoleHandle;
	}

	//
	//color stuff
	//
	enum class colors : WORD {
		fg_red = 0x0004,
		fg_l_red = 0x000C,

		// greens
		Wfg_green = 0x0002,
		fg_l_green = 0x000A,

		// blues
		fg_cyan = 0x000B,
		fg_blue = 0x0009,
		fg_l_blue = 0x0003,
		fg_d_blue = 0x0001,

		fg_purple = 0x0005,
		fg_l_purple = 0x000D,

		// yellows
		fg_yellow = 0x0006,
		fg_l_yellow = 0x000E,

		// grays
		fg_white = 0x000F,
		fg_l_gray = 0x0007,
		fg_gray = 0x0008
	};

	enum class colorSettings {
		printColor,
		warringColor,
		errorColor,
		timestampColor
	};

	namespace customize {
		inline WORD printColor = static_cast<WORD>(colors::fg_white);

		inline WORD dbgColor = static_cast<WORD>(colors::fg_blue);
		inline WORD warringColor = static_cast<WORD>(colors::fg_yellow);
		inline WORD errorColor = static_cast<WORD>(colors::fg_red);

		inline WORD timestampColor = static_cast<WORD>(colors::fg_purple);

		inline std::string consoleTitle = "Dbg console";

		inline bool timestamp = true;

		bool setConsoleTitle(std::string _consoleTitle);
		bool setColor(colorSettings option, colors _color);
	}

	namespace functions {
		std::string getTimestamp();
		bool setConsoleTextColor(WORD _color);
		bool setConsoleTextColor(colors _color);
	}

	bool init();
	bool print(std::string _message, std::string _prefix = "[dbg]", WORD _prefixColor = static_cast<WORD>(logger::customize::dbgColor));
	bool dbg(std::string _message);
	bool warring(std::string _message);
	bool error(std::string _message);
}
#endif