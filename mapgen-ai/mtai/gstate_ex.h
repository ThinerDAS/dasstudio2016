#ifndef GSTATE_EX_HEADER
#define GSTATE_EX_HEADER

#include "gstate.h"
#include "eprintf.h"

class gstate_ex : public gstate
{
public:
    //int shop_times; // This can not be deducted by a naked gstate, so when gstate is passed to the opt functions, the shop_times variable is permanently lost.
    int shop_stage;
    //int t_stage;

    gstate_ex() : gstate(), shop_stage ( 0 ) //, t_stage ( 0 )
    {}
    gstate_ex ( const gstate& gs ) : gstate ( gs ), shop_stage ( 0 ) //, t_stage ( 0 )
    {}
    gstate_ex ( gstate&& gs ) : gstate ( gs ), shop_stage ( 0 ) //, t_stage ( 0 )
    {}
    void shallow_copy ( const gstate_ex& gs )
    {
        ev = gs.ev;
        shop_stage = gs.shop_stage;
        //t_stage = gs.t_stage;
    }
};



#endif
