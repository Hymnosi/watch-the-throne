#define ZPostChannelJoinAddr 0x004C5330
#define ZCharacter__OnDamagedAddr 0x0047AE10

struct MUID{
	unsigned long HighID;
	unsigned long LowID;
};
typedef DWORD (__cdecl* ZGetGameClientF)(void);
ZGetGameClientF ZGetGameClient = (ZGetGameClientF)0x004AD770;//0x004ABCC0;


//void (__cdecl* ChangeTextureFormat)(int iRes);
CDetour SetMapTextureLevelDetour;
void __cdecl SetMapTextureLevelHook(int iRes);
//void(__cdecl* ChangeBaseTextureLevel)(unsigned long iRes);

MUID* myUID = new MUID();


const char* test = "test";
//void(__stdcall* ZClanListBox__SetInfo)(void* pThis, int, int, const char*, int);
CDetour ZClanListBox__SetInfoDet;
//CDetour ZObject__OnDamagedDet;
void __stdcall ZClanListBox__SetInfoHook(int,int,const char*, int);
bool (__stdcall* ZChatInput)(char*);
void (__cdecl* ZChatOutput)(const char* szMsg, int iType, int iLoc, DWORD dwColor);
HMODULE (__stdcall* LoadLibraryAFunc)(LPCSTR lpFileName);
vector<char*> IgnoreList;
typedef void (__cdecl* ZPostAdminBanF)(MUID*, char*);
ZPostAdminBanF ZPostAdminBan = (ZPostAdminBanF)0x0042D700;//0x0042C020;
void (__cdecl* ZPostChannelJoin)(MUID*, MUID*);
void (__stdcall* OnAdminAnnounce)(const char*, int);
void(__stdcall* ZObjectOnDamaged)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
void (__cdecl* RenderFrame)();
typedef void (__cdecl* ZPostWhisperF)(char*, char*, char*);
ZPostWhisperF ZPostWhisper = (ZPostWhisperF)0x0042C100;
void (__stdcall* OnUserWhisperFunc)(const char*,const char*,const char*);
HMODULE __stdcall LoadLibraryAHook(LPCSTR lpFileName)
{
	return LoadLibraryAFunc(lpFileName);
	//return LoadLibraryADet.Org(lpFileName);
}
void __cdecl SetMapTextureLevelHook(int iRes)
{
	if(iRes == 2)
		SetMapTextureLevelDetour.Org(8);
	else
		SetMapTextureLevelDetour.Org(iRes);
}
	
char* GetAccountName()
{
	return (char*)(0x0066D533);//NEEDS TO BE UPDATED
}
char* GetCharacterName()
{
	return (char*)(0x006AA314);//0x0066C73C
}
char* GetCharNameFromClass(DWORD classLoc)
{
	return (char*)(classLoc + 0x460);
}
int packetNum = 0;
bool bGetFPS = false;
int iFPSCount = 0;
int iLastSecondFPS = 0;
void __stdcall ZClanListBox__SetInfoHook(int nIndex, int nEmblemId, const char* szName, int nPlayers)
{

	if(nPlayers == 2)
		ZClanListBox__SetInfoDet.Org(nIndex, nEmblemId, "2 Vs 2", nPlayers);
	else if(nPlayers == 3)
		ZClanListBox__SetInfoDet.Org(nIndex, nEmblemId, "3 Vs 3", nPlayers);
	else if(nPlayers == 4)
		ZClanListBox__SetInfoDet.Org(nIndex, nEmblemId, "4 Vs 4", nPlayers);
}
MUID* MyID = new MUID();
MUID* MyChannelID = new MUID();
DWORD aa, bb,cc,dd,ee,ff,gg,hh,ii;
DWORD ZCharHit = 0x0;
namespace Hooked
{
	void __cdecl RenderFrameHook()
	{
		__asm pushad
		if(bGetFPS)
			iFPSCount++;
		__asm popad
		if(iLastSecondFPS > 800)
			Sleep(1);
		RenderFrame();
	}
	char szUser[16];
	bool bPasswordRequested = false;
	void Echo(const char *szMsg, ...)
    {
      char szBuf[0x4000];
      va_list vaArgs;
      va_start(vaArgs, szMsg);
      _vsnprintf(szBuf, sizeof(szBuf), szMsg, vaArgs);
      va_end(vaArgs);
      ZChatOutput(szBuf, 2, 0, 0xFFFFFFFF);
    }
	void __cdecl ChannelJoin(MUID* userID, MUID* channelID)
	{
		//Keep track of MUIDs as often as we can
		MyID->LowID = userID->LowID;
		MyChannelID->LowID = channelID->LowID;
		ZPostChannelJoin(userID, channelID);
	}
	void ChatOutput(const char* szMsg, int iType, int iLoc, DWORD dwColor)
	{
		char IgnoreString1[256];
		char IgnoreString2[256];
		sscanf_s(szMsg, "^4%s^9", IgnoreString1);
		for(size_t i = 0; i < IgnoreList.size(); i++)
			if((string(szMsg).find(IgnoreString1) != string::npos))
				return;
		ZChatOutput(szMsg, iType, iLoc, dwColor);
	}
	unsigned long GetMyID()
	{
		unsigned long uidPlayer = 0x0;
		__asm{
			call ZGetGameClient
			mov eax, dword ptr ds:[eax+0x1A4]
			mov uidPlayer, eax
		}
		return uidPlayer;
	}
	unsigned long GetStageID()
	{
		unsigned long StageID = 0x0;
		__asm{
			call ZGetGameClient
			mov eax, dword ptr ds:[eax+0x1B4]
			mov StageID, eax
		}
		return StageID;
	}
	DWORD ChatECX = 0x0;
	bool __stdcall ChatInput(char* szInput)
	{
		
		__asm mov ChatECX, ecx;
		if(!bPasswordRequested)
		{
			if(!memcmp(szInput, "/ban", 4))
			{
				sscanf_s(szInput, "/ban %s", szUser);
				//Echo("Enter password for admin command.");
				sprintf_s(Global_Request, "db ban %s %s", szUser, GetCharacterName());
				SendRequestToConsole(Global_Request);
				ZPostAdminBan(MyID, szUser);
				//bPasswordRequested = true;
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!memcmp(szInput, "/ip ban", 7))
			{
				sscanf_s(szInput, "/ip ban %s %s", szUser);
				//Echo("Enter password for admin command.");
				sprintf_s(Global_Request, "ip ban %s %s", szUser, GetCharacterName());
				SendRequestToConsole(Global_Request);
				ZPostAdminBan(MyID, szUser);
				//bPasswordRequested = true;
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!memcmp(szInput, "/ignore", 7))
			{
				char NewName[32];
				sscanf_s(szInput, "/ignore %32s", NewName);
				IgnoreList.push_back(NewName);
				ofstream file("ignore.txt");
				for(size_t i = 0; i < IgnoreList.size(); i++)
					file << IgnoreList.at(i) << endl;
				file.close();
				Echo("%s added to ignore list.", NewName);
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!memcmp(szInput, "/create account", 15))
			{
				char CreationString[128];
				sscanf_s(szInput, "/create account %[^\n]", CreationString);
				//Echo("Enter password for admin command.");
				sprintf_s(Global_Request, "create account %s", CreationString);
				SendRequestToConsole(Global_Request, szInput, false);
				//bPasswordRequested = true;
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!stricmp(szInput, "/motd"))
			{
				sprintf_s(Global_Request, "get motd %d", GetMyID());

				SendRequestToConsole(Global_Request);
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!memcmp(szInput, "/announce", 9))
			{
				char szAnnounce[128];
				sscanf_s(szInput, "/announce %[^\n]", &szAnnounce);
				//Echo("Enter password for admin command.");
				sprintf_s(Global_Request, "admin announce %s\n%s", szAnnounce, GetCharacterName());
				SendRequestToConsole(Global_Request);
				//bPasswordRequested = true;
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!memcmp(szInput, "/admin finish stage", 19))
			{
				//Echo("Enter password for admin command.");
				sprintf_s(Global_Request, "admin finish stage %d %s", GetStageID(), GetCharacterName());
				//bPasswordRequested = true;
				__asm mov ecx, ChatECX;
				return true;
			}
			else if(!memcmp(szInput, "/admin give bounty", 18))
			{
				Echo("Enter password for admin command.");
				sscanf_s(szInput, "/admin %[^\n]", Global_Request);
				bPasswordRequested = true;
				__asm mov ecx, ChatECX;
				return true;
			}
			else
			{
				__asm mov ecx, ChatECX;
				return ZChatInput(szInput);
			}
		}
		else
		{
			__asm mov ecx, ChatECX;
			bPasswordRequested = false;
			SendRequestToConsole(Global_Request, szInput, false);
			//if(Global_Result.compare("OK") == 0)
			//{
//				if(string(Global_Request).find("ban") != string::npos)
					ZPostAdminBan(MyID, szUser);
				//Echo("Command sent successfully");
			//}
			//else if(Global_Result.compare("INVALID PASSWORD") == 0)
//				Echo("Error: Invalid Password");
			//else
//				Echo("Error: please check command syntax");
			return true;
		}
		
	}
	DWORD GameClientECX = 0x0;
	void __stdcall AdminAnnounceHook(const char* szMsg, int iType)
	{
		__asm mov GameClientECX, ecx
		if(strstr(szMsg, "is dominating"))
		{
			Echo(szMsg);
			const char* soundFX = "dominating";
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
			return;
		}
		else if(strstr(szMsg, "on a rampage"))
		{
			Echo(szMsg);
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
			return;
		}
		else if(strstr(szMsg, "on a killing spree"))
		{
			Echo(szMsg);
			const char* soundFX = "killingspree";
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
			return;
		}
		else if(strstr(szMsg, "had a monsterkill"))
		{
			Echo(szMsg);
			const char* soundFX = "monsterkill";
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
			return;
		}
		else if(strstr(szMsg, "is unstoppable"))
		{
			Echo(szMsg);
			const char* soundFX = "unstoppable";
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
			return;
		}
		else if(strstr(szMsg, "had an ultrakill"))
		{
			Echo(szMsg);
			const char* soundFX = "ultrakill";
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
			return;
		}
		else if(strstr(szMsg, "is godlike!"))
		{
			Echo(szMsg);
			const char* soundFX = "godlike";
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
			return;
		}
		else if(strstr(szMsg, "is wicked sick"))
		{
			Echo(szMsg);
			const char* soundFX = "wickedsick";
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
			return;
		}
		else if(strstr(szMsg, "had a ludicrous kill"))
		{
			Echo(szMsg);
			const char* soundFX = "ludicrouskill";
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
			return;
		}
		else if(strstr(szMsg, "Holy shit,"))
		{
			Echo(szMsg);
			const char* soundFX = "holyshit";
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
			return;
		}
		
		__asm mov ecx, GameClientECX;
		OnAdminAnnounce(szMsg, iType);
	}
	
	char DamageBuffer[128];
	bool CanDamage = false;

	
	void __stdcall OnDamaged(DWORD Attacker, DWORD b, DWORD c, DWORD d, DWORD e, DWORD f, DWORD g, DWORD h, DWORD i)
	{
		if(!CanDamage)
		{
			__asm mov ZCharHit, ecx
			__asm pushad
			//Echo("%8X,%8X,%8X,%8X,%8X,%8X,%8X,%8X,%8X,", Attacker,b,c,d,e,f,g,h,i);
			aa=Attacker;
			bb=b;
			cc=c;
			dd=d;
			ee=e;
			ff=f;
			gg=g;
			hh=h;
			ii=i;
			if(!stricmp(GetCharacterName(), GetCharNameFromClass(Attacker)))
			{
				sprintf_s(DamageBuffer, "DAMAGE %d,%8X,%8X,%8X,%8X,%8X,%8X,%8X,%8X", GetMyID(),b,c,d,e,f,g,h,i);
				ZPostWhisper("Me", GetCharNameFromClass(ZCharHit), DamageBuffer);
			}
			__asm popad
			return;
		}
		ZObjectOnDamaged(Attacker,b,c,d,e,f,g,h,i);		
	}
	void  __stdcall OnUserWhisper(const char* szFrom,const char* szTo,const char* szMessage)
	{
		__asm pushad
		if(!memcmp(szMessage, "DAMAGE", 6))
		{
			CanDamage = true;
			DWORD CAttacker;
			long CharID = 0;
			DWORD par1, par2, par3, par4, par5, par6, par7, par8;
			sscanf_s(szMessage, "DAMAGE %d,%8X,%8X,%8X,%8X,%8X,%8X,%8X,%8X", &CharID, &par1,&par2,&par3,&par4,&par5,&par6,&par7,&par8);
			//Echo("DAMAGE %d,%8X,%8X,%8X,%8X,%8X,%8X,%8X,%8X", CharID, par1,par2,par3,par4,par5,par6,par7,par8);

			//Sleep(5000);
			MUID* attackerUID = new MUID();
			attackerUID->LowID = CharID;
			BYTE* functionAddress = (BYTE*)Hooked::OnDamaged;
			__asm{
				mov eax, 0x004A0E20
				call eax
				mov ecx, eax
				mov eax, 0x004873E0
				push attackerUID				
				call eax
				mov CAttacker, eax
			}
			attackerUID->LowID = GetMyID();
			__asm{
				mov eax, 0x004A0E20
				call eax
				mov ecx, eax
				mov eax, 0x004873E0
				push attackerUID				
				call eax
				mov ZCharHit, eax
			}
			__asm{
				mov ecx, ZCharHit
				push par8
				push par7
				push par6
				push par5
				push par4
				push par3
				push par2
				push par1
				push CAttacker
				call functionAddress
			}
			CanDamage = false;
			__asm popad
			return;
		}
		__asm popad
		OnUserWhisperFunc(szFrom, szTo, szMessage);

	}
	
}