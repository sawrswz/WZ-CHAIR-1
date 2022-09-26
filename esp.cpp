#include "stdafx.h"
#include "esp.h"
#include "sdk.h"
#include "aim.h"
#include "settings.h"
#include "Menu.h"
#include "offsets.h"
#include"Menu.h"
#include"input.h"
#include "classes.h"

bool once = false;
using Vector2 = vec2_t;
void weaponFunction(int index);
float valuesSpreadBackup[962][22];
bool noSpread = false;

bool enable_loot = false;
const sdk::loot_definition_t loot_defs[] =
{
	{ "accessory_money", "CASH", &Settings::lootEsp::b_cash },
	{ "loot_wm_armor_plate", "Amor", &enable_loot },
	{ "lm_armor_vest_01", "Satchel", &enable_loot },
	{ "gasmask_a", "Gasmask", &Settings::lootEsp::b_cash },
	{ "offhand_vm_supportbox_armor_br", "Armor Box",&Settings::lootEsp::b_cash},
	{ "offhand_vm_supportbox_br", "Ammo Box", &Settings::lootEsp::b_cash },
	{ "loot_crate_01_br_legendary_01", "CRATE LEGENDARY", &Settings::lootEsp::b_crates },
	{ "military_loot_crate", "CRATE MILITARY", &Settings::lootEsp::b_crates },

	// Tablets
	// Mission
	{ "mission_tablet_01_scavenger", "Mission Scavenger", &Settings::lootEsp::b_missions },
	{ "mission_tablet_01_assassination", "Mission Most_Wanted", &Settings::lootEsp::b_missions },
	/*{ "mission_tablet_01_timedrun", "mission_tablet_01_timedrun", &trr },*/

	// Ammo
	{ "loot_ammo_pistol_smg", "SMG AMMO", &Settings::lootEsp::b_ammo },
	{ "ammo_box_ar", "AR AMMO", &Settings::lootEsp::b_ammo },
	{ "ammo_shotgun", "Shotgun AMMO", &Settings::lootEsp::b_ammo },
	{ "ammo_marksman_sniper", "SNIPER AMMO", &Settings::lootEsp::b_ammo },
	/*{ "ammo_rocket", "ROCKET AMMO", &trr },*/

	//Weapons
	{ "wm_weapon_root_s4_ar", "Assault Rifle", &Settings::lootEsp::b_weapons },
	{ "wm_weapon_root_s4_pi", "Pistol", &Settings::lootEsp::b_weapons },
	{ "wm_weapon_root_s4_sm", "SMG", &Settings::lootEsp::b_weapons },
	{ "wm_weapon_root_s4_sn", "Sniper", &Settings::lootEsp::b_weapons },
	{ "wm_weapon_root_s4_lm", "LMG", &Settings::lootEsp::b_weapons },

	//Equipment
	{ "lm_offhand_wm_grenade_flash", "flash", &enable_loot },
	{ "offhand_vm_grenade_decoy", "decoy", &enable_loot },
	{ "lm_heal_stim_syringe_01", "Stim", &enable_loot },
	{ "offhand_wm_grenade_thermite_br", "Thermite", &enable_loot },
	{ "lm_offhand_wm_grenade_mike67", "Grenade", &enable_loot },
	{ "lm_offhand_wm_grenade_semtex", "Semtex", &enable_loot },
	{ "lm_offhand_wm_grenade_snapshot", "Snapshot", &enable_loot },
	{ "lm_offhand_wm_grenade_concussion", "Concussion", &enable_loot },
	{ "lm_offhand_wm_c4", "C4", &enable_loot },
	{ "weapon_wm_knife_offhand_thrown_electric", "Throwing Knife Electric", &enable_loot },
	{ "m_weapon_wm_knife_offhand_thrown_fire", "Throwing Knife Fire", &enable_loot  },
	{ "lm_offhand_wm_at_mine", "Mine", &enable_loot },
	{ "lm_weapon_wm_knife_offhand_thrown", "Throwing Knife", &enable_loot  },
	{ "lm_offhand_wm_claymore", "Claymore", &enable_loot },

	//Unknown
	/*{"lm_offhand_wm_tac_cover", "tac_cover" },
	{"offhand_wm_jerrycan", "jerrycan" },
	{"x2_buy_station_rig_skeleton", "buystation" },
	{"veh8_mil_air_malfa_small_noscript", "veh8_mil_air" },
	{"offhand_wm_trophy_system_br", "TrophySystem" },
	{"perk_manual_01_dead_silence", "deadsilence" },*/

	//other strike,Uav ect
	{ "wm_killstreak_handset_ch3_airstrike", "Airstrike" },
	{ "killstreak_tablet_01_clusterstrike", "Clusterstrike" },
	{ "killstreak_tablet_01_uav", " UAV" },

};

Vector2 radar_rotate(const float x, const float y, float angle)
{
	angle = (float)(angle * (M_PI / 180.f));
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	Vector2 returnVec;
	returnVec.x = cosTheta * x + sinTheta * y;
	returnVec.y = sinTheta * x - cosTheta * y;
	return returnVec;
}

ImVec2 rotate(const ImVec2& center, const ImVec2& pos, float angle)
{
	ImVec2 Return;
	angle *= -(M_PI / 180.0f);
	float cos_theta = cos(angle);
	float sin_theta = sin(angle);
	Return.x = (cos_theta * (pos[0] - center[0]) - sin_theta * (pos[1] - center[1])) + center[0];
	Return.y = (sin_theta * (pos[0] - center[0]) + cos_theta * (pos[1] - center[1])) + center[1];
	return Return;
}

void imgui_draw_cross()
{
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
	ImVec2 winpos = ImGui::GetWindowPos();
	ImVec2 winsize = ImGui::GetWindowSize();
	draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(70, 70, 70, 255), 1.f);
	draw_list->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(70, 70, 70, 255), 1.f);
	draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);
	draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);
}

int RadarSize;
int RadarRadius1;
ImGuiWindowFlags flag_menu = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
ImGuiWindowFlags flag_no_menu = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;

void RadarGui(sdk::player_t local_entity, sdk::player_t entity, bool IsFriendly, bool is_dead, DWORD color)
{
	if (Settings::esp::b_radar)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		const float bor_size = style.WindowBorderSize;
		style.WindowBorderSize = 0.0f;
		style.WindowBorderSize = bor_size;
		style.WindowPadding = ImVec2(8, 8);
		style.WindowRounding = 10.0f;
		style.ChildRounding = 0.f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 6.0f;
		style.PopupRounding = 3.f;
		style.ItemSpacing = ImVec2(8, 8);
		style.ItemInnerSpacing = ImVec2(8, 6);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 6.0f;
		style.WindowTitleAlign = { 0.5f,0.5f };
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.4f);

		if (g_menu::b_menu_open)
			ImGui::Begin("Radar", 0, flag_menu);
		else
			ImGui::Begin("Radar", 0, flag_no_menu);

		ImGui::SetWindowSize(ImVec2(230, 230));
		RadarSize = 233;
		RadarRadius1 = RadarSize / 2;
		imgui_draw_cross();
		g_radar::draw_entity(local_entity, entity, IsFriendly, is_dead, color, ImGui::GetWindowPos(), RadarRadius1);
		ImGui::End();
	}
}

void draw_health(int i_health, vec3_t pos)
{
	Vector2 bottom, top;

	if (!sdk::WorldToScreen(pos, &bottom))
		return;

	pos.z += 60;
	if (!sdk::WorldToScreen(pos, &top))
		return;

	top.y -= 5;
	auto height = top.y - bottom.y;
	auto width = height / 2.f;
	auto x = top.x - width / 1.8f;
	auto y = top.y;
	auto bar_width = max(0, min(127, i_health)) * (bottom.y - top.y) / 127.f;
	auto health = max(0, min(127, i_health));
	uint32_t color_health = ImGui::ColorConvertFloat4ToU32(ImVec4(0.00f, 1.000f, 0.000f, 1.000f));
	g_draw::fill_rectangle(x, y, 4, 127 * (bottom.y - top.y) / 127.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)));
	g_draw::fill_rectangle(x + 1, y + 1, 2, bar_width - 2, color_health);
}


int get_player_health(int i)
{
	uint64_t bgs = *(uint64_t*)(g_data::base + offsets::name_array);
	if (bgs)
	{
		sdk::name_t* clientInfo_ptr = (sdk::name_t*)(bgs + offsets::name_array_pos + (i * 0xD0));
		int health = clientInfo_ptr->health;
		return health;
	}
	return 0;
}

void updateInput() noexcept
{
	if (g_menu::aimbotKeyMode == 0)
		keyPressed = aimbotkey.isDown();
	if (g_menu::aimbotKeyMode == 1 && aimbotkey.isPressed())
		keyPressed = !keyPressed;
}

namespace main_game
{
	void esp_loop(ImFont* font);

	void ui_header()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
	}

	void ui_end()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
	}

	void init(ImFont* font)
	{
		ui_header();
		main_loop(font);
		ui_end();
	}
	
	void esp_loop(ImFont* font)
	{
	
		ImColor current_color;
		globals::b_in_game = sdk::in_game();
		globals::max_player_count = sdk::get_max_player_count();
		auto cl_info = decryption::get_client_info();
		globals::clientinfo = cl_info;
	
		if (!globals::b_in_game)
		{
			sdk::clear_vel_map();
			once = false;
			return;
		}

		auto ref_def = sdk::get_refdef();
		if (is_bad_ptr(ref_def))
			return;

		g_data::refdef2 = *(sdk::refdef_t*)ref_def;
		g_data::refdef = ref_def;
		globals::refdef_ptr = (uintptr_t)ref_def;
		sdk::player_t local_player = sdk::get_local_player();
		globals::local_ptr = local_player.address;
		if (is_bad_ptr(globals::local_ptr))
			return;
		globals::local_pos = local_player.get_pos();


		if (Settings::esp::b_crosshair)
			g_draw::draw_crosshair();

		if (Settings::esp::b_fov)
			g_draw::draw_fov(Settings::esp::fov_size);

		//if (Settings::esp::b_auav)
		//	sdk::enable_uav();

		if (!globals::is_aiming)
			aim_assist::reset_lock();

		if (Settings::esp::b_visible)
		{
			g_data::visible_base = sdk::get_visible_base();
		}

		if (Settings::aimbot::b_recoil)
			sdk::no_recoil();

		WeaponCompleteDefArr* weapons = (WeaponCompleteDefArr*)(g_data::base + offsets::weapon_definitions);

		if (sdk::in_game())
		{
			if (Settings::aimbot::nospread)
			{
				noSpread = !noSpread;

				if (noSpread)
				{
					for (int count = 0; count < 962; count++)
					{
						if (weapons->weaponCompleteDefArr[count]->weapDef)
						{
							valuesSpreadBackup[count][0] = weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay;
							valuesSpreadBackup[count][1] = weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadProneDecay;
							valuesSpreadBackup[count][2] = weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadSprintDecay;
							valuesSpreadBackup[count][3] = weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadInAirDecay;
							valuesSpreadBackup[count][4] = weapons->weaponCompleteDefArr[count]->weapDef->fHipReticleSidePos;
							valuesSpreadBackup[count][5] = weapons->weaponCompleteDefArr[count]->weapDef->fAdsIdleAmount;
							valuesSpreadBackup[count][6] = weapons->weaponCompleteDefArr[count]->weapDef->fHipIdleAmount;
							valuesSpreadBackup[count][7] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleSpeed;
							valuesSpreadBackup[count][8] = weapons->weaponCompleteDefArr[count]->weapDef->hipIdleSpeed;
							valuesSpreadBackup[count][9] = weapons->weaponCompleteDefArr[count]->weapDef->fIdleCrouchFactor;
							valuesSpreadBackup[count][10] = weapons->weaponCompleteDefArr[count]->weapDef->fIdleProneFactor;
							valuesSpreadBackup[count][11] = weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxPitch;
							valuesSpreadBackup[count][12] = weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw;
							valuesSpreadBackup[count][13] = weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch;
							valuesSpreadBackup[count][14] = weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw;
							valuesSpreadBackup[count][15] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime;
							valuesSpreadBackup[count][16] = weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime;
							valuesSpreadBackup[count][17] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin;
							valuesSpreadBackup[count][18] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax;
							valuesSpreadBackup[count][19] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate;
							valuesSpreadBackup[count][20] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd;
							valuesSpreadBackup[count][21] = weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd;
							// WRITE
							weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadProneDecay = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadSprintDecay = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadInAirDecay = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fHipReticleSidePos = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fAdsIdleAmount = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fHipIdleAmount = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->adsIdleSpeed = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->hipIdleSpeed = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fIdleCrouchFactor = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fIdleProneFactor = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxPitch = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd = 0.0F;
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd = 0.0F;
						}
					}
				}
				else
				{
					for (int count = 0; count < 962; count++)
					{
						if (weapons->weaponCompleteDefArr[count]->weapDef)
						{
							weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadDuckedDecay = valuesSpreadBackup[count][0];
							weapons->weaponCompleteDefArr[count]->weapDef->fHipSpreadProneDecay = valuesSpreadBackup[count][1];
							weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadSprintDecay = valuesSpreadBackup[count][2];
							weapons->weaponCompleteDefArr[count]->weapDef->hipSpreadInAirDecay = valuesSpreadBackup[count][3];
							weapons->weaponCompleteDefArr[count]->weapDef->fHipReticleSidePos = valuesSpreadBackup[count][4];
							weapons->weaponCompleteDefArr[count]->weapDef->fAdsIdleAmount = valuesSpreadBackup[count][5];
							weapons->weaponCompleteDefArr[count]->weapDef->fHipIdleAmount = valuesSpreadBackup[count][6];
							weapons->weaponCompleteDefArr[count]->weapDef->adsIdleSpeed = valuesSpreadBackup[count][7];
							weapons->weaponCompleteDefArr[count]->weapDef->hipIdleSpeed = valuesSpreadBackup[count][8];
							weapons->weaponCompleteDefArr[count]->weapDef->fIdleCrouchFactor = valuesSpreadBackup[count][9];
							weapons->weaponCompleteDefArr[count]->weapDef->fIdleProneFactor = valuesSpreadBackup[count][10];
							weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxPitch = valuesSpreadBackup[count][11];
							weapons->weaponCompleteDefArr[count]->weapDef->fGunMaxYaw = valuesSpreadBackup[count][12];
							weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxPitch = valuesSpreadBackup[count][13];
							weapons->weaponCompleteDefArr[count]->weapDef->fViewMaxYaw = valuesSpreadBackup[count][14];
							weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpStartTime = valuesSpreadBackup[count][15];
							weapons->weaponCompleteDefArr[count]->weapDef->adsIdleLerpTime = valuesSpreadBackup[count][16];
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMin = valuesSpreadBackup[count][17];
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadMax = valuesSpreadBackup[count][18];
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadDecayRate = valuesSpreadBackup[count][19];
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadFireAdd = valuesSpreadBackup[count][20];
							weapons->weaponCompleteDefArr[count]->weapDef->slideSpreadTurnAdd = valuesSpreadBackup[count][21];
						}
					}
				}
			}
		}

		updateInput();
		sdk::start_tick();

		for (int i = 0; i < globals::max_player_count; ++i)
		{
			
			if (local_player.get_index() == i)
				continue;

			auto cl_info_base = decryption::get_client_info_base();
			if (is_bad_ptr(cl_info_base))
				return;

			sdk::player_t player = sdk::get_player(i);
			if (is_bad_ptr(player.address)) 
				continue;

			bool is_friendly = player.team_id() == local_player.team_id();;

			if (is_friendly && !Settings::esp::b_friendly)
				continue;

			auto health = get_player_health(i);
			auto is_visible = player.is_visible();
			auto is_valid = player.is_valid();
			auto is_dead = player.is_dead();

			if (!is_valid || is_dead || health <= 0) 
				continue;

			float fdistance = 0;
			float display_string = 0;

			fdistance = sdk::units_to_m(globals::local_pos.distance_to(player.get_pos()));

			if (fdistance > Settings::esp::max_distance)
				continue;

			if (Settings::esp::b_visible && is_visible)
				current_color = is_friendly ? Settings::colors::visible_team : Settings::colors::visible_enemy;
			else
				current_color = is_friendly ? Settings::colors::not_visible_team : Settings::colors::not_visible_enemy;

			if (Settings::esp::b_radar)
				RadarGui(local_player, player, is_friendly, is_dead, current_color);

			vec2_t ScreenPos, BoxSize;
			if (!sdk::w2s(player.get_pos(), &ScreenPos, &BoxSize))continue;
			
			if (Settings::esp::b_box)
			{
				switch (Settings::esp::box_index)
				{
				case 0:g_draw::draw_corned_box(ScreenPos, BoxSize, current_color, 1.f); break;
				case 1:g_draw::draw_box(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, current_color, 1.f); break;
				default:
					break;
				}
			}

			if (Settings::esp::b_line)
			{
				ImVec2 origin;
				switch (Settings::esp::line_choose)
				{
				case 0:
				{
					origin.x = g_data::refdef->Width / 2;
					origin.y = g_data::refdef->Height / 2;
					break;
				}
				case 1:
				{
					origin.x =g_data::refdef->Width / 2;
					origin.y =g_data::refdef->Height;
					break;

				}
				default:
					break;
				}
				g_draw::draw_line(origin, ImVec2(ScreenPos.x, ScreenPos.y), current_color, 1.f);
			}
			if (Settings::esp::b_names)
			{
				std::string name = sdk::get_player_name(i);
				g_draw::draw_sketch_edge_text(font, name, ImVec2(
					ScreenPos.x + (BoxSize.x / 2),
					ScreenPos.y + BoxSize.y + 5 + display_string),
					18.0f,
					current_color, 
					true
				);
				display_string += 15;
			}

			if (Settings::esp::b_health)
			{
				g_draw::draw_health(get_player_health(i), player.get_pos());
			}

			if (Settings::esp::b_distance)
			{
				std::string distance_string("[ ");
				distance_string += std::to_string(fdistance);
				std::string::size_type end = distance_string.find('.');
				if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
				distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
				distance_string += "m ]";
				g_draw::draw_sketch_edge_text(font, distance_string, ImVec2(
					ScreenPos.x + (BoxSize.x / 2),
					ScreenPos.y + BoxSize.y + 5 + display_string),
					18.0f,
					current_color, 
					true
				);
			}

			if (Settings::esp::b_skeleton)
			{
				g_draw::draw_bones(i, player.get_pos(), current_color);
			}

			if (Settings::aimbot::b_lock)
			{
				vec3_t current_bone;
				vec2_t bone_screen_pos;
				sdk::update_vel_map(sdk::local_index(), local_player.get_pos());

				if (Settings::aimbot::b_target_bone)
				{
					if (!sdk::get_bone_by_player_index(i, aim_assist::get_bone_opt(), &current_bone))
						continue;

					sdk::update_vel_map(i, current_bone);

					if (Settings::aimbot::b_prediction && fdistance > 4.f)
						current_bone = sdk::get_prediction(i, local_player.get_pos(), current_bone, player.get_pos());

					if (!sdk::WorldToScreen(current_bone, &bone_screen_pos))
						continue;

					if (Settings::esp::b_aim_point)
						g_draw::draw_circle(ImVec2(bone_screen_pos.x, bone_screen_pos.y), 5, current_color, 100.0f);

					if (player.get_stance() == sdk::KNOCKED && Settings::aimbot::b_skip_knocked)
						continue;

					if (Settings::esp::b_visible && !player.is_visible())
						continue;

					if (is_friendly)
						continue;

					switch (Settings::aimbot::i_priority)
					{
					case 0:
						aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
						break;
					case 1:
						aim_assist::get_enemy_in_fov(bone_screen_pos);
						break;
					case 2:
						if (fdistance < Settings::aimbot::f_min_closest)
						{
							if (player.get_stance() == sdk::KNOCKED)
								continue;
							aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
						}
						else {
							aim_assist::get_enemy_in_fov(bone_screen_pos);
						}
						break;
					}
				}
				else
				{
					if (!once) {
						sdk::clear_vel_map();
						once = true;
					}

					sdk::update_vel_map(i, player.get_pos());

					current_bone = player.get_pos();

					if (Settings::aimbot::b_prediction && fdistance > 4.f)
						current_bone = sdk::get_prediction(i, local_player.get_pos(), current_bone);

					if (!sdk::head_to_screen(current_bone, &bone_screen_pos, player.get_stance()))
						continue;

					if (Settings::esp::b_aim_point)
						g_draw::draw_circle(ImVec2(bone_screen_pos.x, bone_screen_pos.y), 5, current_color, 100.0f);

					if (player.get_stance() == sdk::KNOCKED && Settings::aimbot::b_skip_knocked)
						continue;

					if (Settings::esp::b_visible && !player.is_visible())
						continue;

					if (is_friendly)
						continue;

					switch (Settings::aimbot::i_priority)
					{
					case 0:
						aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
						break;
					case 1:
						aim_assist::get_enemy_in_fov(bone_screen_pos);
						break;
					case 2:
						if (fdistance < Settings::aimbot::f_min_closest)
						{
							if (player.get_stance() == sdk::KNOCKED)
								continue;
							aim_assist::get_closest_enemy(bone_screen_pos, fdistance);
						}
						else {
							aim_assist::get_enemy_in_fov(bone_screen_pos);
						}
						break;
					}
				}
			}
		}
		aim_assist::is_aiming();
	}
	void loot_esp(ImFont* font)
	{
		if (sdk::in_game())
		{
			//utils::color_var item_color = Settings::colors::Green;
			ImColor current_color;
			current_color = Settings::colors::Green;
			float f_distance = 0;

			for (auto j = 0; j < 1000; j++)
			{
				if (sdk::in_game())
				{
					float display_string = 0;
					auto loot = sdk::get_loot_by_id(j);

					if (!loot->is_valid())
						continue;

					auto name = loot->get_name();

					if (is_bad_ptr(name) || !strlen(name))
						continue;

					auto pos = loot->get_position();
					if (pos.is_Zero())
						continue;

					vec2_t screen_pos;
					if (!sdk::WorldToScreen(pos, &screen_pos))
						continue;

					f_distance = sdk::units_to_m(globals::local_pos.distance_to(pos));
					if (f_distance > Settings::lootEsp::max_dist)
						continue;

					for (auto k = 0; k < IM_ARRAYSIZE(loot_defs); k++)
					{
						auto* loot_def = &loot_defs[k];

						if (name && strstr(name, loot_def->name))
						{
							if (strstr(name, "accessory_money") ||
								strstr(name, "loot_wm_armor_plate") ||
								strstr(name, "lm_armor_vest_01") ||
								strstr(name, "gasmask_a")) {

								current_color = Settings::colors::cash_color;
							}
							if (strstr(name, "mission_tablet_01_scavenger") ||
								strstr(name, "mission_tablet_01_assassination")) {

								current_color = Settings::colors::mission_color;
							}
							if (strstr(name, "loot_ammo_pistol_smg") ||
								strstr(name, "ammo_box_ar") ||
								strstr(name, "ammo_shotgun") ||
								strstr(name, "ammo_marksman_sniper")) {

								current_color = Settings::colors::ammo_color;
							}
							if (strstr(name, "loot_crate_01_br_legendary_01")) {
								current_color = Settings::colors::crate_color;
							}
							if (strstr(name, "military_loot_crate"))
							{
								current_color = Settings::colors::crate_color3;
							}

							if (loot_def->show != 0 && !*loot_def->show)
								continue;

							if (Settings::lootEsp::b_name)
							{
								std::string buff = std::string(loot_def->text);
								g_draw::draw_sketch_edge_text(font, buff, ImVec2(
									screen_pos.x,
									screen_pos.y + 5 + display_string),
									18.0f,
									current_color,
									true);
							}
							if (Settings::lootEsp::b_distance)
							{
								std::string distance_string("[ ");
								distance_string += std::to_string(f_distance);
								std::string::size_type end = distance_string.find('.');
								if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
									distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
								distance_string += "m ]";
								g_draw::draw_sketch_edge_text(font, distance_string, ImVec2(
									screen_pos.x,
									screen_pos.y + 5 + 20),
									18.0f,
									current_color,
									true);
							}
						}
					}
				}
			}
		}
	}
	void main_loop(ImFont* font) 
	{
		esp_loop(font);
		loot_esp(font);
	}
}

