#ifndef MAPGEN_ENGINE_HEADER
#define MAPGEN_ENGINE_HEADER

#include "mtstat.h"

//mtstat genmap ( int difficulty_lower, int difficulty_upper, const mtstat& initial );

class mapgen_engine
{
    mtstat m_initial;
public:

    mapgen_engine ( mtstat initial );
    mtstat genmap ( int dif_lower_bound, int dif_upper_bound ) const;
    inline const mtstat& initial() const
    {
        return m_initial;
    }
};

#endif
