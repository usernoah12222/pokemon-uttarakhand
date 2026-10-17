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

/* Hardware Register Bit Aliases matching specification & libtonc */
#define MODE0       DCNT_MODE0
#define BG0_ENABLE  DCNT_BG0
#define BG1_ENABLE  DCNT_BG1
#define OBJ_ENABLE  (DCNT_OBJ | DCNT_OBJ_1D)

/* Tile Definitions */
#define T_GRASS     0
#define T_WALL      1
#define T_WATER     2
#define T_EXIT      3
#define T_GYM       4
#define T_TRAINER   5

/* =========================================================================
 * 1. GRAPHICS DATA GENERATION (PROGRAMMATIC 16-COLOR PALETTE & 4BPP TILES)
 * ========================================================================= */

const u16 game_palette[16] = {
    RGB15(0,  0,  0),   /* 0: Transparent / Black */
    RGB15(6,  18, 6),   /* 1: Valley Grass Green */
    RGB15(4,  13, 4),   /* 2: Deep Pine Green */
    RGB15(14, 10, 6),   /* 3: Himalayan Rock Brown */
    RGB15(9,  6,  3),   /* 4: Dark Cliff Brown */
    RGB15(4,  12, 26),  /* 5: Alaknanda/Ganga River Blue */
    RGB15(12, 20, 30),  /* 6: River Rapids Cyan */
    RGB15(30, 30, 31),  /* 7: Himalayan Snow White */
    RGB15(28, 23, 4),   /* 8: Temple Gold / Exit Sign */
    RGB15(26, 5,  5),   /* 9: Buransh Crimson / Roof Red */
    RGB15(28, 20, 15),  /* 10: Character Skin Tone */
    RGB15(5,  8,  18),  /* 11: Nehru Cap / Tunic Navy */
    RGB15(22, 14, 4),   /* 12: Trekking Backpack Ochre */
    RGB15(18, 6,  24),  /* 13: Monal Plumage Purple */
    RGB15(20, 22, 24),  /* 14: Barf-Leopard Frost Grey */
    RGB15(31, 31, 31)   /* 15: Pure UI White */
};

/* 8x8 Background Tileset (6 Tiles, 4bpp: each u32 holds 8 horizontal pixels) */
const TILE bg_tiles[6] = {
    /* 0: GRASS (Valley Meadow with grass blades) */
    {{ 0x11111111, 0x12111121, 0x11211211, 0x11111111,
       0x11112111, 0x12111211, 0x11111111, 0x11121111 }},
    /* 1: WALL/OBSTACLE (Snow-capped Himalayan Rock) */
    {{ 0x47777774, 0x77733777, 0x73333337, 0x33444333,
       0x34433443, 0x43333334, 0x33444433, 0x44444444 }},
    /* 2: WATER (Ganga River Currents) */
    {{ 0x55555555, 0x56655665, 0x65566556, 0x55555555,
       0x55665566, 0x56556555, 0x55555555, 0x66555566 }},
    /* 3: ENTRANCE/TOWN EXIT (Golden Milestone Gate) */
    {{ 0x11888811, 0x18877881, 0x88788788, 0x88888888,
       0x11833811, 0x11833811, 0x11833811, 0x11888811 }},
    /* 4: GYM/BUILDING (Uttarakhand Temple / Gym Roof) */
    {{ 0x11188111, 0x11999911, 0x19988991, 0x99999999,
       0x37788773, 0x37444473, 0x37488473, 0x33488433 }},
    /* 5: TRAINER / GYM MASTER OVERWORLD ICON */
    {{ 0x11999911, 0x19977991, 0x11AAAA11, 0x11A00A11,
       0x19BBBB91, 0x19BBBB91, 0x11B11B11, 0x11411411 }}
};

/* 16x16 OAM Sprites (4 tiles each: Top-Left, Top-Right, Bottom-Left, Bottom-Right) */
const TILE sprite_tiles[16] = {
    /* Tiles 0-3: Player (16x16 Male Hero with Pahadi/Nehru Cap & Trek Backpack) */
    {{ 0x000BBBBB, 0x00BBBB8B, 0x00BBBBBB, 0x00AAAAAA, 0x00AA0AAA, 0x00AAAAAA, 0x000AAAA0, 0x00CCCCCC }},
    {{ 0xBBBBB000, 0xB8BBBB00, 0xBBBBBB00, 0xAAAAAA00, 0xAAA0AA00, 0xAAAAAA00, 0x0AAAA000, 0xCCCCCC00 }},
    {{ 0x0CCBBBBB, 0x0CCBBBBB, 0x0CCBBBBB, 0x00BBBBBB, 0x000BB0BB, 0x000BB0BB, 0x00044044, 0x00044044 }},
    {{ 0xBBBBBCC0, 0xBBBBBCC0, 0xBBBBBCC0, 0xBBBBBB00, 0xBB0BB000, 0xBB0BB000, 0x44044000, 0x44044000 }},

    /* Tiles 4-7: NPC / Gym Leader (Saffron/Crimson Robes & Crown) */
    {{ 0x00088888, 0x00999999, 0x00998899, 0x00AAAAAA, 0x00AA0AAA, 0x00AAAAAA, 0x00099990, 0x00999999 }},
    {{ 0x88888000, 0x99999900, 0x99889900, 0xAAAAAA00, 0xAAA0AA00, 0xAAAAAA00, 0x09999000, 0x99999900 }},
    {{ 0x09988889, 0x09988889, 0x00999999, 0x00999999, 0x00099099, 0x00099099, 0x00033033, 0x00033033 }},
    {{ 0x98888990, 0x98888990, 0x99999900, 0x99999900, 0x99099000, 0x99099000, 0x33033000, 0x33033000 }},

    /* Tiles 8-11: Wild Monal (Himalayan Monal - Plume Crest & Rainbow Wings) */
    {{ 0x00880880, 0x00088800, 0x000DDDDD, 0x00DD70DD, 0x088DDDDD, 0x888DDDDD, 0x00055555, 0x00552255 }},
    {{ 0x00000000, 0x00000000, 0xDDDD0000, 0xDDDDD000, 0xDDDDDD00, 0x55559990, 0x55229999, 0x55222999 }},
    {{ 0x00552225, 0x00055222, 0x00005555, 0x00000808, 0x00000808, 0x00008808, 0x00000000, 0x00000000 }},
    {{ 0x22299900, 0x22999000, 0x55990000, 0x80800000, 0x80800000, 0x80880000, 0x00000000, 0x00000000 }},

    /* Tiles 12-15: Wild Barf-Leopard (Snow Leopard - Spotted Frost Pelt) */
    {{ 0x00770077, 0x07E777E7, 0x07EEEEE7, 0x07E9E9E7, 0x07EE0EE7, 0x007EEE70, 0x007E4E77, 0x07EEEEE4 }},
    {{ 0x00000000, 0x00000000, 0x00000077, 0x000007E7, 0x00007E70, 0x7777E700, 0xE4EEE700, 0xEE4EE700 }},
    {{ 0x07E4EEE7, 0x07EE4EE7, 0x00777777, 0x007E07E0, 0x007E07E0, 0x00770770, 0x00000000, 0x00000000 }},
    {{ 0xEEE4E700, 0xE4EEE700, 0x77777000, 0x0E70E700, 0x0E70E700, 0x07707700, 0x00000000, 0x00000000 }}
};

/* =========================================================================
 * 2. MAP & STATE MATRICES (16 UTTARAKHAND TOWNS, 20x32 TILES EACH)
 * ========================================================================= */

/* Row Helper Macros (32 columns each) for clean, verifiable 20x32 layouts */
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
    /* 0: Srinagar Garhwal (Littleroot Town - Starter Valley & Prof's Lab) */
    { R_MTN, R_PK1, R_VAL, R_VAL, R_LAB, R_VAL, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RD0, R_VAL, R_VAL, R_RIV, R_RIV, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    /* 1: Devprayag (Oldale Town - Confluence of Alaknanda & Bhagirathi) */
    { R_MTN, R_RIV, R_RIV, R_BRG, R_VAL, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_RIV, R_BRG, R_RIV, R_VAL, R_PK1, R_MTN, R_MTN },
    /* 2: Pauri Garhwal (Petalburg City - Pauri Pine Forest Gym - Badge 1) */
    { R_MTN, R_PK1, R_PK2, R_VAL, R_GYM, R_NPC, R_VAL, R_PK2, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_PK1, R_VAL, R_VAL, R_PK2, R_PK1, R_MTN, R_MTN },
    /* 3: Rishikesh (Rustboro City - Lakshman Jhula Bridge Gym, Leader: Swami - Badge 2) */
    { R_MTN, R_PK1, R_RIV, R_BRG, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_BRG, R_RIV, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    /* 4: Lansdowne (Dewford Town - Garhwali Regiment Camp Gym, Leader: Major - Badge 3) */
    { R_MTN, R_PK1, R_PK1, R_PK2, R_GYM, R_NPC, R_VAL, R_VAL, R_PK2, R_VAL,
      R_RDM, R_VAL, R_PK2, R_VAL, R_VAL, R_PK1, R_PK2, R_PK1, R_MTN, R_MTN },
    /* 5: Haridwar (Slateport City - Busy Ganga Ghats & Har Ki Pauri Shrine) */
    { R_MTN, R_VAL, R_RIV, R_RIV, R_BRG, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_RIV, R_RIV, R_BRG, R_VAL, R_VAL, R_MTN, R_MTN },
    /* 6: Roorkee (Mauville City - IIT Roorkee Tech Gym, Leader: Dr. Roy - Badge 4) */
    { R_MTN, R_VAL, R_LAB, R_VAL, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_LAB, R_VAL, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    /* 7: Almora (Verdanturf Town - Scenic Kumaon Ridge) */
    { R_MTN, R_PK1, R_PK2, R_VAL, R_LAB, R_VAL, R_VAL, R_PK2, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_PK1, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN, R_MTN },
    /* 8: Joshimath (Fallarbor Town - High Mountain Base Camp) */
    { R_MTN, R_MTN, R_PK1, R_PK2, R_LAB, R_VAL, R_VAL, R_PK1, R_VAL, R_VAL,
      R_RDM, R_VAL, R_PK2, R_PK1, R_VAL, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN },
    /* 9: Badrinath (Lavaridge Town - Tapt Kund Hot Spring Gym - Badge 5) */
    { R_MTN, R_MTN, R_PK1, R_LAK, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_LAK, R_VAL, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN, R_MTN },
    /* 10: Nainital (Fortree City - Naini Lake High Peaks Gym - Badge 6) */
    { R_MTN, R_PK1, R_LAK, R_LAK, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_LAK, R_LAK, R_LAK, R_VAL, R_PK2, R_PK1, R_MTN, R_MTN },
    /* 11: Mussoorie (Lilycove City - Queen of the Hills & Contest Hall) */
    { R_MTN, R_PK1, R_VAL, R_LAB, R_GYM, R_NPC, R_VAL, R_VAL, R_PK2, R_VAL,
      R_RDM, R_VAL, R_VAL, R_LAB, R_VAL, R_PK2, R_PK1, R_MTN, R_MTN, R_MTN },
    /* 12: Kedarnath (Mossdeep City - Kedarnath Temple Peaks Gym - Badge 7) */
    { R_MTN, R_MTN, R_MTN, R_PK1, R_GYM, R_NPC, R_VAL, R_PK2, R_VAL, R_VAL,
      R_RDM, R_VAL, R_RIV, R_BRG, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN, R_MTN },
    /* 13: Valley of Flowers (Pacifidlog Town - Alpine Flora Village) */
    { R_MTN, R_PK2, R_VAL, R_VAL, R_VAL, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDM, R_VAL, R_VAL, R_LAK, R_VAL, R_VAL, R_VAL, R_PK2, R_MTN, R_MTN },
    /* 14: Auli (Sootopolis City - Snow Slopes Crater Gym, Leader: Himanshu - Badge 8) */
    { R_MTN, R_MTN, R_PK1, R_LAK, R_GYM, R_NPC, R_VAL, R_PK1, R_VAL, R_VAL,
      R_RDM, R_VAL, R_LAK, R_PK2, R_PK1, R_MTN, R_MTN, R_MTN, R_MTN, R_MTN },
    /* 15: Dehradun (Ever Grande City - Capital & Uttarakhand Champion League) */
    { R_MTN, R_PK1, R_LAB, R_GYM, R_GYM, R_NPC, R_VAL, R_VAL, R_VAL, R_VAL,
      R_RDE, R_VAL, R_VAL, R_LAB, R_VAL, R_VAL, R_PK1, R_MTN, R_MTN, R_MTN }
};

/* =========================================================================
 * 3. POKEMON (REGIONAL + GEN 1/2/3) & GEN 1-3 MOVESETS
 * ========================================================================= */

typedef struct {
    const char* name;
    const char* move1;
    u8 dmg1;
    const char* move2;
    u8 dmg2;
    u8 max_hp;
    u8 sprite_id; /* 8 = Monal/Bird, 12 = Barf-Leopard/Beast, 4 = Trainer */
} PkmnSpec;

/* Regional Uttarakhand Variants paired with Gen 1-3 Classics & Moves */
const PkmnSpec pkmn_db[10] = {
    {"BURANSH (TORCHIC)",    "EMBER",        16, "FLAMETHROWER", 24, 65,  8},
    {"MONAL (ARTICUNO)",     "ICE BEAM",     18, "AERIAL ACE",   15, 55,  8},
    {"KASTURI (STANTLER)",   "RAZOR LEAF",   15, "TAKE DOWN",    18, 52, 12},
    {"GURAL (GRAVELER)",     "ROCK SLIDE",   17, "EARTHQUAKE",   22, 60, 12},
    {"BARF-LEOPARD (ABSOL)", "SLASH",        18, "SHADOW BALL",  20, 64, 12},
    {"PIKACHU (GEN 1)",      "THUNDERBOLT",  19, "QUICK ATTACK", 12, 48,  8},
    {"MACHOKE (GEN 1)",      "CROSS CHOP",   21, "SEISMIC TOSS", 16, 62, 12},
    {"MARSHTOMP (GEN 3)",    "SURF",         20, "MUD SHOT",     15, 60, 12},
    {"GARDEVOIR (GEN 3)",    "PSYCHIC",      22, "MAGICAL LEAF", 16, 65,  8},
    {"METAGROSS (GEN 3)",    "METEOR MASH",  25, "HYPER BEAM",   28, 80, 12}
};

typedef struct {
    const char* town_name;
    const char* npc_title;
    const char* npc_msg;
    int badge_idx;        /* -1 if no badge required/awarded here, 0..7 for 8 Gyms */
    const char* leader;
    u8 boss_pkmn;
} TownMeta;

const TownMeta towns[16] = {
    {"01:SRINAGAR GARHWAL", "PROF. RAWAT",  "Welcome! Take BURANSH (Torchic) & reach Dehradun!", -1, NULL, 0},
    {"02:DEVPRAYAG SANGAM", "PRAYAG MONK",  "Alaknanda & Bhagirathi meet here to form Ganga!", -1, NULL, 7},
    {"03:PAURI GARHWAL",    "PAURI GYM",    "Dad's Forest Gym! Prove your bond with Kasturi!", 0, "LEADER VIKRAM", 2},
    {"04:RISHIKESH BRIDGE", "SWAMI'S GYM",  "Lakshman Jhula Rock Gym! Steady your mind!", 1, "LEADER SWAMI", 3},
    {"05:LANSDOWNE CAMP",   "REGIMENT GYM", "Garhwali Regiment Camp! Stand firm, soldier!", 2, "LEADER MAJOR", 6},
    {"06:HARIDWAR GHAT",    "HAR KI PAURI", "Ganga Aarti illuminates the sacred waters!", -1, NULL, 7},
    {"07:ROORKEE TECH",     "IIT TECH GYM", "High-voltage circuits of IIT Roorkee!", 3, "DR. ROY", 5},
    {"08:ALMORA RIDGE",     "KUMAON ELDER", "Breathe the crisp pine air of Almora hills.", -1, NULL, 2},
    {"09:JOSHIMATH BASE",   "MOUNTAINEER",  "Rest here before ascending the high peaks!", -1, NULL, 3},
    {"10:BADRINATH DHAM",   "TAPT KUND GYM","Steam rises from Tapt Kund thermal springs!", 4, "ACHARYA DEV", 0},
    {"11:NAINITAL LAKE",    "NAINI GYM",    "Soar above Naini Lake with Himalayan Monal!", 5, "LEADER NANDA", 1},
    {"12:MUSSOORIE HILLS",  "CONTEST JUDGE","Welcome to Mussoorie, Queen of the Hills!", -1, NULL, 5},
    {"13:KEDARNATH PEAKS",  "KEDAR GYM",    "Mandakini roars beside Kedarnath Temple!", 6, "SAGE RUDRA", 8},
    {"14:VALLEY OF FLOWERS","BOTANIST",     "Rare alpine blooms carpet this glacial valley.", -1, NULL, 2},
    {"15:AULI SNOW SLOPES", "AULI SKI GYM", "Master the icy slopes of Auli crater!", 7, "HIMANSHU", 4},
    {"16:DEHRADUN LEAGUE",  "CHAMPION HALL","Final Challenge: Uttarakhand Pokemon League!", 8, "CHAMPION DEV", 9}
};

/* =========================================================================
 * 4. GLOBAL GAME STATE & 8 BADGE VARIABLES
 * ========================================================================= */

int cur_map = 0;
int player_x = 4, player_y = 10;
int player_hp = 75, player_max_hp = 75;
int caught_count = 1;

/* 8 Explicit Badge Variables for the 8 Regional Gyms + Champion Flag */
u8 badge_pauri     = 0; /* Badge 1: Pauri Forest Gym */
u8 badge_rishikesh = 0; /* Badge 2: Rishikesh Lakshman Jhula Gym (Swami) */
u8 badge_lansdowne = 0; /* Badge 3: Lansdowne Regiment Gym (Major) */
u8 badge_roorkee   = 0; /* Badge 4: IIT Roorkee Gym (Dr. Roy) */
u8 badge_badrinath = 0; /* Badge 5: Badrinath Tapt Kund Gym */
u8 badge_nainital  = 0; /* Badge 6: Nainital High Peaks Gym */
u8 badge_kedarnath = 0; /* Badge 7: Kedarnath Temple Peaks Gym */
u8 badge_auli      = 0; /* Badge 8: Auli Snow Slopes Gym (Himanshu) */
u8 league_champ    = 0;

OBJ_ATTR obj_buffer[128];
u16 prev_keys = 0;

/* Helper to read/write the 8 badge variables by index */
u8 get_badge(int idx) {
    switch (idx) {
        case 0: return badge_pauri;
        case 1: return badge_rishikesh;
        case 2: return badge_lansdowne;
        case 3: return badge_roorkee;
        case 4: return badge_badrinath;
        case 5: return badge_nainital;
        case 6: return badge_kedarnath;
        case 7: return badge_auli;
        case 8: return league_champ;
        default: return 1;
    }
}

void set_badge(int idx) {
    switch (idx) {
        case 0: badge_pauri = 1; break;
        case 1: badge_rishikesh = 1; break;
        case 2: badge_lansdowne = 1; break;
        case 3: badge_roorkee = 1; break;
        case 4: badge_badrinath = 1; break;
        case 5: badge_nainital = 1; break;
        case 6: badge_kedarnath = 1; break;
        case 7: badge_auli = 1; break;
        case 8: league_champ = 1; break;
    }
}

int total_badges(void) {
    return badge_pauri + badge_rishikesh + badge_lansdowne + badge_roorkee +
           badge_badrinath + badge_nainital + badge_kedarnath + badge_auli;
}

/* Poll hardware REG_KEYINPUT directly for newly pressed keys */
u16 poll_keys_pressed(void) {
    u16 cur = ~REG_KEYINPUT & KEY_MASK;
    u16 pressed = cur & ~prev_keys;
    prev_keys = cur;
    return pressed;
}

void wait_frames(int frames) {
    for (int i = 0; i < frames; i++) VBlankIntrWait();
}

/* =========================================================================
 * 5. VRAM MAP LOADING, SPRITES & DIALOGUE ENGINE
 * ========================================================================= */

void draw_hud(void) {
    char buf[40];
    tte_erase_rect(0, 0, 240, 16);
    snprintf(buf, sizeof(buf), "#{P:4,2}%s B:%d/8", towns[cur_map].town_name, total_badges());
    tte_write(buf);
}

void load_map_vram(int map_idx) {
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 32; x++) {
            se_mem[28][y * 32 + x] = map_data[map_idx][y][x];
        }
    }
    draw_hud();
}

void update_sprites(int show_enemy, u8 enemy_tile) {
    /* OAM 0: Player 16x16 Sprite */
    obj_set_attr(&obj_buffer[0],
        ATTR0_SQUARE | ATTR0_4BPP | ATTR0_Y(player_y * 8 - 4),
        ATTR1_SIZE_16 | ATTR1_X(player_x * 8 - 4),
        ATTR2_ID(0) | ATTR2_PALBANK(0));

    /* OAM 1: Battle Enemy / Wild Pokemon Sprite when in combat */
    if (show_enemy) {
        obj_set_attr(&obj_buffer[1],
            ATTR0_SQUARE | ATTR0_4BPP | ATTR0_Y(36),
            ATTR1_SIZE_16 | ATTR1_X(176),
            ATTR2_ID(enemy_tile) | ATTR2_PALBANK(0));
    } else {
        obj_hide(&obj_buffer[1]);
    }
    oam_copy(oam_mem, obj_buffer, 2);
}

/* Bottom-screen dialogue window routine */
void show_dialogue(const char* title, const char* text) {
    char buf[160];
    tte_erase_rect(0, 112, 240, 160);
    snprintf(buf, sizeof(buf), "#{P:8,116}[%s]\n#{P:8,128}%s\n#{P:8,144}(PRESS A)", title, text);
    tte_write(buf);

    while (1) {
        VBlankIntrWait();
        if (poll_keys_pressed() & (KEY_A | KEY_B | KEY_START)) break;
    }
    tte_erase_rect(0, 112, 240, 160);
    draw_hud();
}

/* =========================================================================
 * 6. COMBAT ENGINE (WILD ENCOUNTERS & GYM LEADERS)
 * ========================================================================= */

void screen_flash(void) {
    for (int i = 0; i < 3; i++) {
        pal_bg_mem[1] = RGB15(31, 31, 31);
        wait_frames(5);
        pal_bg_mem[1] = game_palette[1];
        wait_frames(5);
    }
}

/* Returns 1 if player wins/catches, 0 if defeated */
int run_battle(u8 pkmn_idx, int is_gym, const char* leader_name) {
    const PkmnSpec* foe = &pkmn_db[pkmn_idx];
    const PkmnSpec* hero = &pkmn_db[0]; /* Player's partner: Buransh (Torchic) */
    int foe_hp = foe->max_hp + (is_gym ? 15 : 0);
    int cursor = 0;
    char msg[160];

    screen_flash();
    update_sprites(1, is_gym ? 4 : foe->sprite_id);

    if (is_gym) {
        snprintf(msg, sizeof(msg), "%s sent out %s!", leader_name, foe->name);
        show_dialogue("GYM CHALLENGE", msg);
    } else {
        snprintf(msg, sizeof(msg), "Wild %s appeared!", foe->name);
        show_dialogue("HIMALAYAN WILD", msg);
    }

    while (foe_hp > 0 && player_hp > 0) {
        tte_erase_rect(0, 104, 240, 160);
        snprintf(msg, sizeof(msg),
            "#{P:6,106}FOE %s HP:%d\n"
            "#{P:6,118}YOU %s HP:%d/%d\n"
            "#{P:6,132}%c1:%s  %c2:%s\n"
            "#{P:6,144}%c3:%s",
            foe->name, foe_hp,
            hero->name, player_hp, player_max_hp,
            (cursor == 0) ? '>' : ' ', hero->move1,
            (cursor == 1) ? '>' : ' ', hero->move2,
            (cursor == 2) ? '>' : ' ', is_gym ? "HEAL HERB" : "CATCH/SPARE");
        tte_write(msg);

        VBlankIntrWait();
        u16 keys = poll_keys_pressed();
        if (keys & (KEY_RIGHT | KEY_DOWN)) cursor = (cursor + 1) % 3;
        if (keys & (KEY_LEFT | KEY_UP))    cursor = (cursor + 2) % 3;

        if (keys & KEY_A) {
            if (cursor == 0 || cursor == 1) {
                const char* mname = (cursor == 0) ? hero->move1 : hero->move2;
                int dmg = (cursor == 0) ? hero->dmg1 : hero->dmg2;
                dmg += total_badges() * 2;
                foe_hp -= dmg;
                if (foe_hp < 0) foe_hp = 0;
                snprintf(msg, sizeof(msg), "Used %s! (%d DMG)", mname, dmg);
                show_dialogue("ATTACK", msg);
            } else {
                if (!is_gym) {
                    if (foe_hp <= foe->max_hp / 2 || (rand() % 2 == 0)) {
                        caught_count++;
                        snprintf(msg, sizeof(msg), "Caught/Spared %s! (Dex:%d)", foe->name, caught_count);
                        show_dialogue("SUCCESS", msg);
                        update_sprites(0, 0);
                        return 1;
                    } else {
                        show_dialogue("ESCAPED BALL", "It broke free! Weaken it first!");
                    }
                } else {
                    player_hp += 28;
                    if (player_hp > player_max_hp) player_hp = player_max_hp;
                    show_dialogue("SANJEEVANI HERB", "Restored 28 HP with mountain herbs!");
                }
            }

            /* Enemy Turn */
            if (foe_hp > 0) {
                int use_m2 = rand() & 1;
                const char* emove = use_m2 ? foe->move2 : foe->move1;
                int edmg = (use_m2 ? foe->dmg2 : foe->dmg1) / 2;
                player_hp -= edmg;
                if (player_hp < 0) player_hp = 0;
                snprintf(msg, sizeof(msg), "Foe used %s! (-%d HP)", emove, edmg);
                show_dialogue(foe->name, msg);
            }
        }
    }

    update_sprites(0, 0);
    if (player_hp <= 0) {
        player_hp = player_max_hp;
        show_dialogue("BLACKED OUT", "Recovered at Tapt Kund Hot Springs!");
        return 0;
    }

    player_max_hp += 4;
    player_hp = player_max_hp;
    show_dialogue("VICTORY!", "Enemy fainted! Buransh grew stronger!");
    return 1;
}

/* =========================================================================
 * 7. INTERACTION & NAVIGATION LOGIC
 * ========================================================================= */

void interact_tile(u8 tile) {
    const TownMeta* tm = &towns[cur_map];
    if (tile == T_GYM || tile == T_TRAINER) {
        if (tm->badge_idx >= 0) {
            if (get_badge(tm->badge_idx)) {
                show_dialogue(tm->npc_title, "You have already mastered this Gym!");
            } else {
                show_dialogue(tm->npc_title, tm->npc_msg);
                if (run_battle(tm->boss_pkmn, 1, tm->leader)) {
                    set_badge(tm->badge_idx);
                    if (tm->badge_idx == 8) {
                        show_dialogue("DEHRADUN LEAGUE", "CONGRATS! You are Uttarakhand Champion!");
                    } else {
                        show_dialogue("BADGE EARNED!", "New Uttarakhand Gym Badge unlocked!");
                    }
                }
            }
        } else {
            player_hp = player_max_hp;
            show_dialogue(tm->npc_title, tm->npc_msg);
        }
    }
}

void try_move(int dx, int dy) {
    int nx = player_x + dx;
    int ny = player_y + dy;

    /* Screen edge transition checks */
    if (nx < 0) {
        if (cur_map > 0) {
            cur_map--;
            player_x = 30;
            load_map_vram(cur_map);
        }
        return;
    }
    if (nx > 31) {
        const TownMeta* tm = &towns[cur_map];
        if (tm->badge_idx >= 0 && tm->badge_idx < 8 && !get_badge(tm->badge_idx)) {
            show_dialogue("ROUTE BLOCKED", "Defeat this town's Gym Master to advance East!");
            return;
        }
        if (cur_map < 15) {
            cur_map++;
            player_x = 1;
            load_map_vram(cur_map);
            show_dialogue(towns[cur_map].town_name, towns[cur_map].npc_msg);
        }
        return;
    }
    if (ny < 0 || ny > 19) return;

    u8 dest = map_data[cur_map][ny][nx];

    /* Stop on obstacles (Himalayan cliffs, Ganga water, or Gym buildings) */
    if (dest == T_WALL || dest == T_WATER) return;
    if (dest == T_GYM || dest == T_TRAINER) {
        interact_tile(dest);
        return;
    }

    player_x = nx;
    player_y = ny;

    /* Stepping onto Exit Milestone (3) at map edges */
    if (dest == T_EXIT) {
        if (player_x == 31) try_move(1, 0);
        else if (player_x == 0) try_move(-1, 0);
        return;
    }

    /* Random Wild Encounter on Grass (0) */
    if (dest == T_GRASS && (rand() % 18 == 0)) {
        /* Pick regional Monal, Barf-Leopard, Kasturi, Gural, or Gen 1-3 wilds */
        u8 wild_pool[6] = {1, 4, 2, 3, 5, 7};
        run_battle(wild_pool[rand() % 6], 0, NULL);
    }
}

/* =========================================================================
 * 8. MAIN HARDWARE INITIALIZATION & GAME LOOP
 * ========================================================================= */

int main(void) {
    /* Initialize Interrupts for VBlank synchronization */
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    /* Configure Display Control Register: Mode 0, BG0 (Map), BG1 (Text), OBJ */
    REG_DISPCNT = BG0_ENABLE | BG1_ENABLE | OBJ_ENABLE | MODE0;

    /* Setup BG0 for 32x32 4bpp Map Tiles (Charblock 0, Screenblock 28) */
    REG_BG0CNT = BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_REG_32x32;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    /* Load Palette & Programmatic 4bpp Tiles into VRAM */
    memcpy(pal_bg_mem, game_palette, sizeof(game_palette));
    memcpy(pal_obj_mem, game_palette, sizeof(game_palette));
    memcpy(&tile_mem[0][0], bg_tiles, sizeof(bg_tiles));
    memcpy(&tile_mem[4][0], sprite_tiles, sizeof(sprite_tiles));

    /* Setup BG1 Text Engine (Charblock 2, Screenblock 30) */
    tte_init_se_default(1, BG_CBB(2) | BG_SBB(30));

    /* Initialize OAM Sprite Buffer */
    oam_init(obj_buffer, 128);
    load_map_vram(cur_map);
    update_sprites(0, 0);

    show_dialogue("DEV BHOOMI", "Start in Srinagar Garhwal! Reach Dehradun League!");

    while (1) {
        VBlankIntrWait();
        u16 keys = poll_keys_pressed();

        if (keys & KEY_UP)    try_move(0, -1);
        else if (keys & KEY_DOWN)  try_move(0, 1);
        else if (keys & KEY_LEFT)  try_move(-1, 0);
        else if (keys & KEY_RIGHT) try_move(1, 0);
        else if (keys & KEY_A) {
            /* Check adjacent tiles for NPCs or Gym buildings */
            if (player_y > 0)  interact_tile(map_data[cur_map][player_y - 1][player_x]);
            if (player_y < 19) interact_tile(map_data[cur_map][player_y + 1][player_x]);
            if (player_x > 0)  interact_tile(map_data[cur_map][player_y][player_x - 1]);
            if (player_x < 31) interact_tile(map_data[cur_map][player_y][player_x + 1]);
        }

        update_sprites(0, 0);
    }

    return 0;
}
