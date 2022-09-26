#include "stdafx.h"
#include "Menu.h"
//#include imgui.h"
//#include "imgui/imgui_impl_win32.h"
//#include "imgui/imgui_impl_dx12.h"
#include "sdk.h"
#include "xor.hpp"
#include "unlocker.h"
#include "settings.h"
//#include "snowflake.h"
#include "input.h"
#include <imgui_internal.h>
#include "imgui/imgui.h"

bool b_debug_open = false;
void hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size)noexcept;
std::string fov_scale;

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};


enum MENUSETTING : unsigned int
{
	ESP,
	AIMBOT,
	MISC,
	COLOR,
	SETTINGS
};

ImFont* main_font1 = nullptr;
ImFont* main_font2 = nullptr;
ImFont* main_font3 = nullptr;
ImFont* main_font4 = nullptr;
ImFont* main_font5 = nullptr;
ImFont* main_font6 = nullptr;
ImFont* main_font7 = nullptr;
ImFont* main_font8 = nullptr;


ImFont* GetFont(int index)
{
	switch (index)
	{
	case 0: return main_font1;
	case 1:return main_font2;
	case 2:return main_font3;
	case 3:return main_font4;
	case 4:return main_font5;
	case 5:return main_font6;
	case 6:return main_font7;
	case 7:return main_font8;
	default:
		break;
	}
}

extern void init_Font_List()
{

	ImGuiIO& io = ImGui::GetIO();

	main_font1 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 18.0f);

	main_font2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 18.0f);

	main_font3 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Cambria.ttf", 18.0f);

	main_font4 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Candara.ttf", 18.0f);

	main_font5 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Corbel.ttf", 18.0f);

	main_font6 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Segoe UI.ttf", 18.0f);

	main_font7 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18.0f);

	main_font8 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 18.0f);

}

void DrawRectRainbow()
{
	float rainbowSpeed = 0.001;
	static float staticHue = 0;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 panelPos = ImGui::GetWindowPos();
	staticHue -= rainbowSpeed;
	if (staticHue < -1.f) staticHue += 1.f;
	for (int i = 0; i < 860; i++)
	{
		float hue = staticHue + (1.f / (float)860) * i;
		if (hue < 0.f) hue += 1.f;
		ImColor cRainbow = ImColor::HSV(hue, 1.f, 1.f);
		draw_list->AddRectFilled(ImVec2(panelPos.x + i, panelPos.y), ImVec2(panelPos.x + i + 1, panelPos.y), cRainbow);
	}
}
RECT rc = { 0 };

bool init_font = true;
namespace g_menu
{
	int aimbotKeyMode{ 0 };
	bool b_menu_open = true;
	bool b_render_meu = true;
	std::string str_config_name = "";
	const char* box_types[] = { "2D Corner Box", "2D Box" };
	const char* Fontname[]{ "Arial","Calibri","Cambria","Candara","Corbel","Segoe UI","Tahoma","Verdana" };
	const char* line_type[] = { "Middle", "Bottom" };
	char* weaponname_internal;
	char* weaponname;
	char* weaponname_alt;
	POINT mouse;
	bool create_snow = true;
	void menu()
	{
		if (GetAsyncKeyState(VK_INSERT) & 0x1)
		{
			b_menu_open = !b_menu_open;
		}
		if (b_menu_open && screenshot::visuals)
		{
			if (create_snow)
			{
				//Snowflake::CreateSnowFlakes(snow1, 250, 5.f/*minimum size*/, 20.f/*maximum size*/, 0/*imgui window x position*/, 0/*imgui window y position*/, 2560, 1440, Snowflake::vec3(0.f, 0.003f)/*gravity*/, IM_COL32(255, 255, 255, 100)/*color*/);
				create_snow = false;

			}
			ImGui::PushFont(GetFont(Settings::misc::font_index));
			ImGui::SetNextWindowSize({ 600, 468 });
			ImGui::Begin(xorstr("Sprite"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2((float)ImGui::GetIO().MousePos.x - 12, (float)ImGui::GetIO().MousePos.y), ImVec2(((float)ImGui::GetIO().MousePos.x - 12) + (12 * 2), (float)ImGui::GetIO().MousePos.y), ImGui::GetColorU32({ 255, 0, 0, 255 }), 1);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2((float)ImGui::GetIO().MousePos.x, (float)ImGui::GetIO().MousePos.y - 12), ImVec2((float)ImGui::GetIO().MousePos.x, ((float)ImGui::GetIO().MousePos.y - 12) + (12 * 2)), ImGui::GetColorU32({ 255, 0, 0,255 }), 1);
			if (Settings::misc::b_snow)
			{
				GetCursorPos(&mouse);
				//Snowflake::Update(snow1, Snowflake::vec3(mouse.x, mouse.y)/*mouse x and y*/, Snowflake::vec3(2560, 1440)/*hWnd x and y positions*/); // you can change a few things inside the update function
			}
			ImGui::Text(xorstr("In Game: %s"), globals::b_in_game ? "yes" : "no");
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::SameLine();
			static int tab;
			{
				if (ImGui::sub(xorstr("Visuals"), 1 == tab))  tab = 1;
				ImGui::SameLine();
				if (ImGui::sub(xorstr("Aimbot"), 2 == tab))  tab = 2;
				ImGui::SameLine();
				if (ImGui::sub(xorstr("Misc"), 3 == tab))  tab = 3;
				ImGui::SameLine();
				if (ImGui::sub(xorstr("Items"), 4 == tab))  tab = 4;
				ImGui::SameLine();
				if (ImGui::sub(xorstr("Colors"), 5 == tab))  tab = 5;
			}

			if (tab == 0)
			{
				ImGui::Text(xorstr("Welcome to Sprite dear user"));
				ImGui::Separator();
				ImGui::Text(xorstr("Changelogs :"));
				ImGui::Text(xorstr("-Released"));
				ImGui::Text(xorstr("-No Spread"));
				ImGui::Separator();
				ImGui::Text(xorstr("Status: ")); ImGui::SameLine(); ImGui::TextColored(ImVec4(0, 1, 0, 1), xorstr("Undetected"));
				ImGui::Text(xorstr("  "));
				ImGui::Text(xorstr("  "));
				ImGui::Text(xorstr("Discord : Ware#7294"));
			}

			if (tab == 1)
			{
				ImGui::Checkbox(xorstr("Visible Check"), &Settings::esp::b_visible);
				ImGui::Checkbox(xorstr("Show Only Visible"), &Settings::esp::b_visible_only);
				ImGui::Checkbox(xorstr("Boxs"), &Settings::esp::b_box);
				ImGui::SameLine();
				ImGui::Combo("##", &Settings::esp::box_index, box_types, 2);
				ImGui::Checkbox(xorstr("Health"), &Settings::esp::b_health);
				ImGui::Checkbox(xorstr("Lines"), &Settings::esp::b_line);
				ImGui::SameLine();
				ImGui::Combo("####", &Settings::esp::line_choose, line_type, 2);
				ImGui::Checkbox(xorstr("Skeletons"), &Settings::esp::b_skeleton);
				ImGui::Checkbox(xorstr("Gamgertag"), &Settings::esp::b_names);
				ImGui::Checkbox(xorstr("Distance"), &Settings::esp::b_distance);
				ImGui::Checkbox(xorstr("Show Team"), &Settings::esp::b_friendly);
				ImGui::SliderInt(xorstr("##MAXDISTANCE"), &Settings::esp::max_distance, 0, 1000, xorstr("ESP Distance: %d"));
				ImGui::Checkbox(xorstr("Radar"), &Settings::esp::b_radar);
				if (Settings::esp::b_radar)
				{
					ImGui::SetWindowSize(ImVec2(550, 690), ImGuiCond_Always);
					ImGui::SliderInt(xorstr("##RADARZOOM"), &Settings::esp::radar_zoom, 10, 100, xorstr("Radar Zoom: %d"));
				}
				else
				{
					ImGui::SetWindowSize(ImVec2(550, 625), ImGuiCond_Always);

				}
			}

			if (tab == 2)
			{
				hotkey("Aim Key", aimbotkey, 80.f, { 110,40 });
				ImGui::Checkbox(xorstr("Aim Lock"), &Settings::aimbot::b_lock);
				ImGui::Checkbox(xorstr("No Spread"), &Settings::aimbot::nospread);
				ImGui::Checkbox(xorstr("Skip Knocked"), &Settings::aimbot::b_skip_knocked);
				ImGui::Checkbox(xorstr("Enable FOV"), &Settings::esp::b_fov);
				ImGui::Checkbox(xorstr("Prediction"), &Settings::aimbot::b_prediction);
				ImGui::Checkbox(xorstr("Enable Smooth"), &Settings::aimbot::b_smooth);
				if (Settings::aimbot::b_lock)
				{

					ImGui::SliderFloat(xorstr("##LOCKSPEED"), &Settings::aimbot::f_speed, 2.5f, 30.0f, xorstr("Speed: %0.1f"));
					ImGui::Combo(xorstr("Target Priority"), &Settings::aimbot::i_priority, xorstr("Closest\0FOV\0Closest and FOV\0"));
					if (Settings::aimbot::i_priority == 0 || Settings::aimbot::i_priority == 2)
					{
						ImGui::SliderFloat(xorstr("##PRIORDIST"), &Settings::aimbot::f_min_closest, 5.0f, 50.0f, xorstr("Priority distance: %0.1fm"));
					}
					ImGui::Checkbox(xorstr("Use Bones"), &Settings::aimbot::b_target_bone);
					if (Settings::aimbot::b_target_bone)
					{
						ImGui::Combo(xorstr("Target Bone"), &Settings::aimbot::i_bone, xorstr("Spine\0Head\0Neck\0Chest\0"));
					}
					if (Settings::esp::b_fov)
					{
						ImGui::SliderInt(xorstr("##LOCKFOV"), &Settings::esp::fov_size, 10.f, 800.f, xorstr("FOV Size: %0d"));
					}
				}
			}

			if (tab == 3)
			{
				ImGui::Checkbox(xorstr("Show Crosshair"), &Settings::esp::b_crosshair);
				ImGui::Checkbox(xorstr("No Recoil"), &Settings::aimbot::b_recoil);
				ImGui::Checkbox("Advanced UAV", &Settings::esp::b_auav);
				ImGui::BeginChild(xorstr("##1CONFIGSCOL1"));
				{
					ImGui::PushItemWidth(-1);
					static std::string str_warn = xorstr("Config Must be at least 3 char length!");
					ImGui::Text(xorstr("Config:"));
					static char buf[128] = "";
					const auto width = ImGui::GetWindowWidth();
					ImGui::PushItemWidth(width - 29);
					ImGui::InputText(xorstr("##CONFIGNAME"), buf, 32);
					ImGui::PopItemWidth();
					ImGui::SameLine();
					if (ImGui::Button(xorstr("+"), ImVec2(20, 20)))
					{
						if (std::string(buf).length() < 3)
						{
							ImGui::OpenPopup(xorstr("warn"));
							str_warn = xorstr("Config Must be at least 3 char length!");
						}
						else
						{
							Settings::Save_Settings(buf);
						}
					}
					auto vec_files = Settings::GetList();
					static auto i_config = 0;
					ImGui::PushItemWidth(-1);
					ImGui::Combo(xorstr("##CONFIG"), &i_config, vector_getter, static_cast<void*>(&vec_files), vec_files.size());
					ImGui::PopItemWidth();

					const auto _pad = width / 2;
					if (ImGui::Button(xorstr("Save"), ImVec2(_pad - 4, 20)))
					{
						if (!vec_files.empty())
							Settings::Save_Settings(vec_files[i_config]);

					}
					ImGui::SameLine();
					if (ImGui::Button(xorstr("Load"), ImVec2(_pad - 4, 20)))
					{
						if (!vec_files.empty())
							Settings::Load_Settings(vec_files[i_config]);
					}
					if (ImGui::Button(xorstr("Delete"), ImVec2(width, 20)))
					{
						if (!vec_files.empty()) {

							auto str_config = xorstr("C:\\Sprite\\Configs\\") + vec_files[i_config] + xorstr(".cfg");
							if (remove(str_config.c_str()) != 0)
							{
								str_warn = xorstr("Could not delete the file!");
								ImGui::OpenPopup(xorstr("warn"));
							}
							str_config = xorstr("");
							str_config_name = xorstr("");
						}
					}
					if (ImGui::BeginPopup(xorstr("warn")))
					{
						ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), str_warn.c_str());
						ImGui::EndPopup();
					}
				}
				ImGui::EndChild();
			}

			if (tab == 4)
			{
				ImGui::Text(xorstr("Items"));
				ImGui::Separator();
				ImGui::SliderInt(xorstr("##MAXDISTANCE"), &Settings::lootEsp::max_dist, 5, 1000, xorstr("ESP Distance: %dm"));
				ImGui::Separator();
				ImGui::Checkbox(xorstr("Name"), &Settings::lootEsp::b_name);
				ImGui::Checkbox(xorstr("Distance"), &Settings::lootEsp::b_distance);
				ImGui::Dummy(ImVec2(-5, 5));
				ImGui::Text(xorstr("Filter"));
				ImGui::Separator();
				ImGui::Checkbox(xorstr("Show Weapons"), &Settings::lootEsp::b_weapons);
				ImGui::Checkbox(xorstr("Show Legendary crates"), &Settings::lootEsp::b_crates);
				ImGui::Checkbox(xorstr("Show Military crates"), &Settings::lootEsp::b_crates3);
				ImGui::Checkbox(xorstr("Show Missions"), &Settings::lootEsp::b_missions);
				ImGui::Checkbox(xorstr("Show Ammo"), &Settings::lootEsp::b_ammo);
			}

			if (tab == 5)
			{
				ImGui::Text(xorstr("Player"));
				ImGui::Separator();
				ImGui::Text(xorstr("Visible Team Color"));
				ImGui::ColorEdit4(xorstr("##esp Color1"), (float*)&Settings::colors::visible_team);
				ImGui::Spacing();
				ImGui::Text(xorstr("Not Visible Team Color"));
				ImGui::ColorEdit4("##esp Color2", (float*)&Settings::colors::not_visible_team);
				ImGui::Spacing();
				ImGui::Text(xorstr("Visible Enemy Color"));
				ImGui::ColorEdit4(xorstr("##esp Color3"), (float*)&Settings::colors::visible_enemy);
				ImGui::Spacing();
				ImGui::Text(xorstr("Not Visible Enemy Color"));
				ImGui::ColorEdit4(xorstr("##esp Color4"), (float*)&Settings::colors::not_visible_enemy);
				ImGui::Spacing();
				ImGui::Text(xorstr("Items"));
				ImGui::Separator();
				ImGui::Text(xorstr("Cash Color"));
				ImGui::ColorEdit4(xorstr("##cash_col"), (float*)&Settings::colors::cash_color);
				ImGui::Spacing();
				ImGui::Text(xorstr("Weapon Color"));
				ImGui::ColorEdit4("##weapon_col", (float*)&Settings::colors::weapon_color);
				ImGui::Spacing();
				ImGui::Text(xorstr("Legendary Crate Color"));
				ImGui::ColorEdit4(xorstr("##create_1"), (float*)&Settings::colors::crate_color);
				ImGui::Spacing();
				ImGui::Text(xorstr("Military Crate Color"));
				ImGui::ColorEdit4(xorstr("##create_3"), (float*)&Settings::colors::crate_color3);
				ImGui::Spacing();
				ImGui::Text(xorstr("Ammo Color"));
				ImGui::ColorEdit4(xorstr("##ammo_col"), (float*)&Settings::colors::ammo_color);
				ImGui::Text("Font: ");
				ImGui::SameLine();
				ImGui::Combo("###Font", &Settings::misc::font_index, Fontname, IM_ARRAYSIZE(Fontname));
			}
			ImGui::PopFont();
			ImGui::End();
		}
	}
}
void hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size) noexcept
{
	const auto id = ImGui::GetID(label);
	ImGui::PushID(label);
	ImGui::TextUnformatted(label);
	ImGui::SameLine(samelineOffset);
	if (ImGui::GetActiveID() == id)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
		ImGui::Button("...", size);
		ImGui::PopStyleColor();

		ImGui::GetCurrentContext()->ActiveIdAllowOverlap = true;
		if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]) || key.setToPressedKey())
			ImGui::ClearActiveID();
	}
	else if (ImGui::Button(key.toString(), size))
	{
		ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
	}
	ImGui::PopID();
}