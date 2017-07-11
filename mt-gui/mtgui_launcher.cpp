#include "mtgui.h"
#include "assets.h"
#include <cstdio>

mtgui mtg;

#ifdef __EMSCRIPTEN__
void em_enterFrame()
{
    mtg.enterFrame();
}
#endif

static int fsize ( FILE* fp )
{
    int prev = ftell ( fp );
    fseek ( fp, 0L, SEEK_END );
    int sz = ftell ( fp );
    fseek ( fp, prev, SEEK_SET ); //go back to where we were
    return sz;
}

int main ( int argc, char* argv[] )
{
    setvbuf ( stdin, NULL, _IONBF, 0 );
    if ( argc > 2 )
    {
        fprintf ( stderr,
                  "Usage: %s <mtfile>\n"
                  "       %s\n"
                  , argv[0], argv[0] );
        exit ( 0 );
    }
    load_assets();    //int mtfile_size =
    if ( argc == 2 )
    {
        FILE* mtfile = fopen ( argv[1], "rb" );
        int s = fsize ( mtfile );
        char* mtstream = new char[s];
        fread ( mtstream, 1, s, mtfile );
        fclose ( mtfile );
        mtg.initialize_mtstat ( mtstream );
        delete[] mtstream;
    }
    else
    {
        //fread ( mtstream, 1, 0x100000, stdin );
    }
    //mtgui mtg;
    mtg.run();
    return 0;
}
