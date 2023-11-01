/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#include "hbgl.h"

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
// static
static void set_color_fromHex( unsigned long hexColor )
{
   if( ( hexColor <= 0xffffff ) )
   {
      float r, g, b;
      r = ( ( hexColor >> 16 ) & 0xFF ) / 255.0;
      g = ( ( hexColor >> 8 )  & 0xFF ) / 255.0;
      b = (   hexColor         & 0xFF ) / 255.0;
      glColor3f( r, g, b );
   }
   else if( ( hexColor <= 0xffffffff ) )
   {
      float r, g, b, a;
      r = ( ( hexColor >> 24 ) & 0xFF ) / 255.0;
      g = ( ( hexColor >> 16 ) & 0xFF ) / 255.0;
      b = ( ( hexColor >> 8 )  & 0xFF ) / 255.0;
      a = ( hexColor & 0xFF )           / 255.0;
      glColor4f( r, g, b, a );
   }
   else
   {
      printf( "Invalid hex value passed \n" );
   }
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

void Point( int x, int y, unsigned int color )
{
   set_color_fromHex( color );
   glPointSize( 1.0 );
   glBegin( GL_POINTS );
   glVertex2f( x, y + 1 );
   glEnd();
}

void PointSize( int x, int y, int point_size, unsigned int color )
{
   if( point_size < 1 )
   {
      fprintf( stderr, "Point size should be greater than 0.\n" );
      point_size = 1;
   }

   set_color_fromHex( color );
   glPointSize( point_size );
   glBegin( GL_POINTS );
   glVertex2f( x, y + 1 );
   glEnd();
}

void Line( int x1, int y1, int x2, int y2, unsigned int color )
{
   set_color_fromHex( color );
   glBegin( GL_LINES );
   glVertex2f( x1, y1 );
   glVertex2f( x2, y2 );
   glEnd();
}

void LineWidth( int x1, int y1, int x2, int y2, int line_width, unsigned int color )
{
   if( line_width < 1 )
   {
      fprintf( stderr, "Line width should be greater than 0.\n" );
      line_width = 1;
   }

   set_color_fromHex( color );
   glLineWidth( line_width );
   glBegin( GL_LINES );
   glVertex2f( x1, y1 );
   glVertex2f( x2, y2 );
   glEnd();
}

void Rect( int x1, int y1, int x2, int y2, unsigned int color )
{
   set_color_fromHex( color );
   glBegin( GL_LINE_LOOP );
   glVertex2f( x1, y1 );
   glVertex2f( x2, y1 );
   glVertex2f( x2, y2 );
   glVertex2f( x1, y2 );
   glEnd();
}

void RectWidth( int x1, int y1, int x2, int y2, int line_width, unsigned int color )
{
   if( line_width < 1 )
   {
      fprintf( stderr, "Line width should be greater than 0.\n" );
      line_width = 1;
   }

   set_color_fromHex( color );
   glLineWidth( line_width );
   glBegin( GL_LINE_LOOP );
   glVertex2f( x1, y1 );
   glVertex2f( x2, y1 );
   glVertex2f( x2, y2 );
   glVertex2f( x1, y2 );
   glEnd();
}


void FillRect( int x1, int y1, int x2, int y2, unsigned int color )
{
   set_color_fromHex( color );
   glBegin( GL_POLYGON );
   glVertex2f( x1, y1 );
   glVertex2f( x2, y1 );
   glVertex2f( x2, y2 );
   glVertex2f( x1, y2 );
   glEnd();
}
