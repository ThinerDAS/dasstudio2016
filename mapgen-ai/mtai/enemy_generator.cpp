/*
 * Auto generate by h2cppx
 */

#include "mtstat.h"
#include "enemy_generator.h"
#include <cstdlib>

// TODO: to test a generate mob to make sure that it will end finally.

static const int STANDARD_SCALE = 64;

static inline int min ( int a, int b )
{
    return a > b ? b : a;
}
/*
static inline int max ( int a, int b )
{
    return a > b ? a : b;
}
*/
static int score ( mtstat::enemy e )
{
    if ( e[4]&mtstat::SD )
    {
        e[0] /= STANDARD_SCALE;
    }
    int d[8];
    const int BASE_DMG = STANDARD_SCALE * STANDARD_SCALE;
    d[0] = mtstat::dam ( 260, 0, 0, e );
    d[1] = mtstat::dam ( 180, 80, 0, e );
    d[2] = mtstat::dam ( 100, 160, 0, e );
    d[3] = mtstat::dam ( 120, 80, 0, e );
    d[4] = mtstat::dam ( 220, 0, BASE_DMG / 2, e );
    d[5] = mtstat::dam ( 140, 80, BASE_DMG / 2, e );
    d[6] = mtstat::dam ( 100, 120, BASE_DMG / 2, e );
    d[7] = mtstat::dam ( 120, 80, BASE_DMG / 2, e );
    int zeros = 0, infs = 0;
    long sum = 0;
    for ( int i = 0; i < 8; i++ )
    {
        if ( d[i] == 0 )
        {
            zeros++;
        }
        if ( d[i] > BASE_DMG * 3 )
        {
            infs++;
        }
        if ( zeros + infs > 4 )
        {
            return 0;
        }
        sum += min ( d[i], BASE_DMG * 3 );
    }
    return sum / 8; //+16*(e[4]&mtstat::BF);
}

static int rand_spec()
{
    int ret = 0;
    int rd;
    rd = rand();
    if ( rd % 8 == 0 )
    {
        rd /= 8;
        if ( rd % 4 == 3 )
        {
            rd /= 4;
            ret |= rd % 16;
        }
        else
        {
            ret |= rd % 4;
        }
    }
    rd = rand();
    if ( rd % 16 == 0 )
    {
        ret |= mtstat::MA;
    }
    rd /= 16;
    if ( rd % 16 == 0 )
    {
        ret |= mtstat::SD;
    }
    rd /= 16;
    if ( rd % 16 == 0 )
    {
        ret |= mtstat::SA;
    }
    rd /= 16;
    if ( rd % 16 == 0 )
    {
        ret |= mtstat::BF;
    }
    return ret;
}

static mtstat::enemy generate_standard_enemy()
{
    mtstat::enemy ret;
    ret[0] = rand() % 8192 + 256;
    ret[1] = rand() % 128 + 32;
    ret[2] = rand() % 128 + 0;
    ret[4] = 0;
    const int BASE_DMG = STANDARD_SCALE * STANDARD_SCALE;
    const int STANDARD_SCORE = BASE_DMG, STANDARD_DIFF = STANDARD_SCALE * min ( 4, STANDARD_SCALE );
    int min_diff = abs ( score ( ret ) - STANDARD_SCORE );
    for ( int t = 0; t < 1024; t++ )
    {
        if ( min_diff < STANDARD_DIFF )
        {
            break;
        }
        mtstat::enemy cur = ret;
        int rd = rand();
        int rc = rd % 4;
        int rb = 256, rr = 1024;
        switch ( rc )
        {
            case 0:
            {
                rb = 256;
                rr = 1024;
            }
            break;
            case 1:
            {
                rb = 32;
                rr = 128;
            }
            break;
            case 2:
            {
                rb = 0;
                rr = 128;
            }
            break;
            default:
                rc = 4;
                break;
        }
        {
            rd /= 4;
            if ( rd % 4 == 0 )
            {
                rd /= 4;
                rr <<= ( 1 + rd % 2 );
            }
            int best_val = -1, best_diff = min_diff;
            for ( int i = 0; i < 4; i++ )
            {
                int cur_val = ( rc == 4 ) ? rand_spec() : ( rand() % rr + rb );
                cur[rc] = cur_val;
                int cur_diff = abs ( score ( cur ) - STANDARD_SCORE );
                if ( cur_diff < min_diff )
                {
                    best_diff = cur_diff;
                    best_val = cur_val;
                }
            }
            if ( best_val != -1 )
            {
                ret[rc] = best_val;
                min_diff = best_diff;
            }
        }
    }
    if ( ret[4]&mtstat::SD )
    {
        ret[0] /= STANDARD_SCALE;
    }
    return ret;
}

mtstat::enemy generate_enemy ( int lv )
{
    mtstat::enemy ret = generate_standard_enemy();
    ret[0] = ( long ) ret[0] * lv / STANDARD_SCALE + 5;
    ret[1] = ( long ) ret[1] * lv / STANDARD_SCALE + 1;
    ret[2] = ( long ) ret[2] * lv / STANDARD_SCALE;
    ret[3] = lv;
    return ret;
}

