#pragma once

struct Limits 
{
	DWORD	m_Address;
	DWORD data;
	WORD size;
};
void PatchData(struct Limits * patchlimits);
#define g_globalAvailableMemory	(*(ui32 *)(0x00578E10))
#define g_pDirectDraw			(*(LPDIRECTDRAW *)(0x00578E04))
#define g_hWnd					(*(HWND *)(0x00579320))

#define g_screen_res_x			(*(i32 *)(0x0055B1A8))
#define g_screen_res_y			(*(i32 *)(0x0055B1AC))
#define g_screen_stride			(*(ui32 *)(0x0055B1B0))

#define	g_lpPrimarySurface			(*(LPDIRECTDRAWSURFACE *)(0x005792EC))
#define	g_lpBackBufferSurface		(*(LPDIRECTDRAWSURFACE *)(0x00578E18))

#define	g_lpSurface1			(*(LPDIRECTDRAWSURFACE *)(0x00578E58))
#define	g_lpSurface2			(*(LPDIRECTDRAWSURFACE *)(0x00579334))

#define g_pPalette					(*(LPDIRECTDRAWPALETTE *)(0x00578E64))
#define g_pGlobalPalette			(*(tagPALETTEENTRY *)(0x00578EE8))

#define g_error_status				(*(ui8 *)(0x00578E68))

//#define dword_55B1B4				(*(ui32 *)(0x0055B1B4))
#define video_screen_refresh_flag (*(i32*)(0x0055B1B4))
#define video_screen_dump_flag (*(i32*)(0x0055B1A0))

#define dword_578E20				(*(ui8 **)(0x00578E20))

#define byte_579350					(*(ui8 *)(0x00579350))

#define word_57931C					(*(ui16 *)(0x0057931C))

#define	FrameCount					(*(ui32 *)(0x0055B1B8))

#define ThreadHandle				(*(ui32 *)(0x0055B1BC))

#define global_QuitFlag				(*(ui8 *)(0x0055B1A4))

#define dword_579328				(*(ui32 *)(0x00579328))

#define bRenderBuffer				(*(ui8 *)(0x00578EDC))

#define word_578AC4					(*(i16 *)(0x00578AC4))
#define	dword_578B3C				(*(i32 *)(0x00578B3C))

#define g_pCursorSurface1		(*(LPDIRECTDRAWSURFACE *)(0x00578810))
#define g_pCursorSurface2		(*(LPDIRECTDRAWSURFACE *)(0x00578814))

#define MouseX						(*(ui16 *)(0x00578AA0))
#define MouseY						(*(ui16 *)(0x00578A9C))

#define cached_mouse_x				(*(ui16 *)(0x00578ABC))
#define cached_mouse_y				(*(ui16 *)(0x00578AB8))
//////////////////////////////////////////////////////////////////////////
HWND	CreateDirectDraw(DWORD videoMode);
void	FlipSurface();
int __stdcall	WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd);
HRESULT	UnlockAppropriateRenderBuffer(); //0x005281A0
void	UnlockPrimarySurface();
void	BlitToBackbuffer();
void	DrawCursorXY(i16 x, i16 y);
void	ForceCursorPos(i16 x, i16 y);
void	CreateCursorSurfaces();
void	DrawAutoCursor();
HRESULT	RestoreSurfaces();
void	CleanupSurfaces();
HRESULT __cdecl LockPrimarySurface(); //0x00528470
HRESULT __cdecl LockAppropriateRenderBuffer(); //0x005282D0

void	WorkerThread(void*param);

//#define config_language					(*(ui8 *)(0x00597152))
signed int MOX2_Set_Mox_Language_();
void __cdecl MOX2_Set_Mox_Alt_Path_();//00483CE0
i32 __cdecl set_path_sub_51D0F0(const char *path);//0x0051D0F0

i32_cdecl_funcname_void_t __cdecl fields_Assign_Update_Function(i32_cdecl_funcname_void_t pFunc);//00523E00


//#define finddata_57B5D8					(*(struct _finddata_t *)(0x0057B5D8)) //differant size now 0x128 instead 0x118
i32 DIR(const char* a1, char *a2);

#define	dword_timestamp_57B5D0				(*(i32 *)(0x0057B5D0)) //used only in get_timestamp_52D3D0 ?
struct tm* get_timestamp_52D3D0(char* a1); //get_timestamp
int sub_49E9B0(char* filename, ui16* month, ui16* mday, ui16* year, ui16* hour, ui16* min1, ui16* sec);


//exitting
i16 sub_527630();
i16 sub_527640();
i16 fields_Clear_Fields();
void sub_533630();
void __cdecl exit_Exit_With_Message(const CHAR *lpText);


void __cdecl ERIC_Eric_Defaults_(); //empty
void __cdecl nullsub(); //empty
char* DoSomeInit();
void __cdecl KEN_Ken_Init_();  //004C1D40 empty
i32 __cdecl BILL_Bill_Init_();
i32* __cdecl HAROLD_Harold_Init_();

void __cdecl COLCALC_Rebuild_WFI_List_();//empty 0x04FBEF0 COLCALC_Rebuild_WFI_List_

i32 __cdecl sub_4F0E90();
i16 __cdecl sub_4F0ED0();

BYTE __cdecl FILEDEF_Load_Game_(i16 numfile);
void __cdecl FILEDEF_Save_Game_(i16 numfile);//0043C840
i8 __cdecl FILEDEF_Save_Game_Settings_(); //0043CDB0
void __cdecl FILEDEF_Load_Game_Settings_(); //0043CE10
i8 __cdecl FILEDEF_Update_Game_Settings_Version_Number_(); //0043D300

ui32 __cdecl random_Set_Random_Seed(ui32 seed);
ui32 __cdecl random_Get_Random_Seed(); //0052D4D0
ui32 __cdecl random_set_sub_52D450();
ui32 __cdecl random_get_sub_52D460();
ui32 __cdecl random_Random(ui32 random); //0052D470
ui32 __cdecl random_from_time_sub_52D4E0(); //0x0052D4E0
ui32 __cdecl MOX2_Set_Game_Random_Seed_(); //0x00483CC0

bool __cdecl MOVEBOX_Moveable_Box_Selected_(i16 a1); //00513B90
void __cdecl RUSS_Assert_Settings_(); //empty 004D7810


void __cdecl video_Set_Page_Off(); //0x00528680
i32 __cdecl video_Set_Refresh_Full(); //0x00527C80
void __cdecl video_Set_Refresh_Stencil(); //0x00527CD0
void __cdecl video_Set_Refresh_Differential();//0x00527D00
void __cdecl video_Set_Page_Back(); //0x005286A0
int __cdecl video_Copy_Back_To_Off();//00528700
int __cdecl video_Copy_Off_To_Back();//005286C0

void __cdecl ERIC_Fast_Fade_In_(); //0x004BA740
void __cdecl ERIC_Fast_Fade_Out_();//0x004BA690

void *__cdecl do_malloc_hooked(size_t size); // 0x0052E9C0
void *__cdecl malloc_hooked(size_t);//0x005421A0
void __cdecl free_hooked(void *);//0x00542270
size_t __cdecl msize_hooked(void *ptr);//00546F50
void* __cdecl realloc_hooked(void *lpMem, size_t size);//00546D80

i16 __cdecl SHIPSTAK_Ship_Has_Special_(i16 nShip, ui16 bit); //0x00460600
i32	MOX2_Main(void*param);//0x00483470

void __cdecl capture_Release_Version(); //0520200
i16 __cdecl capture_Check_Release_Version();//00520210
void ParseCommandLine(const char * szCommandLine);

//font
char* __cdecl HAROLD_Get_Font_File_(char* fontfile); //0x004F24D0
i16 __cdecl fonts_Set_PSTR_Style(i16 font_style);//0x00526D00
ui8* MISC_Font_Colors2_(int a1, __int16 type, __int16 colors, __int16 palette); //0x0049DDB0
i16 __cdecl get_type_color_sub_4F1E30();
//alloc
void __cdecl ALLOC_Allocate_Data_Space_();//0x00500B20
i32* allocate_Allocate_Dos_Space(size_t size);//0x0052E8E0
i32* allocate_Allocate_Space(size_t size);//0x0052E880
i32* allocate_Allocate_Space_No_Header(size_t size); //0x0052EA60

i16 HAROLD_Map_Scale_To_Zoom_Level_();//004F1090


i8 __cdecl FILEDEF_Read_Moo_Custom_Player_Data_();//0x0043D1C0


i8 __cdecl video_Toggle_Pages();//0x00527EA0
i8 __cdecl MOX2__TOGGLE_PAGES_();//0x00483C80

void RUSS_PFMT_(int a1, int a2, const char *format, ...);//004D7850
//void __cdecl fonts_Print(int a1, int a2, char* pstr); //00525910

i16 __cdecl set_map_maxxy_sub_4B6C40();//0x004B6C40
i16 __cdecl get_galaxy_size_sub_4F1050();//0x004F1050


i16 zoom_scale_sub_4F0880(i16 galaxy_size);
int __cdecl zoom_x_sub_4CD5D0(__int16 a1, __int16 a2);
int __cdecl zoom_y_sub_4CD5B0(__int16 a1, __int16 a2);

//i32 __cdecl INITGAME_Init_New_Game_(__int16 a1, __int16 galaxy_size, __int16 a3, __int16 num_players); //0x00478820
i32 __cdecl INITGAME_Init_Ships_Array_();//0x00478B30
i32 __cdecl INITGAME_Init_Colonies_Array_();//0x00478D80
i8 __cdecl MSG_Allocate_Msg_Slots_(); //0x004726E0
i16* __cdecl strings_Swap_Short(i16* a1, i16* a2);//0052CE60

void __cdecl sub_536F23(int flags, void *data, int beg, int len);
void __cdecl sub_536EE8(i8 flags, i8* data, int beg, int len);
ui32 __cdecl graphics_fill(i16 x1, i16 y1, i16 x2, i16 y2, i16 flags);//00528CC0

int __stdcall WndProc(HWND hWnd, ui32 Msg, ui32 uVirtKey, LPARAM lParam);
void __cdecl sub_4C6930();
i16 __cdecl graphics_Set_Window(i16 a1, i16 a2, i16 a3, i16 a4); //00528BA0

i8 __cdecl sub_527C60();
i8 __cdecl clear_video_sub_52D290();
i8 __cdecl sub_40BCC0(__int16 a1, __int16 a2);


//ui32 __cdecl sub_535E50(i16 x1, i16 y1, i32 header);
ui32 __cdecl sub_535E50(i16 x1, i16 y1, i32 pframe);
void __cdecl draw_uncompressed_sub_5365B0(ui16 x1, ui16 y1, i32 header);
unsigned int __cdecl sub_536280(__int16 x1, __int16 y1, int header);
unsigned int __cdecl sub_524830(__int16 a1, __int16 a2, int a3);
int __cdecl sub_524C30(int a1);
//void __cdecl sub_5365B0(unsigned __int16 x1, unsigned __int16 y1, int header);
unsigned int __cdecl sub_5250F0(__int16 a1, __int16 a2, int a3);
unsigned int __cdecl sub_536040(__int16 x1, __int16 y1, int buf);
//__int16 __cdecl animate_Draw(__int16 x1, __int16 y1, int header);//00524490
__int16 __cdecl animate_Draw(__int16 x1, __int16 y1, struct PicFrameHeader* header);
int __cdecl animate_Set_Animation_Palette();//00524930
void __cdecl draw_compressed_sub_536508(unsigned __int16 x1, unsigned __int16 y1, int buf);

//map
__int16 __cdecl map_sub_4C7260(__int16 x1, __int16 y1, __int16 scale);//0x004C7260
__int16 __cdecl MAINSCR_Center_Map_(__int16 x1, __int16 y1); //004C49D0
i32 __cdecl HAROLD_Get_Up_Scaled_Value_(i16 a1);//004F0A50

void __cdecl MAINSCR_Print_Star_Names_();//004CBA80
//void MAINSCR_Draw_Main_Screen_Filled_(unsigned __int16 a1, __int16 a2, __int16 a3);//004C6550

int __cdecl ESTRINGS_E_Strings_(__int16 index); // 0x004A0050
char __cdecl ESTRINGS_Load_E_Strings_(); // 004A0060
int __cdecl farload_Farload_Data_Static(char *a1, int a2, int a3, int a4, __int16 a5, __int16 a6); //0051C7B0

i16 __cdecl ship_add_sub_5094A0(__int16 player_id, __int16 a2, __int16 colony_id);
__int16 __cdecl HAROLD_N_Player_Ships_(__int16 owner); //0x004F1330
i16 __cdecl HAROLD_XY_To_Star_Id_(__int16 x, __int16 y); //0x004F1DC0
void helper_clean_ship_array();
//net
i8 __cdecl net_check_GUID_sub_538680(const char *nettypename);
i32 __stdcall guid_sub_5385C0(int a1, const void *a2, int a3, int a4, int a5);

void __cdecl DP_sub_539CE0(signed int a1);
#ifdef DPLAY_USE_IN_DLL
int __cdecl net_sub_538760();

i32 __cdecl sub_539AD0(DWORD* a1);

void __cdecl kill_net_sub_534B40();
int __cdecl sub_5387A0(DPID            dpId,
    LPCSTR           lpName1,
    LPCSTR       lpName2);
i32 __stdcall sub_538780(DPID            dpId,
   // DWORD           dwPlayerType,
   // LPCDPNAME       lpName,
   LPCSTR   lpFriendlyName,
   LPCSTR   lpFormalName,
    DWORD           dwFlags,
    LPVOID          lpContext );
void __cdecl DP_Create_Player_sub_538940(LPDPID a1,  LPSTR a2,  LPSTR a3);
signed int __cdecl DP_enum_players_sub_534DB0(int a1);
i32 __cdecl net_create_sub_5386D0(i32 type);


HRESULT __cdecl net_sub_535400();//allow adding new players
HRESULT __cdecl net_sub_535420();//disallow
i32 __cdecl netcode_Net_Create_Game(int a1, int a2, int a3, int a4,  char *a5, const char *a6, const char *a7,  char*a8); //00535290
i32 __cdecl netcode_Net_Join_Game(const char *a1, const char *a2,  char *a3);
const char* __cdecl sub_5350C0(ui32 a1);
int __cdecl DP_enum_sessions_sub_534FA0(int a1, int a2, int a3, int a4);
void __cdecl DP_err_sub_534E30(HRESULT err);
i16 __cdecl DP_send_sub_539950(int data, unsigned __int16 size, int a3, unsigned __int16 a4, char a5, __int16 a6);


	
	void __cdecl sub_538850(int a1);
	
#endif
	//net
	signed int __cdecl net_decode_sub_539B80(DWORD *data);
	int __cdecl net_decode_sub_539BE0(DWORD *data);
	void __cdecl net_decode_sub_539BC0(DWORD *data);
	signed int __cdecl netcode_Net_Init(int type, int guid1, int guid2, int guid3, int guid4, int pFunc, char *data);
	//
	int __cdecl sub_539C90(DWORD *data);
i8 __cdecl sub_539C00(DWORD* data);
i8 __cdecl net_decode_sub_539A60();

ui8 __cdecl sub_52DDC0();
void MAINMENU_Draw_Main_Menu_Screen_(/*int a1,int a2*/); //0x004E95B0
i16 __cdecl sub_526060();
i32 __cdecl RUSS_Mox_Update_(void);


void UpdateThread( void * param);

int __cdecl callback_sub_534A70();

