/*
 * Auto generate by h2cppx
 */

#include "gtemplate.h"
#include "map_template_generator.h"
#include "eprintf.h"
#include <vector>
#include <algorithm>
//#include <set>
#include <cstdlib>

static const int MS = mtstat::MAPSIZE, MSQ = MS * MS;

int equiv_tile ( const std::vector<int>& map_serial, int f, int i, int j )
{
    if ( i < 0 || i >= MS || j < 0 || j >= MS )
    {
        return gtemplate::SYM_WALL;
    }
    if ( f < 0 || f * MSQ >= ( int ) map_serial.size() )
    {
        eprintf ( "The coordinate %d,%d,%d is invalid because the floor is not valid.\n", f, i, j );
        return gtemplate::SYM_WALL;
    }
    return map_serial[f * MSQ + i * MS + j];
}

bool diggable ( const std::vector<int>& map_serial, int f, int i, int j )
{
    if ( i < 0 || i >= MS )
    {
        return false;
    }
    if ( j < 0 || j >= MS )
    {
        return false;
    }
    if ( f < 0 || f * MSQ >= ( int ) map_serial.size() )
    {
        return false;
    }
    if ( map_serial[f * MSQ + i * MS + j] != gtemplate::SYM_WALL )
    {
        return false;
    }
    return ( ( equiv_tile ( map_serial, f, i, j - 1 ) == gtemplate::SYM_WALL
               && equiv_tile ( map_serial, f, i, j + 1 ) == gtemplate::SYM_WALL )
             || ( equiv_tile ( map_serial, f, i - 1, j ) == gtemplate::SYM_WALL
                  && equiv_tile ( map_serial, f, i + 1, j ) == gtemplate::SYM_WALL ) );
}

void dig_tile ( std::vector<int>& map_serial, std::vector<int>& grid_list, int coord )
{
    if ( map_serial[coord] == gtemplate::SYM_WALL )
    {
        map_serial[coord] = gtemplate::SYM_LOOT;
        grid_list.push_back ( coord );
    }
}

bool try_dig_tile ( std::vector<int>& map_serial, std::vector<int>& grid_list, int f, int i, int j )
{
    if ( diggable ( map_serial, f, i, j ) )
    {
        dig_tile ( map_serial, grid_list, f * MSQ + i * MS + j );
        return true;
    }
    return false;
}

void create_portal ( std::vector<int>& map_serial, std::vector<mtstat::portal_pair>& portals, std::vector<int>& grid_list, int coord1, int coord2 )
{
    // Don't build a portal on an empty tile! XXX
    dig_tile ( map_serial, grid_list, coord1 );
    dig_tile ( map_serial, grid_list, coord2 );
    map_serial[coord1] = gtemplate::SYM_EMPTY;
    map_serial[coord2] = gtemplate::SYM_EMPTY;
    portals.push_back ( {{coord1, coord2}} );
}

//bool big_room ( std::vector<int>& map_serial, std::vector<int>& grid_list, int coord )
//{
/*
   int f = coord / MS / MS, i = coord / MS % MS, j = coord % MS;
   bool row_flag[3] = {true, true, true}, col_flag[3] = {true, true, true};
   for ( int y = 0; y < 3; y++ )
   {
       for ( int x = 0; x < 3; x++ )
       {
           bool excavateable_flag = diggable ( map_serial, f, i + y - 1, j + x - 1 );
           row_flag[y] &= excavateable_flag;
           col_flag[x] &= excavateable_flag;
       }
   }
   int orig_glist_len = grid_list.size();
   for ( int z = 0; z < 3; z++ )
   {
       if ( row_flag[z] )
       {
           for ( int w = 0; w < 3; w++ )
           {
               dig_tile ( map_serial, grid_list, coord + ( z - 1 ) *MS + ( w - 1 ) );
           }
       }
       if ( col_flag[z] )
       {
           for ( int w = 0; w < 3; w++ )
           {
               dig_tile ( map_serial, grid_list, coord + ( z - 1 ) + ( w - 1 ) *MS );
           }
       }
   }
   return orig_glist_len < ( int ) grid_list.size();*/
//return false;
//}

bool spread ( std::vector<int>& map_serial, std::vector<mtstat::portal_pair>& portals, std::vector<int>& grid_list, int coord )
{
    //if ( map_serial[coord] == gtemplate::SYM_WALL )
    //{
    //    return false;
    //}
    int rd = rand();
    int choice = rd % 16;
    int f = coord / MS / MS, i = coord / MS % MS, j = coord % MS;
    if ( choice == 15 )
    {
        // make a big room
        //return big_room ( map_serial, grid_list, coord );
        choice = 14;
    }
    //else
    {
        //bool succeed_flag = false;
        for ( int t = 0; t < 5; t++ )
        {
            int real_choice = ( choice + t ) % 5;
            //int newf = f, newi = i, newj = j;
            switch ( real_choice )
            {
                case 0:
                    //left
                    if ( try_dig_tile ( map_serial, grid_list, f, i, j - 1 ) )
                    {
                        return true;
                    }
                    break;
                case 1:
                    //down
                    if ( try_dig_tile ( map_serial, grid_list, f, i + 1, j ) )
                    {
                        return true;
                    }
                    break;
                case 2:
                    //up
                    if ( try_dig_tile ( map_serial, grid_list, f, i - 1, j ) )
                    {
                        return true;
                    }
                    break;
                case 3:
                    //right
                    if ( try_dig_tile ( map_serial, grid_list, f, i, j + 1 ) )
                    {
                        return true;
                    }
                    break;
                default:
                    //portal
                {
                    bool fail = false;
                    for ( int x = 0; x < 9; x++ )
                    {
                        for ( int y = 0; y < 9; y++ )
                        {
                            if ( equiv_tile ( map_serial, f, i + x - 4, j + y - 4 ) == gtemplate::SYM_EMPTY )
                            {
                                fail = true;
                                goto FAIL;
                            }
                        }
                    }
FAIL:
                    if ( fail )
                    {
                        continue;
                    }
                    for ( int t = 0; t < 16; t++ )
                    {
                        int cur_coord = rand() % map_serial.size();
                        int cf = cur_coord / MS / MS, ci = cur_coord / MS % MS, cj = cur_coord % MS;
                        for ( int x = 0; x < 9; x++ )
                        {
                            for ( int y = 0; y < 9; y++ )
                            {
                                if ( equiv_tile ( map_serial, cf, ci + x - 4, cj + y - 4 ) == gtemplate::SYM_EMPTY )
                                {
                                    continue;
                                }
                            }
                        }
                        if ( diggable ( map_serial, cur_coord / MS / MS, cur_coord / MS % MS, cur_coord % MS ) )
                        {
                            create_portal ( map_serial, portals, grid_list, coord, cur_coord );
                            return true;
                            //succeed_flag = true;
                            //goto FINAL;
                        }
                    }
                }
                break;
            }
        }
        //FINAL:
        return false;
    }
}

bool doorable ( const std::vector<int>& map_serial, int f, int i, int j )
{
    return ( equiv_tile ( map_serial, f, i, j - 1 ) == gtemplate::SYM_WALL
             && equiv_tile ( map_serial, f, i, j + 1 ) == gtemplate::SYM_WALL
             && equiv_tile ( map_serial, f, i - 1, j ) != gtemplate::SYM_WALL
             && equiv_tile ( map_serial, f, i + 1, j ) != gtemplate::SYM_WALL )
           || ( equiv_tile ( map_serial, f, i, j - 1 ) != gtemplate::SYM_WALL
                && equiv_tile ( map_serial, f, i, j + 1 ) != gtemplate::SYM_WALL
                && equiv_tile ( map_serial, f, i - 1, j ) == gtemplate::SYM_WALL
                && equiv_tile ( map_serial, f, i + 1, j ) == gtemplate::SYM_WALL )
           ;
}

bool obstacleable ( const std::vector<int>& map_serial, int f, int i, int j )
{
    bool surrounding[8];
    int ioffset[8] = { -1, -1, -1, 0, 1, 1, 1, 0}, joffset[8] = { -1, 0, 1, 1, 1, 0, -1, -1};
    for ( int t = 0; t < 8; t++ )
    {
        surrounding[t] = equiv_tile ( map_serial, f, i + ioffset[t], j + joffset[t] ) == gtemplate::SYM_WALL;
    }
    for ( int t = 0; t < 4; t++ )
    {
        int p = ( 2 * t + 1 ) % 8, q = ( p + 1 ) % 8, r = ( q + 1 ) % 8;
        if ( surrounding[p] && surrounding[r] )
        {
            surrounding[q] = true;
        }
    }
    bool cur_color = surrounding[0];
    int begin_point = 0;
    for ( begin_point = 1; begin_point < 8; begin_point++ )
    {
        if ( surrounding[begin_point] != cur_color )
        {
            cur_color = !cur_color;
            break;
        }
    }
    if ( begin_point == 8 )
    {
        return false;
    }
    int cur_point = begin_point;
    for ( int t = 0; t < 2; t++ )
    {
        for ( ; ; cur_point++ )
        {
            if ( surrounding[cur_point % 8] != cur_color )
            {
                cur_color = !cur_color;
                break;
            }
        }
    }
    if ( ( cur_point - begin_point ) % 8 == 0 )
    {
        return false;
    }
    return true;
}

genmaptempl_return_t generate_map_template ( int floors, int orig_coord, std::vector<mtstat::map> restriction )
{
    genmaptempl_return_t ret;
    ret.maps = std::vector<mtstat::map> ( floors );
    std::vector<int> map_serial ( floors * MSQ, gtemplate::SYM_WALL );
    map_serial[orig_coord] = gtemplate::SYM_EMPTY;
    std::vector<int> grid_list;
    grid_list.push_back ( orig_coord );
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < MS; i++ )
        {
            for ( int j = 0; j < MS; j++ )
            {
                if ( restriction[f][i][j] != gtemplate::SYM_WALL )
                {
                    int coord = f * MSQ + i * MS + j;
                    map_serial[coord] = gtemplate::SYM_EMPTY;
                    //grid_list.push_back ( coord );
                }
            }
        }
    }
    //std::set<int> chosen_node;
    //chosen_node.insert(orig_coord);
    // excavate
    {
        bool flag = true;
        while ( flag )
        {
            flag = false;
            std::random_shuffle ( grid_list.begin(), grid_list.end() );
            for ( int coord : grid_list )
            {
                if ( spread ( map_serial, ret.portals, grid_list, coord ) )
                {
                    flag = true;
                    break;
                }
            }
#ifdef DEBUG
            /*
            for ( int f = 0; f < floors; f++ )
            {
                for ( int i = 0; i < MS; i++ )
                {
                    for ( int j = 0; j < MS; j++ )
                    {
                        fprintf ( stderr, "%2d", map_serial[f * MSQ + i * MS + j] );
                    }
                    fputs ( "\n", stderr );
                }
                fputs ( "\n", stderr );
            }
            fputs ( "=====\n", stderr );
            */
#endif
        }
    }
#ifdef DEBUG
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < MS; i++ )
        {
            for ( int j = 0; j < MS; j++ )
            {
                fprintf ( stderr, "%2d", map_serial[f * MSQ + i * MS + j] );
            }
            fputs ( "\n", stderr );
        }
        fputs ( "\n", stderr );
    }
    fputs ( "=====\n", stderr );
#endif
    std::vector<int> order ( floors * MSQ );
    for ( int i = 0, s = floors * MSQ; i < s; i++ )
    {
        order[i] = i;
    }
    std::random_shuffle ( order.begin(), order.end() );
    for ( int stage = 0; stage < 5; stage++ )
    {
        for ( int coord : order )
        {
            if ( map_serial[coord] != gtemplate::SYM_EMPTY
                    && map_serial[coord] != gtemplate::SYM_WALL )
            {
                int f = coord / MS / MS, i = coord / MS % MS, j = coord % MS;
                int sur_count0[gtemplate::SYM_COUNT] = {};
                int sur_count1[gtemplate::SYM_COUNT] = {};
                int sur_count2[gtemplate::SYM_COUNT] = {};
                for ( int x = 0; x < 3; x++ )
                {
                    for ( int y = 0; y < 3; y++ )
                    {
                        int e_tile = equiv_tile ( map_serial, f, i + x - 1, j + y - 1 );
                        sur_count1[e_tile]++;
                        if ( ( x + y ) % 2 == 1 )
                        {
                            sur_count0[e_tile]++;
                        }
                    }
                }
                for ( int x = 0; x < 5; x++ )
                {
                    for ( int y = 0; y < 5; y++ )
                    {
                        int e_tile = equiv_tile ( map_serial, f, i + x - 2, j + y - 2 );
                        sur_count2[e_tile]++;
                    }
                }
                switch ( stage )
                {
                    case 0:
                        if ( map_serial[coord] == gtemplate::SYM_LOOT && doorable ( map_serial, f, i, j ) )
                        {
                            int val = gtemplate::SYM_DOOR;
                            map_serial[coord] = val;
                        }
                        break;
                    case 1:
                        if ( map_serial[coord] == gtemplate::SYM_DOOR )
                        {
                            if ( sur_count0[gtemplate::SYM_DOOR] != 0 || sur_count1[gtemplate::SYM_DOOR] > 2 )
                            {
                                map_serial[coord] = gtemplate::SYM_LOOT;
                            }
                        }
                        break;
                    case 2:
                        if ( map_serial[coord] == gtemplate::SYM_LOOT && obstacleable ( map_serial, f, i, j ) )
                        {
                            map_serial[coord] = gtemplate::SYM_MOB;
                        }
                        break;
                    case 3:
                        if ( map_serial[coord] == gtemplate::SYM_MOB &&
                                ( sur_count0[gtemplate::SYM_MOB] > 1
                                  || sur_count2[gtemplate::SYM_MOB] > 7 ) )
                        {
                            map_serial[coord] = gtemplate::SYM_LOOT;
                        }
                        break;
                    default:
                        if ( map_serial[coord] == gtemplate::SYM_LOOT && ( sur_count2[gtemplate::SYM_LOOT] > 12 || sur_count0[gtemplate::SYM_LOOT] > 3 ) )
                        {
                            map_serial[coord] = gtemplate::SYM_EMPTY;
                        }
                        break;
                }
            }
        }
    }
#ifdef DEBUG
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < MS; i++ )
        {
            for ( int j = 0; j < MS; j++ )
            {
                fprintf ( stderr, "%2d", map_serial[f * MSQ + i * MS + j] );
            }
            fputs ( "\n", stderr );
        }
        fputs ( "\n", stderr );
    }
    fputs ( "=====\n", stderr );
#endif
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < MS; i++ )
        {
            for ( int j = 0; j < MS; j++ )
            {
                if ( restriction[f][i][j] != gtemplate::SYM_WALL )
                {
                    ret.maps[f][i][j] = restriction[f][i][j];
                }
                else
                {
                    ret.maps[f][i][j] = map_serial[f * MSQ + i * MS + j];
                }
            }
        }
    }
    return ret;
}
