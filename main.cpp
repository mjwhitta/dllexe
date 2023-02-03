// DO NOT EDIT BELOW
// ...
// unless you know what you're doing...

// Shared entry point, needs impl in payload.cpp
int entry(int argc, char** argv);

#ifndef DLL
// EXE main just calls shared entry
int main(int argc, char** argv) {
    return entry(argc, argv);
}
#else
#include <windows.h>

//
// Standard DLL exports call shared entry
//
__declspec(dllexport) void CALLBACK DllCanUnloadNow(
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
);

__declspec(dllexport) void CALLBACK DllGetClassObject(
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
);

__declspec(dllexport) void CALLBACK DllRegisterServer(
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
);

__declspec(dllexport) void CALLBACK DllUnregisterServer(
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
);

// Custom DLL export for rundll32
extern "C" __declspec(dllexport) void CALLBACK Start (
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
) {}

// DLL entry needs to be called in a new thread, so this function
// wraps it to meet the expected function signature
DWORD WINAPI DLLAttach(LPVOID lpParam) {
    return entry(0, 0);
}

// DLLMain will create a new thread for DLL_PROCESS_ATTACH, otherwise
// do nothing
BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:  {
            HANDLE t = CreateThread(0, 0, DLLAttach, 0, 0, 0);
            if (t) CloseHandle(t);
            break;
          }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
#endif
