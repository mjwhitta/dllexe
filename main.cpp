// DO NOT EDIT BELOW
// ...
// unless you know what you're doing...
#include <windows.h>

// Shared entry point, needs impl in payload.cpp
int entry(int argc, char** argv);

// Use mutex for single execution per user
boolean UserAlreadyRunning() {
    DWORD namelen = 256;
    char username[namelen];

    GetUserNameA(username, &namelen);
    CreateMutex(0, TRUE, username);

    return (GetLastError() == ERROR_ALREADY_EXISTS);
}

#ifndef DLL
// EXE main just calls shared entry
int main(int argc, char** argv) {
    // Uncomment below for single execution per user
    // if (UserAlreadyRunning()) exit(0);
    return entry(argc, argv);
}
#else
//
// Standard DLL exports
//
extern "C" __declspec(dllexport) HRESULT CALLBACK DllCanUnloadNow() {
    Sleep(1);
    return 0;
}

extern "C" __declspec(dllexport) HRESULT CALLBACK DllGetClassObject(
    REFCLSID rclsid, REFIID riid, LPVOID* ppv
) {
    Sleep(1);
    return 0;
}

extern "C" __declspec(dllexport) void CALLBACK DllRegisterServer(
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
) {
    Sleep(1);
}

extern "C" __declspec(dllexport) void CALLBACK DllUnregisterServer(
    HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow
) {
    Sleep(1);
}

// DLL entry needs to be called in a new thread, so this function
// wraps it to meet the expected function signature
DWORD WINAPI DllAttach(LPVOID lpParam) {
    if (UserAlreadyRunning()) exit(0);
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
        case DLL_PROCESS_ATTACH: {
            HANDLE t = CreateThread(0, 0, DllAttach, 0, 0, 0);
            if (t) CloseHandle(t);
            break;
        }
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}
#endif

// Custom DLL export for rundll32
extern "C" __declspec(dllexport) void CALLBACK Start() {
    Sleep(1);
}
