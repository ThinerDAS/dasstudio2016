#ifndef GINTERFACE_HEADER
#define GINTERFACE_HEADER

#include "gsheet.h"
#include "gstate.h"
#include "graph.h"
//#include "groute.h"
#include "event_list.h"

#include <string>

//This class only implement the functionality of propagating an state of mt,
//not recording the route. Recording part should be finished by the users
//of the interface.

class ginterface
{
    gstate m_initial;
    graph m_internal_graph;
    const mtstat* m_mts;
    const gtemplate* m_gt;
    std::vector<event_vector> tileEV;
public:
    static const long SALT_FISH_UNIT = 16;

    static std::vector<event_vector> generate_tileEV ( const mtstat& mts );

    ginterface ( const gtemplate& gt );
    ginterface ( const mtstat& mts, bool template_map = false );

    void update_gsheet(const gsheet& gs);

    gstate new_initial_state() const;

    bool accessible ( const gstate& gs, unit_event ue ) const;
    bool propagate ( gstate& gs, unit_event ue ) const;
    bool propagate_no_check ( gstate& gs, unit_event ue ) const;
    bool propagate ( gstate& gs, const event_list& el ) const;

    std::string translate ( const event_list& el ) const;

    static long score ( const gstate& gstate );
    //event_list propagate ( gstate& gstate, const groute& gr ) const;
    // Side effect are produced after a node. Note that the side effect are
    // divided by nodes, through a "trivial" side-effect - node itself.

    inline const graph& int_graph() const
    {
        return m_internal_graph;
    }
    inline const gtemplate& gt() const
    {
        return *m_gt;
    }
    inline const mtstat& initialmts() const
    {
        //return m_gt->initialmts();
        return *m_mts;
    }

    graph::bools nextto ( const graph::bools& taken ) const;
};

#endif
