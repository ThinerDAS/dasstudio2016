#include "zlibext.h"
#include <string>

#include <stdio.h>
/* For "exit". */
#include <stdlib.h>
/* For "strerror". */
#include <string.h>
/* For "errno". */
#include <errno.h>
#include <zlib.h>

/* CHUNK is the size of the memory chunk used by the zlib routines. */

#define CHUNK 0x4000//00

/* The following macro calls a zlib routine and checks the return
   value. If the return value ("status") is not OK, it prints an error
   message and exits the program. Zlib's error statuses are all less
   than zero. */

#define CALL_ZLIB(x) {                                                  \
        int status;                                                     \
        status = x;                                                     \
        if (status < 0 && status != Z_BUF_ERROR) {                      \
            fprintf (stderr,                                            \
                     "%s:%d: %s returned a bad status of %d.\n",        \
                     __FILE__, __LINE__, #x, status);                   \
            exit (EXIT_FAILURE);                                        \
        }                                                               \
    }

/* These are parameters to inflateInit2. See
   http://zlib.net/manual.html for the exact meanings. */

#define windowBits 15
#define ENABLE_ZLIB_GZIP 32


std::string decompress ( const std::string in )
{
    std::string out;
    const char* in_c = in.c_str();
    size_t in_len = in.length();
    z_stream strm = {};
    unsigned char* in_buf = new unsigned char[CHUNK];
    unsigned char* out_buf = new unsigned char[CHUNK];
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = in_buf;
    strm.avail_in = 0;
    CALL_ZLIB ( inflateInit2 ( &strm, windowBits | ENABLE_ZLIB_GZIP ) );
    size_t chunks = ( in_len + CHUNK - 1 ) / CHUNK;
    const char* current_inptr = in_c;
    for ( size_t i = 0; i < chunks; i++ )
    {
        int chunk_size = ( i == chunks - 1 ) ? in_len % CHUNK : CHUNK;
        memcpy ( in_buf, current_inptr, chunk_size );
        strm.avail_in = chunk_size;
        strm.next_in = in_buf;
        do
        {
            size_t have;
            strm.avail_out = CHUNK;
            strm.next_out = out_buf;
            CALL_ZLIB ( inflate ( &strm, Z_NO_FLUSH ) );
            have = CHUNK - strm.avail_out;
            out.append ( reinterpret_cast<const char*> ( out_buf ), have );
        }
        while ( strm.avail_out == 0 );
        current_inptr += CHUNK;
    }
    inflateEnd ( &strm );
    delete[] in_buf;
    delete[] out_buf;
    return out;
}

std::string compress ( const std::string in )
{
    std::string out;
    const char* in_c = in.c_str();
    size_t in_len = in.length();
    z_stream strm = {};
    unsigned char* in_buf = new unsigned char[CHUNK];
    unsigned char* out_buf = new unsigned char[CHUNK];
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = in_buf;
    strm.avail_in = 0;
    CALL_ZLIB ( deflateInit2 ( &strm, 9, Z_DEFLATED, windowBits, 9, Z_DEFAULT_STRATEGY ) );
    size_t chunks = ( in_len + CHUNK - 1 ) / CHUNK;
    const char* current_inptr = in_c;
    for ( size_t i = 0; i < chunks; i++ )
    {
        int chunk_size = ( i == chunks - 1 ) ? in_len % CHUNK : CHUNK;
        memcpy ( in_buf, current_inptr, chunk_size );
        strm.avail_in = chunk_size;
        strm.next_in = in_buf;
        do
        {
            size_t have;
            strm.avail_out = CHUNK;
            strm.next_out = out_buf;
            int flush = Z_NO_FLUSH;
            if ( i == chunks - 1 )
            {
                flush = Z_FINISH;
            }
            CALL_ZLIB ( deflate ( &strm, flush ) );
            have = CHUNK - strm.avail_out;
            out.append ( reinterpret_cast<const char*> ( out_buf ), have );
        }
        while ( strm.avail_out == 0 );
        current_inptr += CHUNK;
    }
    deflateEnd ( &strm );
    delete[] in_buf;
    delete[] out_buf;
    return out;
}
