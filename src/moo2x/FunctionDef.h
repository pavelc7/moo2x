#pragma once

typedef	void (* QuitCleanup_t)();
extern	QuitCleanup_t	QuitCleanup;

typedef	void (* sub_528740_t)(DWORD screenStride, DWORD numlines);
extern	sub_528740_t	sub_528740;

//typedef	void (* video_Set_Page_Off_t)();
//extern	video_Set_Page_Off_t	video_Set_Page_Off;

//typedef	void (* graphics_fill_t)(ui32 x1, ui32 y1, ui32 x2, ui32 y2, ui32 flags);
//extern	graphics_fill_t	graphics_fill;

//typedef	void (* video_Set_Page_Back_t)();
//extern	video_Set_Page_Back_t	video_Set_Page_Back;

typedef	void (* sub_5287A0_t)();
extern	sub_5287A0_t	sub_5287A0;

typedef	void (* sub_5287C0_t)();
extern	sub_5287C0_t	sub_5287C0;

typedef	void (* graphics_Reset_Window_t)();
extern	graphics_Reset_Window_t	graphics_Reset_Window;

//typedef	void	(* ParseCommandLine_t)(const char * szCommandLine);
//extern	ParseCommandLine_t	ParseCommandLine;

//typedef	int (__stdcall * WndProc_t)(HWND hWnd, ui32 Msg, ui32 uVirtKey, LPARAM lParam);
//extern	WndProc_t	WndProc;

//typedef	int	(* MOX2_Main_t)(void*);
//extern	MOX2_Main_t	MOX2_Main;


typedef	void	(* sub_526F80_t)();
extern	sub_526F80_t	sub_526F80;

//typedef	void	(* video_Set_Refresh_Stencil_t)();
//extern	video_Set_Refresh_Stencil_t	video_Set_Refresh_Stencil;

//typedef	void	(* exit_Exit_With_Message_t)(const char * szMessage);
//extern	exit_Exit_With_Message_t	exit_Exit_With_Message;

//typedef	i32	(* DIR_t)(char * name,char*path);
//extern	DIR_t	DIR;
//for exitting
typedef	char* (* sub_52D290_t)();
extern sub_52D290_t sub_52D290;
typedef	int	(* sub_52DC80_t)();
extern sub_52DC80_t sub_52DC80;
typedef	void (* sub_534B40_t)();
extern sub_534B40_t sub_534B40;

typedef	i32 (* i32_cdecl_funcname_void_t)();
typedef	i32 (* i32_cdecl_funcname_i32_t)(i32);
typedef	i32* (* pi32_cdecl_funcname_i32_t)(i32);
extern i32_cdecl_funcname_void_t HAROLD_Harold_Defaults_; //004F0AA0

typedef	void (* void_cdecl_funcname_void_t)();
typedef	void (* void_cdecl_funcname_i32_t)(i32);
typedef	void* (* pvoid_cdecl_funcname_ui32_t)(ui32);

typedef	char* (* pchar_cdecl_funcname_void_t)();
extern pchar_cdecl_funcname_void_t BILL_Bill_Default_;

typedef	i8 (* i8_cdecl_funcname_void_t)();
typedef	i16 (* i16_cdecl_funcname_void_t)();
extern i16_cdecl_funcname_void_t sub_4F0F60;


//char *__cdecl sub_407920()
extern pchar_cdecl_funcname_void_t sub_407920;
extern i32_cdecl_funcname_void_t HAROLD_Save_Mouse_List_;//int __cdecl HAROLD_Save_Mouse_List_() 004F2190
 
//extern pchar_cdecl_funcname_void_t RUSS_Mox_Update_;//004D7820 RUSS_Mox_Update_ char* __cdecl RUSS_Mox_Update_()
extern pchar_cdecl_funcname_void_t ASSERT_hgetch_; //0x004AB400

//typedef	i32 (* i32_cdecl_funcname_5xi16_t)(i16 x1, i16 y1, i16 x2, i16 y2, i16 color);
//extern i32_cdecl_funcname_5xi16_t graphics_fill; //i32 __cdecl graphics_Fill(i16 x1, i16 y1, i16 x2, i16 y2, i16 color); //0x00528CC0

extern i32_cdecl_funcname_void_t MISC_Increment_Mouse_Clock_Image_; //0x0049EA60 

typedef	i32 (* sub_43D290_t)(MOX__set_t* a1);
extern sub_43D290_t sub_43D290;//0x0043D290 int __cdecl sub_43D290(int a1)

void	InitializeFunctionDef();

extern i8_cdecl_funcname_void_t MAPGEN_Load_Nebula_Pictures_;//004B7040
extern i32_cdecl_funcname_void_t MAINSCR_Update_Ship_Icons_In_Between_Turns_;//004CD550
extern i16_cdecl_funcname_void_t FLEETPOP_Reload_Fleet_Box_Data_; //00405990
extern void_cdecl_funcname_void_t HAROLD_Confirm_Planet_And_Star_Index_Link_;//004F0640
extern void_cdecl_funcname_void_t MAINSCR_Update_Star_Owners_;//004CD560
extern i32_cdecl_funcname_void_t HAROLD_Load_Leader_Titles_;//004F26F0
extern void_cdecl_funcname_void_t HAROLD_Restore_Mouse_List_;//004F21D0
extern i32_cdecl_funcname_i32_t RUSS_Release_All_Blocks_;//004D7830

extern void_cdecl_funcname_void_t HAROLD_Set_Sound_Volume_To_Settings_; // 004F1970 

extern i32_cdecl_funcname_void_t INITGAME_Set_Default_Game_Settings_; //0x00478E80

//extern pi32_cdecl_funcname_i32_t allocate_Allocate_Space; //0x0052E880
extern pi32_cdecl_funcname_i32_t  buffer_Allocate_Buffer_Space;//0x0052C200
//extern pi32_cdecl_funcname_i32_t  allocate_Allocate_Dos_Space;//0x0052E8E0

typedef	ui32 (* ui32_cdecl_funcname_i32_t)(i32);
extern ui32_cdecl_funcname_i32_t sound_Set_Audio_Buffer; //0x0052D580

typedef i32 (*init_Init_Drivers_t)(int mode, const char *fontfile);
extern init_Init_Drivers_t init_Init_Drivers;//0x005335B0

extern void_cdecl_funcname_void_t play_intro_sub_425A70;

typedef i32 (*fonts_Load_Palette_t)(i16 entry_name_num, i16 min1, i16 max1);
extern fonts_Load_Palette_t fonts_Load_Palette; //00525400

typedef LRESULT (*mouse_Set_Mouse_List_t)(i32* a1, i16 a2);
extern mouse_Set_Mouse_List_t mouse_Set_Mouse_List; //005274D0

extern i8_cdecl_funcname_void_t JIM_Play_Background_Music_;//0x004251D0


//typedef i8*  (*MISC_Font_Colors2_t)(i32 ,i16 a2, i16 a3, i16 a4);//0x0049DDB0
//extern MISC_Font_Colors2_t MISC_Font_Colors2_;//0x0049DDB0 usercall

//unsigned int __cdecl fonts_Set_Font_Style(int a1, int a2)
typedef ui32 (*fonts_Set_Font_Style_t)(i32 a1, i32 a2);
extern fonts_Set_Font_Style_t fonts_Set_Font_Style;//0x005255A0

typedef int (*INITGAME_Init_New_Game_t)(__int16 a1, __int16 a2, int a3, __int16 a4);
extern INITGAME_Init_New_Game_t INITGAME_Init_New_Game_; //0x00478820

extern i8_cdecl_funcname_void_t MOX2_Load_Mox_Global_Strings_; //0x00483F60

extern i32_cdecl_funcname_void_t LOADER_Load_Pictures_;//0x004B3140

extern i32_cdecl_funcname_void_t MOX2_Screen_Control_;//0x00483830

//typedef void (__fastcall *fonts_Print_Display_t)(int a1, int a2, __int16 a3, __int16 a4, char* pstr, __int16 a6); 
//extern fonts_Print_Display_t fonts_Print_Display;//00525930

typedef void (*fonts_Print_t)(int a1, int a2, char* pstr); //0x00525910
extern fonts_Print_t fonts_Print;//0x00525910

extern pvoid_cdecl_funcname_ui32_t malloc_in_orion95;//0x005421A0

extern i32_cdecl_funcname_void_t SHIPMOVE_Allocate_New_Ship_Slots_;//0x00436F50
extern pchar_cdecl_funcname_void_t INITGAME_Init_Leaders_;//00479580
extern pchar_cdecl_funcname_void_t  INITGAME_Init_Players_;//00479000

typedef HRESULT (*palstore_Darken_Palette_t)(int ); 
extern palstore_Darken_Palette_t palstore_Darken_Palette_; // 0052B850
typedef int (*palette_Clear_Palette_Changes_t)(__int16 a1, __int16 a2);
extern palette_Clear_Palette_Changes_t palette_Clear_Palette_Changes;

typedef void (*setvesa_Add_Square_To_Scan_List_t)(int x1, int y1, int x2, int y2);
extern setvesa_Add_Square_To_Scan_List_t setvesa_Add_Square_To_Scan_List_; //0x005287E0

extern void_cdecl_funcname_void_t sub_527ED0;

typedef void (*sub_526DD0_t)(__int16 a1, __int16 a2, __int16 a3);
extern sub_526DD0_t sub_526DD0;

//typedef void (*sub_536F23_t)(int flags, void *data, int beg, int len);
//extern sub_536F23_t sub_536F23;

typedef int (*buffer_Buffer_Reload_t)(const char *filename, int frame, i32* buf);
extern buffer_Buffer_Reload_t buffer_Buffer_Reload;

//typedef __int16 (*animate_Draw_t)(__int16 a1, __int16 a2, int a3); //00524490
//extern animate_Draw_t animate_Draw;//00524490

typedef int  (*sub_528A30_t)(signed int a1, int y, int x);
extern sub_528A30_t sub_528A30;

typedef unsigned int (*sub_535C00_t)(signed int x1, signed int y1, int buf);
extern sub_535C00_t sub_535C00;

typedef unsigned int (*sub_535D20_t)(__int16 a1, __int16 a2, int a3);
extern sub_535D20_t sub_535D20;

typedef unsigned int (*sub_524990_t)(__int16 x1, __int16 y1, int buf);
extern sub_524990_t sub_524990;
typedef unsigned int (*sub_524BB0_t)(int a1);
extern sub_524BB0_t sub_524BB0;
typedef int (*sub_524AE0_t)(int a1);
extern sub_524AE0_t sub_524AE0;
typedef unsigned int (*sub_524A50_t)(int a1);
extern sub_524A50_t sub_524A50;


typedef i8  (*sub_4CA430_t)(__int16 a1, char a2, int a3, char *a4, int *a5);
extern sub_4CA430_t sub_4CA430;
typedef __int16  (*sub_4CA720_t)(__int16 a1, char a2, int a3, int a4);
extern sub_4CA720_t sub_4CA720;
typedef void (*MAINSCR_Draw_Relocation_Links_t)(int a1);
extern MAINSCR_Draw_Relocation_Links_t MAINSCR_Draw_Relocation_Links_;
typedef ui8 (*sub_4C6880_t)();
extern sub_4C6880_t sub_4C6880;
typedef void (*sub_4CA990_t)(unsigned __int16 a1, __int16 a2);
extern sub_4CA990_t sub_4CA990;

typedef int (*sub_51CA30_t)(const char *a1, int a2, int a3, int a4, unsigned int a5, unsigned __int16 a6, __int16 a7);
extern sub_51CA30_t sub_51CA30;

extern i32_cdecl_funcname_void_t sub_4A02F0;

extern i32_cdecl_funcname_void_t sub_535570;
#ifdef DPLAY_USE_IN_DLL
 extern LPDPENUMSESSIONSCALLBACK sub_538990;
#endif

typedef  int (__cdecl * i32_pfunc_4xUI32_t)(DWORD, DWORD, DWORD, DWORD);
typedef  int (__cdecl * i32_pfunc_3xUI32_t)(DWORD, DWORD, DWORD);
typedef i32 (*sub_539800_t)(unsigned __int16 a1, int a2,i32_pfunc_4xUI32_t a3);
extern sub_539800_t sub_539800;

typedef i32 (__cdecl *sub_53A560_t)(__int16 a1, int a2, unsigned __int16 a3, int a4);
extern sub_53A560_t sub_53A560;

extern i32_cdecl_funcname_void_t menu_credit_sub_4E9780;

typedef void (__cdecl *fonts_Print_Centered_t)(__int16 x1, __int16 y1, char *text);
extern fonts_Print_Centered_t fonts_Print_Centered;

extern void_cdecl_funcname_void_t sound_sub_52E630;
extern i32_cdecl_funcname_i32_t net_sub_5156C0;

//net
typedef signed int (__cdecl *simmodem_Init_Modem_t)(char *a1, int a2);
extern simmodem_Init_Modem_t simmodem_Init_Modem;

typedef signed int (__cdecl *sub_5337F0_t)(int a1, int a2, int a3);
extern sub_5337F0_t sub_5337F0;

#ifndef DPLAY_USE_IN_DLL
typedef int (__cdecl *sub_5387A0_t)(DWORD            dpId,
    LPCSTR           lpName1,
    LPCSTR       lpName2);
extern sub_5387A0_t sub_5387A0;
extern void_cdecl_funcname_i32_t sub_538850;
extern i32_cdecl_funcname_i32_t net_create_sub_5386D0;
extern i32_pfunc_4xUI32_t DP_enum_sessions_sub_534FA0;
typedef i32 (__cdecl *sub_539AD0_t)(DWORD* a1);
extern sub_539AD0_t sub_539AD0;
#endif