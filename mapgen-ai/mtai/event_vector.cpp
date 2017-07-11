/*
 * Auto generate by h2cppx
 */


#include "event_vector.h"

bool easy_take ( const event_vector& ev )
{
    static ev_entry positive[] =
    {
        EV_HP,
        EV_AT,
        EV_DF,
        EV_MF,
        EV_G,
        EV_W,
        EV_YK,
        EV_BK,
        EV_RK,
        EV_Px,
        EV_Bb,
        EV_Tk,
        EV_FLAG,
    },
    neutral[] =
    {
        EV_NEGATIVE_ONETIME,
    };
    for ( int i = 0, s = sizeof ( positive ) / sizeof ( positive[0] );
            i < s; i++ )
    {
        if ( ev[positive[i]] < 0 )
        {
            return false;
        }
    }
    for ( int i = 0, s = sizeof ( neutral ) / sizeof ( neutral[0] );
            i < s; i++ )
    {
        if ( ev[neutral[i]] != 0 )
        {
            return false;
        }
    }
    return true;
}

