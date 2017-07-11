#include <vector>
#include <array>
#include <cstdlib>
#include <cstdio>
#include "mtstat.h"
#include <string>

//TODO: 1. Test cases!! In order to avoid potential bugs.
//2. send as much data to stderr as possible for `their` debugging

/*Some clarafication here:
 * 1. About portals. There are some portal pairs in the mt. Here, to simplify the cases, we assume
 * that no two portal spots are at the same coordinate. So any ambiguity that where a multi-ended
 * portal should lead to is eliminated here. We also assume that portals are built upon empty tiles.
*/
mtstat::mtstat() : score_threshold{},
    brave_hp ( 0 ), brave_at ( 0 ), brave_df ( 0 ), brave_mf ( 0 ),
    brave_yellowkey ( 0 ), brave_bluekey ( 0 ), brave_redkey ( 0 ),
    brave_gold ( 0 ), brave_tokens ( 0 ), brave_pickaxe ( 0 ), brave_bomb ( 0 ),
    brave_x ( 0 ), brave_y ( 0 ), brave_floor ( 0 ),
    unit_hp ( 0 ), unit_at ( 0 ), unit_df ( 0 ), unit_mf ( 0 ),
    fanfare_flag ( 0 ), reg ( 0 )
{
}

static bool readint ( const char*& stream, int& dest, bool quiet = true )
{
    int pos = 0;
    int result = sscanf ( stream, "%d%n", &dest, &pos );
    if ( result == 0 && !quiet )
    {
        fputs ( "Read int failed!", stderr );
        exit ( 1 );
    }
    stream += pos;
    return result != 0;
}

bool mtstat::init_from_stream ( const char* stream )
{
    fputs ( "Reading from buffer to initialize mtstat...\n", stderr );
#define readint_rof(s,d) if(!readint(s,d)) {return false;}
    int total_maps, total_enemies, total_portals;
    for ( int i = 0; i < THRESHOLD_STAIRS; i++ )
    {
        readint_rof ( stream, score_threshold[i] );
    }
    readint_rof ( stream, brave_hp );
    readint_rof ( stream, brave_at );
    readint_rof ( stream, brave_df );
    readint_rof ( stream, brave_mf );
    readint_rof ( stream, brave_yellowkey );
    readint_rof ( stream, brave_bluekey );
    readint_rof ( stream, brave_redkey );
    readint_rof ( stream, brave_gold );
    readint_rof ( stream, brave_tokens );
    readint_rof ( stream, brave_pickaxe );
    readint_rof ( stream, brave_bomb );
    readint_rof ( stream, brave_x );
    readint_rof ( stream, brave_y );
    readint_rof ( stream, brave_floor );
    readint_rof ( stream, total_maps );
    readint_rof ( stream, total_enemies );
    readint_rof ( stream, total_portals );
    readint_rof ( stream, unit_hp );
    readint_rof ( stream, unit_at );
    readint_rof ( stream, unit_df );
    readint_rof ( stream, unit_mf );
    fanfare_flag = 0;
    reg = 0;
    maps = std::vector<map> ( total_maps );
    enemies = std::vector<enemy> ( total_enemies );
    portals = std::vector<portal_pair> ( total_portals );
    for ( int f = 0; f < total_maps; f++ )
    {
        for ( int i = 0; i < MAPSIZE; i++ )
        {
            for ( int j = 0; j < MAPSIZE; j++ )
            {
                int entry;
                readint_rof ( stream, entry );
                maps[f][i][j] = entry;
            }
        }
    }
    for ( int f = 0; f < total_enemies; f++ )
    {
        for ( int i = 0; i < 5; i++ )
        {
            int entry;
            readint_rof ( stream, entry );
            enemies[f][i] = entry;
        }
    }
    for ( int f = 0; f < total_portals; f++ )
    {
        for ( int i = 0; i < 2; i++ )
        {
            int entry;
            readint_rof ( stream, entry );
            portals[f][i] = entry;
        }
    }
    return true;
}

void push_number_to_string ( std::string& str, int i )
{
    char buffer[16];
    sprintf ( buffer, "%d ", i );
    str.append ( buffer );
}

void new_line ( std::string& str )
{
    while ( str.back() == ' ' )
    {
        str.pop_back();
    }
    str.push_back ( '\n' );
}

std::string mtstat::to_string()
{
    std::string ret;
    for ( int i = 0; i < THRESHOLD_STAIRS; i++ )
    {
        push_number_to_string ( ret, score_threshold[i] );
    }
    new_line ( ret );
    push_number_to_string ( ret, brave_hp );
    push_number_to_string ( ret, brave_at );
    push_number_to_string ( ret, brave_df );
    push_number_to_string ( ret, brave_mf );
    new_line ( ret );
    push_number_to_string ( ret, brave_yellowkey );
    push_number_to_string ( ret, brave_bluekey );
    push_number_to_string ( ret, brave_redkey );
    new_line ( ret );
    push_number_to_string ( ret, brave_gold );
    push_number_to_string ( ret, brave_tokens );
    push_number_to_string ( ret, brave_pickaxe );
    push_number_to_string ( ret, brave_bomb );
    new_line ( ret );
    push_number_to_string ( ret, brave_x );
    push_number_to_string ( ret, brave_y );
    push_number_to_string ( ret, brave_floor );
    new_line ( ret );
    push_number_to_string ( ret, maps.size() );
    push_number_to_string ( ret, enemies.size() );
    push_number_to_string ( ret, portals.size() );
    new_line ( ret );
    push_number_to_string ( ret, unit_hp );
    push_number_to_string ( ret, unit_at );
    push_number_to_string ( ret, unit_df );
    push_number_to_string ( ret, unit_mf );
    new_line ( ret );
    for ( int f = 0, s = maps.size(); f < s; f++ )
    {
        new_line ( ret );
        for ( int i = 0; i < MAPSIZE; i++ )
        {
            for ( int j = 0; j < MAPSIZE; j++ )
            {
                push_number_to_string ( ret, maps[f][i][j] );
            }
            new_line ( ret );
        }
    }
    new_line ( ret );
    for ( int f = 0, s = enemies.size(); f < s; f++ )
    {
        for ( int i = 0; i < 5; i++ )
        {
            push_number_to_string ( ret, enemies[f][i] );
        }
        new_line ( ret );
    }
    new_line ( ret );
    for ( int f = 0, s = portals.size(); f < s; f++ )
    {
        for ( int i = 0; i < 2; i++ )
        {
            push_number_to_string ( ret, portals[f][i] );
        }
        new_line ( ret );
    }
    return ret;
}

bool mtstat::process_from_stream ( const char* stream, size_t n )
{
    for ( size_t i = 0; i < n; i++ )
    {
        bool success = process_char ( stream[i] );
        if ( !success )
        {
            return false;
        }
    }
    return true;
}

int mtstat::dam ( int mon_ID ) const
{
    return dam ( brave_at, brave_df, brave_mf, enemies[mon_ID] );
}

int mtstat::dam ( int brave_at, int brave_df, int brave_mf, const enemy& e )
{
    int ehp = e[0], eat = e[1], edf = e[2], esp = e[4];
    int b_dam_pw = brave_at - edf;
    if ( b_dam_pw <= 0 )
    {
        return 0x3fffffff;
    }
    int e_dam_pw = eat - brave_df;
    if ( esp & Spec::MA )
    {
        e_dam_pw = eat;
    }
    if ( e_dam_pw <= 0 )
    {
        return 0;
    }
    if ( esp & Spec::SD )
    {
        b_dam_pw = 1;
    }
    if ( esp & Spec::CNMASK )
    {
        e_dam_pw *= ( esp & Spec::CNMASK ) + 1;
    }
    int e_attimes = ( ehp - 1 ) / b_dam_pw;
    if ( esp & Spec::SA )
    {
        e_attimes++;
    }
    long long result = e_attimes * e_dam_pw - brave_mf;
    if ( result > 0x3fffffff )
    {
        result = 0x3fffffff;
    }
    if ( result <= 0 )
    {
        return 0;
    }
    return ( int ) result;
}

bool mtstat::goto_coord ( int f, int x, int y )
{
    //return false only when the brave dies.
    //bool passable = true;
    bool taken = true;
    int event = maps[f][y][x];
    if ( event == Tile::YELDOOR )
    {
        if ( brave_yellowkey == 0 )
        {
            reg = 0;
            return true;//Not passable.
        }
        brave_yellowkey--;
    }
    else if ( event == Tile::BLUEDOOR )
    {
        if ( brave_bluekey == 0 )
        {
            reg = 0;
            return true;//Not passable.
        }
        brave_bluekey--;
    }
    else if ( event == Tile::REDDOOR )
    {
        if ( brave_redkey == 0 )
        {
            reg = 0;
            return true;//Not passable.
        }
        brave_redkey--;
    }
    else if ( event == Tile::WALL )
    {
        if ( brave_pickaxe == 0 || reg != 'p' )
            //Only when the pickaxe flag is set can a wall be picked
        {
            reg = 0;
            return true;//Not passable.
        }
        brave_pickaxe--;
    }
    else if ( event == Tile::SOLIDWALL )
    {
        reg = 0;
        return true;//Not passable, under any case.
    }
    else if ( event >= Tile::MOB )
    {
        int mon_ID = event - Tile::MOB;
        if ( reg == 'b' && brave_bomb > 0 && ! ( enemies[mon_ID][4]&Spec::BF ) )
        {
            //use bomb instead of direct battle here.
            brave_bomb--;
        }
        else
        {
            int damage = dam ( mon_ID );
            if ( brave_hp <= damage )
            {
                reg = 0;
                return true;//Not passable. Effectively avoided fatal cases.
            }
            brave_hp -= damage;
            brave_gold += enemies[mon_ID][3];
        }
    }
    else if ( event >= Tile::REDCRY && event < Tile::REDCRY + 8 )
    {
        int amp = event & 0x7;
        brave_at += unit_at << amp;
    }
    else if ( event >= Tile::BLUECRY && event < Tile::BLUECRY + 8 )
    {
        int amp = event & 0x7;
        brave_df += unit_df << amp;
    }
    else if ( event >= Tile::GREENCRY && event < Tile::GREENCRY + 8 )
    {
        int amp = event & 0x7;
        brave_mf += unit_mf << amp;
    }
    else if ( event >= Tile::WHITECRY && event < Tile::WHITECRY + 8 )
    {
        taken = false;
    }
    else if ( event >= Tile::POTION && event < Tile::POTION + 16 )
    {
        int amp = event - Tile::POTION;
        brave_hp += unit_hp << amp;
    }
    else
    {
        switch ( event )
        {
            case Tile::EMPTY:
                break;
            case Tile::YELKEY:
                brave_yellowkey++;
                break;
            case Tile::BLUEKEY:
                brave_bluekey++;
                break;
            case Tile::REDKEY:
                brave_redkey++;
                break;
            case Tile::GOLD:
                brave_gold += 100;
                break;
            case Tile::TOKEN:
                brave_tokens++;
                break;
            case Tile::PICKAXE:
                brave_pickaxe++;
                break;
            case Tile::BOMB:
                brave_bomb++;
                break;
            case Tile::FLAG:
                fanfare_flag++;
                break;
            default:
                fprintf ( stderr, "Unhandled map event, ID = %d\n", event );
                break;
        }
    }
    if ( taken )
    {
        maps[f][y][x] = Tile::EMPTY;
    }
    brave_floor = f;
    brave_x = x;
    brave_y = y;
    //clear regs!
    reg = 0;
    return true;
}

bool mtstat::process_char ( char action )
{
    //returns false when one action is fatal.
    switch ( action )
    {
        //direction keys
        case 'h':
            if ( brave_x > 0 )
            {
                return goto_coord ( brave_floor, brave_x - 1, brave_y );
            }
            break;
        case 'k':
            if ( brave_y > 0 )
            {
                return goto_coord ( brave_floor, brave_x, brave_y - 1 );
            }
            break;
        case 'j':
            if ( brave_y < MAPSIZE - 1 )
            {
                return goto_coord ( brave_floor, brave_x, brave_y + 1 );
            }
            break;
        case 'l':
            if ( brave_x < MAPSIZE - 1 )
            {
                return goto_coord ( brave_floor, brave_x + 1, brave_y );
            }
            break;
        //go into portal (you guessed it, you can just go past a portal without stepping into it!)
        case 'g':
        {
            int src = brave_floor * MAPSIZE * MAPSIZE + brave_y * MAPSIZE + brave_x;
            for ( int i = 0, s = portals.size(); i < s; i++ )
            {
                int dest = -1;
                const portal_pair& pp = portals[i];
                if ( pp[0] == src )
                {
                    dest = pp[1];
                }
                else if ( pp[1] == src )
                {
                    dest = pp[0];
                }
                if ( dest != -1 )
                {
                    return goto_coord ( dest / ( MAPSIZE * MAPSIZE ), dest % MAPSIZE, ( dest / MAPSIZE ) % MAPSIZE );
                    break;
                }
            }
        }
        break;
        //choose upgrading path
        case 'a':
        case 'd':
        case 'm':
        {
            if ( reg == 's' )
            {
                if ( brave_gold >= 100 )
                {
                    brave_gold -= 100;
                    switch ( action )
                    {
                        case 'a':
                            brave_at += unit_at << brave_tokens;
                            break;
                        case 'd':
                            brave_df += unit_df << brave_tokens;
                            break;
                        case 'm':
                            brave_mf += unit_mf << brave_tokens;
                            break;
                    }
                }
            }
            else if ( ( maps[brave_floor][brave_y][brave_x] - Tile::WHITECRY ) / 8 == 0 )
            {
                int amp = maps[brave_floor][brave_y][brave_x] - Tile::WHITECRY;
                switch ( action )
                {
                    case 'a':
                        brave_at += unit_at << amp;
                        break;
                    case 'd':
                        brave_df += unit_df << amp;
                        break;
                    case 'm':
                        brave_mf += unit_mf << amp;
                        break;
                }
                maps[brave_floor][brave_y][brave_x] = Tile::EMPTY;
            }
        }
        break;
        //use items
        case 'p':
            if ( brave_pickaxe > 0 )
            {
                reg = 'p';
            }
            break;
        case 'b':
            if ( brave_bomb > 0 )
            {
                reg = 'b';
            }
            break;
        case 's':
            if ( brave_gold >= 100 )
            {
                reg = 's';
            }
            break;
    }
    return true;
}

int mtstat::score() const
{
    int stair_scores[] = {0, 200, 400, 600, 800, 1000};
    if ( fanfare_flag )
    {
        long raw_score = brave_hp;
        if ( raw_score > score_threshold[THRESHOLD_STAIRS - 1] )
        {
            return stair_scores[THRESHOLD_STAIRS - 1];
        }
        for ( int i = THRESHOLD_STAIRS - 2; i >= 0; i-- )
        {
            if ( raw_score > score_threshold[i] )
            {
                long a1 = stair_scores[i], a2 = stair_scores[i + 1];
                long b1 = score_threshold[i], b2 = score_threshold[i + 1];
                return a1 + ( a2 - a1 ) * ( raw_score - b1 ) / ( b2 - b1 );
            }
        }
    }
    return 0;
}

/*

void mtstat::drawTile1 ( int event ) const
{
    printf ( "%3d|", event );
}

void mtstat::drawTile2 ( int event ) const
{
    if ( event == Tile::EMPTY )
    {
        printf ( "    " );
    }
    else if ( ( event & 0xf8 ) == Tile::REDCRY )
    {
        printf ( "\e[31m\e[5mA%3d\e[25m\e[39m", unit_at << ( event & 0x7 ) );
    }
    else if ( ( event & 0xf8 ) == Tile::BLUECRY )
    {
        printf ( "\e[34m\e[5mD%3d\e[25m\e[39m", unit_df << ( event & 0x7 ) );
    }
    else if ( ( event & 0xf8 ) == Tile::GREENCRY )
    {
        printf ( "\e[32m\e[5mM%3d\e[25m\e[39m", unit_at << ( event & 0x7 ) );
    }
    else if ( ( event & 0xf8 ) == Tile::WHITECRY )
    {
        printf ( "\e[5mx%3d\e[25m", 1 << ( event & 0x7 ) );
    }
    else if ( event >= Tile::POTION && event < Tile::POTION + 16 )
    {
        printf ( "\e[32m\e[4mHPv%x\e[24m\e[39m", event - Tile::POTION );
    }
    else if ( event == Tile::YELKEY )
    {
        printf ( "\e[33m\e[4mYKEY\e[24m\e[39m" );
    }
    else if ( event == Tile::BLUEKEY )
    {
        printf ( "\e[34m\e[4mBKEY\e[4m\e[39m" );
    }
    else if ( event == Tile::REDKEY )
    {
        printf ( "\e[31m\e[4mRKEY\e[24m\e[39m" );
    }
    else if ( event == Tile::GOLD )
    {
        printf ( "\e[33m\e[1mGOLD\e[21m\e[39m" );
    }
    else if ( event == Tile::TOKEN )
    {
        printf ( "\e[34m\e[1mTOKS\e[21m\e[39m" );
    }
    else if ( event == Tile::PICKAXE )
    {
        printf ( "\e[37m\e[1mPICK\e[21m\e[39m" );
    }
    else if ( event == Tile::BOMB )
    {
        printf ( "\e[37m\e[1mBOMB\e[21m\e[39m" );
    }
    else if ( event == Tile::FLAG )
    {
        printf ( "\e[1m\e[4mFLAG\e[24m\e[21m" );
    }
    else if ( event == Tile::YELDOOR )
    {
        printf ( "\e[43m\e[1mY DR\e[21m\e[49m" );
    }
    else if ( event == Tile::BLUEDOOR )
    {
        printf ( "\e[44m\e[1mB DR\e[21m\e[49m" );
    }
    else if ( event == Tile::REDDOOR )
    {
        printf ( "\e[41m\e[1mR DR\e[21m\e[49m" );
    }
    else if ( event == Tile::WALL )
    {
        printf ( "\e[47m\e[32mwall\e[39m\e[49m" );
    }
    else if ( event == Tile::SOLIDWALL )
    {
        printf ( "\e[47m\e[31m\e[1mWAXL\e[21m\e[39m\e[49m" );
    }
    else if ( event >= Tile::MOB )
    {
        printf ( "\e[42m\e[1mMB%2d\e[21m\e[49m", event - Tile::MOB );
    }
    else
    {
        printf ( "????" );
    }
}

void mtstat::drawOnConsole() const
{
    system ( "clear" );
    printf ( "HP: %8d AT: %8d DF: %8d MF: %8d \n\n"
             "G: %8d T: %2d YK: %2d BK: %2d RK: %2d P: %2d B: %2d\n\n"
             "X: %2d Y: %2d FLOOR: %2d U: (%4d, %4d, %4d, %4d) FFF: %d reg='%c'\n\n"
             "Score: %4d\n",
             brave_hp, brave_at, brave_df, brave_mf, brave_gold, brave_tokens, brave_yellowkey, brave_bluekey, brave_redkey, brave_pickaxe, brave_bomb, brave_x, brave_y, brave_floor, unit_hp, unit_at, unit_df, unit_mf, fanfare_flag, reg, score() );
    for ( int i = 0; i < MAPSIZE; i++ )
    {
        for ( int j = 0; j < MAPSIZE; j++ )
        {
            if ( i == brave_y && j == brave_x )
            {
                printf ( "\e[7m" );
            }
            drawTile1 ( maps[brave_floor][i][j] );
            if ( i == brave_y && j == brave_x )
            {
                printf ( "\e[27m" );
            }
        }
        puts ( "" );
        for ( int j = 0; j < MAPSIZE; j++ )
        {
            if ( i == brave_y && j == brave_x )
            {
                printf ( "\e[7m" );
            }
            drawTile2 ( maps[brave_floor][i][j] );
            if ( i == brave_y && j == brave_x )
            {
                printf ( "\e[27m" );
            }
        }
        puts ( "" );
    }
    for ( int i = 0, s = enemies.size(); i < s; i++ )
    {
        printf ( "Mob %2d : HP %8d AT %8d DF %8d \n G %4d Spec. %4x Dam \e[31m\e[1m%8d\e[21m\e[39m \n\n", i, enemies[i][0], enemies[i][1], enemies[i][2], enemies[i][3], enemies[i][4], dam ( i ) );
    }
}

*/
