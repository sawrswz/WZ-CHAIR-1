#pragma once
#include "stdafx.h"
extern bool b_menu_open;

namespace g_menu
{
	extern	int aimbotKeyMode;
	extern bool b_menu_open;
	extern std::string str_config_name;
	void menu();
	extern char* weaponname_internal;
	extern char* weaponname;
	extern char* weaponname_alt;
}

extern ImFont* main_font1;
extern ImFont* main_font2;
extern ImFont* main_font3;
extern ImFont* main_font4;
extern ImFont* main_font5;
extern ImFont* main_font6;
extern ImFont* main_font7;
extern ImFont* main_font8;
extern void init_Font_List();