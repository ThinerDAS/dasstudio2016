/*
 * Auto generate by h2cppx
 */

// TODO: Maybe we need some parallel calculation here!
//
// TODO: If it turn out to be here who account for a lot of time, be sure to check first the computational complexity. Some algorithm here are definitely not efficient in the meaning of computational complexity.
//
// TODO: fully test the functions! such as the shopper.
//
// TODO: ending node
//
// TODO: when optimizing note that even if the ratio of the time that each part occupies are balanced, there are still something to be optimize. Inside a module some may cause the speed deficiency of the module as a whole. The redundant operations are really much!
//
// TODO: Capure the Flag! Flag is the goal!

#define PROFILE

#include "ai_core.h"
#include "groute.h"
#include "gstate_ex.h"
#include "gstate_ex.h"
#include "ginterface.h"
#include "event_vector.h"
#include "event_list.h"
#include "bitset_util.h"
#include "eprintf.h"

#include "vector_pool.h"

#include <vector>
#include <array>
#include <cstdlib> // rand
#include <cassert>
#include <cstring>
#include <bitset>
#include <type_traits>

#include <map>
#include <set>

#include <unistd.h> // alarm
#include <signal.h>

#include <algorithm> // std::random_shuffle, std::fill

const int S_B_NUM = 100000000; // Some big number

const int MIN_MASK_N = 16; // when node numbers are less than 16 the masks are not used, instead related calculation is bruteforced.

//const auto empty_lambda = [] ( auto& ) {};
const auto empty_lambda = [] ( auto... ) {};

std::vector<std::vector<int>> frequency;

bool interrupted = false;

static inline int min ( int a, int b )
{
    return a > b ? b : a;
}

static inline int max ( int a, int b )
{
    return a > b ? a : b;
}

static inline int log2_cap ( unsigned long i )
{
    return 8 * sizeof ( unsigned long ) - __builtin_clzl ( i );
}

static inline int log2_for_2_power ( unsigned long i )
{
    return __builtin_ctzl ( i );
}

static long partial_score ( const event_vector& ev )
{
    long t = ev[EV_Tk];
    return ( long ) ev[EV_HP] + ( long ) ev[EV_AT] * 512 + ( long ) ev[EV_DF] * 512 + ( long ) ev[EV_MF] * 256 + ( long ) ev[EV_YK] * 1048576 + ( long ) ev[EV_BK] * 1048576 * 2 + ( long ) ev[EV_RK] * 1048576 * 16 + ( ( long ) ev[EV_Px] * 1048576 << t ) + ( long ) ev[EV_FLAG] * 0x100000000 + ( ( long ) ev[EV_Bb] * 1048576 << t ) - ( long ) ev[EV_SALT] * ginterface::SALT_FISH_UNIT * 65536;
}

static long progression_score ( const event_vector& ev )
{
    long t = ev[EV_Tk];
    return ( long ) ev[EV_HP] + ( long ) ev[EV_AT] * 16777216 + ( long ) ev[EV_DF] * 16777216 + ( long ) ev[EV_MF] * 8388608 + ( long ) ev[EV_YK] * 65536 + ( long ) ev[EV_BK] * 131072 + ( long ) ev[EV_RK] * 262144 + ( ( long ) ev[EV_Px] * 262144 << t ) + ( long ) ev[EV_FLAG] * 0x100000000 + ( ( long ) ev[EV_Bb] * 64 << t ) - ( long ) ev[EV_SALT] * ginterface::SALT_FISH_UNIT * 32;
}
// psi stands for partial score integrated

unit_event n2ue ( const ginterface& gi, int node_id );

int emu_shop_progress ( const ginterface& gi, const gstate_ex& gs, const groute& gr );

template<typename F_t = decltype ( empty_lambda ) >
void finish_shop ( const ginterface& gi, gstate_ex& gs, const groute& gr, F_t f = empty_lambda, bool single = false );

template<typename F_t = decltype ( empty_lambda ) >
bool propagate_nodeonly_builtin ( const ginterface& gi, gstate_ex& gs, int node_id, F_t f = empty_lambda );

template<typename F_t = decltype ( empty_lambda ) >
void propagate_one_builtin ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr, F_t f = empty_lambda );

template<typename F1_t = decltype ( empty_lambda ), typename F2_t = decltype ( empty_lambda ), typename F3_t = decltype ( empty_lambda ), typename F4_t = decltype ( empty_lambda ) >
void peruse_builtin_general ( const ginterface& gi, const gstate_ex& gs, const groute& gr, F1_t f_ev = empty_lambda, F2_t f_gs = empty_lambda, F3_t f_gs_end = empty_lambda , F4_t f_gs_err = empty_lambda );

groute opt_from_middle_builtin ( const ginterface& gi, const gstate_ex& gs, ai_level lv );

long bfs_score ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int level = 8 );

void fast_psi_score ( const ginterface& gi, const gstate_ex& gs, groute& gr );

long fast_psi ( const ginterface& gi, const gstate_ex& gs, const groute& gr );

long fast_score ( const ginterface& gi, const gstate_ex& gs, const groute& gr );

gstate fast_final_state ( const ginterface& gi, const gstate_ex& gs, const groute& gr );

ai_return_t translate ( const ginterface& gi, const gstate_ex& gs, const groute& gr );

bool heuristic_smoothen ( const ginterface& gi, const gstate_ex& gs, const std::vector<int>& ns, groute& gr, int level = 4 );

groute topovalid_heuristic_randfill ( const ginterface& gi, const gstate_ex& gs, const std::vector<int>& ns, int ts, int level = 4 );

int choose_best_node ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int level = 4, const std::set<int> s = {} );

groute topovalid_randfill ( const ginterface& gi, const gstate_ex& gs, const std::vector<int>& ns, int ts );

bool topo_validate ( const ginterface& gi, const gstate_ex& gs, groute& gr, bool optimized = false );

bool accessible ( const ginterface& gi, const gstate_ex& gs, int node_id );

bool propagate_one_no_profiling_rev ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr );

bool propagate_one_no_profiling ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr );

event_list propagate_one ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr );

void intelligent_shopper ( int goal, const int* in, int* out );

typedef std::vector<int> mask_t;

std::vector<                // for different n
std::vector<            // for different level of optimizing
std::vector<mask_t>
>
> mask_collection ( graph::MAX_GRAPH_NODES );

std::vector<std::vector<mask_t>>& node_opt_chain ( int n );

std::vector<std::vector<int>> perm_opt_order_collection ( graph::MAX_GRAPH_NODES );

// first -> k1, second -> j

const std::vector<int>& perm_opt_order ( int n );

groute concat ( const ginterface& gi, const gstate_ex& gs, const groute& templ, const groute& gr1, const groute& gr2, int gr1_len );

void graph_list_complete ( const std::vector<int>& orig_s, std::vector<int>& cand_s );

bool opt_toolset ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv );

// Higher level of following opt (higher than med-level) will spawn
// thread of opt_from_middle that opt for them.
bool opt_on_node_mask ( const ginterface& gi, const gstate_ex& gs, groute& gr, const mask_t& mask );

bool opt_on_node ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv ); //, const std::vector<std::vector<std::vector<int>>>& mask );

bool opt_on_sw ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv );

bool opt_on_t ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv );

// helper struct
/*
enum progress_index
{
    //PI_P,
    //PI_B,
    PI_SW,
    PI_T,
    PI_COUNT
};

typedef std::vector<std::array<int, PI_COUNT>> groute_progress;

std::pair<groute_progress, gstate_ex> generate_progress_builtin ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int node_range );

groute_progress generate_progress ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int node_range = -1 );
*/

// As a AI greater equal mid-level, it should do normal optimization,
// then, for every branch, it will force a retry that guaranteedly
// modifies this step and retry an optimization of a lower level in
// a recursive style: that is, to call itself with a lower level and
// one more step given. After all retrial has been applied,
// If for any branch, this cast succeeded to make the result better,
// it returns to start and re-optimize itself; otherwise, it returns.
//
// For levels that are lower than mid-level, proof is not available.
//
// For sub-levels, some small optimization strategy varies, like the
// modification methods. The higher the level is, the more aggressive
// and complete the methods will be. But it is guaranteed that Mid-0
// will have no less optimization trial than Easy-4.

void op_freq ( const std::vector<std::vector<mask_t>>& mask_bundle, bool full_data = false )
{
    eprintf ( "frequency profile data:\n" );
    for ( int t = 0; t < 5; t += 2 )
    {
        int sum = 0;
        for ( int i = 0, s = frequency[t].size(); i < s; i++ )
        {
            if ( full_data
                    && false
               )
            {
                fprintf ( stderr, "Mask(%d,%4d):", t, i );
                for ( int j = 0, s = mask_bundle[t][i].size(); j < s; j++ )
                {
                    fprintf ( stderr, "%4d ", mask_bundle[t][i][j] );
                }
                fprintf ( stderr, "; times=%4d\n", frequency[t][i] );
            }
            sum += frequency[t][i];
        }
        fprintf ( stderr, "%d class: %d\n", t, sum );
    }
}

ai_return_t opt_from_beginning ( const ginterface& gi, ai_level lv )
{
    gstate gs ( gi.new_initial_state() );
    return opt_from_middle ( gi, gs, lv );
}

ai_return_t opt_from_middle ( const ginterface& gi, const gstate& gs, ai_level lv )
{
    groute max_r = opt_from_middle_builtin ( gi, gs, lv );
    return translate ( gi, gs, max_r );
}


void output ( const gstate_ex& gs )
{
    eprintf ( "Main value: HP:%8d, Salt:%8d, Flag:%2d, ADM:%4d,%4d,%4d, Partial:%ld, Gscore:%ld\n", gs.ev[EV_HP], gs.ev[EV_SALT], gs.ev[EV_FLAG], gs.ev[EV_AT], gs.ev[EV_DF], gs.ev[EV_MF], partial_score ( gs.ev ), ginterface::score ( gs ) );
}

groute opt_from_middle_builtin ( const ginterface& gi, const gstate_ex& gs, ai_level lv )
{
    groute max_r;
    const graph& i_graph = gi.int_graph();
    const std::vector<event_vector>& vertices = i_graph.vertices();
    const category_mark& cm ( i_graph.cm() );
    graph::bools negtaken = ~ ( gs.taken );
    // To prepare all the unused nodes in order to optimize their order.
    std::vector<int> unused_nodes;
    //std::vector<int> unused_p;
    //std::vector<int> unused_b;
    forbbe ( i, cm.noneasy_offset, cm.noneasy_end, negtaken )
    {
        unused_nodes.push_back ( i );
        if ( ( i >= cm.mob_offset ) && ( i < cm.mob_end ) )
        {
            unused_nodes.push_back ( i | groute::B_MARK );
        }
    }
    unused_nodes.push_back ( -1 );
    // To sum up the loots in the nodes that has not been taken. Most useful are p/b counts.
    event_vector sumev ( gs.ev );
    forbbe ( i, cm.easy_offset, cm.easy_end, negtaken )
    {
        sumev += vertices[i];
    }
    // Loop times
    int loop_times = 0;
    if ( lv >= AI_GOD )
    {
        loop_times = 256;
    }
    else if ( lv >= AI_EXTR )
    {
        loop_times = 64;
    }
    else if ( lv >= AI_EXP )
    {
        loop_times = 32;
    }
    else if ( lv >= AI_AD )
    {
        loop_times = 16;
    }
    else if ( lv >= AI_MED )
    {
        loop_times = 8;
    }
    else if ( lv >= AI_EASY )
    {
        loop_times = 2;
    }
#ifdef PROFILE
    //loop_times *= 8;
#endif
    // Go to the main loop
    // TODO: if either of the choices are less than 16, then test all of it out is the fastest!
    max_r = topovalid_heuristic_randfill ( gi, gs, unused_nodes, sumev[EV_Tk] );
    for ( int t = 0; t < loop_times; t++ )
    {
        if ( interrupted )
        {
            break;
        }
        groute gr;
        gr = topovalid_heuristic_randfill ( gi, gs, unused_nodes, sumev[EV_Tk] );
        //break;
        /*
        if ( t == 0 )
        {
            gr.node_route.clear();
            gr.node_route.assign ( {84, 77, 75
                                    , 83
                                    , 71
                                    , 88, 76
                                    , 73, 70, 69, 80, 82, 85, 78, 79
                                    , 115, 114, 109, 110, 112, 113, 68, 118, 105, 104, 99, 95, 111, 106, 108, 65, 101, 74, 94, 59, 91, 93, 60, 92, 61, 90, -1//, 73, 70, 69, 80, 82, 85, 78, 79

                                   } );
            int nrsize = gr.node_route.size();
            gstate fin;
            graph_list_complete ( max_r.node_route, gr.node_route );
            //topo_validate ( gi, gs, gr );
            //fast_psi_score ( gi, gs, gr );
            //eprintf ( "Shiny route score: val=%ld", gr.val );
            //gstate fin = fast_final_state ( gi, gs, gr );
            //output ( fin );
            opt_on_node_mask ( gi, gs, gr, {nrsize - 8, nrsize - 7, nrsize - 6, nrsize - 5, nrsize - 4, nrsize - 3, nrsize - 2, nrsize - 1} );
            eprintf ( "Shiny route score afterwards: val=%ld", gr.val );
            fin = fast_final_state ( gi, gs, gr );
            output ( fin );
            break;
        }*/
        //int can_be_opt = 0;
        //do
        //{
#ifdef PROFILE
        node_opt_chain ( unused_nodes.size() );
        std::vector<std::vector<int>> orig_freq = frequency;
        while ( opt_on_node ( gi, gs, gr, AI_EASY ) ) {} ;
        frequency = orig_freq;
#endif
        opt_toolset ( gi, gs, gr, lv );        //}
        //while ( can_be_opt > 0 );
        //now can_be_opt is zero definitely.
        eprintf ( "gr.val=%ld, max_r.val=%ld; real_score:%ld;%ld", gr.val, max_r.val, fast_score ( gi, gs, gr ), fast_score ( gi, gs, max_r ) );
        gstate gs1 = fast_final_state ( gi, gs, gr ), gs2 = fast_final_state ( gi, gs, max_r );
        output ( gs1 );
        output ( gs2 );
        if ( gr.val > max_r.val )
        {
            max_r = gr;
        }
        op_freq ( node_opt_chain ( unused_nodes.size() ), true );
    }
#ifdef PROFILE
    op_freq ( node_opt_chain ( unused_nodes.size() ), true );
#endif
    return max_r;
}

bool opt_toolset ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv )
{
REOPTIMIZATION:
    for ( ai_level al = AI_EASY; al <= lv; al = ( ai_level ) ( ( int ) al + 1 ) )
    {
        assert ( fast_score ( gi, gs, gr ) == gr.val );
        if ( interrupted )
        {
            break;
        }
        if ( opt_on_node ( gi, gs, gr, al ) )
        {
            assert ( fast_score ( gi, gs, gr ) == gr.val );
            eprintf ( "opt not ended\n" );
            goto REOPTIMIZATION;
            //can_be_opt++;
        }
        else
        {
            assert ( fast_score ( gi, gs, gr ) == gr.val );
            eprintf ( "opt ended this time\n" );
        }
        assert ( fast_score ( gi, gs, gr ) == gr.val );
        if ( opt_on_sw ( gi, gs, gr, al ) )
        {
            assert ( fast_score ( gi, gs, gr ) == gr.val );
            goto REOPTIMIZATION;
            //can_be_opt++;
        }
        assert ( fast_score ( gi, gs, gr ) == gr.val );
        if ( opt_on_t ( gi, gs, gr, al ) )
        {
            assert ( fast_score ( gi, gs, gr ) == gr.val );
            goto REOPTIMIZATION;
            //can_be_opt++;
        }
        assert ( fast_score ( gi, gs, gr ) == gr.val );
        //if ( can_be_opt > 8 )
        //{
        //    can_be_opt = 0;
        //    goto REOPTIMIZATION;
        //}
    }
    return false;
}


bool opt_on_node_mask ( const ginterface& gi, const gstate_ex& gs, groute& gr, const mask_t& mask )
{
    // TODO: optimize in order to reduce the dynamic allocations!
    // TODO: finish off shop before propagate nodes! And finish off shop before comparison!
    // TODO: if we accept that even if it is not topovalid for the other_nodes given a specific mask node layout, we can still generate a route to negotiate between them. That requires a bit higher time complexity, just like global p/b
    const int s = mask.size();
    if ( s == 0 )
    {
        return false;
    }
#ifdef DEBUG_MASK
    {
        eprintf ( "Mask content:" );
        for ( int i = 0; i < s; i++ )
        {
            fprintf ( stderr, "%d(%d) ", mask[i], gr.node_route[mask[i]] );
        }
        fprintf ( stderr, "\n" );
    }
#endif
    int dead_kmask = 0; //dead mask of k, which is the index of -1 in note chain that has been taken by the mask.
    int dead_mask_id = -1;
    std::vector<int>& orig_ns = gr.node_route;
    for ( int i = 0; i < s; i++ )
    {
        if ( orig_ns[mask[i]] == -1 )
        {
            dead_kmask = 1 << i;
            dead_mask_id = i;
            break;
        }
    }
    std::set<int> expanded_mask;
    for ( int i = 0; i < s; i++ )
    {
        expanded_mask.insert ( mask[i] );
    }
    int total_ns = orig_ns.size();
    int fullsize = 1 << s;
    auto other_events_p = vpool<int>::new_vector ( total_ns - s );
    auto strat_bundle_p = vpool<int>::new_vector ( fullsize * s );
    auto psi_bundle_p = vpool<long>::new_vector ( fullsize );
    auto gstate_bundle_p = vpool<gstate_ex>::new_vector ( fullsize );
    auto gstate_occupied_p = vpool<bool>::new_vector ( fullsize );
    std::vector<int>& other_events = *other_events_p;
    std::vector<int>& strat_bundle = *strat_bundle_p;
    std::vector<long>& psi_bundle = *psi_bundle_p;
    std::vector<gstate_ex>& gstate_bundle = *gstate_bundle_p;
    std::vector<bool>& gstate_occupied = *gstate_occupied_p;
    for ( int i = 0, j = 0; i < total_ns; i++ )
    {
        if ( !expanded_mask.count ( i ) )
        {
            other_events[j] = orig_ns[i];
            j++;
        }
    }
    //std::fill ( psi_bundle.begin(), psi_bundle.end(), 0 );
    std::fill ( gstate_occupied.begin(), gstate_occupied.end(), false );
    gstate_occupied[0] = true;
    gstate_bundle[0] = gs;
    psi_bundle[0] = 0;
    auto& poorder = perm_opt_order ( s );
    int n1pos = -1;
    for ( int i = 0; i <= total_ns - s; i++ )
    {
        for ( int jj = 0, pos = poorder.size(); jj < pos; jj++ )
        {
            int k1 = poorder[jj];
            if ( !gstate_occupied[k1] )
            {
                continue;
            }
            for ( int j = 0; j < s; j++ )
            {
                if ( k1 & ( 1 << j ) )
                {
                    continue;
                }
                if ( ( k1 & dead_kmask ) != 0 )
                {
                    continue;
                }
                int cur_node = orig_ns[ ( unsigned ) mask[j]];
                int k2 = k1 | ( 1 << j );
                int offs = __builtin_popcount ( k1 );
                if ( cur_node == -1 && gstate_bundle[k1].ev[EV_FLAG] <= 0 )
                {
                    continue;
                }
                gstate_ex competitor_gstate ( gstate_bundle[k1] );
                bool node_applied = false;
                // TODO: apply shop before evaluating!
                // cur_node = -1 is not applied!!! XXX
                if ( ( cur_node == -1 || ( node_applied = propagate_one_no_profiling ( gi, competitor_gstate, cur_node, gr ) ) ) )
                {
                    // A rude elimination here!!
                    if ( competitor_gstate.ev[EV_SALT] <= gr.salt )
                    {
                        bool better_flag = false;
                        long cur_psi = psi_bundle[k1] + progression_score ( competitor_gstate.ev );
                        if ( !gstate_occupied[k2] )
                        {
                            better_flag = true;
                        }
                        else
                        {
                            finish_shop ( gi, competitor_gstate, gr, empty_lambda );
                            long main_diff = partial_score ( competitor_gstate.ev ) - partial_score ( gstate_bundle[k2].ev );
                            if ( main_diff > 0 || ( main_diff == 0 && cur_psi > psi_bundle[k2] ) )
                            {
                                better_flag = true;
                            }
                        }
                        if ( better_flag )
                        {
                            //strat_bundle[k2] = strat_bundle[k1];
                            /*
                            eprintf ( "Substitution happens %d->%d at i=%d,realpos=%d\n", k1, k2, i, i + offs );
                            eprintf ( "Competitor is\n" );
                            output ( competitor_gstate );
                            if ( gstate_occupied[k2] )
                            {
                                eprintf ( "While defender is\n" );
                                output ( gstate_bundle[k2] );
                            }*/
                            for ( int i = 0; i < s; i++ )
                            {
                                strat_bundle[k2 * s + i] = strat_bundle[k1 * s + i];
                            }
                            strat_bundle[k2 * s + j] = i + offs;
                            gstate_bundle[k2] = competitor_gstate;
                            /*TODO if ( node_applied && gstate_occupied[k2] )
                            {
                                gstate_bundle[k2].shallow_copy ( competitor_gstate );
                            }
                            else
                            {
                                gstate_bundle[k2] = competitor_gstate;
                            }*/
                            psi_bundle[k2] = cur_psi;
                            gstate_occupied[k2] = true;
                        }
                    }
                }
            }
        }
        if ( i == total_ns - s )
        {
            break;
        }
        int cur_node = other_events[i];
        if ( cur_node == -1 )
        {
            n1pos = i;
            break;
        }
        else
        {
            for ( int k = 0; k < fullsize; k++ )
            {
                if ( ( k & dead_kmask ) == 0 && gstate_occupied[k] )
                {
                    bool flag = ( propagate_one_no_profiling ( gi, gstate_bundle[k] , cur_node, gr ) );
                    if ( !flag || ( gstate_bundle[k].ev[EV_SALT] > gr.salt ) )
                    {
                        gstate_occupied[k] = false;
                    }
                    else
                    {
                        psi_bundle[k] += progression_score ( gstate_bundle[k].ev );
                        finish_shop ( gi, gstate_bundle[k], gr, empty_lambda );
                    }
                }
            }
        }
    }
    long max_score = gr.val, max_psi = gr.psi, maxi = -1;
    for ( int k = 0; k < fullsize; k++ )
    {
        if ( dead_kmask && ! ( k & dead_kmask ) )
        {
            continue;
        }
        if ( !gstate_occupied[k] )
        {
            continue;
        }
        int folds = 0;
        if ( dead_kmask )
        {
            folds = total_ns - strat_bundle[k * s + dead_mask_id] - 1;
        }
        else
        {
            folds = total_ns - n1pos - __builtin_popcount ( k );
        }
        psi_bundle[k] += progression_score ( gstate_bundle[k].ev ) * folds;
        long cur_score = gi.score ( gstate_bundle[k] );
        if ( gstate_bundle[k].ev[EV_FLAG] && ( cur_score > max_score || ( cur_score == max_score && psi_bundle[k] > max_psi ) ) )
        {
            maxi = k;
            max_score = cur_score;
            max_psi = psi_bundle[k];
        }
    }
    if ( maxi == -1 )
    {
        return false;
    }
    auto best_route_p = vpool<int>::new_vector ( total_ns );
    std::vector<int>& best_route = *best_route_p; // = strat_bundle[maxi];
    std::fill ( best_route.begin(), best_route.end(), -2 );
    // another unnecessary definition here
    for ( int i = 0, j = total_ns - 1; i < s; i++ )
    {
        int mk_final_pos;
        if ( maxi & ( 1 << i ) )
        {
            mk_final_pos = strat_bundle[maxi * s + i];
        }
        else
        {
            mk_final_pos = j--;
        }
        best_route[mk_final_pos] = orig_ns[mask[i]];
    }
    for ( int i = 0, j = 0, ss = total_ns - s; i < ss; i++, j++ )
    {
        while ( best_route[j] != -2 )
        {
            j++;
        }
        best_route[j] = other_events[i];
    }
    //XXX here may be wrong
    //graph_list_complete ( orig_ns, best_route );
    bool retflag = gr.val < max_score;
#ifdef PROFILE
    if ( retflag )
    {
        eprintf ( "Mask content:" );
        for ( int i = 0; i < s; i++ )
        {
            fprintf ( stderr, "%d(%d) ", mask[i], gr.node_route[mask[i]] );
        }
        fprintf ( stderr, "\n" );
        output ( gstate_bundle[maxi] );
    }
#endif
    gr.node_route = best_route;
    gr.val = max_score;
    gr.psi = max_psi;
    gr.salt = gstate_bundle[maxi].ev[EV_SALT];
    //assert ( fast_score ( gi, gs, gr ) == gr.val );
    //eprintf ( "max_psi=%ld, fast_psi=%ld, fast_psi_score -> %ld\n", max_psi, fast_psi ( gi, gs, gr ), ( fast_psi_score ( gi, gs, gr ), gr.psi ) );
    //assert ( false );
    //assert ( fast_psi ( gi, gs, gr ) == max_psi );
    //assert ( ( fast_psi_score ( gi, gs, gr ) , gr.psi == max_psi ) );
    return retflag;
}

bool opt_on_mask_single ( const ginterface& gi, const gstate_ex& gs, groute& gr, int i )
{
    return opt_on_node_mask ( gi, gs, gr, mask_t ( {i} ) );
}

bool opt_on_node ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv )
{
    assert ( fast_score ( gi, gs, gr ) == gr.val );
    if ( lv < AI_MED )
    {
        int total_ns = gr.node_route.size();
        heuristic_smoothen ( gi, gs, gr.node_route, gr );
        bool flag = false;
        if ( total_ns <= MIN_MASK_N )
        {
            std::vector<int> mk ( total_ns );
            for ( int i = 0; i < total_ns; i++ )
            {
                mk[i] = i;
            }
            if ( opt_on_node_mask ( gi, gs, gr, mk ) )
            {
                flag = true;
            }
        }
        else
        {
            auto& mask = node_opt_chain ( gr.node_route.size() );
            int t = lv - AI_EASY; // mask type
            int s = mask[t][0].size();
            eprintf ( "total_ns:%d, ai_level:%d\n", total_ns, lv );
            std::vector<bool> mask_profile ( mask[t].size() );
            if ( t == 4 )
            {
                groute cur_gr;
                int range = total_ns;
                int dead_ind = -1;
                for ( int i = 0; i < total_ns; i++ )
                {
                    if ( gr.node_route[i] == -1 )
                    {
                        dead_ind = i;
                        break;
                    }
                }
                while ( range > 8 )
                {
                    int subrange = range >> 1;
                    bool end_flag = false;
                    for ( int i = 0; !end_flag; i += subrange )
                    {
                        if ( i + range >= total_ns )
                        {
                            i = total_ns - range;
                            end_flag = true;
                        }
                        if ( i > dead_ind )
                        {
                            break;
                        }
                        cur_gr = gr;
                        std::random_shuffle ( cur_gr.node_route.begin() + i, cur_gr.node_route.begin() + ( i + range ) );
                        eprintf ( "Shuffle %d-%d", i, i + range );
                        topo_validate ( gi, gs, cur_gr );
                        heuristic_smoothen ( gi, gs, gr.node_route, cur_gr );
                        fprintf ( stderr, "\e[3%dm", ( i / subrange ) % 6 + 1 );
                        opt_toolset ( gi, gs, cur_gr, AI_EASY_3 );
                        fprintf ( stderr, "\e[39m" );
                        if ( cur_gr.val > gr.val )
                        {
                            long before_score = fast_score ( gi, gs, gr );
                            long after_score = fast_score ( gi, gs, cur_gr );
                            assert ( before_score < after_score );
                            gr = cur_gr;
                            flag = true;
                        }
                    }
                    range >>= 1;
                }
            }
            if ( ( t & 1 ) == 0 )
            {
                //for ( std::vector<int>& mk : mask[t] )
                for ( int i = 0, ms = mask[t].size(); i < ms; i++ )
                {
                    const mask_t& mk = mask[t][i];
                    if ( opt_on_node_mask ( gi, gs, gr, mk ) )
                    {
                        flag = true;
                        mask_profile[i] = true;
                        frequency[t][i]++;
                    }
                }
#ifdef PROFILE
                op_freq ( mask );
#endif
            }
            else
            {
                //XXX here is the local optimization part.
                std::vector<int> mk;
                mk.reserve ( s );
                std::vector<int> orig_ns = gr.node_route;
                const graph& g = gi.int_graph();
                const std::vector<graph::bools>& av = g.adjacent_vector();
                const category_mark& cm = g.cm();
                const int real_node_cnt = cm.mob_end - cm.door_offset;
                std::vector<int> total_arrangement ( real_node_cnt );
                for ( int i = 0; i < real_node_cnt; i++ )
                {
                    total_arrangement[i] = i + cm.door_offset;
                }
                std::random_shuffle ( total_arrangement.begin(), total_arrangement.end() );
                //XXX: here it depends on the graph specific layout
                for ( int i = 0; i < real_node_cnt - 1; i++ )
                {
                    mk.clear();
                    if ( ( i & 0xff ) == 0xff )
                    {
                        eprintf ( "Progress: %d / %d\n", i, real_node_cnt - 1 );
                    }
                    graph::bools reach = av[i];
                    graph::bools chosen;
                    //if(i!=0||s!=8){
                    chosen.set ( total_arrangement[i] );
                    chosen.set ( total_arrangement[i + 1] );
                    int chosen_num = 3;
                    for ( int tt = 0; tt < MIN_MASK_N; tt++ )
                    {
                        //TODO: change a choosing strategy to pripritize mobs instead of roots
                        forbbe ( j, cm.door_offset, cm.mob_end, reach )
                        {
                            if ( !chosen[j] )
                            {
                                chosen.set ( j );
                                reach |= av[j];
                                chosen_num++;
                                if ( chosen_num == s )
                                {
                                    goto FINISH_CHOOSING;
                                }
                            }
                        }
                    }
FINISH_CHOOSING:
                    if ( chosen_num < s )
                    {
                        for ( int j = cm.door_offset; j < cm.mob_end; j++ )
                        {
                            if ( !chosen[j] )
                            {
                                chosen.set ( j );
                                reach |= av[j];
                                chosen_num++;
                                if ( chosen_num == s )
                                {
                                    break;
                                }
                            }
                        }
                    }//}else
                    //{chosen.set(70);chosen.set(74);chosen.set(82);chosen.set(83);chosen.set(76);chosen.set(87);chosen.set(75);eprintf("HACK_OPTIMIZE\n");}
                    int ms = 0;
                    for ( int j = 0; j < total_ns; j++ )
                    {
                        if ( gr.node_route[j] == -1 || ( ! ( gr.node_route[j] & ~groute::NODE_MASK ) && ( chosen[gr.node_route[j]&groute::NODE_MASK] ) ) )
                        {
                            mk.push_back ( j );
                            ms++;
                        }
                    }
                    assert ( ms <= 2 * s );
                    if ( opt_on_node_mask ( gi, gs, gr, mk ) )
                    {
                        // used as a random input source here.
                        opt_on_node_mask ( gi, gs, gr, mk );
                        flag = true;
                    }
                }
            }
        }
        return flag;
    }
    else
    {
        //groute_progress grp = generate_progress ( gi, gs, gr );
        int n = gr.node_route.size();
        int dead_ind = -1;
        for ( int i = 0; i < n; i++ )
        {
            if ( gr.node_route[i] == -1 )
            {
                dead_ind = i;
                break;
            }
        }
        if ( dead_ind == -1 )
        {
            eprintf ( "Bad node route as it is not terminated with -1!!" );
            return false;
        }
        std::vector<gstate_ex> gstate_bundle ( dead_ind + 1 );
        gstate_bundle[0] = gs;
        for ( int i = 0; i < dead_ind; i++ )
        {
            gstate_bundle[i + 1] = gstate_bundle[i];
            propagate_one_no_profiling ( gi, gstate_bundle[i + 1], gr.node_route[i], gr );
        }
        int loop_begin = min ( dead_ind - 1, n - 16 );
        int maxi = -1;//, maxj = -1;
        long max_val = gr.val;
        groute max_gr;
        int emplacement = max ( 1, min ( max ( 1, ( AI_AD - lv - 1 ) * ( loop_begin / 16 ) ), loop_begin / 4 ) );
        for ( int i = loop_begin; i >= 0; i -= emplacement )
        {
            //for ( int j = i + 1; j < n; j++ )
            //{
            //int cur_node = gr.node_route[j];
            //if ( cur_node == -1 )
            //{
            //    continue;
            //}
            gstate_ex cur_gs = gstate_bundle[i];
            cur_gs.shop_stage = 0; // XXX the gr environment is changed, so it must be first cleared before first used!!
            //if ( propagate_one_no_profiling ( gi, cur_gs, cur_node, gr ) )
            //{
            fprintf ( stderr, "\e[4%dm", ( i / emplacement ) % 6 + 1 );
            //fprintf ( stderr, "\e[8m", ( i / emplacement ) % 6 + 1 );
            groute res = opt_from_middle_builtin ( gi, cur_gs, ( ai_level ) ( lv - 5 ) );
            fprintf ( stderr, "\e[49m" );
            //fprintf ( stderr, "\e[0m" );
            if ( res.val > max_val )
            {
                maxi = i;
                //maxj = j;
                max_val = gr.val;
                max_gr = res;
                break;
            }
            //}
            //}
        }
        if ( maxi != -1 )
        {
            groute gr_concat = gr;
            long before_score = fast_score ( gi, gs, gr );
            //std::swap ( gr_concat.node_route[maxi], gr_concat.node_route[maxj] );
            gr = concat ( gi, gs, gr_concat, gr_concat, max_gr, maxi );
            long after_score = fast_score ( gi, gs, gr );
            assert ( before_score < after_score );
            return true;
        }
        return false;
    }
}

bool opt_on_sw_single ( const ginterface& gi, const gstate_ex& gs, groute& gr, int block_id )
{
    // the current gr should be the default value of the block.
    int total_ns = gr.node_route.size();
    gstate_ex gs_c_b ( gs );
    int stop_id = -1;
    int block_type = gr.shop_thr[block_id].first;
    const ev_entry shop_ev_type[3] = {EV_AT, EV_DF, EV_MF};
    ev_entry ev_type = shop_ev_type[block_type];
    for ( int i = 0; i < total_ns; i++ )
    {
        if ( emu_shop_progress ( gi, gs, gr ) >= block_id )
        {
            stop_id = i;
            break;
        }
        if ( gr.node_route[i] == -1 )
        {
            break;
        }
        propagate_one_no_profiling ( gi, gs_c_b, gr.node_route[i], gr );
    }
    if ( stop_id == -1 )
    {
        return false;
    }
    std::vector<int> dests;
    std::vector<gstate_ex> stats;
    std::vector<groute> helper_gres;
    dests.push_back ( gs_c_b.ev[ev_type] );
    stats.push_back ( gs_c_b );
    helper_gres.push_back ( gr );
    helper_gres[0].shop_thr[block_id].second = gs_c_b.ev[ev_type];
    groute tmp_gr = gr;
    tmp_gr.shop_thr[block_id].second = 0x7fffffff;
    for ( int i = stop_id; i < total_ns; i++ )
    {
        while ( true )
        {
            if ( stats.back().ev[ev_type] < helper_gres.back().shop_thr[block_id].second )
            {
                finish_shop ( gi, stats.back(), helper_gres.back(), empty_lambda, true );
                if ( stats.back().ev[ev_type] < helper_gres.back().shop_thr[block_id].second )
                {
                    break;
                }
            }
            helper_gres.push_back ( gr );
            dests.push_back ( stats.back().ev[ev_type] + 1 );
            helper_gres.back().shop_thr[block_id].second = dests.back();
            stats.push_back ( stats.back() );
            finish_shop ( gi, stats.back(), helper_gres.back(), empty_lambda, true );
        }
        int cur_node = gr.node_route[i];
        for ( int i = 0, s = stats.size(); i < s; i++ )
        {
            propagate_one_no_profiling ( gi, stats[i], cur_node, helper_gres[i] );
        }
        if ( cur_node == -1 )
        {
            break;
        }
    }
    int max_val = gr.shop_thr[block_id].first;
    long max_score = gr.val;
    bool retflag = false;
    //int maxi = -1;
    for ( int i = 0, s = stats.size(); i < s; i++ )
    {
        long cur_score = fast_score ( gi, gs, helper_gres[i] ); //gi.score ( stats[i] );
        if ( cur_score > max_score )
        {
            max_score = cur_score;
            max_val = dests[i];
            retflag = true;
            //maxi = i;
        }
    }
    if ( retflag )
    {
        gr.shop_thr[block_id].second = max_val;
        fast_psi_score ( gi, gs, gr );
        //eprintf ( "gr.val=%ld, max_score=%ld\n", gr.val, max_score );
        //assert ( gr.val == max_score );
    }
    return retflag;
}
bool opt_on_sw ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv )
{
    ( void ) lv;
    int ss = gr.shop_thr.size();
    bool flag = false;
    bool ret = false;
    do
    {
        flag = false;
        // stretch block
        for ( int i = 0; i < ss; i++ )
        {
            if ( opt_on_sw_single ( gi, gs, gr, i ) )
            {
                flag = true;
            }
        }
        groute helper_gr ( gr );
        int total_ns = gr.node_route.size();
        gstate_ex cur_gs ( gs );
        // insert block
        const ev_entry shop_ev_type[3] = {EV_AT, EV_DF, EV_MF};
        for ( int i = 0; i < total_ns; i++ )
        {
            int cur_block = cur_gs.shop_stage;
            // iterate through every possible gap and try inserting block
            // if inserting block is effective, then apply it
            // first iterate through the possible gaps; and after no more gap is
            // available currently, propagate node, and when the node is -1, break
            while ( true )
            {
                int block_type = gr.shop_thr[cur_block].first;
                int ev_type = shop_ev_type[block_type];
                if ( cur_gs.ev[ev_type] >= gr.shop_thr[cur_block].second )
                {
                    cur_block++;
                    continue;
                }
                int cur_val = cur_gs.ev[ev_type];
                groute cand_gr;
                std::vector<std::pair<int, int>> ins ( 2 );
                ins[0].first = block_type;
                ins[0].second = cur_val;
                for ( int i = 0; i < 3; i++ )
                {
                    if ( i == ins[0].first )
                    {
                        continue;
                    }
                    ins[1].first = i;
                    int orig_val = cur_gs.ev[shop_ev_type[i]];
                    ins[1].second = orig_val;
                    cand_gr = gr;// XXX
                    cand_gr.shop_thr.insert ( cand_gr.shop_thr.begin() + cur_block, ins.begin(), ins.end() );
                    opt_on_sw_single ( gi, gs, cand_gr, cur_block + 1 );
                    if ( cand_gr.shop_thr[cur_block + 1].second == orig_val )
                    {
                        //eprintf ( "The comparison appears to be the same.\n" );
                    }
                    else
                    {
                        gr = cand_gr;
                        flag = true;
                        break;
                    }
                }
                // if the block is applied, then the cur_block must +=1
                helper_gr = gr;
                block_type = gr.shop_thr[cur_block].first;
                ev_type = shop_ev_type[block_type];
                helper_gr.shop_thr[cur_block].second = cur_gs.ev[ev_type] + 1;
                finish_shop ( gi, cur_gs, gr, empty_lambda, true );
                if ( cur_gs.ev[ev_type] < helper_gr.shop_thr[cur_block].second )
                {
                    break;
                }
            }
            int cur_node = gr.node_route[i];
            propagate_one_no_profiling ( gi, cur_gs, cur_node, gr );
            if ( cur_node == -1 )
            {
                break;
            }
            /*
            helper_gres.push_back ( gr );
            dests.push_back ( stats.back().ev[ev_type] + 1 );
            helper_gres.back().shop_thr[block_id].second = dests.back();
            stats.push_back ( stats.back() );
            finish_shop ( gi, stats.back(), helper_gres.back(), empty_lambda, true );
            if ( stats.back().ev[ev_type] < helper_gres.back().shop_thr[block_id].second )
            {
                break;
            }*/
        }
        if ( flag )
        {
            ret = true;
        }
    }
    while ( flag );
    return ret;
}

bool opt_on_t_i ( const ginterface& gi, const gstate_ex& gs, groute& gr, int stray_id, bool associate_sw = false )
{
    int l, r;
    if ( stray_id == 0 )
    {
        l = 0;
    }
    else
    {
        l = gr.shop_tb_tokens[stray_id - 1];
    }
    if ( stray_id == ( int ) gr.shop_tb_tokens.size() - 1 )
    {
        r = 0x7fffffff;
    }
    else
    {
        r = gr.shop_tb_tokens[stray_id + 1];
    }
    gstate_ex gs_c_b ( gs ); // common beginning
    groute tmp_gr = gr;
    tmp_gr.node_route.clear();
    tmp_gr.shop_tb_tokens[stray_id] = l;
    int stop_id = -1;
    for ( int i = 0, s = gr.node_route.size(); i < s; i++ )
    {
        if ( gs_c_b.ev[EV_Tk] == stray_id && gs_c_b.ev[EV_SHOPTIMES] + gs_c_b.ev[EV_G] / 100 >= l )
        {
            stop_id = i;
            break;
        }
        if ( gr.node_route[i] == -1 )
        {
            break;
        }
        propagate_one_no_profiling ( gi, gs_c_b, gr.node_route[i], tmp_gr );
    }
    if ( stop_id == -1 )
    {
        return false;
    }
    // find out where it will end
    gstate_ex gs_f_e ( gs_c_b ); // gs the farthest end
    tmp_gr.shop_tb_tokens[stray_id] = r;
    for ( int i = stop_id, s = gr.node_route.size(); i < s; i++ )
    {
        if ( gs_f_e.ev[EV_Tk] == stray_id + 1 || gs_f_e.ev[EV_SHOPTIMES] == r )
        {
            r = gs_f_e.ev[EV_SHOPTIMES];
            break;
        }
        if ( gr.node_route[i] == -1 )
        {
            r = gs_f_e.ev[EV_SHOPTIMES];
            break;
        }
        propagate_one_no_profiling_rev ( gi, gs_f_e, gr.node_route[i], tmp_gr );
    }
    std::vector<gstate_ex> gses ( r - l + 1 );
    std::vector<groute> helper_gres ( r - l + 1, tmp_gr );
    for ( int i = 0; i < r - l + 1; i++ )
    {
        gses[i] = gs_c_b;
        helper_gres[i].shop_tb_tokens[stray_id] = i + l;
    }
    if ( associate_sw )
    {
        long max_score = gr.val;
        groute max_gr;
        for ( int i = 0; i < r - l + 1; i++ )
        {
            groute& cur_gr = helper_gres[i];
            opt_on_sw ( gi, gs, cur_gr, AI_EASY );
            long cur_score = fast_score ( gi, gs, cur_gr );
            if ( cur_score > max_score )
            {
                max_gr = cur_gr;
                max_score = cur_score;
            }
        }
        if ( max_score > gr.val )
        {
            gr = max_gr;
            fast_psi_score ( gi, gs, gr );
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        std::vector<gstate_ex> gses ( r - l + 1 );
        std::vector<groute> helper_gres ( r - l + 1, tmp_gr );
        for ( int i = 0; i < r - l + 1; i++ )
        {
            gses[i] = gs_c_b;
            helper_gres[i].shop_tb_tokens[stray_id] = i + l;
        }
        for ( int i = stop_id, s = gr.node_route.size(); i < s; i++ )
        {
            int cur_node = gr.node_route[i];
            for ( int j = 0; j < r - l + 1; j++ )
            {
                propagate_one_no_profiling ( gi, gses[j], cur_node, helper_gres[j] );
            }
            if ( cur_node == -1 )
            {
                break;
            }
        }
        int max_val = gr.shop_tb_tokens[stray_id];
        long max_score = gr.val;
        //assert ( max_score == gr.val );
        for ( int i = 0; i < r - l + 1; i++ )
        {
            long cur_score = gi.score ( gses[i] );
            if ( cur_score > max_score )
            {
                max_score = cur_score;
                max_val = l + i;
            }
        }
        if ( max_val != gr.shop_tb_tokens[stray_id] )
        {
            gr.shop_tb_tokens[stray_id] = max_val;
            fast_psi_score ( gi, gs, gr );
            assert ( gr.val == max_score );
            return true;
        }
    }
    return false;
}

bool opt_on_t ( const ginterface& gi, const gstate_ex& gs, groute& gr, ai_level lv )
{
    //gstate_ex final_gs = fast_final_state ( gi, gs, gr );
    ( void ) lv;
    bool finished = false;
    bool ret = false;
    while ( !finished )
    {
        finished = true;
        for ( int i = 0, s = gr.shop_tb_tokens.size(); i < s; i++ )
        {
            if ( opt_on_t_i ( gi, gs, gr, i ) )
            {
                finished = false;
            }
        }
        if ( !finished )
        {
            ret = true;
        }
    }
    if ( lv > AI_EASY )
    {
        finished = false;
        while ( !finished )
        {
            finished = true;
            for ( int i = 0, s = gr.shop_tb_tokens.size(); i < s; i++ )
            {
                if ( opt_on_t_i ( gi, gs, gr, i, true ) )
                {
                    finished = false;
                }
            }
            if ( !finished )
            {
                ret = true;
            }
        }
    }
    return ret;
}

void graph_list_complete ( const std::vector<int>& orig_s, std::vector<int>& cand_s )
{
    std::set<int> s;
    for ( int i : orig_s )
    {
        s.insert ( i + 1 );
    }
    for ( int i : cand_s )
    {
        s.erase ( i + 1 );
    }
    for ( int i : s )
    {
        cand_s.push_back ( i - 1 );
    }
}

const std::vector<int>& perm_opt_order ( int n )
{
    if ( !perm_opt_order_collection[n].empty() )
    {
        return perm_opt_order_collection[n];
    }
    std::vector<int>& res = perm_opt_order_collection[n];
    int fullsize = ( 1 << n ) - 1;
    std::vector<std::vector<int>> drawer ( n );
    for ( int i = 0; i < fullsize; i++ )
    {
        int c = __builtin_popcount ( i );
        drawer[c].push_back ( i );
    }
    for ( int i = 0; i < n; i++ )
    {
        res.insert ( res.end(), drawer[i].begin(), drawer[i].end() );
    }
    return res;
}

std::vector<std::vector<mask_t>>& node_opt_chain ( int total_choices )
{
    // We assert that the total choices are no less than 8
    // 2,2,3,3,4
    // 512,1024,256,512,128
    if ( !mask_collection[total_choices].empty() )
    {
        return mask_collection[total_choices];
    }
    std::vector<std::vector<std::vector<int>>> mask ( 18 );
    int MASK_COUNT[18];
    for ( int t = 2; t < 18; t++ )
    {
        MASK_COUNT[t] = 0;
        int max_stride = ( total_choices + 1 ) / t;
        int stride = max_stride;
        int split = 2;
        bool transit_flag = false;
        {
            int stride = 1;
            int substride = ( stride * t + 1 ) / 2;
            for ( int i = 0; i + stride < total_choices; i += substride )
            {
                for ( int j = 0; j < stride; j++ )
                {
                    int case_count = t, start = i + j, count = t, step = stride, cap = total_choices;
                    mask[case_count].push_back ( std::vector<int> ( t ) );
                    for ( int i = 0; i < count; i++ )
                    {
                        mask[case_count][MASK_COUNT[case_count]][i] = ( start + i * step ) % cap;
                    }
                    MASK_COUNT[case_count]++;
                }
            }
            if ( !transit_flag )
            {
                split++;
                stride = max_stride * 2 / split;
                if ( stride == max_stride * 2 / ( split + 1 ) )
                {
                    transit_flag = true;
                }
            }
            else
            {
                stride--;
            }
        }
        while ( stride > 1 )
        {
            int substride = ( stride * t + 1 ) / 2;
            for ( int i = 0; i + stride < total_choices; i += substride )
            {
                for ( int j = 0; j < stride; j++ )
                {
                    int case_count = t, start = i + j, count = t, step = stride, cap = total_choices;
                    mask[case_count].push_back ( std::vector<int> ( t ) );
                    for ( int i = 0; i < count; i++ )
                    {
                        mask[case_count][MASK_COUNT[case_count]][i] = ( start + i * step ) % cap;
                    }
                    MASK_COUNT[case_count]++;
                }
            }
            if ( !transit_flag )
            {
                split++;
                stride = max_stride * 2 / split;
                if ( stride == max_stride * 2 / ( split + 1 ) )
                {
                    transit_flag = true;
                }
            }
            else
            {
                stride--;
            }
        }
    }
    std::vector<int> ordered_indexes ( total_choices );
    for ( int i = 0; i < total_choices; i++ )
    {
        ordered_indexes[i] = i;
    }
    for ( int t = 2; t < 18; t++ )
    {
        int goal = 1024;//16384 >> t;
        std::random_shuffle ( ordered_indexes.begin(), ordered_indexes.end() );
        int u = 0;
        for ( int i = mask[t].size(); i < goal; i++ )
        {
            std::vector<int> mk ( t );
            for ( int j = 0; j < t; j++ )
            {
                mk[j] = ordered_indexes[u + j];
            }
            mask[t].push_back ( mk );
            u += t;
            if ( u + t > total_choices )
            {
                u = 0;
                std::random_shuffle ( ordered_indexes.begin(), ordered_indexes.end() );
            }
        }
    }
    std::vector<std::vector<mask_t>>& ret = mask_collection[total_choices];
    ret.resize ( 5 );
    ret[0].insert ( ret[0].end(), mask[4].begin(), mask[4].begin() + 1024 );
    ret[1].insert ( ret[1].end(), mask[4].begin(), mask[4].begin() + 1 );
    ret[2].insert ( ret[2].end(), mask[12].begin(), mask[12].begin() + 128 );
    ret[3].insert ( ret[3].end(), mask[12].begin(), mask[12].begin() + 1 );
    ret[4].insert ( ret[4].end(), mask[16].begin(), mask[16].begin() + 16 );
    frequency = std::vector<std::vector<int>> ( 5 );
    for ( int i = 0; i < 5; i++ )
    {
        frequency[i] = std::vector<int> ( ret[i].size() );
    }
    eprintf ( "Frequency array cleared!\n" );
    return ret;
}

long heuristic_difficulty ( const ginterface& gi, const gstate_ex gs, const groute& gr, int level = 4 )
{
    int zero_dam_nodes = 0;
    struct gstate_annotated
    {
        gstate_ex gs;
        long score;
        int orig_node;
        bool finished;
    };
    std::vector<std::map<std::set<int>, gstate_annotated>> bfs_list ( level + 1 );
    gstate_annotated orig_ga{gs, partial_score ( gs.ev ), 0, false};
    finish_shop ( gi, orig_ga.gs, gr );
    int total_ns = gr.node_route.size();
    const int LAYER_SIZE = 16;
    auto& g = gi.int_graph();
    //auto& cm = g.cm();
    //eprintf ( "original gs:" );
    //output ( orig_ga.gs );
    for ( int i = 0; i < total_ns; i++ )
    {
        int cur_node = gr.node_route[i];
        if ( ( cur_node == -1 && orig_ga.gs.ev[EV_FLAG] ) || ( cur_node != -1 && accessible ( gi, orig_ga.gs, cur_node ) ) )
        {
            if ( ! ( cur_node & groute::B_MARK ) )
            {
                int tile_ID = ( g.vertices() [cur_node][EV_NEGATIVE_ONETIME] ) & ( ~graph_big::DOOR_FLAG ) ;
                if ( tile_ID > mtstat::MOB )
                {
                    long dam = mtstat::dam ( gs.ev[EV_AT], gs.ev[EV_DF], gs.ev[EV_MF], gi.initialmts().enemies[tile_ID - mtstat::MOB] );
                    if ( dam == 0 )
                    {
                        zero_dam_nodes++;
                    }
                }
            }
            gstate_annotated cur_ga = orig_ga;
            /*
            if(cur_node==108)
            {
                eprintf("lets lookat 108");
                output(cur_ga.gs);
            }*/
            propagate_one_no_profiling_rev ( gi, cur_ga.gs, cur_node, gr );
            cur_ga.score = partial_score ( cur_ga.gs.ev );
            cur_ga.orig_node = i;
            cur_ga.finished = cur_ga.finished || ( cur_node == -1 );
            bfs_list[0][ {i}] = cur_ga;
            /*
            if(cur_node==108)
            {
                eprintf("lets lookat 108");
                output(cur_ga.gs);
            }*/
        }
    }
    for ( int i = 0; i < level; i++ )
    {
        auto& last_list = bfs_list[i];
        if ( last_list.size() > LAYER_SIZE * 2 )
        {
            std::vector<long> scores;
            for ( auto& bfs_pair : last_list )
            {
                scores.push_back ( bfs_pair.second.score );
            }
            std::nth_element ( scores.begin(), scores.end() - LAYER_SIZE, scores.end() );
            long thr = * ( scores.end() - LAYER_SIZE );
            for ( auto it = last_list.begin(); it != last_list.end(); )
            {
                if ( it->second.score < thr )
                {
                    last_list.erase ( it++ );
                }
                else
                {
                    ++it;
                }
            }
            if ( last_list.size() > LAYER_SIZE * 2 )
            {
                for ( auto it = last_list.begin(); it != last_list.end() && last_list.size() > LAYER_SIZE; )
                {
                    if ( it->second.score <= thr )
                    {
                        last_list.erase ( it++ );
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
        auto& cur_list = bfs_list[i + 1];
        if ( i == level )
        {
            break;
        }
        for ( auto& bfs_pair : last_list )
        {
            int branches = 0;
            for ( int j = 0; j < total_ns && branches < LAYER_SIZE; j++ )
            {
                int cur_node = gr.node_route[j];
                bool taken = false;
                if ( cur_node == -1 )
                {
                    if ( bfs_pair.second.gs.ev[EV_FLAG] )
                    {
                        taken = true;
                    }
                }
                else if ( accessible ( gi, bfs_pair.second.gs, cur_node ) )
                {
                    taken = true;
                }
                if ( taken )
                {
                    std::set<int> key = bfs_pair.first;
                    key.insert ( j );
                    gstate_annotated cur_ga = bfs_pair.second;
                    if ( !cur_ga.finished )
                    {
                        propagate_one_no_profiling_rev ( gi, cur_ga.gs, cur_node, gr );
                        cur_ga.score = partial_score ( cur_ga.gs.ev );
                        cur_ga.finished = cur_ga.finished || ( cur_node == -1 );
                    }
                    auto it = cur_list.find ( key );
                    if ( it == cur_list.end() )
                    {
                        cur_list[key] = cur_ga;
                        branches++;
                    }
                    else if ( cur_ga.score > it->second.score )
                    {
                        it->second = cur_ga;
                    }
                }
            }
        }
    }
    std::set<int> total_nodes;
    for ( auto& bfs_pair : bfs_list[level - 2] )
    {
        total_nodes.insert ( bfs_pair.second.orig_node );
    }
    long val = total_nodes.size() * 65536 >> ( log2_cap ( zero_dam_nodes + 1 ) );
    return val;
}

int choose_best_node ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int level, const std::set<int> s )
{
    struct gstate_annotated
    {
        gstate_ex gs;
        long score;
        int orig_node;
        bool finished;
    };
    std::vector<std::map<std::set<int>, gstate_annotated>> bfs_list ( level + 1 );
    gstate_annotated orig_ga{gs, partial_score ( gs.ev ), 0, false};
    finish_shop ( gi, orig_ga.gs, gr );
    int total_ns = gr.node_route.size();
    const int LAYER_SIZE = 16;
    //eprintf ( "original gs:" );
    //output ( orig_ga.gs );
    for ( int i = 0; i < total_ns; i++ )
    {
        if ( !s.empty() && s.count ( i ) == 0 )
        {
            continue;
        }
        int cur_node = gr.node_route[i];
        if ( ( cur_node == -1 && orig_ga.gs.ev[EV_FLAG] ) || ( cur_node != -1 && accessible ( gi, orig_ga.gs, cur_node ) ) )
        {
            gstate_annotated cur_ga = orig_ga;
            /*
            if(cur_node==108)
            {
                eprintf("lets lookat 108");
                output(cur_ga.gs);
            }*/
            propagate_one_no_profiling_rev ( gi, cur_ga.gs, cur_node, gr );
            cur_ga.score = partial_score ( cur_ga.gs.ev );
            cur_ga.orig_node = i;
            cur_ga.finished = cur_ga.finished || ( cur_node == -1 );
            bfs_list[0][ {i}] = cur_ga;
            /*
            if(cur_node==108)
            {
                eprintf("lets lookat 108");
                output(cur_ga.gs);
            }*/
        }
    }
    for ( int i = 0; i < level; i++ )
    {
        auto& last_list = bfs_list[i];
        if ( last_list.size() > LAYER_SIZE * 2 )
        {
            std::vector<long> scores;
            for ( auto& bfs_pair : last_list )
            {
                scores.push_back ( bfs_pair.second.score );
            }
            std::nth_element ( scores.begin(), scores.end() - LAYER_SIZE, scores.end() );
            long thr = * ( scores.end() - LAYER_SIZE );
            for ( auto it = last_list.begin(); it != last_list.end(); )
            {
                if ( it->second.score < thr )
                {
                    last_list.erase ( it++ );
                }
                else
                {
                    ++it;
                }
            }
            if ( last_list.size() > LAYER_SIZE * 2 )
            {
                for ( auto it = last_list.begin(); it != last_list.end() && last_list.size() > LAYER_SIZE; )
                {
                    if ( it->second.score <= thr )
                    {
                        last_list.erase ( it++ );
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
        auto& cur_list = bfs_list[i + 1];
        for ( auto& bfs_pair : last_list )
        {
            int branches = 0;
            for ( int j = 0; j < total_ns && branches < LAYER_SIZE; j++ )
            {
                int cur_node = gr.node_route[j];
                bool taken = false;
                if ( cur_node == -1 )
                {
                    if ( bfs_pair.second.gs.ev[EV_FLAG] )
                    {
                        taken = true;
                    }
                }
                else if ( accessible ( gi, bfs_pair.second.gs, cur_node ) )
                {
                    taken = true;
                }
                if ( taken )
                {
                    std::set<int> key = bfs_pair.first;
                    key.insert ( j );
                    gstate_annotated cur_ga = bfs_pair.second;
                    if ( !cur_ga.finished )
                    {
                        propagate_one_no_profiling_rev ( gi, cur_ga.gs, cur_node, gr );
                        cur_ga.score = partial_score ( cur_ga.gs.ev );
                        cur_ga.finished = cur_ga.finished || ( cur_node == -1 );
                    }
                    auto it = cur_list.find ( key );
                    if ( it == cur_list.end() )
                    {
                        cur_list[key] = cur_ga;
                        branches++;
                    }
                    else if ( cur_ga.score > it->second.score )
                    {
                        it->second = cur_ga;
                    }
                }
            }
        }
    }
    long max_score = 0x8000000000000000;
    std::set<int> maxkey;
    gstate_annotated* gap = NULL;
    for ( int l = level; l >= 0; l-- )
    {
        for ( auto& bfs_pair : bfs_list[l] )
        {
            long cur_score = bfs_pair.second.score;
            if ( cur_score > max_score )
            {
                max_score = cur_score;
                maxkey = bfs_pair.first;
                gap = & ( bfs_pair.second );
            }
        }
        if ( gap != NULL )
        {
            break;
        }
    }/*
    eprintf ( "Optimize result:" );
    for ( auto& bfs_pair : bfs_list[level] )
    {
        for ( int i : bfs_pair.first )
        {
            fprintf ( stderr, "%d ", gr.node_route[i] );
        }
        fprintf ( stderr, "->[score=%ld,orig_n=%d(%d)]", bfs_pair.second.score,bfs_pair.second.orig_node,gr.node_route[bfs_pair.second.orig_node] );
        output ( bfs_pair.second.gs );
    }*/
    if ( gap == NULL )
    {
        for ( int j = 0; j < total_ns; j++ )
        {
            if ( gr.node_route[j] == -1 )
            {
                return j;
            }
        }
        //return -1;
    }
    return gap->orig_node;
}
/*
long bfs_score ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int level )
{
    // TODO: bfs on shop
    std::vector<std::map<std::set<int>, gstate_ex>> bfs_list ( level + 1 );
    gstate_ex orig_gs = gs;
    finish_shop ( gi, orig_gs, gr );
    bfs_list[0] = {{{{}, orig_gs}}};
    int total_ns = gr.node_route.size();
    const int LAYER_SIZE = 256;
    for ( int i = 0; i < level; i++ )
    {
        auto& last_list = bfs_list[i];
        if ( last_list.size() > LAYER_SIZE * 2 )
        {
            std::vector<long> scores;
            for ( auto& bfs_pair : last_list )
            {
                scores.push_back ( partial_score ( bfs_pair.second.ev ) );
            }
            std::nth_element ( scores.begin(), scores.end() - LAYER_SIZE, scores.end() );
            long thr = * ( scores.end() - LAYER_SIZE );
            for ( auto it = last_list.begin(); it != last_list.end(); )
            {
                if ( partial_score ( it->second.ev ) < thr )
                {
                    last_list.erase ( it++ );
                }
                else
                {
                    ++it;
                }
            }
        }
        auto& cur_list = bfs_list[i + 1];
        for ( auto& bfs_pair : last_list )
        {
            for ( int j = 0; j < total_ns; j++ )
            {
                int cur_node = gr.node_route[j];
                bool taken = false;
                if ( cur_node == -1 )
                {
                    if ( bfs_pair.second.ev[EV_FLAG] )
                    {
                        taken = true;
                    }
                }
                else if ( accessible ( gi, bfs_pair.second, cur_node ) )
                {
                    taken = true;
                }
                if ( taken )
                {
                    std::set<int> key = bfs_pair.first;
                    key.insert ( j );
                    gstate_ex gst = bfs_pair.second;
                    propagate_one_no_profiling ( gi, gst, cur_node, gr );
                    finish_shop ( gi, gst, gr );
                    auto it = cur_list.find ( key );
                    if ( it == cur_list.end() )
                    {
                        cur_list[key] = gst;
                    }
                    else if ( partial_score ( gst.ev ) > partial_score ( it->second.ev ) )
                    {
                        it->second = gst;
                    }
                }
            }
        }
    }
    long max_score = 0x8000000000000000;
    std::set<int> maxkey;
    for ( auto& bfs_pair : bfs_list[level] )
    {
        long cur_score = partial_score ( bfs_pair.second.ev );
        if ( cur_score > max_score )
        {
            max_score = cur_score;
            maxkey = bfs_pair.first;
        }
    }
    //{
    //gstate_ex& maxgs = bfs_list[level][maxkey];
    ////}
    return max_score;
}*/

bool heuristic_smoothen ( const ginterface& gi, const gstate_ex& gs, const std::vector<int>& ns, groute& gr, int level )
{
    int total_ns = ns.size();
    gstate_ex max_gs ( gs ), cur_gs;
    groute cur_gr;
    bool helped = false;
    eprintf ( "Topoheuhelp\n" );
    for ( int i = 0; i < total_ns; i++ )
    {
        //eprintf ( "Heuristic on %dth node\n", i );
        //output ( max_gs );
        cur_gs = max_gs;
        cur_gr = gr;
        std::set<int> cand_nodes;
        for ( int j = 0; j < level * 4 && j < total_ns; j++ )
        {
            cand_nodes.insert ( j );
        }
        int maxj = choose_best_node ( gi, cur_gs, cur_gr, level, cand_nodes );
        int tmp = cur_gr.node_route[maxj];
        for ( int k = maxj; k > i; k-- )
        {
            cur_gr.node_route[k] = cur_gr.node_route[k - 1];
        }
        cur_gr.node_route[i] = tmp;
        //std::swap ( gr.node_route[maxj], gr.node_route[i] );//XXX killed
        topo_validate ( gi, gs, cur_gr, true );
        fast_psi_score ( gi, gs, cur_gr );
        if ( cur_gr.val < gr.val )
        {
            //eprintf ( "RARE case happens that heuristic one will do no better than the original one.\n" );
        }
        if ( cur_gr.val > gr.val )
        {
            gr = cur_gr;
            helped = true;
        }
        //eprintf ( "Chosen node: %d", ret.node_route[i] );
        if ( gr.node_route[i] == -1 )
        {
            break;
        }
        propagate_one_no_profiling ( gi, max_gs, gr.node_route[i], gr );
        assert ( fast_score ( gi, gs, gr ) == gr.val );
    }
    assert ( fast_score ( gi, gs, gr ) == gr.val );
    if ( helped )
    {
        eprintf ( "Helped!" );
    }
    else
    {
        eprintf ( "No much help!" );
    }
    return helped;
}

groute topovalid_heuristic_randfill ( const ginterface& gi, const gstate_ex& gs, const std::vector<int>& ns, int ts, int level )
{
    // Note: it can only work in the case where tokens are less than 16.
    groute ret ( topovalid_randfill ( gi, gs, ns, ts ) );
    int total_ns = ns.size();
    gstate_ex max_gs ( gs ), cur_gs;
    eprintf ( "Topoheufill\n" );
    for ( int i = 0; i < total_ns; i++ )
    {
        //eprintf ( "Heuristic on %dth node\n", i );
        //output ( max_gs );
        cur_gs = max_gs;
        int maxj = choose_best_node ( gi, cur_gs, ret, level );
        std::swap ( ret.node_route[maxj], ret.node_route[i] );
        //eprintf ( "Chosen node: %d", ret.node_route[i] );
        if ( ret.node_route[i] == -1 )
        {
            break;
        }
        propagate_one_no_profiling ( gi, max_gs, ret.node_route[i], ret );
    }
    fast_psi_score ( gi, gs, ret );
    return ret;
}

groute topovalid_randfill ( const ginterface& gi, const gstate_ex& gs, const std::vector<int>& ns, int ts )
{
    // Note: it can only work in the case where tokens are less than 16.
    groute ret;
    for ( int t = 0; t < 16; t++ )
    {
        ret.node_route = ns;
        std::random_shuffle ( ret.node_route.begin(), ret.node_route.end() - 1 );
        ret.shop_tb_tokens.resize ( ts + 1 );
        for ( int i = 0; i < ts + 1; i++ )
        {
            ret.shop_tb_tokens[i] = 100 * i + 100;
        }
        ret.shop_tb_tokens[ts] = S_B_NUM;
        int shop_order[3] = {0, 1, 2};
        int shop_rand = rand() % 6;
        int s1 = shop_rand % 3;
        int s2 = shop_rand % 2;
        std::swap ( shop_order[2], shop_order[s1] );
        std::swap ( shop_order[1], shop_order[s2] );
        ret.shop_thr.resize ( 3 );
        ret.shop_thr[0] = std::pair<int, int> ( shop_order[0], S_B_NUM );
        ret.shop_thr[1] = std::pair<int, int> ( shop_order[1], S_B_NUM );
        ret.shop_thr[2] = std::pair<int, int> ( shop_order[2], S_B_NUM );
        // Validate the topo layout by using the limits of gi.
        if ( topo_validate ( gi, gs, ret ) )
        {
            break;
        }
    }
    return ret;
}

bool topo_validate ( const ginterface& gi, const gstate_ex& gs, groute& gr, bool optimized )
{
    gstate_ex cur_gs ( gs );
    bool flag = true;
    int n = gr.node_route.size();
    if ( !optimized )
    {
        for ( int i = n - 1; i >= 0; i-- )
        {
            if ( gr.node_route[i] == -1 )
            {
                int tmp = gr.node_route[n - 1];
                gr.node_route[n - 1] = gr.node_route[i];
                gr.node_route[i] = tmp;
                break;
            }
        }
    }
    for ( int i = 0; i < n; i++ )
    {
        if ( gr.node_route[i] == -1 )
        {
            break;
        }
        if ( !propagate_one_no_profiling ( gi, cur_gs, gr.node_route[i], gr ) )
        {
            bool succeed_flag = false;
            for ( int j = i + 1; j < n; j++ )
            {
                if ( gr.node_route[j] == -1 || propagate_one_no_profiling ( gi, cur_gs, gr.node_route[j], gr ) )
                {
                    succeed_flag = true;
                    int tmp = gr.node_route[j];
                    for ( int k = j; k > i; k-- )
                    {
                        gr.node_route[k] = gr.node_route[k - 1];
                    }
                    gr.node_route[i] = tmp;
                    /* SO WILD!
                    int tmp = gr.node_route[j];
                    gr.node_route[j] = gr.node_route[i];
                    gr.node_route[i] = tmp;
                    */
                    if ( tmp == -1 )
                    {
                        goto FINAL;
                    }
                    break;
                }
            }
            if ( !succeed_flag )
            {
                eprintf ( "topo_validate failed. The -1 who marks an end of a node series is not found.\n" );
                flag = false;
                goto FINAL;
            }
        }
    }
FINAL:
    if ( flag )
    {
        fast_psi_score ( gi, gs, gr );
    }
    return flag;
}

unit_event n2ue ( const ginterface& gi, int node_id )
{
    const auto& i_graph = gi.int_graph();
    const auto& cm = i_graph.cm();
    unit_event ue ( groute::NODE_MASK & node_id );
    //const auto& reach = gs.reach;
    if ( node_id & groute::B_MARK )
    {
        ue.e = unit_event::EVENT_B;
    }
    else if ( ue.n >= cm.wall_offset )
    {
        ue.e = unit_event::EVENT_P;
    }
    return ue;
}

bool accessible ( const ginterface& gi, const gstate_ex& gs, int node_id )
{
    if ( node_id == -1 )
    {
        return true;
    }
    return gi.accessible ( gs, n2ue ( gi, node_id ) );
}

int emu_shop_progress ( const ginterface& gi, const gstate_ex& gs, const groute& gr )
{
    //XXX: sync
    const auto& i_graph = gi.int_graph();
    const auto& cm = i_graph.cm();
    const auto& reach = gs.reach;
    int s_shop_times = gs.ev[EV_G] / 100;
    int tok = gs.ev[EV_Tk];
    int total_shop_times = 0;
    s_shop_times = min ( s_shop_times, gr.shop_tb_tokens[tok] - gs.ev[EV_SHOPTIMES] );
    int shop_levels[16];
    memset ( shop_levels, 0, sizeof ( shop_levels ) );
    if ( s_shop_times != 0 )
    {
        total_shop_times += s_shop_times;
    }
    shop_levels[gs.ev[EV_Tk]] += s_shop_times;
    std::vector<int> wcand[16];
    forbbe ( i, cm.w_offset, cm.w_end, reach )
    {
        int wsize = i_graph.vertices() [i] [EV_W];
        assert ( wsize > 0 );
        int wmag = log2_for_2_power ( wsize );
        shop_levels[wmag]++;
        wcand[wmag].push_back ( i );
        total_shop_times++;
        // NOTE here that reach does not contain taken, and that w that are reachable but not taken IS in reach.
    }
    if ( total_shop_times == 0 )
    {
        return gs.shop_stage;
    }
    else
    {
        int res = gs.shop_stage;
        const ev_entry shop_ev_type[3] = {EV_AT, EV_DF, EV_MF};
        int unit[3];
        unit[0] = gi.initialmts().unit_at;
        unit[1] = gi.initialmts().unit_df;
        unit[2] = gi.initialmts().unit_mf;
        for ( int i = gs.shop_stage, s = gr.shop_thr.size();
                total_shop_times != 0 && i < s;
                i++ )
        {
            int this_shoptype = gr.shop_thr[i].first;
            int this_shopthr = gr.shop_thr[i].second;
            if ( gs.ev[shop_ev_type[this_shoptype]] >= this_shopthr )
            {
                continue;
            }
            int diff = this_shopthr - gs.ev[shop_ev_type[this_shoptype]];
            int uniti = unit[this_shoptype];
            int times = ( diff + uniti - 1 ) / uniti;
            // Here is our intelligent shopping algorithm.
            int actual_shops[16];
            intelligent_shopper ( times, shop_levels, actual_shops );
            for ( int j = 0; j < 16; j++ )
            {
                int st = actual_shops[j];
                if ( st == 0 )
                {
                    continue;
                }
                total_shop_times -= st;
                shop_levels[j] -= st;
                if ( j == tok )
                {
                    if ( s_shop_times > 0 )
                    {
                        int sshoptimes = min ( st, s_shop_times );
                        st -= sshoptimes;
                        s_shop_times -= sshoptimes;
                    }
                }
                if ( st != 0 )
                {
                    for ( int k = 0; k < st; k++ )
                    {
                        wcand[j].pop_back();
                    }
                }
            }
            if ( total_shop_times == 0 )
            {
                break;
            }
            res++;
        }
        return res;
    }
}

template<typename F_t>
void finish_shop ( const ginterface& gi, gstate_ex& gs, const groute& gr, F_t f, bool single )
{
    static_assert (
        std::is_same<void,
        std::result_of_t < F_t (
            unit_event
        ) >
        >::value,
        "This function requires a lambda expression that takes in a unit_event and returns void." );
    // Then we need to use s/w wisely.
    const auto& i_graph = gi.int_graph();
    const auto& cm = i_graph.cm();
    const auto& reach = gs.reach;
    int s_shop_times = gs.ev[EV_G] / 100;
    int tok = gs.ev[EV_Tk];
    int total_shop_times = 0;
    s_shop_times = min ( s_shop_times, gr.shop_tb_tokens[tok] - gs.ev[EV_SHOPTIMES] );
    int shop_levels[16];
    memset ( shop_levels, 0, sizeof ( shop_levels ) );
    if ( s_shop_times != 0 )
    {
        total_shop_times += s_shop_times;
    }
    shop_levels[gs.ev[EV_Tk]] += s_shop_times;
    std::vector<int> wcand[16];
    forbbe ( i, cm.w_offset, cm.w_end, reach )
    {
        int wsize = i_graph.vertices() [i] [EV_W];
        assert ( wsize > 0 );
        int wmag = log2_for_2_power ( wsize );
        shop_levels[wmag]++;
        wcand[wmag].push_back ( i );
        total_shop_times++;
        // NOTE here that reach does not contain taken, and that w that are reachable but not taken IS in reach.
    }
    if ( total_shop_times != 0 )
    {
        const ev_entry shop_ev_type[3] = {EV_AT, EV_DF, EV_MF};
        const unit_event::shop_type shop_event_type[3] = {unit_event::SHOP_AT, unit_event::SHOP_DF, unit_event::SHOP_MF};
        int unit[3];
        unit[0] = gi.initialmts().unit_at;
        unit[1] = gi.initialmts().unit_df;
        unit[2] = gi.initialmts().unit_mf;
        for ( int i = gs.shop_stage, s = gr.shop_thr.size();
                total_shop_times != 0 && i < s;
                i++ )
        {
            int this_shoptype = gr.shop_thr[i].first;
            int this_shopthr = gr.shop_thr[i].second;
            if ( gs.ev[shop_ev_type[this_shoptype]] >= this_shopthr )
            {
                gs.shop_stage++;
                continue;
            }
            int diff = this_shopthr - gs.ev[shop_ev_type[this_shoptype]];
            int uniti = unit[this_shoptype];
            int times = ( diff + uniti - 1 ) / uniti;
            // Here is our intelligent shopping algorithm.
            int actual_shops[16];
            intelligent_shopper ( times, shop_levels, actual_shops );
            for ( int j = 0; j < 16; j++ )
            {
                int st = actual_shops[j];
                if ( st == 0 )
                {
                    continue;
                }
                total_shop_times -= st;
                shop_levels[j] -= st;
                if ( j == tok )
                {
                    if ( s_shop_times > 0 )
                    {
                        int sshoptimes = min ( st, s_shop_times );
                        st -= sshoptimes;
                        s_shop_times -= sshoptimes;
                        //for ( int k = 0; k < sshoptimes; k++ )
                        //{
                        unit_event ue =  unit_event ( sshoptimes, unit_event::EVENT_S, shop_event_type[this_shoptype] ) ;
                        f ( ue );
                        bool flagp = gi.propagate ( gs, ue ); // TODO: to combine shops into one event by chained shop event??
                        assert ( flagp );
                        //}
                    }
                }
                if ( st != 0 )
                {
                    for ( int k = 0; k < st; k++ )
                    {
                        unit_event ue = unit_event ( wcand[j].back(), unit_event::EVENT_W, shop_event_type[this_shoptype] );
                        f ( ue );
                        bool flagp = gi.propagate ( gs, ue );
                        assert ( flagp );
                        wcand[j].pop_back();
                    }
                }
            }
            if ( total_shop_times == 0 )
            {
                break;
            }
            if ( single )
            {
                break;
            }
            gs.shop_stage++;
        }
    }
}

template<typename F_t>
bool propagate_nodeonly_builtin ( const ginterface& gi, gstate_ex& gs, int node_id, F_t f )
{
    static_assert (
        std::is_same<void,
        std::result_of_t < F_t (
            unit_event
        ) >
        >::value,
        "This function requires a lambda expression that takes in a unit_event and returns void." );
    if ( node_id == -1 )
    {
        return true;
    }
    unit_event ue ( n2ue ( gi, node_id ) );
#ifndef NDEBUG
    if ( !gi.accessible ( gs, ue ) )
    {
        eprintf ( "This may be unintended! the node #%d is inaccessible from the current gstate.\n", node_id );
        return false;
    }
#endif
    f ( ue );
    gi.propagate_no_check ( gs, ue );
    return true;
}

template<typename F_t>
void propagate_one_builtin ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr, F_t f )
{
    static_assert (
        std::is_same<void,
        std::result_of_t < F_t (
            unit_event
        ) >
        >::value,
        "This function requires a lambda expression that takes in a unit_event and returns void." );
    finish_shop ( gi, gs, gr, f );
    propagate_nodeonly_builtin ( gi, gs, node_id, f );
}

// XXX sync!
bool propagate_one_no_profiling ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr )
{
    bool ac = accessible ( gi, gs, node_id );
    if ( !ac )
    {
        return false;
    }
    propagate_one_builtin ( gi, gs, node_id, gr, [] ( unit_event ) {} );
    return true;
}

bool propagate_one_no_profiling_rev ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr )
{
    bool ac = accessible ( gi, gs, node_id );
    if ( !ac )
    {
        return false;
    }
    finish_shop ( gi, gs, gr, empty_lambda );
    propagate_nodeonly_builtin ( gi, gs, node_id, empty_lambda );
    return true;
}


event_list propagate_one ( const ginterface& gi, gstate_ex& gs, int node_id, const groute& gr )
{
    bool ac = accessible ( gi, gs, node_id );
    if ( !ac )
    {
        return event_list();
    }
    event_list el;
    propagate_one_builtin ( gi, gs, node_id, gr, [&el] ( unit_event ue )
    {
        el.push_back ( ue );
    } );
    return el;
}

void intelligent_shopper ( int goal, const int* in, int* out )
{
    memset ( out, 0, sizeof ( int ) * 16 );
    if ( __builtin_expect ( goal <= 0, 0 ) )
    {
        return;
    }
    if ( __builtin_expect ( goal == 1, 0 ) )
    {
        for ( int i = 0; i < 16; i++ )
        {
            if ( in[i] > 0 )
            {
                out[i] = 1;
                return;
            }
        }
        return;
    }
    else
    {
        int dig = min ( log2_cap ( goal ), 16 );
        int sum[16];
        sum[0] = in[0];
        int exist_mask = 0;
        for ( int i = 0; i < 16; i++ )
        {
            if ( in[i] )
            {
                exist_mask |= ( 1 << i );
            }
        }
        for ( int i = 1; i < dig; i++ )
        {
            sum[i] = sum[i - 1] + ( in[i] << i );
        }
        while ( goal > 0 )
        {
            if ( sum[dig - 1] < goal )
            {
                int higher_mask = ( exist_mask >> dig ) << dig;
                if ( higher_mask == 0 )
                {
                    memcpy ( out, in, sizeof ( int ) * dig ); // else take all we have now.
                }
                else
                {
                    int higher_magnitude = __builtin_ctz ( higher_mask );
                    out[higher_magnitude] += 1;
                }
                return;
            }
            {
                int first_n = 31 - __builtin_clz ( exist_mask & ( ( 1 << dig ) - 1 ) );
                int first_n_in = min ( in[first_n], goal >> first_n );
                goal -= first_n_in << first_n;
                out[first_n] += first_n_in;
                dig = first_n;
            }
        }
    }
}

//The following three function did virtually the same, with only the return value differs.
//So when modify note that the three function should be sync.

template<typename F1_t, typename F2_t, typename F3_t, typename F4_t>
void peruse_builtin_general ( const ginterface& gi, const gstate_ex& gs, const groute& gr, F1_t f_ev, F2_t f_gs, F3_t f_gs_end , F4_t f_gs_err )
{
    static_assert (
        std::is_same<void,
        std::result_of_t < F1_t (
            unit_event
        ) >
        >::value,
        "This function requires a lambda expression f_ev that takes in a unit_event and returns void." );
    static_assert (
        std::is_same<void,
        std::result_of_t < F2_t (
            gstate_ex&
        ) >
        >::value,
        "This function requires a lambda expression f_gs that takes in a gstate_ex& and returns void." );
    static_assert (
        std::is_same<void,
        std::result_of_t < F3_t (
            gstate_ex&
        ) >
        >::value,
        "This function requires a lambda expression f_gs_end that takes in a gstate_ex& and returns void." );
    gstate_ex cur_gs = gs;
    for ( int i : gr.node_route )
    {
        if ( !accessible ( gi, cur_gs, i ) )
        {
            f_gs_err ( cur_gs, i );
            return;
        }
        propagate_one_builtin ( gi, cur_gs, i, gr, f_ev );
        f_gs ( cur_gs );
        if ( i == -1 )
        {
            break;
        }
    }
    f_gs_end ( cur_gs );
}

long fast_psi ( const ginterface& gi, const gstate_ex& gs, const groute& gr )
{
    long ret = 0;
    peruse_builtin_general ( gi, gs, gr, empty_lambda, [&ret] ( gstate_ex & gs )
    {
        ret += progression_score ( gs.ev );
    } , [&ret, &gr] ( gstate_ex & gs )
    {
        for ( int i = 0, s = gr.node_route.size(); i < s; i++ )
        {
            if ( gr.node_route[i] == -1 )
            {
                ret += progression_score ( gs.ev ) * ( s - i - 1 );
                break;
            }
        }
    } );
    return ret;
}

long fast_score ( const ginterface& gi, const gstate_ex& gs, const groute& gr )
{
    long ret = 0;
    peruse_builtin_general ( gi, gs, gr, empty_lambda, empty_lambda, [&gi, &ret] ( gstate_ex & gs )
    {
        ret = gi.score ( gs );
    } );
    return ret;
}

gstate fast_final_state ( const ginterface& gi, const gstate_ex& gs, const groute& gr )
{
    gstate ret;
    peruse_builtin_general ( gi, gs, gr, empty_lambda, empty_lambda, [&gi, &ret] ( gstate_ex & gs )
    {
        ret = gs;
    } );
    return ret;
}


void fast_psi_score ( const ginterface& gi, const gstate_ex& gs, groute& gr )
{
    // psi, score, and salt
    long val = 0, psi = 0, salt = 0;
    peruse_builtin_general ( gi, gs, gr, empty_lambda, [&psi] ( gstate_ex & gs )
    {
        psi += progression_score ( gs.ev );
    } ,
    [&gi, &val, &salt, &psi, &gr] ( gstate_ex & gs )
    {
        val = gi.score ( gs );
        salt = gs.ev[EV_SALT];
        for ( int i = 0, s = gr.node_route.size(); i < s; i++ )
        {
            if ( gr.node_route[i] == -1 )
            {
                psi += progression_score ( gs.ev ) * ( s - i - 1 );
                break;
            }
        }
    } );
    gr.val = val;
    gr.psi = psi;
    gr.salt = salt;
}

long ai_difficulty ( const ginterface& gi, const gstate& gs, const groute& gr )
{
    long diff = 0;
    peruse_builtin_general ( gi, gs, gr, empty_lambda, [&gi, &gr, &diff] ( gstate_ex& )
    {
    }, [&diff] ( gstate_ex & res )
    {
        if ( res.ev[EV_SALT] )
        {
            diff = 0x8000000000000000;
        }
        else
        {
            diff -= res.ev[EV_HP];
            diff -= res.ev[EV_YK];
            diff -= res.ev[EV_BK];
            diff -= res.ev[EV_RK];
            diff -= res.ev[EV_Px];
            diff -= res.ev[EV_Bb];
        }
    }, [&diff] ( gstate_ex&, int )
    {
        diff = 0x8000000000000000;
    } );
    if ( diff == ( long ) 0x8000000000000000 )
    {
        return diff;
    }
    peruse_builtin_general ( gi, gs, gr, empty_lambda, [&gi, &gr, &diff] ( gstate_ex & gs )
    {
        diff += heuristic_difficulty ( gi, gs, gr );
    } );
    return diff;
}

ai_return_t translate ( const ginterface& gi, const gstate_ex& gs, const groute& gr )
{
    ai_return_t ret;
    peruse_builtin_general ( gi, gs, gr,
                             [&ret] ( unit_event ue )
    {
        ret.el.push_back ( ue );
    }, empty_lambda,
    [&gi, &ret] ( gstate_ex & gs )
    {
        ret.val = gi.score ( gs );
    } );
    ret.gr = gr;
    return ret;
}
/*
std::pair<groute_progress, gstate_ex> generate_progress_builtin ( const ginterface& gi, const gstate& gs, const groute& gr, int node_range )
{
    // generate the progress after the event occurs
    if ( node_range == -1 )
    {
        node_range = gr.node_route.size();
    }
    groute_progress ret ( gr.node_route.size() );
    gstate_ex cur_gs = gs;
    int p_progress = 0, b_progress = 0, sw_progress = 0, t_progress = 0;
    int p_total = gr.p_mark.size();
    int b_total = gr.b_mark.size();
    int sw_total = gr.shop_thr.size();
    int t_total = gr.shop_tb_tokens.size();
    const ev_entry shop_ev_type[3] = {EV_AT, EV_DF, EV_MF};
    for ( int ii = 0, s = node_range; ii < s; ii++ )
    {
        int i = gr.node_route[ii];
        if ( i == -1 )
        {
            break;
        }
        event_list el = propagate_one ( gi, cur_gs, i, gr );
        assert ( el.size() != 0 );
        while ( cur_gs.taken[gr.p_mark[p_progress]]
                && p_progress < p_total )
        {
            p_progress++;
        }
        while ( cur_gs.taken[gr.b_mark[b_progress]]
                && b_progress < b_total )
        {
            b_progress++;
        }
        while ( sw_progress < sw_total )
        {
            int this_shoptype = gr.shop_thr[sw_progress].first;
            int this_shopthr = gr.shop_thr[sw_progress].second;
            if ( gs.ev[shop_ev_type[this_shoptype]] < this_shopthr )
            {
                break;
            }
            sw_progress++;
        }
        while ( cur_gs.ev[EV_SHOPTIMES] >= gr.shop_tb_tokens[t_progress]
                && t_progress < t_total )
        {
            t_progress++;
        }
        ret[ii][PI_P] = p_progress;
        ret[ii][PI_B] = b_progress;
        ret[ii][PI_SW] = sw_progress;
        ret[ii][PI_T] = t_progress;
    }
    return std::pair<groute_progress, gstate_ex> ( ret, cur_gs );
}

groute_progress generate_progress ( const ginterface& gi, const gstate_ex& gs, const groute& gr, int node_range )
{
    return generate_progress_builtin ( gi, gs, gr, node_range ).first;
}
*/

void graph_list_concat ( const std::vector<int>& templ, const std::vector<int>& list1, const std::vector<int>& list2, int l1_len, std::vector<int>& ret )
{
    ret = list1;
    ret.resize ( l1_len );
    ret.insert ( ret.end(), list2.begin(), list2.end() );
    graph_list_complete ( templ, ret );
}

groute concat ( const ginterface& gi, const gstate_ex& gs, const groute& templ, const groute& gr1, const groute& gr2, int gr1_len )
{
    groute ret ( gr1 );
    ret.val = gr2.val;
    //auto grp = generate_progress_builtin ( gi, gs, gr1, gr1_len );
    gstate_ex mid_state = gs;
    finish_shop ( gi, mid_state, gr1 );
    for ( int i = 0; i < gr1_len; i++ )
    {
        propagate_one_no_profiling ( gi, mid_state, gr1.node_route[i], gr1 );
    }
    //auto grp_element = grp.first[gr1_len - 1];
    graph_list_concat ( templ.node_route, gr1.node_route, gr2.node_route, gr1_len, ret.node_route );
    //graph_list_concat ( templ.p_mark, gr1.p_mark, gr2.p_mark, grp_element[PI_P], ret.p_mark );
    //graph_list_concat ( templ.b_mark, gr1.b_mark, gr2.b_mark, grp_element[PI_B], ret.b_mark );
    // concat the sw part
    int ssw = mid_state.shop_stage;
    ret.shop_thr.resize ( ssw + 1 );
    const ev_entry shop_ev_type[3] = {EV_AT, EV_DF, EV_MF};
    ret.shop_thr[ssw].second = mid_state.ev[shop_ev_type[ret.shop_thr[ssw].first]];
    ret.shop_thr.insert ( ret.shop_thr.end(), gr2.shop_thr.begin(), gr2.shop_thr.end() );
    //concat the t part
    int st = mid_state.ev[EV_Tk];
    ret.shop_tb_tokens.resize ( st );
    if ( ( int ) ( gr2.shop_tb_tokens.size() ) > st )
    {
        ret.shop_tb_tokens.insert ( ret.shop_tb_tokens.end(), gr2.shop_tb_tokens.begin() + st, gr2.shop_tb_tokens.end() );
    }
    fast_psi_score ( gi, gs, ret );
    return ret;
}
