#ifndef GSTATE_HEADER
#define GSTATE_HEADER

#include "graph.h"
#include "event_vector.h"

class gstate
{
public:
    event_vector ev;
    graph::bools taken;
    graph::bools reach;
};

#endif
