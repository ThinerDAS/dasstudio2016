#include "gsheet_score.h"
#include "gsheet.h"
#include "graph.h"
#include "ai_core.h"
//#include "groute.h"
//#include "groute_proof.h"
#include "event_list.h"
#include "heuristic.h" //take in the whole state and bring out the eval of the crux? what is the input data type?

#include "event_vector.h"

#include "eprintf.h"

#include <cstdio>

double score ( const gsheet& gs )
{
    ginterface gi ( gs );
    ai_return_t ar = opt_from_beginning ( gi, AI_MED_2 );
    double total_score = 0.0;
    gstate cur_gs = gi.new_initial_state();
    event_list& el ( ar.el );
    for ( unit_event ue : el )
    {
        double partial_score = 0;
        double diff = 0.0;
        int val = 0;
        heuristic_return_t hr = choice_difficulty ( gi, cur_gs, ue );
        unit_event hrue = hr.most_probable;
        diff = hr.difficulty;
        gstate branch_gs ( cur_gs );
        gi.propagate ( branch_gs, hrue );
        ai_return_t arc = opt_from_middle ( gi, branch_gs, AI_EASY_2 );
        val = ar.val - arc.val;
        if ( val < 0 )
        {
            eprintf ( "gsheet_score: potential weakness of the AI? we find a better solution from a branch compared to the solution given by the complete AI scheme." );
            val = 0;
        }
        gi.propagate ( cur_gs, ue );
        partial_score = val * diff;
        total_score += partial_score;
        eprintf ( "gsheet_score: partial score for event [%d, %d, %d]: %f * %f = %f\n", ue.e, ue.s, ue.n, val, diff, partial_score );
    }
    return total_score;
}
