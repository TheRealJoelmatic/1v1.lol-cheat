#include "logger.h"
namespace logger {
	namespace customize {
		bool setConsoleTitle(std::string _consoleTitle) {
			logger::customize::consoleTitle = _consoleTitle;
			return SetConsoleTitleA(_consoleTitle.c_str()) != 0;
		}

		bool setColor(logger::colorSettings option, logger::colors _color) {

			if (option == logger::colorSettings::printColor) {
				logger::customize::printColor = static_cast<WORD>(_color);
			}
			if (option == logger::colorSettings::warringColor) {
				logger::customize::warringColor = static_cast<WORD>(_color);
			}
			if (option == logger::colorSettings::errorColor) {
				logger::customize::errorColor = static_cast<WORD>(_color);
			}
			if (option == logger::colorSettings::timestampColor) {
				logger::customize::timestampColor = static_cast<WORD>(_color);
			}

			return true;
		}
	}

	namespace functions {
		std::string getTimestamp() {
			std::time_t currentTime = std::time(nullptr);
			std::tm localTime;
			localtime_s(&localTime, &currentTime);

			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(2) << localTime.tm_hour << ":"
				<< std::setfill('0') << std::setw(2) << localTime.tm_min << ":"
				<< std::setfill('0') << std::setw(2) << localTime.tm_sec;

			return "[" + oss.str() + "] ";
		}

		bool setConsoleTextColor(WORD _color) {
			return SetConsoleTextAttribute(logger::console::consoleHandle, static_cast<WORD>(_color));
		}
		bool setConsoleTextColor(logger::colors _color) {
			return SetConsoleTextAttribute(logger::console::consoleHandle, static_cast<WORD>(_color));
		}
	}

	//
	//internal stuff
	//
	bool init() {
		AllocConsole();
		AttachConsole(GetCurrentProcessId());

		SetConsoleTitleA(logger::customize::consoleTitle.c_str());

		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);

		console::consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		logger::functions::setConsoleTextColor(colors::fg_white);
		return true;
	}

	//
	// printing stuff
	//

	bool print(std::string _message, std::string _prefix, WORD _prefixColor) {
		std::string timestamp = logger::functions::getTimestamp();
		logger::functions::setConsoleTextColor(logger::customize::timestampColor);
		std::cout << timestamp;
		logger::functions::setConsoleTextColor(_prefixColor);
		std::cout << _prefix + " ";
		logger::functions::setConsoleTextColor(logger::customize::printColor);
		std::cout << _message << std::endl;

		return true;
	}


	bool dbg(std::string _message) {
		logger::print(_message);
		return true;
	}

	bool warring(std::string _message) {
		logger::print(_message, "[Warring]", logger::customize::warringColor);
		return true;
	}
	bool error(std::string _message) {
		logger::print(_message, "[Error]", logger::customize::errorColor);
		return true;
	}
}