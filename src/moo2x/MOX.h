#pragma once

const int MULTI = 128;//256;

#include	"Ship.h"

#define MOX__stardate		(*(ui32 *)(0x0059BA64))

#define	MOX__ship			(*(MOX__ship_t **)(0x0059BD3C))

#define	MOX__NUM_PLAYERS	(*(ui8 *)(0x005A2C1C))

#define	MOX__FULL_DRAW_SCREEN_FLAG (*(ui32 *)(0x005A2C24))

#define	MOX__NUM_NEBULAS (*(i8 *)(0x00596d10))

#define	MOX__current_screen  (*(ui32 *)(0x00597302))
#define	MOX__settings (*(MOX__set_t*)(0x00597080))

#define	MOX__player (*(MOX__player_t **)(0x0059B3D0))
#define MOX__settings (*(MOX__set_t*)(0x00597080))
#define MOX__NUM_SHIPS	(*(i16*)(0x0059B4BC))
#define MOX__PLAYER_NUM (*(i16*)(0x005A18F2))
#define MOX__max_map_scale	(*(i16*)(0x00597768))
#define MOX__max_zoom_count	(*(i16*)(0x0059B448))
#define MOX__star_bg_index	(*(i8*)(0x005A49F8))
#define MOX__MAP_MAX_X	(*(i16*)(0x00597998))
#define MOX__MAP_MAX_Y	(*(i16*)(0x0059799A))
#define MOX__fleet_icon_button_count	(*(i16*)(0x00596D18))

#define MOX__leaders (*(MOX__leader_t**)(0x0059BF40))
#define pMOX__nebula (*(MOX__nebula_t**)(0x00597770))
#define pMOX__nebula_screen_seg (*(i32**)(0x005A2BEC))
#define REPORT__displayed_msgs (*(i8*)(0x005A5276))
#define DIPLODEF__COUNCIL_FLAG (*(i16*)(0x0059600E))
#define DIPLODEF__next_council_meeting_turn (*(i16*)(0x0059600C))
#define MOX__antaran_resource_level (*(i16*)(0x0059790C))
#define MOX__chance_for_antaran_invasion (*(i16*)(0x00596D24))
#define MOX__disallow_saves_code  (*(i16*)(0x00597854))
#define EVENTS__last_event_year  (*(i32*)(0x005A65A8))
#define EVENTS__event_delay_count  (*(i16*)(0x005A65AE))
#define MOX__antarans (*(MOX__antarans_t*)(0x0059BA20))
#define MOX__cur_map_scale (*(i16*)(0x0059D604))

#define MOX__save_game_description (*(char*)(0x005974F0))
#define MOX__NUM_COLONIES   (*(i16*)(0x00597856))
#define MOX__game_type (*(BYTE*)(0x005974DD))  
#define MOX__colony (*(MOX__colony_t**)(0x005A1828))
#define MOX__planet_count   (*(i16*)(0x0059700E))  
#define MOX__planet (*(MOX__planet_t**)(0x005978F8)) 
#define MOX__NUM_STARS (*(i16*)(0x0059740A))
#define MOX__star (*(MOX__star_t**)(0x005A18F4)) 

#define pMOX__screen_seg (*(i32**)(0x005973E4))

#define WM_USER_UPDATE 0x401
#define WM_USER_UPDATEMOUSE 0x402
#define dword_55C08C (*(i32*)(0x0055C08C))
#define word_578DE0 (*(i16*)(0x00578DE0))
#define byte_57E120  (*(i8*)(0x0057E120))
#define byte_57E124  (*(i8*)(0x0057E124))
#define dword_57E0F8 ((i32*)(0x0057E0F8))
#define dword_589A1C (*(i32*)(0x00589A1C))