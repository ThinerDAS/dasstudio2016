/*
 * Auto generate by h2cppx
 */

//#define DEEP_GRAPH_DEBUG

#define NDEBUG

#ifdef DEEP_GRAPH_DEBUG
#undef NDEBUG
#endif
#include "gsheet.h"
#include "graph.h"
#include "bitset_util.h"
#include "event_vector.h"
#include "eprintf.h"
#include "mtstat.h" // used by pack() which is closely related to the infrastructure of the mt.

#include <cassert>
#include <algorithm>

graph_big::bools operator& ( const graph_big::bools& b1, const graph_big::bools& b2 )
{
    graph_big::bools ret;
    std::set_intersection ( b1.begin(), b1.end(), b2.begin(), b2.end(), std::inserter ( ret, ret.begin() ) );
    return ret;
}

graph_big::bools operator| ( const graph_big::bools& b1, const graph_big::bools& b2 )
{
    graph_big::bools ret;
    std::set_union ( b1.begin(), b1.end(), b2.begin(), b2.end(), std::inserter ( ret, ret.begin() ) );
    return ret;
}

graph_big::bools& operator&= ( graph_big::bools& b1, const graph_big::bools& b2 )
{
    return b1 = b1 & b2;
}

graph_big::bools& operator|= ( graph_big::bools& b1, const graph_big::bools& b2 )
{
    return b1 = b1 | b2;
}

graph_big::bools& operator<<= ( graph_big::bools& b1, int d )
{
    //assert(d>=0) //XXX
    if ( d == 0 )
    {
        return b1;
    }
    if ( d < 0 )
    {
        return b1 >>= ( -d );
    }
    for ( graph_big::bools::reverse_iterator i = b1.rbegin(); i != b1.rend(); ++i )
    {
        const_cast<int&> ( *i ) += d;
    }
    return b1;
}

graph_big::bools& operator>>= ( graph_big::bools& b1, int d )
{
    //assert(d>=0) //XXX
    if ( d == 0 )
    {
        return b1;
    }
    if ( d < 0 )
    {
        return b1 >>= ( -d );
    }
    for ( graph_big::bools::iterator i = b1.begin(); i != b1.end(); ++i )
    {
        const_cast<int&> ( *i ) -= d;
    }
    return b1;
}

int graph_big::addVertex ( event_vector ev )
{
    int index = m_vertices.size();
    m_vertices.push_back ( ev );
    m_adjacent_vector.resize ( index + 1 );
    m_trace.resize ( index + 1 );
    //m_trace[index].set ( index );
    m_trace[index].insert ( index );
    eprintf ( "Added vertex %d\n", index );
    return index;
}

void graph_big::updateVertex ( int v, event_vector ev )
{
    m_vertices[v] = ev;
}

void graph_big::addEdge ( int v1, int v2 )
{
    if ( v1 == v2 )
    {
        return;
    }
    //m_adjacent_vector[v1].set ( v2 );
    //m_adjacent_vector[v2].set ( v1 );
    m_adjacent_vector[v1].insert ( v2 );
    m_adjacent_vector[v2].insert ( v1 );
    eprintf ( "Added edge (%d, %d)\n", v1, v2 );
}

void graph_big::removeVertex ( int v )
{
    bools& gb = m_adjacent_vector[v];
    for ( int i : gb )
    {
        if ( i == v )
        {
            continue;
        }
        m_adjacent_vector[i].erase ( v );
    }
    m_vertices[v] &= 0;
    gb.clear();
    m_trace[v].clear();
    eprintf ( "Removed vertex %d\n", v );
}

void graph_big::removeEdge ( int v1, int v2 )
{
    m_adjacent_vector[v1].erase ( v2 );
    m_adjacent_vector[v2].erase ( v1 );
    eprintf ( "Removed edge (%d, %d)\n", v1, v2 );
}

void graph_big::contractVertex ( int v1, int v2 )
{
    if ( v1 == v2 )
    {
        return;
    }
    m_vertices[v1] += m_vertices[v2];
    bools& gb = m_adjacent_vector[v2];
    for ( int i : gb )
    {
        if ( i == v2 )
        {
            continue;
        }
        if ( i != v1 )
        {
            addEdge ( v1, i );
        }
        m_adjacent_vector[i].erase ( v2 );
    }
    m_vertices[v2] &= 0;
    gb.clear();
    m_trace[v1] |= m_trace[v2];
    m_trace[v2].clear();
    eprintf ( "Contracted vertex %d, %d\n", v1, v2 );
}

void graph_big::swapVertex ( int v1, int v2 )
{
    if ( v1 == v2 )
    {
        return;
    }
    std::swap ( m_vertices[v1], m_vertices[v2] );
    std::swap ( m_adjacent_vector[v1],
                m_adjacent_vector[v2] );
    std::swap ( m_trace[v1], m_trace[v2] );
    for ( int i = 0, s = m_vertices.size(); i < s; i++ )
    {
        bools& gb = m_adjacent_vector[i];
        int c1 = gb.count ( v1 ), c2 = gb.count ( v2 );
        if ( c1 != c2 )
        {
            if ( c1 )
            {
                gb.erase ( v1 );
                gb.insert ( v2 );
            }
            else
            {
                gb.erase ( v2 );
                gb.insert ( v1 );
            }
        }
    }
    eprintf ( "Swapped vertex %d, %d\n", v1, v2 );
}

bool graph_big::branchVertex ( int v )
{
    bools& gb = m_adjacent_vector[v];
    bool ret = false;
    for ( int i : gb )
    {
        if ( i == v )
        {
            continue;
        }
        int prev_s = 0;
        if ( !ret )
        {
            prev_s = m_adjacent_vector[i].size();
        }
        m_adjacent_vector[i] |= gb;
        m_adjacent_vector[i].erase ( i );
        if ( !ret )
        {
            if ( ( int ) m_adjacent_vector[i].size() != prev_s )
            {
                ret = true;
            }
        }
    }
    eprintf ( "Branched vertex %d, %s.\n", v, ret ? "effective" : "already branched well" );
    return ret;
}

graph_big& graph_big::operator+= ( const graph_big& g )
{
    int orig_len = m_vertices.size();
    m_vertices.insert ( m_vertices.end(), g.m_vertices.begin(), g.m_vertices.end() );
    m_adjacent_vector.insert ( m_adjacent_vector.end(), g.m_adjacent_vector.begin(), g.m_adjacent_vector.end() );
    m_trace.insert ( m_trace.end(), g.m_trace.begin(), g.m_trace.end() );
    int cur_len = m_vertices.size();
    for ( int i = orig_len; i < cur_len; i++ )
    {
        m_adjacent_vector[i] <<= orig_len;
        m_trace[i] <<= orig_len;
    }
    return *this;
}


const category_mark& graph_big::pack ( int entry_v )
{
    // There are two stages: Reduce, and Compress.
    // Reduce: to contract the easy nodes and make some modifications on nodes based on its intrinsics.
    // Namely: Contract easy nodes, make w nodes become branch to easy nodes
    // and make every node branch to non-easy nodes.
    int s = m_vertices.size();
    {
        bool reduce_flag = false;
        do
        {
            eprintf ( "We have come back to the very beginning.\n" );
            //fflush(stderr);
            reduce_flag = false;
            // Before everything: branch all the ws beforehands.
            for ( int i = 0; i < s; i++ )
            {
                if ( m_adjacent_vector[i].empty() )
                {
                    continue;
                }
                if ( easy_take ( m_vertices[i] ) && m_vertices[i][EV_W] != 0 )
                {
                    if ( branchVertex ( i ) ) // In case the w are enclosed in layers and single branching won't completely make a clear way.
                    {
                        reduce_flag = true;
                    }
                }
            }
            if ( reduce_flag )
            {
                continue;
            }
            eprintf ( "First level of reduction.\n" );
            // Stage 1: easy node contraction
            for ( int i = 0; i < s; i++ )
            {
                if ( m_adjacent_vector[i].empty() )
                {
                    continue;
                }
                if ( easy_take ( m_vertices[i] ) && m_vertices[i][EV_W] == 0 )
                {
                    bools gb = m_adjacent_vector[i]; // when contracting vertices this may be modified!
                    for ( int j : gb )
                    {
                        if ( m_adjacent_vector[j].empty() )
                        {
                            continue;// This case is impossible unless something is wrong.
                        }
                        if ( easy_take ( m_vertices[j] ) && m_vertices[j][EV_W] == 0 )
                        {
                            contractVertex ( i, j );
                            reduce_flag = true;
                        }
                    }
                }
            }
            if ( reduce_flag )
            {
                continue;
            }
            eprintf ( "Second level of reduction.\n" );
            // Stage 2: branch easy nodes.
            for ( int i = 0; i < s; i++ )
            {
                if ( m_adjacent_vector[i].empty() )
                {
                    continue;
                }
                if ( easy_take ( m_vertices[i] ) ) // && m_vertices[i][EV_W] != 0 ) // which is wrong XXX
                {
                    if ( branchVertex ( i ) )
                    {
                        reduce_flag = true;
                    }
                }
            }
            if ( reduce_flag )
            {
                continue;
            }
            eprintf ( "Third level of reduction. (dijkstra)\n" );
            // Stage 3: short circuiting
            {
                for ( int i = 0; i < s; i++ )
                {
                    if ( m_adjacent_vector[i].empty() )
                    {
                        continue;
                    }
                    if ( !easy_take ( m_vertices[i] ) && m_vertices[i][EV_NEGATIVE_ONETIME] < mtstat::SOLIDWALL )
                    {
                        int ev_negative_fingerprint = m_vertices[i][EV_NEGATIVE_ONETIME];
                        bools& gbi = m_adjacent_vector[i];
                        bool sc_flag = true;
                        for ( int j : gbi ) for ( bools::iterator ik = gbi.lower_bound ( j + 1 ); ik != gbi.end(); ++ik )
                            {
                                int k = *ik;
                                // if j and k have no other way that can short circuit i, then the necessity of i has been proved in this loop, and it just needs to break by goto. (The for is still a two-dimensional for, so you can not break directly.)
                                // By the way, as the infrastructure take every negative event as a seperate tile event, there is no need for us to do the addition of the event vectors. We can ignore the positive part of the event, and for the negative tile event, we only need to check whether it is the same event as the event i. If it is not the same, then it is incomparable, thus unable to be used for short circuiting.
                                // use dijkstra
                                bool sc_smallflag = false;
                                std::set<std::pair<int, int>> active_vs;
                                active_vs.insert ( {0, j} );
                                std::vector<int> min_dist ( m_vertices.size(), 0x7fffffff );
                                min_dist[j] = 0;
                                while ( !active_vs.empty() )
                                {
                                    int where = active_vs.cbegin()->second;
                                    if ( min_dist[k] <= 1 )
                                    {
                                        sc_smallflag = true;
                                        break;
                                    }
                                    if ( where == k )
                                    {
                                        break;
                                    }
                                    active_vs.erase ( active_vs.begin() );
                                    int min_dist_where = min_dist[where];
                                    if ( min_dist_where > 1 )
                                    {
                                        break;
                                    }
                                    for ( auto ed : m_adjacent_vector[where] )
                                    {
                                        int len = 0x7fffffff;
                                        if ( easy_take ( m_vertices[ed] ) )
                                        {
                                            len = 0;
                                        }
                                        else if ( m_vertices[ed][EV_NEGATIVE_ONETIME] == ev_negative_fingerprint )
                                        {
                                            len = 1;
                                        }
                                        else
                                        {
                                            continue;
                                        }
                                        if ( min_dist[ed] > min_dist_where + len )
                                        {
                                            active_vs.erase ( {min_dist[ed], ed} );
                                            min_dist[ed] = min_dist_where + len;
                                            active_vs.insert ( {min_dist[ed], ed} );
                                        }
                                    }
                                }
                                /*std::vector<bool> bfs_total_negvalue ( 1 );
                                std::vector<int> bfs_endv ( 1 );
                                std::vector<bools> been_to ( 1 );
                                bfs_endv[0] = j;
                                been_to[0].insert ( j );
                                for ( int l = 0; l < ( int ) bfs_endv.size(); l++ )
                                {
                                    bools gbl = nextto ( been_to[l] );
                                    if ( gbl.count ( k ) )
                                    {
                                        sc_smallflag = true;
                                        break;
                                    }
                                    if ( bfs_total_negvalue[l] )
                                    {
                                        for ( int m : gbl )
                                        {
                                            if ( m == i )
                                            {
                                                continue;
                                            }
                                            if ( easy_take ( m_vertices[m] ) )
                                            {
                                                bfs_total_negvalue.push_back ( true );
                                                bfs_endv.push_back ( m );
                                                been_to.push_back ( been_to[l] );
                                                been_to.back().insert ( m );
                                            }
                                        }
                                    }
                                    else
                                    {
                                        for ( int m : gbl )
                                        {
                                            if ( m == i )
                                            {
                                                continue;
                                            }
                                            if ( easy_take ( m_vertices[m] ) )
                                            {
                                                bfs_total_negvalue.push_back ( false );
                                                bfs_endv.push_back ( m );
                                                been_to.push_back ( been_to[l] );
                                                been_to.back().insert ( m );
                                            }
                                            else if ( m_vertices[m][EV_NEGATIVE_ONETIME] == ev_negative_fingerprint )
                                            {
                                                bfs_total_negvalue.push_back ( true );
                                                bfs_endv.push_back ( m );
                                                been_to.push_back ( been_to[l] );
                                                been_to.back().insert ( m );
                                            }
                                        }
                                    }
                                }*/
                                if ( !sc_smallflag )
                                {
                                    sc_flag = false;
                                    goto LOOP_END;
                                }
                            }
LOOP_END:
                        if ( sc_flag )
                        {
                            removeVertex ( i );
                            reduce_flag = true;
                        }
                    }
                }
            }
            if ( reduce_flag )
            {
                continue;
            }
        }
        while ( reduce_flag );
    }
    //Here comes the second part: compressing.
    //0 is the entry.
    category_mark& cm = m_cm;
    {
        std::vector<int> spawn_vs, easy_vs, w_vs, door_vs, mob_vs, wall_vs, total_vs;
        for ( int i = 0; i < s; i++ )
        {
            if ( m_adjacent_vector[i].empty() )
            {
                continue;
            }
            else if ( m_trace[i].count ( entry_v ) )
            {
                spawn_vs.push_back ( i );
            }
            else if ( easy_take ( m_vertices[i] ) )
            {
                if ( m_vertices[i][EV_W] == 0 )
                {
                    easy_vs.push_back ( i );
                }
                else
                {
                    w_vs.push_back ( i );
                }
            }
            else
            {
                int tile_id = m_vertices[i][EV_NEGATIVE_ONETIME];
                if ( tile_id == mtstat::WALL )
                {
                    wall_vs.push_back ( i );
                }
                else if ( tile_id < mtstat::WALL )
                {
                    door_vs.push_back ( i );
                }
                else if ( m_vertices[i][EV_NEGATIVE_ONETIME]&DOOR_FLAG )
                {
                    door_vs.push_back ( i );
                }
                else
                {
                    mob_vs.push_back ( i );
                }
            }
        }
        cm.easy_offset = 0;
        total_vs.insert ( total_vs.end(), spawn_vs.begin(), spawn_vs.end() );
        total_vs.insert ( total_vs.end(), easy_vs.begin(), easy_vs.end() );
        cm.easy_end = cm.w_offset = total_vs.size();
        total_vs.insert ( total_vs.end(), w_vs.begin(), w_vs.end() );
        cm.w_end = cm.noneasy_offset = cm.door_offset = total_vs.size();
        total_vs.insert ( total_vs.end(), door_vs.begin(), door_vs.end() );
        cm.door_end = cm.mob_offset = total_vs.size();
        total_vs.insert ( total_vs.end(), mob_vs.begin(), mob_vs.end() );
        cm.mob_end = cm.wall_offset = total_vs.size();
        total_vs.insert ( total_vs.end(), wall_vs.begin(), wall_vs.end() );
        cm.wall_end = cm.noneasy_end = total_vs.size();
        // Based on the total vs, we rearrange the nodes.
        std::vector<int> total_vs_r;
        int ts = total_vs.size();
        int orig_size = m_vertices.size();
        total_vs_r.resize ( orig_size, -1 );
        for ( int i = 0; i < ts; i++ )
        {
            total_vs_r[total_vs[i]] = i;
        }
        std::vector<bool> total_vbool ( orig_size, false );
        {
            // Copied from last session of eg, may be buggy.
            for ( int i = 0; i < orig_size; i++ )
            {
                if ( !total_vbool[i] )
                {
                    int seed = i;
                    while ( total_vs_r[seed] != -1 && !total_vbool[seed] )
                    {
                        if ( total_vs_r[seed] == i )
                        {
                            total_vbool[seed] = true;
                            break;
                        }
                        swapVertex ( i, total_vs_r[seed] );
                        total_vbool[seed] = true;
                        seed = total_vs_r[seed];
                    }
                }
            }
        }
        m_vertices.resize ( ts );
        m_adjacent_vector.resize ( ts );
        m_trace.resize ( ts );
        // And then, everything is done.
    }
    return m_cm;
}

graph_big::bools graph_big::nextto ( const bools& taken ) const
{
    bools ret;
    //ret.reset();
    for ( int i : taken )
    {
        ret |= m_adjacent_vector[i];
    }
    for ( int i : taken )
    {
        ret.erase ( i );
    }
    return ret;
}

graph::bools graph::nextto ( const bools& taken ) const
{
    bools ret;
    //ret.reset();
    forb ( i, taken )
    {
        ret |= m_adjacent_vector[i];
    }
    ret &= ( ~taken );
    return ret;
}

graph graph_big::shrink() const
{
    graph ret;
    eprintf ( "The shrinked graph has %d nodes, while the upper limit is %d.\n", ( int ) m_vertices.size(), ( int ) graph::MAX_GRAPH_NODES );
    int s = m_vertices.size();
    ret.m_vertices = m_vertices;
    ret.m_adjacent_vector.resize ( s );
    ret.m_trace = m_trace;
    ret.m_cm = m_cm;
    for ( int i = 0; i < s; i++ )
    {
        //ret.m_adjacent_vector.push_back ( graph::bools ( gb.to_string().substr ( graph_big::MAX_GRAPH_NODES - graph::MAX_GRAPH_NODES, graph::MAX_GRAPH_NODES ) ) );
        for ( int j : m_adjacent_vector[i] )
        {
            ret.m_adjacent_vector[i].set ( j );
        }
    }
    return ret;
}

void graph::update_gsheet_no_check ( const gsheet& gs, std::vector<event_vector>& tileEV )
{
    mtstat mts = gs.output();
    forb ( i, gs.mod_bools )
    {
        event_vector new_vec ( EV_COUNT );
        for ( int j : m_trace[i] )
        {
            const int MS = mtstat::MAPSIZE;
            int f = j / MS / MS, y = ( j / MS ) % MS, x = j % MS;
            new_vec += tileEV[mts.maps[f][y][x]];
        }
        m_vertices[i] = new_vec;
    }
    gs.mod_bools.reset();
}
