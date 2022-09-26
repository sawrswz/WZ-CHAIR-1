#include "settings.h"
#include "xor.hpp"
#include "Menu.h"

namespace Settings 
{
	bool thirdperson = false;
	namespace menu {
		bool b_auto_show = false;
	}
	namespace esp {
		bool b_auav = false;
		bool b_visible = false;
		bool b_visible_only = false;
		bool b_box = false;
		bool b_line = false;
		bool b_skeleton = false;
		bool b_names = false;
		bool b_distance = false;
		bool b_fov = false;
		bool b_crosshair = false;
		bool b_friendly = false;
		bool b_radar = false;
		bool b_health = false;
		bool b_aim_point = false;
		int fov_size = 75; // 5 ~ 800
		int max_distance = 350; //5 ~ 1000
		int radar_zoom = 221;
		int box_index = 0;
		int line_choose = 0;
		float thickness = 1.5f;
	}
	namespace lootEsp {
		bool b_cash = true;
		bool b_weapons = true;
		bool b_crates = true;
		bool b_crates2 = false;
		bool b_crates3 = true;
		bool b_killstreak = true;
		bool b_missions = false;
		bool b_ammo = true;
		bool b_name = true;
		bool b_distance = true;
		int max_dist = 500;
	}
	namespace aimbot {
		bool b_smooth = false;
		bool b_lock = true;
		bool nospread = false;
		bool b_recoil = false;
		bool b_skip_knocked = false;
		bool b_prediction = true;
		bool b_target_bone = true;
		float f_bullet_speed = 2250.5f; // 1 ~ 5000
		float f_min_closest = 7.f; // 5 ~ 50
		float f_speed = 2.5f; // 1 ~ 30
		float f_smooth = 2.5f; //1 ~ 30
		int i_bone = 1; //0 ~ 3
		int i_priority = 2; // 0 ~ 2
	}
	namespace misc {
		bool b_fov_scale = false;
		float f_fov_scale = 1.2f; // 1.2 ~ 2.0
		bool b_snow = true;
		int font_index = 7;
	}

	namespace colors
	{
		ImColor White = { 1.f,1.f,1.f,1.f };
		ImColor Black = { 0.f,0.f,0.f,1.f };
		ImColor Red = { 1.f,0.f,0.f,1.f };
		ImColor Green = { 0.f,1.f,0.f,1.f };
		ImColor Blue = { 0.f,0.f,1.f,1.f };
		ImColor visible_team = { 30, 215, 157, 255 };
		ImColor not_visible_team = { 255, 166, 0, 255 };
		ImColor visible_enemy = { 30, 215, 157, 255 };
		ImColor not_visible_enemy = { 255, 166, 0, 255 };
		ImColor radar_bg = { 1.f,1.f,1.f,0.f };
		ImColor radar_boarder = { 30, 215, 157, 255 };
		ImColor crate_color = { 255, 255, 255 };
		ImColor crate_color2 = { 255, 255, 255 };
		ImColor crate_color3 = { 255, 255, 255 };
		ImColor cash_color = { 255, 255, 255 };
		ImColor weapon_color = { 255, 255, 255 };
		ImColor mission_color = { 255, 255, 255 };
		ImColor killstreak_color = { 255, 255, 255 };
		ImColor ammo_color = { 255, 255, 255 };
	}

	BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%d", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}
	BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%f", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}
	float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
	{
		char szData[32];

		GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

		return (float)atof(szData);
	}

	void Save_Settings(std::string fileName) 
	{
		char file_path[MAX_PATH];
		sprintf_s(file_path, xorstr("C:\\Sprite\\config\\%s%s"), fileName.c_str(), xorstr(".ini"));
		WritePrivateProfileInt("esp", "thickness", esp::thickness, file_path);
		WritePrivateProfileInt("esp", "line_choose", esp::line_choose, file_path);
		WritePrivateProfileInt("esp", "box_index", esp::box_index, file_path);
		WritePrivateProfileInt("esp", "b_uav", esp::b_auav, file_path);
		WritePrivateProfileInt("esp", "b_box", esp::b_box, file_path);
		WritePrivateProfileInt("esp", "visible only", esp::b_visible_only, file_path);
		WritePrivateProfileInt("esp", "b_crosshair", esp::b_crosshair, file_path);
		WritePrivateProfileInt("esp", "b_distance", esp::b_distance, file_path);
		WritePrivateProfileInt("esp", "b_fov", esp::b_fov, file_path);
		WritePrivateProfileInt("esp", "b_friendly", esp::b_friendly, file_path);
		WritePrivateProfileInt("esp", "b_health", esp::b_health, file_path);
		WritePrivateProfileInt("esp", "b_line", esp::b_line, file_path);
		WritePrivateProfileInt("esp", "b_names", esp::b_names, file_path);
		WritePrivateProfileInt("esp", "b_radar", esp::b_radar, file_path);
		WritePrivateProfileInt("esp", "b_skeleton", esp::b_skeleton, file_path);
		WritePrivateProfileInt("esp", "b_visible", esp::b_visible, file_path);
		WritePrivateProfileInt("esp", "i_fov_size", esp::fov_size, file_path);
		WritePrivateProfileInt("esp", "i_max_distance", esp::max_distance, file_path);
		WritePrivateProfileInt("esp", "i_radar_zoom", esp::radar_zoom, file_path);
		WritePrivateProfileInt("aimbot", "b_smooth", aimbot::b_smooth, file_path);
		WritePrivateProfileInt("aimbot", "b_lock", aimbot::b_lock, file_path);
		WritePrivateProfileInt("aimbot", "b_prediction", aimbot::b_prediction, file_path);
		WritePrivateProfileInt("aimbot", "b_recoil", aimbot::b_recoil, file_path);
		WritePrivateProfileInt("aimbot", "b_skip_knocked", aimbot::b_skip_knocked, file_path);
		WritePrivateProfileInt("aimbot", "b_target_bone", aimbot::b_target_bone, file_path);
		WritePrivateProfileFloat("aimbot", "f_bullet_speed", aimbot::f_bullet_speed, file_path);
		WritePrivateProfileFloat("aimbot", "f_min_closest", aimbot::f_min_closest, file_path);
		WritePrivateProfileFloat("aimbot", "f_smooth", aimbot::f_smooth, file_path);
		WritePrivateProfileFloat("aimbot", "f_speed", aimbot::f_speed, file_path);
		WritePrivateProfileInt("aimbot", "i_bone", aimbot::i_bone, file_path);
		WritePrivateProfileInt("aimbot", "i_priority", aimbot::i_priority, file_path);
		WritePrivateProfileInt("misc", "b_fov_scale", misc::b_fov_scale, file_path);
		WritePrivateProfileInt("misc", "font_index", misc::font_index, file_path);
		WritePrivateProfileFloat("misc", "f_fov_scale", misc::f_fov_scale, file_path);
		WritePrivateProfileInt("misc", "snow ", misc::b_snow, file_path);
		WritePrivateProfileInt("colors", "not_visible_enemy", colors::not_visible_enemy, file_path);
		WritePrivateProfileInt("colors", "not_visible_team", colors::not_visible_team, file_path);
		WritePrivateProfileInt("colors", "visible_enemy", colors::visible_enemy, file_path);
		WritePrivateProfileInt("colors", "visible_team", colors::visible_team, file_path);
		WritePrivateProfileInt("colors", "radar_bg", colors::radar_bg, file_path);
		WritePrivateProfileInt("colors", "radar_boarder", colors::radar_boarder, file_path);
	}

	void Load_Settings(std::string fileName)
	{
		char file_path[MAX_PATH];
		sprintf_s(file_path, xorstr("C:\\Sprite\\config\\%s%s"), fileName.c_str(), xorstr(".ini"));
		esp::thickness = GetPrivateProfileIntA("esp", "thickness", esp::thickness, file_path);
		misc::font_index = GetPrivateProfileIntA("misc", "font_index", misc::font_index, file_path);
		esp::line_choose = GetPrivateProfileIntA("esp", "line_choose", esp::line_choose, file_path);
		esp::box_index = GetPrivateProfileIntA("esp", "box_index", esp::box_index, file_path);
		esp::b_auav = GetPrivateProfileIntA("esp", "b_uav", esp::b_auav, file_path);
		esp::b_visible_only = GetPrivateProfileIntA("esp", "visible only", esp::b_visible_only, file_path);
		esp::b_box = GetPrivateProfileIntA("esp", "b_box", esp::b_box, file_path);
		esp::b_crosshair = GetPrivateProfileIntA("esp", "b_crosshair", esp::b_crosshair, file_path);
		esp::b_distance = GetPrivateProfileIntA("esp", "b_distance", esp::b_distance, file_path);
		esp::b_fov = GetPrivateProfileIntA("esp", "b_fov", esp::b_fov, file_path);
		esp::b_friendly = GetPrivateProfileIntA("esp", "b_friendly", esp::b_friendly, file_path);
		esp::b_health = GetPrivateProfileIntA("esp", "b_health", esp::b_health, file_path);
		esp::b_line = GetPrivateProfileIntA("esp", "b_line", esp::b_line, file_path);
		esp::b_names = GetPrivateProfileIntA("esp", "b_names", esp::b_names, file_path);
		esp::b_radar = GetPrivateProfileIntA("esp", "b_radar", esp::b_radar, file_path);
		esp::b_skeleton = GetPrivateProfileIntA("esp", "b_skeleton", esp::b_skeleton, file_path);
		esp::b_visible = GetPrivateProfileIntA("esp", "b_visible", esp::b_visible, file_path);
		esp::fov_size = GetPrivateProfileIntA("esp", "i_fov_size", esp::fov_size, file_path);
		esp::max_distance = GetPrivateProfileIntA("esp", "i_max_distance", esp::max_distance, file_path);
		esp::radar_zoom = GetPrivateProfileIntA("esp", "i_radar_zoom", esp::radar_zoom, file_path);
		aimbot::b_smooth = GetPrivateProfileIntA("aimbot", "b_smooth", aimbot::b_smooth, file_path);
		aimbot::b_lock = GetPrivateProfileIntA("aimbot", "b_lock", aimbot::b_lock, file_path);
		aimbot::nospread = GetPrivateProfileIntA("aimbot", "nospread", Settings::aimbot::nospread, file_path);
		aimbot::b_prediction = GetPrivateProfileIntA("aimbot", "b_prediction", aimbot::b_prediction, file_path);
		aimbot::b_recoil = GetPrivateProfileIntA("aimbot", "b_recoil", aimbot::b_recoil, file_path);
		aimbot::b_skip_knocked = GetPrivateProfileIntA("aimbot", "b_skip_knocked", aimbot::b_skip_knocked, file_path);
		aimbot::b_target_bone = GetPrivateProfileIntA("aimbot", "b_target_bone", aimbot::b_target_bone, file_path);
		aimbot::f_bullet_speed = GetPrivateProfileFloat("aimbot", "f_bullet_speed", aimbot::f_bullet_speed, file_path);
		aimbot::f_min_closest = GetPrivateProfileFloat("aimbot", "f_min_closest", aimbot::f_min_closest, file_path);
		aimbot::f_smooth = GetPrivateProfileFloat("aimbot", "f_smooth", aimbot::f_smooth, file_path);
		aimbot::f_speed = GetPrivateProfileFloat("aimbot", "f_speed", aimbot::f_speed, file_path);
		aimbot::i_bone = GetPrivateProfileIntA("aimbot", "i_bone", aimbot::i_bone, file_path);
		aimbot::i_priority = GetPrivateProfileIntA("aimbot", "i_priority", aimbot::i_priority, file_path);
		misc::b_fov_scale = GetPrivateProfileIntA("misc", "b_fov_scale", misc::b_fov_scale, file_path);
		misc::f_fov_scale = GetPrivateProfileFloat("misc", "f_fov_scale", misc::f_fov_scale, file_path);
		misc::b_snow = GetPrivateProfileIntA("misc", "b_fov_scale", misc::b_snow, file_path);
		colors::not_visible_enemy = GetPrivateProfileIntA("colors", "not_visible_enemy", colors::not_visible_enemy, file_path);
		colors::not_visible_team = GetPrivateProfileIntA("colors", "not_visible_team", colors::not_visible_team, file_path);
		colors::visible_enemy = GetPrivateProfileIntA("colors", "visible_enemy", colors::visible_enemy, file_path);
		colors::visible_team = GetPrivateProfileIntA("colors", "visible_team", colors::visible_team, file_path);
		colors::radar_bg = GetPrivateProfileIntA("colors", "radar_bg", colors::radar_bg, file_path);
		colors::radar_boarder = GetPrivateProfileIntA("colors", "radar_boarder", colors::radar_boarder, file_path);
		colors::cash_color = GetPrivateProfileIntA("colors", "cash_color", colors::cash_color, file_path);
		colors::weapon_color = GetPrivateProfileIntA("colors", "weapon_color", colors::weapon_color, file_path);
		colors::crate_color = GetPrivateProfileIntA("colors", "crate_color", colors::crate_color, file_path);
		colors::crate_color3 = GetPrivateProfileIntA("colors", "crate_color3", colors::crate_color3, file_path);
		colors::ammo_color = GetPrivateProfileIntA("colors", "ammo_color", colors::ammo_color, file_path);
		g_menu::str_config_name = fileName;
	};

	std::vector<std::string> GetList() 
	{
		std::vector<std::string> configs;
		WIN32_FIND_DATA ffd;
		LPCSTR directory = xorstr("C:\\Sprite\\config\\*");
		auto hFind = FindFirstFile(directory, &ffd);
		while (FindNextFile(hFind, &ffd))
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string file_name = ffd.cFileName;
				if (file_name.size() < 4) // .cfg
					continue;
				std::string end = file_name;
				end.erase(end.begin(), end.end() - 4);
				if (end != xorstr(".ini"))
					continue;
				file_name.erase(file_name.end() - 4, file_name.end());
				configs.push_back(file_name);
			}
		}
		return configs;
	}

	void Auto_Load() 
	{
		if (!GetList().empty())
		{
			Load_Settings(GetList().at(0));
		}
		else {
			CreateDirectoryA(xorstr("C:\\Sprite"), NULL);
			CreateDirectoryA(xorstr("C:\\Sprite\\config"), NULL);
		}
	}

}

namespace weapon_info
{
	const char* name;
	const char* alt_name;
	uintptr_t velocity;
	uintptr_t weapon_def;
	float bullet_speed;
}

namespace d3d12test
{
	ID3D12CommandQueue* d3d12CommandQueue2 = nullptr;
	ID3D12CommandQueue* d3d12CommandQueueoriginal = nullptr;
}

namespace MenuColors
{
	bool use_custom_theme = false;
	bool AntiAliasing = false;
	bool AntiAliasingLines = false;
	ImVec4 Color_Text = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
	ImVec4 Text_Disabled = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
	ImVec4 WindowBackround = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	ImVec4 PopupBackround = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	ImVec4 Border_Color = ImColor(30, 215, 157, 255);
	ImVec4 Border_shadow = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	ImVec4 Frame_Backround = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	ImVec4 Frame_Backround_Hovered = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	ImVec4 Frame_Backround_Active = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	ImVec4 Titlebar_Backround = ImColor(30, 215, 157, 255);
	ImVec4 TitleBgCollapsed = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	ImVec4 TitleBgActive = ImColor(30, 215, 157, 255);
	ImVec4 Menubar_Backround = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	ImVec4 Scrollbar_Backround = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
	ImVec4 ScrollBar_Grab = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	ImVec4 Scrollbar_Grab_Hovered = ImColor(30, 215, 157, 255);
	ImVec4 Scrollbar_Grab_Active = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
	ImVec4 Check_Mark = ImColor(30, 215, 157, 255);
	ImVec4 Slider_Grab = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	ImVec4 Slider_grab_Active = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	ImVec4 Button = ImColor(30, 215, 157, 255);
	ImVec4 Button_Hovered = ImColor(30, 215, 157, 255);
	ImVec4 Button_Active = ImColor(30, 215, 157, 255);
	ImVec4 Header = ImColor(30, 215, 157, 255);
	ImVec4 Header_Hovered = ImColor(30, 215, 157, 255);
	ImVec4 Header_Active = ImColor(30, 215, 157, 255);
	ImVec4 PlotLines = ImColor(30, 215, 157, 255);
	ImVec4 PLotlines_Hovered = ImColor(30, 215, 157, 255);
	ImVec4 Plotlines_Active = ImColor(30, 215, 157, 255);
	ImVec4 PlotHistogram = ImColor(30, 215, 157, 255);
	ImVec4 PlotHistogram_Active = ImColor(30, 215, 157, 255);
	ImVec4 TextSelected_Backround = ImColor(30, 215, 157, 255);
	ImVec4 ModalWindowDarkening = ImColor(30, 215, 157, 255);
	float windowbordersize = 0.f;
	ImVec2 windowpadding(15, 15);
	float windowrounding = 5.0f;
	float childrounding;
	ImVec2 framepadding(5, 5);
	float framerounding = 4.0;
	float popuprounding = 3.0;
	ImVec2 itemspacing(12, 8);
	ImVec2 iteminnerspacing(8, 6);
	float indentspacing = 25.f;
	float scrollbarsize = 15.f;
	float scrollbarrounding = 9.0;
	float grabminsize = 5.0;
	float grabrounding = 3.0;
	ImVec2 windowtitleallignx(0.5f, 0.5f);
}