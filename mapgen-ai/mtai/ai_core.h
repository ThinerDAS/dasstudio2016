#ifndef AI_CORE_HEADER
#define AI_CORE_HEADER

// Several optimization functions that operates on routes.

#include "ginterface.h"
#include "groute.h"
//#include "groute_proof.h"
#include "event_list.h"
#include "gstate.h"
#include "gstate_ex.h"

enum ai_level
{
    AI_RAND,  // no better than random
    AI_EASY,
    AI_EASY_1,
    AI_EASY_2,
    AI_EASY_3,
    AI_EASY_4,
    AI_MED,   // medium
    AI_MED_1,
    AI_MED_2,
    AI_MED_3,
    AI_MED_4,
    AI_AD,    // advanced
    AI_AD_1,
    AI_AD_2,
    AI_AD_3,
    AI_AD_4,
    AI_EXP,   // expert
    AI_EXP_1,
    AI_EXP_2,
    AI_EXP_3,
    AI_EXP_4,
    AI_EXTR,  // extreme
    AI_EXTR_1,
    AI_EXTR_2,
    AI_EXTR_3,
    AI_EXTR_4,
    AI_GOD,   // god
    AI_LEVELS
};

extern bool interrupted;

struct ai_return_t
{
    long val;
    event_list el;
    groute gr;
};

ai_return_t opt_from_beginning ( const ginterface& gi, ai_level lv = AI_MED );

ai_return_t opt_from_middle ( const ginterface& gi, const gstate& gs, ai_level lv = AI_MED );

long ai_difficulty(const ginterface& gi, const gstate& gs, const groute& gr);

ai_return_t translate ( const ginterface& gi, const gstate_ex& gs, const groute& gr );

#endif
