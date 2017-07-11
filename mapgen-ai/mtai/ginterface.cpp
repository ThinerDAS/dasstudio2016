/*
 * Auto generate by h2cppx
 */


#include "ginterface.h"
#include "graph.h"
#include "event_vector.h"
#include "eprintf.h"
#include "bitset_util.h"
#include <cassert>

//void generate_graph ( const mtstat& mts, graph& ret );

std::vector<event_vector> ginterface::generate_tileEV ( const mtstat& mts )
{
    const int SIV = 10; // some initial value
    std::vector<event_vector> tileEV = std::vector<event_vector> ( 256, event_vector ( EV_COUNT ) );
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
            tileEV[i][EV_NEGATIVE_ONETIME] |= graph_big::DOOR_FLAG; // to tell the graph processor that the mob is a door, namely not bombable.
        }
    }
    tileEV[gtemplate::SYM_WALL] = tileEV[mtstat::WALL];
    tileEV[gtemplate::SYM_DOOR] = tileEV[mtstat::YELDOOR];
    tileEV[gtemplate::SYM_MOB] = tileEV[mtstat::MOB];
    tileEV[gtemplate::SYM_LOOT] = tileEV[mtstat::YELKEY];
    return tileEV;
}

ginterface::ginterface ( const mtstat& mts, bool template_map )
{
    // Platform specific.
    // generate map and initial stat.
    m_mts = &mts;
    m_gt = NULL;
    graph_big tmp_map;
    graph_big big_map;
    const int MAPSIZE_SQUARE = mtstat::MAPSIZE * mtstat::MAPSIZE;
    //std::vector<event_vector> tileEV ( 256, event_vector ( EV_COUNT ) );
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
    m_initial.ev = event_vector ( EV_COUNT );
    for ( int j = 0; j < EV_COUNT; j++ )
    {
        if ( ev_mts_cp[j] )
        {
            m_initial.ev[j] = * ( ev_mts_cp[j] );
        }
    }
    tileEV = generate_tileEV ( mts );
    for ( int i = 0, fs = mts.maps.size(); i < fs; i++ )
    {
        tmp_map = fromRectMap ( mts.maps[i], tileEV );
        big_map += tmp_map;
    }
    for ( mtstat::portal_pair pp : mts.portals )
    {
        big_map.addEdge ( pp[0], pp[1] );
    }
    if ( template_map )
    {
        for ( int h = 0, s = big_map.vertices().size(); h < s; h++ )
        {
            int f = h / mtstat::MAPSIZE / mtstat::MAPSIZE, i = h / mtstat::MAPSIZE % mtstat::MAPSIZE, j = h % mtstat::MAPSIZE;
            int cur_tile = mts.maps[f][i][j];
            if ( cur_tile == gtemplate::SYM_LOOT && gtemplate::spec ( h ) )
            {
                big_map.updateVertex ( h, tileEV[mtstat::WHITECRY] );
            }
        }
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
    m_internal_graph = big_map.shrink();
    m_initial.taken.set ( 0 );
    m_initial.reach |= m_internal_graph.adjacent_vector() [0];
    m_initial.ev += m_internal_graph.vertices() [0];
}

ginterface::ginterface ( const gtemplate& gt ) : ginterface ( gt.initialmts(), true )
{
}

void ginterface::update_gsheet ( const gsheet& gs )
{
    m_internal_graph.update_gsheet_no_check ( gs, tileEV );
}

gstate ginterface::new_initial_state() const
{
    return m_initial;
}

bool ginterface::accessible ( const gstate& gstate, unit_event ue ) const
{
    // Sometimes repeatitively accessible will be called, which is not efficient.
    // if it returns false, it is guaranteed that the gs won't be modified.
    int node = ue.n;
#ifndef NDEBUG
    const category_mark& cm = m_internal_graph.cm();
    bool range_flag = false;
    switch ( ue.e )
    {
        case unit_event::EVENT_NORMAL:
            range_flag = ( node >= cm.door_offset && node < cm.door_end ) || ( node >= cm.mob_offset && node < cm.mob_end );
            break;
        case unit_event::EVENT_P:
            range_flag = ( node >= cm.wall_offset && node < cm.wall_end );
            break;
        case unit_event::EVENT_B:
            range_flag = ( node >= cm.mob_offset && node < cm.mob_end );
            break;
        case unit_event::EVENT_S:
            range_flag = true;
            break;
        case unit_event::EVENT_W:
            range_flag = ( node >= cm.w_offset && node < cm.w_end );
            break;
        default:
            break;
    }
    if ( !range_flag )
    {
        eprintf ( "Invalid ue because of an out-of-range error! ue={e=%d, n=%d, s=%d}\n", ue.e, ue.n, ue.s );
        return false;
    }
#endif
    switch ( ue.e )
    {
        case unit_event::EVENT_P:
            if ( gstate.ev[EV_Px] == 0 )
            {
                return false;
            }
            break;
        case unit_event::EVENT_B:
            if ( gstate.ev[EV_Bb] == 0 )
            {
                return false;
            }
            break;
        case unit_event::EVENT_S:
            return gstate.ev[EV_G] >= ue.n * 100;
            break;
        default:
            break;
    }
    if ( gstate.taken[node] )
    {
        return false;
    }
    // TODO: "any" function that take two parameter as begin and end.
    bool reachable = ( m_internal_graph.adjacent_vector() [node] & gstate.taken ).any();
    //graph::bools reach = nextto ( gstate.taken );
    //bool reachable2=reach[node];assert(reachable==reachable2);
    return reachable;
    //return false;
}

int dam ( const mtstat& mts, int brave_at, int brave_df, int brave_mf, int mon_ID )
{
    return mtstat::dam ( brave_at, brave_df, brave_mf, mts.enemies[mon_ID] );
}

void propagate_ev ( gstate& gs, const mtstat& mts, const event_vector& ev, bool bomb = false )
{
    if ( bomb )
    {
        gs.ev[EV_Bb]--;
    }
    else
    {
        if ( ev[EV_NEGATIVE_ONETIME] > 0 )
        {
            int tile_id = ev[EV_NEGATIVE_ONETIME] & ( ~graph_big::DOOR_FLAG );
            if ( tile_id < mtstat::MOB )
            {
                switch ( tile_id )
                {
                    case mtstat::YELDOOR:
                        if ( gs.ev[EV_YK] == 0 )
                        {
                            gs.ev[EV_SALT] += 128;
                        }
                        else
                        {
                            gs.ev[EV_YK]--;
                        }
                        break;
                    case mtstat::BLUEDOOR:
                        if ( gs.ev[EV_BK] == 0 )
                        {
                            gs.ev[EV_SALT] += 1024;
                        }
                        else
                        {
                            gs.ev[EV_BK]--;
                        }
                        break;
                    case mtstat::REDDOOR:
                        if ( gs.ev[EV_RK] == 0 )
                        {
                            gs.ev[EV_SALT] += 16384;
                        }
                        else
                        {
                            gs.ev[EV_RK]--;
                        }
                        break;
                    case mtstat::WALL:
                        if ( gs.ev[EV_Px] == 0 )
                        {
                            gs.ev[EV_SALT] += 16384;
                            eprintf ( "Pickaxe deficiency!\n" );
                        }
                        else
                        {
                            gs.ev[EV_Px]--;
                        }
                        break;
                    default:
                        eprintf ( "Unencountered case %d!\n", tile_id );
                        break;
                }
            }
            else
            {
                int mon_ID = ( tile_id - mtstat::MOB );
                int dmg = dam ( mts, gs.ev[EV_AT], gs.ev[EV_DF], gs.ev[EV_MF], mon_ID );
                int brave_hp = gs.ev[EV_HP];
                if ( brave_hp <= dmg )
                {
                    int salt = 1 + ( ( dmg - brave_hp ) / ginterface::SALT_FISH_UNIT >> gs.ev[EV_Tk] );
                    gs.ev[EV_SALT] += salt;
                    gs.ev[EV_HP] = brave_hp + ( salt << gs.ev[EV_Tk] ) * ginterface::SALT_FISH_UNIT - dmg;
                }
                else
                {
                    gs.ev[EV_HP] = brave_hp - dmg;
                }
                gs.ev[EV_G] += mts.enemies[mon_ID][3];
            }
        }
        gs.ev += ev;
        gs.ev[EV_NEGATIVE_ONETIME] = 0;
    }
}

void propagate_node ( gstate& gs, const mtstat& mts, const graph& grh, int node_id, bool bomb = false )
{
    // Take the node along with the easy nodes that are carried by it.
#ifndef NDEBUG
    if ( gs.taken[node_id] )
    {
        eprintf ( "The node %d has been taken before!\n", node_id );
        //return;
    }
#endif
    const category_mark& cm = grh.cm();
    //graph::bools prev = gs.taken;
    propagate_ev ( gs, mts, grh.vertices() [node_id], bomb );
    gs.taken.set ( node_id );
    const graph::bools& reach = grh.adjacent_vector() [node_id]; //& ( ~gs.taken );
    //gs.taken|=reach;//grh.nextto ( gs.taken );
    //gs.reach |= grh.adjacent_vector() [node_id];
    forbbe ( i, cm.easy_offset, cm.easy_end, reach )
    {
        if ( !gs.taken[i] )
        {
            //{
            //    gs.reach.reset(i);
            //}
            //else
            //{
            propagate_ev ( gs, mts, grh.vertices() [i] );
            gs.taken.set ( i );
        }
        //}
    }
    gs.reach |= grh.adjacent_vector() [node_id];
    gs.reach &= gs.taken;
}

bool ginterface::propagate ( gstate& gs, unit_event ue ) const
{
    // if it returns false, it is guaranteed that the gs won't be modified.
    //const category_mark& cm = m_internal_graph.cm();
    if ( !accessible ( gs, ue ) )
    {
        return false;
    }
    return propagate_no_check ( gs, ue );
}

bool ginterface::propagate_no_check ( gstate& gs, unit_event ue ) const
{
    // if it returns false, it is guaranteed that the gs won't be modified.
    //const category_mark& cm = m_internal_graph.cm();
    assert ( accessible ( gs, ue ) );
    switch ( ue.e )
    {
        case unit_event::EVENT_NORMAL:// same as P/B, first kill the node which may involve calculating damage; then loot all the adjacent easy nodes. don;t forget to mark the nodes as taken.
            propagate_node ( gs, *m_mts, m_internal_graph, ue.n );
            break;
        case unit_event::EVENT_P:
            propagate_node ( gs, *m_mts, m_internal_graph, ue.n );
            break;
        case unit_event::EVENT_B:
            propagate_node ( gs, *m_mts, m_internal_graph, ue.n, true );
            break;
        case unit_event::EVENT_S:// easy shopping.
        {
            ev_entry shop_ev_type = EV_RK;
            int unit = -1;
            switch ( ue.s )
            {
                case unit_event::SHOP_AT:
                    shop_ev_type = EV_AT;
                    unit = initialmts().unit_at;
                    break;
                case unit_event::SHOP_DF:
                    shop_ev_type = EV_DF;
                    unit = initialmts().unit_df;
                    break;
                case unit_event::SHOP_MF:
                    shop_ev_type = EV_MF;
                    unit = initialmts().unit_mf;
                    break;
                default:
                    eprintf ( "Unknown shop event %d\n", ue.s );
                    break;
            }
            gs.ev[shop_ev_type] += ( ue.n * unit ) << gs.ev[EV_Tk];
            gs.ev[EV_G] -= ue.n * 100;
            gs.ev[EV_SHOPTIMES] += ue.n;
        }
        break;
        case unit_event::EVENT_W:// loot w node and mark it as taken
        {
            ev_entry shop_ev_type = EV_RK;
            int unit = -1;
            switch ( ue.s )
            {
                case unit_event::SHOP_AT:
                    shop_ev_type = EV_AT;
                    unit = initialmts().unit_at;
                    break;
                case unit_event::SHOP_DF:
                    shop_ev_type = EV_DF;
                    unit = initialmts().unit_df;
                    break;
                case unit_event::SHOP_MF:
                    shop_ev_type = EV_MF;
                    unit = initialmts().unit_mf;
                    break;
                default:
                    eprintf ( "Unknown white event %d\n", ue.s );
                    break;
            }
            gs.ev[shop_ev_type] += unit * m_internal_graph.vertices() [ue.n][EV_W];
            gs.taken.set ( ue.n );
            gs.reach.reset ( ue.n );
        }
        break;
        default:
            break;
    }
    return true;
}

bool ginterface::propagate ( gstate& gs, const event_list& el ) const
{
    // if it returns false, it is guaranteed that the gs won't be modified.
    for ( const unit_event& ue : el )
    {
        if ( !propagate ( gs, ue ) )
        {
            return false;
        }
    }
    return true;
}
//#pragma GCC optimize ("O0")

int clear_map_region_builtin ( graph_big::bools& conquered, graph_big::bools& target, const std::vector<int>& portals_expanded, int orig_coord, std::string& route )
{
    if ( target.empty() )
    {
        return orig_coord;
    }
    const int MS = mtstat::MAPSIZE;
    graph_big::bools been_to;
    std::vector<int> bfs_coords ( 1, orig_coord );
    std::vector<int> come_by ( 1, 0 );
    std::vector<int> come_from ( 1, 0 );
    const char alphabet[] = "?kjhlg";
    been_to.insert ( orig_coord );
    // come_by: 0 - invalid, 1-4 - kjhl, 5 - g
    for ( int i = 0; i < ( int ) bfs_coords.size(); i++ )
    {
        int cur_coord = bfs_coords[i];
        int cur_x = cur_coord % MS, cur_y = ( cur_coord / MS ) % MS;
        for ( int cur_dir = 1; cur_dir <= 5; cur_dir++ )
        {
            int next_coord;
            bool can_go = false;
            switch ( cur_dir )
            {
                case 1:
                    if ( cur_y > 0 )
                    {
                        next_coord = cur_coord - MS;
                        can_go = true;
                    }
                    break;
                case 2:
                    if ( cur_y < MS - 1 )
                    {
                        next_coord = cur_coord + MS;
                        can_go = true;
                    }
                    break;
                case 3:
                    if ( cur_x > 0 )
                    {
                        next_coord = cur_coord - 1;
                        can_go = true;
                    }
                    break;
                case 4:
                    if ( cur_x < MS - 1 )
                    {
                        next_coord = cur_coord + 1;
                        can_go = true;
                    }
                    break;
                case 5:
                    if ( portals_expanded[cur_coord] > -1 )
                    {
                        next_coord = portals_expanded[cur_coord];
                        can_go = true;
                    }
                    break;
                default:
                    break;
            }
            if ( can_go )
            {
                if ( been_to.count ( next_coord ) )
                {
                    continue;
                }
                else if ( target.count ( next_coord ) )
                {
                    std::string tmp;
                    tmp.push_back ( alphabet[cur_dir] );
                    int j = i;
                    while ( j != 0 )
                    {
                        tmp.push_back ( alphabet[come_by[j]] );
                        j = come_from[j];
                    }
                    route.insert ( route.end(), tmp.rbegin(), tmp.rend() );
                    conquered.insert ( next_coord );
                    target.erase ( next_coord );
                    //eprintf ( "i=%d\n", i );
                    return clear_map_region_builtin ( conquered, target, portals_expanded, next_coord, route );
                }
                else if ( conquered.count ( next_coord ) )
                {
                    bfs_coords.push_back ( next_coord );
                    come_by.push_back ( cur_dir );
                    come_from.push_back ( i );
                    been_to.insert ( next_coord );
                }
            }
        }
    }
    eprintf ( "Maybe the map is not connected? The function did not find a way out.\n" );
    return -1;
}

int clear_map_region ( const graph_big::bools& conquered, const graph_big::bools& target, const mtstat& mts, int orig_coord, std::string& route )
{
    std::vector<int> portals_expanded ( mts.maps.size() *mtstat::MAPSIZE * mtstat::MAPSIZE, -1 );
    for ( mtstat::portal_pair pp : mts.portals )
    {
        portals_expanded[pp[0]] = pp[1];
        portals_expanded[pp[1]] = pp[0];
    }
    graph_big::bools c ( conquered ), t ( target );
    return clear_map_region_builtin ( c, t, portals_expanded, orig_coord, route );
}

void pseudo_take_node ( const graph& g, graph_big::bools& actual_taken, int node_id )
{
    // only untake the node itself, not the node spawned from the node
    graph_big::bools target = g.trace() [node_id];
    actual_taken |= target;
}

void pseudo_untake_node ( const graph& g, graph_big::bools& actual_taken, int node_id )
{
    // only untake the node itself, not the node spawned from the node
    graph_big::bools target = g.trace() [node_id];
    for ( int i : target )
    {
        actual_taken.erase ( i );
    }
}

int take_node ( const graph& g, const mtstat& mts, graph_big::bools& actual_taken, int orig_coord, int node_id, std::string& route )
{
    if ( orig_coord == -1 )
    {
        eprintf ( "The route to node #%d is blocked by an uncaught -1 event.\n", node_id );
        return -1;
    }
    graph_big::bools target = g.trace() [node_id];
    graph_big::bools t2 = target;
    //graph_big::bools actual_target = target;
    for ( int i : t2 )
    {
        const int ms = mtstat::MAPSIZE;
        if ( mts.maps[i / ( ms * ms )][ ( i / ms ) % ms][i % ms] == 0 )
        {
            target.erase ( i );
            actual_taken.insert ( i );
        }
    }
    int next_coord = clear_map_region ( actual_taken, target, mts, orig_coord, route );
    if ( next_coord == -1 )
    {
        eprintf ( "in the way to node #%d\n", node_id );
    }
    actual_taken |= target;
    return next_coord;
}

std::string ginterface::translate ( const event_list& el ) const
{
    // TODO when testing!
    const mtstat& mts = initialmts();
    const graph& g = m_internal_graph;
    int cur_coord = mts.brave_floor * mtstat::MAPSIZE * mtstat::MAPSIZE + mts.brave_y * mtstat::MAPSIZE + mts.brave_x;
    gstate initial = m_initial;
    graph::bools taken = initial.taken;
    graph_big::bools actual_taken_coords;
    const category_mark& cm = g.cm();
    std::string route;
    const char shop_choices[] = "adm";
    forb ( i, taken )
    {
        cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, i, route );
    }
    for ( unit_event ue : el )
    {
        switch ( ue.e )
        {
            case unit_event::EVENT_NORMAL:
            {
                cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, ue.n, route );
                taken.set ( ue.n );
                graph::bools reach = g.nextto ( taken );
                forbbe ( i, cm.w_offset, cm.w_end, reach )
                {
                    pseudo_take_node ( g, actual_taken_coords, i );
                }
                forbbe ( i, cm.easy_offset, cm.easy_end, reach )
                {
                    cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, i, route );
                    taken.set ( i );
                }
            }
            break;
            case unit_event::EVENT_P:
            {
                cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, ue.n, route );
                route.insert ( route.end() - 1, 'p' );
                taken.set ( ue.n );
                graph::bools reach = g.nextto ( taken );
                forbbe ( i, cm.w_offset, cm.w_end, reach )
                {
                    pseudo_take_node ( g, actual_taken_coords, i );
                }
                forbbe ( i, cm.easy_offset, cm.easy_end, reach )
                {
                    cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, i, route );
                    taken.set ( i );
                }
            }
            break;
            case unit_event::EVENT_B:
            {
                cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, ue.n, route );
                route.insert ( route.end() - 1, 'b' );
                taken.set ( ue.n );
                graph::bools reach = g.nextto ( taken );
                forbbe ( i, cm.w_offset, cm.w_end, reach )
                {
                    pseudo_take_node ( g, actual_taken_coords, i );
                }
                forbbe ( i, cm.easy_offset, cm.easy_end, reach )
                {
                    cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, i, route );
                    taken.set ( i );
                }
            }
            break;
            case unit_event::EVENT_S:// easy shopping.
            {
                route.push_back ( 's' );
                for ( int i = 0; i < ue.n; i++ )
                {
                    route.push_back ( shop_choices[ue.s] );
                }
            }
            break;
            case unit_event::EVENT_W:// loot w node and mark it as taken
            {
                //eprintf ( "White event %d!\n", ue.n );
                pseudo_untake_node ( g, actual_taken_coords, ue.n );
                cur_coord = take_node ( g, mts, actual_taken_coords, cur_coord, ue.n, route );
                route.push_back ( shop_choices[ue.s] );
            }
            break;
            default:
                break;
        }
    }
    return route;
}

long ginterface::score ( const gstate& gstate )
{
    long res = ( long ) gstate.ev[EV_SALT] * SALT_FISH_UNIT * ( -65536l );
    if ( gstate.ev[EV_FLAG] > 0 )
    {
        res += ( long ) gstate.ev[EV_HP];
        res += 0x100000000;
    }
    return res;
}

graph::bools ginterface::nextto ( const graph::bools& taken ) const
{
    return m_internal_graph.nextto ( taken );
}

