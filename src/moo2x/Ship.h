#pragma once

#pragma pack(push, 1)

#define MAX_SPECIALS 36
#define MAX_SHIP_WEAPONS 8
/*struct s_ship_weapons {
   i16  type;
   i8       count;
   i8       current_count;      // damaged if < count
   i8       firing_arc;         // 1-F, 2-FX, 4-Bx, 8-B, 16-360
   i16  specials;           // bit vector
   i8       ammo;               // valid for missiles only
};*/
#define SHIPWEAPON_RECORD_SIZE 8
struct	MOX__ship_weapon_t
{
	ui16	weapon_type;
	ui8		initial_quantity;
	ui8		quantity;// damaged if < count
	ui8		firing_arc;// 1-F, 2-FX, 4-Bx, 8-B, 16-360
	ui16	weapon_mods;// bit vector
	ui8		ammo_remaining;// valid for missiles only
};
#define SHIPDESIGN_RECORD_SIZE 30+5+SHIPWEAPON_RECORD_SIZE*MAX_SHIP_WEAPONS
struct MOX__ship_design_t
{
   char  name[16];
   i8  size;
   i8  ship_type;
   i8  shield_type;
   i8  ftl_type;
   i8  speed;
   i8  computer_type;
   i8  armor_type;
   ui8  specials[(MAX_SPECIALS+7)/8]; //special_device_flags //bytes
   struct MOX__ship_weapon_t ship_weapon[MAX_SHIP_WEAPONS];
   i8  picture_num;
   i8  builder;//previous_owner;
   i16  cost;
   ui8 combat_speed;
   ui16  date_of_design;
};
struct	MOX__ship_t
{
	/*char			name[16];
	ui8				ship_type_class;
	ui8				ship_type;
	ui8				shield_type;
	ui8				drive_type;
	ui8				field_14;
	ui8				computer_type;
	ui8				armor_type;
	ui8				specials[5];
	MOX__ship_weapon_t	weapons[8];
	ui8				ship_shape;
	ui8				builder;
	ui16			cost_to_build;
	ui8				combat_speed;
	ui16			build_stardate;*/
	MOX__ship_design_t d;
	i8				current_owner;
	ui8				state_of_ship;
	ui16			destination_star;
	ui16			map_x;
	ui16			map_y;
	ui8				navigator_in_fleet_group_flag; // does the moving group have navigator?
	ui8				group_travelling_speed;// possibly less than ftl_type
	ui8				ETA_for_group; // until arrival
	ui8				shield_damage;
	ui8				engine_damage;
	ui8				computer_damage;
	ui8				crew_quality;
	ui16			crew_experience;
	ui16			ship_officer;
	ui8				damage_flags[5];//bit vector Set if Special is damaged
	ui16			armor_damage;
	ui16			structural_damage;
	ui8				ai_mission;   // AI Field
	ui8				just_built_flag;
};


/*struct moo2_planet
{
  __int16 colony_link;
  char star_link;
  char orbit;
  char planet_type;
  char planet_size;
  char planet_g;
  char field_7;
  char planet_environment_class;
  char drawing_number;
  char planet_minerall_class;
  char food_base;
  char number_of_teraforms_done;
  char field_D;
  char field_E;
  char planet_special;
  char flags;
};*/
/*
struct moo2_star
{
  char name[15];
  __int16 map_x;
  __int16 map_y;
  char star_size;
  char system_primary_owner;
  char field_15;
  char star_type;
  char field_17[17];
  char system_special;
  char wormhole_leads_to;
  char field_2A[15];
  char System_has_warp_interdictor;
  char field_3A[5];
  char system_has_artemis_net;
  char has_dimensional_portal;
  char field_41[9];
  __int16 planet_1;
  __int16 planet_2;
  __int16 planet_3;
  __int16 planet_4;
  __int16 planet_5;
  __int16 relocation_star_id[8];
  char field_64[13];
};*/



#pragma	pack(pop)
