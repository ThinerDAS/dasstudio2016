#ifndef MTGUI_HEADER
#define MTGUI_HEADER

#include "mtstat.h"

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include <SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#else
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

class mtgui
{
    const static int SCREEN_WIDTH = 800;
    const static int SCREEN_HEIGHT = 600;
    const static bool FULLSCREEN = false;

    mtstat orig_stat; // In order to implement resetting mts.

    mtstat cur_stat;

    std::string step_traces;

    void trace_rebuild ( std::string str );
    void trace_append ( std::string str );
    void LSP ( int i );
    void transit_to_tower ( int id = -1 );

    enum font
    {
        UBUNTU_MONO_R_TTF,
        FREE_MONO_BOLD_TTF,
        UBUNTU_MONO_R_TTF_HUGE,
        UBUNTU_MONO_R_TTF_SMALL,
        FREE_MONO_BOLD_TTF_SMALL,
        UBUNTU_MONO_R_TTF_TINY,
        FREE_MONO_BOLD_TTF_TINY,
        FONT_COUNT
    };
    enum img
    {
        MTICON_PNG,
        ITEM_PNG,
        BG_PNG,
        IMG_COUNT
    };
    enum sound
    {
        DOOR_OGG,
        CRYSTAL_OGG,
        SHOP_OGG,
        POTION_OGG,
        BOMB_OGG,
        TREASURE_OGG,
        KEY_OGG,
        PORTAL_OGG,
        WALK_OGG,
        BATTLE_OGG,
        BGM_OGG,
        WALL_OGG,
        SOUND_COUNT
    };
    TTF_Font* fonts[FONT_COUNT];
    SDL_Surface* imgs[IMG_COUNT];
    Mix_Chunk* sounds[SOUND_COUNT];

    enum layer
    {
        LAYER_SCREEN_SURFACE, // The final screen
        LAYER_BG,
        LAYER_MAP,
        LAYER_STAT,
        LAYER_UI,
        DUMMY_LAYER_UNDER_HINT,
        LAYER_COUNT
    };
    SDL_Window* gWindow;
    SDL_Surface* layers[LAYER_COUNT];
    bool layer_need_update[LAYER_COUNT];

    enum UI
    {
        UI_MAP,
        UI_SHOP,
        UI_MOB_PEDIA,
        UI_LOAD,
        UI_SAVE,
        UI_PLAY,
        UI_CLIPBOARD,
        UI_PLAYING,
        UI_MTID,
        UI_WAITING
    };

    UI cur_ui;
    //int ui_val;

    bool load_codecs();
    bool load_assets();
    bool init_mainUI();
    void renderTile ( int map_x, int map_y, int tile_ID );
    void initBG ( SDL_Surface* bg, SDL_Surface* wallpaper );
    void update_map();
    void update_stat();
    void update_strace();
    void update_ui();
    std::string hint_str;
    void under_hint ( std::string str );
    //std::string get_buffer_clipboard();
    void update_screen(); // every frame you only need to call this function directly, not the previous ones.
    bool process_keydown ( SDL_Keycode sym, bool emulate_map = false );

    bool mutebgm;
    bool mutese;
    int bgm_channel; // to store the channel that plays the bgm.
    void playSound ( sound s );

    enum direction
    {
        up,
        down,
        left,
        right
    };

    direction face;

    void walk ( direction d );

    bool parity;

    bool quit;

    void deinit();

public:

    void enterFrame();
    void run();
    void initialize_mtstat ( const char* stream );
    //#ifdef __EMSCRIPTEN__
    void onload_config();
    void onsave_config();
    void onfail_load_config();
    void onfail_save_config();
    //#endif
};

#endif
