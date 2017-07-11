#ifndef GSHEET_HEADER
#define GSHEET_HEADER

#include "mtstat.h"
#include "gtemplate.h"
#include "graph.h"

class gsheet
{
    const gtemplate* m_gt;
    
public:
    std::vector<int> doors;
    std::vector<int> mobs;
    std::vector<int> loots;
    //std::vector<mtstat::enemy> enemy_data;
    mutable graph::bools mod_bools;

    gsheet ( const gtemplate& gt );

    //graph filled_graph() const;

    inline const gtemplate& gt() const
    {
        return *m_gt;
    }
    inline const mtstat& initialmts() const
    {
        return m_gt->initialmts();
    }

    void set_mod_coord(int coord);

    mtstat output() const;
};

#endif
