/*
 *
 */

#include "hbgl.h"

int main()
{
   HBGL *app = WindowNew( 830, 450, "Test text" );
   Font *font = SystemFontNew( app, "Courier_New" );

   Background( app, 0xF1F1F1 );

   while( ! MainLoop( app ) )
   {
      BeginDrawing( app );

         for( int i = 0; i < 20; i++ )
         {
            int x = rand() % 830;
            int y = rand() % 450;

            int font_size = rand() % 63 + 10;

            unsigned int color = rand() % 0xFFFFFF;

            DrawFont( font, x, y, "Hello, World!", font_size, color );
         }

      EndDrawing( app );
      WaitEvents();
   }

   CloseAll( app );
}
