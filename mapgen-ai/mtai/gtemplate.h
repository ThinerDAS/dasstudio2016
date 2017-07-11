#ifndef GTEMPLATE_HEADER
#define GTEMPLATE_HEADER

#define BASIC

#include "mtstat.h"
#include <vector>
#include "graph.h"

class gtemplate
{
    const mtstat* m_initial_mts;

    std::vector<int> m_doors_coordinate;
    std::vector<int> m_mobs_coordinate;
    std::vector<int> m_loots_coordinate;

    std::vector<int> m_node_map;

public:

    enum sym
    {
        SYM_EMPTY,
        SYM_WALL,
        SYM_DOOR,
        SYM_MOB,
        SYM_LOOT,
        SYM_COUNT
    };

    gtemplate ( const mtstat& mts );

    static bool spec ( int coord )
    {
#ifdef BASIC
        return false;
#endif
        // check whether it is a SYM_LOOT with an w.
        coord ^= coord << 1;
        return ( coord % 7 + coord % 11 + coord % 13 ) % 8 == 0;
    }

    inline const mtstat& initialmts() const
    {
        return *m_initial_mts;
    }
    inline const std::vector<int>& doors_coordinate() const
    {
        return m_doors_coordinate;
    }
    inline const std::vector<int>& mobs_coordinate() const
    {
        return m_mobs_coordinate;
    }
    inline const std::vector<int>& loots_coordinate() const
    {
        return m_loots_coordinate;
    }

    inline const std::vector<int>& node_map() const
    {
        return m_node_map;
    }

    void set_node_map ( const graph& g );

};

#endif
