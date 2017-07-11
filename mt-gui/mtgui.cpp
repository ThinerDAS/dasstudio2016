#include "mtgui.h"
#include "mtstat.h"
#include "encodeabledict/edict.h"
#include "graph/graph.h"
#include "graph/event_vector.h"

#include <string>
#include <vector>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include <SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <emscripten.h>
#else
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>
#include <math.h>
#endif

#include "assets.h"

//TODO: be kind to operations by accident, unintentional permanent loss of data.

static const int MAPX = 160, MAPY = 20;

static const int MOB_PER_PAGE = 5;
static const int SLOT_PER_PAGE = MOB_PER_PAGE;

static const int save_slots = 1000;

static bool show_graph = false;
static graph g;

static int ui_val = 0;

static int tower_id = 0;
static int ui_tower_id = 0;

static std::vector<int> mobs_in_order;
static int effective_mob_count = 0;

static std::string strace_for_playing;

static edict config;

static bool need_to_save_config = false;

static int save_config_clock = 30;

static int octity = 0;

#ifdef __EMSCRIPTEN__

static char* tmp_strdata_buffer = 0;

static int readline ( FILE* f, char* buffer, size_t len )
{
    size_t i;
    memset ( buffer, 0, len );
    for ( i = 0; i < len; i++ )
    {
        int c = fgetc ( f );
        //fprintf (stderr, "%d ", c );
        if ( !feof ( f ) )
        {
            if ( c == '\r' )
            {
                buffer[i] = 0;
            }
            else if ( c == '\n' )
            {
                buffer[i] = 0;
                return i + 1;
            }
            else
            {
                buffer[i] = c;
            }
        }
        else
        {
            //fprintf(stderr, "read_line(): recv returned %d\n", c);
            return -1;
        }
    }
    buffer[len - 1] = 0;
    return -1;
}

#endif

#ifndef __EMSCRIPTEN__
static int fsize ( FILE* fp )
{
    int prev = ftell ( fp );
    fseek ( fp, 0L, SEEK_END );
    int sz = ftell ( fp );
    fseek ( fp, prev, SEEK_SET ); //go back to where we were
    return sz;
}
#endif

std::string indexed ( std::string identifier, int id )
{
    char buf[16];
    sprintf ( buf, "%x", id );
    return identifier + std::string ( buf );
}

std::string indexed ( std::string identifier, int id1, int id2 )
{
    char buf[32];
    sprintf ( buf, "%x;%x", id1, id2 );
    return identifier + std::string ( buf );
}
#ifdef __EMSCRIPTEN__
void onload_strdata ( void* _this, void* buf, int size )
{
    {
        if ( size == 0 )
        {
            goto FAIL;
        }
        edict new_conf ( std::string ( ( char* ) buf, size ) );
        if ( new_conf.size() == 0 )
        {
            goto FAIL;
        }
        config = new_conf; //TODO: check integrity before validate
        ( ( mtgui* ) _this )->onload_config();
    }
    return;
FAIL:
    fprintf ( stderr, "Emscripten: The data from emscripten_idb_async_load function is invalid. " );
    ( ( mtgui* ) _this )->onfail_load_config();
}

void onsave_strdata ( void* _this )
{
    ( ( mtgui* ) _this )->onsave_config();
}

void onerror_load_strdata ( void* _this )
{
    fprintf ( stderr, "Emscripten emscripten_idb_async_load function failed to fetch data. " );
    ( ( mtgui* ) _this )->onfail_load_config();
}

void onerror_save_strdata ( void* _this )
{
    fprintf ( stderr, "Emscripten emscripten_idb_async_store function failed to store data. " );
    ( ( mtgui* ) _this )->onfail_save_config();
}

extern mtgui mtg;

#endif

void refresh_graph ( const mtstat& mts )
{
    const int SIV = 10; // some initial value
    graph_big tmp_map;
    graph_big big_map;
    const int MAPSIZE_SQUARE = mtstat::MAPSIZE * mtstat::MAPSIZE;
    std::vector<event_vector> tileEV ( 256, event_vector ( EV_COUNT ) );
    mtstat mts_hack = mts;
    int* ev_mts_cp[EV_COUNT] =
    {
        &mts_hack.brave_hp,
        &mts_hack.brave_at,
        &mts_hack.brave_df,
        &mts_hack.brave_mf,
        &mts_hack.brave_gold,
        NULL,
        &mts_hack.brave_yellowkey,
        &mts_hack.brave_bluekey,
        &mts_hack.brave_redkey,
        &mts_hack.brave_pickaxe,
        &mts_hack.brave_bomb,
        &mts_hack.brave_tokens,
        &mts_hack.fanfare_flag,
    };
    for ( int i = mtstat::REDCRY; i <= mtstat::FLAG; i++ )
    {
        for ( int j = 0; j < EV_COUNT; j++ )
        {
            if ( ev_mts_cp[j] )
            {
                * ( ev_mts_cp[j] ) = SIV;
            }
        }
        mts_hack.maps[0][0][0] = i;
        mts_hack.goto_coord ( 0, 0, 0 );
        for ( int j = 0; j < EV_COUNT; j++ )
        {
            if ( ev_mts_cp[j] )
            {
                tileEV[i][j] = * ( ev_mts_cp[j] ) - SIV;
            }
        }
    }
    for ( int i = 0; i < 8; i++ )
    {
        tileEV[mtstat::WHITECRY + i][EV_W] = 1 << i;
    }
    for ( int i = mtstat::YELDOOR; i <= mtstat::SOLIDWALL; i++ )
    {
        tileEV[i][EV_NEGATIVE_ONETIME] = i;
    }
    for ( int i = mtstat::MOB, s = mts.enemies.size(); i <= mtstat::TILE_EFFECTIVE_MASK; i++ )
    {
        if ( i - mtstat::MOB >= s )
        {
            break;
        }
        tileEV[i][EV_NEGATIVE_ONETIME] = i;
        if ( mts.enemies[i - mtstat::MOB][4]&mtstat::BF )
        {
            tileEV[i][EV_INTERNAL_FLAG] = 1; // to tell the graph processor that the mob is a door, namely not bombable.
        }
    }
    for ( int i = 0, fs = mts.maps.size(); i < fs; i++ )
    {
        tmp_map = fromRectMap ( mts.maps[i], tileEV );
        big_map += tmp_map;
    }
    for ( mtstat::portal_pair pp : mts.portals )
    {
        big_map.addEdge ( pp[0], pp[1] );
    }
    const std::vector<event_vector>& vs = big_map.vertices();
    for ( int i = 0, s = vs.size(); i < s; i++ )
    {
        if ( vs[i][EV_NEGATIVE_ONETIME] == mtstat::SOLIDWALL )
        {
            big_map.removeVertex ( i );
        }
    }
    big_map.pack ( mts.brave_floor * MAPSIZE_SQUARE + mts.brave_y * mtstat::MAPSIZE + mts.brave_x );
    g = big_map.shrink();
}


void mtgui::onload_config()
{
    transit_to_tower();
}

void mtgui::onsave_config()
{
    under_hint ( "Successfully saved user data locally. " );
    layer_need_update[LAYER_STAT] = true;
}

void mtgui::onfail_load_config()
{
    cur_ui = UI_CLIPBOARD;
    layer_need_update[LAYER_MAP] = true;
    layer_need_update[LAYER_STAT] = true;
    layer_need_update[LAYER_UI] = true;
}

void mtgui::onfail_save_config()
{
    under_hint ( "Failed to store user data locally! " );
}
std::string load_strdata()
{
#ifdef __EMSCRIPTEN__
    emscripten_idb_async_load ( "mtini", "mtini", ( void* ) &mtg, onload_strdata, onerror_load_strdata );
    return std::string();
#else
    FILE* f = fopen ( "mt.ini", "rb" );
    if ( !f )
    {
        return std::string();
    }
    int fs = fsize ( f );
    if ( fs == 0 )
    {
        fclose ( f );
        return std::string();
    }
    else
    {
        char* fdata = ( char* ) malloc ( fs );
        fread ( fdata, 1, fs, f );
        std::string ret ( fdata, fs );
        free ( fdata );
        fclose ( f );
        return ret;
    }
#endif
}

bool save_strdata ( std::string str )
{
#ifdef __EMSCRIPTEN__
    int s = str.length();
    tmp_strdata_buffer = ( char* ) realloc ( tmp_strdata_buffer, s );
    memcpy ( tmp_strdata_buffer, str.c_str(), s );
    emscripten_idb_async_store ( "mtini", "mtini", tmp_strdata_buffer, s, ( void* ) &mtg, onsave_strdata, onerror_save_strdata );
    return true;//false;
#else
    FILE* f = fopen ( "mt.ini", "wb" );
    if ( !f )
    {
        return false;
    }
    const char* sdata = str.c_str();
    int ssize = str.length();
    fwrite ( sdata, 1, ssize, f );
    fclose ( f );
    return true;
#endif
}

bool set_config ( std::string& strdata )
{
    if ( strdata.empty() )
    {
        return false;
    }
    edict new_conf ( strdata );
    if ( new_conf.size() == 0 )
    {
        return false;
    }
    config = new_conf; //TODO: check integrity before validate
    return true;
}

bool load_config()
{
    std::string strdata = load_strdata();
    return set_config ( strdata );
}

bool save_config()
{
    need_to_save_config = true;
    save_config_clock = 30;
    return true;
    //return save_strdata ( config.tostring() );
}

int time_now() //in ms
{
    return SDL_GetTicks();
#ifdef __EMSCRIPTEN__
    return emscripten_get_now();
#else
#ifdef __linux__
    long ms; // Milliseconds
    struct timespec spec;
    clock_gettime ( CLOCK_REALTIME, &spec );
    ms = round ( spec.tv_nsec / 1.0e6 );
    return ms;
#endif
#endif
}

Uint32 rgb ( const SDL_Surface* dst, unsigned int color )
{
    int r = ( color >> 16 ) & 0xff;
    int g = ( color >> 8 ) & 0xff;
    int b = color & 0xff;
    return SDL_MapRGB ( dst->format, r, g, b );
}

Uint32 rgba ( const SDL_Surface* dst, unsigned int color )
{
    int a = ( color >> 24 ) & 0xff;
    int r = ( color >> 16 ) & 0xff;
    int g = ( color >> 8 ) & 0xff;
    int b = color & 0xff;
    return SDL_MapRGBA ( dst->format, r, g, b, a );
}

SDL_Color sdlc ( unsigned int color )
{
    Uint8 a = ( color >> 24 ) & 0xff;
    Uint8 r = ( color >> 16 ) & 0xff;
    Uint8 g = ( color >> 8 ) & 0xff;
    Uint8 b = color & 0xff;
    SDL_Color result{.r = r, .g = g, .b = b, .a = a};
    return result;
}

TTF_Font* default_font;

void renderTextWrap ( SDL_Surface* dst, const char* text, int x, int y, int width = 2400, unsigned int color = 0x00FFFFFF, bool rjust = false, TTF_Font* font = NULL, int times = 1 )
{
    if ( font == NULL )
    {
        font = default_font;
    }
    SDL_Rect dstrect;
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = 0;
    dstrect.h = 0;
    SDL_Surface* src = TTF_RenderText_Blended_Wrapped ( font, text, sdlc ( color ), width ); // or TTF_RenderText_Blended_Wrapped
    if ( rjust )
    {
        dstrect.x -= src->w;
    }
    for ( int i = 0; i < times; i++ )
    {
        SDL_BlitSurface ( src, NULL, dst, &dstrect );
    }
    SDL_FreeSurface ( src );
}

void renderText ( SDL_Surface* dst, const char* text, int x, int y, unsigned int color = 0x00FFFFFF, bool rjust = false, TTF_Font* font = NULL, int times = 1 )
{
    if ( font == NULL )
    {
        font = default_font;
    }
    SDL_Rect dstrect;
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = 0;
    dstrect.h = 0;
    SDL_Surface* src = TTF_RenderText_Blended ( font, text, sdlc ( color ) ); // or TTF_RenderText_Blended_Wrapped
    if ( rjust )
    {
        dstrect.x -= src->w;
    }
    for ( int i = 0; i < times; i++ )
    {
        SDL_BlitSurface ( src, NULL, dst, &dstrect );
    }
    SDL_FreeSurface ( src );
}

void renderTextShaded ( SDL_Surface* dst, const char* text, int x, int y, unsigned int color = 0x00FFFFFF, bool rjust = false, TTF_Font* font = NULL, unsigned int shade_color = 0x000000 )
{
    if ( font == NULL )
    {
        font = default_font;
    }
    SDL_Rect dstrect;
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = 0;
    dstrect.h = 0;
    SDL_Surface* src = TTF_RenderText_Blended ( font, text, sdlc ( shade_color ) ); // or TTF_RenderText_Blended_Wrapped
    if ( rjust )
    {
        dstrect.x -= src->w;
    }
    dstrect.x -= 1;
    dstrect.y -= 1;
    for ( int i = 0; i < 3; i++ )
    {
        for ( int j = 0; j < 3; j++ )
        {
            SDL_BlitSurface ( src, NULL, dst, &dstrect );
            dstrect.x++;
        }
        dstrect.x -= 3;
        dstrect.y++;
    }
    SDL_FreeSurface ( src );
    dstrect.x = x;
    dstrect.y = y;
    src = TTF_RenderText_Blended ( font, text, sdlc ( color ) ); // or TTF_RenderText_Blended_Wrapped
    if ( rjust )
    {
        dstrect.x -= src->w;
    }
    SDL_BlitSurface ( src, NULL, dst, &dstrect );
    SDL_FreeSurface ( src );
}

SDL_Surface* createSurface ( int w, int h )
{
    SDL_Surface* surface = SDL_CreateRGBSurface ( 0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );
    if ( surface == NULL )
    {
        SDL_Log ( "Unable to create surface! SDL Error: %s\n",  SDL_GetError() );
    }
    return surface;
}

int fillRect ( SDL_Surface* dst, unsigned int color, int x = -1, int y = -1, int w = -1, int h = -1 )
{
    Uint32 arg_color = rgb ( dst, color );
    if ( x < 0 )
    {
        return SDL_FillRect ( dst, NULL, arg_color );
    }
    else
    {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        return SDL_FillRect ( dst, &rect, arg_color );
    }
}

int fillRectA ( SDL_Surface* dst, unsigned int color, int x = -1, int y = -1, int w = -1, int h = -1 )
{
    Uint32 arg_color = rgba ( dst, color );
    if ( x < 0 )
    {
        return SDL_FillRect ( dst, NULL, arg_color );
    }
    else
    {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        return SDL_FillRect ( dst, &rect, arg_color );
    }
}

int blitSurfaceByCoordinate ( SDL_Surface* src, SDL_Surface* dst, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y ) //, int dst_w = -1, int dst_h = -1 )
{
    //without scaling
    SDL_Rect srcrect, dstrect;
    int dst_w = src_w;
    int dst_h = src_h;
    srcrect.x = src_x;
    srcrect.y = src_y;
    srcrect.w = src_w;
    srcrect.h = src_h;
    dstrect.x = dst_x;
    dstrect.y = dst_y;
    dstrect.w = dst_w;
    dstrect.h = dst_h;
    return SDL_BlitSurface ( src, &srcrect, dst, &dstrect );
}

void hole ( SDL_Surface* mask, SDL_Surface* dst, int x, int y, int w, int h )
{
    SDL_Rect srcrect, dstrect;
    srcrect.x = srcrect.y = dstrect.w = dstrect.h = 0;
    dstrect.x = x;
    dstrect.y = y;
    srcrect.w = w;
    srcrect.h = h;
    SDL_BlitSurface ( mask, &srcrect, dst, &dstrect );
    fillRectA ( dst, 0xFFFF0000, x - 2, y - 2, 2, h + 4 );
    fillRectA ( dst, 0xFFFF0000, x - 2, y - 2, w + 4, 2 );
    fillRectA ( dst, 0xFFFF0000, x + w, y - 2, 2, h + 4 );
    fillRectA ( dst, 0xFFFF0000, x - 2, y + h, w + 4, 2 );
}
static const char brand[] = "\xb4\x5d\x67\x67\x84\xa1\xb4\x45\xf3\x8e\xe5\x93\xc7\xe4\x2\x75\x37\x8d\x70\x75\x37\x59\x75\x58\x59\x75\x8e\x7b\xcd\x59\x37\xc8\xcd\x59\xfc\x98\x75\x93\x8e\xb0\x98\xc8\x8d\x70\xb0\xfc\x98\x37\x36\x98\xb0\xcd\x6b\x1f\xb0\x93\x36\x98\x4f\xea\x71\xcd\x2\x4f\x70\x2\x8e\x98";
static const int brand_length = 9;
static const int brand_key = 53;
void mtgui::initBG ( SDL_Surface* bg, SDL_Surface* wallpaper )
{
    SDL_BlitSurface ( wallpaper, NULL, bg, NULL );
    char buffer[256];
    for ( int i = 0; i < brand_length; i++ )
    {
        buffer[0] = ( ( ( int ) brand[i] ) * brand_key ) & 0xff;
        buffer[1] = 0;
        renderText ( bg, buffer, 700 + i * 10, 0, 0x00FF00 );
    }
    SDL_Surface* dim = createSurface ( 800, 600 );
    fillRectA ( dim, 0xDF000000 );
    hole ( dim, bg, 20, 20, 120, 300 );
    hole ( dim, bg, 20, 340, 120, 240 );
    //hole ( dim, bg, 160, 20, 480, 20 );
    hole ( dim, bg, 160, 20, 480, 480 );
    hole ( dim, bg, 160, 520, 480, 60 );
    hole ( dim, bg, 660, 20, 120, 130 );
    hole ( dim, bg, 660, 170, 120, 410 );
    for ( int i = 0; i < brand_length; i++ )
    {
        buffer[0] = ( ( ( int ) brand[i] ) * brand_key ) & 0xff;
        buffer[1] = 0;
        renderText ( bg, buffer, 20 + i * 10, 30, 0x000000 );
    }
    renderText ( bg, ":HP", 135, 30, 0xFFFF00, true );
    renderText ( bg, ":AT", 135, 60, 0xFF7F7F, true );
    renderText ( bg, ":DF", 135, 90, 0x3FBFFF, true );
    renderText ( bg, ":MF", 135, 120, 0x7FFF7F, true );
    renderText ( bg, ": G", 135, 150, 0xFFBF3F, true );
    renderText ( bg, "YK", 50, 190, 0xFFFF00, true );
    renderText ( bg, "BK", 90, 190, 0x7F7FFF, true );
    renderText ( bg, "RK", 130, 190, 0xFF0000, true );
    renderText ( bg, "Px", 50, 250, 0xBF0000, true );
    renderText ( bg, "Bb", 90, 250, 0x7F7F7F, true );
    renderText ( bg, "Tk", 130, 250, 0x00FF00, true );
    renderText ( bg, "X", 675, 30, 0xFFFF00 );
    renderText ( bg, "Y", 715, 30, 0xFFFF00 );
    renderText ( bg, "F", 755, 30, 0xFFFF00 );
    renderText ( bg, "Flags:", 670, 90, 0x7FFFFF );
    renderText ( bg, "  REG:", 670, 120, 0x7FFF7F );
    renderText ( bg, "Basic infos:", 20, 345, 0xFFFFFF, false );
    sprintf ( buffer, "Thresholds:" );
    renderText ( bg, buffer, 20, 495, 0xFFFFFF, false );
    SDL_FreeSurface ( dim );
}

void mtgui::trace_append ( std::string str )
{
    step_traces += str;
    cur_stat.process_from_stream ( str.c_str(), str.length() );
}

void mtgui::trace_rebuild ( std::string str )
{
    cur_stat = orig_stat;
    step_traces = str;
    cur_stat.process_from_stream ( str.c_str(), str.length() );
}

void mtgui::LSP ( int i )
{
    std::string index = indexed ( "mtsavedata", tower_id, i );
    switch ( cur_ui )
    {
        case UI_LOAD:
            if ( i == -1 )
            {
                trace_rebuild ( strace_for_playing );
                under_hint ( "Load successful!" );
                playSound ( TREASURE_OGG );
            }
            else if ( config.count ( index ) )
            {
                trace_rebuild ( config[index] );
                under_hint ( "Load successful!" );
                playSound ( TREASURE_OGG );
            }
            else
            {
                playSound ( CRYSTAL_OGG );
                under_hint ( "Nothing to load!" );
            }
            cur_ui = UI_MAP;
            break;
        case UI_SAVE:
            config[index] = step_traces;
            save_config();
            under_hint ( "Save successful!" );
            playSound ( TREASURE_OGG );
            cur_ui = UI_MAP;
            break;
        case UI_PLAY:
            //TODO
            if ( i == -1 )
            {
                cur_stat = orig_stat;
                under_hint ( "Load successful from clipboard! Now we play back the savedata. " );
                playSound ( TREASURE_OGG );
                cur_ui = UI_PLAYING;
                step_traces.clear();
            }
            else if ( config.count ( index ) )
            {
                strace_for_playing = config[index];
                if ( strace_for_playing.length() != 0 )
                {
                    cur_stat = orig_stat;
                    under_hint ( "Load successful! Now we play back the savedata. " );
                    playSound ( TREASURE_OGG );
                    cur_ui = UI_PLAYING;
                    step_traces.clear();
                }
                else
                {
                    playSound ( CRYSTAL_OGG );
                    under_hint ( "The savedata is empty. " );
                }
            }
            else
            {
                playSound ( CRYSTAL_OGG );
                under_hint ( "Nothing to load!" );
            }
            under_hint ( strace_for_playing );
            ui_val = 0;
            break;
        default:
            fprintf ( stderr, "Exception occurs in LSP process! The UI is neither load, save, nor play.\n" );
            break;
    }
}

void mtgui::transit_to_tower ( int id )
{
    if ( id == -1 )
    {
        bool fetch = false;
        if ( config.count ( "cur_mtid" ) )
        {
            std::string id_s = config["cur_mtid"];
            id = atoi ( id_s.c_str() );
            if ( id >= 0 && id <= 9999 )
            {
                tower_id = id;
                fetch = true;
            }
        }
        if ( !fetch )
        {
            tower_id = id = 0;
            config["cur_mtid"] = "0";
            save_config();
        }
    }
    else
    {
        if ( id > 9999 || id < 0 )
        {
            fprintf ( stderr, "Tower ID out of range!\n" );
            return;
        }
        else
        {
            tower_id = id;
            char buffer[16];
            sprintf ( buffer, "%d", id );
            config["cur_mtid"] = buffer;
            save_config();
        }
    }
    //tower_id = id;
    std::string keystr = indexed ( "mtstat", id ) ;
    mtstat tmp_stat;
    layer_need_update[LAYER_MAP] = true;
    layer_need_update[LAYER_STAT] = true;
    layer_need_update[LAYER_UI] = true;
    char buffer[256];
    if ( config.count ( keystr ) && tmp_stat.init_from_stream ( config[keystr].c_str() ) )
    {
        cur_stat = orig_stat = tmp_stat;
        refresh_graph ( cur_stat );
        step_traces.clear();
        cur_ui = UI_MAP;
        playSound ( PORTAL_OGG );
        sprintf ( buffer, "Successfully load tower #%d data!", id );
        under_hint ( buffer );
    }
    else
    {
        cur_ui = UI_CLIPBOARD;
        step_traces.clear();
        playSound ( CRYSTAL_OGG );
        sprintf ( buffer, "You may initialize tower #%d data from clipboard. ", id );
        under_hint ( buffer );
    }
}

bool mtgui::load_codecs()
{
    int flags = IMG_INIT_PNG;
    int initted = IMG_Init ( flags );
    if ( ( initted & flags ) != flags )
    {
        SDL_Log ( "Fail to load PNG supports!\n" );
        return false;
    }
    if ( TTF_Init() == -1 )
    {
        SDL_Log ( "Fail to load TTF supports!\n" );
        return false;
    }
    flags = MIX_INIT_OGG;
    initted = Mix_Init ( flags );
    if ( ( initted & flags ) != flags )
    {
        SDL_Log ( "Fail to load OGG supports!\n" );
        return false;
    }
    if ( Mix_OpenAudio ( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 )
    {
        SDL_Log ( "Fail to load audio device!\n" );
        return false;
    }
    return true;
}

bool mtgui::load_assets()
{
    bool success = true;
    //load fonts
#define LOAD_TTF(ENUM_NAME,FILE_NAME,SIZE) \
    fonts[(ENUM_NAME)] = TTF_OpenFontRW ( SDL_RWFromMem ( assets_font_##FILE_NAME, assets_font_##FILE_NAME##_len ), 1, SIZE )
    LOAD_TTF ( FREE_MONO_BOLD_TTF, FreeMonoBold_ttf, 20 );
    LOAD_TTF ( UBUNTU_MONO_R_TTF, UbuntuMono_R_ttf, 20 );
    LOAD_TTF ( UBUNTU_MONO_R_TTF_HUGE, UbuntuMono_R_ttf, 32 );
    LOAD_TTF ( FREE_MONO_BOLD_TTF_SMALL, FreeMonoBold_ttf, 16 );
    LOAD_TTF ( UBUNTU_MONO_R_TTF_SMALL, UbuntuMono_R_ttf, 16 );
    LOAD_TTF ( FREE_MONO_BOLD_TTF_TINY, FreeMonoBold_ttf, 12 );
    LOAD_TTF ( UBUNTU_MONO_R_TTF_TINY, UbuntuMono_R_ttf, 12 );
    fonts[FREE_MONO_BOLD_TTF] = TTF_OpenFontRW ( SDL_RWFromMem ( assets_font_FreeMonoBold_ttf, assets_font_FreeMonoBold_ttf_len ), 1, 20 );
    for ( int i = 0; i < FONT_COUNT; i++ )
    {
        if ( fonts[i] == NULL )
        {
            SDL_Log ( "Fail to load font %d!\n", i );
            success = false;
        }
    }
    default_font = fonts[0];
#undef LOAD_TTF
    //load images
#define LOAD_IMG(ENUM_NAME,FILE_NAME) \
    imgs[(ENUM_NAME)] = IMG_Load_RW ( SDL_RWFromMem ( assets_img_##FILE_NAME, assets_img_##FILE_NAME##_len ), 1 )
    LOAD_IMG ( MTICON_PNG, mticon_png );
    LOAD_IMG ( ITEM_PNG, item_png );
    LOAD_IMG ( BG_PNG, bg_png );
    for ( int i = 0; i < IMG_COUNT; i++ )
    {
        if ( imgs[i] == NULL )
        {
            SDL_Log ( "Fail to load image %d!\n", i );
            success = false;
        }
    }
#undef LOAD_IMG
    //load images
#define LOAD_WAV(ENUM_NAME,FILE_NAME) \
    sounds[(ENUM_NAME)] = Mix_LoadWAV_RW( SDL_RWFromMem ( assets_sound_##FILE_NAME, assets_sound_##FILE_NAME##_len ), 1 )
    LOAD_WAV ( DOOR_OGG, door_ogg );
    LOAD_WAV ( CRYSTAL_OGG, crystal_ogg );
    LOAD_WAV ( SHOP_OGG, shop_ogg );
    LOAD_WAV ( POTION_OGG, potion_ogg );
    LOAD_WAV ( BOMB_OGG, bomb_ogg );
    LOAD_WAV ( TREASURE_OGG, treasure_ogg );
    LOAD_WAV ( KEY_OGG, key_ogg );
    LOAD_WAV ( PORTAL_OGG, portal_ogg );
    LOAD_WAV ( WALK_OGG, walk_ogg );
    LOAD_WAV ( BATTLE_OGG, battle_ogg );
    LOAD_WAV ( BGM_OGG, bgm_ogg );
    LOAD_WAV ( WALL_OGG, wall_ogg );
    for ( int i = 0; i < SOUND_COUNT; i++ )
    {
        if ( sounds[i] == NULL )
        {
            SDL_Surface* l = layers[LAYER_STAT];
            fillRectA ( l, 0 );
            SDL_Log ( "Fail to load sound %d!\n", i );
            success = false;
        }
    }
#undef LOAD_WAV
    return success;
}
bool mtgui::init_mainUI()
{
    //Initialization flag
    bool success = true;
    //Create window
    gWindow = SDL_CreateWindow ( "Magic Tower", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if ( gWindow == NULL )
    {
        SDL_Log ( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        if ( FULLSCREEN )
        {
            SDL_SetWindowFullscreen ( gWindow, SDL_WINDOW_FULLSCREEN );
        }
        //Get window surface
        layers[LAYER_SCREEN_SURFACE] = SDL_GetWindowSurface ( gWindow );
        layer_need_update[LAYER_MAP] = true;
        layer_need_update[LAYER_STAT] = true;
        layer_need_update[LAYER_UI] = true;
        if ( orig_stat.maps.size() == 0 )
        {
#ifdef __EMSCRIPTEN__
            cur_ui = UI_WAITING;
            under_hint ( "Waiting for the emscripten to load userdata stored locally ... " );
#else
            cur_ui = UI_CLIPBOARD;
            under_hint ( "Copy the raw map data to the clipboard and press <Enter> in order to initialize. " );
#endif
        }
        else
        {
            cur_ui = UI_MAP;
        }
        for ( int i = LAYER_SCREEN_SURFACE + 1; i < LAYER_COUNT; i++ )
        {
            layers[i] = createSurface ( SCREEN_WIDTH, SCREEN_HEIGHT );
        }
        initBG ( layers[LAYER_BG], imgs[BG_PNG] );
        update_screen();
        bgm_channel =  Mix_PlayChannel ( -1, sounds[BGM_OGG] , -1 );
        mutebgm = false;
        mutese = false;
        parity = false;
    }
    return success;
}

static int translator[] =
{
    5,  50, 50, 50, 50, 50, 50, 50,
    12, 12, 12, 12, 12, 12, 12, 12,
    13, 13, 13, 13, 13, 13, 13, 13,
    14, 14, 14, 14, 14, 14, 14, 14,
    15, 15, 15, 15, 15, 15, 15, 15,

    19, 20, 21, 22, 19, 20, 21, 22,
    19, 20, 21, 22, 19, 20, 21, 22,

    16, 17, 18, 31, 36, 34, 37, 11,

    50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  1,  2,  7,  6,  50, 50, 50,

    50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50,

    74, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113,

    74, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113,

    74, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97,
    98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113,
    63, 72, 114, 115, 116, 117, 118, 119,
};

void mtgui::renderTile ( int map_x, int map_y, int tile_ID )
{
    int renderID = translator[tile_ID & 0xff];
    char comment[256] = {};
    if ( renderID > 47 )
    {
        renderID = 2 * renderID - 48;
        if ( parity )
        {
            renderID++;
        }
    }
    renderID += 12;
    blitSurfaceByCoordinate ( imgs[ITEM_PNG], layers[LAYER_MAP], ( renderID % 12 ) * 32, ( renderID / 12 ) * 32, 32, 32, MAPX + 32 * map_x, MAPY + 32 * map_y );
    if ( tile_ID >= 8 && tile_ID < 16 )
    {
        sprintf ( comment, "+%d", orig_stat.unit_at << ( tile_ID & 0x7 ) );
    }
    else if ( tile_ID >= 16 && tile_ID < 24 )
    {
        sprintf ( comment, "+%d", orig_stat.unit_df << ( tile_ID & 0x7 ) );
    }
    else if ( tile_ID >= 24 && tile_ID < 32 )
    {
        sprintf ( comment, "+%d", orig_stat.unit_mf << ( tile_ID & 0x7 ) );
    }
    else if ( tile_ID >= 32 && tile_ID < 40 )
    {
        sprintf ( comment, "%dx", 1 << ( tile_ID & 0x7 ) );
    }
    else if ( tile_ID >= 40 && tile_ID < 56 )
    {
        sprintf ( comment, "+%d", orig_stat.unit_hp << ( tile_ID - 40 ) );
    }
    int color = 0xFFFFFF;
    if ( tile_ID >= 0x80 )
    {
        color = 0xFF0000;
        int mob_id = tile_ID - 0x80;
        int damage = cur_stat.dam ( mob_id ), hp = cur_stat.brave_hp;
        if ( damage > 99999999 )
        {
            damage = 99999999;
        }
        if ( damage == 0 )
        {
            color = 0xFFFFFF;
        }
        else if ( damage < hp / 2 )
        {
            color = 0x3FFF3F;
        }
        else if ( damage < hp )
        {
            color = 0xFFFF3F;
        }
        if ( damage == 99999999 )
        {
            sprintf ( comment, "INF" );
        }
        else if ( damage > 99999 )
        {
            sprintf ( comment, "%dK", damage / 1000 );
        }
        else
        {
            sprintf ( comment, "%d", damage );
        }
    }
    if ( strlen ( comment ) > 0 )
    {
        renderTextShaded ( layers[LAYER_MAP], comment, MAPX + 32 * ( map_x + 1 ), MAPY + 32 * map_y, color, true, fonts[UBUNTU_MONO_R_TTF_TINY] );
    }
    //sprintf ( comment, "+%d", 1 );
}

void mtgui::update_map()
{
    SDL_Surface* l = layers[LAYER_MAP];
    int f = cur_stat.brave_floor;
    fillRectA ( l, 0 );
    if ( orig_stat.maps.size() != 0 )
    {
        for ( int i = 0; i < 15; i++ )
        {
            for ( int j = 0; j < 15; j++ )
            {
                renderTile ( j, i, cur_stat.maps[f][i][j] );
            }
        }
        char comment[256];
        for ( int i = 0, s = cur_stat.portals.size(); i < s; i++ )
        {
            if ( cur_stat.portals[i][0] / 225 == f )
            {
                int coord = cur_stat.portals[i][0] % 225;
                int map_x = coord % 15, map_y = coord / 15;
                int coord2 = cur_stat.portals[i][1];
                int f2 = coord2 / 225, x2 = coord2 % 15, y2 = ( coord2 / 15 ) % 15;
                int renderID = 22;
                blitSurfaceByCoordinate ( imgs[ITEM_PNG], layers[LAYER_MAP], ( renderID % 12 ) * 32, ( renderID / 12 ) * 32, 32, 32, MAPX + 32 * map_x, MAPY + 32 * map_y );
                sprintf ( comment, "%4dF\n%2d|%2d", f2, x2, y2 );
                comment[5] = 0;
                renderTextShaded ( layers[LAYER_MAP], comment, MAPX + 32 * ( map_x + 1 ), MAPY + 32 * map_y, 0xFFFFFF, true, fonts[UBUNTU_MONO_R_TTF_TINY] );
                renderTextShaded ( layers[LAYER_MAP], comment + 6, MAPX + 32 * ( map_x + 1 ), MAPY + 32 * map_y + 16, 0xFFFFFF, true, fonts[UBUNTU_MONO_R_TTF_TINY] );
            }
            if ( cur_stat.portals[i][1] / 225 == f )
            {
                int coord = cur_stat.portals[i][1] % 225;
                int map_x = coord % 15, map_y = coord / 15;
                int coord2 = cur_stat.portals[i][0];
                int f2 = coord2 / 225, x2 = coord2 % 15, y2 = ( coord2 / 15 ) % 15;
                int renderID = 22;
                blitSurfaceByCoordinate ( imgs[ITEM_PNG], layers[LAYER_MAP], ( renderID % 12 ) * 32, ( renderID / 12 ) * 32, 32, 32, MAPX + 32 * map_x, MAPY + 32 * map_y );
                sprintf ( comment, "%4dF\n%2d|%2d", f2, x2, y2 );
                comment[5] = 0;
                renderTextShaded ( layers[LAYER_MAP], comment, MAPX + 32 * ( map_x + 1 ), MAPY + 32 * map_y, 0xFFFFFF, true, fonts[UBUNTU_MONO_R_TTF_TINY] );
                renderTextShaded ( layers[LAYER_MAP], comment + 6, MAPX + 32 * ( map_x + 1 ), MAPY + 32 * map_y + 16, 0xFFFFFF, true, fonts[UBUNTU_MONO_R_TTF_TINY] );
            }
        }
        int renderID = face;
        int map_x = cur_stat.brave_x, map_y = cur_stat.brave_y;
        blitSurfaceByCoordinate ( imgs[ITEM_PNG], layers[LAYER_MAP], ( renderID % 12 ) * 32, ( renderID / 12 ) * 32, 32, 32, MAPX + 32 * map_x, MAPY + 32 * map_y );
    }
    layer_need_update[LAYER_MAP] = false;
}

void mtgui::update_stat()
{
    SDL_Surface* l = layers[LAYER_STAT];
    fillRectA ( l, 0 );
    char buffer[256];
    sprintf ( buffer, "%d", cur_stat.brave_hp );
    renderText ( l, buffer, 100, 30, 0xFFFF00, true );
    sprintf ( buffer, "%d", cur_stat.brave_at );
    renderText ( l, buffer, 100, 60, 0xFF7F7F, true );
    sprintf ( buffer, "%d", cur_stat.brave_df );
    renderText ( l, buffer, 100, 90, 0x3FBFFF, true );
    sprintf ( buffer, "%d", cur_stat.brave_mf );
    renderText ( l, buffer, 100, 120, 0x7FFF7F, true );
    sprintf ( buffer, "%d", cur_stat.brave_gold );
    renderText ( l, buffer, 100, 150, 0xFFBF3F, true );
    sprintf ( buffer, "%d", cur_stat.brave_yellowkey );
    renderText ( l, buffer, 50, 220, 0xFFFF00, true );
    sprintf ( buffer, "%d", cur_stat.brave_bluekey );
    renderText ( l, buffer, 90, 220, 0x7F7FFF, true );
    sprintf ( buffer, "%d", cur_stat.brave_redkey );
    renderText ( l, buffer, 130, 220, 0xFF0000, true );
    sprintf ( buffer, "%d", cur_stat.brave_pickaxe );
    renderText ( l, buffer, 50, 280, 0xBF0000, true );
    sprintf ( buffer, "%d", cur_stat.brave_bomb );
    renderText ( l, buffer, 90, 280, 0x7F7F7F, true );
    sprintf ( buffer, "%d", cur_stat.brave_tokens );
    renderText ( l, buffer, 130, 280, 0x00FF00, true );
    sprintf ( buffer, "%d", cur_stat.brave_x );
    renderText ( l, buffer, 690, 60, 0xFFFFFF, true );
    sprintf ( buffer, "%d", cur_stat.brave_y );
    renderText ( l, buffer, 730, 60, 0xFFFFFF, true );
    sprintf ( buffer, "%d", cur_stat.brave_floor );
    renderText ( l, buffer, 770, 60, 0xFFFFFF, true );
    sprintf ( buffer, "%d", cur_stat.fanfare_flag );
    renderText ( l, buffer, 770, 90, 0xFFFFFF, true );
    sprintf ( buffer, "unit_hp=%d", orig_stat.unit_hp );
    renderText ( l, buffer, 25, 375, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    sprintf ( buffer, "unit_at=%d", orig_stat.unit_at );
    renderText ( l, buffer, 25, 405, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    sprintf ( buffer, "unit_df=%d", orig_stat.unit_df );
    renderText ( l, buffer, 25, 435, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    sprintf ( buffer, "unit_mf=%d", orig_stat.unit_mf );
    renderText ( l, buffer, 25, 465, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    sprintf ( buffer, "%3d|%3d|%3d", orig_stat.score_threshold[0], orig_stat.score_threshold[1], orig_stat.score_threshold[2] );
    renderText ( l, buffer, 25, 525, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    sprintf ( buffer, "%3d|%3d|%3d", orig_stat.score_threshold[3], orig_stat.score_threshold[4], orig_stat.score_threshold[5] );
    renderText ( l, buffer, 25, 555, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    int strace_len = step_traces.length();
    if ( strace_len > 0 )
    {
        update_strace();//renderTextWrap ( l, step_traces.c_str(), 660, 170, 120, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    }
    if ( hint_str.length() != 0 )
    {
        renderTextWrap ( l, hint_str.c_str(), 160, 520, 480, 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_SMALL] );
    }
    int reg = cur_stat.reg;
    if ( reg )
    {
        sprintf ( buffer, "'%c'", reg );
        renderText ( l, buffer, 770, 120, 0xFFFFFF, true );
    }
    //TODO: more underbar hint
    layer_need_update[LAYER_STAT] = false;
    layer_need_update[DUMMY_LAYER_UNDER_HINT] = false;
}

void mtgui::update_strace()
{
    //TODO: more color in strace
    const int ROWS = 20, COLS = 11;
    const int TOTAL_CHARS = ROWS * COLS;
    SDL_Surface* l = layers[LAYER_STAT];
    int length = step_traces.length();
    std::string st_to_show;
    if ( length > TOTAL_CHARS )
    {
        st_to_show = step_traces.substr ( length - TOTAL_CHARS, length );
        length = TOTAL_CHARS;
    }
    else
    {
        st_to_show = step_traces;
    }
    char buffer[16] = {};
    for ( int i = 0; i < length; i++ )
    {
        buffer[0] = st_to_show[i];
        int color = 0xFFFFFF;
        TTF_Font* font = fonts[UBUNTU_MONO_R_TTF];
        //if ( i == length - 1 )
        //{
        //font = fonts[UBUNTU_MONO_R_TTF];
        //}
        //else
        if ( buffer[0] == 'g' )
        {
            color = 0x7F7FFF;
        }
        else if ( buffer[0] == 's' )
        {
            color = 0xFFBF3F;
        }
        else if ( buffer[0] == 'a' )
        {
            color = 0xFF7F7F;
        }
        else if ( buffer[0] == 'd' )
        {
            color = 0x3FBFFF;
        }
        else if ( buffer[0] == 'm' )
        {
            color = 0x7FFF7F;
        }
        else if ( buffer[0] == 'p' )
        {
            color = 0xBF0000;
        }
        else if ( buffer[0] == 'b' )
        {
            color = 0x7F7F7F;
        }//...
        if ( i == length - 1 )
        {
            renderTextShaded ( l, buffer, 675 + ( i % COLS ) * 10, 175 + ( i / COLS ) * 20, color, true, font, 0xFF0000 );
        }
        else
        {
            renderText ( l, buffer, 675 + ( i % COLS ) * 10, 175 + ( i / COLS ) * 20, color, true, font );
        }
    }
}

static void init_mob_pedia ( const mtstat& st )
{
    int enemy_count = st.enemies.size();
    std::vector<bool> eb ( enemy_count, false );
    for ( int i = 0; i < mtstat::MAPSIZE; i++ )
    {
        for ( int j = 0; j < mtstat::MAPSIZE; j++ )
        {
            int tile = st.maps[st.brave_floor][i][j];
            if ( tile >= mtstat::MOB )
            {
                eb[tile - mtstat::MOB] = true;
            }
        }
    }
    effective_mob_count = 0;
    mobs_in_order = std::vector<int> ( enemy_count );
    for ( int i = 0; i < enemy_count; i++ )
    {
        if ( eb[i] )
        {
            mobs_in_order[effective_mob_count++] = i;
        }
    }
    int mc = effective_mob_count;
    for ( int i = 0; i < enemy_count; i++ )
    {
        if ( !eb[i] )
        {
            mobs_in_order[mc++] = i;
        }
    }
}

int generate_id_color ( long seed )
{
    seed ^= 0xdead;
    seed = seed * seed % 65537;
    seed = seed * seed % 65537;
    seed = seed * seed % 65537;
    seed = seed * seed % 65537;
    int ret = 0;
    for ( int i = 0; i < 3; i++ )
    {
        ret = ( ret << 8 ) | ( 0x7f + ( seed % 4 ) * 0x20 );
        seed = seed * seed % 65537;
    }
    return ret;
}

void mtgui::update_ui()
{
    SDL_Surface* l = layers[LAYER_UI];
    fillRectA ( l, 0 );
    switch ( cur_ui )
    {
        case UI_MAP:
        case UI_PLAYING:
            // draw the graph data
            if ( show_graph )
            {
                for ( int i = 0, s = g.vertices().size(); i < s; i++ )
                {
                    unsigned int color = generate_id_color ( i );
                    const int MS = mtstat::MAPSIZE;
                    for ( int j : g.trace() [i] )
                    {
                        if ( j / MS / MS == cur_stat.brave_floor )
                        {
                            int x = j % MS, y = j / MS % MS;
                            fillRectA ( l, color | 0x7F000000, MAPX + x * 32, MAPY + y * 32, 32, 32 );
                            char buffer[256];
                            sprintf ( buffer, "%d", i );
                            renderTextShaded ( l, buffer, MAPX + ( x + 1 ) * 32 - 4, MAPY + y * 32 + 4, 0xFFFFFF, true, fonts[UBUNTU_MONO_R_TTF_TINY], color );
                            sprintf ( buffer, "%d", j );
                            renderTextShaded ( l, buffer, MAPX + ( x + 1 ) * 32 - 4, MAPY + y * 32 + 16 + 4, 0xFFFFFF, true, fonts[UBUNTU_MONO_R_TTF_TINY], color );
                        }
                    }
                }
            }
            break;
        case UI_CLIPBOARD:
            fillRectA ( l, 0xEFBF7FFF, MAPX, MAPY, 480, 480 );
            //under_hint ( "Copy the map data into clipboard, and press <Enter> in order to play it. "
            //#ifdef __EMSCRIPTEN__
            //""
            //#endif
            //);
            break;
        case UI_MOB_PEDIA:
        {
            fillRectA ( l, 0xEFBF7FFF, MAPX, MAPY, 480, 480 );
            int s = mobs_in_order.size();
            for ( int i = 0; i < MOB_PER_PAGE; i++ )
            {
                int mob_id_i = ui_val * MOB_PER_PAGE + i;
                if ( mob_id_i < s )
                {
                    int mob_id = mobs_in_order[mob_id_i];
                    int renderID = translator[ ( mob_id + mtstat::MOB ) & 0xff];
                    if ( renderID > 47 )
                    {
                        renderID = 2 * renderID - 48;
                        if ( parity )
                        {
                            renderID++;
                        }
                    }
                    renderID += 12;
                    blitSurfaceByCoordinate ( imgs[ITEM_PNG], l, ( renderID % 12 ) * 32, ( renderID / 12 ) * 32, 32, 32, MAPX + 32 , MAPY + 32 + 96 * i );
                    char* buffer = new char[256];
                    renderText ( l, "HP:", MAPX + 80, MAPY + 32 - 24 + 96 * i ,  mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFFFF );
                    sprintf ( buffer, "%d", cur_stat.enemies[mob_id][0] );
                    renderText ( l, buffer, MAPX + 192, MAPY + 32 - 24 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFF00, true );
                    renderText ( l, "AT:", MAPX + 80, MAPY + 32 - 8 + 96 * i ,  mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFFFF );
                    sprintf ( buffer, "%d", cur_stat.enemies[mob_id][1] );
                    renderText ( l, buffer, MAPX + 192, MAPY + 32 - 8 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFF7F7F, true );
                    renderText ( l, "DF:", MAPX + 80, MAPY + 32 + 8 + 96 * i ,  mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFFFF );
                    sprintf ( buffer, "%d", cur_stat.enemies[mob_id][2] );
                    renderText ( l, buffer, MAPX + 192, MAPY + 32 + 8 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0x3FBFFF, true );
                    renderText ( l, " G:", MAPX + 80, MAPY + 32 + 24 + 96 * i ,  mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFFFF );
                    sprintf ( buffer, "%d", cur_stat.enemies[mob_id][3] );
                    renderText ( l, buffer, MAPX + 192, MAPY + 32 + 24 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFBF3F, true );
                    renderText ( l, "DMG:", MAPX + 224, MAPY + 32 + 16 + 96 * i ,  mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFFFF, false, fonts[UBUNTU_MONO_R_TTF_HUGE] );
                    int damage = cur_stat.dam ( mob_id ), hp = cur_stat.brave_hp;
                    if ( damage > 99999999 )
                    {
                        damage = 99999999;
                    }
                    int color = 0xFF0000;
                    if ( damage == 0 )
                    {
                        color = 0xFFFFFF;
                    }
                    else if ( damage < hp / 2 )
                    {
                        color = 0x3FFF3F;
                    }
                    else if ( damage < hp )
                    {
                        color = 0xFFFF3F;
                    }
                    sprintf ( buffer, "%d", damage );
                    renderText ( l, buffer, MAPX + 416, MAPY + 32 + 16 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : color, true , fonts[UBUNTU_MONO_R_TTF_HUGE] );
                    int spec = cur_stat.enemies[mob_id][4];
                    if ( spec & mtstat::MA )
                    {
                        sprintf ( buffer, "Magic" );
                        renderText ( l, buffer, MAPX + 224, MAPY + 32 - 24 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xBF3FFF );
                    }
                    if ( spec & mtstat::SD )
                    {
                        sprintf ( buffer, "Solid" );
                        renderText ( l, buffer, MAPX + 288, MAPY + 32 - 24 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0x7F7F7F );
                    }
                    if ( spec & mtstat::BF )
                    {
                        sprintf ( buffer, "Invulnerable" );
                        renderText ( l, buffer, MAPX + 224, MAPY + 32 - 8 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xBF0000 );
                    }
                    if ( spec & mtstat::CNMASK )
                    {
                        sprintf ( buffer, "%dx Chained", ( spec & mtstat::CNMASK ) + 1 );
                        renderText ( l, buffer, MAPX + 352, MAPY + 32 - 24 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xBFFFBF );
                    }
                    if ( spec & mtstat::SA )
                    {
                        sprintf ( buffer, "Speedy" );
                        renderText ( l, buffer, MAPX + 352, MAPY + 32 - 8 + 96 * i , mob_id_i >= effective_mob_count ? 0xBFBFBF : 0xFFFF7F );
                    }
                    delete[] buffer;
                }
            }
        }
        break;
        case UI_LOAD:
        case UI_SAVE:
        case UI_PLAY:
        {
            fillRectA ( l, 0xEFBF7FFF, MAPX, MAPY, 480, 480 );
            fillRectA ( l, parity ? 0xEFBFBFFF : 0xEFBFBFFF, MAPX + 10, MAPY + 10 + 96 * ( ui_val % SLOT_PER_PAGE ), 460, 76 );
            for ( int i = 0; i < SLOT_PER_PAGE; i++ )
            {
                int slot_id = ui_val / SLOT_PER_PAGE * SLOT_PER_PAGE + i;
                char* buffer = new char[256];
                sprintf ( buffer, "SLOT %d", slot_id + 1 );
                renderText ( l, buffer, MAPX + 32, MAPY + 32 - 16 + 96 * i , 0xFFFF3F, false , fonts[UBUNTU_MONO_R_TTF_HUGE] );
                std::string index = indexed ( "mtsavedata", tower_id, slot_id );
                if ( config.count ( index ) )
                {
                    const std::string& tmp_route = config[index];
                    mtstat tmp_stat = orig_stat;
                    tmp_stat.process_from_stream ( tmp_route.c_str(), tmp_route.length() );
                    sprintf ( buffer, "%d", tmp_stat.brave_hp );
                    renderTextShaded ( l, buffer, MAPX + 304, MAPY + 32 - 24 + 96 * i, 0xFFFF00, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_at );
                    renderTextShaded ( l, buffer, MAPX + 304, MAPY + 32 - 8 + 96 * i, 0xFF7F7F, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_df );
                    renderTextShaded ( l, buffer, MAPX + 304, MAPY + 32 + 8 + 96 * i, 0x3FBFFF, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_mf );
                    renderTextShaded ( l, buffer, MAPX + 304, MAPY + 32 + 24 + 96 * i, 0x7FFF7F, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_gold );
                    renderTextShaded ( l, buffer, MAPX + 416, MAPY + 32 - 24 + 96 * i, 0xFFBF3F, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_yellowkey );
                    renderTextShaded ( l, buffer, MAPX + 352, MAPY + 32 + 96 * i, 0xFFFF00, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_bluekey );
                    renderTextShaded ( l, buffer, MAPX + 384, MAPY + 32 + 96 * i, 0x7F7FFF, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_redkey );
                    renderTextShaded ( l, buffer, MAPX + 416, MAPY + 32 + 96 * i, 0xFF0000, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_pickaxe );
                    renderTextShaded ( l, buffer, MAPX + 352, MAPY + 32 + 24 + 96 * i, 0xBF0000, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_bomb );
                    renderTextShaded ( l, buffer, MAPX + 384, MAPY + 32 + 24 + 96 * i, 0x7F7F7F, true, NULL, 0xBFBFBF );
                    sprintf ( buffer, "%d", tmp_stat.brave_tokens );
                    renderTextShaded ( l, buffer, MAPX + 416, MAPY + 32 + 24 + 96 * i, 0x00FF00, true, NULL, 0xBFBFBF );
                }
                else
                {
                    renderText ( l, "[No Data]", MAPX + 224, MAPY + 32 - 16 + 96 * i , 0xBFBFBF, false , fonts[UBUNTU_MONO_R_TTF_HUGE] );
                }// if the data is invalid, which is, it does not contain data, then it must be grayed out or not show up
                delete[] buffer;
            }
        }
        break;
        case UI_MTID:
        {
            fillRectA ( l, 0xEFBF7FFF, MAPX, MAPY, 480, 480 );
            fillRectA ( l, parity ? 0xEFBFBFFF : 0xEFBFBFFF, MAPX + 240 + 50 + 5 - ui_val * 50, MAPY + 200, 40 , 32 + 20 );
            int tmp_tower_id = ui_tower_id;
            for ( int i = 0; i < 4; i++ )
            {
                int digit = tmp_tower_id % 10;
                tmp_tower_id /= 10;
                char buffer[256];
                sprintf ( buffer, "%d", digit );
                renderText ( l, buffer, MAPX + 240 + 50 + 17 - i * 50, MAPY + 210 , 0xFFFF3F, false , fonts[UBUNTU_MONO_R_TTF_HUGE] );
            }
        }
        break;
        default:
            break;
    }
    layer_need_update[LAYER_UI] = false;
}

void mtgui::under_hint ( std::string str )
{
    printf ( "[MSG]%s\n", str.c_str() );
    if ( layer_need_update[DUMMY_LAYER_UNDER_HINT] )
    {
        hint_str += str;
    }
    else
    {
        layer_need_update[LAYER_STAT] = true;
        layer_need_update[DUMMY_LAYER_UNDER_HINT] = true;
        hint_str = str;
    }
}

void mtgui::update_screen()
{
    bool update = false;
    if ( layer_need_update[LAYER_MAP] )
    {
        update = true;
        update_map();
    }
    if ( layer_need_update[LAYER_UI] )
    {
        update = true;
        update_ui();
    }
    if ( layer_need_update[LAYER_STAT] )
    {
        update = true;
        update_stat();
    }
    if ( update )
    {
        fillRectA ( layers[LAYER_SCREEN_SURFACE], 0xFF000000 );
        SDL_BlitSurface ( layers[LAYER_BG], NULL, layers[LAYER_SCREEN_SURFACE], NULL );
        SDL_BlitSurface ( layers[LAYER_MAP], NULL, layers[LAYER_SCREEN_SURFACE], NULL );
        SDL_BlitSurface ( layers[LAYER_STAT], NULL, layers[LAYER_SCREEN_SURFACE], NULL );
        SDL_BlitSurface ( layers[LAYER_UI], NULL, layers[LAYER_SCREEN_SURFACE], NULL );
        //Update the surface
        SDL_UpdateWindowSurface ( gWindow );
    }
}
void mtgui::playSound ( sound s )
{
    if ( !mutese )
    {
        Mix_PlayChannel ( -1, sounds[s] , 0 );
    }
}
void mtgui::deinit()
{
    for ( int i = 0; i < FONT_COUNT; i++ )
    {
        TTF_CloseFont ( fonts[i] );
        fonts[i] = NULL;
    }
    for ( int i = 0; i < IMG_COUNT; i++ )
    {
        SDL_FreeSurface ( imgs[i] );
        imgs[i] = NULL;
    }
    for ( int i = 0; i < SOUND_COUNT; i++ )
    {
        Mix_FreeChunk ( sounds[i] );
        sounds[i] = NULL;
    }
    for ( int i = 0; i < LAYER_COUNT; i++ )
    {
        SDL_FreeSurface ( layers[i] );
        layers[i] = NULL;
    }
    Mix_CloseAudio();
    //Destroy window
    SDL_DestroyWindow ( gWindow );
    gWindow = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}

void mtgui::walk ( direction d )
{
    char buffer[256];
    sprintf ( buffer, "You are now at tower #%d. Type '/' for help on controlling. ", tower_id );
    char action[2] = {};
    int xd, yd;
    switch ( d )
    {
        case left:
            action[0] = 'h';
            xd = -1;
            yd = 0;
            break;
        case right:
            action[0] = 'l';
            xd = 1;
            yd = 0;
            break;
        case up:
            action[0] = 'k';
            xd = 0;
            yd = -1;
            break;
        case down:
            action[0] = 'j';
            xd = 0;
            yd = 1;
            break;
        default:
            SDL_Log ( "Walk in invalid direction!\n" );
            break;
    }
    int x = cur_stat.brave_x, y = cur_stat.brave_y;
    int xdest = x + xd, ydest = y + yd;
    const int MAPSIZE = mtstat::MAPSIZE;
    int cur_tile = 1;
    int before_bomb = cur_stat.brave_bomb;
    if ( xdest >= 0 && xdest < MAPSIZE && ydest >= 0 && ydest < MAPSIZE )
    {
        cur_tile = cur_stat.maps[cur_stat.brave_floor][ydest][xdest];
        if ( ( cur_tile >= 0x08 && cur_tile < 0x20 ) || cur_tile == mtstat::GOLD )
        {
            playSound ( CRYSTAL_OGG );
        }
        else if ( cur_tile >= 0x28 && cur_tile < 0x38 )
        {
            playSound ( POTION_OGG );
        }
        else if ( ( cur_tile >= 0x38 && cur_tile < 0x3b ) || cur_tile == mtstat::PICKAXE || cur_tile == mtstat::BOMB )
        {
            playSound ( KEY_OGG );
        }
        else if ( cur_tile == mtstat::TOKEN || cur_tile == mtstat::FLAG )
        {
            playSound ( TREASURE_OGG );
        }
        trace_append ( action );
        if ( xdest == cur_stat.brave_x && ydest == cur_stat.brave_y )
        {
            //As predicted.
            if ( cur_tile == mtstat::YELDOOR || cur_tile == mtstat::BLUEDOOR || cur_tile == mtstat::REDDOOR )
            {
                playSound ( DOOR_OGG );
            }
            else if ( cur_tile == mtstat::WALL )
            {
                playSound ( WALL_OGG );
            }
            else if ( cur_tile >= mtstat::MOB )
            {
                if ( cur_stat.brave_bomb == before_bomb - 1 )
                {
                    playSound ( BOMB_OGG );
                }
                else
                {
                    playSound ( BATTLE_OGG );
                }
            }
        }
    }
    layer_need_update[LAYER_MAP] = true;
    layer_need_update[LAYER_STAT] = true;
    face = d;
    if ( cur_ui != UI_PLAYING )
    {
        playSound ( WALK_OGG );
    }
    else
    {
        static bool toplay = false;
        if ( ( ( time_now() / 128 ) & 7 ) == 0 )
        {
            if ( toplay )
            {
                playSound ( WALK_OGG );
            }
            toplay = false;
        }
        else
        {
            toplay = true;
        }
    }
}

static std::string get_buffer_clipboard()
{
#ifdef __EMSCRIPTEN__
    const int BUF_LEN = 0x100000;
    char* data = new char[BUF_LEN];
    std::string result;
    if ( readline ( stdin, data, BUF_LEN ) != -1 )
    {
        result = std::string ( data );
    }
    delete[] data;
    return result;
#else
    if ( SDL_HasClipboardText() )
    {
        char* data = SDL_GetClipboardText();
        std::string result ( data );
        SDL_free ( data );
        return result;
    }
    return std::string();
#endif
}

bool mtgui::process_keydown ( SDL_Keycode sym, bool emulate_map )
{
    UI ccur_ui = emulate_map ? UI_MAP : cur_ui;
    switch ( sym )
    {
        case SDLK_COMMA:
            mutebgm = !mutebgm;
            if ( mutebgm )
            {
                Mix_Pause ( bgm_channel );
            }
            else
            {
                Mix_Resume ( bgm_channel );
            }
            break;
        case SDLK_PERIOD:
            mutese = !mutese;
            break;
        default:
            break;
    }
    switch ( ccur_ui )
    {
        case UI_MAP:
            switch ( sym )
            {
                //TODO: close music; mob book
                case SDLK_z:
                    if ( step_traces.length() != 0 )
                    {
                        step_traces.pop_back();
                        trace_rebuild ( step_traces );
                        layer_need_update[LAYER_MAP] = true;
                        layer_need_update[LAYER_STAT] = true;
                        layer_need_update[LAYER_UI] = true;
                        playSound ( CRYSTAL_OGG );
                    }
                    break;
                case SDLK_LEFT:
                case SDLK_h:
                    walk ( left );
                    break;
                case SDLK_RIGHT:
                case SDLK_l:
                    walk ( right );
                    break;
                case SDLK_UP:
                case SDLK_k:
                    walk ( up );
                    break;
                case SDLK_DOWN:
                case SDLK_j:
                    walk ( down );
                    break;
                case SDLK_g:
                case SDLK_RETURN:
                {
                    int old_x = cur_stat.brave_x, old_y = cur_stat.brave_y, old_f = cur_stat.brave_floor;
                    trace_append ( "g" );
                    layer_need_update[LAYER_STAT] = true;
                    layer_need_update[LAYER_UI] = true;
                    if ( old_x != cur_stat.brave_x || old_y != cur_stat.brave_y || old_f != cur_stat.brave_floor )
                    {
                        layer_need_update[LAYER_MAP] = true;
                        playSound ( PORTAL_OGG );
                    }
                }
                break;
                case SDLK_a:
                case SDLK_4:
                {
                    int old_at = cur_stat.brave_at;
                    trace_append ( "a" );
                    layer_need_update[LAYER_STAT] = true;
                    if ( old_at != cur_stat.brave_at )
                    {
                        layer_need_update[LAYER_MAP] = true;
                        playSound ( SHOP_OGG );
                    }
                }
                break;
                case SDLK_d:
                case SDLK_5:
                {
                    int old_df = cur_stat.brave_df;
                    trace_append ( "d" );
                    layer_need_update[LAYER_STAT] = true;
                    if ( old_df != cur_stat.brave_df )
                    {
                        layer_need_update[LAYER_MAP] = true;
                        playSound ( SHOP_OGG );
                    }
                }
                break;
                case SDLK_m:
                case SDLK_6:
                {
                    int old_mf = cur_stat.brave_mf;
                    trace_append ( "m" );
                    layer_need_update[LAYER_STAT] = true;
                    if ( old_mf != cur_stat.brave_mf )
                    {
                        layer_need_update[LAYER_MAP] = true;
                        playSound ( SHOP_OGG );
                    }
                }
                break;
                case SDLK_p:
                case SDLK_1:
                    trace_append ( "p" );
                    layer_need_update[LAYER_STAT] = true;
                    break;
                case SDLK_b:
                case SDLK_2:
                    trace_append ( "b" );
                    layer_need_update[LAYER_STAT] = true;
                    break;
                case SDLK_s:
                case SDLK_3:
                    trace_append ( "s" );
                    layer_need_update[LAYER_STAT] = true;
                    break;
                case SDLK_q:
                    //TODO: make a uniformed interface for INI/cookie access
                    //and SL io format.
                    cur_ui = UI_LOAD;
                    ui_val = 0;
                    layer_need_update[LAYER_UI] = true;
                    under_hint ( "Load savedata. " );
                    return false;
                    break;
                case SDLK_w:
                    cur_ui = UI_SAVE;
                    ui_val = 0;
                    layer_need_update[LAYER_UI] = true;
                    under_hint ( "Save savedata. " );
                    return false;
                    break;
                case SDLK_e:
                    cur_ui = UI_PLAY;
                    ui_val = 0;
                    layer_need_update[LAYER_UI] = true;
                    under_hint ( "Play savedata. " );
                    return false;
                case SDLK_r:
                    cur_stat = orig_stat;
                    step_traces.clear();
                    layer_need_update[LAYER_MAP] = true;
                    layer_need_update[LAYER_STAT] = true;
                    layer_need_update[LAYER_UI] = true;
                    playSound ( PORTAL_OGG );
                    return false;
                    break;
                case SDLK_t:
                    cur_ui = UI_MTID;
                    ui_val = 0; // the least significant digit.
                    ui_tower_id = tower_id;
                    layer_need_update[LAYER_UI] = true;
                    under_hint ( "Set the tower ID that you want to play. " );
                    return false;
                case SDLK_x:
                    cur_ui = UI_MOB_PEDIA;
                    layer_need_update[LAYER_UI] = true;
                    init_mob_pedia ( cur_stat );
                    ui_val = 0;
                    return false;
                    break;
                case SDLK_f:
                    show_graph = !show_graph;
                    layer_need_update[LAYER_UI] = true;
                    break;
                case SDLK_SLASH:
                    under_hint ( "Arrow/hjkl--walk, 1/p--pickaxe, 2/b--bomb, 3/s--shop, 4/a;5/d;6/m--+at;df;mf, q--load, w--save, e--play, r--reset, t--set towerID, ','--mute bgm, '.'--mute sound, '/'--help"
#ifdef __EMSCRIPTEN__
                                 ", '-'--load userdata, '='--dump userdata to stdout"
#endif
                               );
                    break;
#ifdef __EMSCRIPTEN__
                case SDLK_MINUS:
                {
                    std::string strdata;
                    if ( ( std::cin >> strdata ) && set_config ( strdata ) )
                    {
                        under_hint ( "Successfully replaced the config into this file. " );
                        playSound ( TREASURE_OGG );
                        transit_to_tower();
                    }
                    else
                    {
                        under_hint ( "The data is invalid!" );
                        playSound ( CRYSTAL_OGG );
                    }
                }
                break;
                case SDLK_EQUALS:
                {
                    std::string dmp = config.tostring();
                    printf ( "[userdata]\n%s\n", dmp.c_str() );
                }
                break;
#endif
                default:
                    break;
            }
            break;
        case UI_CLIPBOARD:
            switch ( sym )
            {
                case SDLK_RETURN:
                {
                    std::string clipboard_data = get_buffer_clipboard();
                    if ( !clipboard_data.empty() )
                    {
                        mtstat tmp_stat;
                        if ( tmp_stat.init_from_stream ( clipboard_data.c_str() ) )
                        {
                            std::string keystr = indexed ( "mtstat", tower_id ) ;
                            config[keystr] = clipboard_data;
                            transit_to_tower ( tower_id );
                            save_config();
                            return false;
                        }
                        else
                        {
                            under_hint ( "Data in clipboard is not valid mtstat data!" );
                            playSound ( CRYSTAL_OGG );
                        }
                    }
                    else
                    {
                        under_hint ( "No data in clipboard or clipboard inaccessible!" );
                        playSound ( CRYSTAL_OGG );
                    }
                }
                break;
                case SDLK_t:
                    cur_ui = UI_MTID;
                    ui_val = 0; // the least significant digit.
                    ui_tower_id = tower_id;
                    layer_need_update[LAYER_UI] = true;
                    under_hint ( "Set the tower ID that you want to play. " );
                    return false;
#ifdef __EMSCRIPTEN__
                case SDLK_MINUS:
                {
                    std::string strdata;
                    if ( ( std::cin >> strdata ) && set_config ( strdata ) )
                    {
                        under_hint ( "Successfully replaced the config into this file. To reset and see the effect, press 'r'. " );
                        playSound ( TREASURE_OGG );
                        transit_to_tower();
                    }
                    else
                    {
                        under_hint ( "The data is invalid!" );
                        playSound ( CRYSTAL_OGG );
                    }
                }
                break;
                case SDLK_EQUALS:
                {
                    std::string dmp = config.tostring();
                    printf ( "[userdata]\n%s\n", dmp.c_str() );
                }
                break;
#endif
                default:
                    break;
            }
            break;
        case UI_MOB_PEDIA:
            switch ( sym )
            {
                case SDLK_RETURN:
                case SDLK_x:
                    layer_need_update[LAYER_UI] = true;
                    cur_ui = UI_MAP;
                    return false;
                    break;
                case SDLK_LEFT:
                case SDLK_h:
                case SDLK_UP:
                case SDLK_k:
                {
                    int N = ( cur_stat.enemies.size() + MOB_PER_PAGE - 1 ) / MOB_PER_PAGE;
                    ui_val = ( ui_val + N - 1 ) % N;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_RIGHT:
                case SDLK_l:
                case SDLK_DOWN:
                case SDLK_j:
                {
                    int N = ( cur_stat.enemies.size() + MOB_PER_PAGE - 1 ) / MOB_PER_PAGE;
                    ui_val = ( ui_val + N + 1 ) % N;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                default:
                    break;
            }
            break;
        case UI_SAVE:
        case UI_LOAD:
        case UI_PLAY:
            switch ( sym )
            {
                case SDLK_MINUS:
                    if ( ccur_ui == UI_PLAY || ccur_ui == UI_LOAD )
                    {
                        strace_for_playing = get_buffer_clipboard();
                        LSP ( -1 );
                        layer_need_update[LAYER_UI] = true;
                        layer_need_update[LAYER_MAP] = true;
                        layer_need_update[LAYER_STAT] = true;
                    }
                    break;
                case SDLK_EQUALS:
                {
                    std::string index = indexed ( "mtsavedata", tower_id, ui_val );
                    if ( config.count ( index ) )
                    {
                        printf ( "[%s]:%s\n", index.c_str(), config[index].c_str() );
                    }
                }
                break;
                case SDLK_RETURN:
                    LSP ( ui_val );
                    layer_need_update[LAYER_UI] = true;
                    layer_need_update[LAYER_MAP] = true;
                    layer_need_update[LAYER_STAT] = true;
                    break;
                case SDLK_x:
                case SDLK_q:
                case SDLK_w:
                case SDLK_e:
                    layer_need_update[LAYER_UI] = true;
                    cur_ui = UI_MAP;
                    return false;
                    break;
                case SDLK_LEFT:
                case SDLK_h:
                {
                    ui_val = ( ui_val + save_slots - 5 ) % save_slots;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_UP:
                case SDLK_k:
                {
                    ui_val = ( ui_val + save_slots - 1 ) % save_slots;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_RIGHT:
                case SDLK_l:
                {
                    ui_val = ( ui_val + save_slots + 5 ) % save_slots;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_DOWN:
                case SDLK_j:
                {
                    ui_val = ( ui_val + save_slots + 1 ) % save_slots;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                default:
                    break;
            }
            break;
        case UI_MTID:
            switch ( sym )
            {
                case SDLK_RETURN:
                    transit_to_tower ( ui_tower_id );
                    layer_need_update[LAYER_UI] = true;
                    layer_need_update[LAYER_MAP] = true;
                    layer_need_update[LAYER_STAT] = true;
                    break;
                case SDLK_x:
                case SDLK_t:
                    layer_need_update[LAYER_UI] = true;
                    cur_ui = UI_MAP;
                    return false;
                    break;
                case SDLK_LEFT:
                case SDLK_h:
                {
                    ui_val = ( ui_val + 1 ) % 4;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_UP:
                case SDLK_k:
                {
                    int signifs[] = {1, 10, 100, 1000};
                    int signif = signifs[ui_val];
                    int dig = ( ui_tower_id / signif ) % 10;
                    if ( dig == 9 )
                    {
                        ui_tower_id -= 9 * signif;
                    }
                    else
                    {
                        ui_tower_id += signif;
                    }
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_RIGHT:
                case SDLK_l:
                {
                    ui_val = ( ui_val + 4 - 1 ) % 4;
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                case SDLK_DOWN:
                case SDLK_j:
                {
                    int signifs[] = {1, 10, 100, 1000};
                    int signif = signifs[ui_val];
                    int dig = ( ui_tower_id / signif ) % 10;
                    if ( dig == 0 )
                    {
                        ui_tower_id += 9 * signif;
                    }
                    else
                    {
                        ui_tower_id -= signif;
                    }
                    layer_need_update[LAYER_UI] = true;
                }
                break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return true;
}

void mtgui::enterFrame()
{
    int timenow = time_now();
    int cur_octity = ( timenow / 1 ) & 7;
    if ( cur_octity != octity )
    {
        octity = cur_octity;
        if ( cur_ui == UI_PLAYING )
        {
            char code = strace_for_playing[ui_val];
            SDL_Keycode emu_code;
            switch ( code )
            {
                case 'h':
                    emu_code = SDLK_h;
                    break;
                case 'j':
                    emu_code = SDLK_j;
                    break;
                case 'k':
                    emu_code = SDLK_k;
                    break;
                case 'l':
                    emu_code = SDLK_l;
                    break;
                case 'g':
                    emu_code = SDLK_g;
                    break;
                case 'p':
                    emu_code = SDLK_p;
                    break;
                case 'b':
                    emu_code = SDLK_b;
                    break;
                case 's':
                    emu_code = SDLK_s;
                    break;
                case 'a':
                    emu_code = SDLK_a;
                    break;
                case 'd':
                    emu_code = SDLK_d;
                    break;
                case 'm':
                    emu_code = SDLK_m;
                    break;
                default:
                    emu_code = 0;
                    fprintf ( stderr, "Exception when playing: an unexpected opcode for the route has been encountered. CHAR: %c (0x%x), ui_val=%d\n", code, code, ui_val );
                    break;
            }
            process_keydown ( emu_code, true );
            ui_val++;
            if ( ( size_t ) ui_val == strace_for_playing.length() )
            {
                trace_rebuild ( strace_for_playing );
                cur_ui = UI_MAP;
                playSound ( PORTAL_OGG );
            }
        }
    }
    if ( ( ( ( timenow / 500 ) & 1 ) != 0 ) != parity )
    {
        parity = !parity;
        layer_need_update[LAYER_MAP] = true;
        layer_need_update[LAYER_UI] = true;
    }
    SDL_Event event;
    while ( !quit )
    {
        if ( !SDL_PollEvent ( &event ) )
        {
            break;
        }
        if ( event.type == SDL_QUIT )
        {
            quit = true;
        }
        else
        {
            switch ( event.type )
            {
                case SDL_KEYDOWN:
                    if ( !process_keydown ( event.key.keysym.sym ) )
                    {
                        goto NO_FURTHER_EVENT;
                    }
                    break;
                default:
                    break;
            }
        }
    }
NO_FURTHER_EVENT:
    update_screen();
    if ( need_to_save_config && ( --save_config_clock ) == 0 )
    {
        need_to_save_config = false;
        save_strdata ( config.tostring() );
    }
}

#ifdef __EMSCRIPTEN__
void em_enterFrame();
#endif

void mtgui::run()
{
    if ( load_config() )
    {
        onload_config();
    }
    if ( SDL_Init ( SDL_INIT_VIDEO ) < 0 )
    {
        SDL_Log ( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return;
    }
    if ( !load_codecs() )
    {
        SDL_Log ( "Failed to load codecs!\n" );
        return;
    }
    if ( !load_assets() )
    {
        SDL_Log ( "Failed to load assets!\n" );
        return;
    }
    if ( !init_mainUI() )
    {
        SDL_Log ( "Failed to initialize main UI!\n" );
        return;
    }
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop ( em_enterFrame, 0, 1 );
#else
    quit = false;
    while ( !quit )
    {
        enterFrame();
        SDL_Delay ( 5 );
    }    //Free resources and close SDL
    deinit(); // In case the emscripten use the normal mode instead of infinite loop mode, the close must not be inside the main function on emscripten version
#endif
}
void mtgui::initialize_mtstat ( const char* stream )
{
    orig_stat.init_from_stream ( stream );
    cur_stat = orig_stat;
    face = down;
}
