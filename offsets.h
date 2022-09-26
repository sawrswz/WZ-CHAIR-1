#pragma once
namespace offsets {
    constexpr auto refdef = 0x14D5DAA8;
    constexpr auto name_array = 0x14D69F70;
    constexpr auto name_array_pos = 0x4C70;
    constexpr auto camera_base = 0x126C8230;
    constexpr auto camera_pos = 0x1E8;
    constexpr auto local_index = 0x32168;
    constexpr auto local_index_pos = 0x204;
    constexpr auto recoil = 0x31D68;
    constexpr auto game_mode = 0x148922D8;
    constexpr auto weapon_definitions = 0x1278D6C0;
    constexpr auto distribute = 0x1747F368;
    constexpr auto visible = 0x632AF20;
    constexpr auto playerstate = 0x1F23C6A0;

    namespace player 
    {
        constexpr auto size = 0x60B8;
        constexpr auto valid = 0xC5C;
        constexpr auto pos = 0x2C0;
        constexpr auto team = 0x688;
        constexpr auto stance = 0xD60;
        constexpr auto weapon_index = 0x1738;
        constexpr auto dead_1 = 0x220;
        constexpr auto dead_2 = 0x374;
    }

    namespace bone {
        constexpr auto base_pos = 0xA60A4;
        constexpr auto index_struct_size = 0x150;
    }
}