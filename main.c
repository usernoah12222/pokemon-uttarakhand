/* POKEMON EMERALD: DEV BHOOMI UTTARAKHAND EDITION (GBA HOMEBREW) */
#include <tonc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODE0       DCNT_MODE0
#define BG0_ENABLE  DCNT_BG0
#define BG1_ENABLE  DCNT_BG1
#define OBJ_ENABLE  (DCNT_OBJ | DCNT_OBJ_1D)

#define T_GRASS   0
#define T_WALL    1
#define T_WATER   2
#define T_EXIT    3
#define T_GYM     4
#define T_TRAINER 5

/* 1. 16-Color Hardware Palette (15-bit BGR Hex Constants) */
const u16 game_palette[16] = {
    0x0000, 0x0246, 0x01A4, 0x194E, 0x0CC9, 0x6984, 0x7A8C, 0x7FDE,
    0x12EC, 0x14BA, 0x3E9C, 0x4905, 0x11D6, 0x60D2, 0x62D4, 0x7FFF
};

/* 8x8 Background Tileset: Grass, Wall, Water, Exit, Gym, Trainer */
const TILE bg_tiles[6] = {
    {{0x11111111,0x12111121,0x11211211,0x11111111,0x11112111,0x12111211,0x11111111,0x11121111}},
    {{0x47777774,0x77733777,0x73333337,0x33444333,0x34433443,0x43333334,0x33444433,0x44444444}},
    {{0x55555555,0x56655665,0x65566556,0x55555555,0x55665566,0x56556555,0x55555555,0x66555566}},
    {{0x11888811,0x18877881,0x88788788,0x88888888,0x11833811,0x11833811,0x11833811,0x11888811}},
    {{0x11188111,0x11999911,0x19988991,0x99999999,0x37788773,0x37444473,0x37488473,0x33488433}},
    {{0x11999911,0x19977991,0x11AAAA11,0x11A00A11,0x19BBBB91,0x19BBBB91,0x11B11B11,0x11411411}}
};

/* 16x16 OAM Sprites: Player (0-3), Gym Leader (4-7), Monal (8-11), Barf-Leopard (12-15) */
const TILE sprite_tiles[16] = {
    {{0x000BBBBB,0x00BBBB8B,0x00BBBBBB,0x00AAAAAA,0x00AA0AAA,0x00AAAAAA,0x000AAAA0,0x00CCCCCC}},
    {{0xBBBBB000,0xB8BBBB00,0xBBBBBB00,0xAAAAAA00,0xAAA0AA00,0xAAAAAA00,0x0AAAA000,0xCCCCCC00}},
    {{0x0CCBBBBB,0x0CCBBBBB,0x0CCBBBBB,0x00BBBBBB,0x000BB0BB,0x000BB0BB,0x00044044,0x00044044}},
    {{0xBBBBBCC0,0xBBBBBCC0,0xBBBBBCC0,0xBBBBBB00,0xBB0BB000,0xBB0BB000,0x44044000,0x44044000}},
    {{0x00088888,0x00999999,0x00998899,0x00AAAAAA,0x00AA0AAA,0x00AAAAAA,0x00099990,0x00999999}},
    {{0x88888000,0x99999900,0x99889900,0xAAAAAA00,0xAAA0AA00,0xAAAAAA00,0x09999000,0x99999900}},
    {{0x09988889,0x09988889,0x00999999,0x00999999,0x00099099,0x00099099,0x00033033,0x00033033}},
    {{0x98888990,0x98888990,0x99999900,0x99999900,0x99099000,0x99099000,0x33033000,0x33033000}},
    {{0x00880880,0x00088800,0x000DDDDD,0x00DD70DD,0x088DDDDD,0x888DDDDD,0x00055555,0x00552255}},
    {{0x00000000,0x00000000,0xDDDD0000,0xDDDDD000,0xDDDDDD00,0x55559990,0x55229999,0x55222999}},
    {{0x00552225,0x00055222,0x00005555,0x00000808,0x00000808,0x00008808,0x00000000,0x00000000}},
    {{0x22299900,0x22999000,0x55990000,0x80800000,0x80800000,0x80880000,0x00000000,0x00000000}},
    {{0x00770077,0x07E777E7,0x07EEEEE7,0x07E9E9E7,0x07EE0EE7,0x007EEE70,0x007E4E77,0x07EEEEE4}},
    {{0x00000000,0x00000000,0x00000077,0x000007E7,0x00007E70,0x7777E700,0xE4EEE700,0xEE4EE700}},
    {{0x07E4EEE7,0x07EE4EE7,0x00777777,0x007E07E0,0x007E07E0,0x00770770,0x00000000,0x00000000}},
    {{0xEEE4E700,0xE4EEE700,0x77777000,0x0E70E700,0x0E70E700,0x07707700,0x00000000,0x00000000}}
};

/* 2. 16 Uttarakhand Regional Maps (20x32 Tiles Each) */
#define R_MTN {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
#define R_VAL {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
#define R_PK1 {1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1}
#define R_PK2 {1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1}
#define R_RIV {1,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,1}
#define R_BRG {1,2,2,2,2,0,0,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,0,0,2,2,2,2,2,2,1}
#define R_LAK {1,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,1}
#define R_GYM {1,0,0,0,0,0,0,0,1,1,4,1,1,0,0,0,0,0,1,1,4,1,1,0,0,0,0,0,0,0,0,1}
#define R_NPC {1,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
#define R_LAB {1,0,0,0,1,4,1,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,1,0,0,0,0,1}
#define R_RD0 {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}
#define R_RDM {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}
#define R_RDE {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}

const u8 map_data[16][20][32] = {
    {R_MTN,R_PK1,R_VAL,R_VAL,R_LAB,R_VAL,R_VAL,R_VAL,R_VAL,R_VAL,R_RD0,R_VAL,R_VAL,R_RIV,R_RIV,R_VAL,R_VAL,R_PK2,R_MTN,R_MTN},
    {R_MTN,R_RIV,R_RIV,R_BRG,R_VAL,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_RIV,R_RIV,R_BRG,R_RIV,R_VAL,R_PK1,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_PK2,R_VAL,R_GYM,R_NPC,R_VAL,R_PK2,R_VAL,R_VAL,R_RDM,R_VAL,R_VAL,R_PK1,R_VAL,R_VAL,R_PK2,R_PK1,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_RIV,R_BRG,R_GYM,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_RIV,R_BRG,R_RIV,R_VAL,R_VAL,R_PK2,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_PK1,R_PK2,R_GYM,R_NPC,R_VAL,R_VAL,R_PK2,R_VAL,R_RDM,R_VAL,R_PK2,R_VAL,R_VAL,R_PK1,R_PK2,R_PK1,R_MTN,R_MTN},
    {R_MTN,R_VAL,R_RIV,R_RIV,R_BRG,R_GYM,R_NPC,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_RIV,R_RIV,R_RIV,R_BRG,R_VAL,R_VAL,R_MTN,R_MTN},
    {R_MTN,R_VAL,R_LAB,R_VAL,R_GYM,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_VAL,R_LAB,R_VAL,R_VAL,R_VAL,R_PK2,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_PK2,R_VAL,R_LAB,R_VAL,R_VAL,R_PK2,R_VAL,R_VAL,R_RDM,R_VAL,R_VAL,R_PK1,R_VAL,R_VAL,R_PK2,R_MTN,R_MTN,R_MTN},
    {R_MTN,R_MTN,R_PK1,R_PK2,R_LAB,R_VAL,R_VAL,R_PK1,R_VAL,R_VAL,R_RDM,R_VAL,R_PK2,R_PK1,R_VAL,R_PK1,R_MTN,R_MTN,R_MTN,R_MTN},
    {R_MTN,R_MTN,R_PK1,R_LAK,R_GYM,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_LAK,R_VAL,R_PK1,R_MTN,R_MTN,R_MTN,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_LAK,R_LAK,R_GYM,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_LAK,R_LAK,R_LAK,R_VAL,R_PK2,R_PK1,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_VAL,R_LAB,R_GYM,R_NPC,R_VAL,R_VAL,R_PK2,R_VAL,R_RDM,R_VAL,R_VAL,R_LAB,R_VAL,R_PK2,R_PK1,R_MTN,R_MTN,R_MTN},
    {R_MTN,R_MTN,R_MTN,R_PK1,R_GYM,R_NPC,R_VAL,R_PK2,R_VAL,R_VAL,R_RDM,R_VAL,R_RIV,R_BRG,R_PK1,R_MTN,R_MTN,R_MTN,R_MTN,R_MTN},
    {R_MTN,R_PK2,R_VAL,R_VAL,R_VAL,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDM,R_VAL,R_VAL,R_LAK,R_VAL,R_VAL,R_VAL,R_PK2,R_MTN,R_MTN},
    {R_MTN,R_MTN,R_PK1,R_LAK,R_GYM,R_NPC,R_VAL,R_PK1,R_VAL,R_VAL,R_RDM,R_VAL,R_LAK,R_PK2,R_PK1,R_MTN,R_MTN,R_MTN,R_MTN,R_MTN},
    {R_MTN,R_PK1,R_LAB,R_GYM,R_GYM,R_NPC,R_VAL,R_VAL,R_VAL,R_VAL,R_RDE,R_VAL,R_VAL,R_LAB,R_VAL,R_VAL,R_PK1,R_MTN,R_MTN,R_MTN}
};

/* 3. Pokemon & Uttarakhand Town Data */
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
    {"MACHOKE (GEN 1)",      "CROSS CHOP",   21, "SEISMIC TOSS", 16, 62, 12},
    {"MARSHTOMP (GEN 3)",    "SURF",         20, "MUD SHOT",     15, 60, 12},
    {"GARDEVOIR (GEN 3)",    "PSYCHIC",      22, "MAGICAL LEAF", 16, 65,  8},
    {"METAGROSS (GEN 3)",    "METEOR MASH",  25, "HYPER BEAM",   28, 80, 12}
};

typedef struct {
    const char* town_name;
    const char* npc_title;
    const char* npc_msg;
    int badge_idx;
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

/* 4. Global State & 8 Badge Variables */
int cur_map = 0, player_x = 4, player_y = 10, player_hp = 75, player_max_hp = 75, caught_count = 1;
u8 badge_pauri = 0, badge_rishikesh = 0, badge_lansdowne = 0, badge_roorkee = 0;
u8 badge_badrinath = 0, badge_nainital = 0, badge_kedarnath = 0, badge_auli = 0, league_champ = 0;
OBJ_ATTR obj_buffer[128];
u16 prev_keys = 0;

u8 get_badge(int idx) {
    switch (idx) {
        case 0: return badge_pauri;     case 1: return badge_rishikesh;
        case 2: return badge_lansdowne; case 3: return badge_roorkee;
        case 4: return badge_badrinath; case 5: return badge_nainital;
        case 6: return badge_kedarnath; case 7: return badge_auli;
        case 8: return league_champ;    default: return 1;
    }
}

void set_badge(int idx) {
    switch (idx) {
        case 0: badge_pauri = 1; break;     case 1: badge_rishikesh = 1; break;
        case 2: badge_lansdowne = 1; break; case 3: badge_roorkee = 1; break;
        case 4: badge_badrinath = 1; break; case 5: badge_nainital = 1; break;
        case 6: badge_kedarnath = 1; break; case 7: badge_auli = 1; break;
        case 8: league_champ = 1; break;
    }
}

int total_badges(void) {
    return badge_pauri + badge_rishikesh + badge_lansdowne + badge_roorkee +
           badge_badrinath + badge_nainital + badge_kedarnath + badge_auli;
}

u16 poll_keys_pressed(void) {
    u16 cur = ~REG_KEYINPUT & KEY_MASK;
    u16 pressed = cur & ~prev_keys;
    prev_keys = cur;
    return pressed;
}

void wait_frames(int frames) {
    for (int i = 0; i < frames; i++) VBlankIntrWait();
}

void draw_hud(void) {
    char buf[40];
    tte_erase_rect(0, 0, 240, 16);
    snprintf(buf, sizeof(buf), "#{P:4,2}%s B:%d/8", towns[cur_map].town_name, total_badges());
    tte_write(buf);
}

void load_map_vram(int map_idx) {
    for (int y = 0; y < 20; y++)
        for (int x = 0; x < 32; x++)
            se_mem[28][y * 32 + x] = map_data[map_idx][y][x];
    draw_hud();
}

void update_sprites(int show_enemy, u8 enemy_tile) {
    obj_set_attr(&obj_buffer[0],
        ATTR0_SQUARE | ATTR0_4BPP | ATTR0_Y(player_y * 8 - 4),
        ATTR1_SIZE_16 | ATTR1_X(player_x * 8 - 4),
        ATTR2_ID(0) | ATTR2_PALBANK(0));
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

void screen_flash(void) {
    for (int i = 0; i < 3; i++) {
        pal_bg_mem[1] = 0x7FFF;
        wait_frames(5);
        pal_bg_mem[1] = game_palette[1];
        wait_frames(5);
    }
}

int run_battle(u8 pkmn_idx, int is_gym, const char* leader_name) {
    const PkmnSpec* foe = &pkmn_db[pkmn_idx];
    const PkmnSpec* hero = &pkmn_db[0];
    int foe_hp = foe->max_hp + (is_gym ? 15 : 0), cursor = 0;
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
            "#{P:6,106}FOE %s HP:%d\n#{P:6,118}YOU %s HP:%d/%d\n#{P:6,132}%c1:%s %c2:%s\n#{P:6,144}%c3:%s",
            foe->name, foe_hp, hero->name, player_hp, player_max_hp,
            (cursor == 0) ? '>' : ' ', hero->move1,
            (cursor == 1) ? '>' : ' ', hero->move2,
            (cursor == 2) ? '>' : ' ', is_gym ? "HEAL HERB" : "CATCH/SPARE");
        tte_write(msg);

        VBlankIntrWait();
        u16 keys = poll_keys_pressed();
        if (keys & (KEY_RIGHT | KEY_DOWN)) cursor = (cursor + 1) % 3;
        if (keys & (KEY_LEFT | KEY_UP))    cursor = (cursor + 2) % 3;

        if (keys & KEY_A) {
            if (cursor < 2) {
                const char* mname = (cursor == 0) ? hero->move1 : hero->move2;
                int dmg = ((cursor == 0) ? hero->dmg1 : hero->dmg2) + total_badges() * 2;
                foe_hp = (foe_hp > dmg) ? (foe_hp - dmg) : 0;
                snprintf(msg, sizeof(msg), "Used %s! (%d DMG)", mname, dmg);
                show_dialogue("ATTACK", msg);
            } else if (!is_gym) {
                if (foe_hp <= foe->max_hp / 2 || (rand() % 2 == 0)) {
                    caught_count++;
                    snprintf(msg, sizeof(msg), "Caught/Spared %s! (Dex:%d)", foe->name, caught_count);
                    show_dialogue("SUCCESS", msg);
                    update_sprites(0, 0);
                    return 1;
                }
                show_dialogue("ESCAPED BALL", "It broke free! Weaken it first!");
            } else {
                player_hp += 28;
                if (player_hp > player_max_hp) player_hp = player_max_hp;
                show_dialogue("SANJEEVANI HERB", "Restored 28 HP with mountain herbs!");
            }

            if (foe_hp > 0) {
                int use_m2 = rand() & 1;
                const char* emove = use_m2 ? foe->move2 : foe->move1;
                int edmg = (use_m2 ? foe->dmg2 : foe->dmg1) / 2;
                player_hp = (player_hp > edmg) ? (player_hp - edmg) : 0;
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
                    if (tm->badge_idx == 8)
                        show_dialogue("DEHRADUN LEAGUE", "CONGRATS! You are Uttarakhand Champion!");
                    else
                        show_dialogue("BADGE EARNED!", "New Uttarakhand Gym Badge unlocked!");
                }
            }
        } else {
            player_hp = player_max_hp;
            show_dialogue(tm->npc_title, tm->npc_msg);
        }
    }
}

void try_move(int dx, int dy) {
    int nx = player_x + dx, ny = player_y + dy;
    if (nx < 0) {
        if (cur_map > 0) { cur_map--; player_x = 30; load_map_vram(cur_map); }
        return;
    }
    if (nx > 31) {
        const TownMeta* tm = &towns[cur_map];
        if (tm->badge_idx >= 0 && tm->badge_idx < 8 && !get_badge(tm->badge_idx)) {
            show_dialogue("ROUTE BLOCKED", "Defeat this town's Gym Master to advance East!");
            return;
        }
        if (cur_map < 15) {
            cur_map++; player_x = 1; load_map_vram(cur_map);
            show_dialogue(towns[cur_map].town_name, towns[cur_map].npc_msg);
        }
        return;
    }
    if (ny < 0 || ny > 19) return;

    u8 dest = map_data[cur_map][ny][nx];
    if (dest == T_WALL || dest == T_WATER) return;
    if (dest == T_GYM || dest == T_TRAINER) { interact_tile(dest); return; }

    player_x = nx; player_y = ny;
    if (dest == T_EXIT) {
        if (player_x == 31) try_move(1, 0);
        else if (player_x == 0) try_move(-1, 0);
        return;
    }
    if (dest == T_GRASS && (rand() % 18 == 0)) {
        u8 wild_pool[6] = {1, 4, 2, 3, 5, 7};
        run_battle(wild_pool[rand() % 6], 0, NULL);
    }
}

int main(void) {
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    REG_DISPCNT = BG0_ENABLE | BG1_ENABLE | OBJ_ENABLE | MODE0;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_REG_32x32;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;

    memcpy(pal_bg_mem, game_palette, sizeof(game_palette));
    memcpy(pal_obj_mem, game_palette, sizeof(game_palette));
    memcpy(&tile_mem[0][0], bg_tiles, sizeof(bg_tiles));
    memcpy(&tile_mem[4][0], sprite_tiles, sizeof(sprite_tiles));

    tte_init_se_default(1, BG_CBB(2) | BG_SBB(30));
    oam_init(obj_buffer, 128);
    load_map_vram(cur_map);
    update_sprites(0, 0);

    show_dialogue("DEV BHOOMI", "Start in Srinagar Garhwal! Reach Dehradun League!");

    while (1) {
        VBlankIntrWait();
        u16 keys = poll_keys_pressed();
        if (keys & KEY_UP)         try_move(0, -1);
        else if (keys & KEY_DOWN)  try_move(0, 1);
        else if (keys & KEY_LEFT)  try_move(-1, 0);
        else if (keys & KEY_RIGHT) try_move(1, 0);
        else if (keys & KEY_A) {
            if (player_y > 0)  interact_tile(map_data[cur_map][player_y - 1][player_x]);
            if (player_y < 19) interact_tile(map_data[cur_map][player_y + 1][player_x]);
            if (player_x > 0)  interact_tile(map_data[cur_map][player_y][player_x - 1]);
            if (player_x < 31) interact_tile(map_data[cur_map][player_y][player_x + 1]);
        }
        update_sprites(0, 0);
    }
    return 0;
}
