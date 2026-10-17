/*
 * POKEMON EMERALD: DEV BHOOMI UTTARAKHAND EDITION (GBA HOMEBREW)
 * Toolchain: devkitARM + libtonc (Single-File Zero-Asset Build)
 *
 * === UTTARAKHAND CONVERSION TABLE (EMERALD -> UTTARAKHAND) ===
 *  0. Littleroot Town  -> Srinagar Garhwal (Starter Town, Prof's Himalayan Lab)
 *  1. Oldale Town      -> Devprayag (Confluence of Alaknanda & Bhagirathi -> Ganga)
 *  2. Petalburg City   -> Pauri Garhwal (Dad's Gym -> Pauri Forest Gym, Badge 1)
 *  3. Rustboro City    -> Rishikesh (Stone Gym -> Lakshman Jhula Gym, Leader: Swami, Badge 2)
 *  4. Dewford Town     -> Lansdowne (Fighter Gym -> Garhwali Regiment Gym, Leader: Major, Badge 3)
 *  5. Slateport City   -> Haridwar (Oceanic Port -> Har Ki Pauri Ganga Ghat)
 *  6. Mauville City    -> Roorkee (Electric Gym -> IIT Roorkee Tech Gym, Leader: Dr. Roy, Badge 4)
 *  7. Verdanturf Town  -> Almora (Scenic Kumaon Cultural Mountain Town)
 *  8. Fallarbor Town   -> Joshimath (High-Altitude Himalayan Base Camp)
 *  9. Lavaridge Town   -> Badrinath (Hot Springs Gym -> Tapt Kund Gym, Badge 5)
 * 10. Fortree City     -> Nainital (Canopy/Fly Gym -> Naini Lake Peaks Gym, Badge 6)
 * 11. Lilycove City    -> Mussoorie (Queen of the Hills & Contest Hall)
 * 12. Mossdeep City    -> Kedarnath (Psychic/Sky Gym -> Kedarnath Temple Gym, Badge 7)
 * 13. Pacifidlog Town  -> Valley of Flowers (Remote Alpine Flora Sanctuary)
 * 14. Sootopolis City  -> Auli (Crater Gym -> Auli Snow Slopes Gym, Leader: Himanshu, Badge 8)
 * 15. Ever Grande City -> Dehradun (Capital City - Uttarakhand Champion League)
 */

#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODE0       DCNT_MODE0
#define BG0_ENABLE  DCNT_BG0
#define BG1_ENABLE  DCNT_BG1
#define OBJ_ENABLE  (DCNT_OBJ | DCNT_OBJ_1D)

#define T_GRASS     0
#define T_WALL      1
#define T_WATER     2
#define T_EXIT      3
#define T_GYM       4
#define T_TRAINER   5

/* =========================================================================
 * 1. GRAPHICS DATA GENERATION (RAW 15-BIT HARDWARE PALETTE & 4BPP TILES)
 * ========================================================================= */

const u16 game_palette[16] = {
    0x0000, /* 0: Transparent / Black */
    0x0246, /* 1: Valley Grass Green   (6,  18, 6)  */
    0x01A4, /* 2: Deep Pine Green      (4,  13, 4)  */
    0x194E, /* 3: Himalayan Rock Brown (14, 10, 6)  */
    0x0CC9, /* 4: Dark Cliff Brown     (9,  6,  3)  */
    0x6984, /* 5: Ganga River Blue     (4,  12, 26) */
    0x7A8C, /* 6: River Rapids Cyan    (12, 20, 30) */
    0x7FDE, /* 7: Himalayan Snow White (30, 30, 31) */
    0x12EC, /* 8: Temple Gold / Exit   (28, 23, 4)  */
    0x14BA, /* 9: Buransh Crimson      (26, 5,  5)  */
    0x3E9C, /* 10: Character Skin      (28, 20, 15) */
    0x4905, /* 11: Nehru Cap / Navy    (5,  8,  18) */
    0x11D6, /* 12: Trekking Backpack   (22, 14, 4)  */
    0x60D2, /* 13: Monal Plumage       (18, 6,  24) */
    0x62D4, /* 14: Barf-Leopard Grey   (20, 22, 24) */
    0x7FFF  /* 15: Pure UI White       (31, 31, 31) */
};

const TILE bg_tiles[6] = {
    {{ 0x11111111, 0x12111121, 0x11211211, 0x11111111,
       0x11112111, 0x12111211, 0x11111111, 0x11121111 }},
    {{ 0x47777774, 0x77733777, 0x73333337, 0x33444333,
       0x34433443, 0x43333334, 0x33444433, 0x44444444 }},
    {{ 0x55555555, 0x56655665, 0x65566556, 0x55555555,
       0x55665566, 0x56556555, 0x55555555, 0x66555566 }},
    {{ 0x11888811, 0x18877881, 0x88788788, 0x88888888,
       0x11833811, 0x11833811, 0x11833811, 0x11888811 }},
    {{ 0x11188111, 0x11999911, 0x19988991, 0x99999999,
       0x37788773, 0x37444473, 0x37488473, 0x33488433 }},
    {{ 0x11999911, 0x19977991, 0x11AAAA11, 0x11A00A11,
       0x19BBBB91, 0x19BBBB91, 0x11B11B11, 0x11411411 }}
};

const TILE sprite_tiles[16] = {
    {{ 0x000BBBBB, 0x00BBBB8B, 0x00BBBBBB, 0x00AAAAAA, 0x00AA0AAA, 0x00AAAAAA, 0x000AAAA0, 0x00CCCCCC }},
    {{ 0xBBBBB000, 0xB8BBBB00, 0xBBBBBB00, 0xAAAAAA00, 0xAAA0AA00, 0xAAAAAA00, 0x0AAAA000, 0xCCCCCC00 }},
    {{ 0x0CCBBBBB, 0x0CCBBBBB, 0x0CCBBBBB, 0x00BBBBBB, 0x000BB0BB, 0x000BB0BB, 0x00044044, 0x00044044 }},
    {{ 0xBBBBBCC0, 0xBBBBBCC0, 0xBBBBBCC0, 0xBBBBBB00, 0xBB0BB000, 0xBB0BB000, 0x44044000, 0x44044000 }},
    {{ 0x00088888, 0x00999999, 0x00998899, 0x00AAAAAA, 0x00AA0AAA, 0x00AAAAAA, 0x00099990, 0x00999999 }},
    {{ 0x88888000, 0x99999900, 0x99889900, 0xAAAAAA00, 0xAAA0AA00, 0xAAAAAA00, 0x09999000, 0x99999900 }},
    {{ 0x09988889, 0x09988889, 0x00999999, 0x00999999, 0x00099099, 0x00099099, 0x00033033, 0x00033033 }},
    {{ 0x98888990, 0x98888990, 0x99999900, 0x99999900, 0x99099000, 0x99099000, 0x33033000, 0x33033000 }},
    {{ 0x00880880, 0x00088800, 0x000DDDDD, 0x00DD70DD, 0x088DDDDD, 0x888DDDDD, 0x00055555, 0x00552255 }},
    {{ 0x00000000, 0x00000000, 0xDDDD0000, 0xDDDDD000, 0xDDDDDD00, 0x55559990, 0x55229999, 0x55222999 }},
    {{ 0x00552225, 0x00055222, 0x00005555, 0x00000808, 0x00000808, 0x00008808, 0x00000000, 0x00000000 }},
    {{ 0x22299900, 0x22999000, 0x55990000, 0x80800000, 0x80800000, 0x80880000, 0x00000000, 0x00000000 }},
    {{ 0x00770077, 0x07E777E7, 0x07EEEEE7, 0x07E9E9E7, 0x07EE0EE7, 0x007EEE70, 0x007E4E77, 0x07EEEEE4 }},
    {{ 0x00000000, 0x00000000, 0x00000077, 0x000007E7, 0x00007E70, 0x7777E700, 0xE4EEE700, 0xEE4EE700 }},
    {{ 0x07E4EEE7, 0x07EE4EE7, 0x00777777, 0x007E07E0, 0x007E07E0, 0x00770770, 0x00000000, 0x00000000 }},
    {{ 0xEEE4E700, 0xE4EEE700, 0x77777000, 0x0E70E700, 0x0E70E700, 0x07707700, 0x00000000, 0x00000000 }}
};

/* =========================================================================
 * 2. MAP DATA (16 TOWNS, 20x32 TILES EACH)
 * ========================================================================= */

#define R_MTN  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
#define R_VAL  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
#define R_PK1  {1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1}
#define R_PK2  {1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1}
#define R_RIV  {1,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,1}
#define R_BRG  {1,2,2,2,2,0,0,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,0,0,2,2,2,2,2,2,1}
#define R_LAK  {1,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,1}
#define R_GYM  {1,0,0,0,0,0,0,0,1,1,4,1,1,0,0,0,0,0,1,1,4,1,1,0,0,0,0,0,0,0,0,1}
#define R_NPC  {1,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
#define R_LAB  {1,0,0,0,1,4,1,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,1,0,0,0,0,1}
#define R_RD0  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}
#define R_RDM  {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}
#define R_RDE  {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}

const u8 map_data[16][20][32] = {
    { R_MTN, R_PK1, R_VAL, R_VAL, R_LAB, R_VAL, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RD0, R_VAL, R_VAL, R_RIV, R_RIV, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    { R_MTN, R_RIV, R_RIV, R_BRG, R_VAL, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_RIV, R_BRG, R_RIV, R_VAL, R_PK1, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_PK2, R_VAL, R_GYM, R_NPC, R_VAL, R_PK2, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_PK1, R_VAL, R_VAL, R_PK2, R_PK1, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_RIV, R_BRG, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_BRG, R_RIV, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_PK1, R_PK2, R_GYM, R_NPC, R_VAL, R_VAL, R_PK2, R_VAL,
      R_RDM, R_VAL, R_PK2, R_VAL, R_VAL, R_PK1, R_PK2, R_PK1, R_MTN, R_MTN },
    { R_MTN, R_VAL, R_RIV, R_RIV, R_BRG, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_RIV, R_RIV, R_BRG, R_VAL, R_VAL, R_MTN, R_MTN },
    { R_MTN, R_VAL, R_LAB, R_VAL, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_LAB, R_VAL, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_PK2, R_VAL, R_LAB, R_VAL, R_VAL, R_PK2, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_PK1, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN, R_MTN },
    { R_MTN, R_MTN, R_PK1, R_PK2, R_LAB, R_VAL, R_VAL, R_PK1, R_VAL, R_VAL,
      R_RDM, R_VAL, R_PK2, R_PK1, R_VAL, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN },
    { R_MTN, R_MTN, R_PK1, R_LAK, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_LAK, R_VAL, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_LAK, R_LAK, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_LAK, R_LAK, R_LAK, R_VAL, R_PK2, R_PK1, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_VAL, R_LAB, R_GYM, R_NPC, R_VAL, R_VAL, R_PK2, R_VAL,
      R_RDM, R_VAL, R_VAL, R_LAB, R_VAL, R_PK2, R_PK1, R_MTN, R_MTN, R_MTN },
    { R_MTN, R_MTN, R_MTN, R_PK1, R_GYM, R_NPC, R_VAL, R_PK2, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_BRG, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN, R_MTN },
    { R_MTN, R_PK2, R_VAL, R_VAL, R_VAL, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_LAK, R_VAL, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    { R_MTN, R_MTN, R_PK1, R_LAK, R_GYM, R_NPC, R_VAL, R_PK1, R_VAL, R_VAL,
      R_RDM, R_VAL, R_LAK, R_PK2, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN, R_MTN },
    { R_MTN, R_PK1, R_LAB, R_GYM, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDE, R_VAL, R_VAL, R_LAB, R_VAL, R_VAL, R_PK1, R_MTN, R_MTN, R_MTN }
};

/* =========================================================================
 * 3. POKEMON (REGIONAL + GEN 1/2/3) & MOVESETS
 * ========================================================================= */

typedef struct {
    const char* name;
    const char* move1;
    u8 dmg1;
    const char* move2;
    u8 dmg2;
    u8 max_hp;
    u8 sprite_id;
} PkmnSpec;

const PkmnSpec pkmn_db[10] = {
    {"BURANSH (TORCHIC)",    "EMBER",        16, "FLAMETHROWER", 24, 65,  8},
    {"MONAL (ARTICUNO)",     "ICE BEAM",     18, "AERIAL ACE",   15, 55,  8},
    {"KASTURI (STANTLER)",   "RAZOR LEAF",   15, "TAKE DOWN",    18, 52, 12},
    {"GURAL (GRAVELER)",     "ROCK SLIDE",   17, "EARTHQUAKE",   22, 60, 12},
    {"BARF-LEOPARD (ABSOL)", "SLASH",        18, "SHADOW BALL",  20, 64, 12},
    {"PIKACHU (GEN 1)",      "THUNDERBOLT",  19, "QUICK ATTACK", 12, 48,  8},
    {"MACHOKE (GEN
