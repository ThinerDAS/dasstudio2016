#ifndef MAP_TEMPLATE_GENERATOR_HEADER
#define MAP_TEMPLATE_GENERATOR_HEADER

#include "mtstat.h"
#include <vector>

struct genmaptempl_return_t
{
    std::vector<mtstat::map> maps;
    std::vector<mtstat::portal_pair> portals;
};

genmaptempl_return_t generate_map_template ( int floors, int orig_coord, std::vector<mtstat::map> restriction = std::vector<mtstat::map>() );

#endif
