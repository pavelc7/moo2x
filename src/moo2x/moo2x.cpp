#include	"stdafx.h"
#include	"Patcher.h"
 
#pragma pack(push, 1)
struct jmp_far
{
  BYTE instr_push;  //здесь будет код инструкции push
  DWORD arg;         //аргумент push
  BYTE  instr_ret;    //здесь будет код инструкции ret
};
#pragma pack(pop)
struct jmp_inject
{
	BYTE old[6]; //область для хранения 6-ти затираемых байт начала функции
	LPVOID adr; //будущий адрес оригинальной функции
	DWORD written; //вспомогательная переменная
	jmp_far jump; //здесь будет машинный код инструкции перехода
};
#define MAX_INJECTS 3
jmp_inject injects[MAX_INJECTS];
struct a
{
	char* m_name;
	char* m_dll;
	LPVOID m_Handler;
};
int WINAPI Intercept_MessageBoxA(HWND hwnd, char *text, char *hdr, ui32 utype);
void WINAPI Intercept_OutputDebugStringA(LPCTSTR lpOutputString);
DWORD WINAPI Intercept_GetVersion(void);
a funcs[]=
{
	"MessageBoxA","user32.dll",&Intercept_MessageBoxA,
	"GetVersion","kernel32.dll",&Intercept_GetVersion,
	"OutputDebugStringA","kernel32.dll",&Intercept_OutputDebugStringA,
	NULL,NULL,NULL
};
//////////////////////////////////////////////////////////////////////////
struct Function 
{
	DWORD	m_Address;
	LPVOID	m_Handler;
};
extern	i32 MOX2_Set_Mox_Language_(void);

Function	Hooks[] =
{
	// Joes
	0x00527960, &CreateDirectDraw,
	0x00527E60, &FlipSurface,
	0x005277B0, &WinMain,
	0x005281A0, &UnlockAppropriateRenderBuffer,
	0x00528500, &UnlockPrimarySurface,
	0x00527E10, &BlitToBackbuffer,
	0x005272F0, &DrawCursorXY,
	0x00526EF0,	&ForceCursorPos,
	0x005273C0, &CreateCursorSurfaces,
	0x00527090, &DrawAutoCursor,
	0x00528B30, &RestoreSurfaces,
	0x00527D30,	&CleanupSurfaces,

	0x00528470, &LockPrimarySurface,
	0x005282D0, &LockAppropriateRenderBuffer,
	// struct
	0x0052CFB5, &struct_Test_Bit_Field_,
	0x0052CFED, &struct_Set_Bit_Field_,
	0x0052D027, &struct_Clear_Bit_Field_,
	0x0052D0B7,	&struct_Clear_Structure_,
	0x0052D0FA, &struct_Copy_Structure_,
	// HACCESS
	0x0049B270, &HACCESS_Ship_Owner_,
	
	//mine	
	0x00483EF0, &MOX2_Set_Mox_Language_,
	0x00483CE0, &MOX2_Set_Mox_Alt_Path_,
	0x0051D0F0, &set_path_sub_51D0F0,
	0x0052D360, &DIR,
	0x0052D3D0, &get_timestamp_52D3D0,
	0x0049E9B0, &sub_49E9B0,
	//exitting
	0x00527630, &sub_527630,
	0x00527640, &sub_527640,
	0x00521080, &fields_Clear_Fields,
	0x00533630, &sub_533630,	
	0x0052D310, &exit_Exit_With_Message,
	//
	0x00478E30, &DoSomeInit,
	0x004BAA40, &ERIC_Eric_Defaults_, //empty
	0x004C1D50, &nullsub, //empty
	0x004C1D40, &KEN_Ken_Init_,  // empty
	0x004FBEF0, &COLCALC_Rebuild_WFI_List_,// empty
	0x00406780, &BILL_Bill_Init_,
	0x004F0DD0, &HAROLD_Harold_Init_,
	//0x00478820, &INITGAME_Init_New_Game_,
	0x00478B30, &INITGAME_Init_Ships_Array_,
	0x00478D80, &INITGAME_Init_Colonies_Array_,
	0x004726E0, &MSG_Allocate_Msg_Slots_,
	//
	0x004F0E90, &sub_4F0E90,
	0x004F0ED0, &sub_4F0ED0,
	//
	0x0043C0F0, &FILEDEF_Load_Game_,
	0x0043C840, &FILEDEF_Save_Game_,
	0x0043CDB0, &FILEDEF_Save_Game_Settings_,
    0x0043CE10, &FILEDEF_Load_Game_Settings_,
	0x0043D300, &FILEDEF_Update_Game_Settings_Version_Number_,
	0x0043D1C0, &FILEDEF_Read_Moo_Custom_Player_Data_,
	//random
	0x0052D4C0, &random_Set_Random_Seed,
	0x0052D4D0, &random_Get_Random_Seed,
	0x0052D450, &random_set_sub_52D450,
	0x0052D460, &random_get_sub_52D460,
	0x0052D470, &random_Random,
	0x0052D4E0, &random_from_time_sub_52D4E0,
	0x00483CC0, &MOX2_Set_Game_Random_Seed_,

	0x00527EA0, &video_Toggle_Pages,
	0x00483C80, &MOX2__TOGGLE_PAGES_,
	0x00527C60, &sub_527C60,
	0x0052D290, &clear_video_sub_52D290,

	0x00513B90, &MOVEBOX_Moveable_Box_Selected_,
	0x00528680, &video_Set_Page_Off, 	
	0x00527C80, &video_Set_Refresh_Full,
	0x00527CD0, &video_Set_Refresh_Stencil,
	0x00527D00, &video_Set_Refresh_Differential,
	0x005286A0, &video_Set_Page_Back,
	0x00528700, &video_Copy_Back_To_Off,
	0x005286C0, &video_Copy_Off_To_Back,
	0x004BA740, &ERIC_Fast_Fade_In_,
	0x004BA690, &ERIC_Fast_Fade_Out_,

	//
	0x004D7810, &RUSS_Assert_Settings_, //empty
	//ship
	0x00460600, &SHIPSTAK_Ship_Has_Special_,
	//alloc
	0x00500B20, &ALLOC_Allocate_Data_Space_,
	0x0052E8E0, &allocate_Allocate_Dos_Space,
	0x0052E880, &allocate_Allocate_Space,
	0x0052EA60, &allocate_Allocate_Space_No_Header,
	0x0052E9C0, &do_malloc_hooked,
	0x005421A0, &malloc_hooked,
    0x00542270, &free_hooked,
	0x00546D80, &realloc_hooked,
	0x00546F50, &msize_hooked,
	//replaced
	0x004C1B20, &WorkerThread,
	0x00483470,	&MOX2_Main,
	0x005276F0, &ParseCommandLine,
	0x0052CE60, &strings_Swap_Short,
	0x00536F23, &sub_536F23,
	0x00536EE8, &sub_536EE8,
	0x00528CC0, &graphics_fill,
	//font
	0x004F24D0, &HAROLD_Get_Font_File_,
	0x00526D00, &fonts_Set_PSTR_Style,
	0x004F1E30, &get_type_color_sub_4F1E30,
	//0x0049DDB0, &MISC_Font_Colors2_,//usercall
	//0x00525910, &fonts_Print,
	//autofunc
	0x00523E00, &fields_Assign_Update_Function,
	//
	0x00520200, &capture_Release_Version,
    0x00520210, &capture_Check_Release_Version,
	//debug
	0x004D7850, &RUSS_PFMT_,
	//mapgen
	0x004B6C40, &set_map_maxxy_sub_4B6C40,
	0x004F1050, &get_galaxy_size_sub_4F1050,
	0x004F1090, &HAROLD_Map_Scale_To_Zoom_Level_,
	0x004F0880, &zoom_scale_sub_4F0880,
	0x004CD5D0, &zoom_x_sub_4CD5D0,
	0x004CD5B0, &zoom_y_sub_4CD5B0,
	0x004C6930, &sub_4C6930,//draw
	0x00536710, &WndProc,
	0x00528BA0, &graphics_Set_Window,
	0x0040BCC0, &sub_40BCC0,
	0x00535E50, &sub_535E50,
	0x005365B0, &draw_uncompressed_sub_5365B0,

	0x00536280, &sub_536280,
	0x00524830, &sub_524830,
	0x00524C30, &sub_524C30,
	0x005250F0, &sub_5250F0,
	0x00536040, &sub_536040,
	0x00524490, &animate_Draw,
	0x00524930, &animate_Set_Animation_Palette,
	0x00536508, &draw_compressed_sub_536508,
	//map
	0x004C7260, &map_sub_4C7260,
	0x004C49D0, &MAINSCR_Center_Map_,
	0x004F0A50, &HAROLD_Get_Up_Scaled_Value_,
	0x004CBA80, &MAINSCR_Print_Star_Names_,

	0x004A0050, &ESTRINGS_E_Strings_,
	0x004A0060, &ESTRINGS_Load_E_Strings_,
	0x0051C7B0, &farload_Farload_Data_Static,
	0x005094A0, &ship_add_sub_5094A0,
	0x004F1330, &HAROLD_N_Player_Ships_,
	0x004F1DC0, &HAROLD_XY_To_Star_Id_,
	//0x004C6550, &MAINSCR_Draw_Main_Screen_Filled_,
	//net dplay
	0x00538680, &net_check_GUID_sub_538680,
	0x005385C0, &guid_sub_5385C0,
	//dplay
	0x00539CE0, &DP_sub_539CE0,
#ifdef DPLAY_USE_IN_DLL
	0x00534B40, &kill_net_sub_534B40,
	0x00534DB0, &DP_enum_players_sub_534DB0,
	0x00535290, &netcode_Net_Create_Game,
	0x00535400, &net_sub_535400,
	0x00535420, &net_sub_535420,
	0x005386D0, &net_create_sub_5386D0,
	0x00538760, &net_sub_538760,
	0x00538780, &sub_538780,
	0x005387A0, &sub_5387A0,
	0x00538940, &DP_Create_Player_sub_538940,
	0x00539AD0, &sub_539AD0,
	0x005350F0, &netcode_Net_Join_Game,
	0x005350C0, &sub_5350C0,
	0x00534FA0, &DP_enum_sessions_sub_534FA0,
	0x00534E30, &DP_err_sub_534E30,
	0x00539950, &DP_send_sub_539950,
		
	0x00538850, &sub_538850,
	
#endif
	//net
	0x00539B80, &net_decode_sub_539B80,
	0x00539BE0, &net_decode_sub_539BE0,
	0x00539BC0, &net_decode_sub_539BC0,
	0x00534A90, &netcode_Net_Init,
	//
	0x00539C90, &sub_539C90,
	0x00539C00, &sub_539C00,
	0x00539A60, &net_decode_sub_539A60,
	//callback
	0x00534A70, &callback_sub_534A70,
	//sound
	0x0052DDC0, &sub_52DDC0,
	//menu	
	0x004E95B0, &MAINMENU_Draw_Main_Menu_Screen_,
	0x00526060, &sub_526060,

	0x004D7820, &RUSS_Mox_Update_,
	NULL,		NULL
};

//////////////////////////////////////////////////////////////////////////
void	InstallHooks()
{
	Function * func = &Hooks[0];
	while (func->m_Address != NULL)
	{
		nPatcher::CreateJmp((LPVOID)func->m_Address, func->m_Handler);
		++func;
	}
}


Limits	limits1[] =
{
	NULL, NULL,NULL
};
DWORD maxships[]={
	0x00403ED2, 
	0x00404466, 
	0x00404481, 
	0x00405202, 
	0x00405A54,
//not maxship	0x00405DD1
//	0x00405F81
//	0x00406166,
//	0x00406444,
	0x004094C3,
//	0x0041E793,
//	0x0041ED88,
//	0x0041EE29,
//0x004271A4
	//0x00436AAA,add
	//0x00436C08,sub
	//0x00436C1A,sub
	0x00436E6B,
	0x00436E72,
	//0x00437279,sub
	//0x0043E757,
	//0x0043E87D,
	//0x00446DC8,
	//0x00447297,
	//0x0044AD37,
	//0x0044AF07,
	//0x0044B134,
	//0x00460BEA,add
	//0x00460C17,add
	//0x00460C38,add
	//0x00460C59,add
	0x004606AD, //push 1f4h in SHIPSTAK_Find_Ship_Stacks_ for  allocate_Allocate_First_Block
	0x00460E64,
	//0x00461150,sub
	0x00477DDC,
	//0x00483246,mov
	//0x0048325A,mov
	//0x00483282,mov
	//0x004881AA,
	//0x0048903F,
	0x0048D611,
	0x0049A88F,
	0x0049A89D,
	0x0049B227,
	0x0049B291,
	0x0049B367,
	0x004A150D,
	0x004A1524,
	0x004A1532,
	0x004A154A,
	0x004AFD59,
	//0x004B0A0E,
	0x004BAC2E,
	//0x004BD177,sub	
	0x004BE917,//sub_4BE8F0
	0x004C4B78,
	//0x004D0915,
	//0x004D09F1,
	//0x004D0A8F,
	//0x004E3E71,push fonts
	//0x004F05C7,
	//0x004F05D3,sub
	//0x004F05FB,
	//0x004F0609,
	0x004F136D,
//	0x00509355,
	0x00509515,
	0x0050951C,
	NULL
};
DWORD maxships_mult_500[]={
	0x00437084,//in SHIPMOVE_Allocate_New_Ship_Slots_
	0x0043C457,//in FileDef_Load
	0x00480F37,//in sub_480EE0
	0x00486811,//in sub_4867A0
	0x005163E5,//in sub_516220 something about netsend
	0x00516936,//in NETMOX_Decode_Broadcast_Game_Data_Differential_
	NULL
};
void PatchLimits(DWORD*list,int size,DWORD newdata)
{
	assert(size<=4);
	for(;*list != NULL;++list)
	{
		nPatcher::PatchData((LPVOID)*list,(LPVOID)&newdata,size);		
	}
}
void PatchData(struct Limits * patchlimits)
{
	Limits * limits = &patchlimits[0];
	while (limits->m_Address != NULL)
	{
		nPatcher::PatchData((LPVOID)limits->m_Address,(LPVOID)&limits->data,limits->size);
		++limits;
	}
}
///////////////////////////
struct ReplaceInsideArrays
{
	DWORD m_Address;
	DWORD m_bufsize;
	void* m_replace;
};
ReplaceInsideArrays replaceinsidearrays[] = {
	0x0059C030, 0x1F4 /*500*/ , 0, //MOX__ship_stack_start
	0x00597DA0, 0x9C4 /*2500*/, 0, //MOX__ship_node
	0x005A2CE0, 0xD70 /*3440*/, 0, //MOX__ship_icon
	NULL,0,0
}; 
void FreePatchedInsideArrays(ReplaceInsideArrays* find1)
{
	ReplaceInsideArrays * finds = &find1[0];
	while (finds->m_Address != NULL)
	{
		
		free(finds->m_replace);
		++finds;
	}	
}
int PatchInsideArrays(ReplaceInsideArrays* find1,int k)
{
	i32 beg = 0x00401000;
	i32 end = 0x0054EF7D-beg;
	//MOX__ship_node 00597DA0   00597DA2  00597DA4   //118
	//MOX__ship_stack_start  0059C030   //58		
	//ui8 find[4] = {0x00,0x59,0xC0,0x30}; 
	//ui8 find[4] = {0x00,0x59,0x7D,0xA0}; 
	//005A2CE0 MOX__ship_icon //267 //--005A47C0
		
	//ui8 find4[4];
	int count=0;
	ReplaceInsideArrays * finds = &find1[0];
	ui8*arr=(ui8*)beg;
	while (finds->m_Address != NULL)
	{
		assert(finds->m_bufsize<=0xFFFF);
		finds->m_replace = malloc(finds->m_bufsize*k);
		pLog.Printf("0x00%X\t0x00%X\n",finds->m_Address,finds->m_Address+finds->m_bufsize);
		for(int i=0;i<end;i++)
		{
			if(arr[i+2]==LOBYTE(HIWORD(finds->m_Address)) && arr[i+3]==HIBYTE(HIWORD(finds->m_Address)))
			{
				for(int j=LOWORD(finds->m_Address);j<LOWORD(finds->m_Address)+finds->m_bufsize;j++)
				{
					if(arr[i]==LOBYTE(j) && arr[i+1]==HIBYTE(j) )
					{
						DWORD shift = j-LOWORD(finds->m_Address);
						DWORD rep=(DWORD)finds->m_replace+shift;
						nPatcher::PatchData((LPVOID)finds->m_Address,(LPVOID)&rep,4);
						//pLog.Printf("0x00%X\n",i+beg);
						count++;
					}
				}
			}
		}
		pLog.Printf("%d replaced\n",count);
		++finds;
	}		
	return 0;
}
///////////////////////////
// __stdcall
int injected=0;
int WINAPI Intercept_MessageBoxA(HWND hwnd, char *text, char *hdr, ui32 utype)
{
	int k=0;
	//if(!injected){
	//	injected++;
	//	InitializeFunctionDef();
	//	InstallHooks();		
	//	PatchData(limits1);
	//}

  //Сначала восстанавливаем 6 первых байт функции. Это не обязательное 
  // действие, просто мы решили подшутить над пользователем, и все 
  // сообщения функции MessageBoxA переделать на свои, поэтому нам придется
  // вызвать оригинальную функцию, а для этого следует восстановить ее адрес:
	WriteProcessMemory(GetCurrentProcess(), (void*)injects[k].adr, 
                     (void*)&injects[k].old, 6, &injects[k].written);
 
  //Здесь вы можете порезвиться от души и выполнить любые, пришедшие вам 
  // в голову действия. Мы просто заменили сообщение функции на свое:
  //char *str = "Hi From MessageBOX!!!!";
 
  //Вызываем оригинальную функцию через указатель
  int ret = IDYES;
  char*hdrstr="Shrinker demonstration version";
  if(0!=strncmp(hdr,hdrstr,strlen(hdrstr)))
  {
  ret = //IDYES;
	  ((int (__stdcall*)(HWND, char*, char*, ui32))injects[k].adr)(hwnd, 
            text/*str*/, hdr, utype);
  }
  //Снова заменяем  6 байт функции на команду перехода на нашу функцию
  WriteProcessMemory(GetCurrentProcess(), (void*)injects[k].adr, 
                     (void*)&injects[k].jump, 6,&injects[k].written);
  return ret;
}
//данное определение аналогично __srtdcall
DWORD WINAPI Intercept_GetVersion(void)
{
  int k=1;
  //if(!injected){
	//	injected++;
	//	InitializeFunctionDef();
	//	InstallHooks();		
	//	PatchData(limits1);
  //}
  //Сначала восстанавливаем 6 первых байт функции. Это не обязательное 
  // действие, просто мы решили подшутить над пользователем, и все 
  // сообщения функции MessageBoxA переделать на свои, поэтому нам придется
  // вызвать оригинальную функцию, а для этого следует восстановить ее адрес:
	WriteProcessMemory(GetCurrentProcess(), (void*)injects[k].adr, 
                     (void*)&injects[k].old, 6, &injects[k].written);
 
  //Здесь вы можете порезвиться от души и выполнить любые, пришедшие вам 
  // в голову действия. Мы просто заменили сообщение функции на свое:
  //char *str = "Hi From MessageBOX!!!!";
 
  //Вызываем оригинальную функцию через указатель
 // int ret = ((BOOL (__stdcall*)(HWND, char*, char*, ui32))injects[k].adr)(hwnd, 
 //            /*text*/str, hdr, utype);
 DWORD ret = ((DWORD (__stdcall*)(void))injects[k].adr)();
 
 
  //Снова заменяем  6 байт функции на команду перехода на нашу функцию
  WriteProcessMemory(GetCurrentProcess(), (void*)injects[k].adr, 
                     (void*)&injects[k].jump, 6,&injects[k].written);
  return ret;
}
 
void WINAPI Intercept_OutputDebugStringA(LPCTSTR lpOutputString)
{
	int k=2;
//Сначала восстанавливаем 6 первых байт функции. Это не обязательное 
// действие, просто мы решили подшутить над пользователем, и все 
// сообщения функции MessageBoxA переделать на свои, поэтому нам придется
// вызвать оригинальную функцию, а для этого следует восстановить ее адрес:
WriteProcessMemory(GetCurrentProcess(), (void*)injects[k].adr, 
	(void*)&injects[k].old, 6, &injects[k].written);

//Здесь вы можете порезвиться от души и выполнить любые, пришедшие вам 
// в голову действия. Мы просто заменили сообщение функции на свое:
//char *str = "Hi From MessageBOX!!!!";

//Вызываем оригинальную функцию через указатель
// int ret = ((BOOL (__stdcall*)(HWND, char*, char*, ui32))injects[k].adr)(hwnd, 
//            /*text*/str, hdr, utype);
((void (__stdcall*)(LPCTSTR))injects[k].adr)(lpOutputString);
pLog.Printf("[OutputDebugString] %s\n",lpOutputString);

//Снова заменяем  6 байт функции на команду перехода на нашу функцию
WriteProcessMemory(GetCurrentProcess(), (void*)injects[k].adr, 
	(void*)&injects[k].jump, 6,&injects[k].written);
return ;
}
void InterceptFunctions(void)
{
  //DWORD op;
  for(int i=0;i<MAX_INJECTS;i++)
  {
  //сначала получим абсолютный адрес функции для перехвата
  injects[i].adr = (LPVOID)GetProcAddress(GetModuleHandle(funcs[i].m_dll),
                    funcs[i].m_name);
  if(injects[i].adr == 0)
  {
    MessageBox(NULL, "Can`t get adr_", "Error!", 0);
    return;
  }
  /*
 // Объявление функции
typedef int (__stdcall *DLL_FUNC_BUTTONNAME)(unsigned char *, int, char *, int *);
DLL_FUNC_BUTTONNAME DLL_ButtonName;
 HMODULE hDll = LoadLibrary("user32.dll");
// Получения адреса функции
DLL_ButtonName = (DLL_FUNC_BUTTONNAME)GetProcAddress (hDLL, "ButtonName");
 
// Вызов функции 
DLL_ButtonName(BuffIn, 10, BuffOut, &BuffOutSize);
*/
  // Зададим машинный код инструкции перехода, который затем впишем 
  // в начало полученного адреса:
  injects[i].jump.instr_push = 0x68;
  injects[i].jump.arg = (DWORD)funcs[i].m_Handler;
  injects[i].jump.instr_ret = 0xC3;
 
  //Прочитаем и сохраним первые оригинальные 6 байт стандартной API функции
  ReadProcessMemory(GetCurrentProcess(),(void*) injects[i].adr, 
                    (void*)&injects[i].old, 6, &injects[i].written);
 
//Запишем команду перехода на нашу функцию поверх этих 6-ти байт
WriteProcessMemory(GetCurrentProcess(), (void*)injects[i].adr, 
     (void*)&injects[i].jump, sizeof(jmp_far), &injects[i].written);

}

}
int tests();
int WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
{	
	if (dwReason == DLL_PROCESS_ATTACH /*|| dwReason == DLL_THREAD_ATTACH*/)
	{	
		
		// Intialize logs
		pLog.SetEnable(TRUE);		

		nConfig::Initialize();
		
		InitializeFunctionDef();
		InstallHooks();		

		pLog.Write("Initialized.\n");

		
		if (nConfig::Debug_Breakpoint)
		{
			MessageBox(0, "I am MessageBox from MOO2. You can now switch to debugger.", "MOO2", MB_OK);
		}
		InterceptFunctions();
		
		tests();	
		DWORD a=0x00534AC8;
		BYTE d=0x02;
		nPatcher::PatchData((LPVOID)a,&d,1);//tcp/ip
		 
		//PatchInsideArrays(replaceinsidearrays,NEWMAX_SHIPS/MAX_SHIPS+1);

		//PatchLimits(maxships,2,NEWMAX_SHIPS);
		//PatchLimits(maxships_mult_500,4,NEWMAX_SHIPS*SHIP_RECORD_SIZE);
		

		/*int count=0;
		for(int i=0;i<end;i++)
		{
			if(arr[i+2]==find[1] && arr[i+3]==find[0])
				for(int j=0x2CE0;j<0x47C0;j++)
				if(arr[i]==LOBYTE(j) && arr[i+1]==HIBYTE(j) )
				{
					pLog.Printf("0x00%X\n",i+beg);
					count++;
				}
		}*/
		/*i32 beg = 0x00401000;
		i32 end = 0x0054EF7D-beg;
		int count=0;
		ui8 find[3]={ 0x66 , 0xF4, 0x01};//cmp ..,1f4h
		ui8*arr=(ui8*)beg;
		for(int i=0;i<end;i++)
		{
			if( arr[i]==find[0] && arr[i+2]==find[1] && arr[i+3]==find[2] )	
			{
				pLog.Printf("0x00%X\n",i+beg+2);
				count++;
			}
			if (arr[i]==find[0] && arr[i+3]==find[1] && arr[i+4]==find[2])
			{
				pLog.Printf("0x00%X\n",i+beg+3);
				count++;
			}
		}
		pLog.Printf("%d\n",count);*/
		/*for(int i=0;i<end;i++)
		{
			if(arr[i+2]==find[1] && arr[i+3]==find[0])
				for(int j=0x2CE0;j<0x47C0;j++)
				if(arr[i]==LOBYTE(j) && arr[i+1]==HIBYTE(j) )
				{
					pLog.Printf("0x00%X\n",i+beg);
					count++;
				}
		}*/
		//pLog.Printf("addresses %d replaced\n",count);

		//pLog.Deinitialize();
		//exit(1);
		
	} else
	if (dwReason == DLL_PROCESS_DETACH)
	{
		FreePatchedInsideArrays(replaceinsidearrays);
		pLog.Write("Deinitialized.\n");
		/*if (nConfig::Debug_Breakpoint)
		{
			MessageBox(0, "Detaching...", "MOO2", MB_OK);
		}*/
	}

	return TRUE;
}


int tests(void)
{	
	savegame_t savegame;
	assert(sizeof(MOX__colony_t)==COLONY_RECORD_SIZE);
	assert(sizeof(MOX__star_t)==STAR_RECORD_SIZE);
	assert(sizeof(MOX__planet_t)==PLANET_RECORD_SIZE);
	assert(sizeof(MOX__set_t)==MOXSET_RECORD_SIZE);
	assert(sizeof(MOX__ship_t)==SHIP_RECORD_SIZE); 
	assert(sizeof(MOX__moveable_box_t)==MOVEABLE_BOX_RECORD_SIZE);
	assert(sizeof(MOX__ship_weapon_t)==SHIPWEAPON_RECORD_SIZE);
	assert(sizeof(MOX__ship_design_t)==SHIPDESIGN_RECORD_SIZE);
	

	assert(sizeof(sound_t)==2);
	assert(sizeof(MOX__player_t)==PLAYER_RECORD_SIZE);
	
	assert(sizeof(savegame_t)==SAVEGAVE_RECORD_SIZE);
	return 0;	
}