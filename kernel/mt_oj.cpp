#include "mtstat.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // rand, srand, atoi
#include <time.h>   // time(0)

#include <unistd.h> // alarm
#include <signal.h>

int score = 0;

void alarm_handler ( int signo )
{
    ( void ) signo;
    fprintf ( stderr, "SCORE:[%d]\n", score );
    exit ( 0 );
}

int main ( int argc, char* argv[] )
{
    srand ( time ( 0 ) );
    if ( argc != 2 )
    {
        //ARG_FAIL:
        fprintf ( stderr, "Usage: %s [mtfile]\n", argv[0] );
        return -1;
    }
    FILE* mtfile = fopen ( argv[1], "rb" );
    char* buf = new char[1048576];
    //int s =
    fread ( buf, 1, 1048576, mtfile );
    fclose ( mtfile );
    mtstat mts;
    mts.init_from_stream ( buf );
    printf ( "%s", buf );
    alarm ( 60 * 30 );
    if ( signal ( SIGALRM, alarm_handler ) == SIG_ERR )
    {
        fprintf ( stderr, "Failed to set the alarm!\n" );
        return -1;
    }
    // The real topic.
    scanf ( "%1048576s", buf );
    alarm ( 60 );
    mts.process_from_stream ( buf, strlen ( buf ) );
    score = mts.score();
    printf ( "Your score: %d. Congratulations~\n", score );
    fprintf ( stderr, "SCORE:[%d]\n", score );
    return 0;
}
