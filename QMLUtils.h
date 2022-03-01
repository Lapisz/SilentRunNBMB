#pragma once
#include <iostream>
#include <windows.h>

using namespace std;

char* concat_charArr(const char* first, const char* second, const char* third = NULL, const char* fourth = NULL, const char* fifth = NULL);

char* bool_to_charArr(bool input) {
    char returnValue[6];
    if (input) {
        strncpy_s(returnValue, "true" , sizeof(returnValue));
    }
    else {
        strncpy_s(returnValue, "false", sizeof(returnValue));
    }

    return returnValue;
}

//Adds char strings together, up to five of them
char* concat_charArr(const char* first, const char* second, const char* third, const char* fourth, const char* fifth) {
    size_t totalLength = strlen(first) + strlen(second);
    if (third != NULL) totalLength += strlen(third);
    if (fourth != NULL) totalLength += strlen(fourth);
    if (fifth != NULL) totalLength += strlen(fifth);

    if (totalLength > ((size_t)2047)) {
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
char* get_current_exe_path() {
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

int check_hi_success(HINSTANCE hi, bool printMsg = false, const char* successMsg = NULL, const char* failedMsg = NULL) {
    int hi_int = static_cast<int>(reinterpret_cast<uintptr_t>(hi));
    if (hi_int < 32) {
        //Failed
        if ((printMsg == true) && (*successMsg != NULL) && (*failedMsg != NULL)) {
            cout << failedMsg << endl;
        }
        
        return 1;
    }
    else {
        //Success
        if ((printMsg == true) && (*successMsg != NULL) && (*failedMsg != NULL)) {
            cout << successMsg << endl;
        }
        
        return 0;
    }
}