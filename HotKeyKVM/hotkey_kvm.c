
#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <physicalmonitorenumerationapi.h>
#include <LowLevelMonitorConfigurationAPI.h>

#define VIRTUAL_KEY_P 0x50
#define VIRTUAL_KEY_S 0x53
#define VIRTUAL_KEY_H 0x48
#define MAX_MON 4
#define STR_BUFFER 4096
#pragma comment( lib, "Dxva2" )

PHYSICAL_MONITOR monitors[MAX_MON] = { NULL };
PSTR CAP_STRSTR = NULL;
BYTE VCP_CODE;
BYTE VCP_VALUE;

BOOL CALLBACK LoadMonitors(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    DWORD monSize;
    BOOL success = GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &monSize);
    GetPhysicalMonitorsFromHMONITOR(hMonitor, monSize, monitors);
    return TRUE;
}

void switchMonitor() {
    EnumDisplayMonitors(NULL, NULL, &LoadMonitors, 0);

    for (int i = 0; i < MAX_MON; i++) {
        if (wcslen(monitors[i].szPhysicalMonitorDescription) > 0) {

            CHAR buffer[STR_BUFFER] = { 0 };
            BOOL success = CapabilitiesRequestAndCapabilitiesReply(monitors[i].hPhysicalMonitor, (LPSTR)&buffer, STR_BUFFER);
            if (!success) {
                Sleep(250);
                CapabilitiesRequestAndCapabilitiesReply(monitors[i].hPhysicalMonitor, (LPSTR)&buffer, STR_BUFFER);
            }

            if (strstr(buffer, CAP_STRSTR)) {
                printf("Swichting ...\n");
                SetVCPFeature(monitors[i].hPhysicalMonitor, VCP_CODE, VCP_VALUE);
            }
        }
    }
}

void dumpCaps() {
    EnumDisplayMonitors(NULL, NULL, &LoadMonitors, 0);

    for (int i = 0; i < MAX_MON; i++) {
        if (wcslen(monitors[i].szPhysicalMonitorDescription) > 0) {

            CHAR buffer[STR_BUFFER] = { 0 };
            BOOL success = CapabilitiesRequestAndCapabilitiesReply(monitors[i].hPhysicalMonitor, (LPSTR)&buffer, STR_BUFFER);
            if (!success) {
                Sleep(250);
                CapabilitiesRequestAndCapabilitiesReply(monitors[i].hPhysicalMonitor, (LPSTR)&buffer, STR_BUFFER);
            }

            wprintf(L"Monitor: %s\n", monitors[i].szPhysicalMonitorDescription);
            printf("Caps: %s\n", buffer);
        }
    }
}

int main(int argc, char* argv[]) {
    printf("HotKeyKVM - zerotens\n");
    printf("-----------------------------\n\n");
    if (argc < 4) {
        printf("Missing configurations, exiting ...\n\n");
        printf("%s <Search String Model Name> <VCP CODE Hex> <Wanted Value>\n", argv[0]);
        printf("Example:\n");
        printf("%s U2722DE 0x60 0x0f\n", argv[0]);
        printf("\n\nDumping Caps:\n");
        dumpCaps();
        exit(0);
    }

    CAP_STRSTR = argv[1];
    VCP_CODE = strtol(argv[2], NULL, 16);
    VCP_VALUE = strtol(argv[3], NULL, 16);
    
    printf("\n-----------------------------\n");
    printf("Hide Window: ALT + H\n");
    printf("Show Window: ALT + S\n");
    printf("Switch Monitor: ALT + P\n");
    printf("-----------------------------\n");
    RegisterHotKey(NULL, VIRTUAL_KEY_H, MOD_ALT, VIRTUAL_KEY_H);
    RegisterHotKey(NULL, VIRTUAL_KEY_P, MOD_ALT, VIRTUAL_KEY_P);
    RegisterHotKey(NULL, VIRTUAL_KEY_S, MOD_ALT, VIRTUAL_KEY_S);
    ShowWindow(GetConsoleWindow(), SW_SHOW);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY) {
            switch (msg.wParam) {
                case VIRTUAL_KEY_P:
                    switchMonitor();
                    break;
                case VIRTUAL_KEY_H:
                    ShowWindow(GetConsoleWindow(), SW_HIDE);
                    break;
                case VIRTUAL_KEY_S:
                    ShowWindow(GetConsoleWindow(), SW_SHOW);
                    break;
            }
        }
    }
    return 0;
}
