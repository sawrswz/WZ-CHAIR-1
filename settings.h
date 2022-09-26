#pragma once
#include "stdafx.h"
//#include "Singleton.h"

#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000

#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)     

namespace Settings
{
	extern bool thirdperson;
	namespace menu
	{
		extern bool b_auto_show;
	}

	namespace esp 
	{
		extern bool b_auav;
		extern bool b_visible;
		extern bool b_box;
		extern bool b_line;
		extern bool b_skeleton;
		extern bool b_names;
		extern bool b_distance;
		extern bool b_fov;
		extern bool b_crosshair;
		extern bool b_friendly;
		extern bool b_radar;
		extern bool b_health;
		extern bool b_aim_point;
		extern int fov_size;
		extern int max_distance;
		extern int radar_zoom;
		extern int box_index;
		extern int line_choose;
		extern bool b_visible_only;
		extern float thickness;
	}
	namespace lootEsp {
		extern bool b_cash;
		extern bool b_weapons;
		extern bool b_crates;
		extern bool b_crates2;
		extern bool b_crates3;
		extern bool b_killstreak;
		extern bool b_missions;
		extern bool b_ammo;
		extern bool b_name;
		extern bool b_distance;
		extern int max_dist;
	}
	namespace aimbot {
		extern bool b_smooth;
		extern bool b_lock;
		extern bool nospread;
		extern bool b_recoil;
		extern bool b_skip_knocked;
		extern bool b_prediction;
		extern bool b_target_bone;
		extern float f_bullet_speed;
		extern float f_min_closest;
		extern float f_speed;
		extern float f_smooth;
		extern int i_bone;
		extern int i_priority;
	}

	namespace misc {
		extern bool b_fov_scale;
		extern bool b_snow;
		extern float f_fov_scale;
		extern int font_index;

	}
	namespace colors
	{
		extern ImColor White;
		extern ImColor Black;
		extern ImColor Red;
		extern ImColor Green;
		extern ImColor Blue;
		extern ImColor visible_team;
		extern ImColor not_visible_team;
		extern ImColor visible_enemy;
		extern ImColor not_visible_enemy;
		extern ImColor radar_bg;
		extern ImColor radar_boarder;
		extern ImColor crate_color;
		extern ImColor crate_color2;
		extern ImColor crate_color3;
		extern ImColor cash_color;
		extern ImColor weapon_color;
		extern ImColor mission_color;
		extern ImColor killstreak_color;
		extern ImColor ammo_color;
	}

	void Save_Settings(std::string path);
	void Load_Settings(std::string path);
	void Auto_Load();
	std::vector<std::string> GetList();
}

namespace weapon_info
{
	extern const char* name;
	extern const char* alt_name;
	extern uintptr_t velocity;
	extern float bullet_speed;
	extern uintptr_t weapon_def;
}

namespace d3d12test
{
	extern ID3D12CommandQueue* d3d12CommandQueue2;
	extern ID3D12CommandQueue* d3d12CommandQueueoriginal;
	extern uintptr_t discord;
}


namespace MenuColors
{
	extern bool use_custom_theme;
	extern bool AntiAliasing;
	extern bool AntiAliasingLines;
	extern ImVec4 Color_Text;
	extern ImVec4 Text_Disabled;
	extern ImVec4 WindowBackround;
	extern ImVec4 PopupBackround;
	extern ImVec4 Border_Color;
	extern ImVec4 Border_shadow;
	extern ImVec4 Frame_Backround;
	extern ImVec4 Frame_Backround_Hovered;
	extern ImVec4 Frame_Backround_Active;
	extern ImVec4 Titlebar_Backround;
	extern ImVec4 TitleBgCollapsed;
	extern ImVec4 TitleBgActive;
	extern ImVec4 Menubar_Backround;
	extern ImVec4 Scrollbar_Backround;
	extern ImVec4 ScrollBar_Grab;
	extern ImVec4 Scrollbar_Grab_Hovered;
	extern ImVec4 Scrollbar_Grab_Active;
	extern ImVec4 Check_Mark;
	extern ImVec4 Slider_Grab;
	extern ImVec4 Slider_grab_Active;
	extern ImVec4 Button;
	extern ImVec4 Button_Hovered;
	extern ImVec4 Button_Active;
	extern ImVec4 Header;
	extern ImVec4 Header_Hovered;
	extern ImVec4 Header_Active;
	extern ImVec4 PlotLines;
	extern ImVec4 PLotlines_Hovered;
	extern ImVec4 Plotlines_Active;
	extern ImVec4 PlotHistogram;
	extern ImVec4 PlotHistogram_Active;
	extern ImVec4 TextSelected_Backround;
	extern ImVec4 ModalWindowDarkening;
	extern float windowbordersize;
	extern ImVec2 windowpadding;
	extern float windowrounding;
	extern float childrounding;
	extern ImVec2 framepadding;
	extern float framerounding;
	extern float popuprounding;
	extern ImVec2 itemspacing;
	extern ImVec2 iteminnerspacing;
	extern float indentspacing;
	extern float scrollbarsize;
	extern float scrollbarrounding;
	extern float grabminsize;
	extern float grabrounding;
	extern ImVec2 windowtitleallignx;
}