// =====================================================================================
//
//       Filename:  main.cpp
//
//    Description:  the tester for the ai_core
//
//        Version:  1.0
//        Created:  10/06/2016 03:17:53 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  thiner (),
//   Organization:
//
// =====================================================================================

#include "mtai/ai_core.h"
#include "mtai/ginterface.h"
#include "mtai/mtstat.h"

#include <stdio.h>
#include <string>
#include "mtai/eprintf.h"
#include <unistd.h>

#include <signal.h>

#include <cstdlib>
#include <ctime>

void interrupt_handler ( int signo )
{
    ( void ) signo;
    eprintf ( "Interrupted!\n" );
    interrupted = true;
}

int main()
{
    srand ( time ( 0 ) );
    char* buf = new char[1048576];
    int s = fread ( buf, 1, 1048576, stdin );
    ( void ) s;
    mtstat mts;
    mts.init_from_stream ( buf );
    alarm ( 29 * 60 );
    if ( signal ( SIGALRM, interrupt_handler ) == SIG_ERR )
    {
        eprintf ( "Failed to set the alarm handler!\n" );
    }
    if ( signal ( SIGINT, interrupt_handler ) == SIG_ERR )
    {
        eprintf ( "Failed to set the interrupt handler!\n" );
    }
    ginterface gi ( mts );
    ai_return_t ret = opt_from_beginning ( gi, AI_MED_4 ); //AI_MED_4 );
    for ( int i = 0, s = ret.el.size(); i < s; i++ )
    {
        unit_event ue = ret.el[i];
        eprintf ( "Event #%d: %d, %d, %d\n", i, ue.n, ue.e, ue.s );
    }
    std::string str = gi.translate ( ret.el );
    printf ( "[route]%s\n", str.c_str() );
    mts.process_from_stream ( str.c_str(), str.length() );
    printf ( "[score]%d\n", mts.score() );
    printf ( "[hp]%d\n", mts.brave_hp );
    //ai_return_t ret = opt_from_beginning ( gi );
    return 0;
}
