#ifndef GRAPH_HEADER
#define GRAPH_HEADER

//#include "mtstat.h"
#include "event_vector.h"

class gsheet;
//#include "gsheet.h"

#include <bitset>
#include <set>
#include <vector>
#include <array>

class graph;
class graph_big;

template <size_t WIDTH, size_t HEIGHT>
graph_big fromRectMap ( const std::array<std::array<int, WIDTH>, HEIGHT>& mapdata, const std::vector<event_vector>& tileEV ); //, int base_address = 0 );

struct category_mark
{
    int easy_offset;
    int easy_end;

    int w_offset;
    int w_end;

    int noneasy_offset; // noteasy covers the following three sections
    int noneasy_end;

    int door_offset; // invincible to p/b
    int door_end;

    int mob_offset;  // vulnerable to b (generally)
    int mob_end;

    int wall_offset; // vulnerable to p
    int wall_end;
};

//graph combineTwoGraph ( const graph& g1, const graph& g2 );

class graph_big
{
public:
    static const int MAX_GRAPH_NODES = 16777216;//16384;
    static const int DOOR_FLAG = 0x1000000;
    typedef std::set<int> bools;

private:
    std::vector<event_vector> m_vertices;
    std::vector<bools> m_adjacent_vector;
    std::vector<bools> m_trace;          // invariant from pack, but not other
    category_mark m_cm;

public:
    int addVertex ( event_vector ev );
    void updateVertex ( int v, event_vector ev );
    void addEdge ( int v1, int v2 );
    void removeVertex ( int v );
    void removeEdge ( int v1, int v2 );
    void contractVertex ( int v1, int v2 );
    void swapVertex ( int v1, int v2 );
    bool branchVertex ( int v );

    graph_big& operator+= ( const graph_big& g );
    const category_mark& pack ( int entry_v ); // Note that the indestructable monster are doors! not mobs who can be bombed!
    // Also note that the pack here do both reduce and compress!

    bools nextto ( const bools& taken ) const;

    inline const std::vector<event_vector>& vertices() const
    {
        return m_vertices;
    }
    inline const std::vector<bools>& adjacent_vector() const
    {
        return m_adjacent_vector;
    }
    inline const std::vector<bools>& trace() const
    {
        return m_trace;
    }
    inline const category_mark& cm() const
    {
        return m_cm;
    }

    graph shrink() const;

    template <size_t WIDTH, size_t HEIGHT>
    friend graph_big fromRectMap ( const std::array<std::array<int, WIDTH>, HEIGHT>& mapdata, const std::vector<event_vector>& tileEV );
    // base_address is 0 in some cases.
    //friend graph combineTwoGraph ( const graph& g1, const graph& g2 );
    // graph combining do modify the base address.
};

graph_big::bools operator& ( const graph_big::bools& b1, const graph_big::bools& b2 );

graph_big::bools operator| ( const graph_big::bools& b1, const graph_big::bools& b2 );

graph_big::bools& operator&= ( graph_big::bools& b1, const graph_big::bools& b2 );

graph_big::bools& operator|= ( graph_big::bools& b1, const graph_big::bools& b2 );

graph_big::bools& operator<<= ( graph_big::bools& b1, int d );

graph_big::bools& operator>>= ( graph_big::bools& b1, int d );

template <size_t WIDTH, size_t HEIGHT>
graph_big fromRectMap ( const std::array<std::array<int, WIDTH>, HEIGHT>& mapdata, const std::vector<event_vector>& tileEV )
{
    graph_big g;
    for ( int i = 0; i < ( int ) HEIGHT; i++ )
    {
        for ( int j = 0; j < ( int ) WIDTH; j++ )
        {
            int index = i * HEIGHT + j;
            g.addVertex ( tileEV[mapdata[i][j]] );
            if ( i > 0 )
            {
                g.addEdge ( index, index - WIDTH );
            }
            if ( j > 0 )
            {
                g.addEdge ( index, index - 1 );
            }
        }
    }
    return g;
}

class graph
{
public:
    static const int MAX_GRAPH_NODES = 4096;//16384;
    typedef std::bitset<MAX_GRAPH_NODES> bools;

private:
    std::vector<event_vector> m_vertices;
    std::vector<bools> m_adjacent_vector;
    std::vector<graph_big::bools> m_trace;          // invariant from pack, but not other
    category_mark m_cm;

public:
    bools nextto ( const bools& taken ) const;
    void update_gsheet_no_check ( const gsheet& gs, std::vector<event_vector>& tileEV );

    inline const std::vector<event_vector>& vertices() const
    {
        return m_vertices;
    }
    inline const std::vector<bools>& adjacent_vector() const
    {
        return m_adjacent_vector;
    }
    inline const std::vector<graph_big::bools>& trace() const
    {
        return m_trace;
    }
    inline const category_mark& cm() const
    {
        return m_cm;
    }

    friend class graph_big;
};

#endif
