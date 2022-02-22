//QMLaunch by Lapisz
//imports and definitions
#include <iostream>
#include <process.h>
#include <windows.h>
#include "ExternalUtils.h"
#include "QMLUtils.h"
#include <ctime>

using namespace std;

//==========functions==========
int monitoringLoop();

int runFile(const char* pathToFile, const char* args, bool hideWindow);
int runHiddenShellCmd(const char* command);
//=============================

//global vars
const char* VERSION = "1.1";
const wchar_t* STATIC_DESC = L"dont touch!!! for qml app use only";

int main(int argc, char* argv[])
{   
    char* path_charPtr = get_current_exe_path();
    LPWSTR path = charArr_to_LPWSTR(path_charPtr);
    char* fileName = reverse_splicer(path_charPtr, '.', '\\');

    //commandline handler
    if (argc == 2) {
        if (strcmp(argv[1], "enableOnStartup") == 0) {
            HINSTANCE hI = ShellExecute(NULL, L"runas", path, L"genShortcutAlreadyHasAdmin", NULL, SW_HIDE);
            check_hi_success(hI, true, "Enabled on startup", "Failed to enable on startup. Maybe run as admin?");
        }
        else if (strcmp(argv[1], "genShortcutAlreadyHasAdmin") == 0) {
            //puts system drive letter into a variable
            char* systemDriveBuf;
            size_t bufferSize;
            _dupenv_s(&systemDriveBuf, &bufferSize, "SYSTEMDRIVE");

            //make string for the location of the shortcut
            char* shortcutPath = concat_charArr(systemDriveBuf, "\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\", fileName, ".lnk");
            LPWSTR args = charArr_to_LPWSTR("run");
            char* startInPath_charPtr = reverse_splicer(path_charPtr, '\\', '\0');
            LPWSTR startInPath = charArr_to_LPWSTR(startInPath_charPtr);

            CreateLink(path, startInPath, args, shortcutPath, L"dont touch!!! for qml app use only");
        }
        else if (strcmp(argv[1], "delShortcutAlreadyHasAdmin") == 0) {
            //see genShortcutAlreadyHasAdmin////
            /**/ char* systemDriveBuf;
            /**/ size_t bufferSize;
            /**/ _dupenv_s(&systemDriveBuf, &bufferSize, "SYSTEMDRIVE");
            /**/
            /**/ char* shortcutPath = concat_charArr(systemDriveBuf, "\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\", fileName, ".lnk");
            ////////////////////////////////////

            remove(shortcutPath);
        }
        else if (strcmp(argv[1], "help") == 0) {
            cout << "QMLaunch v" << VERSION << " by Lapisz - https://lapisz.net" << endl;
            cout << endl;
            cout << fileName << " [args]" << endl;
            cout << "args:" << endl;
            cout << "...   disableOnStartup - disables running on startup (will request admin access)" << endl;
            cout << "...   enableOnStartup  - enables running on startup (will request admin access)" << endl;
            cout << "...   help             - displays this screen" << endl;
            cout << "...   run              - quietly runs runnable.bat or afk mining depending on config" << endl;
            cout << "...   stop             - stops the mining" << endl;
        }
        else if (strcmp(argv[1], "run") == 0) {
            if (/*conditional statement from YAML to be added*/true) {
                ShellExecute(NULL, L"open", path, L"runMonitor", NULL, SW_HIDE);
            }
            else {
                //the runHiddenShellCmd is not executed directly because it doesnt work properly when ran from an existing cmd.exe window, simlilar thing with the above runMonitor
                ShellExecute(NULL, L"open", path, L"runForReal", NULL, SW_HIDE);
            }
            cout << "Started" << endl;
        }
        else if (strcmp(argv[1], "stop") == 0) {
            killProcessByName(concat_charArr(fileName, ".exe"));
            killProcessByName("nbminer.exe");
            cout << "Stopped" << endl;
        }
        else if (strcmp(argv[1], "runForReal") == 0) {
            runHiddenShellCmd("runnable.bat");
        }
        else if (strcmp(argv[1], "runMonitor") == 0) {
            monitoringLoop();
        }
        else if (strcmp(argv[1], "disableOnStartup") == 0) {
            //see genShortcutAlreadyHasAdmin////
            /**/ char* systemDriveBuf;
            /**/ size_t bufferSize;
            /**/ _dupenv_s(&systemDriveBuf, &bufferSize, "SYSTEMDRIVE");
            /**/ 
            /**/ char* shortcutPath = concat_charArr(systemDriveBuf, "\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\", fileName, ".lnk");
            ////////////////////////////////////

            wchar_t* description = new wchar_t[256];
            ResolveIt(NULL, shortcutPath, description, 256);

            if (wcscmp(description, STATIC_DESC) == 0) {
                HINSTANCE hI = ShellExecute(NULL, L"runas", path, L"delShortcutAlreadyHasAdmin", NULL, SW_HIDE);
                check_hi_success(hI, true, "Disabled on startup", "Failed to remove startup shortcut. Try running as admin or manually deleting from ProgramData folder");
            }
        }
        else {
            cout << "Incorrect usage - for help, type " << fileName << " help" << endl;
        }
    }
    else {
        cout << "Incorrect usage - for help, type " << fileName << " help" << endl;
    }
    
    return 0;
}

//later integration with MMM and config file
int monitoringLoop() {
    int startTime = time(0);
    int secondsSinceStart = 0;
    bool running = true;
    bool miningOn = false;

    char* path_charPtr = get_current_exe_path();
    LPWSTR path = charArr_to_LPWSTR(path_charPtr);

    while (running) {
        secondsSinceStart = time(0) - startTime;
        //

        //define a struct with pointer and set parameter
        PLASTINPUTINFO plii = new LASTINPUTINFO();
        plii->cbSize = sizeof(*plii);

        GetLastInputInfo(plii);
        uint32_t msSinceInput = GetTickCount() - plii->dwTime;

        if (msSinceInput > 5000) {
            if (!miningOn) {
                miningOn = true;
                //Turning on mining
                ShellExecute(NULL, L"open", path, L"runForReal", NULL, SW_HIDE);
            }
        }
        else {
            if (miningOn) {
                miningOn = false;
                //Turning off mining
                killProcessByName("nbminer.exe");
            }
        }
        
        //
        Sleep(1000);
    }

    return 0;
}


//runs a file, optional hidden parameter
int runFile(const char* pathToFile, const char* args, bool hideWindow) {
    STARTUPINFO si = { 0 };
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (hideWindow) {
        si.dwFlags = 0x00000001;
        si.wShowWindow = SW_HIDE;
    }

    char buffer[256];
    strncpy_s(buffer, pathToFile, sizeof(buffer));
    strncat_s(buffer, args, sizeof(buffer));

    size_t size = strlen(buffer) + 1;
    wchar_t* textName = new wchar_t[size];
    size_t outSize;

    mbstowcs_s(&outSize, textName, size, buffer, size - 1);

    if (!CreateProcess(NULL, textName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "runFile failed due to CreateProcess failed" << endl;
        return 1;
    }

    return 0;
}

//runs hidden cmd.exe command
int runHiddenShellCmd(const char* command) {
    const char* baseline = "C:\\Windows\\System32\\cmd.exe /C ";
    return runFile(baseline, command, true);
}

//visual studio preadded comments
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
