﻿#pragma once
#include "stdafx.h"
#include "vec.h"

#define ror

//auto padding
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define is_valid_ptr(p) ((uintptr_t)(p) <= 0x7FFFFFFEFFFF && (uintptr_t)(p) >= 0x1000) 
#define is_bad_ptr(p)	(!is_valid_ptr(p))


namespace menu_setting
{
	extern int Tab;
}
namespace sdk
{
	enum BONE_INDEX : unsigned long
	{

		BONE_POS_HELMET = 8,

		BONE_POS_HEAD = 7,
		BONE_POS_NECK = 6,
		BONE_POS_CHEST = 5,
		BONE_POS_MID = 4,
		BONE_POS_TUMMY = 3,
		BONE_POS_PELVIS = 2,

		BONE_POS_RIGHT_FOOT_1 = 21,
		BONE_POS_RIGHT_FOOT_2 = 22,
		BONE_POS_RIGHT_FOOT_3 = 23,
		BONE_POS_RIGHT_FOOT_4 = 24,

		BONE_POS_LEFT_FOOT_1 = 17,
		BONE_POS_LEFT_FOOT_2 = 18,
		BONE_POS_LEFT_FOOT_3 = 19,
		BONE_POS_LEFT_FOOT_4 = 20,

		BONE_POS_LEFT_HAND_1 = 13,
		BONE_POS_LEFT_HAND_2 = 14,
		BONE_POS_LEFT_HAND_3 = 15,
		BONE_POS_LEFT_HAND_4 = 16,

		BONE_POS_RIGHT_HAND_1 = 9,
		BONE_POS_RIGHT_HAND_2 = 10,
		BONE_POS_RIGHT_HAND_3 = 11,
		BONE_POS_RIGHT_HAND_4 = 12
	};

	enum STANCE : int
	{
		STAND = 0,
		CROUNCH = 1,
		PRONE = 2,
		KNOCKED = 3
	};

	class name_t {
	public:
		uint32_t idx;
		char name[0x24];
		uint8_t unk1[0x64];
		uint32_t health;
	};

	class refdef_t {
	public:
		char pad_0000[8]; //0x0000
		__int32 Width; //0x0008
		__int32 Height; //0x000C
		float FovX; //0x0010
		float FovY; //0x0014
		float Unk; //0x0018
		char pad_001C[8]; //0x001C
		vec3_t ViewAxis[3]; //0x0024
	};

	class Result
	{
	public:
		bool hasResult;
		float a;
		float b;
	};

	struct velocityInfo_t
	{
		vec3_t lastPos;
		vec3_t delta;
	};

	class player_t
	{
	public:
		player_t(uintptr_t address) {
			this->address = address;
		}
		uintptr_t address{};

		uint32_t get_index();

		bool is_valid();

		bool is_visible();

		bool is_dead();

		int team_id();

		int get_stance();

		vec3_t get_pos();

		float get_rotation();
		int weapon_index();
		
	};

	struct loot {
		auto get_name() -> char*;

		auto is_valid() -> bool;

		auto get_position()->vec3_t;
	};

	struct loot_definition_t
	{
		char name[50];
		char text[50];
		bool* show = NULL;
	};

	class WeaponDef
	{
		char	pad0000[0x404];
		int		iProjectileSpeed;
	};

	class WeaponCompleteDef
	{
	public:
		char* szInternalName;		//0x0000
		WeaponDef* weapDef;			//0x0008
		char* szDisplayName;		//0x0010
		char* hideTags;			//0x0018
		char		pad_0020[552];		//0x0020
		char* szAltWeaponName;	//0x0248
		char		pad_0250[32];	    //0x0250
	}; //Size: 0x0270

	vec3_t get_camera_location();

	bool WorldToScreen(const vec3_t& WorldPos, vec2_t* ScreenPos);

	float units_to_m(float units);

	void rotation_point_alpha(float x, float y, float z, float alpha, vec3_t* outVec3);

	bool w2s(const vec3_t& WorldPos, vec2_t* ScreenPos, vec2_t* BoxSize);

	bool head_to_screen(vec3_t pos, vec2_t* pos_out, int stance);

	bool bones_to_screen(vec3_t* BonePosArray, vec2_t* ScreenPosArray, const long Count);

	bool is_valid_bone(vec3_t origin, vec3_t* bone, const long Count);

	bool in_game();

	int get_max_player_count();

	int local_index();

	name_t* get_name_ptr(int i);

	refdef_t* get_refdef();

	vec3_t get_camera_pos();

	player_t get_player(int i);

	player_t get_local_player();
	
	std::string get_player_name(int i);

	bool get_bone_by_player_index(int i, int bone_index, vec3_t* Out_bone_pos);

	void no_recoil();

	void enable_uav();

	int get_player_health(int i);

	void start_tick();

	void update_vel_map(int index, vec3_t vPos);

	void clear_vel_map();

	vec3_t get_speed(int index);

	vec3_t get_prediction(int index, vec3_t source, vec3_t destination);
	vec3_t get_prediction(int index, vec3_t source, vec3_t bone, vec3_t position);
	vec3_t prediction_solver(vec3_t local_pos, vec3_t position, int index, float bullet_speed);

	uintptr_t get_visible_base();

	auto get_loot_by_id(const uint32_t id)->loot*;

	int get_client_count();

	void CBuff(std::string newcmd);
}

namespace decryption 
{
	uint64_t get_client_info();

	uint64_t get_client_info_base();

	uint64_t get_bone_ptr();

	uint16_t get_bone_index(uint32_t index);
}

namespace g_data
{
	extern HWND hWind;
	extern uintptr_t base;
	extern uintptr_t peb;
	extern uintptr_t visible_base;
	extern sdk::refdef_t* refdef;
	extern sdk::refdef_t refdef2;
	extern FARPROC Targetbitblt;
	extern FARPROC TargetStretchbitblt;
	extern FARPROC Target_NtGdiBitBlt;
	extern HMODULE dx9rot;
	void init();
}

namespace g_draw 
{
	void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness);
	void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness);
	void draw_corned_box(const vec2_t& rect, const vec2_t& size, uint32_t color, float thickness);
	void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color);
	void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness);
	void draw_sketch_edge_text(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor = 0xFF000000);
	void draw_crosshair();
	void draw_fov(const float aimbot_fov);
	void draw_bones(unsigned long i, vec3_t origem, DWORD color);
	void draw_health(int i_health, vec3_t pos);
}

namespace globals
{
	extern bool b_in_game;
	extern bool local_is_alive;
	extern bool is_aiming;
	extern int max_player_count;
	/*extern const char* aim_lock_point[];
	extern const char* aim_lock_key[];
	extern const const char* aim_priority_opt[];
	extern const const char* unlock_opt[];*/
	extern uintptr_t refdef_ptr;
	extern uintptr_t local_ptr;
	extern uintptr_t player_ptr;
	extern vec3_t local_pos;
	extern uintptr_t clientinfo;
	extern uintptr_t clientinfobase;
}

namespace colors
{
	extern ImColor Color;
	extern ImColor VisibleColorTeam;
	extern ImColor NotVisibleColorTeam;
	extern ImColor VisibleColorEnemy;
	extern ImColor NotVisibleColorEnemy;
	extern ImColor radar_bg_color;
	extern ImColor radar_boarder_color;
}
namespace screenshot
{
	extern bool visuals;
	extern bool* pDrawEnabled;
	extern int screenshot_counter;
	extern uint32_t bit_blt_log;
	extern const char* bit_blt_fail;
	extern uintptr_t bit_blt_anotherlog;
	extern uint32_t	GdiStretchBlt_log;
	extern const char* GdiStretchBlt_fail;
	extern uintptr_t GdiStretchBlt_anotherlog;
	extern uintptr_t texture_copy_log;
	extern uintptr_t virtualqueryaddr;
}

namespace g_radar {

	void draw_entity(sdk::player_t local_entity, sdk::player_t entity, bool IsFriendly, bool IsAlive, DWORD color,  ImVec2 window_pos, int radar_radius1);
	void show_radar_background();
}