/*
 *
 */

#include "hbgl.h"

int main()
{
   HBGL *app = WindowNew( 830, 450, "Test window" );

   Background( app, 0xF1F1F1 );

   while( ! MainLoop( app ) )
   {
      BeginDrawing( app );

         // code ...

      EndDrawing( app );
      WaitEvents();
   }

   CloseAll( app );
}
