#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <fstream>
using namespace std;
#include <WS2tcpip.h>
#include <Winsock2.h>
#include "WSock.h"



#pragma comment(lib, "CDetour.lib")
#pragma comment(lib, "detours.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#include "CByteArray.h"


#include "detours.h"
#include "CDetour.h"
#include "CDetourDis.h"
#include "HookedFunctions.h"

void CreateConsoleLog(LPCWSTR winTitle)
{
	AllocConsole();
	SetConsoleTitle((LPCTSTR)winTitle);

	int hConHandle;
	long lStdHandle;
	FILE *fp;

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
}

int ticCount = 0;

void fpsThread(void)
{
	while(1)
	{
		bGetFPS = true;
		Sleep(1000);
		iLastSecondFPS = iFPSCount;
		bGetFPS = false;
	    iFPSCount = 0;
	}
}
void main(void)
{
	while(1)
	{
		if(GetAsyncKeyState(0x60))
		{
			Hooked::Echo("My FPS is: %d", iLastSecondFPS);
		}
		if(GetAsyncKeyState(0x61))
		{
			const char* soundFX = "rampage";
			__asm{
				mov eax, 0x004AD780 //GetSoundManager
				call eax
				mov ecx, eax
				push 0
				push 0
				push soundFX
				mov eax, 0x00494E80 //PlaySoundA
				call eax
			}
		}
		else if(GetAsyncKeyState(0x62))
		{
			printf("%d\n", Hooked::GetStageID());
		}
		else if(GetAsyncKeyState(0x63))
		{
			/*BYTE* functionAddress = (BYTE*)Hooked::OnDamaged;
			__asm{
				mov ecx, ZCharHit
				push ii
				push hh
				push gg
				push ff
				push ee
				push dd
				push cc
				push bb
				push aa
				call functionAddress
			}*/
			DWORD CharacterDWORD = 0x0;
			
			myUID->LowID = 16;
			__asm{
				mov eax, 0x004A0E20
				call eax
				mov ecx, eax
				mov eax, 0x004873E0
				push myUID				
				call eax
				mov CharacterDWORD, eax
			}
			Hooked::Echo("%8X", CharacterDWORD);
		}
		else if(GetAsyncKeyState(0x64))
		{
			Hooked::CanDamage = !Hooked::CanDamage;
			Hooked::Echo("CanDamage is now set to: %d", Hooked::CanDamage);
			Sleep(1000);
		}
		else if(GetAsyncKeyState(0x65))
		{
			Hooked::Echo("MyID: %d", Hooked::GetMyID());
			Sleep(1000);
		}

		


		Sleep(100);//Do nothing....
	}
}

BOOL APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	if(Reason == DLL_PROCESS_ATTACH)
	{
		CreateConsoleLog(L"Debug window");
		HMODULE g_hModule;
		//On attach set the hooks
		//SetMapTextureLevel = (void (__cdecl*)(int))DetourFunction((PBYTE)0x004E7690, (PBYTE)Hooked::MapTextureLevel);
		//ZClanListBox__SetInfo = (void (__stdcall*)(void* pThis, int, int, const char*, int))DetourFunction((PBYTE)0x00418E40, (PBYTE)Hooked::ClanSetInfo);
		/*ZClanListBox__SetInfoDet.Detour((BYTE*)0x00418E40, (BYTE*)ZClanListBox__SetInfoHook);
		ZClanListBox__SetInfoDet.Apply();
		SetMapTextureLevelDetour.Detour((BYTE*)0x004E7690, (BYTE*)SetMapTextureLevelHook);
		SetMapTextureLevelDetour.Apply();*/
		/*LoadLibraryADet.Detour((BYTE*)0x005E6168, (BYTE*)LoadLibraryAHook);
		LoadLibraryADet.Apply();*/
		//LoadLibraryAFunc = (HMODULE (__stdcall)(LPCSTR))DetourFunction((PBYTE)0x005E6168, (PBYTE)LoadLibraryAHook);
		//LoadLibraryAFunc = (HMODULE (__cdecl*)(LPCSTR))DetourFunction((PBYTE)0x005E6168, (PBYTE)LoadLibraryAHook);
		ZObjectOnDamaged = (void (__stdcall*)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD))DetourFunction((PBYTE)ZCharacter__OnDamagedAddr, (PBYTE)Hooked::OnDamaged);
		//ZObject__OnDamagedDet.Detour((BYTE*)ZCharacter__OnDamagedAddr, (BYTE*)Hooked::OnDamaged);
		//ZObject__OnDamagedDet.Apply();
		LoadLibraryAFunc = (HMODULE (__stdcall*)(LPCSTR))DetourFunction((PBYTE)LoadLibrary, (PBYTE)LoadLibraryAHook);
		ZChatInput = (bool (__stdcall*)(char*))DetourFunction((PBYTE)0x0042B930, (PBYTE)Hooked::ChatInput);//0x0042A300
		ZChatOutput = (void (__cdecl*)(const char* szMsg, int iType, int iLoc, DWORD dwColor))DetourFunction((PBYTE)0x0042B860, (PBYTE)Hooked::ChatOutput);//0x0042A230
		RenderFrame = (void (__cdecl*)())DetourFunction((PBYTE)0x004E67F0, (PBYTE)Hooked::RenderFrameHook);//0x004E5300
		ZPostChannelJoin = (void (__cdecl*)(MUID*, MUID*))DetourFunction((PBYTE)ZPostChannelJoinAddr, (PBYTE)Hooked::ChannelJoin);
		//*(BYTE*)(0x005E6168) = *(BYTE*)LoadLibraryAHook;
		OnAdminAnnounce = (void (__stdcall*)(const char*, int))DetourFunction((PBYTE)0x004C8090, (PBYTE)Hooked::AdminAnnounceHook);
		OnUserWhisperFunc = (void (__stdcall*)(const char*,const char*,const char*))DetourFunction((PBYTE)0x004C82E0, (PBYTE)Hooked::OnUserWhisper);
		DWORD g_threadID;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&main, NULL, NULL, &g_threadID);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&fpsThread, NULL, NULL, &g_threadID);
		g_hModule = hDLL;
		DisableThreadLibraryCalls(hDLL);
	}
	return TRUE;
}