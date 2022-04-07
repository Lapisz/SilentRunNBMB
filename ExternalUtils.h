#pragma once
//this file is from Microsoft - https://docs.microsoft.com/en-us/windows/win32/shell/links#creating-a-shortcut-and-a-folder-shortcut-to-a-file

// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces 
//              to create and store a shortcut to the specified object. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszPathLink - Address of a buffer that contains the path where the 
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the 
//                Shell link, stored in the Comment field of the link
//                properties.

#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

HRESULT CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszStartInPath, LPCWSTR lpszArgs, LPCSTR lpszPathLink, LPCWSTR lpszDesc)
{
    HRESULT hres = CoInitialize(NULL);
    IShellLink* psl = NULL;

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description. 
        psl->SetPath(lpszPathObj);
        psl->SetWorkingDirectory(lpszStartInPath);
        psl->SetArguments(lpszArgs);
        psl->SetDescription(lpszDesc);

        // Query IShellLink for the IPersistFile interface, used for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH];

            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

            // Add code here to check return value from MultiByteWideChar 
            // for success.

            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}

// ResolveIt - Uses the Shell's IShellLink and IPersistFile interfaces 
//             to retrieve the path and description from an existing shortcut. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// hwnd         - A handle to the parent window. The Shell uses this window to 
//                display a dialog box if it needs to prompt the user for more 
//                information while resolving the link.
// lpszLinkFile - Address of a buffer that contains the path of the link,
//                including the file name.
// lpszPath     - Address of a buffer that receives the path of the link
// target, including the file name.
// lpszDesc     - Address of a buffer that receives the description of the 
//                Shell link, stored in the Comment field of the link
//                properties.


#include <strsafe.h>

HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, /*LPWSTR lpszPath, int iPathBufferSize, */LPWSTR lpszDesc, int descBufSize)
{
    HRESULT hres = CoInitialize(NULL);
    IShellLink* psl;
    WCHAR szGotPath[MAX_PATH];
    WCHAR szDescription[MAX_PATH];
    WIN32_FIND_DATA wfd;

    //*lpszPath = 0; // Assume failure 
    *lpszDesc = 0;

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Get a pointer to the IPersistFile interface. 
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH];

            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH);

            // Add code here to check return value from MultiByteWideChar 
            // for success.

            // Load the shortcut. 
            hres = ppf->Load(wsz, STGM_READ);

            if (SUCCEEDED(hres))
            {
                // Resolve the link. 
                hres = psl->Resolve(hwnd, 0);

                if (SUCCEEDED(hres))
                {
                    // Get the path to the link target. 
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);

                    if (SUCCEEDED(hres))
                    {
                        // Get the description of the target. 
                        hres = psl->GetDescription(szDescription, MAX_PATH);

                        if (SUCCEEDED(hres))
                        {
                            //hres = StringCbCopy(lpszPath, iPathBufferSize, szGotPath);
                            hres = StringCbCopy(lpszDesc, descBufSize, szDescription);
                            if (SUCCEEDED(hres))
                            {
                                // Handle success
                            }
                            else
                            {
                                // Handle the error
                                std::cout << "Severe: Failed to access description" << std::endl;
                            }
                        }
                    }
                }
            }
            else {
                std::cout << "Severe: Failed to access file" << std::endl;
            }

            // Release the pointer to the IPersistFile interface. 
            ppf->Release();
        }

        // Release the pointer to the IShellLink interface. 
        psl->Release();
    }
    
    return hres;
}

//https://stackoverflow.com/questions/7956519/how-to-kill-processes-by-name-win32-api
#include <Tlhelp32.h>
#include <process.h>
#include "QMLUtils.h"
void killProcessByName(const char* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (wcscmp(pEntry.szExeFile, charArr_to_LPWSTR(filename)) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

//Some help from https://stackoverflow.com/questions/865152/how-can-i-get-a-process-handle-by-its-name-in-c
bool doesProcessExist(const char* filename) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    LPWSTR Lfilename = charArr_to_LPWSTR(filename);
    bool returnValue = false;

    //loads first process in list of all processes (this is likely system)
    if (Process32First(snapshot, &entry) == TRUE) {
        //goes through all the processes and checks if any match the requested process
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (wcscmp(entry.szExeFile, Lfilename) == 0) {
                //yes the requested process exists
                returnValue = true;
            }
        }
    }

    CloseHandle(snapshot);
    delete[] Lfilename; //free memory used by charArr_to_LPWSTR() to prevent memory leak in case this function (doesProcessExist()) is put in a loop
    return returnValue;
}
