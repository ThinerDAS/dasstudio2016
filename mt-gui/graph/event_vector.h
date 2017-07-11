#ifndef EVENT_VECTOR_HEADER
#define EVENT_VECTOR_HEADER

#include <valarray>

enum ev_entry
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
    EV_SALT,
    EV_SHOPTIMES,
    EV_NEGATIVE_ONETIME, // tile_ID
    EV_INTERNAL_FLAG,
    EV_COUNT
};

typedef std::valarray<int> event_vector;

bool easy_take(const event_vector& ev);

#endif
