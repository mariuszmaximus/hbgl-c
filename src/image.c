/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#define STB_IMAGE_IMPLEMENTATION
#include "hbgl.h"

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
Image *ImageNew( HBGL *pHBGL, const char *image_path )
{
   static int imageCounter = 0;  // Statyczna zmienna do zliczania obrazów

   Image *pImage = malloc( sizeof( Image ) );
   if( ! pImage )
   {
      fprintf( stderr, "Failed to allocate memory for new image.\n" );
      return NULL;
   }

   memset( pImage, 0, sizeof( Image ) );

   pImage->pHBGL = pHBGL;  // Ustawiamy wskaźnik do HBGL

   // Dodanie obrazu do listy obrazów w HBGL
   pImage->pHBGL->imageCount++;
   pImage->pHBGL->images = realloc( pImage->pHBGL->images, pImage->pHBGL->imageCount * sizeof( Image * ) );
   if( ! pImage->pHBGL->images )
   {
      fprintf( stderr, "Failed to reallocate memory for image array.\n" );
      free( pImage );
      return NULL;
   }
   pImage->pHBGL->images[ pImage->pHBGL->imageCount - 1 ] = pImage;

   pImage->imageID = ++imageCounter;  // przydzielanie unikatowego ID

   unsigned char *data;
   data = stbi_load( image_path, &pImage->width, &pImage->height, &pImage->channels, 0 );
   if( ! data )
   {
      fprintf( stderr, "Failed to load image: %s\n", image_path );
      // Cleanup if image loading failed
      pImage->pHBGL->imageCount--;
      if( pImage->pHBGL->imageCount > 0 )
      {
         pImage->pHBGL->images[ pImage->pHBGL->imageCount ] = NULL;
      }
      else
      {
         free( pImage->pHBGL->images );
         pImage->pHBGL->images = NULL;
      }

      // Returning Image with zeroed dimensions to avoid errors with unloaded images.
      memset( pImage, 0, sizeof( Image ) );
      return pImage;
   }

   glGenTextures( 1, &pImage->textureID );
   glBindTexture( GL_TEXTURE_2D, pImage->textureID );

   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );  // Recommended by NVIDIA

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   if( pImage->channels == 3 )
   {
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, pImage->width, pImage->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
   }
   else if( pImage->channels == 4 )
   {
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pImage->width, pImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
   }

   stbi_image_free( data );

   return pImage;
}

void DrawImage( Image *pImage, int x, int y, int width, int height )
{
   if( pImage && pImage->pHBGL && pImage->pHBGL->images )
   {
      // Aktualizacja pozycji obrazu
      pImage->x = x;
      pImage->y = y;

      bool wasEnabled = glIsEnabled( GL_TEXTURE_2D );
      if( ! wasEnabled )
      {
         glEnable( GL_TEXTURE_2D );
      }

      glBindTexture( GL_TEXTURE_2D, pImage->textureID );

      glBegin( GL_QUADS );
         glTexCoord2f( 0.0f, 1.0f ); glVertex2f( ( float )   x, ( float ) ( y + height ) );
         glTexCoord2f( 1.0f, 1.0f ); glVertex2f( ( float ) ( x + width ), ( float ) ( y + height ) );
         glTexCoord2f( 1.0f, 0.0f ); glVertex2f( ( float ) ( x + width ), ( float ) y );
         glTexCoord2f( 0.0f, 0.0f ); glVertex2f( ( float )   x, ( float ) y );
      glEnd();

      glBindTexture( GL_TEXTURE_2D, 0 );

      if( ! wasEnabled )
      {
         glDisable( GL_TEXTURE_2D );
      }
   }
   else
   {
      return;
   }
}

void FreeImage( Image *pImage )
{
   if( pImage != NULL )
   {
      glDeleteTextures( 1, &pImage->textureID );
      free( pImage );
   }
}
