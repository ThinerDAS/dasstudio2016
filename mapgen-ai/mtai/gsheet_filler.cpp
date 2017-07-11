/*
 * Auto generate by h2cppx
 */


#include "gsheet_filler.h"

#include <cstdlib>
#include <algorithm>

template<typename F>
static void fill_spec ( std::vector<int>& v_to_fill, int begin, int end, const std::vector<int>& value_range, const std::vector<int>& coords, F spec_chk )
{
    int s = value_range.size();
    for ( int i = begin; i < end; i++ )
    {
        if ( spec_chk ( coords[i] ) )
        {
            v_to_fill[i] = value_range[rand() % s];
        }
    }
}

static void fill_random_in_range ( std::vector<int>& v_to_fill, int begin, int end, const std::vector<int>& value_range )
{
    int s = value_range.size();
    for ( int i = begin; i < end; i++ )
    {
        v_to_fill[i] = value_range[rand() % s];
    }
}

gsheet_filler::gsheet_filler ( const mtstat& mts, bool basic ) : m_mts ( mts ), m_basic ( basic )
{
    int mob_begin = 0;
    int mob_range = mts.enemies.size();
    int loot_begin = 0;
    int loot_range = 1;
    int floors = mts.maps.size();
    std::vector<int> door_choice;
    door_choices.reserve ( 6 * mob_range );
    for ( int i = 0; i < 4 * mob_range; i++ )
    {
        door_choice.push_back ( mtstat::YELDOOR );
    }
    for ( int i = 0; i < mob_range; i++ )
    {
        door_choice.push_back ( mtstat::BLUEDOOR );
    }
    for ( int i = 0; i < mob_range; i++ )
    {
        if ( mts.enemies[mob_begin + i][4]&mtstat::BF )
        {
            door_choice.push_back ( mtstat::MOB + mob_begin + i );
        }
    }
    std::vector<int> mob_choice;
    mob_choice.reserve ( mob_range );
    for ( int i = 0; i < mob_range; i++ )
    {
        mob_choice.push_back ( mtstat::MOB + mob_begin + i );
    }
    std::vector<int> loot_choice, w_choice;
    loot_choice.reserve ( 6 * loot_range + 8 );
    for ( int i = 0; i < loot_range; i++ )
    {
        loot_choice.push_back ( mtstat::REDCRY + loot_begin + i );
        loot_choice.push_back ( mtstat::BLUECRY + loot_begin + i );
        loot_choice.push_back ( mtstat::GREENCRY + loot_begin + i );
        w_choice.push_back ( mtstat::WHITECRY + loot_begin + i );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 + 1 );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 + 1 );
        if ( !m_basic )
        {
            loot_choice.push_back ( mtstat::YELKEY );
            if ( ( i & 3 ) == 0 )
            {
                loot_choice.push_back ( mtstat::BLUEKEY );
            }
        }
    }
    for ( int i = 0; i < 3; i++ )
    {
        loot_choice.insert ( loot_choice.end(), loot_choice.begin(), loot_choice.end() );
    }
    if ( !m_basic )
    {
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::BLUEKEY );
        loot_choice.push_back ( mtstat::GOLD );
        loot_choice.push_back ( mtstat::PICKAXE );
        loot_choice.push_back ( mtstat::BOMB );
    }
    door_choices = std::vector<std::vector<int>> ( floors, door_choice );
    mob_choices = std::vector<std::vector<int>> ( floors, mob_choice );
    loot_choices = std::vector<std::vector<int>> ( floors, loot_choice );
    w_choices = std::vector<std::vector<int>> ( floors, w_choice );
}

void gsheet_filler::set_mob_range ( int floor_begin, int floor_end, int mob_begin, int mob_end )
{
    int mob_range = mob_end - mob_begin;
    std::vector<int> door_choice, bfm_choice;
    door_choices.reserve ( 6 * mob_range );
    for ( int i = 0; i < 4 * mob_range; i++ )
    {
        door_choice.push_back ( mtstat::YELDOOR );
    }
    for ( int i = 0; i < mob_range; i++ )
    {
        door_choice.push_back ( mtstat::BLUEDOOR );
    }
    for ( int i = 0; i < mob_range; i++ )
    {
        if ( m_mts.enemies[mob_begin + i][4]&mtstat::BF )
        {
            door_choice.push_back ( mtstat::MOB + mob_begin + i );
        }
    }
    std::vector<int> mob_choice;
    mob_choice.reserve ( mob_range );
    for ( int i = 0; i < mob_range; i++ )
    {
        mob_choice.push_back ( mtstat::MOB + mob_begin + i );
    }
    for ( int i = floor_begin; i < floor_end; i++ )
    {
        door_choices[i] = door_choice;
        mob_choices[i] = mob_choice;
    }
}

void gsheet_filler::set_loot_range ( int floor_begin, int floor_end, int loot_begin, int loot_end )
{
    int loot_range = loot_end - loot_begin;
    std::vector<int> loot_choice, w_choice;
    //loot_choice.reserve ( 6 * loot_range + 8 );
    for ( int i = 0; i < loot_range; i++ )
    {
        loot_choice.push_back ( mtstat::REDCRY + loot_begin + i );
        loot_choice.push_back ( mtstat::BLUECRY + loot_begin + i );
        loot_choice.push_back ( mtstat::GREENCRY + loot_begin + i );
        w_choice.push_back ( mtstat::WHITECRY + loot_begin + i );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 + 1 );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 );
        loot_choice.push_back ( mtstat::POTION + loot_begin + i * 2 + 1 );
        if ( !m_basic )
        {
            loot_choice.push_back ( mtstat::YELKEY );
            if ( ( i & 3 ) == 0 )
            {
                loot_choice.push_back ( mtstat::BLUEKEY );
            }
        }
    }
    for ( int i = 0; i < 1 + 2 / loot_range; i++ )
    {
        loot_choice.insert ( loot_choice.end(), loot_choice.begin(), loot_choice.end() );
    }
    if ( !m_basic )
    {
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::YELKEY );
        loot_choice.push_back ( mtstat::BLUEKEY );
        loot_choice.push_back ( mtstat::GOLD );
        loot_choice.push_back ( mtstat::PICKAXE );
        loot_choice.push_back ( mtstat::BOMB );
    }
    for ( int i = floor_begin; i < floor_end; i++ )
    {
        loot_choices[i] = loot_choice;
        w_choices[i] = w_choice;
    }
}

void gsheet_filler::random_fill_gsheet ( gsheet& gs ) const
{
    int floors = door_choices.size();
    auto spec_chk = [] ( int i )
    {
        return gtemplate::spec ( i );
    };
    const int MS = mtstat::MAPSIZE, MSQ = MS * MS;
    {
        const std::vector<int>& crs = gs.gt().doors_coordinate();
        std::vector<int>& v_to_fill = gs.doors;
        int begin = 0;
        for ( int i = 0; i < floors; i++ )
        {
            int end = std::distance ( crs.begin(), std::lower_bound ( crs.begin(), crs.end(), ( i + 1 ) * MSQ ) );
            fill_random_in_range ( v_to_fill, begin, end, door_choices[i] );
            begin = end;
        }
    }
    {
        const std::vector<int>& crs = gs.gt().mobs_coordinate();
        std::vector<int>& v_to_fill = gs.mobs;
        int begin = 0;
        for ( int i = 0; i < floors; i++ )
        {
            int end = std::distance ( crs.begin(), std::lower_bound ( crs.begin(), crs.end(), ( i + 1 ) * MSQ ) );
            fill_random_in_range ( v_to_fill, begin, end, mob_choices[i] );
            begin = end;
        }
    }
    {
        const std::vector<int>& crs = gs.gt().loots_coordinate();
        std::vector<int>& v_to_fill = gs.loots;
        int begin = 0;
        for ( int i = 0; i < floors; i++ )
        {
            int end = std::distance ( crs.begin(), std::lower_bound ( crs.begin(), crs.end(), ( i + 1 ) * MSQ ) );
            fill_random_in_range ( v_to_fill, begin, end, loot_choices[i] );
            fill_spec ( v_to_fill, begin, end, w_choices[i], crs, spec_chk );
            begin = end;
        }
    }
    for ( int i : gs.gt().doors_coordinate() )
    {
        gs.set_mod_coord ( i );
    }
    for ( int i : gs.gt().mobs_coordinate() )
    {
        gs.set_mod_coord ( i );
    }
    for ( int i : gs.gt().loots_coordinate() )
    {
        gs.set_mod_coord ( i );
    }
}

void gsheet_filler::random_mutate_gsheet ( gsheet& gs, int mutate_points ) const
{
    int i = rand() % 4, end = i + mutate_points;
    const int MS = mtstat::MAPSIZE, MSQ = MS * MS;
    for ( ; i < end; i++ )
    {
        switch ( i % 4 )
        {
            case 0:
            {
                const std::vector<int>& crs = gs.gt().doors_coordinate();
                if ( crs.size() == 0 )
                {
                    continue;
                }
                std::vector<int>& v_to_fill = gs.doors;
                int randpick = rand() % crs.size();
                int coord = crs[randpick];
                int floor = coord / MSQ;
                const std::vector<int>& choice = door_choices[floor];
                v_to_fill[randpick] = choice[rand() % choice.size()];
                gs.set_mod_coord ( coord );
            }
            break;
            case 1:
            {
                const std::vector<int>& crs = gs.gt().mobs_coordinate();
                if ( crs.size() == 0 )
                {
                    continue;
                }
                std::vector<int>& v_to_fill = gs.mobs;
                int randpick = rand() % crs.size();
                int coord = crs[randpick];
                int floor = coord / MSQ;
                const std::vector<int>& choice = mob_choices[floor];
                v_to_fill[randpick] = choice[rand() % choice.size()];
                gs.set_mod_coord ( coord );
            }
            break;
            default:
            {
                const std::vector<int>& crs = gs.gt().loots_coordinate();
                if ( crs.size() == 0 )
                {
                    continue;
                }
                std::vector<int>& v_to_fill = gs.loots;
                int randpick = rand() % crs.size();
                int coord = crs[randpick];
                int floor = coord / MSQ;
                const std::vector<int>& choice = gtemplate::spec ( coord ) ? w_choices[floor] : loot_choices[floor];
                v_to_fill[randpick] = choice[rand() % choice.size()];
                gs.set_mod_coord ( coord );
            }
            break;
        }
    }
}

