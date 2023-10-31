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

         DrawFont( font, 100, 100, "Hello, World!", 36, 0x323232 );
         DrawFont( font, 100, 150, "ą, ć, ę, ł, ń, ó, ś, ź, ż.", 36, 0x323232 );

      EndDrawing( app );
      WaitEvents();
   }

   CloseAll( app );
}
