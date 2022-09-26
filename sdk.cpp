#include "sdk.h"
#include "defs.h"
#include "offsets.h"
#include "xor.hpp"
#include "imgui/imgui_internal.h"
#include "lazyimporter.h"
#include "settings.h"

namespace process
{
	HWND hwnd;

	BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hWnd, &dwPid);
		if (dwPid == lParam)
		{
			hwnd = hWnd;
			return FALSE;
		}
		return TRUE;
	}

	HWND get_process_window()
	{
		if (hwnd)
			return hwnd;

		EnumWindows(EnumWindowCallBack, GetCurrentProcessId());

		if (hwnd == NULL)
			Exit();

		return hwnd;
	}
}

namespace sdk
{
	const DWORD nTickTime = 64;//64 ms
	bool bUpdateTick = false;
	std::map<DWORD, velocityInfo_t> velocityMap;

	vec3_t get_camera_location()
	{
		float x = *(float*)(*(uint64_t*)(g_data::base + offsets::camera_base) + 0x108);
		float y = *(float*)(*(uint64_t*)(g_data::base + offsets::camera_base) + 0x118);
		float z = *(float*)(*(uint64_t*)(g_data::base + offsets::camera_base) + 0x128);

		return vec3_t{ x, y, z };
	}

	bool WorldToScreen(const vec3_t& WorldPos, vec2_t* ScreenPos)
	{
		vec3_t ViewOrig = get_camera_location();
		auto refdef = g_data::refdef;
		vec3_t vLocal, vTransform;
		vLocal = WorldPos - ViewOrig;
		// get our dot products from viewAxis
		vTransform.x = vLocal.dot(refdef->ViewAxis[1]);
		vTransform.y = vLocal.dot(refdef->ViewAxis[2]);
		vTransform.z = vLocal.dot(refdef->ViewAxis[0]);
		// make sure it is in front of us
		if (vTransform.z < 0.01f)
			return false;
		ScreenPos->x = ((refdef->Width / 2) * (1 - (vTransform.x / refdef->FovX / vTransform.z)));
		ScreenPos->y = ((refdef->Height / 2) * (1 - (vTransform.y / refdef->FovY / vTransform.z)));

		if (ScreenPos->x < 1 || ScreenPos->y < 1 || (ScreenPos->x > refdef->Width) || (ScreenPos->y > refdef->Height)) {
			return false;
		}

		return true;
	}

	float units_to_m(float units) {
		return units * 0.0254;
	}

	float m_to_units(float meters) {
		return 0.0254 / meters;
	}

	float xangle(const vec3_t& LocalPos, const vec3_t& WorldPos)
	{
		float dl = sqrt((WorldPos.x - LocalPos.x) * (WorldPos.x - LocalPos.x) + (WorldPos.y - LocalPos.y) * (WorldPos.y - LocalPos.y));

		if (dl == 0.0f)
			dl = 1.0f;

		float dl2 = abs(WorldPos.x - LocalPos.x);
		float teta = ((180.0f / M_PI) * acos(dl2 / dl));

		if (WorldPos.x < LocalPos.x)
			teta = 180.0f - teta;

		if (WorldPos.y < LocalPos.y)
			teta = teta * -1.0f;

		if (teta > 180.0f)
			teta = (360.0f - teta) * (-1.0f);

		if (teta < -180.0f)
			teta = (360.0f + teta);

		return teta;
	}

	void rotation_point_alpha(float x, float y, float z, float alpha, vec3_t* outVec3)
	{
		if (!g_data::dx9rot)
		{
			std::ofstream out("C:\\ricocheat\\error.txt");
			out << "ERROR---0xd100001\n";
			out.close();
			exit(0);

		}
		typedef LinearTransform* (WINAPI* t_D3DXMatrixRotationY)(LinearTransform* pOut, FLOAT Angle);
		static t_D3DXMatrixRotationY D3DXMatrixRotationY = NULL;
		if (D3DXMatrixRotationY == NULL)
			D3DXMatrixRotationY = (t_D3DXMatrixRotationY)GetProcAddress(g_data::dx9rot, xorstr("D3DXMatrixRotationY"));

		typedef Vec4* (WINAPI* t_D3DXVec3Transform)(Vec4* pOut, CONST Vec4* pV, CONST LinearTransform* pM);
		static t_D3DXVec3Transform D3DXVec4Transform = NULL;
		if (D3DXVec4Transform == NULL)
			D3DXVec4Transform = (t_D3DXVec3Transform)GetProcAddress(g_data::dx9rot, xorstr("D3DXVec4Transform"));

		Matrix4x4 rot1;
		Vec4 vec = { x, z, y, 1.0f };
		D3DXMatrixRotationY((LinearTransform*)&rot1, alpha * M_PI / 180.0f);
		D3DXVec4Transform((Vec4*)&vec, (const Vec4*)&vec, (const LinearTransform*)&rot1);

		outVec3->x = vec.x;
		outVec3->y = vec.z;
		outVec3->z = vec.y;
	};

	bool w2s(const vec3_t& WorldPos, vec2_t* ScreenPos, vec2_t* BoxSize)
	{
		auto ViewOrig = get_camera_location();
		float angleX = xangle(ViewOrig, WorldPos);
		vec3_t posl, posr;
		rotation_point_alpha(-16.0f, 0.0f, 65.0f, -angleX + 90.0f, &posl);
		rotation_point_alpha(16.0f, 0.0f, -5.0f, -angleX + 90.0f, &posr);
		vec3_t vposl, vposr;
		vposl.x = WorldPos.x + posl.x;
		vposl.y = WorldPos.y + posl.y;
		vposl.z = WorldPos.z + posl.z;
    	vposr.x = WorldPos.x + posr.x;
		vposr.y = WorldPos.y + posr.y;
		vposr.z = WorldPos.z + posr.z;
		vec2_t screenPosl, screenPosr;
		if (!WorldToScreen(vposl, &screenPosl) || !WorldToScreen(vposr, &screenPosr))
		{
			return false;
		}
		BoxSize->x = abs(screenPosr.x - screenPosl.x);
		BoxSize->y = abs(screenPosl.y - screenPosr.y);
		ScreenPos->x = screenPosr.x - BoxSize->x;
		ScreenPos->y = screenPosl.y;
		return true;
	}

	bool head_to_screen(vec3_t pos, vec2_t* pos_out, int stance)
	{
		vec2_t w2s_head;

		pos.z += 55.f;
		if (!WorldToScreen(pos, &w2s_head))
			return false;

		else if (stance == sdk::CROUNCH)
		{
			pos.z -= 25.f;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}
		else if (stance == sdk::KNOCKED)
		{
			pos.z -= 28.f;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}
		else if (stance == sdk::PRONE)
		{
			pos.z -= 50.f;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}

		pos_out->x = w2s_head.x;
		pos_out->y = w2s_head.y;

		return true;
	}

	bool bones_to_screen(vec3_t* BonePosArray, vec2_t* ScreenPosArray, const long Count)
	{
		for (long i = 0; i < Count; ++i)
		{
			if (!WorldToScreen(BonePosArray[i], &ScreenPosArray[i]))
				return false;
		}
		return true;
	}

	bool is_valid_bone(vec3_t origin, vec3_t* bone, const long Count)
	{
		for (long i = 0; i < Count; ++i)
		{
			if (bone[i].distance_to(bone[i + 1]) > 41 && origin.distance_to(bone[i]) > 200) //118 ~= 3metres, 39~= 1metre
				return false;
		}
		return true;
	}

	bool in_game()
	{
		return *(int*)(g_data::base + offsets::game_mode) > 1;
	}

	int get_max_player_count()
	{
		return *(int*)(g_data::base + offsets::game_mode);
	}

	player_t get_player(int i)
	{
		uint64_t decryptedPtr = decryption::get_client_info();

		if (is_valid_ptr(decryptedPtr))
		{
			uint64_t client_info = decryption::get_client_info_base();

			if (is_valid_ptr(client_info))
			{
				return player_t(client_info + (i * offsets::player::size));
			}
		}
		return player_t(NULL);
	}

	player_t get_local_player()
	{
		uint64_t decryptedPtr = decryption::get_client_info();

		if (is_bad_ptr(decryptedPtr))
			return player_t(NULL);

		auto local_index = *(uintptr_t*)(decryptedPtr + offsets::local_index);
		if (is_bad_ptr(local_index))
			return player_t(NULL);

		auto index = *(int*)(local_index + offsets::local_index_pos);
		return get_player(index);
	}

	int local_index()
	{
		uint64_t decryptedPtr = decryption::get_client_info();

		if (is_valid_ptr(decryptedPtr))
		{
			auto local_index = *(uintptr_t*)(decryptedPtr + offsets::local_index);
			return *(int*)(local_index + offsets::local_index_pos);
		}
		return 0;
	}

	name_t* get_name_ptr(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + offsets::name_array);

		if (bgs)
		{
			name_t* pClientInfo = (name_t*)(bgs + offsets::name_array_pos + ((i + i * 8) << 4));
			if (pClientInfo)
			{
				return pClientInfo;
			}
		}
		return 0;
	}

	refdef_t* get_refdef()
	{
		uint32_t crypt_0 = *(uint32_t*)(g_data::base + offsets::refdef);
		uint32_t crypt_1 = *(uint32_t*)(g_data::base + offsets::refdef + 0x4);
		uint32_t crypt_2 = *(uint32_t*)(g_data::base + offsets::refdef + 0x8);
		// lower 32 bits
		uint32_t entry_1 = (uint32_t)(g_data::base + offsets::refdef);
		uint32_t entry_2 = (uint32_t)(g_data::base + offsets::refdef + 0x4);
		// decryption
		uint32_t _low = entry_1 ^ crypt_2;
		uint32_t _high = entry_2 ^ crypt_2;
		uint32_t low_bit = crypt_0 ^ _low * (_low + 2);
		uint32_t high_bit = crypt_1 ^ _high * (_high + 2);
		return (refdef_t*)(((uint64_t)high_bit << 32) + low_bit);
	}

	vec3_t get_camera_pos()
	{
		vec3_t pos = vec3_t{};
		auto camera_ptr = *(uint64_t*)(g_data::base + offsets::camera_base);
		if (is_bad_ptr(camera_ptr))return pos;
		pos = *(vec3_t*)(camera_ptr + offsets::camera_pos);
		return pos;
	}

	std::string get_player_name(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + offsets::name_array);

		if (bgs)
		{
			name_t* clientInfo_ptr = (name_t*)(bgs + offsets::name_array_pos + (i * 0xD0));
			int length = strlen(clientInfo_ptr->name);
			for (int j = 0; j < length; ++j)
			{
				char ch = clientInfo_ptr->name[j];
				bool is_english = ch >= 0 && ch <= 127;
				if (!is_english)
					return xorstr("Player");
			}
			return clientInfo_ptr->name;
		}
		return xorstr("Player");
	}

	bool get_bone_by_player_index(int i, int bone_id, vec3_t* Out_bone_pos)
	{
		uint64_t decrypted_ptr = decryption::get_bone_ptr();

		if (is_bad_ptr(decrypted_ptr))return false;

		unsigned short bone_index = decryption::get_bone_index(i);

		if (bone_index != 0)
		{
			uint64_t bone_ptr = *(uint64_t*)(decrypted_ptr + (bone_index * offsets::bone::index_struct_size) + 0xC0);

			if (is_bad_ptr(bone_ptr))return false;
			{
				uint64_t client_info = decryption::get_client_info();

				if (is_bad_ptr(client_info))return false;

				vec3_t bone_pos = *(vec3_t*)(bone_ptr + (bone_id * 0x20) + 0x10);

				vec3_t BasePos = *(vec3_t*)(client_info + offsets::bone::base_pos);

				bone_pos.x += BasePos.x;
				bone_pos.y += BasePos.y;
				bone_pos.z += BasePos.z;

				*Out_bone_pos = bone_pos;
				return true;
			}
		}
		return false;
	}

	void no_recoil()
	{
		uint64_t cl_info = decryption::get_client_info();

		if (cl_info)
		{
			// up, down
			uint64_t r12 = cl_info;
			r12 += offsets::recoil;
			uint64_t rsi = r12 + 0x4;
			DWORD edx = *(uint64_t*)(r12 + 0xC);
			DWORD ecx = (DWORD)r12;
			ecx ^= edx;
			DWORD eax = (DWORD)((uint64_t)ecx + 0x2);
			eax *= ecx;
			ecx = (DWORD)rsi;
			ecx ^= edx;
			DWORD udZero = eax;
			//left, right
			eax = (DWORD)((uint64_t)ecx + 0x2);
			eax *= ecx;
			DWORD lrZero = eax;
			*(DWORD*)(r12) = udZero;
			*(DWORD*)(rsi) = lrZero;
		}
	}

	//void enable_uav()
	//{
	//	auto cg_t = *(uint64_t*)(g_data::base + offsets::cg_t_ptr);
	//	const auto offset1 = 0x304;
	//	const auto enable = 33619969;

	//	if (sdk::in_game())
	//	{
	//		if (cg_t != 0)
	//		{
	//			*(int*)(cg_t + offset1) = enable;
	//		}
	//	}
	//}

	int get_player_health(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_data::base + offsets::name_array);

		if (is_valid_ptr(bgs))
		{
			name_t* pClientInfo = (name_t*)(bgs + offsets::name_array_pos + (i * 0xD0));

			if (is_valid_ptr(pClientInfo))
			{
				return pClientInfo->health;
			}
		}
		return 0;
	}

	void start_tick()
	{
		static DWORD lastTick = 0;
		DWORD t = GetTickCount();
		bUpdateTick = lastTick < t;

		if (bUpdateTick)
			lastTick = t + nTickTime;
	}

	void update_vel_map(int index, vec3_t vPos)
	{
		if (!bUpdateTick)
			return;

		velocityMap[index].delta = vPos - velocityMap[index].lastPos;
		velocityMap[index].lastPos = vPos;
	}

	void clear_vel_map()
	{
		if (!velocityMap.empty()) { velocityMap.clear(); }
	}

	vec3_t get_speed(int index)
	{
		return velocityMap[index].delta;
	}

	vec3_t get_prediction(int index, vec3_t source, vec3_t destination)
	{
		auto local_velocity = get_speed(local_index());
		auto target_velocity = get_speed(index);

		const auto distance = source.distance_to(destination);
		const auto travel_time = distance / Settings::aimbot::f_bullet_speed;
		auto pred_destination = destination + (target_velocity - local_velocity) * travel_time;
		pred_destination.z += 0.5f * m_to_units(9.8f) * travel_time * travel_time;

		return pred_destination;
	}

	vec3_t get_prediction(int index, vec3_t source, vec3_t bone, vec3_t position)
	{
		auto local_velocity = get_speed(local_index());
		auto target_velocity = get_speed(index);

		const auto distance = source.distance_to(position);
		const auto travel_time = distance / Settings::aimbot::f_bullet_speed;
		auto pred_destination = bone + (target_velocity - local_velocity) * travel_time;
		pred_destination.z += 0.5f * m_to_units(9.8f) * (travel_time * travel_time);

		return pred_destination;
	}

	int get_client_count()
	{
		auto cl_info = decryption::get_client_info();
		if (is_valid_ptr(cl_info))
		{
			auto client_ptr = *(uint64_t*)(cl_info + offsets::local_index);
			if (is_valid_ptr(client_ptr))
			{
				return *(int*)(client_ptr + 0x1C);
			}
		}

		return 0;
	}

	Result MidnightSolver(float a, float b, float c)
	{
		Result res;

		double subsquare = b * b - 4 * a * c;

		if (subsquare < 0)
		{
			res.hasResult = false;
			return res;
		}
		else
		{
			res.hasResult = true,
				res.a = (float)((-b + sqrt(subsquare)) / (2 * a));
			res.b = (float)((-b - sqrt(subsquare)) / (2 * a));
		}
		return res;
	}

	vec3_t prediction_solver(vec3_t local_pos, vec3_t position, int index, float bullet_speed)
	{
		vec3_t aimPosition = vec3_t();
		auto target_speed = get_speed(index);

		local_pos -= position;

		float a = (target_speed.x * target_speed.x) + (target_speed.y * target_speed.y) + (target_speed.z * target_speed.z) - ((bullet_speed * bullet_speed) * 100);
		float b = (-2 * local_pos.x * target_speed.x) + (-2 * local_pos.y * target_speed.y) + (-2 * local_pos.z * target_speed.z);
		float c = (local_pos.x * local_pos.x) + (local_pos.y * local_pos.y) + (local_pos.z * local_pos.z);

		local_pos += position;

		Result r = MidnightSolver(a, b, c);

		if (r.a >= 0 && !(r.b >= 0 && r.b < r.a))
		{
			aimPosition = position + target_speed * r.a;
		}
		else if (r.b >= 0)
		{
			aimPosition = position + target_speed * r.b;
		}

		return aimPosition;

	}

	uint64_t get_visible_base()
	{
		for (int32_t j = 4000; j >= 0; --j)
		{
			uint64_t n_index = (j + (j << 2)) << 0x6;
			uint64_t vis_base = *(uint64_t*)(g_data::base + offsets::distribute);

			if (is_bad_ptr(vis_base))
				continue;

			uint64_t vis_base_ptr = vis_base + n_index;
			uint64_t cmp_function = *(uint64_t*)(vis_base_ptr + 0x90);

			if (is_bad_ptr(cmp_function))
				continue;

			uint64_t about_visible = g_data::base + offsets::visible;
			if (cmp_function == about_visible)
			{
				return vis_base_ptr;
			}
		}
		return NULL;
	}

	// player class methods
	bool player_t::is_valid() {
		if (is_bad_ptr(address))return 0;

		return *(bool*)((uintptr_t)address + offsets::player::valid);
	}

	bool player_t::is_dead() {
		if (is_bad_ptr(address))return 0;

		auto dead1 = *(bool*)((uintptr_t)address + offsets::player::dead_1);
		auto dead2 = *(bool*)((uintptr_t)address + offsets::player::dead_2);
		return dead1 || dead2;
	}

	int player_t::team_id() {

		if (is_bad_ptr(address))return 0;
		return *(int*)((uintptr_t)address + offsets::player::team);
	}

	int player_t::get_stance() {

		auto ret = *(int*)((uintptr_t)address + offsets::player::stance);
		return ret;
	}

	vec3_t player_t::get_pos()
	{
		if (is_bad_ptr(address))return {};
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + offsets::player::pos);

		if (is_bad_ptr(local_pos_ptr))return{};
		else
			return *(vec3_t*)(local_pos_ptr + 0x40);
		return vec3_t{};
	}

	float player_t::get_rotation()
	{
		if (is_bad_ptr(address))return 0;
		auto local_pos_ptr = *(uintptr_t*)((uintptr_t)address + offsets::player::pos);

		if (is_bad_ptr(local_pos_ptr))return 0;

		auto rotation = *(float*)(local_pos_ptr + 0x58);

		if (rotation < 0)
			rotation = 360.0f - (rotation * -1);

		rotation += 90.0f;

		if (rotation >= 360.0f)
			rotation = rotation - 360.0f;

		return rotation;
	}

	int player_t::weapon_index()
	{
		if (is_bad_ptr(this->address))return 0;

		return *(int*)(this->address + offsets::player::weapon_index);

	}

	uint32_t player_t::get_index()
	{
		auto cl_info = decryption::get_client_info();
		if (is_valid_ptr(cl_info))
		{
			auto cl_info_base = decryption::get_client_info_base();
			if (is_valid_ptr(cl_info_base))
			{
				return ((uintptr_t)address - cl_info_base) / offsets::player::size;
			}
			return 0;
		}
	}

	bool player_t::is_visible()
	{
		if (is_valid_ptr(g_data::visible_base))
		{
			uint64_t VisibleList = *(uint64_t*)(g_data::visible_base + 0x108);
			if (is_bad_ptr(VisibleList))
				return false;

			uint64_t rdx = VisibleList + (player_t::get_index() * 9 + 0x14E) * 8;
			if (is_bad_ptr(rdx))
				return false;

			DWORD VisibleFlags = (rdx + 0x10) ^ *(DWORD*)(rdx + 0x14);
			if (is_bad_ptr(VisibleFlags))
				return false;

			DWORD v511 = VisibleFlags * (VisibleFlags + 2);
			if (is_bad_ptr(v511))
				return false;

			BYTE VisibleFlags1 = *(DWORD*)(rdx + 0x10) ^ v511 ^ BYTE1(v511);
			if (VisibleFlags1 == 3) {
				return true;
			}
		}
		return false;
	}

    auto loot::get_name() -> char*
    {
        auto ptr = *(uintptr_t*)(this + 0);
        if (is_bad_ptr(ptr))
            return NULL;

        return *(char**)(ptr);
    }

    auto loot::is_valid() -> bool
    {
        return *(char*)(this + 0xC8) == 1 && *(char*)(this + 0xC9) == 1;
    }

    auto loot::get_position() -> vec3_t
    {
        return *(vec3_t*)(this + 0x160);
    }

    auto get_loot_by_id(const uint32_t id) -> loot*
    {
        return (loot*)(g_data::base + 0x14EBF860) + (static_cast<uint64_t>(id) * 0x250);
    }

	//void CBuff(std::string newcmd)
	//{
	//	/*auto modbase = (uintptr_t)GetModuleHandleA(0);*/
	//	auto buff = *(uintptr_t*)(g_data::base + offsets::c_buff1);
	//	memcpy((void*)(buff), newcmd.data(), newcmd.length() + 1);
	//	*(int*)(g_data::base + offsets::c_buff2) = newcmd.length() + 1;

	//}

	//void CBuff(std::string newcmd)
	//{
	//	auto modbase = (uintptr_t)GetModuleHandleA(0);
	//	auto buff = *(uintptr_t*)(modbase + 0x159F3FF0); // 48 8D 05 ? ? ? ? B9 ? ? ? ? 48 03 F8 E8 ? ? ? ? 48 8B CD E8 ? ? ? ? 4C 63 4F 0C 8B F0 41 8D 14 01 3B 57 08 7C 32 9C 53
	//	memcpy((void*)(buff), newcmd.data(), newcmd.length() + 1);
	//	*(int*)(modbase + 0x159F3FFC) = newcmd.length() + 1; // 01 1D ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 41 B0 01
	//}
}

namespace decryption
{
    uint64_t get_client_info()
    {
        auto baseModuleAddr = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
        rbx = *(uintptr_t*)(baseModuleAddr + 0x14D5B0A8);
        if (!rbx)
            return rbx;
        rdx = Peb;              //mov rdx, gs:[rax]
        rax = rbx;              //mov rax, rbx
        rax >>= 0x20;           //shr rax, 0x20
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x12;           //shr rax, 0x12
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rax >>= 0x24;           //shr rax, 0x24
        rbx ^= rax;             //xor rbx, rax
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= *(uintptr_t*)(baseModuleAddr + 0x733E0EC);             //xor rcx, [0x000000000515F308]
        rax = 0x24390C5736146F5F;               //mov rax, 0x24390C5736146F5F
        rcx = _byteswap_uint64(rcx);            //bswap rcx
        rbx *= *(uintptr_t*)(rcx + 0xd);              //imul rbx, [rcx+0x0D]
        rbx += rdx;             //add rbx, rdx
        rbx *= rax;             //imul rbx, rax
        rax = 0x372979DA26E7BA9C;               //mov rax, 0x372979DA26E7BA9C
        rbx ^= rax;             //xor rbx, rax
        return rbx;
    }
    uint64_t get_client_info_base()
    {
        auto baseModuleAddr = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
        rax = *(uintptr_t*)(get_client_info() + 0xae820);
        if (!rax)
            return rax;
        rbx = Peb;              //mov rbx, gs:[rcx]
        rcx = rbx;              //mov rcx, rbx
        rcx <<= 0x1B;           //shl rcx, 0x1B
        rcx = _byteswap_uint64(rcx);            //bswap rcx
        rcx &= 0xF;
        auto clientSwitch = rcx;
        switch (rcx) {
        case 0:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE21174]
            r15 = baseModuleAddr + 0x79AD730A;              //lea r15, [0x00000000778F8472]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515F240]
            rcx = 0xE7D778D960F95853;               //mov rcx, 0xE7D778D960F95853
            rax *= rcx;             //imul rax, rcx
            rcx = 0x1C58685857ED1C87;               //mov rcx, 0x1C58685857ED1C87
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1F;           //shr rcx, 0x1F
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x3E;           //shr rcx, 0x3E
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1C;           //shr rcx, 0x1C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x38;           //shr rcx, 0x38
            rax ^= rcx;             //xor rax, rcx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rax += rdi;             //add rax, rdi
            rcx = 0xCAF837B01CB827CD;               //mov rcx, 0xCAF837B01CB827CD
            rax ^= rcx;             //xor rax, rcx
            rcx = r15;              //mov rcx, r15
            rcx = ~rcx;             //not rcx
            rcx += rbx;             //add rcx, rbx
            rax += rcx;             //add rax, rcx
            return rax;
        }
        case 1:
        {
            r11 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r11, [0x000000000515EDCA]
            rdi = baseModuleAddr + 0x1BE83557;              //lea rdi, [0x0000000019CA41CE]
            rdx = baseModuleAddr + 0x5AB832E0;              //lea rdx, [0x00000000589A3F0B]
            r15 = 0xDEC4BB0585BB5396;               //mov r15, 0xDEC4BB0585BB5396
            rax += r15;             //add rax, r15
            r15 = baseModuleAddr + 0x6A48A9BA;              //lea r15, [0x00000000682AB2CA]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r11;             //xor rcx, r11
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rbx;              //mov rcx, rbx
            rcx = ~rcx;             //not rcx
            rcx += rdx;             //add rcx, rdx
            rax ^= rcx;             //xor rax, rcx
            rcx = rdi;              //mov rcx, rdi
            rcx = ~rcx;             //not rcx
            rcx += rbx;             //add rcx, rbx
            rax ^= rcx;             //xor rax, rcx
            rcx = 0xABDCEBED1A7E084D;               //mov rcx, 0xABDCEBED1A7E084D
            rax *= rcx;             //imul rax, rcx
            rcx = rbx;              //mov rcx, rbx
            rcx *= r15;             //imul rcx, r15
            rdx = rbx;              //mov rdx, rbx
            rdx = ~rdx;             //not rdx
            rdx -= rcx;             //sub rdx, rcx
            rcx = baseModuleAddr + 0x28BB71A9;              //lea rcx, [0x00000000269D7D34]
            rcx = ~rcx;             //not rcx
            rax += rcx;             //add rax, rcx
            rax += rdx;             //add rax, rdx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1E;           //shr rcx, 0x1E
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x3C;           //shr rcx, 0x3C
            rax ^= rcx;             //xor rax, rcx
            return rax;
        }
        case 2:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE2089D]
            r11 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r11, [0x000000000515E95E]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x13;           //shr rcx, 0x13
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x26;           //shr rcx, 0x26
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0xD;            //shr rcx, 0x0D
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1A;           //shr rcx, 0x1A
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x34;           //shr rcx, 0x34
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x1D6DF4CEAC2D586B;               //mov rcx, 0x1D6DF4CEAC2D586B
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x639C3CFFB1FBF058;               //mov rcx, 0x639C3CFFB1FBF058
            rax += rcx;             //add rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x16;           //shr rcx, 0x16
            rax ^= rcx;             //xor rax, rcx
            rdx = rdi + 0xaeee;             //lea rdx, [rdi+0xAEEE]
            rdx += rbx;             //add rdx, rbx
            r8 = 0;                 //and r8, 0xFFFFFFFFC0000000
            r8 = _rotl64(r8, 0x10);                 //rol r8, 0x10
            r8 ^= r11;              //xor r8, r11
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x2C;           //shr rcx, 0x2C
            rdx ^= rcx;             //xor rdx, rcx
            r8 = _byteswap_uint64(r8);              //bswap r8
            rax ^= rdx;             //xor rax, rdx
            rax *= *(uintptr_t*)(r8 + 0xb);               //imul rax, [r8+0x0B]
            rcx = 0x8B5F856C6ECF066B;               //mov rcx, 0x8B5F856C6ECF066B
            rax *= rcx;             //imul rax, rcx
            return rax;
        }
        case 3:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE203A7]
            r15 = baseModuleAddr + 0x5035;          //lea r15, [0xFFFFFFFFFDE253D0]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515E473]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rdi + 0x124fe71b;                 //lea rcx, [rdi+0x124FE71B]
            rcx += rbx;             //add rcx, rbx
            rax ^= rcx;             //xor rax, rcx
            rcx = 0xADECBF21D93AA017;               //mov rcx, 0xADECBF21D93AA017
            rax *= rcx;             //imul rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0xE;            //shr rcx, 0x0E
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1C;           //shr rcx, 0x1C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x38;           //shr rcx, 0x38
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x626177032A10C415;               //mov rcx, 0x626177032A10C415
            rax -= rcx;             //sub rax, rcx
            rax -= rbx;             //sub rax, rbx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x13;           //shr rcx, 0x13
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x26;           //shr rcx, 0x26
            rax ^= rcx;             //xor rax, rcx
            rcx = rbx;              //mov rcx, rbx
            rcx ^= r15;             //xor rcx, r15
            rax -= rcx;             //sub rax, rcx
            return rax;
        }
        case 4:
        {
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r10, [0x000000000515DFCC]
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1FE85]
            rdx = baseModuleAddr + 0xD4BC;          //lea rdx, [0xFFFFFFFFFDE2D2D2]
            rcx = 0x8DCD567436EE272E;               //mov rcx, 0x8DCD567436EE272E
            rax ^= rcx;             //xor rax, rcx
            rcx = 0xF5FA4D56A78416F;                //mov rcx, 0xF5FA4D56A78416F
            rax *= rcx;             //imul rax, rcx
            rcx = rdx;              //mov rcx, rdx
            rcx = ~rcx;             //not rcx
            rcx ^= rbx;             //xor rcx, rbx
            rax += rcx;             //add rax, rcx
            rcx = rdi + 0x1ca67e94;                 //lea rcx, [rdi+0x1CA67E94]
            rcx += rbx;             //add rcx, rbx
            rax += rcx;             //add rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x16;           //shr rcx, 0x16
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x2C;           //shr rcx, 0x2C
            rax ^= rcx;             //xor rax, rcx
            rax -= rdi;             //sub rax, rdi
            rax += rbx;             //add rax, rbx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            return rax;
        }
        case 5:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1F997]
            r15 = baseModuleAddr + 0x3C09;          //lea r15, [0xFFFFFFFFFDE23594]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515DA55]
            uintptr_t RSP_0x58;
            RSP_0x58 = 0xDBE417EAC23E7A62;          //mov rcx, 0xDBE417EAC23E7A62 : RSP+0x58
            rax ^= RSP_0x58;                //xor rax, [rsp+0x58]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = 0xE76D4193D03DE4E7;               //mov rcx, 0xE76D4193D03DE4E7
            rax *= rcx;             //imul rax, rcx
            rcx = 0x19492EFD6783BA0F;               //mov rcx, 0x19492EFD6783BA0F
            rax += rbx;             //add rax, rbx
            rax += rcx;             //add rax, rcx
            rax += rdi;             //add rax, rdi
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x11;           //shr rcx, 0x11
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x22;           //shr rcx, 0x22
            rax ^= rcx;             //xor rax, rcx
            rax ^= rdi;             //xor rax, rdi
            rcx = rbx;              //mov rcx, rbx
            rcx *= r15;             //imul rcx, r15
            rax -= rcx;             //sub rax, rcx
            return rax;
        }
        case 6:
        {
            rdi = baseModuleAddr + 0x25A1B7D2;              //lea rdi, [0x000000002383AD1E]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515D5F5]
            rcx = 0x342A37B03A07B5BD;               //mov rcx, 0x342A37B03A07B5BD
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x4B20FE66DFF80FEB;               //mov rcx, 0x4B20FE66DFF80FEB
            rax *= rcx;             //imul rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0xB;            //shr rcx, 0x0B
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x16;           //shr rcx, 0x16
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x2C;           //shr rcx, 0x2C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x13;           //shr rcx, 0x13
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x26;           //shr rcx, 0x26
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x2F747A85A49E9847;               //mov rcx, 0x2F747A85A49E9847
            rax += rcx;             //add rax, rcx
            rcx = rbx;              //mov rcx, rbx
            rcx ^= rdi;             //xor rcx, rdi
            rax += rcx;             //add rax, rcx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0xC;            //shr rcx, 0x0C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x18;           //shr rcx, 0x18
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x30;           //shr rcx, 0x30
            rax ^= rcx;             //xor rax, rcx
            return rax;
        }
        case 7:
        {
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r10, [0x000000000515D0E6]
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1EF9F]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1C;           //shr rcx, 0x1C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x38;           //shr rcx, 0x38
            rax ^= rcx;             //xor rax, rcx
            rdx = rbx;              //mov rdx, rbx
            rdx = ~rdx;             //not rdx
            rcx = baseModuleAddr + 0x29B85665;              //lea rcx, [0x00000000279A42F0]
            rcx = ~rcx;             //not rcx
            rdx *= rcx;             //imul rdx, rcx
            rax += rdx;             //add rax, rdx
            rdx = baseModuleAddr + 0xAA38;          //lea rdx, [0xFFFFFFFFFDE29961]
            rcx = rbx;              //mov rcx, rbx
            rcx ^= rdx;             //xor rcx, rdx
            rax += rcx;             //add rax, rcx
            rcx = rbx;              //mov rcx, rbx
            rcx = ~rcx;             //not rcx
            rcx -= rdi;             //sub rcx, rdi
            rcx += 0xFFFFFFFFCD530BC2;              //add rcx, 0xFFFFFFFFCD530BC2
            rax += rcx;             //add rax, rcx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rcx = *(uintptr_t*)(rcx + 0xb);               //mov rcx, [rcx+0x0B]
            uintptr_t RSP_0x78;
            RSP_0x78 = 0xB113AAEED17D8F17;          //mov rcx, 0xB113AAEED17D8F17 : RSP+0x78
            rcx *= RSP_0x78;                //imul rcx, [rsp+0x78]
            rax *= rcx;             //imul rax, rcx
            rcx = 0x6D0A140B1082247F;               //mov rcx, 0x6D0A140B1082247F
            rax -= rcx;             //sub rax, rcx
            return rax;
        }
        case 8:
        {
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515CC04]
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1EABD]
            r11 = baseModuleAddr + 0x7FB690C2;              //lea r11, [0x000000007D987B74]
            rcx = 0x790E6B551E2A8B0D;               //mov rcx, 0x790E6B551E2A8B0D
            rax *= rcx;             //imul rax, rcx
            rax -= rdi;             //sub rax, rdi
            rcx = 0x1894DC56F153F3B7;               //mov rcx, 0x1894DC56F153F3B7
            rax *= rcx;             //imul rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x16;           //shr rcx, 0x16
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x2C;           //shr rcx, 0x2C
            rax ^= rcx;             //xor rax, rcx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rbx;              //mov rcx, rbx
            rcx *= r11;             //imul rcx, r11
            rax -= rcx;             //sub rax, rcx
            rax ^= rdi;             //xor rax, rdi
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x28;           //shr rcx, 0x28
            rax ^= rcx;             //xor rax, rcx
            return rax;
        }
        case 9:
        {
            r10 = baseModuleAddr + 0x32B9C2D5;              //lea r10, [0x00000000309BA8B7]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515C6D7]
            rcx = rbx;              //mov rcx, rbx
            rcx = ~rcx;             //not rcx
            rcx *= r10;             //imul rcx, r10
            rcx += rbx;             //add rcx, rbx
            rax += rcx;             //add rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x13;           //shr rcx, 0x13
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x26;           //shr rcx, 0x26
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x58CB70C68EEEC667;               //mov rcx, 0x58CB70C68EEEC667
            rax *= rcx;             //imul rax, rcx
            rcx = 0xB941325C9158D84C;               //mov rcx, 0xB941325C9158D84C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x20;           //shr rcx, 0x20
            rax ^= rcx;             //xor rax, rcx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            return rax;
        }
        case 10:
        {
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r10, [0x000000000515C33C]
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1E1F5]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x26;           //shr rcx, 0x26
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x29CC2632F4B1160A;               //mov rcx, 0x29CC2632F4B1160A
            rax += rcx;             //add rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x19;           //shr rcx, 0x19
            rax ^= rcx;             //xor rax, rcx
            rdx = rax;              //mov rdx, rax
            rdx >>= 0x32;           //shr rdx, 0x32
            rdx ^= rax;             //xor rdx, rax
            rcx = rbx;              //mov rcx, rbx
            rcx = ~rcx;             //not rcx
            rax = baseModuleAddr + 0x44D3CAD3;              //lea rax, [0x0000000042B5AB29]
            rax ^= rcx;             //xor rax, rcx
            rax += rdx;             //add rax, rdx
            rcx = 0xB14689E750ABA411;               //mov rcx, 0xB14689E750ABA411
            rax *= rcx;             //imul rax, rcx
            rcx = baseModuleAddr + 0x1EBE;          //lea rcx, [0xFFFFFFFFFDE1FFCB]
            rcx = ~rcx;             //not rcx
            rcx -= rbx;             //sub rcx, rbx
            rax += rcx;             //add rax, rcx
            rax ^= rdi;             //xor rax, rdi
            return rax;
        }
        case 11:
        {
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515BE84]
            rax ^= rbx;             //xor rax, rbx
            rcx = 0x78E161DBBE8754C;                //mov rcx, 0x78E161DBBE8754C
            rax += rcx;             //add rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0xB;            //shr rcx, 0x0B
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x16;           //shr rcx, 0x16
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x2C;           //shr rcx, 0x2C
            rax ^= rcx;             //xor rax, rcx
            rcx = baseModuleAddr + 0x4B2DAB9E;              //lea rcx, [0x00000000490F86C7]
            rax += rbx;             //add rax, rbx
            rax += rcx;             //add rax, rcx
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = 0xA23ABFC71A2C9033;               //mov rcx, 0xA23ABFC71A2C9033
            rax *= rcx;             //imul rax, rcx
            rax ^= rbx;             //xor rax, rbx
            return rax;
        }
        case 12:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1D974]
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r10, [0x000000000515BA33]
            rax ^= rdi;             //xor rax, rdi
            rax -= rdi;             //sub rax, rdi
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1C;           //shr rcx, 0x1C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x38;           //shr rcx, 0x38
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x675127872EFC0D47;               //mov rcx, 0x675127872EFC0D47
            rax *= rcx;             //imul rax, rcx
            rcx = 0x48D2B5DC38AA1660;               //mov rcx, 0x48D2B5DC38AA1660
            rax -= rcx;             //sub rax, rcx
            rax ^= rbx;             //xor rax, rbx
            rcx = 0xA8510779F48C446F;               //mov rcx, 0xA8510779F48C446F
            rax *= rcx;             //imul rax, rcx
            return rax;
        }
        case 13:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1D512]
            r15 = baseModuleAddr + 0x8928;          //lea r15, [0xFFFFFFFFFDE25E2E]
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r10, [0x000000000515B5E3]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1E;           //shr rcx, 0x1E
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x3C;           //shr rcx, 0x3C
            rcx ^= rax;             //xor rcx, rax
            rax = 0x22251DD2A30DC968;               //mov rax, 0x22251DD2A30DC968
            rax += rcx;             //add rax, rcx
            rax += rdi;             //add rax, rdi
            rcx = rbx;              //mov rcx, rbx
            rcx *= r15;             //imul rcx, r15
            rax -= rcx;             //sub rax, rcx
            rcx = 0x633D2DAD90803456;               //mov rcx, 0x633D2DAD90803456
            rax ^= rcx;             //xor rax, rcx
            rcx = 0xD4011F511C76E3E5;               //mov rcx, 0xD4011F511C76E3E5
            rax *= rcx;             //imul rax, rcx
            rax += rdi;             //add rax, rdi
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            return rax;
        }
        case 14:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1D080]
            r11 = *(uintptr_t*)(baseModuleAddr + 0x733E134);              //mov r11, [0x000000000515B13B]
            rdx = baseModuleAddr + 0x6401676D;              //lea rdx, [0x0000000061E335F7]
            rdx = ~rdx;             //not rdx
            //failed to translate: inc rdx
            rdx += rbx;             //add rdx, rbx
            rax ^= rdx;             //xor rax, rdx
            r8 = 0;                 //and r8, 0xFFFFFFFFC0000000
            r8 = _rotl64(r8, 0x10);                 //rol r8, 0x10
            r8 ^= r11;              //xor r8, r11
            r8 = _byteswap_uint64(r8);              //bswap r8
            rax *= *(uintptr_t*)(r8 + 0xb);               //imul rax, [r8+0x0B]
            rcx = rdi + 0x1770;             //lea rcx, [rdi+0x1770]
            rcx += rbx;             //add rcx, rbx
            rax ^= rcx;             //xor rax, rcx
            rax ^= rbx;             //xor rax, rbx
            rcx = 0x60D4C4A5EC2A1D;                 //mov rcx, 0x60D4C4A5EC2A1D
            rax *= rcx;             //imul rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x26;           //shr rcx, 0x26
            rax ^= rcx;             //xor rax, rcx
            rcx = 0xEE43A59951B56B0D;               //mov rcx, 0xEE43A59951B56B0D
            rax *= rcx;             //imul rax, rcx
            rcx = 0x2A34A99B5497CB4B;               //mov rcx, 0x2A34A99B5497CB4B
            rax -= rcx;             //sub rax, rcx
            return rax;
        }
        case 15:
        {
            rdi = baseModuleAddr;           //lea rdi, [0xFFFFFFFFFDE1CC31]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E134);               //mov r9, [0x000000000515ACD1]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r9;              //xor rcx, r9
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0xb);              //imul rax, [rcx+0x0B]
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x28;           //shr rcx, 0x28
            rax ^= rcx;             //xor rax, rcx
            rax -= rdi;             //sub rax, rdi
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x1E;           //shr rcx, 0x1E
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x3C;           //shr rcx, 0x3C
            rax ^= rcx;             //xor rax, rcx
            rcx = 0x76841B01FF4968ED;               //mov rcx, 0x76841B01FF4968ED
            rax *= rcx;             //imul rax, rcx
            rcx = 0x2981179F151DBEED;               //mov rcx, 0x2981179F151DBEED
            rax -= rcx;             //sub rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x3;            //shr rcx, 0x03
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x6;            //shr rcx, 0x06
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0xC;            //shr rcx, 0x0C
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x18;           //shr rcx, 0x18
            rax ^= rcx;             //xor rax, rcx
            rcx = rax;              //mov rcx, rax
            rcx >>= 0x30;           //shr rcx, 0x30
            rax ^= rcx;             //xor rax, rcx
            return rax;
        }
        }
    }
    uint64_t get_bone_ptr()
    {
        auto baseModuleAddr = g_data::base;
        auto Peb = __readgsqword(0x60);
        uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
        rdx = *(uintptr_t*)(baseModuleAddr + 0x12C49EA8);
        if (!rdx)
            return rdx;
        rbx = Peb;              //mov rbx, gs:[rax]
        rax = rbx;              //mov rax, rbx
        rax = _rotl64(rax, 0x25);               //rol rax, 0x25
        rax &= 0xF;
        auto clientSwitch = rax;
        switch (rax) {
        case 0:
        {
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA62D8B]
            r12 = baseModuleAddr + 0xF6DB;          //lea r12, [0xFFFFFFFFFDA7245B]
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004DA0F29]
            rdx += r11;             //add rdx, r11
            rcx = r12;              //mov rcx, r12
            rcx = ~rcx;             //not rcx
            rcx ^= rbx;             //xor rcx, rbx
            rdx += rcx;             //add rdx, rcx
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = 0x44324E364FBC28A0;               //mov rax, 0x44324E364FBC28A0
            rdx ^= rax;             //xor rdx, rax
            rax = rbx;              //mov rax, rbx
            uintptr_t RSP_0x60;
            RSP_0x60 = baseModuleAddr + 0xE3F9;             //lea rax, [0xFFFFFFFFFDA71124] : RSP+0x60
            rax *= RSP_0x60;                //imul rax, [rsp+0x60]
            rdx += rax;             //add rdx, rax
            rax = 0xD5BDAD246639A35D;               //mov rax, 0xD5BDAD246639A35D
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = 0x1D2ADBD9E0A3FF47;               //mov rax, 0x1D2ADBD9E0A3FF47
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 1:
        {
            //failed to translate: pop rbx
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004DA0A4B]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA6281C]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3;            //shr rax, 0x03
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x6;            //shr rax, 0x06
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xC;            //shr rax, 0x0C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x18;           //shr rax, 0x18
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x30;           //shr rax, 0x30
            rdx ^= rax;             //xor rdx, rax
            rax = 0x40543B285AD8C365;               //mov rax, 0x40543B285AD8C365
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x10;           //shr rax, 0x10
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            rdx += rbx;             //add rdx, rbx
            rcx = rbx;              //mov rcx, rbx
            rcx = ~rcx;             //not rcx
            rax = baseModuleAddr + 0xE027;          //lea rax, [0xFFFFFFFFFDA703DF]
            rdx += rax;             //add rdx, rax
            rdx += rcx;             //add rdx, rcx
            r15 = 0x9072B1618CFFA63C;               //mov r15, 0x9072B1618CFFA63C
            rdx += r15;             //add rdx, r15
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = rbx;              //mov rax, rbx
            rax = ~rax;             //not rax
            rax -= r11;             //sub rax, r11
            rax -= 0x465F;          //sub rax, 0x465F
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 2:
        {
            //failed to translate: pop rbx
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004DA0482]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA62247]
            r15 = baseModuleAddr + 0xF2E4;          //lea r15, [0xFFFFFFFFFDA71513]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1F;           //shr rax, 0x1F
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3E;           //shr rax, 0x3E
            rdx ^= rax;             //xor rdx, rax
            rax = rbx;              //mov rax, rbx
            rax = ~rax;             //not rax
            rax ^= r15;             //xor rax, r15
            rdx -= rax;             //sub rdx, rax
            rax = 0xD66687D3B4EE1137;               //mov rax, 0xD66687D3B4EE1137
            rdx *= rax;             //imul rdx, rax
            rdx -= r11;             //sub rdx, r11
            rdx += 0xFFFFFFFFFFFF9DC6;              //add rdx, 0xFFFFFFFFFFFF9DC6
            rdx += rbx;             //add rdx, rbx
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = 0x373443B78C6B08FE;               //mov rax, 0x373443B78C6B08FE
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rcx = rdx;              //mov rcx, rdx
            rax >>= 0x1F;           //shr rax, 0x1F
            rcx ^= rax;             //xor rcx, rax
            rdx = rcx;              //mov rdx, rcx
            rdx >>= 0x3E;           //shr rdx, 0x3E
            rdx ^= rcx;             //xor rdx, rcx
            rax = 0xA3D2A2D7A8605359;               //mov rax, 0xA3D2A2D7A8605359
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 3:
        {
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA61DED]
            r15 = baseModuleAddr + 0x8D8C;          //lea r15, [0xFFFFFFFFFDA6AB61]
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9FFC7]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rdx ^= r11;             //xor rdx, r11
            rax = 0x898D335C66A77E67;               //mov rax, 0x898D335C66A77E67
            rdx *= rax;             //imul rdx, rax
            rax = 0x4F10AB227F96615F;               //mov rax, 0x4F10AB227F96615F
            rdx += rax;             //add rdx, rax
            rdx += rbx;             //add rdx, rbx
            rax = rdx;              //mov rax, rdx
            rax >>= 0x17;           //shr rax, 0x17
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2E;           //shr rax, 0x2E
            rdx ^= rax;             //xor rdx, rax
            rax = rbx;              //mov rax, rbx
            rax *= r15;             //imul rax, r15
            rdx += rax;             //add rdx, rax
            return rdx;
        }
        case 4:
        {
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9FC77]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA61A48]
            rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
            rax = rdx;              //mov rax, rdx
            rax >>= 0x27;           //shr rax, 0x27
            rax ^= rdx;             //xor rax, rdx
            rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
            rcx ^= r10;             //xor rcx, r10
            rcx = _byteswap_uint64(rcx);            //bswap rcx
            rax *= *(uintptr_t*)(rcx + 0x7);              //imul rax, [rcx+0x07]
            rdx = rax + r11 * 2;            //lea rdx, [rax+r11*2]
            rax = 0x8416974097B7C3F;                //mov rax, 0x8416974097B7C3F
            rdx -= rax;             //sub rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1C;           //shr rax, 0x1C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x38;           //shr rax, 0x38
            rdx ^= rax;             //xor rdx, rax
            rax = 0x6A13A6D3EE76449E;               //mov rax, 0x6A13A6D3EE76449E
            rdx += rax;             //add rdx, rax
            rax = 0x254B6A06E32651D1;               //mov rax, 0x254B6A06E32651D1
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 5:
        {
            r15 = baseModuleAddr + 0x75072C68;              //lea r15, [0x0000000072AD4143]
            r12 = baseModuleAddr + 0xA5FD;          //lea r12, [0xFFFFFFFFFDA6BACC]
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9F67D]
            rdx -= rbx;             //sub rdx, rbx
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x6;            //shr rax, 0x06
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xC;            //shr rax, 0x0C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x18;           //shr rax, 0x18
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x30;           //shr rax, 0x30
            rdx ^= rax;             //xor rdx, rax
            rcx = r12;              //mov rcx, r12
            rcx = ~rcx;             //not rcx
            rax = rbx;              //mov rax, rbx
            rax = ~rax;             //not rax
            rcx *= rax;             //imul rcx, rax
            rdx ^= rcx;             //xor rdx, rcx
            rax = r15;              //mov rax, r15
            rax ^= rbx;             //xor rax, rbx
            rdx -= rax;             //sub rdx, rax
            rax = 0xDED2D97E5A832A99;               //mov rax, 0xDED2D97E5A832A99
            rdx *= rax;             //imul rdx, rax
            rax = 0xFF62BBA477FC8554;               //mov rax, 0xFF62BBA477FC8554
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x4;            //shr rax, 0x04
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x8;            //shr rax, 0x08
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x10;           //shr rax, 0x10
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            return rdx;
        }
        case 6:
        {
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA60EA0]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E228);               //mov r9, [0x0000000004D9F039]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x11;           //shr rax, 0x11
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x22;           //shr rax, 0x22
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rdx -= rbx;             //sub rdx, rbx
            rax = 0x4DC5DF6679FA6649;               //mov rax, 0x4DC5DF6679FA6649
            rdx *= rax;             //imul rdx, rax
            rax = 0x33B40203FA70AFF3;               //mov rax, 0x33B40203FA70AFF3
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1B;           //shr rax, 0x1B
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x36;           //shr rax, 0x36
            rdx ^= rax;             //xor rdx, rax
            rax = 0x204D6B6F19BEF978;               //mov rax, 0x204D6B6F19BEF978
            rdx += rax;             //add rdx, rax
            rax = 0xFFFFFFFFC15A9585;               //mov rax, 0xFFFFFFFFC15A9585
            rax -= rbx;             //sub rax, rbx
            rax -= r11;             //sub rax, r11
            rdx += rax;             //add rdx, rax
            return rdx;
        }
        case 7:
        {
            //failed to translate: pop rdx
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9EA9F]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA60870]
            r12 = baseModuleAddr + 0x485B58BE;              //lea r12, [0x0000000046016116]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x1D;           //shr rax, 0x1D
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3A;           //shr rax, 0x3A
            rdx ^= rax;             //xor rdx, rax
            rax = 0x6F06391CA62554B3;               //mov rax, 0x6F06391CA62554B3
            rdx += rax;             //add rdx, rax
            rax = r12;              //mov rax, r12
            rax ^= rbx;             //xor rax, rbx
            rdx ^= rax;             //xor rdx, rax
            rdx += r11;             //add rdx, r11
            rdx ^= rbx;             //xor rdx, rbx
            rax = rdx;              //mov rax, rdx
            rax >>= 0x3;            //shr rax, 0x03
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x6;            //shr rax, 0x06
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0xC;            //shr rax, 0x0C
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x18;           //shr rax, 0x18
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x30;           //shr rax, 0x30
            rdx ^= rax;             //xor rdx, rax
            rax = 0x72DF7466627095D;                //mov rax, 0x72DF7466627095D
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 8:
        {
            //failed to translate: pop rbx
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9E53D]
            r15 = baseModuleAddr + 0xC864;          //lea r15, [0xFFFFFFFFFDA6CB66]
            r12 = baseModuleAddr + 0x66699E6E;              //lea r12, [0x00000000640FA164]
            rcx = rbx + 0x1;                //lea rcx, [rbx+0x01]
            rax = rbx;              //mov rax, rbx
            rcx *= r15;             //imul rcx, r15
            rax = ~rax;             //not rax
            rax += r12;             //add rax, r12
            rdx += rcx;             //add rdx, rcx
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x25;           //shr rax, 0x25
            rdx ^= rax;             //xor rdx, rax
            rax = 0x385438F937E1A37F;               //mov rax, 0x385438F937E1A37F
            rdx += rax;             //add rdx, rax
            rax = 0x3891E08FCA7A1E9B;               //mov rax, 0x3891E08FCA7A1E9B
            rdx *= rax;             //imul rdx, rax
            rdx -= rbx;             //sub rdx, rbx
            return rdx;
        }
        case 9:
        {
            //failed to translate: pop rdx
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9E01F]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA5FDF0]
            r12 = baseModuleAddr + 0x5CE4254E;              //lea r12, [0x000000005A8A2326]
            rdx -= r11;             //sub rdx, r11
            rax = baseModuleAddr + 0x61B91EBC;              //lea rax, [0x000000005F5F19AC]
            rax += rbx;             //add rax, rbx
            rdx += rax;             //add rdx, rax
            rax = 0x1C80DB5F061C9E27;               //mov rax, 0x1C80DB5F061C9E27
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x11;           //shr rax, 0x11
            rdx ^= rax;             //xor rdx, rax
            rcx = rbx;              //mov rcx, rbx
            rcx = ~rcx;             //not rcx
            rcx *= r12;             //imul rcx, r12
            rax = rdx;              //mov rax, rdx
            rax >>= 0x22;           //shr rax, 0x22
            rcx ^= rax;             //xor rcx, rax
            rdx ^= rcx;             //xor rdx, rcx
            rax = 0x29F30BEF63B7766B;               //mov rax, 0x29F30BEF63B7766B
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = 0xD9FF416E6B54ECB3;               //mov rax, 0xD9FF416E6B54ECB3
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 10:
        {
            r15 = baseModuleAddr + 0x602E;          //lea r15, [0xFFFFFFFFFDA658F6]
            r12 = baseModuleAddr + 0x6F40FD8C;              //lea r12, [0x000000006CE6F648]
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9DA85]
            rax = r15;              //mov rax, r15
            rax = ~rax;             //not rax
            rax ^= rbx;             //xor rax, rbx
            rdx ^= rax;             //xor rdx, rax
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x22;           //shr rax, 0x22
            rdx ^= rax;             //xor rdx, rax
            rax = rbx;              //mov rax, rbx
            rax = ~rax;             //not rax
            rcx = r12;              //mov rcx, r12
            rdx += rax;             //add rdx, rax
            rcx = ~rcx;             //not rcx
            rdx += rcx;             //add rdx, rcx
            rax = 0x1A0C5F96CABC0762;               //mov rax, 0x1A0C5F96CABC0762
            rdx += rax;             //add rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x11;           //shr rax, 0x11
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x22;           //shr rax, 0x22
            rdx ^= rax;             //xor rdx, rax
            rax = 0x193CFAAF49813549;               //mov rax, 0x193CFAAF49813549
            rdx *= rax;             //imul rdx, rax
            rax = 0x17DB0B5AABFD63E0;               //mov rax, 0x17DB0B5AABFD63E0
            rdx += rax;             //add rdx, rax
            return rdx;
        }
        case 11:
        {
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA5F301]
            rcx = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov rcx, [0x0000000004D9D4CC]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x4;            //shr rax, 0x04
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x8;            //shr rax, 0x08
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x10;           //shr rax, 0x10
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x20;           //shr rax, 0x20
            rdx ^= rax;             //xor rdx, rax
            rax = 0xDF58F15B0924441E;               //mov rax, 0xDF58F15B0924441E
            rdx ^= rax;             //xor rdx, rax
            rax = baseModuleAddr + 0x111AD416;              //lea rax, [0x000000000EC0C638]
            rax += rbx;             //add rax, rbx
            rdx += rax;             //add rdx, rax
            rax = 0x347B44CC367D75D5;               //mov rax, 0x347B44CC367D75D5
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x13;           //shr rax, 0x13
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rdx ^= r11;             //xor rdx, r11
            uintptr_t RSP_0x30;
            RSP_0x30 = 0xB7DD2E360BE2691;           //mov rax, 0xB7DD2E360BE2691 : RSP+0x30
            rdx ^= RSP_0x30;                //xor rdx, [rsp+0x30]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= rcx;             //xor rax, rcx
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            return rdx;
        }
        case 12:
        {
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA5EDC4]
            r12 = baseModuleAddr + 0x7F08;          //lea r12, [0xFFFFFFFFFDA66CB4]
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E228);               //mov r9, [0x0000000004D9CF8E]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rdx -= rbx;             //sub rdx, rbx
            rdx -= r11;             //sub rdx, r11
            rax = rbx + r12 * 1;            //lea rax, [rbx+r12*1]
            rdx ^= rax;             //xor rdx, rax
            rax = 0x1B89EDE38FF88EA7;               //mov rax, 0x1B89EDE38FF88EA7
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x21;           //shr rax, 0x21
            rdx ^= rax;             //xor rdx, rax
            rax = 0x50516B974845C4D8;               //mov rax, 0x50516B974845C4D8
            rdx ^= rax;             //xor rdx, rax
            rax = 0x4B4E8A3B0B0F7238;               //mov rax, 0x4B4E8A3B0B0F7238
            rdx -= rax;             //sub rdx, rax
            return rdx;
        }
        case 13:
        {
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9CB84]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA5E955]
            r12 = baseModuleAddr + 0x1E9BB20C;              //lea r12, [0x000000001C419B49]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = rbx;              //mov rax, rbx
            rax = ~rax;             //not rax
            rax *= r12;             //imul rax, r12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x14;           //shr rax, 0x14
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x28;           //shr rax, 0x28
            rdx ^= rax;             //xor rdx, rax
            rax = 0x23B349880703DC53;               //mov rax, 0x23B349880703DC53
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x13;           //shr rax, 0x13
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x26;           //shr rax, 0x26
            rdx ^= rax;             //xor rdx, rax
            rdx ^= r11;             //xor rdx, r11
            rax = 0xA1C7EACC8CEC0475;               //mov rax, 0xA1C7EACC8CEC0475
            rdx += rax;             //add rdx, rax
            rdx += r11;             //add rdx, r11
            return rdx;
        }
        case 14:
        {
            r9 = *(uintptr_t*)(baseModuleAddr + 0x733E228);               //mov r9, [0x0000000004D9C661]
            rax = rdx;              //mov rax, rdx
            rax >>= 0x9;            //shr rax, 0x09
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x12;           //shr rax, 0x12
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x24;           //shr rax, 0x24
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x27;           //shr rax, 0x27
            rdx ^= rax;             //xor rdx, rax
            rdx -= rbx;             //sub rdx, rbx
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r9;              //xor rax, r9
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rax = 0xBC51033E19C6335;                //mov rax, 0xBC51033E19C6335
            rdx *= rax;             //imul rdx, rax
            rax = 0x35603089FD1AC6EF;               //mov rax, 0x35603089FD1AC6EF
            rdx += rax;             //add rdx, rax
            rax = baseModuleAddr + 0x284D5BF6;              //lea rax, [0x0000000025F33C99]
            rax += rbx;             //add rax, rbx
            rdx += rax;             //add rdx, rax
            rax = 0x7B695ECDA0EC2BF5;               //mov rax, 0x7B695ECDA0EC2BF5
            rdx *= rax;             //imul rdx, rax
            return rdx;
        }
        case 15:
        {
            r10 = *(uintptr_t*)(baseModuleAddr + 0x733E228);              //mov r10, [0x0000000004D9C1C8]
            r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDA5DF8D]
            rax = 0;                //and rax, 0xFFFFFFFFC0000000
            rax = _rotl64(rax, 0x10);               //rol rax, 0x10
            rax ^= r10;             //xor rax, r10
            rax = _byteswap_uint64(rax);            //bswap rax
            rdx *= *(uintptr_t*)(rax + 0x7);              //imul rdx, [rax+0x07]
            rdx -= r11;             //sub rdx, r11
            rax = rdx;              //mov rax, rdx
            rax >>= 0x15;           //shr rax, 0x15
            rdx ^= rax;             //xor rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x2A;           //shr rax, 0x2A
            rdx ^= rax;             //xor rdx, rax
            rax = rbx;              //mov rax, rbx
            rax -= r11;             //sub rax, r11
            rax += 0xFFFFFFFFFFFFD083;              //add rax, 0xFFFFFFFFFFFFD083
            rdx += rax;             //add rdx, rax
            rax = 0x81BB34EC5DCD23B9;               //mov rax, 0x81BB34EC5DCD23B9
            rdx *= rax;             //imul rdx, rax
            rax = rdx;              //mov rax, rdx
            rax >>= 0x22;           //shr rax, 0x22
            rdx ^= rax;             //xor rdx, rax
            rax = 0xBDAB75CEB1FECCBD;               //mov rax, 0xBDAB75CEB1FECCBD
            rdx *= rax;             //imul rdx, rax
            rax = 0xFDAA999E96E6423;                //mov rax, 0xFDAA999E96E6423
            rdx += rax;             //add rdx, rax
            return rdx;
        }
        }
    }
    uint16_t get_bone_index(uint32_t bone_index)
    {
        auto baseModuleAddr = g_data::base;
        uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
        rbx = bone_index;
        rcx = rbx * 0x13C8;
        rax = 0xF4E89764C7960C61;               //mov rax, 0xF4E89764C7960C61
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFFDE34940]
        rax = _umul128(rax, rcx, (uintptr_t*)&rdx);             //mul rcx
        r10 = 0xA4BDAD63622D49A5;               //mov r10, 0xA4BDAD63622D49A5
        rdx >>= 0xD;            //shr rdx, 0x0D
        rax = rdx * 0x2173;             //imul rax, rdx, 0x2173
        rcx -= rax;             //sub rcx, rax
        rax = 0xB88B20E3CBBC993;                //mov rax, 0xB88B20E3CBBC993
        r8 = rcx * 0x2173;              //imul r8, rcx, 0x2173
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rdx >>= 0x9;            //shr rdx, 0x09
        rax = rdx * 0x2C64;             //imul rax, rdx, 0x2C64
        r8 -= rax;              //sub r8, rax
        rax = 0xDB20A88F469598D;                //mov rax, 0xDB20A88F469598D
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r8;               //mov rax, r8
        rax -= rdx;             //sub rax, rdx
        rax >>= 0x1;            //shr rax, 0x01
        rax += rdx;             //add rax, rdx
        rax >>= 0x8;            //shr rax, 0x08
        rcx = rax * 0x1E6;              //imul rcx, rax, 0x1E6
        rax = 0x47AE147AE147AE15;               //mov rax, 0x47AE147AE147AE15
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r8;               //mov rax, r8
        rax -= rdx;             //sub rax, rdx
        rax >>= 0x1;            //shr rax, 0x01
        rax += rdx;             //add rax, rdx
        rax >>= 0x4;            //shr rax, 0x04
        rcx += rax;             //add rcx, rax
        rax = rcx * 0x32;               //imul rax, rcx, 0x32
        rcx = r8 * 0x34;                //imul rcx, r8, 0x34
        rcx -= rax;             //sub rcx, rax
        rax = *(uint16_t*)(rcx + r11 * 1 + 0x7355130);                //movzx eax, word ptr [rcx+r11*1+0x7355130]
        r8 = rax * 0x13C8;              //imul r8, rax, 0x13C8
        rax = r10;              //mov rax, r10
        rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
        rax = r10;              //mov rax, r10
        rdx >>= 0xC;            //shr rdx, 0x0C
        rcx = rdx * 0x18DD;             //imul rcx, rdx, 0x18DD
        r8 -= rcx;              //sub r8, rcx
        r9 = r8 * 0x2C8F;               //imul r9, r8, 0x2C8F
        rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
        rdx >>= 0xC;            //shr rdx, 0x0C
        rax = rdx * 0x18DD;             //imul rax, rdx, 0x18DD
        r9 -= rax;              //sub r9, rax
        rax = 0xC29CFBA0F9592267;               //mov rax, 0xC29CFBA0F9592267
        rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
        rcx = r9;               //mov rcx, r9
        r9 &= 0x7;             //and r9d, 0x07
        rdx >>= 0xA;            //shr rdx, 0x0A
        rax = rdx * 0x543;              //imul rax, rdx, 0x543
        rcx -= rax;             //sub rcx, rax
        rax = r9 + rcx * 8;             //lea rax, [r9+rcx*8]
        rsi = *(uint16_t*)(r11 + rax * 2 + 0x735B020);                //movsx esi, word ptr [r11+rax*2+0x735B020]
        return rsi;
    }
}
namespace g_data
{
	uintptr_t base;
	uintptr_t peb;
	HWND hWind;
	uintptr_t visible_base;
	sdk::refdef_t* refdef;
	FARPROC Targetbitblt;
	FARPROC TargetStretchbitblt;
	FARPROC Target_NtGdiBitBlt;
	sdk::refdef_t refdef2;
	HMODULE dx9rot;
	void init()
	{
		base = (uintptr_t)(iat(GetModuleHandleA).get()("ModernWarfare.exe"));
		dx9rot = LoadLibraryA(xorstr("d3dx9_43.dll"));
		Targetbitblt = iat(GetProcAddress).get()(iat(GetModuleHandleA).get()("Gdi32.dll"), "BitBlt");
		Target_NtGdiBitBlt = iat(GetProcAddress).get()(iat(GetModuleHandleA).get()("win32u.dll"), "NtGdiBitBlt");
		TargetStretchbitblt = iat(GetProcAddress).get()(iat(GetModuleHandleA).get()("win32u.dll"), "NtGdiStretchBlt");

		hWind = process::get_process_window();

		peb = __readgsqword(0x60);
	}
}
namespace menu_setting
{
	int Tab = 0;
}
namespace globals
{
	bool b_in_game = false;
	bool local_is_alive = false;
	bool is_aiming = false;
	int max_player_count = -1;

	/*const char* aim_lock_point[] = { "Spine", "Head", "Neck", "Chest" };
	const char* aim_lock_key[] = { "LButton", "RButton", "LShift" };
	const char* aim_priority_opt[] = { "Only by Closest", "Only in FOV", "Closest and FOV" };
	const const char* unlock_opt[] = { "Unlock CW Camos", "Unlock MW Camos" };*/

	uintptr_t local_ptr;
	uintptr_t player_ptr;
	uintptr_t refdef_ptr;
	vec3_t local_pos;
	uintptr_t clientinfo;
	uintptr_t clientinfobase;

}

namespace colors
{
	ImColor Color{ 1.f,1.f,1.f,1.f };
	ImColor VisibleColorTeam{ 0.f, 0.f, 1.f, 1.f };
	ImColor NotVisibleColorTeam{ 0.f, 0.75f, 1.f, 1.f };
	ImColor VisibleColorEnemy{ 1.f, 1.f, 0.f, 1.f };
	ImColor NotVisibleColorEnemy{ 1.f,0.f,0.f,1.f };
	ImColor radar_bg_color{ 1.f,1.f,1.f,0.f };
	ImColor radar_boarder_color{ 1.f,1.f,1.f,0.f };
}

namespace screenshot
{
	bool		visuals = true;
	bool* pDrawEnabled = nullptr;
	int	screenshot_counter = 0;
	uint32_t	bit_blt_log = 0;
	const char* bit_blt_fail;
	uintptr_t	bit_blt_anotherlog;
	uint32_t	GdiStretchBlt_log = 0;
	const char* GdiStretchBlt_fail;
	uintptr_t	GdiStretchBlt_anotherlog;
	uintptr_t	texture_copy_log = 0;
	uintptr_t	virtualqueryaddr = 0;
}

namespace g_draw
{
	void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
	{
		auto window = ImGui::GetBackgroundDrawList();;
		/*float a = (color >> 24) & 0xff;
		float r = (color >> 16) & 0xff;
		float g = (color >> 8) & 0xff;
		float b = (color) & 0xff;*/
		window->AddLine(from, to, color, thickness);
	}

	void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness)
	{
		draw_line(ImVec2(x, y), ImVec2(x + width, y), color, thickness);
		draw_line(ImVec2(x, y), ImVec2(x, y + height), color, thickness);
		draw_line(ImVec2(x, y + height), ImVec2(x + width, y + height), color, thickness);
		draw_line(ImVec2(x + width, y), ImVec2(x + width, y + height), color, thickness);
	}

	void draw_corned_box(const vec2_t& rect, const vec2_t& size, uint32_t color, float thickness)
	{
		size.x - 5;
		const float lineW = (size.x / 5);
		const float lineH = (size.y / 6);
		const float lineT = 1;

		//outline
		draw_line(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x + lineW, rect.y - lineT), color, thickness); //top left
		draw_line(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x - lineT, rect.y + lineH), color, thickness);
		draw_line(ImVec2(rect.x - lineT, rect.y + size.y - lineH), ImVec2(rect.x - lineT, rect.y + size.y + lineT), color, thickness); //bot left
		draw_line(ImVec2(rect.x - lineT, rect.y + size.y + lineT), ImVec2(rect.x + lineW, rect.y + size.y + lineT), color, thickness);
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y - lineT), color, thickness); // top right
		draw_line(ImVec2(rect.x + size.x + lineT, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y + lineH), color, thickness);
		draw_line(ImVec2(rect.x + size.x + lineT, rect.y + size.y - lineH), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, thickness); // bot right
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y + size.y + lineT), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, thickness);

		//inline
		draw_line(ImVec2(rect.x, rect.y), ImVec2(rect.x, rect.y + lineH), color, thickness);//top left
		draw_line(ImVec2(rect.x, rect.y), ImVec2(rect.x + lineW, rect.y), color, thickness);
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y), ImVec2(rect.x + size.x, rect.y), color, thickness); //top right
		draw_line(ImVec2(rect.x + size.x, rect.y), ImVec2(rect.x + size.x, rect.y + lineH), color, thickness);
		draw_line(ImVec2(rect.x, rect.y + size.y - lineH), ImVec2(rect.x, rect.y + size.y), color, thickness); //bot left
		draw_line(ImVec2(rect.x, rect.y + size.y), ImVec2(rect.x + lineW, rect.y + size.y), color, thickness);
		draw_line(ImVec2(rect.x + size.x - lineW, rect.y + size.y), ImVec2(rect.x + size.x, rect.y + size.y), color, thickness);//bot right
		draw_line(ImVec2(rect.x + size.x, rect.y + size.y - lineH), ImVec2(rect.x + size.x, rect.y + size.y), color, thickness);

	}

	void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color)
	{
		const float end_y = y + hight;
		for (float curr_y = y; curr_y < end_y; ++curr_y)
		{
			draw_line(ImVec2(x, curr_y), ImVec2(x + width, curr_y), color, 1.5f);
		}
	}

	void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness)
	{
		float step = (float)M_PI * 2.0f / thickness;
		for (float a = 0; a < (M_PI * 2.0f); a += step)
		{
			draw_line(
				ImVec2(radius * cosf(a) + position.x, radius * sinf(a) + position.y),
				ImVec2(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y),
				color,
				1.5f
			);
		}
	}

	void draw_sketch_edge_text(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor)
	{
		constexpr float fStrokeVal1 = 1.0f;
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float Edge_a = (EdgeColor >> 24) & 0xff;
		float Edge_r = (EdgeColor >> 16) & 0xff;
		float Edge_g = (EdgeColor >> 8) & 0xff;
		float Edge_b = (EdgeColor) & 0xff;
		std::stringstream steam(text);
		std::string line;
		float y = 0.0f;
		int i = 0;
		while (std::getline(steam, line))
		{
			ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
			if (center)
			{
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
			}
			else
			{
				window->DrawList->AddText(pFont, size, ImVec2((pos.x) - fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x) + fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
				window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
			}
			y = pos.y + textSize.y * (i + 1);
			i++;
		}
	}

	void draw_crosshair()
	{
		constexpr long crosshair_size = 15.0f;

		ImVec2 center = ImVec2(g_data::refdef->Width / 2, g_data::refdef->Height / 2);

		g_draw::draw_line(ImVec2((center.x), (center.y) - crosshair_size), ImVec2((center.x), (center.y) + crosshair_size), ImGui::GetColorU32({ 255, 0, 0, 255 }), 1.5f);
		g_draw::draw_line(ImVec2((center.x) - crosshair_size, (center.y)), ImVec2((center.x) + crosshair_size, (center.y)), ImGui::GetColorU32({ 255, 0, 0, 255 }), 1.5f);
	}

	void draw_fov(const float aimbot_fov)
	{
		ImVec2 center = ImVec2(g_data::refdef->Width / 2, g_data::refdef->Height / 2);
		g_draw::draw_circle(center, aimbot_fov, ImColor(30, 215, 157), 100.0f);
	}

	void draw_bones(vec2_t* bones_screenPos, long count, DWORD color)
	{
		long last_count = count - 1;
		for (long i = 0; i < last_count; ++i)
			g_draw::draw_line(ImVec2(bones_screenPos[i].x, bones_screenPos[i].y), ImVec2(bones_screenPos[i + 1].x, bones_screenPos[i + 1].y), color, 1.8f);
	}

	void draw_bones(unsigned long i, vec3_t origem, DWORD color)
	{
		vec3_t header_to_bladder[6], right_foot_to_bladder[5], left_foot_to_bladder[5], right_hand[5], left_hand[5];
		vec2_t screen_header_to_bladder[6], screen_right_foot_to_bladder[5], screen_left_foot_to_bladder[5], screen_right_hand[5], screen_left_hand[5]; screen_left_hand[5];

		if (sdk::get_bone_by_player_index(i, sdk::BONE_POS_HEAD, &header_to_bladder[0]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_NECK, &header_to_bladder[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_CHEST, &header_to_bladder[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_MID, &header_to_bladder[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_TUMMY, &header_to_bladder[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_PELVIS, &header_to_bladder[5]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_1, &right_foot_to_bladder[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_2, &right_foot_to_bladder[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_3, &right_foot_to_bladder[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_4, &right_foot_to_bladder[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_1, &left_foot_to_bladder[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_2, &left_foot_to_bladder[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_3, &left_foot_to_bladder[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_4, &left_foot_to_bladder[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_1, &right_hand[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_2, &right_hand[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_3, &right_hand[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_4, &right_hand[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_1, &left_hand[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_2, &left_hand[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_3, &left_hand[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_4, &left_hand[4]))
		{
			right_foot_to_bladder[0] = header_to_bladder[5];
			left_foot_to_bladder[0] = header_to_bladder[5];
			right_hand[0] = header_to_bladder[3];
			left_hand[0] = header_to_bladder[3];

			if (!sdk::is_valid_bone(origem, header_to_bladder, 6) ||
				!sdk::is_valid_bone(origem, right_foot_to_bladder, 5) ||
				!sdk::is_valid_bone(origem, left_foot_to_bladder, 5) ||
				!sdk::is_valid_bone(origem, right_hand, 5) ||
				!sdk::is_valid_bone(origem, left_hand, 5))
			{
				return;
			}

			if (!sdk::bones_to_screen(header_to_bladder, screen_header_to_bladder, 6))
				return;

			if (!sdk::bones_to_screen(right_foot_to_bladder, screen_right_foot_to_bladder, 5))
				return;

			if (!sdk::bones_to_screen(left_foot_to_bladder, screen_left_foot_to_bladder, 5))
				return;

			if (!sdk::bones_to_screen(right_hand, screen_right_hand, 5))
				return;

			if (!sdk::bones_to_screen(left_hand, screen_left_hand, 5))
				return;

			draw_bones(screen_header_to_bladder, 6, color);
			draw_bones(screen_right_foot_to_bladder, 5, color);
			draw_bones(screen_left_foot_to_bladder, 5, color);
			draw_bones(screen_right_hand, 5, color);
			draw_bones(screen_left_hand, 5, color);
		}
	}

	void draw_health(int i_health, vec3_t pos)
	{
		vec2_t bottom, top;

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
		auto color_health = 0x03FF00FF;
		auto color_health2 = 0xFF0F00FF;
		fill_rectangle(x, y, 4, 127 * (bottom.y - top.y) / 127.f, color_health2);
		fill_rectangle(x + 1, y + 1, 2, bar_width - 2, color_health);

	}
}

namespace g_radar {

	float RadarPosX = 60;
	float RadarPosY = 45;
	long  RadarSize = 220;
	long  RadarRadius = RadarSize / 2;
	float RadarLineInterval = 1.0f;
	float BackgroundInterval = 5.0f;

	void show_radar_background()
	{
		/*RadarPosX = g_data::refdef->Width - RadarSize - 50;*/

		g_draw::fill_rectangle(
			RadarPosX - BackgroundInterval,
			RadarPosY - BackgroundInterval,
			RadarSize + (BackgroundInterval * 2),
			RadarSize + (BackgroundInterval * 2),
			/*0xFFFFFEFE*/
			colors::radar_boarder_color);

		g_draw::fill_rectangle(
			RadarPosX,
			RadarPosY,
			RadarSize,
			RadarSize,
			/*0xFFA8AAAA*/
			colors::radar_bg_color);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY + RadarLineInterval),
			ImVec2(RadarPosX, RadarPosY + RadarSize - RadarLineInterval + 1),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX + RadarSize, RadarPosY + RadarLineInterval),
			ImVec2(RadarPosX + RadarSize, RadarPosY + RadarSize - RadarLineInterval + 1),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY),
			ImVec2(RadarPosX + RadarSize - RadarLineInterval + 2, RadarPosY),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY + RadarSize),
			ImVec2(RadarPosX + RadarSize - RadarLineInterval + 2, RadarPosY + RadarSize),
			0xFF010000,
			1.3f);


		g_draw::draw_line(
			ImVec2(RadarPosX + RadarRadius, RadarPosY + RadarLineInterval),
			ImVec2(RadarPosX + RadarRadius, RadarPosY + RadarSize - RadarLineInterval),
			0xFF010000,
			1.3f);

		g_draw::draw_line(
			ImVec2(RadarPosX, RadarPosY + RadarRadius),
			ImVec2(RadarPosX + RadarSize - RadarLineInterval, RadarPosY + RadarRadius),
			0xFF010000,
			1.3f);


		g_draw::draw_box(RadarPosX + RadarRadius - 3, RadarPosY + RadarRadius - 3, 5, 5, 0xFF000100, 1.0f);
		g_draw::fill_rectangle(RadarPosX + RadarRadius - 2, RadarPosY + RadarRadius - 2, 4, 4, 0xFFFFFFFF);
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

	vec2_t radar_rotate(const float x, const float y, float angle)
	{
		angle = (float)(angle * (M_PI / 180.f));
		float cosTheta = (float)cos(angle);
		float sinTheta = (float)sin(angle);
		vec2_t returnVec;
		returnVec.x = cosTheta * x + sinTheta * y;
		returnVec.y = sinTheta * x - cosTheta * y;
		return returnVec;
	}

	void draw_entity(const ImVec2& pos, float angle, DWORD color)
	{
		constexpr long up_offset = 7;
		constexpr long lr_offset = 5;

		for (int FillIndex = 0; FillIndex < 5; ++FillIndex)
		{
			ImVec2 up_pos(pos.x, pos.y - up_offset + FillIndex);
			ImVec2 left_pos(pos.x - lr_offset + FillIndex, pos.y + up_offset - FillIndex);
			ImVec2 right_pos(pos.x + lr_offset - FillIndex, pos.y + up_offset - FillIndex);

			ImVec2 p0 = rotate(pos, up_pos, angle);
			ImVec2 p1 = rotate(pos, left_pos, angle);
			ImVec2 p2 = rotate(pos, right_pos, angle);

			g_draw::draw_line(p0, p1, FillIndex == 0 ? 0xFF010000 : color, 1.0f);
			g_draw::draw_line(p1, p2, FillIndex == 0 ? 0xFF010000 : color, 1.0f);
			g_draw::draw_line(p2, p0, FillIndex == 0 ? 0xFF010000 : color, 1.0f);
		}
	}

	void draw_death_entity(const ImVec2& pos, DWORD color)
	{
		constexpr float line_radius = 5;
		ImVec2 p0(pos.x - line_radius, pos.y - line_radius);
		ImVec2 p1(pos.x + line_radius, pos.y + line_radius);
		ImVec2 p3(pos.x - line_radius, pos.y + line_radius);
		ImVec2 p4(pos.x + line_radius, pos.y - line_radius);
		g_draw::draw_line(p0, p1, color, 1.5f);
		g_draw::draw_line(p3, p4, color, 1.5f);
	}

	void draw_entity(sdk::player_t local_entity, sdk::player_t entity, bool IsFriendly, bool is_dead, DWORD color, ImVec2 window_pos, int RadarRadius1)
	{
		const float local_rotation = local_entity.get_rotation();
		float rotation = entity.get_rotation();

		rotation = rotation - local_rotation;

		if (rotation < 0)
			rotation = 360.0f - std::fabs(rotation);

		float x_distance = local_entity.get_pos().x - entity.get_pos().x;
		float y_distance = local_entity.get_pos().y - entity.get_pos().y;

		float zoom = Settings::esp::radar_zoom * 0.001f;

		x_distance *= zoom;
		y_distance *= zoom;

		vec2_t point_offset = radar_rotate(x_distance, y_distance, local_rotation);

		long positiveRadarRadius = RadarRadius1 - 5;
		long negaRadarRadius = (RadarRadius1 * -1) + 5;

		if (point_offset.x > positiveRadarRadius)
		{
			point_offset.x = positiveRadarRadius;
		}
		else if (point_offset.x < negaRadarRadius)
		{
			point_offset.x = negaRadarRadius;
		}

		if (point_offset.y > positiveRadarRadius)
		{
			point_offset.y = positiveRadarRadius;
		}
		else if (point_offset.y < negaRadarRadius)
		{
			point_offset.y = negaRadarRadius;
		}

		if (!is_dead)
		{
			draw_entity(ImVec2(window_pos.x + RadarRadius1 + point_offset.x, window_pos.y + RadarRadius1 + point_offset.y), rotation, color);
		}
		else
		{

		}
	}
}
