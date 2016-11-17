#pragma once
#pragma pack(push, 1)
extern	i32		struct_Test_Bit_Field_(const ui8 * pbStruct, ui16 bit);
extern	void	struct_Set_Bit_Field_(ui8 * pbStruct, ui16 bit);
extern	void	struct_Clear_Bit_Field_(ui8 * pbStruct, ui16 bit);
extern	void	struct_Clear_Structure_(void * pbStruct, ui32 length);
extern	void	struct_Copy_Structure_(void * pbTarget, const void * pbSource, ui32 length);

#define SAVEGAVE_RECORD_SIZE 208000

//#define MAX_EVENT_DATAS 36
//#define MAX_MSGS 100
const i32 MAX_STARS=72;
const i32 MAX_MSGS=100;
const i32 MAX_EVENT_DATAS=36;
const i32 MAX_COLONIES=250;
const i32 MAX_PLANETS=360;
const i32 MAX_LEADERS=67;
const i32 MAX_PLAYERS=8;
const i32 MAX_NEBULAS=4;
const i32 MAX_MOVEABLE_BOXS=5;
const i32 MAX_SHIPS=500;

const i32 NEWMAX_PLAYERS =   1* MAX_PLAYERS;
const i32 NEWMAX_PLANETS =   1* MAX_PLANETS; //360
const i32 NEWMAX_COLONIES =  1* MAX_COLONIES; //250
const i32 NEWMAX_NEBULAS =   1* MAX_NEBULAS; //4
const i32 NEWMAX_STARS =     1* MAX_STARS;     //72
const i32 NEWMAX_SHIPS =   /*0x7FF4;*/1* MAX_SHIPS;//0x7FFF;//10 * MAX_SHIPS;    //500 = 1F4h => 7FFFh signed???
const i32 NEWMAX_LEADERS =     1* MAX_LEADERS;    //67

#define MAX_RACES       14
#define N_POP_PLAYERS (MAX_PLAYERS+2)
#define N_POP_RACES (MAX_RACES+2)

enum
{
	scr_Main_Screen=0x0,
	scr_Colony_Screen=0x1,
	scr_Design_Screen=0x3,
	scr_Fleet_Screen=0x4,
	scr_Race_Screen=0x6,
	scr_screenQuitNow=0x7,
	scr_LoadSave_Game_Popup=0x8,
	scr_Info_Screen=0x9,
	scr_Main_Menu_Screen=0xA,//10

	scr_Next_Turn=0xC,
	scr_Newgame_Screen=0xD,//13
	scr_Hall_Of_Fame_Screen=0xE,//14
	scr_Multi_Player_Screen=0xF,//15
	scr_Hotseat_Screen=0x10,
	scr_Hotseat_Select_Player=0x11,
	scr_Planet_Data_Screen=0x12,//18
	//13
	scr_Colony_Summary_Screen=0x14,
	scr_Start_Net_Screen=0x15,//21
	scr_Join_Net_Screen=0x16,//22
	//17
	//18
	scr_Build_Queue_Popup=0x19,//25
	//1a
	//1b
	scr_Distance_Between_Stars_Screen=0x1C,//28
	scr_Officers_Screen=0x1D,//29
	scr_Planet_Colonization_In_Main_Screen=0x1E,
	scr_Building_Outposts_In_Main_Screen=0x1F,
	scr_Planet_Summary_Screen=0x20,
	scr_Colony_Landing_Screen=0x21,
	scr_Unloading_Transports_In_Main_Screen=0x22,
	scr_Scrap_Freighters=0x23,//35
	scr_Tech_Change=0x24,
	scr_Net_Next_Turn=0x25,
	//26
	scr_Reports_Screen=0x27,//39
	scr_Turn_Summary_Popup=0x28,//40
	scr_Load_Net_Screen=0x29,//41
	//2a
	scr_Show_Command_Poiints_Screen=0x2B,//43
};


#define COLONY_RECORD_SIZE 361
//#define MAX_COLONIES 250
struct MOX__Pop_Info_t {
           unsigned player:4;           // player id or POP_ANDROID or POP_NATIVE
           unsigned loyalty:3;          // player id
           unsigned job:2;              // 0-farmer, 1-worker, 2-scientist
           unsigned employed:1;
           unsigned conquered:1;        // true iff loyalty != colony owner
};
struct MOX__colony_t //361 bytes
{//BYTE arr[COLONY_RECORD_SIZE];
	  i8 owner; //00
	  i8 allocated_to;//01
	  i16 planet_id; //02
	  //i8 unk_03;
	  i16 officer;//04
	  //i8 unk_05;
	  i8 is_outpost;//06
	  i8 morale; //07  Morale value is stored as divided by 5
	  i16 pollution;//08-09
	  i8 population;//0a
	  i8 assignment;/*0b			0x00 = Agricultural Colony    // Colony specialty used in display only
			0x01 = Industrial Colony
			0x02 = Research Colony
			0xff = (balanced?) Colony */
	  //		0x00c ~ 0x0b3		colonists
	  struct colonist_t{
		  i8 a,b,c,d;
	  };
	  //colonists = {0x02: [], 0x03: [], 0x82: []} // types of colonists??? 
	  /*
	  FARMER		= 0x02
SCIENTIST	= 0x03
WORKER		= 0x82
	  */
#define MAX_POPULATION_ON_PLANET 42
	  //colonist_t colonists[MAX_POPULATION_ON_PLANET];
	  MOX__Pop_Info_t colonists[MAX_POPULATION_ON_PLANET];
	  i16 pop_raised[N_POP_PLAYERS];//b4-c6 race 0...race 7,androids,natives  // K toward next pop unit for each race
	  i16 pop_grow[N_POP_PLAYERS];//c8-da,	    // each race grows independently
	  i8 n_turns_existed;//dc bookeeping
	  i8 food2_per_farmer;//dd Food per farmer in half-units of food   // Food per farmer in half-units of food
	  i8 industry_per_worker;//de 
	  i8 research_per_scientist;//df
	  i8 max_farms;//e0
	  i8 max_population;//e1
	  i8 climate;//e2
	  i16 ground_strength;//e3-e4 calculated for ai
	  i16 space_strength;//e5-e6 calculated for ai
	  union{
		  struct{
			i16 food;//e7-e8 total food = food - population
			i16 industry;//e9-ea
			i16 research;//eb-ec
		  };
		  i16   total_production[3];     // gross amount produced  0-food, 1-industry, 2-research
	  };
	  //i8 unk_ED[40]; //ed-114
	  i8  mai32enance[3]; //ed          // how much is spent feeding people/androids 0-food, 1-industry, 2-research
	  i16   imports[3];              // + means importing, - means surplus
                                        // (surplus exported only if unblockaded)
	  i8 n_industry_recyclers;           // for recycling pollution reduction
	  // for parcelling out food/metal in priority ordering:
	  // counted in half units due to cybernetic woes.

	  // for parcelling out food/metal in priority ordering:
   // counted in half units due to cybernetic woes.
   i8 food2_needed_for_our_empire;                   //-------------------------------
   i8 food2_needed_for_assimilated;                  //
   i8 food2_needed_for_conquered;                    //
   i8 food2_needed_for_natives;                      //these are all bookeeping entries
   i8 industry2_needed_for_our_empire;               //
   i8 industry2_needed_for_androids;                 //
   i8 industry2_needed_for_assimilated;              //
   i8 industry2_needed_for_conquered;                //
   i8 food2_needed_for_empire[MAX_PLAYERS];          //
   i8 industry2_needed_for_empire[MAX_PLAYERS];      //
   i8 n_food_replicated;                             //
   i8 unk_[5];//??????
	  struct building_item_t
	  {
		  i8 production_id; // can be 0xff
		  i8 flags;
	  };
	  union{
		building_item_t building_items[7]; //0x115-0x122
		i16  producing[7];                            // building queue
	  };
	  /*#		0x115		building item #0				# 0x0b = colony base??? 0xf9 = spy? 0xfd = housing 0xfe = trade goods
#			0x0b = colony base
#			0xf9 = spy
#			0xfd = housing
#			0xfe = trade goods
#			0xff = nothing
#		0x116		?
#		0x117		building item #1				# 0x0b = colony base??? 0xf9 = spy?
#		0x118		?
#		0x119		building item #2				# 0x0b = colony base??? 0xf9 = spy?
#		0x11a		?
#		0x11b		building item #3				# 0x0b = colony base??? 0xf9 = spy?
#		0x11c		?
#		0x11d		building item #4				# 0x0b = colony base??? 0xf9 = spy?
#		0x11e		?
#		0x11f		building item #5				# 0x0b = colony base??? 0xf9 = spy?
#		0x120		?
#		0x121		building item #6				# 0x0b = colony base??? 0xf9 = spy?
#		0x122		? */
	  //i8 unk_123[13];//123h-129h
	   i16  just_produced;                           // if not -1, then colony completed this product last turn
   i16  production_spent;
   i16  n_industry_taxed;
   i8       auto_building;
   i16  production_surplus;                      // for when production is changed
   i16  bought_outright;                         // amount of production bought
                                                       //---------------------------


   i8 occupation_poi32s;                             // 240 poi32s to assimilate 1 pop.
   i8 occupation_policy;                             // 0-genocide , 3-normal


   // military
   i16 military[2];                              // 0-infantry, 1-tanks
   i8 tank_roundoff;
   i8 infantry_roundoff;
	  /*
	  i8 marines;//130h
	  i8 unk_131;
	  i8 armors;//132h
	  i8 unk_133[4];//133-136*/
#define MAX_TYPE_OF_BUILDINGS 49 //???
	  i8 buildings[MAX_TYPE_OF_BUILDINGS];//137h-168h	  
	  ui16 last_turn_building_destroyed;
};
#define PLANET_RECORD_SIZE 17
struct MOX__planet_t //17bytes
{ //BYTE arr[PLANET_RECORD_SIZE];
	i16 colony_id; //	0xffff = no colony here //00-01     Link to Colony table (0-1fff=colony #, FFFF=not colonized)
	i8 star; //02        Link to Star table - Parent star (0-7f = system #)
	i8 orbit;//03        Orbit # (0 - 4)
	i8 type;/*04        Planet Type
               1=Asteroid
               2=Gas Giant
               3=Planet*/
	i8 size; /*05        Planet Size 
               0=tiny
               1=small
               2=medium
               3=large
               4=huge*/
    i8 gravity;/*06        Planet G
               0=Low G
               1=Normal G
               2=Heavy G*/
	i8 group;/* 07 unused?*/
	i8 terrain;/*08        Planet environment class
               0=toxic
               1=radiated
               2=baren
               3=desert
               4=tundra
               5=ocean
               6=swamp
               7=arid
               8=terran
               9=gaia*/
	i8 picture;/*09        Drawing number - Background image on colony screen (0-5=image in planets.lbx)*/
	i8 minerals;/*0A        Planet mineral class
               0=Ultra Poor
               1=Poor
               2=Abundant
               3=Rich
                4=Ultra Rich*/
	i8 foodbase;/*0B        Food Base*/
	i8 terraformations;/*0C        Number of Terraforms done (effects cost of next one)*/
	i8 max_farms;/*0D        Unknown (Initial value is based on Planet Size but changes if colonized)
               2=tiny
               4=small
               5=med
               7=large
               A=huge*/
	i8 max_population;/*0E        Unknown*/
	i8 special;/*0F        Planet special
               0=none
               2=Space Debris
               3=Pirate Cache
               4=Gold Deposites
               5=Gem Deposits
               6=Natives
               7=Spli32er Colony
               8=Hero
               A=Artifacts world
               B=Orion*/
	i8 flags;/*10        Flags (bit 2 = Soil Enhancement)*/	  
};
#define STAR_RECORD_SIZE 113
//see http://code.google.com/p/moomapgen/source/browse/trunk/struct.h
//      https://github.com/tf8/moomapgen/blob/master/struct.h
struct MOX__star_t //113 bytes
{	
	//BYTE arr[STAR_RECORD_SIZE];
	char name[15]; //00-0E     Star System Name (Null terminated string)
	i16 map_x;//0f-10     X-position (lo/hi)
	i16 map_y;//11-12     Y-position (lo/hi)
	i8 size; //13        Star size (0-3=Large-Tiny)
	i8 owner;     //14        System Primary Owner (0-7=player #, FF=unowned)
	i8 picture;  //15        Unknown pict_type
	i8 type; /*16        Star type
               0=Blue
               1=White
               2=Yellow
               3=Orange
               4=Red
               5=Brown
               6=Black Hole*/
	i8 last_planet_selected[MAX_PLAYERS]; //bookeeping     //[17]; //17-27     Unknown
	unsigned char aBHBlocks[(MAX_STARS + 7)/8];   //If bit is set there is a hole between stars      9       bytes
	i8 special; /*28        System Special
               0=none
               2=Space Debris
               3=Pirate Cache
               4=Gold Deposites
               5=Gem Deposits
               6=Natives
               7=Spli32er Colony
               8=Hero
               A=Artifacts world
               B=Orion*/
	i8 wormhole_to;//29        Wormhole leads to (0-7f=system #, FF = no wormhole)
	//i8 unk_2A[15];//2A-38     Unknown
	i8 blockaded_players; //2a
	i8 blockaded_by_bitmask[MAX_PLAYERS]; //2b-32
	i8 visited;//0x33  bitmask as boleans for each player
	i8 just_visited_bitmask;//34 players bitmask to track first visit of this star -> user should get report
	i8 ignore_colony_ship_bitmask;//35 players bitmask to track if player chose to not use a colony ship, cleared on every new colony ship here?
	i8 ignore_combat_bitmask; //36 players bitmask to track if player chose to ignore combat ships = perform blockade only do not fight here?
	i8 colonize_player;//37         0..7 or -1
    i8 colonies_bitmask;//38        has colony / players bitmask / redundant info? 
	i8 has_warp;//39        System has Warp Indictor (0=none, 1-8=owner player 0-7)  has warp i32erdictor / players bitmask
	//3A-3E     Unknown
	i8 next_wfi_in_list;//0x3a  bookeeping ???
    i8 tachyon_com_bitmask;//3b has tachyon communicator / players bitmask
    i8 subspace_com_bitmask;//3c    has subspace communicator / players bitmask
    i8 stargates_bitmask;//3d       has stargate / players bitmask
    i8 jumpgates_bitmask;//0x3e    has jumpgate / players bitmask
	i8 has_artimis_net;//3F        System has Artimis Net (0=none, 1-8=owner player 0-7)  has artemis net players bitmask
	//40-49     Unknown
	i8 portals_bitmask;// 40	 has dimension portal / players bitmask
    i8 stagepoi32_bitmask;//41	 bitvector tells whether star is stagepoi32 for each AI
	i8 players_officers[MAX_PLAYERS]; //42-49
	i16 planets_links[5];//4A-4B     Link to planets table - 1st orbit (planet # 0-1ff, FFFF=none) - closest to sun
	//4C-4D     Link to planets table - 2nd orbit (planet # 0-1ff, FFFF=none)
	//4E-4F     Link to planets table - 3rd orbit (planet # 0-1ff, FFFF=none)
	//50-51     Link to planets table - 4th orbit (planet # 0-1ff, FFFF=none)
	//52-53     Link to planets table - 5th orbit (planet # 0-1ff, FFFF=none) - furthest sun
	//i8 unk_54[19];//54-66     Unknown
	i16 relocation_star_id[MAX_PLAYERS];//54-63
	i8 _64[3];//64-66
	i8 surrender_to[MAX_PLAYERS];//67-6e 
    i8 is_in_nebula;//6f		
	i8 artifacts_gave_app; // has the ancient artifacts app been given out yet?
};
struct sound_t //word
{
	i8 on;
	i8 volume;
};
#define MOXSET_RECORD_SIZE 553
struct MOX__set_t //553
{
	//BYTE arr[553];
	i8 settings[17];
	sound_t soundfx;
	sound_t music;
	i8 active_save_slot;//unk_15;
	i8 multi_player_game_type;
	i8 net_game_name[9]; //
	i32 com_port;	//s_modem_comm_modem_settings[131]; //struct s_modem_comm modem_settings;
	i32 baudrate;
	i8 field_28;
	i32 field_29;
	i32 field_2D;
	i8 clearstring[30]; // "ATZ"
	i8 initstring[30]; // "AT &K0 M1 &D2"
	i8 handupstring[30]; // ""AT H0 S0=0"
	i8 phonenumber[40]; //40? bytes can be max 15-22 digits "5551212"
	i8 istonedial; //1-tone, 0-pulse	

	i8  combat_speed_flag;
    i8  combat_legal_moves_flag;
    i8  combat_msl_impact_flag;
    i8  combat_shield_arcs_flag;
    i8  combat_grid_flag;
	i8 field_B9[25];//_B4[30];	
	ui8 config_language;//D2
	i8 xenons_exist_flag;//field_D3;//d3-d5
	i8 game_difficulty;//field_D4;
	i8 num_players;//D5 num players 1-8 //MOX__NUM_PLAYERS
	i8 galaxy_size;//D6 galaxy size 0-3
	i8 galaxy_age;//field_D7;
	i8 strategic_combat_flag;//field_D8;
	i8 starting_civilization_level;//field_D9; //D9 ==2 =>advanced civ colonies
	i32 random_seed; //DAh start_of_game_seed
	i16 sound_toggle;//field_DE;
	i16 mouse_driver;//field_E0;
	i16 version;
	i8 field_E4[325];
};
//#define MAX_LEADERS 67
struct MOX__leader_t
{
	BYTE arr[59];
};
//#define MAX_PLAYERS 8
enum {
   mai32enance_building,
   mai32enance_freighter,
   mai32enance_ship,
   mai32enance_spy,
   mai32enance_tribute,
   mai32enance_leader,
   n_mai32enance_classes
};
struct MOX__Settler_Info {
      unsigned source_colony:8; // colony id or MAX_COLONIES if already in space (cannot be redirected)
      unsigned dest_planet:8;   // planet id
      unsigned player:4;        // player id or POP_ANDROID or POP_NATIVE
      unsigned eta:4;           // if settler, takes this many turns to arrive
      unsigned job:2;
};

#define  MAX_TECH_FIELDS                83
#define  MAX_TECH_APPLICATIONS         212
#define PLAYER_RECORD_SIZE 3753
struct MOX__player_t
{
	union{
	struct {
		i16 arr[1876];
		i8 unk;
	};
	i8 i8arr[3753];

	struct{
		i8  picture;
   i8  name[20];
   i8  race_name[15];
   i8  eliminated;
   i8  race;
   i8  color;
   i8  personality;
   i8  objectives;
   i16 home_planet_id;


   i16 network_player_id;         //bookeeping
   i8  player_done_flags;             //bookeeping


   ui16  dead_field;
   i8  research_breakthrough;         // did they complete a field this turn?


   i8  tax_rate;                      // taxes are 0,10,20,30,40,50% of industry production
   i32   bc;                            // our current amount of cash


   i16  n_freighters;
   i16  surplus_freighters;       //bookeeping


   i16 command_poi32s;
   i16 command_poi32s_used;


   // sum of following 3 should be <= n_freighters * FREIGHTER_CAPACITY
   i16  food_freighted;
   i16  settlers_freighted;       // # settlers enroute, stored in settlers array:
   MOX__Settler_Info settlers[25]; // slots < settlers_freighted are current settlers
   i16  total_pop;                  // of all our colonies

   // total amounts produced by all colonies this turn:
   i16  food_produced;
   i16  industry_produced;
   i16  research_produced;
   i16  bc_produced;

   // following are >0 if we had extra, <0 if we were short, this turn:
   i16  surplus_food; // only accounting for unblockaded colonies
   i16  surplus_bc;

   // mai32enance bc costs:
   i32 total_mai32enance;
   i16 mai32enance[n_mai32enance_classes]; // sum == total_mai32enance
   i8  tech_fields[MAX_TECH_FIELDS];              // 0-UNRESEARCHABLE, 1-NOT_KNOWN, 2-ON_RESEARCH_LIST, 3-KNOWN
   i8  tech_applications[MAX_TECH_APPLICATIONS];  // 0-UNRESEARCHABLE, 1-NOT_KNOWN, 2-ON_RESEARCH_LIST, 3-KNOWN


   i32   research_accumulated; //Correct
   i8  captured_orion;
   i8  captured_antares;
   i8  won_council_vote;
   i32   player_defeated[MAX_PLAYERS]; //stardate or 0
   ui16 captured_population;
   i8  last_attacker;


   i8  ship_design_theme;      //AI FIELDS
   i8  ship_special_theme;     //AI FIELDS
   i8  tech_apps_learned[4];   //bookeeping
   //i8  tech_modifiers[17];     //not in use


   i8 hyper_advanced_tech[8];  // LEVEL of hyper advanced tech achieved.


   i32 total_known_tech_cost;    // for history display


   //i8 knows_racial_stats;      //not used


   i32 last_tech_report_date[MAX_PLAYERS];  //not used


   // bitfields, tell whether given player knows the app:
   //ui8 player_tech_applications[MAX_PLAYERS][(MAX_TECH_APPLICATIONS + 7) / 8];
   ui8 player_tech_applications[MAX_PLAYERS][27];




   i8  current_research_field;        // should be > 0, indicating which field
   i8  current_research_application;  //
   i8  got_new_app;                   // within the last turn; causes AI to redecide field to research


   i32   ship_range;


   struct MOX__ship_design_t ship_designs[6]; //Correct
   i16  ships_built[6];


   i8  contact[MAX_PLAYERS];
   i8 just_established_contact;       // bitvector by player index
   i8 n_times_established_contact[MAX_PLAYERS];
   i8  within_range[MAX_PLAYERS];     // tells if this player can reach that player
                                        // (contact is symmetric, within_range is not)


   i8 do_not_research;        // should the AI quit researching from now on?


   i8 offensive_stagepoi32;  //AI FIELD


   ui8 best_ftl_drive;
   ui8 ship_speed;


   i8 officer_for_hire;       // -1 or id of available officer this turn


   i16 n_trade_pops;
   i16 current_trade_agreement_level[MAX_PLAYERS];
   i16 trade_agreement_goal[MAX_PLAYERS];


   i16 n_research_pops;
   i16 current_research_agreement_level[MAX_PLAYERS];
   i16 research_agreement_goal[MAX_PLAYERS];


   i8 fighter_beam_type;
   i8 fighter_bomb_type;




   i16  total_research;                           //AI Field
   i16  total_ships;                              //AI Field
   ui64 total_ship_strength[MAX_PLAYERS];  //AI Field
   i16  total_colonies;                           //AI Field
   i8  failed_to_expand;                              //AI Field




//diplomacy stuff
   i8  first_contact[MAX_PLAYERS];
   i8  relations[MAX_PLAYERS];
   i8  base_relations[MAX_PLAYERS];


   i8  treaty[MAX_PLAYERS];
   i8  trade_treaty[MAX_PLAYERS];
   i8  research_treaty[MAX_PLAYERS];
   i16  tribute_treaty[MAX_PLAYERS];


   i8  diplomacy_incident[MAX_PLAYERS];
   i8  diplomacy_message[MAX_PLAYERS];
   i16  diplomacy_severity[MAX_PLAYERS];
   i16  diplomacy_value[MAX_PLAYERS];
   i16  diplomacy_system[MAX_PLAYERS];


   i16  treaty_modifier[MAX_PLAYERS];
   i16  trade_modifier[MAX_PLAYERS];
   i16  tech_exchange_modifier[MAX_PLAYERS];
   i16  peace_modifier[MAX_PLAYERS];


   i8  last_bad_diplomatic_incident[MAX_PLAYERS];
   i8  broken_treaties_modifier[MAX_PLAYERS];
   i8  last_broken_treaty[MAX_PLAYERS];
   i8  last_gift[MAX_PLAYERS];


   i16  ship_war_losses[MAX_PLAYERS];          // 1) small, 2) medium, 3) large, etc.
   i16  colony_war_losses[MAX_PLAYERS];        // 1/killed or captured colonist + 10/captured or destroyed colony
   i8  biological_weapons_flag[MAX_PLAYERS];       // T/F killed a colonist on one of your colonies
   i8  time_since_last_attack[MAX_PLAYERS];        // when I attack X, set [X] to 0


   i8  threats[MAX_PLAYERS];
   i8  dishonored_flag[MAX_PLAYERS];
   i8  peace_duration[MAX_PLAYERS];
   i16  last_turn_diplomacy_severity[MAX_PLAYERS];
   i8  last_broken_treaty_type[MAX_PLAYERS];


   i8  last_turn_treaty[MAX_PLAYERS];


   i8  diplomacy_proposal_request[MAX_PLAYERS];          // 0) none, 1) First Offer, 2) Second Offer, 3) First Offer, Better 2nd Offer
   i8  diplomacy_proposal_tribute_bribe[MAX_PLAYERS];    // 0) none, 1) 5%/year,     2) 10%/year      10% tribute for better offer
   i8  diplomacy_proposal_tech_bribe1[MAX_PLAYERS];      //
   i8  diplomacy_proposal_tech_bribe2[MAX_PLAYERS];      // better offer technology or
   i16  diplomacy_proposal_gold_bribe[MAX_PLAYERS];       // +50% for better offer
   i8  diplomacy_proposal_war_player[MAX_PLAYERS];
   i32   diplomacy_proposal_exchange_max_value[MAX_PLAYERS]; // tech exchange max value
   i8  diplomacy_proposal_exchange_tech[MAX_PLAYERS];      // tech exchange max value
   i16  diplomacy_proposal_system_bribe[MAX_PLAYERS];


   i16  sneak_attack_planet;
   i8  sneak_attack_message;
   i8  sneak_attack_player; // planet's ownership could have changed before we arrive


   i8  council_threat;
   i16  reward_amount[MAX_PLAYERS];
   i8  reward_tech[MAX_PLAYERS];
   i8  reward_attack_player[MAX_PLAYERS];




   i16  food_shortage_duration;


   i8  trust_worthiness[MAX_PLAYERS];
   i8  trust_breaker_player[MAX_PLAYERS];
   i8  trust_breaker_treaty[MAX_PLAYERS];


   i8  stop_spying_duration[MAX_PLAYERS];
   i8  stop_blockading_duration[MAX_PLAYERS];
   i8  war_buildup_duration; // nonzero means prepare for starting war in future


   i16  last_diplomacy_proposal_turn[MAX_PLAYERS];
   i16  diplomacy_proposal_rejection[MAX_PLAYERS];


   i16  opportunity_planet_num[MAX_PLAYERS];
   i16  opportunity_planet_value[MAX_PLAYERS];
   i32  opportunity_actual_attacker_ratio[MAX_PLAYERS];




   //these are padding...
   i16  temp_diplomacy_dummy1[MAX_PLAYERS];


   i8  diplomacy_demand_rejection_message[MAX_PLAYERS];


   i8  last_diplomacy_threat_turn[MAX_PLAYERS];


   i8 delayed_diplomacy_orders[MAX_PLAYERS]; // 10/9 by Russ
         // can be NONE, or DELAYED_DECLARE_WAR, DELAYED_MAKE_PEACE, DELAYED_BREAK_ALLIANCE


//
   i8 current_government;
   i8 pop_growth_bonus;
   i8 food_bonus;
   i8 production_bonus;
   i8 research_bonus;
   i8 tax_bonus;
   i8 ship_defense;
   i8 ship_attack;
   i8 ground_combat_bonus;
   i8 spying_bonus;


   i8  low_g_world;
   i8  heavy_g_world;
   i8  aquatic;
   i8  subterranean;
   i8  large_home_world;
   i8  rich_home_world;
   i8  ancient_home_world;
   i8  cybernetic;
   i8  eats_minerals;
   i8  repulsive;
   i8  i8ismatic;
   i8  uncreative;
   i8  creative;
   i8  environment_immune;
   i8  fantastic_traders;
   i8  telepathic;
   i8  lucky;
   i8  omniscience;
   i8  stealthy_ships;
   i8  trans_dimensional;
   i8  warlord;


   // when they get evolution tech, players will i32eract with race screen
   // during start-o-turn reports, then during the next turn calc the
   // selected changes will be applied:  Meanwhile, they are briefly stored here:
   i8   evolutionary_upgrade[31];
   i8   picked_evolutionary_upgrade;  // false until they get evolution tech and pick new racial stats
   i8   applied_evolutionary_upgrade; // false until they get evolution tech and new racial stats are actually applied


// Elemental history arrays:
   ui8  fleet_history[350];
   ui8  tech_history[350];
   ui8  population_history[350];
   ui8  production_history[350];


   ui8  spies[MAX_PLAYERS];
   i8  history_btns;         //1-bit flags for history graph buttons
   i8  ignoring;             //Set a bit to ignore a player


   i8  peace_flags[MAX_PLAYERS];
   ui8 cheated;                       //bitfield
   ui32  stardate_last_offered_hero;
   i8  current_zoom_level;            //
   i32  cur_map_x;                     // for hotseat games to restore mainscrn map
   i32  cur_map_y;                     //
   i8 council_voted_for;
   i8 surrender_to;                  // normally -1, else a player id
   i16 time_since_last_lucky_event;
   i8  has_galactic_lore;
   ui8 warned_about_cheating_player;  //BITFIELD based on player num
   ui8 cheated_this_turn;             //0 == didn;t cheat this turn
   i32 tribute_income;
   //i8 filler[39]; 

	};
	};
};

#define SHIP_RECORD_SIZE 129
/*struct MOX__ship_t
{
	BYTE arr[129];
};*/
#define MOVEABLE_BOX_RECORD_SIZE 28
struct MOX__moveable_box_t //28 bytes
{
	i16 arr[14];
};
//#define MAX_MOVEABLE_BOXS 5
//http://www.spheriumnorth.com/orion-forum/nfphpbb/viewtopic.php?f=11&t=45
//#define MAX_NEBULAS 4
struct MOX__nebula_t //20bytes
{
	//BYTE arr[20];
	i16 nebula_x;
	i16 nebula_y;
	i8 nebula_type; // 0 to 11
};
struct MOX__bill_savegame_t //12 bytes
{	
	ui16 arr[6];
};
struct REPORT__reports_t //950bytes
{
	i16 arr[475];
};
struct REPORT__something_interesting_happened_t //8
{
	i16 arr[4];
};
struct REPORT__done_colonize_check_t //9
{
	i8 arr[9];
};
struct EVENTS__event_data_t
{
	i8 arr[9];
};
struct MSG__msgs_t
{
	BYTE arr[18];
};
struct MOX__antarans_t
{
	BYTE arr[66];
};

struct savegame_t
{	    
	DWORD first; //0-3h 4bytes
	i8 save_game_description[37]; // 37
	DWORD stardate;//29h
	BYTE game_type;//2Dh //number in save%d.gam  //- 0:Single Player, 1:Hotseat, 2:Network, 3:Modem
	MOX__set_t moxset;//2Eh 553
	DWORD random_seed;//257h
	WORD num_colonies;//25Bh
	MOX__colony_t colonies[MAX_COLONIES];
	WORD planet_count;//162E7
	MOX__planet_t planets[MAX_PLANETS];
	WORD num_stars;//17AD1h
	MOX__star_t stars[MAX_STARS];
	MOX__leader_t leaders[MAX_LEADERS];
	WORD num_players;
	MOX__player_t players[MAX_PLAYERS];
	WORD num_ships;
	MOX__ship_t ships[MAX_SHIPS];
	MOX__moveable_box_t moveable_boxes[MAX_MOVEABLE_BOXS];//31B4Ch
	WORD moveable_box_orderes[MAX_MOVEABLE_BOXS];//31BD8h
	WORD player_num;//31BE2h
	WORD max_map_scale;
	WORD max_zoom_count;
	BYTE star_bg_index;
	WORD max_x;//31be9
	WORD max_y;
	WORD fleet_icon_button_count;
	MOX__nebula_t nebulas[MAX_NEBULAS];
	BYTE num_nebulas;//31C03h
	MOX__bill_savegame_t bill_savegame;
	WORD n_reports;//31C10h
	REPORT__reports_t reports;
	BYTE done_player_reports;
	REPORT__something_interesting_happened_t something_interesting_happened;
	REPORT__done_colonize_check_t done_colonize_check;
	EVENTS__event_data_t event_datas[MAX_EVENT_DATAS];
	MSG__msgs_t msgs[MAX_MSGS];
	BYTE first_msg[MAX_PLAYERS];//32826h
	BYTE max_msgs[MAX_PLAYERS];//3282Eh
	BYTE n_msgs[MAX_PLAYERS];//32836h
	BYTE displayed_msgs;//3283Eh
	BYTE displayed_gnn;
	WORD council_flag;
	WORD next_council_meeting_turn;
	WORD antaran_resource_level;
	WORD chance_for_antaran_invasion;
	WORD disallow_saves_code;
	DWORD last_event_year;
	WORD event_delay_count;
	MOX__antarans_t antarans;//32850h
	WORD cur_map_scale;
	WORD cur_map_x;
	WORD cur_map_y;
	union{
	BYTE arr[1000]; //unused???
	struct {
		ui16 extension;
		ui32 extension_version;
	};
	};
};

struct savegame_modified_t
{	    
	/*DWORD first; //0-3h 4bytes
	i8 save_game_description[37]; // 37
	DWORD stardate;//29h
	BYTE game_type;//2Dh //number in save%d.gam
	WORD moxset_size;
	MOX__set_t moxset;//2Eh 553
	DWORD random_seed;//257h
	WORD num_colonies;//25Bh
	MOX__colony_t* colonies;//[MAX_COLONIES];
	WORD num_planets;//
	MOX__planet_t* planets;//[MAX_PLANETS];
	WORD num_stars;//
	MOX__star_t stars;//[MAX_STARS];
	WORD num_leaders;//
	MOX__leader_t* leaders;//[MAX_LEADERS];
	WORD num_players;
	MOX__player_t* players;//[MAX_PLAYERS];
	*/
	WORD num_ships;
	MOX__ship_t* ships;//[MAX_SHIPS];
	/*WORD num_moveable_boxes;
	MOX__moveable_box_t* moveable_boxes;//[MAX_MOVEABLE_BOXS];//
	WORD* moveable_box_orderes;//[MAX_MOVEABLE_BOXS];//
	WORD player_num;//
	WORD max_map_scale;
	WORD max_zoom_count;
	BYTE star_bg_index;
	WORD max_x;//31be9
	WORD max_y;
	WORD fleet_icon_button_count;
	BYTE num_nebulas;//
	MOX__nebula_t *nebulas;//[MAX_NEBULAS];	
	MOX__bill_savegame_t bill_savegame;
	WORD n_reports;//31C10h
	REPORT__reports_t reports;
	BYTE done_player_reports;
	REPORT__something_i32eresting_happened_t something_i32eresting_happened;
	REPORT__done_colonize_check_t done_colonize_check;
	WORD num_event_datas;
	EVENTS__event_data_t *event_datas;//[MAX_EVENT_DATAS];
	WORD num_msgs;
	MSG__msgs_t msgs;//[MAX_MSGS];
	BYTE* first_msg;//[MAX_PLAYERS];//
	BYTE* max_msgs;//[MAX_PLAYERS];//
	BYTE* n_msgs;//[MAX_PLAYERS];//
	BYTE displayed_msgs;//
	BYTE displayed_gnn;
	WORD council_flag;
	WORD next_council_meeting_turn;
	WORD antaran_resource_level;
	WORD chance_for_antaran_invasion;
	WORD disallow_saves_code;
	DWORD last_event_year;
	WORD event_delay_count;
	MOX__antarans_t antarans;//
	WORD cur_map_scale;
	WORD cur_map_x;
	WORD cur_map_y;
	//BYTE arr[1000]; //unused???
	*/
};
//




//
struct PicFrameHeader
{
	union{
		struct{
	ui16 m_width;//00-01h
	ui16 m_height;//02-03h
	ui16 m_curframe;//04-05h
	ui16 m_nframes;//06-07h
	ui8 m_8;
	ui8 m_nmilliseconds;//08-09h
	ui8 m_nbits1;//10 unused?
	ui8 m_nbits;//11
	ui32 m_begoffset[1];//12-15
	ui32 m_endoffset[1];//16-16	
		};
	ui8 i8arr[16];
	};
};



typedef struct {
	DPID idFrom;//0
	DPID idTo;//1
	ui32 size;//2
	LPDWORD data;//3
} netdata_header_t;
