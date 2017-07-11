#ifndef GSHEET_FILLER_HEADER
#define GSHEET_FILLER_HEADER

#include "mtstat.h"
#include "gsheet.h"

class gsheet_filler
{
    mtstat m_mts;
    bool m_basic;
    std::vector<std::vector<int>> w_choices;
    std::vector<std::vector<int>> door_choices;
    std::vector<std::vector<int>> mob_choices;
    std::vector<std::vector<int>> loot_choices;
public:
    gsheet_filler ( const mtstat& mts, bool basic = false );
    void set_mob_range ( int floor_begin, int floor_end, int mob_begin, int mob_end );
    void set_loot_range ( int floor_begin, int floor_end, int loot_begin, int loot_end );
    void random_fill_gsheet ( gsheet& gs ) const;
    void random_mutate_gsheet ( gsheet& gs, int mutate_points = 1 ) const;
};

#endif
