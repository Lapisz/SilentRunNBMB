//QMLaunch by Lapisz
//imports and definitions
#include <iostream>
#include <process.h>
#include <windows.h>
#include "LinkMaker.h"

using namespace std;

//==========functions==========
int monitoringLoop();

char* concat_charArr(const char* first, const char* second, const char* third = NULL, const char* fourth = NULL, const char* fifth = NULL);
wstring concat_LPCWSTR(LPCWSTR first, LPCWSTR second, LPCWSTR third = NULL, LPCWSTR fourth = NULL, LPCWSTR fifth = NULL);
LPWSTR charArr_to_LPWSTR(const char* input);

char* getCurrentExePath();
char* reverse_splicer(char* input, const char startScanningChar, const char stopScanningChar);

int runFile(const char* pathToFile, const char* args, bool hideWindow);
int runHiddenShellCmd(const char* command);
//=============================

//global vars
const char* VERSION = "1.0";

int main(int argc, char* argv[])
{
    char* path_charPtr = getCurrentExePath();
    LPWSTR path = charArr_to_LPWSTR(path_charPtr);
    char* fileName = reverse_splicer(path_charPtr, '.', '\\');

    //commandline handler
    if (argc == 2) {
        if (strcmp(argv[1], "enableOnStartup") == 0) {
            HINSTANCE hI = ShellExecute(NULL, L"runas", path, L"genShortcutAlreadyHasAdmin", NULL, SW_HIDE);
            int hI_int = static_cast<int>(reinterpret_cast<uintptr_t>(hI));
            if (hI_int < 32) {
                cout << "Failed to enable on startup. Maybe run as admin?" << endl;
            }
            else {
                cout << "Enabled on startup" << endl;
            }
        }
        else if (strcmp(argv[1], "genShortcutAlreadyHasAdmin") == 0) {
            //puts system drive letter into a variable
            char* systemDriveBuf;
            size_t bufferSize;
            _dupenv_s(&systemDriveBuf, &bufferSize, "SYSTEMDRIVE");

            //make string for the location of the shortcut
            char* shortcutPath = concat_charArr(systemDriveBuf, "\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\", fileName, ".lnk");
            LPWSTR args = charArr_to_LPWSTR("run");

            CreateLink(path, args, shortcutPath, L"dont touch!!! for qml app use only");
        }
        else if (strcmp(argv[1], "help") == 0) {
            cout << "QMLaunch v" << VERSION << " by Lapisz - https://lapisz.net" << endl;
            cout << endl;
            cout << fileName << " [args]" << endl;
            cout << "args:" << endl;
            cout << "...   enableOnStartup - enables run on startup (will request admin access)" << endl;
            cout << "...   help            - displays this screen" << endl;
            cout << "...   run             - quietly runs runnable.bat" << endl;
        }
        else if (strcmp(argv[1], "run") == 0) {
            runHiddenShellCmd("runnable.bat");
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

//later integration with MMM
//unimplemented and unused
int monitoringLoop() {
    
}

//Adds char strings together, up to five of them
char* concat_charArr(const char* first, const char* second, const char* third, const char* fourth, const char* fifth) {
    size_t totalLength = strlen(first) + strlen(second);
    if(third != NULL) totalLength += strlen(third);
    if(fourth != NULL) totalLength += strlen(fourth);
    if(fifth != NULL) totalLength += strlen(fifth);

    if (totalLength > ((size_t) 2047)) {
        cout << "Severe: Combined length of all char in concat_charArr cannot be more than 2044" << endl;
        char failed[7] = "failed";
        return failed;
    }

    char buffer[2048];
    strcpy_s(buffer, first);
    strcat_s(buffer, second);
    if (third != NULL) strcat_s(buffer, third);
    if (fourth != NULL) strcat_s(buffer, fourth);
    if (fifth != NULL) strcat_s(buffer, fifth);
    
    return buffer;
}

//Adds LPCWSTR together, up to five of them
//note to self: if output is not used correctly, may end up without the data
wstring concat_LPCWSTR(LPCWSTR first, LPCWSTR second, LPCWSTR third, LPCWSTR fourth, LPCWSTR fifth) {
    wstring wstrone(first);
    wstring wstrtwo(second);
    wstring combined = wstrone + wstrtwo;

    if (third != NULL) {
        wstring wstrthree(third);
        combined += wstrthree;
    }

    if (fourth != NULL) {
        wstring wstrfour(fourth);
        combined += wstrfour;
    }

    if (fifth != NULL) {
        wstring wstrfive(fifth);
        combined += wstrfive;
    }
    
    //returns wstring because there is no extra buffer and windows deletes the stuff that a LPCWSTR would point to after the function ends
    return combined;
}

//converts char string to LPWSTR
LPWSTR charArr_to_LPWSTR(const char* input) {
    size_t size = strlen(input) + 1;
    size_t outSize;
    wchar_t* buffer = new wchar_t[size];
    mbstowcs_s(&outSize, buffer, size, input, size - 1);

    LPWSTR output = buffer;

    return output;
}

//gets path of this program
char* getCurrentExePath() {
    char buffer[257];
    GetModuleFileNameA(GetModuleHandle(0), buffer, sizeof(buffer));

    return buffer;
}

//reverse scan string-end splicer
//takes end of the string with end as startScanningChar and beginning as stopScanningChar, exclusive of the endpoints (those two vars)
//eg C:/example/test.exe - startScanningChar = '.', stopScanningChar = '/', output is "test"
char* reverse_splicer(char* input, const char startScanningChar, const char stopScanningChar) {
    //reverse the input
    char* reversed = new char[257];
    int count = 0;
    for (int i = strlen(input) - 1; i >= 0; i--) {
        reversed[count] = input[i];
        count++;
    }
    reversed[count] = '\0';

    //filter out a segment, with beginning and end points that have been passed in to the function
    int count2 = 0;
    char* reversedSpliced = new char[257];
    bool countingLetters = false;
    for (int j = 0; j < strlen(reversed); j++) {
        
        if ((reversed[j] == startScanningChar) && !countingLetters) {
            countingLetters = true;
        }
        else if (reversed[j] == stopScanningChar) {
            countingLetters = false;
            break;
        }
        else if (countingLetters) {
            reversedSpliced[count2] = reversed[j];
            count2++;
        }
    }
    reversedSpliced[count2] = '\0';

    //reverse the segment so it is back to normal order
    char* processed = new char[257];
    int count3 = 0;
    for (int k = strlen(reversedSpliced) - 1; k >= 0; k--) {
        processed[count3] = reversedSpliced[k];
        count3++;
    }
    processed[count3] = '\0';

    //free up memory
    delete[] reversed;
    delete[] reversedSpliced;

    return processed;
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
