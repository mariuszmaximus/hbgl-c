/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#define STB_TRUETYPE_IMPLEMENTATION
#include "hbgl.h"

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// static
static int decode_utf8( const unsigned char *text, int *out_codepoints, int max_size )
{
   int i = 0, j = 0;
   while( text[ i ] != '\0' && j < max_size - 1 ) // -1 to make room for -1 at the end
   {
      int codepoint = 0;
      if( ( text[ i ] & 0x80 ) == 0 )
      {
         codepoint = text[ i ];
         i += 1;
      }
      else if( ( text[ i ] & 0xE0 ) == 0xC0 )
      {
         codepoint = ( ( text[ i ] & 0x1F ) << 6 ) | ( text[ i + 1 ] & 0x3F );
         i += 2;
      }
      else if( ( text[ i ] & 0xF0 ) == 0xE0 )
      {
         codepoint = ( ( text[ i ] & 0x0F ) << 12 ) | ( ( text[ i + 1 ] & 0x3F ) << 6 ) | ( text[ i + 2 ] & 0x3F );
         i += 3;
      }
      else if( ( text[ i ] & 0xF8 ) == 0xF0 )
      {
         codepoint = ( ( text[ i ] & 0x07 ) << 18 ) | ( ( text [ i + 1 ] & 0x3F ) << 12 ) | ( ( text[ i + 2 ] & 0x3F ) << 6 ) | ( text[ i + 3 ] & 0x3F );
         i += 4;
      }
      else
      {
         return -1; // Invalid UTF-8 character
      }
      out_codepoints[ j++ ] = codepoint;
   }
   out_codepoints[ j ] = -1;              // Finish point code list
   return ( text[ i ] == '\0' ) ? 0 : -1; // Return 0 if all characters were processed
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
Font *SystemFontNew( HBGL *pHBGL, const char *font_name )
{
   char font_path[ 1024 ];
   FILE *fp = NULL;

   #if defined( _WIN32 ) || defined( _WIN64 )
      snprintf( font_path, sizeof( font_path ), "C:\\Windows\\Fonts\\%s.ttf", font_name );
      fp = fopen( font_path, "rb" );
   #elif defined( __APPLE__ ) || defined( __MACH__ )
      snprintf( font_path, sizeof( font_path ), "/Library/Fonts/%s.ttf", font_name );
      fp = fopen( font_path, "rb" );
   #elif defined( __linux__ )
      // Lista katalogów do przeszukania
      // find / -name your_font.ttf 2>/dev/null
      const char *directories[] =
      {
         "/usr/share/fonts/truetype/",
         "/usr/share/fonts/",
         "/usr/local/share/fonts/",
         "/usr/share/fonts/truetype/msttcorefonts/",
         NULL // Terminator
      };

      for( int i = 0; directories[ i ] != NULL; ++i )
      {
         snprintf( font_path, sizeof( font_path ), "%s%s.ttf", directories[ i ], font_name );
         fp = fopen( font_path, "rb" );
         if( fp != NULL )
         {
            break;
         }
      }
   #else
      fprintf( stderr, "Unsupported platform\n" );
      return NULL;
   #endif

   if( ! fp )
   {
      fprintf( stderr, "Failed to open font file: %s\n", font_path );
      return NULL;
   }

   fclose( fp );
   return FontNew( pHBGL, font_path );
}

Font *FontNew( HBGL *pHBGL, const char *font_path )
{
   static int fontCounter = 0;  // Statyczna zmienna do zliczania obrazów

   Font *pFont = malloc( sizeof( Font ) );
   if( ! pFont )
   {
      fprintf( stderr, "Failed to allocate memory for new font.\n" );
      return NULL;
   }

   pFont->pHBGL = pHBGL;  // Ustawiamy wskaźnik do HBGL

   // Dodanie czcionki do listy obrazów w HBGL
   pFont->pHBGL->fontCount++;
   pFont->pHBGL->fonts = realloc( pFont->pHBGL->fonts, pFont->pHBGL->fontCount * sizeof( Font * ) );
   if( ! pFont->pHBGL->fonts )
   {
      fprintf( stderr, "Failed to reallocate memory for font array.\n" );
      free( pFont );
      return NULL;
   }
   pFont->pHBGL->fonts[ pFont->pHBGL->fontCount - 1 ] = pFont;

   pFont->fontID = ++fontCounter;  // przydzielanie unikatowego ID

   FILE *fp = fopen( font_path, "rb" );
   if( ! fp )
   {
      fprintf( stderr, "Failed to open font file: %s\n", font_path );
      return NULL;
   }

   fseek( fp, 0, SEEK_END );
   size_t size = ftell( fp );
   fseek( fp, 0, SEEK_SET );

   unsigned char *ttf_buffer = malloc( size );
   if( ! ttf_buffer )
   {
      fprintf( stderr, "Failed to allocate memory for font buffer.\n" );
      fclose( fp );
      return NULL;
   }

   size_t read_elements = fread( ttf_buffer, 1, size, fp );
   if( read_elements != size )
   {
      fprintf( stderr, "Failed to read font file: expected %zu elements, got %zu.\n", size, read_elements );
      free( ttf_buffer );
      fclose( fp );
      return NULL;
   }

   fclose( fp );

   stbtt_fontinfo  font_info;
   if( ! stbtt_InitFont( &font_info, ttf_buffer, stbtt_GetFontOffsetForIndex( ttf_buffer, 0 ) ) )
   {
      fprintf( stderr, "Failed to initialize the font.\n" );
      free( ttf_buffer );
      return NULL;
   }

   unsigned char temp_bitmap[ 1024 * 1024 ];
   stbtt_BakeFontBitmap( ttf_buffer, 0, 64.0, temp_bitmap, 1024, 1024, 32, 380, pFont->cdata );

   glGenTextures( 1, &pFont->textureID );
   glBindTexture( GL_TEXTURE_2D, pFont->textureID );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, 1024, 1024, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

   free( ttf_buffer );
   return pFont;
}

void DrawFont( Font *pFont, float x, float y, const char *text, float fontSize, unsigned int color )
{
   bool wasEnabled = glIsEnabled( GL_TEXTURE_2D );
   if( ! wasEnabled )
   {
      glEnable( GL_TEXTURE_2D );
   }

   float scale = fontSize / 64.0f;

   if( ( color <= 0xffffff ) )
   {
      float r, g, b;
      r = ( ( color >> 16 ) & 0xFF ) / 255.0f;
      g = ( ( color >> 8 )  & 0xFF ) / 255.0f;
      b = (   color         & 0xFF ) / 255.0f;
      glColor3f( r, g, b );
   }
   else if( ( color <= 0xffffffff ) )
   {
      float r, g, b, a;
      r = ( ( color >> 24 ) & 0xFF ) / 255.0f;
      g = ( ( color >> 16 ) & 0xFF ) / 255.0f;
      b = ( ( color >> 8 )  & 0xFF ) / 255.0f;
      a = (   color         & 0xFF ) / 255.0f;
      glColor4f( r, g, b, a );
   }
   else
   {
      printf( "Invalid hex value passed \n" );
   }

   // Dekodowanie UTF-8
   int codepoints[ 256 ];
   if( decode_utf8( ( const unsigned char * ) text, codepoints, 256 ) < 0 )
   {
      fprintf( stderr, "Incorrect sequence UTF-8.\n" );
      return;
   }

   glBindTexture( GL_TEXTURE_2D, pFont->textureID );

   glBegin( GL_QUADS );

   int i = 0;
   while( codepoints[ i ] != -1 )
   {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad( pFont->cdata, 1024, 1024, codepoints[ i ] - 32, &x, &y, &q, 1 );

      q.x0 *= scale; q.x1 *= scale;
      q.y0 *= scale; q.y1 *= scale;

      glTexCoord2f( q.s0, q.t0 ); glVertex2f( q.x0, q.y0 );
      glTexCoord2f( q.s1, q.t0 ); glVertex2f( q.x1, q.y0 );
      glTexCoord2f( q.s1, q.t1 ); glVertex2f( q.x1, q.y1 );
      glTexCoord2f( q.s0, q.t1 ); glVertex2f( q.x0, q.y1 );

      i++;
   }
   glEnd();

   if( ! wasEnabled )
   {
      glDisable( GL_TEXTURE_2D );
   }
}

void FreeFont( Font *pFont )
{
   if( pFont != NULL )
   {
      glDeleteTextures( 1, &pFont->textureID );
      free( pFont );
   }
}
