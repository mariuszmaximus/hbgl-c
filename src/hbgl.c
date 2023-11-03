/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#include "hbgl.h"

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// static
static void cursor_position_callback( GLFWwindow *window, double x, double y )
{
   HBGL *pHBGL = glfwGetWindowUserPointer( window );

   pHBGL->cursorX = x;
   pHBGL->cursorY = y;
}

static void key_callback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
   HBGL *pHBGL = glfwGetWindowUserPointer( window );

   pHBGL->keyCode     = key;
   pHBGL->keyScancode = scancode;
   pHBGL->keyAction   = action;
   pHBGL->keyMods     = mods;
}

static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods )
{
   HBGL *pHBGL = glfwGetWindowUserPointer( window );

   pHBGL->mouseButton = button;
   pHBGL->mouseAction = action;
   pHBGL->mouseMods   = mods;
}

static void window_maximize_callback( GLFWwindow *window, int maximized )
{
   HBGL *pHBGL = glfwGetWindowUserPointer( window );

   pHBGL->winMaximized = maximized;
}

static void window_size_callback( GLFWwindow *window, int width, int height )
{
   HBGL *pHBGL = glfwGetWindowUserPointer( window );

   pHBGL->width  = width;
   pHBGL->height = height;
}

static void set_clear_color_fromHex( unsigned long hexColor )
{
   if( ( hexColor <= 0xffffff ) )
   {
      float r, g, b;
      r = ( ( hexColor >> 16 ) & 0xFF ) / 255.0;
      g = ( ( hexColor >> 8 )  & 0xFF ) / 255.0;
      b = (   hexColor         & 0xFF ) / 255.0;
      glClearColor( r, g, b, 1.0 );
   }
   else if( ( hexColor <= 0xffffffff ) )
   {
      float r, g, b, a;
      r = ( ( hexColor >> 24 ) & 0xFF ) / 255.0;
      g = ( ( hexColor >> 16 ) & 0xFF ) / 255.0;
      b = ( ( hexColor >> 8 )  & 0xFF ) / 255.0;
      a = (   hexColor         & 0xFF ) / 255.0;
      glClearColor( r, g, b, a );
   }
   else
   {
      printf( "Invalid hex value passed \n" );
   }
}

static void printDiagnostics( HBGL *pHBGL )
{
   // Wyczyść ekran
   printf( "\033[2J" );

   // Ustaw kursor na początek ekranu
   printf( "\033[H" );

   printf( "Window Dimensions: %dx%d\n", pHBGL->width, pHBGL->height );
   printf( "Cursor Position: (%.2f, %.2f)\n", pHBGL->cursorX, pHBGL->cursorY );
   printf( "Last Key: %d, Scancode: %d, Action: %d, Mods: %d\n", pHBGL->keyCode, pHBGL->keyScancode, pHBGL->keyAction, pHBGL->keyMods );
   printf( "Last Mouse Button: %d, Action: %d, Mods: %d\n", pHBGL->mouseButton, pHBGL->mouseAction, pHBGL->mouseMods );

   if( pHBGL->winMaximized )
   {
      printf( "Window is: maximized\n" );
   }
   else
   {
      printf( "Window is: normal\n" );
   }

   printf( "Background Color: %#lx\n", pHBGL->background );

   printf( "\n" );
   printf( "Total Fonts: %d\n", pHBGL->fontCount );
      if( pHBGL->failedFontCount > 0 )
      {
         printf( "Failed to load Fonts: %d\n", pHBGL->failedFontCount );
      }

   printf( "Total Images: %d\n", pHBGL->imageCount );
      if( pHBGL->failedImageCount > 0 )
      {
         printf( "Failed to load Images: %d\n", pHBGL->failedImageCount );
      }

   printf( "\n" );

   if( pHBGL->fontCount > 0 && pHBGL->fonts[ 0 ] )
   {
      printf( "\n" );
      printf("Font: %d\n", pHBGL->fontCount);
      for( int i = 0; i < pHBGL->fontCount; i++ )
      {
         Font *pFont = pHBGL->fonts[ i ];
         printf( "Font ID: %d\n", pFont->fontID );
         printf( "Texture ID: %u\n", pFont->textureID );
         printf( "x: %d\n", pFont->x );
         printf( "y: %d\n", pFont->y );
      }
   }

   if( pHBGL->imageCount > 0 && pHBGL->images[ 0 ] )
   {
      printf( "\n" );
      printf("Image: %d\n", pHBGL->imageCount);
      for( int i = 0; i < pHBGL->imageCount; i++ )
      {
         Image *pImage = pHBGL->images[ i ];
         printf( "Image ID: %d\n", pImage->imageID );
         printf( "Texture ID: %u\n", pImage->textureID );
         printf( "x: %d\n", pImage->x );
         printf( "y: %d\n", pImage->y );
         printf( "Width: %d\n", pImage->width );
         printf( "Height: %d\n", pImage->height );
         printf( "Channels: %d\n", pImage->channels );
      }
   }

   fflush( stdout );
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// internal
void CheckHBGLError( HBGLErrorCode error_code, const char* description, const char* file, int line )
{
   printf( "[HBGL ERROR] %s (code %d) at %s:%d\n", description, error_code, file, line );
}

void CheckOpenGLError( const char *stmt, const char *fname, int line, GLenum *errCode )
{
   GLenum err = glGetError();
   if( err != GL_NO_ERROR )
   {
      printf( "OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt );
      *errCode = err;
   }
   else
   {
     *errCode = GL_NO_ERROR;
   }
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// API functions
HBGL *WindowNew( int width, int height, const char *title )
{
   if( ! glfwInit() )
   {
      exit( EXIT_FAILURE );
   }

   HBGL *pHBGL = malloc( sizeof( HBGL ) );
   if( ! pHBGL )
   {
      fprintf( stderr, "Memory allocation failed for HBGL structure.\n" );
      return NULL;
   }

   memset( pHBGL, 0, sizeof( HBGL ) );

   pHBGL->width  = width;
   pHBGL->height = height;
   pHBGL->title  = title;

   // Font
   pHBGL->fonts = NULL;
   pHBGL->fontCount = 0;
   pHBGL->failedFontCount = 0;

   // Image
   pHBGL->images = NULL;
   pHBGL->imageCount = 0;
   pHBGL->failedImageCount = 0;

   pHBGL->window = glfwCreateWindow( pHBGL->width, pHBGL->height, pHBGL->title, NULL, NULL );
   if( ! pHBGL->window )
   {
      free( pHBGL );
      glfwTerminate();
      return NULL;
   }

   glfwMakeContextCurrent( pHBGL->window );
   glfwSetWindowUserPointer( pHBGL->window, pHBGL );
   glfwSwapInterval( 1 );

   glfwSetCursorPosCallback( pHBGL->window, cursor_position_callback );
   glfwSetKeyCallback( pHBGL->window, key_callback );
   glfwSetMouseButtonCallback( pHBGL->window, mouse_button_callback );
   glfwSetWindowMaximizeCallback( pHBGL->window, window_maximize_callback );
   glfwSetWindowSizeCallback( pHBGL->window, window_size_callback );

   glfwSetWindowSizeLimits( pHBGL->window, pHBGL->width, pHBGL->height, GLFW_DONT_CARE , GLFW_DONT_CARE );

   return pHBGL;
}

/*
 * Using glfwWaitEventsTimeout() instead of glfwWaitEvents() to allow the application
 * to periodically wake up from the event wait state. This is beneficial for performing
 * background tasks such as updating game state, handling network communications, or
 * carrying out periodic checks within the application. The timeout is set to 0.5 seconds,
 * which is a reasonable interval to keep the application responsive without consuming
 * too much CPU. It strikes a balance between efficiency and responsiveness.
 */
bool MainLoop( HBGL *pHBGL )
{
   bool state;

   pHBGL->closeFlag = glfwWindowShouldClose( pHBGL->window );
   glfwSetWindowShouldClose( pHBGL->window, GLFW_FALSE );

   state = glfwGetWindowAttrib( pHBGL->window, GLFW_ICONIFIED );
   if( state == T )
   {
      while( state == T )
      {
         state = glfwGetWindowAttrib( pHBGL->window, GLFW_ICONIFIED );

         glfwWaitEventsTimeout( 0.5 );
      }
   }
   return pHBGL->closeFlag;
}

void begin_drawing( HBGL *pHBGL )
{
   glfwGetFramebufferSize( pHBGL->window, &pHBGL->width, &pHBGL->height );

   glViewport( 0, 0, pHBGL->width, pHBGL->height );
   set_clear_color_fromHex( pHBGL->background );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glDisable( GL_CULL_FACE );
   glDisable( GL_DEPTH_TEST );
   glDisable( GL_BLEND );

   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( 0, pHBGL->width, pHBGL->height, 0, -1, 1 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();

   glEnable( GL_TEXTURE_2D );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void end_drawing( HBGL *pHBGL )
{
   printDiagnostics( pHBGL );
   CHECK_OPENGL_ERROR( "end_drawing" );
   glfwSwapBuffers( pHBGL->window );
}

bool CloseAll( HBGL *pHBGL )
{
   if( pHBGL )
   {
      if( pHBGL->fonts != NULL )
      {
         for( int i = 0; i < pHBGL->fontCount; ++i )
         {
            FreeFont( pHBGL->fonts[ i ] );
            printf( "Font number %d has been closed successfully.\n", i + 1 );
         }
         free( pHBGL->fonts );
      }
      //---
      if( pHBGL->images != NULL )
      {
         for( int i = 0; i < pHBGL->imageCount; ++i )
         {
            FreeImage( pHBGL->images[ i ] );
            printf( "Image number %d has been closed successfully.\n", i + 1 );
         }
         free( pHBGL->images );
      }

      glfwDestroyWindow( pHBGL->window );
      free( pHBGL );
      glfwTerminate();
      return T;
   }
   else
   {
      fprintf( stderr, "Error: CloseWindow called with NULL HBGL pointer.\n" );
      return F;
   }
}

void Background( HBGL *pHBGL, unsigned long color )
{
   pHBGL->background = color;
}

void PollEvents()
{
   glfwPollEvents();
}

void WaitEvents()
{
   glfwWaitEvents();
}
