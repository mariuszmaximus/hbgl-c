/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#define STB_IMAGE_IMPLEMENTATION
#include "hbgl.h"

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
Image *ImageNew( HBGL *pHBGL, const char *image_path )
{
   static int imageCounter = 0;  // Statyczna zmienna do zliczania obrazów

   Image *image = ( Image * ) malloc( sizeof( Image ) );
   if( ! image )
   {
      fprintf( stderr, "Failed to allocate memory for new image.\n" );
      return NULL;
   }

   image->pHBGL = pHBGL;  // Ustawiamy wskaźnik do HBGL

   // Dodanie obrazu do listy obrazów w HBGL
   image->pHBGL->imageCount++;
   image->pHBGL->images = realloc( image->pHBGL->images, image->pHBGL->imageCount * sizeof( Image * ) );
   if( image->pHBGL->images == NULL )
   {
      fprintf( stderr, "Failed to reallocate memory for image array.\n" );
      free( image );
      return NULL;
   }
   image->pHBGL->images[ image->pHBGL->imageCount - 1 ] = image;

   image->imageID = ++imageCounter;  // przydzielanie unikatowego ID

   unsigned char *data;
   data = stbi_load( image_path, &image->width, &image->height, &image->channels, 0 );
   if( ! data )
   {
      fprintf( stderr, "Failed to load image: %s\n", image_path );
      free( image );
      return NULL;
   }

   glGenTextures( 1, &image->textureID );
   glBindTexture( GL_TEXTURE_2D, image->textureID );

   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );  // Recommended by NVIDIA

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   if( image->channels == 3 )
   {
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
   }
   else if( image->channels == 4 )
   {
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
   }

   stbi_image_free( data );

   return image;
}

void DrawImage( Image *image, float x, float y, float width, float height )
{
   bool wasEnabled = glIsEnabled( GL_TEXTURE_2D );
   if( ! wasEnabled )
   {
      glEnable( GL_TEXTURE_2D );
   }

   glBindTexture( GL_TEXTURE_2D, image->textureID );

   glBegin( GL_QUADS );
      glTexCoord2f( 0.0f, 1.0f ); glVertex2f( x, y + height );
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f( x + width, y + height );
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f( x + width, y );
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( x, y );
    glEnd();

   glBindTexture( GL_TEXTURE_2D, 0 );

   if( ! wasEnabled )
   {
      glDisable( GL_TEXTURE_2D );
   }
}

void FreeImage( Image *image )
{
   if( image != NULL )
   {
      glDeleteTextures( 1, &image->textureID );
      free( image );
   }
}
