#ifndef HEURISTIC_HEADER
#define HEURISTIC_HEADER

#include "ginterface.h"
#include "gstate.h"
#include "event_list.h"

class heuristic_return_t
{
public:
    double difficulty;
    unit_event most_probable;
};
heuristic_return_t choice_difficulty ( const ginterface& gi, gstate gs, unit_event ue );

#endif
