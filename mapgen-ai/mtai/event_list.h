#ifndef EVENT_LIST_HEADER
#define EVENT_LIST_HEADER

#include <vector>

class unit_event;

typedef std::vector<unit_event> event_list;

class unit_event
{
public:
    enum event_type
    {
        EVENT_NORMAL,
        EVENT_P,
        EVENT_B,
        EVENT_S,
        EVENT_W,
        EVENT_COUNT
    };

    enum shop_type
    {
        SHOP_AT,
        SHOP_DF,
        SHOP_MF,
        SHOP_T_COUNT
    };

    typedef int node_id_t;

    node_id_t n;
    event_type e;
    shop_type s;

    unit_event (
        node_id_t n = -1 ,
        event_type e = EVENT_NORMAL,
        shop_type s = SHOP_AT
    )
        : n ( n ), e ( e ), s ( s ) {}
};

#endif
