#include "edict.h"
#include "zlibext.h"
#include "base64.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool b64_decode ( const std::string& in, std::string& out )
{
    int dec_len = Base64decode_len ( in.c_str() );
    char* buf_out = ( char* ) malloc ( dec_len + 4 );
    Base64decode ( buf_out, in.c_str() );
    out = std::string ( buf_out, dec_len - 1 );
    free ( buf_out );
    return true;
}

static bool b64_encode ( const std::string& in, std::string& out )
{
    int enc_len = Base64encode_len ( in.length() );
    char* buf_out = ( char* ) malloc ( enc_len + 4 );
    Base64encode ( buf_out, in.c_str(), in.length() );
    out = std::string ( buf_out, enc_len - 1 );
    free ( buf_out );
    return true;
}

static bool shift_string ( std::string& base, std::string& ret )
{
    int32_t cc;
    const char* base_c = base.c_str();
    cc = * ( const int* ) base_c;
    if ( base.length() < size_t ( 4 + cc ) )
    {
        return false;
    }
    ret = base.substr ( 4, cc );
    base.erase ( 0, cc + 4 );
    return true;
}

static bool push_string ( std::string& base, const std::string& srcstr )
{
    int32_t cc;
    cc = srcstr.length();
    base.append ( ( char* ) &cc, 4 );
    base += srcstr;
    return true;
}

edict::edict()
{
}

edict::edict ( const std::string& str )
{
    if ( str.empty() )
    {
        return;
    }
    std::string key, val, cur_str;
    b64_decode ( str, cur_str );
    cur_str = decompress ( cur_str );
    while ( cur_str.length() != 0 )
    {
        if ( shift_string ( cur_str, key ) && shift_string ( cur_str, val ) )
        {
            operator[] ( key ) = val;
        }
        else
        {
            fprintf ( stderr, "edict init error!\n" );
            return;
        }
    }
}

std::string edict::tostring() const
{
    std::string result;
    for ( auto& p : *this )
    {
        push_string ( result, p.first );
        push_string ( result, p.second );
    }
    std::string zl = compress ( result );
    b64_encode ( compress ( result ), result );
    return result;
}

