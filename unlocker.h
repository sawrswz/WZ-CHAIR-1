//#include "stdafx.h"
//#include "xor.hpp"
//#include "types.h"
//
//#define _PRINT_DEBUG
//#define LOG(fmt, ...) Log_(xorstr(fmt), ##__VA_ARGS__)
//
//#define LOG_ADDR(var_name)										\
//		LOG(#var_name ": 0x%llX (0x%llX)", var_name, var_name > base ? var_name - base : 0);	
//
//#define INRANGE(x,a,b)	(x >= a && x <= b) 
//#define getBits( x )	(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
//#define getByte( x )	(getBits(x[0]) << 4 | getBits(x[1]))
//
//typedef int(*DWGetLogonStatus_t)(int);
//
//typedef bool(__fastcall* MoveResponseToInventory_t)(LPVOID, int);
//
//typedef void* (__cdecl* _fpGetLootBase)();
//typedef void(__cdecl* _fpFindStringtable)(const char* name, StringTable** table);
//typedef char* (__cdecl* _fpStringtableGetColumnValueForRow)(void* stringTable, int row, int column);
//
////using DWGetLogonStatus_t = int (*)(int);
////using MoveResponseToInventory_t = bool(__fastcall*)(LPVOID, int);
//
//
//namespace unlock {
//
//	/*extern BOOL g_running;*/
//	extern MoveResponseToInventory_t fpMoveResponseOrig;
//	extern __int64 base;
//	extern __int64 fpGetLootBase;
//	extern __int64 fpGetLogonStatus;
//	extern __int64 fpMoveResponseToInventory;
//	extern __int64 fpFindStringtable;
//	extern __int64 fpStringtableGetColumnValueForRow;
//
//	extern void Log_(const char* fmt, ...);
//
//	void* GetLootBase();
//	void FindStringTable(const char* name, StringTable** table);
//	char* StringTable_GetColumnValueForRow(void* stringTable, int row, int column);
//	void SaveLootTable(StringTable* loot_master);
//	void on_attach();
//	bool __fastcall MoveResponseToInventory_Hooked(LPVOID a1, int a2);
//	
//}