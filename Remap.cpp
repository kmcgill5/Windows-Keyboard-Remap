#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32")

#include <algorithm>
#include <iostream>
#include <string>
#include <windows.h>

struct Key {
	std::string name;
	int flag;
	int scanCode;
};

BYTE data[500];   // Stores new keyboard map for Registry
int index = 0;    // Keeps track of placement in data
const int numExtraKeys = 14;
const Key extraKeys[numExtraKeys] = { {"Right-Click", 0xE0, 0x5D},
                                      {"Calculator", 0xE0, 0x21},
                                      {"Scroll Lock", 0x00, 0x46 },
                                      {"Home", 0xE0, 0x47},
                                      {"End", 0xE0, 0x4F},
                                      {"Page Up", 0xE0, 0x49},
                                      {"Page Down", 0xE0, 0x51},
                                      {"Previous Track", 0xE0, 0x10},
                                      {"Next Track", 0xE0, 0x19},
                                      {"Play/Pause", 0xE0, 0x22},
                                      {"Stop", 0xE0, 0x24},
                                      {"Mute", 0xE0, 0x20},
                                      {"Volume Increment", 0xE0, 0x30},
                                      {"Volume Decrement", 0xE0, 0x2E} };

// Displays Menu
void menu();
// Analyzes Windows Keyboard Messages
LRESULT CALLBACK KeyboardProc(int, WPARAM, LPARAM);

int main() {
	std::string input = "";
	while (input != "Q" && input != "q") {
		// Display Menu
		menu();
		input = "";
		
		// Open Registry Key
		HKEY rKey;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout", 0, KEY_ALL_ACCESS, &rKey) != ERROR_SUCCESS) {
			std::cout << "[97mYou need administrative privileges." << std::endl;
			input = "Q";
		}
		
		// Input
		while (input != "R" && input != "r" && input != "D" && input != "d" && input != "Q" && input != "q") {
			std::cout << "[97mWhat do you want to do? [92m";
			input = std::cin.get();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}

		// Remapping
		if ((input == "R" || input == "r") && !(RegQueryValueExA(rKey, "ScanCode Map", NULL, NULL, NULL, NULL) == ERROR_SUCCESS)) {
			HHOOK kHook;      // Keyboard Hook
			MSG msg = {};     // Variable for Windows Messages
			for (int i = 0; i < 12; i++)   // Starts making map
				data[index++] = (i == 8) ? 0x01: 0x00;
			while (input != "N" && input != "n") {
				menu();
				data[8] += 1;   // Increases number of required bytes for mapping
				input = "";
				std::cout << "Select a key to remap." << std::endl;
				kHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
				GetMessage(&msg, NULL, 0, 0);   // Calls function KeyboardProc
				UnhookWindowsHookEx(kHook);
				while (input != "Y" && input != "y" && input != "N" && input != "n") {
					std::cout << "[97mDo you want to map the key to nothing? [92m";
					input = std::cin.get();
					std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				if (input == "Y" || input == "y") {
					data[index++] = 0x00;
					data[index++] = 0x00;
				}
				else {
					input = "";
					while (input != "Y" && input != "y" && input != "N" && input != "n") {
						std::cout << "[97mDo you want to map a key you have to the key? [92m";
						input = std::cin.get();
						std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
					}
					if (input == "Y" || input == "y") {
						std::cout << "[97mSelect the key." << std::endl;
						kHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
						GetMessage(&msg, NULL, 0, 0);   // Calls function KeyboardProc
						UnhookWindowsHookEx(kHook);
					}
					else {
						for (int i = 0; i < numExtraKeys; i++)   // Displays Menu of Extra Keys
							std::cout << "[97m\t[" << i + 1 << ((i + 1 < 10) ? "]   " : "]  ") << extraKeys[i].name << std::endl;
						std::cout << std::endl;
						while (!std::all_of(input.begin(), input.end(), ::isdigit) || input.find('.') != std::string::npos || stoi(input) < 0 || stoi(input) >= numExtraKeys) {
							std::cout << "[97mEnter a number option: [92m";
							std::cin >> input;
							std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
						}
						data[index++] = extraKeys[stoi(input) - 1].scanCode;
						data[index++] = extraKeys[stoi(input) - 1].flag;
					}
				}
				// Reorder Mapping
				for (int i = 1; i < 3; i++) {
					int temp = data[index - i];
					data[index - i] = data[index - i - 2];
					data[index - i - 2] = temp;
				}
				input = "";
				while (input != "Y" && input != "y" && input != "N" && input != "n") {
					std::cout << "[97mWould you like to map another key? [92m";
					input = std::cin.get();
					std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
			}
			// Null Terminator for map
			for (int i = 0; i < 4; i++)
				data[index++] = 0x00;
			
			// Creating Map
			if (RegSetValueExA(rKey, "ScanCode Map", 0, REG_BINARY, data, index) == ERROR_SUCCESS)   // index is data's length
				std::cout << "[97mThe map was created." << std::endl;
			else
				std::cout << "[97mThere was an error." << std::endl;
			RegCloseKey(rKey);
			system("pause > nul");
		}
		else if (input == "R" || input == "r") {   // In case the old map is present
			std::cout << "[97mYou must delete the old map first." << std::endl;
			system("pause > nul");
		}

		// Deleting
		else if (input == "D" || input == "d") {
			// Deleting Map
			if (RegDeleteValueA(rKey, "ScanCode Map") == ERROR_SUCCESS)
				std::cout << "[97mThe map was deleted." << std::endl;
			else
				std::cout << "[97mThere was no map to delete." << std::endl;
			RegCloseKey(rKey);
			system("pause > nul");
		}
	}

	// Cleanup
	std::cout << "[0m";
	return 0;
}

void menu() {
	system("cls");
	std::cout << "[40;96m===================================================================================================" << std::endl;
	std::cout << "[95m                                         Keyboard Remapper" << std::endl;
	std::cout << "[96m===================================================================================================" << std::endl;
	std::cout << "[91m           R: (Remaps)                      [94mD: (Deletes)                      [90mQ: (Quits)" << std::endl;
	std::cout << std::endl;
	std::cout << "[93mThis program requires administrative privileges to work" << std::endl;
	std::cout << "This program cannot do anything with the Pause/Break key" << std::endl;
	std::cout << "You must delete the old map before creating a new map[97m" << std::endl;
	std::cout << std::endl;
}
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0 && wParam == WM_KEYDOWN) {
		// Scan Code
		data[index++] = ((KBDLLHOOKSTRUCT*)lParam)->scanCode;
		// Extended Flag
		if ((((KBDLLHOOKSTRUCT*)lParam)->flags & 1) == 1)
			data[index++] = 0xE0;
		else
			data[index++] = 0x00;
		PostQuitMessage(0);
	}
	// Default Return for other processes
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}