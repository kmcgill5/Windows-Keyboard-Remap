#pragma comment(lib, "User32")

#include <iostream>
#include <windows.h>

LRESULT CALLBACK KeyboardProc(int, WPARAM, LPARAM);

int main() {
	// Variables
	HHOOK kHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
	MSG msg = {};
	
	// Input
	std::cout << "What key do you want to identify?" << std::endl;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		std::cout << "";
	}
	
	UnhookWindowsHookEx(kHook);
	
	return 0;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0 && wParam == WM_KEYUP) {
		if ((((KBDLLHOOKSTRUCT*)lParam)->flags & 1) == 1)
			std::cout << "E0 ";
		else
			std::cout << "00 ";
		std::cout << std::hex << std::uppercase << ((KBDLLHOOKSTRUCT*)lParam)->scanCode << std::endl;
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}