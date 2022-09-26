//#include "unlocker.h"
//#include "lazyimporter.h"
//#include "settings.h"
//#include "offsets.h"
//
//LPCSTR CW_CAMOS[]{
//	//CW camos
//	"iw8_ar_t9longburst_camos",
//	"iw8_ar_t9mobility_camos",
//	"iw8_ar_t9slowfire_camos",
//	"iw8_ar_t9damage_camos",
//	"iw8_ar_t9slowhandling_camos",
//	"iw8_ar_t9british_camos",
//	"iw8_ar_t9standard_camos",
//	"iw8_ar_t9accurate_camos",
//	"iw8_ar_t9fasthandling_camos",
//	"iw8_ar_t9fastfire_camos",
//	"iw8_ar_t9fastburst_camos",
//	"iw8_sh_t9semiauto_camos",
//	"iw8_sh_t9fullauto_camos",
//	"iw8_sh_t9pump_camos",
//	"iw8_sm_t9standard_camos",
//	"iw8_sm_t9accurate_camos",
//	"iw8_sm_t9spray_camos",
//	"iw8_sm_t9burst_camos",
//	"iw8_sm_t9semiauto_camos",
//	"iw8_sm_t9capacity_camos",
//	"iw8_sm_t9powerburst_camos",
//	"iw8_sm_t9cqb_camos",
//	"iw8_sm_t9nailgun_camos",
//	"iw8_sm_t9fastfire_camos",
//	"iw8_sm_t9heavy_camos",
//	"iw8_sm_t9handling_camos",
//	"iw8_sm_t9standard_camos",
//	"iw8_sm_t9accurate_camos",
//	"iw8_sm_t9spray_camos",
//	"iw8_sm_t9burst_camos",
//	"iw8_sm_t9semiauto_camos",
//	"iw8_sm_t9capacity_camos",
//	"iw8_sm_t9powerburst_camos",
//	"iw8_sm_t9cqb_camos",
//	"iw8_sm_t9nailgun_camos",
//	"iw8_sm_t9fastfire_camos",
//	"iw8_sm_t9heavy_camos",
//	"iw8_sm_t9handling_camos",
//	"iw8_sn_t9cannon_camos",
//	"iw8_sn_t9accurate_camos",
//	"iw8_sn_t9crossbow_camos",
//	"iw8_sn_t9damagesemi_camos",
//	"iw8_sn_t9powersemi_camos",
//	"iw8_sn_t9precisionsemi_camos",
//	"iw8_sn_t9quickscope_camos",
//	"iw8_sn_t9standard_camos",
//	"iw8_lm_t9accurate_camos",
//	"iw8_lm_t9fastfire_camos",
//	"iw8_lm_t9light_camos",
//	"iw8_lm_t9slowfire_camos",
//	"iw8_pi_t9burst_camos",
//	"iw8_pi_t9fullauto_camos",
//	"iw8_pi_t9semiauto_camos",
//	"iw8_pi_t9pistolshot_camos",
//	"iw8_pi_t9revolver_camos"
//};
//
//LPCSTR MW_CAMOS[]{
//	//MW camos
//	"iw8_ar_anovember94_camos",
//	"iw8_ar_valpha_camos",
//	"iw8_ar_mike4_camos",
//	"iw8_ar_akilo47_camos",
//	"iw8_ar_asierra12_camos",
//	"iw8_ar_falpha_camos",
//	"iw8_ar_mcharlie_camos",
//	"iw8_ar_kilo433_camos",
//	"iw8_ar_falima_camos",
//	"iw8_ar_scharlie_camos",
//	"iw8_ar_tango21_camos",
//	"iw8_ar_sierra552_camos",
//	"iw8_ar_galima_camos",
//	"iw8_sm_smgolf45_camos",
//	"iw8_sm_mpapa5_camos",
//	"iw8_sm_charlie9_camos",
//	"iw8_sm_secho_camos",
//	"iw8_sm_victor_camos",
//	"iw8_sm_uzulu_camos",
//	"iw8_sm_mpapa7_camos",
//	"iw8_sm_papa90_camos",
//	"iw8_sm_augolf_camos",
//	"iw8_sm_beta_camos",
//	"iw8_sn_sksierra_camos",
//	"iw8_sn_mike14_camos",
//	"iw8_sn_crossbow_camos",
//	"iw8_sn_kilo98_camos",
//	"iw8_sn_hdromeo_camos",
//	"iw8_sn_delta_camos",
//	"iw8_sn_alpha50_camos",
//	"iw8_sn_sbeta_camos",
//	"iw8_sn_romeo700_camos",
//	"iw8_sn_xmike109_camos",
//	"iw8_sh_aalpha12_camos",
//	"iw8_sh_dpapa12_camos",
//	"iw8_sh_oscar12_camos",
//	"iw8_sh_charlie725_camos",
//	"iw8_sh_romeo870_camos",
//	"iw8_sh_mike26_camos",
//	"iw8_lm_slima_camos",
//	"iw8_lm_kilo121_camos",
//	"iw8_lm_pkilo_camos",
//	"iw8_lm_lima86_camos",
//	"iw8_lm_mgolf34_camos",
//	"iw8_lm_mgolf36_camos",
//	"iw8_lm_mkilo3_camos",
//	"iw8_lm_sierrax_camos",
//	"iw8_pi_golf21_camos",
//	"iw8_pi_papa320_camos",
//	"iw8_pi_decho_camos",
//	"iw8_pi_mike1911_camos",
//	"iw8_pi_cpapa_camos",
//	"iw8_pi_mike9_camos",
//	"iw8_pi_mike_camos",
//	"iw8_la_rpapa7_camos",
//	"iw8_la_gromeo_camos",
//	"iw8_la_juliet_camos",
//	"iw8_la_kgolf_camos",
//	"iw8_me_riotshield_camos",
//	"iw8_knife_camos",
//	"iw8_me_akimboblades_camos",
//	"iw8_me_akimboblunt_camos"
//};
//
//namespace unlock {
//
//	/*BOOL g_running = TRUE;*/
//	MoveResponseToInventory_t fpMoveResponseOrig = NULL;
//
//	__int64 base = NULL;
//	__int64 fpGetLogonStatus = NULL;
//	__int64 fpFindStringtable = NULL;
//	__int64 fpStringtableGetColumnValueForRow = NULL;
//	__int64 fpMoveResponseToInventory = NULL;
//	__int64 fpGetLootBase = NULL;
//
//	void Log_(const char* fmt, ...) {
//		char		text[4096];
//		va_list		ap;
//		va_start(ap, fmt);
//		vsprintf_s(text, fmt, ap);
//		va_end(ap);
//
//		std::ofstream logfile(xorstr("log.txt"), std::ios::app);
//		if (logfile.is_open() && text)	
//			logfile << text << std::endl;
//
//		logfile.close();
//	}
//
//	__int64 find_pattern(__int64 range_start, __int64 range_end, const char* pattern) {
//		const char* pat = pattern;
//		__int64 firstMatch = NULL;
//		__int64 pCur = range_start;
//		__int64 region_end;
//		MEMORY_BASIC_INFORMATION mbi{};
//		while (sizeof(mbi) == VirtualQuery((LPCVOID)pCur, &mbi, sizeof(mbi))) {
//			if (pCur >= range_end - strlen(pattern))
//				break;
//			if (!(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READWRITE))) {
//				pCur += mbi.RegionSize;
//				continue;
//			}
//			region_end = pCur + mbi.RegionSize;
//			while (pCur < region_end)
//			{
//				if (!*pat)
//					return firstMatch;
//				if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
//					if (!firstMatch)
//						firstMatch = pCur;
//					if (!pat[1] || !pat[2])
//						return firstMatch;
//
//					if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
//						pat += 3;
//					else
//						pat += 2;
//				}
//				else {
//					if (firstMatch)
//						pCur = firstMatch;
//					pat = pattern;
//					firstMatch = 0;
//				}
//				pCur++;
//			}
//		}
//		return NULL;
//	}
//
//	bool init() {
//		base = (uintptr_t)(iat(GetModuleHandleA).get()("ModernWarfare.exe"));
//		return true;
//	}
//
//	bool find_sigs()
//	{
//		MODULEINFO moduleInfo;
//		auto is_module_valid = (iat(K32GetModuleInformation).get()(iat(GetCurrentProcess).get()(), iat(GetModuleHandleA).get()("ModernWarfare.exe"), &moduleInfo, sizeof(MODULEINFO)));
//		if (!is_module_valid || !moduleInfo.lpBaseOfDll) {
//			LOG("Couldnt GetModuleInformation");
//			return NULL;
//		}
//
//		LOG("Base: 0x%llx", moduleInfo.lpBaseOfDll);
//		LOG("Size: 0x%llx", moduleInfo.SizeOfImage);
//
//		__int64 searchStart = (__int64)moduleInfo.lpBaseOfDll;
//		__int64 searchEnd = (__int64)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage;
//
//		bool result = true;
//
//		auto resolve_jmp = [](__int64 addr) -> __int64 {
//			return *(int*)(addr + 1) + addr + 5;
//		};
//
//		auto resolve_lea = [](__int64 addr) -> __int64 {
//			return *(int*)(addr + 3) + addr + 7;
//		};
//
//		LOG_ADDR(fpGetLogonStatus = resolve_jmp(
//			find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 83 F8 02 0F 84 ? ? ? ? 48 89"))));
//		LOG_ADDR(fpFindStringtable = resolve_jmp(
//			find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 48 8B 8C 24 ? ? ? ? E8 ? ? ? ? 44"))));
//		LOG_ADDR(fpStringtableGetColumnValueForRow = resolve_jmp(
//			find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 48 8D 4B 02 FF"))));
//		LOG_ADDR(fpMoveResponseToInventory =
//			(find_pattern(searchStart, searchEnd, xorstr("83 7C 24 ? ? 74 1B 83 7C 24 ? ? 0F 84")) - 0x2B));
//		LOG_ADDR(fpGetLootBase = resolve_jmp(
//			(find_pattern(searchStart, searchEnd, xorstr("E8 ? ? ? ? 48 89 44 24 ? 41 B9 ? ? ? ? 44 ")))));
//
//		return result;
//	}
//
//	void get_func_ptr()
//	{
//		fpGetLogonStatus = base + offsets::pGetLogonStatus;
//		fpFindStringtable = base + offsets::pFindStringtable;
//		fpStringtableGetColumnValueForRow = base + offsets::pStringtableGetColumnValueForRow;
//		fpMoveResponseToInventory = base + offsets::pMoveResponseToInventory;
//		fpGetLootBase = base + offsets::pGetLootBase;
//	}
//
//	void* GetLootBase() {
//
//		return reinterpret_cast<void* (__cdecl*)()>(fpGetLootBase)();
//	}
//
//	void FindStringTable(const char* name, StringTable** table) {
//
//		reinterpret_cast<void(__cdecl*)(const char*, StringTable**)>(fpFindStringtable)(name, table);
//	}
//
//	char* StringTable_GetColumnValueForRow(void* stringTable, int row, int column) {
//
//		/*std::ofstream test;
//		test.open("example.csv");
//		test << stringTable;
//		test.close();*/
//		return reinterpret_cast<char* (__cdecl*)(void*, int, int)>(fpStringtableGetColumnValueForRow)(stringTable, row, column);
//	}
//
//	void SaveLootTable(StringTable* loot_master) {
//		std::ofstream myfile;
//		myfile.open("lootMaster2.txt");
//		for (int i = 1; i < loot_master->rowCount; i++) {
//			char* loot_type = StringTable_GetColumnValueForRow(loot_master, i, 2);
//			myfile << loot_type;
//		}
//		myfile.close();
//	}
//
//	bool __fastcall MoveResponseToInventory_Hooked(LPVOID a1, int a2) 
//	{
//		fpMoveResponseOrig(a1, a2);
//
//		auto pLootBase = GetLootBase();
//
//		auto pInventory = (LootItem*)((uintptr_t)pLootBase + 0x40/*64*/);
//
//		auto pNumItems = (uint32_t*)((uintptr_t)pLootBase + 0x3A9C0/*240064*/);
//
//		int curCount = *pNumItems;
//
//		auto updateOrAddItem = [&](int itemId, int quantity) {
//
//			bool bFound = false;
//
//			for (int i = 0; i < 30000; i++) {
//				if (pInventory[i].m_itemId == itemId && pInventory[i].m_itemQuantity < 1) {
//					pInventory[i].m_itemQuantity++;
//					bFound = true;
//					break;
//				}
//			}
//
//			if (!bFound) {
//				pInventory[curCount].m_itemId = itemId;
//				pInventory[curCount].m_itemQuantity = 1;
//
//				curCount++;
//				(*pNumItems)++;
//
//				*(BYTE*)((uintptr_t)pLootBase + 240072) = 0;
//			}
//		};
//
//		StringTable* loot_master = nullptr;
//		FindStringTable(xorstr("loot/loot_master.csv"), &loot_master);
//		//unlock::SaveLootTable(loot_master);
//		//std::ofstream myfile;
//		//myfile.open("C:\\lootMaster2.txt");
//		for (int i = 1; i < loot_master->rowCount; i++) {
//			char* loot_type = StringTable_GetColumnValueForRow(loot_master, i, 2);
//			//myfile << loot_type << "\n";
//			if (strstr(loot_type, "attachment") ||
//				strstr(loot_type, "killstreak") ||
//				strstr(loot_type, "perk") ||
//				strstr(loot_type, "t9") ||  //disables the few cw camos it finds
//				strstr(loot_type, "special") ||
//				strstr(loot_type, "firstparty") ||
//				strstr(loot_type, "consumable") ||
//				strstr(loot_type, "battlepass") ||
//				strstr(loot_type, "dlc") ||
//				strstr(loot_type, "bundle") ||
//				strstr(loot_type, "mission") ||
//				strstr(loot_type, "taunts") ||
//				strstr(loot_type, "intro") ||
//				strstr(loot_type, "super") ||
//				*loot_type == '\0' || //checks for an emtpy string
//				strstr(loot_type, "attachment") && strstr(loot_type, "iw8_") ||
//				strstr(loot_type, "camos") && strstr(loot_type, "iw8_"))
//				continue;
//			if (loot_type[0] == '#')
//				continue;
//			char buf[1024];
//			sprintf_s(buf, "loot/%s_ids.csv", loot_type);
//			//LOG("%s", buf);
//			StringTable* string_table = nullptr;
//			unlock::FindStringTable(buf, &string_table);
//			if (!string_table)
//				continue;
//			for (int s = 0; s < string_table->rowCount; s++) {
//				updateOrAddItem(atoi(StringTable_GetColumnValueForRow(string_table, s, 0)), 1);
//			}
//		}
//		//myfile.close();
//
//		//if (Settings::Get().unlock_opt == 0)
//		//{
//		//	char c_a_buff[1024];
//		//	for (int i = 1; i < ARRAYSIZE(CW_CAMOS); i++) {
//		//		sprintf_s(c_a_buff, "loot/%s_ids.csv", CW_CAMOS[i]);
//		//		//LOG("%s", buf);
//		//		StringTable* string_table = nullptr;
//		//		unlock::FindStringTable(c_a_buff, &string_table);
//		//		if (!string_table)
//		//			continue;
//		//		for (int s = 0; s < string_table->rowCount; s++) {
//		//			updateOrAddItem(atoi(StringTable_GetColumnValueForRow(string_table, s, 0)), 1);
//		//		}
//		//	}
//		//}
//		//if (Settings::Get().unlock_opt == 1)
//		//{
//		//	char c_a_buff[1024];
//		//	for (int i = 1; i < ARRAYSIZE(MW_CAMOS); i++) {
//		//		sprintf_s(c_a_buff, "loot/%s_ids.csv", MW_CAMOS[i]);
//		//		//LOG("%s", buf);
//		//		StringTable* string_table = nullptr;
//		//		unlock::FindStringTable(c_a_buff, &string_table);
//		//		if (!string_table)
//		//			continue;
//		//		for (int s = 0; s < string_table->rowCount; s++) {
//		//			updateOrAddItem(atoi(StringTable_GetColumnValueForRow(string_table, s, 0)), 1);
//		//		}
//		//	}
//		//}
//
//		MH_RemoveHook((LPVOID)fpMoveResponseToInventory);
//
//		return false;
//	}
//
//	void on_attach() {
//
//		init();
//		//find_sigs();
//		get_func_ptr();
//	}
//
//}