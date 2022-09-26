#include "vec.h"

namespace aim_assist 
{
	extern vec2_t lock_position;
	extern float init_center_dist;
	extern float min_center_dist;
	extern float init_distance;
	extern float min_distance;

	void reset_lock();
	bool is_key_down(int vk_key);
	unsigned long get_bone_opt();
	void get_closest_enemy(vec2_t target_pos, float distance);
	void get_enemy_in_fov(vec2_t target_pos);
	void aim_at();
	void is_aiming();
}
void weapon_def(int index);