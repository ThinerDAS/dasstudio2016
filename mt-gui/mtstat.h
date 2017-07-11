#ifndef MTSTAT_HEADER
#define MTSTAT_HEADER

#include <vector>
#include <array>

class mtstat
{
public:
    enum Spec
    {
        CN2 = 1, CN3, CN4, CN5,
        CNMASK = 0x0f,
        MA = 0x10,
        SD = 0x20,
        SA = 0x40,
        BF = 0x80,
        SPEC_EFFECTIVE_MASK=0xff
    };

    /*
    const static int SPEC_MA       = 0x10; // Magic attack
    const static int SPEC_SD       = 0x20; // Solid
    const static int SPEC_SA       = 0x40; // Speedy Attack
    const static int SPEC_BF       = 0x80; // Boss Flag, immune to bomb
    const static int SPEC_CNMASK   = 0x0f; // Chain Attack Mask
    const static int SPEC_CN2      = 0x01; // Chain Attack, 2x
    const static int SPEC_CN3      = 0x02; // 3x
    const static int SPEC_CN4      = 0x03; // 4x
    const static int SPEC_CN5      = 0x04; // 5x
    */

    enum Tile
    {
        EMPTY = 0,
        REDCRY = 0x08,
        BLUECRY = 0x10,
        GREENCRY = 0x18,
        WHITECRY = 0x20,
        POTION = 0x28,
        YELKEY = 0x38,
        BLUEKEY,
        REDKEY,
        GOLD,
        TOKEN,
        PICKAXE,
        BOMB,
        FLAG,
        YELDOOR = 0x60,
        BLUEDOOR,
        REDDOOR,
        WALL,
        SOLIDWALL,
        MOB = 0x80,
        TILE_EFFECTIVE_MASK=0xff
    };

    /*
    const static int TILE_EMPTY    = 0x00;
    const static int TILE_REDCRY   = 0x08;
    const static int TILE_BLUECRY  = 0x10;
    const static int TILE_GREENCRY = 0x18;
    const static int TILE_WHITECRY = 0x20;
    const static int TILE_POTION   = 0x28;

    const static int TILE_YELKEY   = 0x38;
    const static int TILE_BLUEKEY  = 0x39;
    const static int TILE_REDKEY   = 0x3a;
    const static int TILE_GOLD     = 0x3b;
    const static int TILE_TOKEN    = 0x3c;
    const static int TILE_PICKAXE  = 0x3d;
    const static int TILE_BOMB     = 0x3e;
    const static int TILE_FLAG     = 0x3f;

    const static int TILE_YELDOOR  = 0x60;
    const static int TILE_BLUEDOOR = 0x61;
    const static int TILE_REDDOOR  = 0x62;
    const static int TILE_WALL     = 0x63;
    const static int TILE_SOLIDWALL = 0x64;

    const static int TILE_MOB      = 0x80;
    */

    const static int MAPSIZE = 15;

    const static int THRESHOLD_STAIRS = 6;
    typedef std::array<std::array<int, MAPSIZE>, MAPSIZE> map;
    typedef std::array<int, 5> enemy;
    typedef std::array<int, 2> portal_pair;
    int score_threshold[THRESHOLD_STAIRS];
    int brave_hp;
    int brave_at;
    int brave_df;
    int brave_mf;
    int brave_yellowkey;
    int brave_bluekey;
    int brave_redkey;
    int brave_gold;
    int brave_tokens;
    int brave_pickaxe;
    int brave_bomb;
    int brave_x;
    int brave_y;
    int brave_floor;
    int unit_hp;
    int unit_at;
    int unit_df;
    int unit_mf;
    int fanfare_flag;
    int reg;
    //int total_floors;
    //int total_enemies;
    //int total_portal_pairs;
    std::vector<map> maps;
    std::vector<enemy> enemies;
    std::vector<portal_pair> portals;
    //void drawTile1 ( int event ) const;
    //void drawTile2 ( int event ) const;

public:
    mtstat();
    bool init_from_stream ( const char* stream );
    bool process_char ( char action );
    bool goto_coord ( int f, int x, int y );
    bool process_from_stream ( const char* stream, size_t n );
    int dam ( int mon_ID ) const;
    int score() const;
    //void drawOnConsole() const;
};

#endif
