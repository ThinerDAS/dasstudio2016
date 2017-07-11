/*
 * Auto generate by h2cppx
 */

#include <cassert>
#include "gsheet.h"
#include "eprintf.h"

gsheet::gsheet ( const gtemplate& gt ) : m_gt ( &gt ), doors ( gt.doors_coordinate().size() ), mobs ( gt.mobs_coordinate().size() ), loots ( gt.loots_coordinate().size() )
{
}

mtstat gsheet::output() const
{
    assert ( doors.size() == m_gt->doors_coordinate().size() );
    assert ( mobs.size() == m_gt->mobs_coordinate().size() );
    assert ( loots.size() == m_gt->loots_coordinate().size() );
    mtstat res = initialmts();
    const int MS = mtstat::MAPSIZE;
    for ( int t = 0, s = doors.size(); t < s; t++ )
    {
        int coord = m_gt->doors_coordinate() [t];
        int f = coord / ( MS * MS ), i = ( coord / MS ) % MS, j = coord % MS;
        res.maps[f][i][j] = doors[t];
    }
    for ( int t = 0, s = mobs.size(); t < s; t++ )
    {
        int coord = m_gt->mobs_coordinate() [t];
        int f = coord / ( MS * MS ), i = ( coord / MS ) % MS, j = coord % MS;
        res.maps[f][i][j] = mobs[t];
    }
    for ( int t = 0, s = loots.size(); t < s; t++ )
    {
        int coord = m_gt->loots_coordinate() [t];
        int f = coord / ( MS * MS ), i = ( coord / MS ) % MS, j = coord % MS;
        res.maps[f][i][j] = loots[t];
    }
    const int floors = res.maps.size();
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < MS; i++ )
        {
            for ( int j = 0; j < MS; j++ )
            {
                int tile_sym = res.maps[f][i][j];
                switch ( tile_sym )
                {
                    case gtemplate::SYM_WALL:
                        res.maps[f][i][j] = mtstat::WALL;
                        break;
                    case gtemplate::SYM_DOOR:
                        eprintf ( "Unsubstituted tile DOOR at %d,%d,%d\n", f, i, j );
                        break;
                    case gtemplate::SYM_MOB:
                        eprintf ( "Unsubstituted tile MOB at %d,%d,%d\n", f, i, j );
                        break;
                    case gtemplate::SYM_LOOT:
                        eprintf ( "Unsubstituted tile LOOT at %d,%d,%d\n", f, i, j );
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return res;
}

void gsheet::set_mod_coord ( int coord )
{
    int node = m_gt->node_map() [coord];
    if ( node >= 0 )
    {
        mod_bools.set ( node );
    }
    else
    {
        eprintf ( "Unhandled coord invalid event, coord=%d\n", coord );
    }
}
