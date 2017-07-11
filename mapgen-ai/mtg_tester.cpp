// =====================================================================================
//
//       Filename:  mtg_tester.cpp
//
//    Description:  test out the map template generator
//
//        Version:  1.0
//        Created:  10/18/2016 09:42:43 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  thiner (),
//   Organization:
//
// =====================================================================================

#include "mtai/mtstat.h"
#include "mtai/map_template_generator.h"
#include "mtai/enemy_generator.h"
#include "mtai/gtemplate.h"
#include "mtai/gsheet.h"
#include "mtai/gsheet_filler.h"
#include "mtai/eprintf.h"

#include "mtai/ai_core.h"
#include "mtai/ginterface.h"
#include "mtai/mtstat.h"

#include <stdio.h>
#include <string>
#include "mtai/eprintf.h"
#include <unistd.h>

#include <signal.h>

#include <cassert>

#include <cstdlib>

#include <climits>

//const long NINF=LONG_MIN;
//
const bool basic = false;

void interrupt_handler ( int signo )
{
    ( void ) signo;
    eprintf ( "Interrupted!\n" );
    interrupted = true;
}

ai_return_t ai_route ( const ginterface& gi, int score = 0, int time_limit = 60 )
{
    int sub_timelimit = time_limit / 4;
    for ( int i = 0; i < 4; i++ )
    {
        interrupted = false;
        alarm ( sub_timelimit * ( i + 1 ) );
        ai_return_t result = opt_from_beginning ( gi, AI_MED_4 );
        alarm ( 0 );
        interrupted = false;
        gstate gs ( gi.new_initial_state() );
        gi.propagate ( gs, result.el );
        if ( gs.ev[EV_SALT] == 0 && gs.ev[EV_FLAG] > 0 && gi.score ( gs ) > score )
        {
            return result;
        }
    }
    return ai_return_t();
}

long difficulty ( const ginterface& gi, const ai_return_t& airet )
{
    // if can not pass return -inf
    //XXX inefficient
    gstate gs = gi.new_initial_state();
    return ai_difficulty ( gi, gs, airet.gr );
    /*if ( !gi.propagate ( gs, airet.el ) )
    {
        return LONG_MIN;
    }
    */
    return 0;
}

const int MAX_BUF = 1048576;

int read_to_buffer ( char* buf, int maxread, FILE* f )
{
    int s = fread ( buf, 1, maxread, f );
    if ( s == maxread )
    {
        buf[maxread] = 0;
    }
    return s;
}

int main()
{
    if ( signal ( SIGALRM, interrupt_handler ) == SIG_ERR )
    {
        eprintf ( "Failed to set the alarm handler!\n" );
    }
    if ( signal ( SIGINT, interrupt_handler ) == SIG_ERR )
    {
        eprintf ( "Failed to set the interrupt handler!\n" );
    }
    srand ( time ( 0 ) );
    char* buf = new char[MAX_BUF];
    read_to_buffer ( buf, MAX_BUF, stdin );
    mtstat mts;
    mts.init_from_stream ( buf );
    int total_es = mts.enemies.size();
    for ( int i = 0, s = total_es; i < s; i++ )
    {
        mts.enemies[i] = generate_enemy ( 4 + i );
    }
    mts.enemies[total_es - 1][4] |= mtstat::BF;
    int floors = mts.maps.size();
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < mtstat::MAPSIZE; i++ )
        {
            for ( int j = 0; j < mtstat::MAPSIZE; j++ )
            {
                mts.maps[f][i][j] = gtemplate::SYM_WALL;
            }
        }
    }
    mts.maps[mts.brave_floor][mts.brave_y][mts.brave_x] = gtemplate::SYM_EMPTY;
    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 4; j < 11; j++ )
        {
            int new_tile = mtstat::SOLIDWALL;
            mts.maps[0][i][j] = new_tile;
        }
    }
    for ( int i = 0; i < 7; i++ )
    {
        for ( int j = 5; j < 11; j += 2 )
        {
            int new_tile = gtemplate::SYM_LOOT;
            mts.maps[0][i][j] = new_tile;
        }
    }
    mts.maps[0][0][5] = mtstat::TOKEN;
    mts.maps[0][0][7] = mtstat::FLAG;
    mts.maps[0][0][9] = mtstat::TOKEN;
    mts.maps[0][1][5] = mtstat::MOB + mts.enemies.size() / 3 - 1;
    mts.maps[0][1][7] = mtstat::MOB + mts.enemies.size() - 1;
    mts.maps[0][1][9] = mtstat::MOB + 2 * mts.enemies.size() / 3 - 1;
    if ( floors == 1 && basic )
    {
        mts.maps[mts.brave_floor][mts.brave_y][mts.brave_x] = gtemplate::SYM_EMPTY;
        for ( int i = 0; i < 15; i++ )
        {
            for ( int j = 0; j < 15; j++ )
            {
                int new_tile = mtstat::EMPTY;
                mts.maps[0][i][j] = new_tile;
            }
        }
        for ( int i = 0; i < 5; i++ )
        {
            for ( int j = 0; j < 15; j++ )
            {
                mts.maps[0][i][j] = ( j & 1 ) ? gtemplate::SYM_WALL : ( i == 4 ? gtemplate::SYM_MOB : gtemplate::SYM_LOOT );
            }
        }
        for ( int j = 0; j < 14; j++ )
        {
            mts.maps[0][6][j] = gtemplate::SYM_WALL;
        }
        for ( int i = 7; i < 12; i++ )
        {
            for ( int j = 0; j < 14; j++ )
            {
                mts.maps[0][i][j] = ( j & 1 ) ? gtemplate::SYM_WALL : ( i == 11 ? gtemplate::SYM_MOB : gtemplate::SYM_LOOT );
            }
        }
        mts.maps[0][0][6] = mtstat::FLAG;
        mts.maps[0][1][6] = mtstat::MOB + mts.enemies.size() - 1;
    }
    mtstat result;
    ai_return_t ai_choice;
    long max_difficulty = LONG_MIN;
    while ( true )
    {
        if ( interrupted )
        {
            break;
        }
        mtstat cur_mts = mts;
        if ( floors != 1 || !basic )
        {
            genmaptempl_return_t grt = generate_map_template ( floors, mts.brave_floor * mtstat::MAPSIZE * mtstat::MAPSIZE + mts.brave_y * mtstat::MAPSIZE + mts.brave_x, mts.maps );
            if ( basic )
            {
                for ( int f = 0; f < floors; f++ )
                {
                    for ( int i = 0; i < mtstat::MAPSIZE; i++ )
                        for ( int j = 0; j < mtstat::MAPSIZE; j++ )
                        {
                            if ( grt.maps[f][i][j] == gtemplate::SYM_DOOR )
                            {
                                grt.maps[f][i][j] = gtemplate::SYM_LOOT;
                            }
                        }
                }
            }
            cur_mts.maps = grt.maps;
            cur_mts.portals = grt.portals;
        }
        gtemplate gt ( cur_mts );
        ginterface gi ( gt );
        gt.set_node_map ( gi.int_graph() );
        gsheet max_gsh ( gt );
        gsheet gsh ( gt );
        gsheet_filler gshfiller ( mts, basic );
        gshfiller.set_loot_range ( 0, floors, 0, 3 );
        gsheet_filler gshfiller_weak ( gshfiller );
        gshfiller_weak.set_mob_range ( 0, floors, 0, 1 );
        gshfiller_weak.random_fill_gsheet ( max_gsh );
        gi.update_gsheet ( max_gsh );
        ai_return_t airet = ai_route ( gi, 0, 4 );
        if ( airet.el.empty() )
        {
            eprintf ( "CANNOT pass! turn to another map.\n" );
            continue;
        }
        long local_max_dif = difficulty ( gi, airet );
        for ( int i = 0; i < 64; i++ )
        {
            if ( interrupted )
            {
                break;
            }
            for ( int l = 16; l >= 1; l >>= 1 )
            {
                if ( interrupted )
                {
                    break;
                }
                for ( int j = 0; j < 512; j++ )
                {
                    if ( interrupted )
                    {
                        break;
                    }
                    eprintf ( "opt wave: %d, %d, %d\n", i, l, j );
                    gsh = max_gsh;
                    if ( j & 1 )
                    {
                        gshfiller.random_mutate_gsheet ( gsh, l );
                    }
                    else
                    {
                        if ( j & 2 )
                        {
                            int total_ms = gt.mobs_coordinate().size();
                            for ( int t = 0, k = 0; t < l * 65536 && k < l; t++ )
                            {
                                // mob
                                int cur_id = rand() % total_ms;
                                int pivot = gsh.mobs[cur_id] - mtstat::MOB;
                                if ( rand() % total_es > pivot )
                                {
                                    gsh.mobs[cur_id]++;
                                    k++;
                                    gsh.set_mod_coord ( gt.mobs_coordinate() [cur_id] );
                                }
                            }
                        }
                        else
                        {
                            int total_ls = gt.loots_coordinate().size();
                            for ( int t = 0, k = 0; t < l * 65536 && k < l; t++ )
                            {
                                // loot
                                int cur_id = rand() % total_ls;
                                if ( gsh.loots[cur_id] < mtstat::POTION )
                                {
                                    int pivot = gsh.loots[cur_id] & 7;
                                    if ( rand() % 8 < pivot )
                                    {
                                        gsh.loots[cur_id]--;
                                        k++;
                                        gsh.set_mod_coord ( gt.loots_coordinate() [cur_id] );
                                    }
                                }
                                else if ( gsh.loots[cur_id] < mtstat::YELKEY )
                                {
                                    int pivot = gsh.loots[cur_id] & 7;
                                    if ( rand() % 8 < pivot )
                                    {
                                        gsh.loots[cur_id]--;
                                        k++;
                                        gsh.set_mod_coord ( gt.loots_coordinate() [cur_id] );
                                    }
                                }
                            }
                        }
                    }
                    graph::bools modified = gsh.mod_bools;
                    gi.update_gsheet ( gsh );
                    long dif = difficulty ( gi, airet );
                    if ( dif > local_max_dif || ( dif == local_max_dif && ! ( j & 1 ) ) )
                    {
                        eprintf ( "modified! %ld -> %ld\n", local_max_dif, dif );
                        /*
                        {
                            mtstat mtcounter = gsh.output();
                            ginterface gi_counter ( mtcounter );
                            gstate gs = gi_counter.new_initial_state();
                            assert ( gi_counter.propagate ( gs, airet.el ) );
                        }*/
                        local_max_dif = dif;
                        max_gsh = gsh;
                        gstate gs = gi.new_initial_state();
                        airet = translate ( gi, gi.new_initial_state(), airet.gr );
                    }
                    else
                    {
                        max_gsh.mod_bools = modified;
                        gi.update_gsheet ( max_gsh );
                        eprintf ( "unmodified! max is %ld while you are %ld . \n", local_max_dif, dif );
                    }
                    /*
                    {
                        //debug
                        gstate gs = gi.new_initial_state();
                        bool flag = gi.propagate ( gs, airet.el );
                        assert ( flag );
                    }*/
                }
            }
            if ( interrupted )
            {
                break;
            }
            gstate gs = gi.new_initial_state();
            bool flag = gi.propagate ( gs, airet.el );
            assert ( flag );
            ai_return_t new_airet = ai_route ( gi, gi.score ( gs ) );
            if ( !new_airet.el.empty() )
            {
                airet = new_airet;
                local_max_dif = difficulty ( gi, airet );
            }
        }
        long dif = difficulty ( gi, airet );
        if ( dif > max_difficulty )
        {
            max_difficulty = dif;
            result = gsh.output();
            ai_choice = airet;
        }
    }
    std::string opstr = result.to_string();
    printf ( "%s", opstr.c_str() );
    mtstat new_mts = result;
    ginterface gi ( new_mts );
    std::string rtstr = gi.translate ( ai_choice.el );
    printf ( "[route]%s\n", rtstr.c_str() );
    new_mts.process_from_stream ( rtstr.c_str(), rtstr.length() );
    printf ( "[score]%d\n", new_mts.score() );
    printf ( "[hp]%d\n", new_mts.brave_hp );
    return 0;
}
