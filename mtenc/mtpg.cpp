/// Magic tower based passphrase generator!

#include "mtstat.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // rand, srand, atoi
#include <time.h>   // time(0)

#include <set>

#include <string>

const int MAX_BUF = 1048576;

int read_to_buffer ( char* buf, int maxread, FILE* f )
{
    int s = fread ( buf, 1, maxread, f );
    if ( s == maxread )
    {
        buf[maxread] = 0;
    }
    return s;
}

int main ( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        fprintf ( stderr, "Usage: %s [mtfile] [route_file]\n", argv[0] );
        return -1;
    }
    FILE* mtfile = fopen ( argv[1], "rb" );
    if ( !mtfile )
    {
        fprintf ( stderr, "Failed to open the mtfile!\n" );
        return -1;
    }
    FILE* route_file = fopen ( argv[2], "rb" );
    if ( !route_file )
    {
        fprintf ( stderr, "Failed to open the route file!\n" );
        return -1;
    }
    char* mtbuf = new char[MAX_BUF];
    char* rtbuf = new char[MAX_BUF];
    read_to_buffer ( mtbuf, MAX_BUF, mtfile );
    fclose ( mtfile );
    read_to_buffer ( rtbuf, MAX_BUF, route_file );
    fclose ( route_file );
    mtstat mts;
    mts.init_from_stream ( mtbuf );
    std::multiset<int> dam_set;
    int rlen = strlen ( rtbuf );
    for ( int i = 0; i < rlen; i++ )
    {
        int last_hp = mts.brave_hp;
        mts.process_char ( rtbuf[i] );
        int diff_hp = last_hp - mts.brave_hp;
        if ( diff_hp > 0 )
        {
            dam_set.insert ( diff_hp );
        }
    }
    std::string passphrase;
    //int sum = 0;
    char buf[16];
    for ( int i : dam_set )
    {
        sprintf ( buf, "%d-", i );
        passphrase.append ( buf );
        //sum += i;
    }
    //fprintf ( stderr, "sum=%d\n", sum );
    if ( passphrase.length() == 0 )
    {
        fprintf ( stderr, "No damage is taken! That is not a good mt!\n" );
        return -1;
    }
    passphrase.pop_back();
    int score = mts.score();
    if ( score < 800 )
    {
        fprintf ( stderr, "Not a good route enough for a effective encryption! You need to repolish your route or modify the threshold of the route!\nYour score: %d\n", score );
        return -1;
    }
    printf ( "%s", passphrase.c_str() );
    return 0;
}
