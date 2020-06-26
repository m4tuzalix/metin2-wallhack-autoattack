// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <windows.h>
#include <vector>

uintptr_t FinalAddress(uintptr_t ptr, std::vector<DWORD> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i)
    {
        addr = *(uintptr_t*)addr;
        addr += offsets[i];
    }
    return addr; // returns the main pointer from needed assets
}

struct clicks {
    bool wallhack_clicked = FALSE;
    bool autoAttack_clicked = FALSE;
}click;

void attack(int value) {
    DWORD func = 0x0046D810;
    _asm {
        MOV ECX, DWORD PTR DS : [0x667D90]
        SETNE AL
        MOVZX EAX, AL
        PUSH value
        CALL func
    }
}

void wallHack(int value, uintptr_t mod) {
    uintptr_t adres = FinalAddress(mod + 0x267D94, { 0xC, 0x66C });
    int* huj = (int*)adres;
    *huj = value;
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Cheats injected\n" << std::endl;

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"Soria2.pl.exe");
    
    while (true)
    {
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            if (click.wallhack_clicked) {
                click.wallhack_clicked = FALSE;
                wallHack(0, moduleBase);
            }
            else {
                click.wallhack_clicked = TRUE;
                wallHack(1, moduleBase);
            }
        }
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            if (click.autoAttack_clicked) {
                click.autoAttack_clicked = FALSE;
                attack(0);
            }
            else {
                click.autoAttack_clicked = TRUE;
                attack(1);
            }
        }
        Sleep(10);
    }
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

