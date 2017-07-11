/*
 * Auto generate by h2cppx
 */


#include "gtemplate.h"
#include "mtstat.h"

gtemplate::gtemplate ( const mtstat& mts ) : m_initial_mts ( &mts ), m_doors_coordinate ( 0 ), m_mobs_coordinate ( 0 ), m_loots_coordinate ( 0 )
{
    const int floors = mts.maps.size(), MS = mtstat::MAPSIZE;
    for ( int f = 0; f < floors; f++ )
    {
        for ( int i = 0; i < MS; i++ )
        {
            for ( int j = 0; j < MS; j++ )
            {
                int coord = f * ( MS * MS ) + i * MS + j;
                int tile_sym = mts.maps[f][i][j];
                switch ( tile_sym )
                {
                    case SYM_DOOR:
                        m_doors_coordinate.push_back ( coord );
                        break;
                    case SYM_MOB:
                        m_mobs_coordinate.push_back ( coord );
                        break;
                    case SYM_LOOT:
                        m_loots_coordinate.push_back ( coord );
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void gtemplate::set_node_map ( const graph& g )
{
    m_node_map.assign ( m_initial_mts->maps.size() *mtstat::MAPSIZE * mtstat::MAPSIZE, -1 );
    for ( int i = 0, s = g.vertices().size(); i < s; i++ )
    {
        for ( int j : g.trace() [i] )
        {
            m_node_map[j] = i;
        }
    }
}

