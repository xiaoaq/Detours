//////////////////////////////////////////////////////////////////////////////
//
//  Detours Program ([zsub].cpp of [zsub].dll)
//
//  This DLL will detour the ZMVR.dll and CheckToken.dll API so that there function
//  gets called instead.
//

// Do not treat the following warnings as errors
#pragma warning(disable: 4100 4267 4819 4840)

#include <stdio.h>
#include <string>
#include <strsafe.h>
#include <map>
#include <windows.h>
#include "detours.h"

using namespace std;

#define DEBUG_MODE 1

#ifndef DEBUG_MODE
#define DEBUG_MODE 0
#endif

// Ouput log to DebugView.
VOID dprintf(const char* fmt, ...)
{
#ifdef DEBUG_MODE
    const char TAG[8] = "[zsub] ";
    char szBuf[1024 + 8] = { 0 };

    va_list args;
    va_start(args, fmt);
    vsprintf_s(szBuf + 7, sizeof(szBuf) - 7, fmt, args);
    va_end(args);

    sprintf_s(szBuf, sizeof(szBuf), "%s%s", TAG, szBuf + 7);
    OutputDebugStringA(szBuf);
#endif
}

// 递归查找文件,返回目标文件路径
bool find_file_recursive(const string& path, const string& file_name, string& file_path)
{
    WIN32_FIND_DATAA data;
    HANDLE handle = FindFirstFileA((path + "\\*").c_str(), &data);

    if (handle != INVALID_HANDLE_VALUE) {
        do {
            string name(data.cFileName);
            if (name != "." && name != "..") {
                string full_path = path + "\\" + name;
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    if (name == file_name) {
                        file_path = full_path;
                        FindClose(handle);
                        return true;
                    }
                } else {
                    // 递归调用过程中发现了指定文件，则会立刻返回结果并停止搜索操作
                    if (find_file_recursive(full_path, file_name, file_path)) { // 递归调用自身搜索子目录
                        FindClose(handle);
                        return true;
                    }
                }
            }
        } while (FindNextFileA(handle, &data));
        FindClose(handle);
    }
    return false;
}
/////////////// Hook functions //////////////////////

const char* (*pOrigStartBillingThread)(char *, char *, char *);
// Hooked "StartBillingThread" Function
const char* Hsbt(char *developID, char *gameID, char *token)
{
    dprintf("Hsbt...");
    // dprintf("Hsbt Params dID:%s - gID:%s - token:%s", developID, gameID, token);
    // preset white-list games.
    std::map<string, char*> gameMap({
        {"8497ecbd6a3d4e7ca7e1df3a6b9487c7", "ffd83bb69b42477ca36d1a744a40a5c4"}, 
        {"9e0d442ad296424e99001db018dd6c21", "bd8e382d28874261ace9f7c370df93c1"}, 
        {"db7dcaecab2640f489137708826dc5a5", "029195db948d4f34941c47b523d7e8f8"},
        {"da79b7dd3c9046ad97e5f249d853dc52", "48400c890a8f46dba43efeaad542265c"},
        {"c9f94f5a871449449ef077c96cf3bb9e", "1f4c720b0f934b95b48990a2ebf51619"},
        {"b5c28de152ba4949ac58fe691a2339bf", "248a6412b5124b539b5b0d26be29cf1c"},
        {"d280293871bc44fb88308ce323952626", "137fe5c1806f49d994ebe27e7355e88a"},
        {"100fb64b57184a52b80ba04476e6fbd1", "d55f3b65abaa459a82a3a629c4ee4e2c"},
        {"82147f1e01a749a2a153beec85818464", "a6d778a865474cc28b9a9369640064c2"},
        {"ad97f13943d1483998db2190046784ab", "b6ab33788ebc4aa2af5773e2af7bb291"},
        {"44e34294aea645a0ae0cef8a1fbcccb9", "c6c41eabdbae44c88b9cf824b76eeb2f"},
        {"69de76b789164574aaedd2e2684902db", "2abe17bad7c946148130131bd97f991a"},
        {"89a0f27c7ad8460494b65fc4d15dc5c7", "a089862a76834e18b66e61d84abb3dec"},
        {"085b752ab17e4602924339e9b866750c", "bcbed43a7f364105abbeee1b1896c085"},
        {"1c69561a63414787af933f47f63c05d0", "fe308c1f8ccd4b40b303548640cbcb6e"},
        {"ed9f43c19bf94497aca9969c7f65ca38", "e294d682185c48c5aebc8e5f5a171a0d"},
        {"78861ad0a1c6489c9723766d294468b1", "a9ac5c78788e406593f53815595e5680"},
        {"e262e0a7921045c584fc9cec15be8ee7", "609508ef31184e06aca7f6345bdcf3e0"},
        {"f0303024a8214a4e9496a7be800084ab", "77ff2ade6b134ceda13cd752dd8a20c5"},
        {"e5733f4d5f0849799c6475876611c71e", "badfacc4cee24d3181c46fff80252ab9"},
        {"872db460bd76444fb8a389dc5b0cfe38", "9d570fa2f8bf4f66ab77339ae0367121"},
        {"14c715159eb846548c0582ad7ccb537f", "e51d9c9d441241cdb3f7cf3c3cf6b692"},
        {"5d5c75cf151c4706bff2911810039182", "2fda6e9e6ad241548d87dd10eaf8899b"}, // del, for test
        });

    auto it = gameMap.find(token);
    if (it != gameMap.end()) {
        dprintf("Find the key in the gameMap\n");
        return it->second;
    }
    dprintf("No such key in the gameMap\n");
    
    // call original function
    // return pOrigStartBillingThread(developID, gameID, token);

    return "";
}

/////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    HMODULE hTargetDll = NULL;
    LONG error;
    (void)hinst;
    (void)reserved;
    if (DetourIsHelperProcess()) {
        dprintf("DllMain DetourIsHelperProcess return.");
        return TRUE;
    }
    if (dwReason == DLL_PROCESS_ATTACH) {
        DetourRestoreAfterWith();

        dprintf("zsub" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
               " Starting.\n");

        // 文件查找测试
        string start_path = ".";// 当前目录
        string targetFilePath;
        string targetFuncName; // NOTE XIAO, ZMVR.dll 对应 "StartBilligThread", CheckToken.dll对应"CheckToken".
        if(find_file_recursive(start_path, "ZMVR.dll", targetFilePath)){
            targetFuncName =  "StartBillingThread";
        }
        else if(find_file_recursive(start_path, "CheckToken.dll", targetFilePath)){
            targetFuncName =  "CheckToken";
        }
        else{
            // 没有找到目标文件,抛出异常
            dprintf("Error: Not found target file.");
            return FALSE;
        }

        // dprintf("Found File: %s", targetFilePath.c_str());

        // GetModuleHandleW can be used in loaded dll
        hTargetDll = LoadLibrary(targetFilePath.c_str()); // "ZMVR.dll" or "CheckToken.dll"
        if(hTargetDll == NULL){
            
            dprintf("Error: hTargetDll load failed, return.");
            // throw an error
            return FALSE;
        }

        pOrigStartBillingThread = reinterpret_cast<const char* (*)(char*, char*, char*)>(GetProcAddress(hTargetDll, targetFuncName.c_str()));
        if (pOrigStartBillingThread == nullptr){
            dprintf("Error targetF is nullptr, return.");
            return TRUE;
        }
        // dprintf("pOrigStartBillingThread %p", pOrigStartBillingThread);
        
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)pOrigStartBillingThread, Hsbt);
        error = DetourTransactionCommit();

        if (error == NO_ERROR) {
            dprintf("zsub" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
                   " Start D...\n");
        }
        else {
            dprintf("zsub" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
            " Error occur: %ld\n", error);
        }

        // NOTE XIAO, 此处不做释放，可能导致函数注入成功实际拦截失败。
        // FreeLibrary(hTargetDll);

    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)pOrigStartBillingThread, Hsbt);
        error = DetourTransactionCommit();

        if(hTargetDll != NULL){
            FreeLibrary(hTargetDll);
            dprintf("hTargetDll free.");
        }

        dprintf("zsub" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
        " quit. (result=%ld).\n", error);
    }
    return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
