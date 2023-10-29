#ifndef HBGL_H_
#define HBGL_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "stb_truetype.h"
#include "stb_image.h"

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

typedef enum
{
   HBGL_SUCCESS = 0,
   HBGL_ERROR_INITIALIZATION_FAILED,
   HBGL_ERROR_SHADER_COMPILATION_FAILED,
   // ...
} HBGLErrorCode;

typedef enum bool
{
   F = 0,
   T = ( ! 0 )
} bool;

typedef struct _HBGL
{
   GLFWwindow   *window;       // handle of the created window
   int           width;        // desired width, in screen coordinates, of the window
   int           height;       // desired height, in screen coordinates
   const char   *title;        // initial, UTF-8 encoded window title
   bool          closeFlag;    // returns the value of the close flag of the specified window

   double        cursorX;      // new cursor x-coordinate, relative to the left edge of the content area
   double        cursorY;      // new cursor y-coordinate, relative to the top edge of the content area
   int           keyCode;      // the keyboard key that was pressed or released
   int           keyScancode;  // the system-specific scancode of the key
   int           keyAction;    // GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions
   int           keyMods;      // bit field describing which modifier keys were held down
   int           mouseButton;  // mouse button that was pressed or released
   int           mouseAction;  // one of GLFW_PRESS or GLFW_RELEASE, future releases may add more actions
   int           mouseMods;    // bit field describing which modifier keys were held down
   int           winMaximized; // GLFW_TRUE if the window was maximized, or GLFW_FALSE if it was restored

   unsigned long background;

} HBGL;

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// API functions
HBGL *WindowNew( int width, int height, const char *title );
bool MainLoop( HBGL *w );
bool CloseAll( HBGL *w );
void Background( HBGL *w, unsigned long color );
void WaitEvents();

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
void begin_drawing( HBGL *w );
void end_drawing( HBGL *w );

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
void CheckOpenGLError( const char* stmt, const char* fname, int line );
void CheckHBGLError( HBGLErrorCode error_code, const char* description, const char* file, int line );

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// macros
#define CHECK_OPENGL_ERROR( stmt ) CheckOpenGLError( stmt, __FILE__, __LINE__ )
#define CHECK_HBGL_ERROR( error_code, description ) \
   do { \
      HBGLErrorCode code = ( error_code ); \
      if( code != HBGL_SUCCESS ) { \
         CheckHBGLError( code, description, __FILE__, __LINE__ ); \
         return NULL; \
      } \
   } while( 0 )

#define MAX( a, b ) ( ( a ) < ( b ) ? ( b ) : ( a ) )
#define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UNUSED( n ) ( ( void )( n ) )

#define BeginDrawing( w ) do { begin_drawing( w )
#define EndDrawing( w ) end_drawing( w ); } while( 0 )

#endif /* End HBGL_H_ */
