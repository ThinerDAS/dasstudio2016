#include "mapgen_engine.h"
#include "mtstat.h"

#include <cstdlib>
#include <ctime>

void seedrand ( int seed = -1 )
{
    if ( seed == -1 )
    {
        seed = time ( 0 );
    }
    srand ( seed );
}

void dump_mtstat ( const mtstat& mts )
{
}

int main()
{
    seedrand();
    mtstat initial;
    initial.brave_hp = 1000;
    mapgen_engine me ( initial );
    mtstat res = me.genmap ( 10, 100 );
    dump_mtstat ( res );
    return 0;
}
