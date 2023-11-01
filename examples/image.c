/*
 *
 */

#include "hbgl.h"

int main()
{
   HBGL *app = WindowNew( 830, 450, "Test image" );
   Image *img = ImageNew( app, "../docs/assets/img/harbour_logo.png" );

   Background( app, 0xF1F1F1 );

   while( ! MainLoop( app ) )
   {
      BeginDrawing( app );

         int startX = ( app->width  - img->width ) / 2;
         int startY = ( app->height - img->height ) / 2;

         DrawImage( img, startX, startY, img->width, img->height );

      EndDrawing( app );
      WaitEvents();
   }

   CloseAll( app );
}
