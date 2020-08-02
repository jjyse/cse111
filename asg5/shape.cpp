// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include "shape.h"
#include "util.h"
#include <GL/freeglut.h>
#include <libgen.h>

//A mapping of fonts to fontnames.
static unordered_map<void*,string> fontname 
{
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

//A mapping of fontnames to fonts.
static unordered_map<string,void*> fontcode 
{
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

//Enables printing out of vertices.
ostream& operator<< (ostream& out, const vertex& where) 
{
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

//Shape constructor: does nothing.
shape::shape() 
{
   DEBUGF ('c', this);
}

//Accepts a font and text string and sets the font and
//text data.
text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) 
{
   DEBUGF ('c', this);
}

//Creates an ellipse object given a width and height.
ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) 
{
   DEBUGF ('c', this);
}

//Creates an circle object given a width and height.
circle::circle (GLfloat diameter): ellipse (diameter, diameter) 
{
   DEBUGF ('c', this);
}


//Creates an polygon object given a width and height.
polygon::polygon (const vertex_list& vertices_): vertices(vertices_) 
{
   DEBUGF ('c', this);
}

//Creates an rectangle object given a width and height.
//Accepts a list of vertices for a rectangle, which will
//be passed into polygon.
rectangle::rectangle (const GLfloat width, const GLfloat height):
            polygon({{width, height},{width, 0},{0, 0},{0, height}}) 
{
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

//Creates an square object given two widths.
square::square (GLfloat width): rectangle (width, width) 
{
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height): 
         polygon({{0,0},{width/2, height/2},{width, 0},{width/2, 0-height/2}}) 
{
   DEBUGF ('c', this);
}

triangle::triangle (GLfloat width): 
          polygon ({{-width, 0},{width, 0},{0, width}}) 
{
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width): triangle ({{0,0},{width/2, width}, 
                                                    {width,0}}) 
{
   DEBUGF ('c', this);
}

//////////////////////////////////////////////////////////////////
void shape::show (ostream& out)  const
{
   out << this << "->" << demangle (*this) << ": ";
}


//Receives coordinates for the center of the shape and
//a color to color the object.
void text::draw (const vertex& center, const rgbcolor& color) 
{
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   const GLubyte* glgreeting =
         reinterpret_cast<const GLubyte*> (textdata.c_str());

   void* font = *glut_fonts;
   int greeting_width = glutBitmapLength (font, glgreeting);
   int greeting_height = glutBitmapHeight (font);

   glColor3ubv (color);

   float xpos = center.xpos;
   float ypos = center.ypos;
   glRasterPos2f (xpos, ypos);

   glutBitmapString (font, glgreeting);

}

//Receives coordinates for the center of the shape and
//a color to color the object.
void ellipse::draw (const vertex& center, const rgbcolor& color) 
{
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   glBegin (GL_POLYGON);
   glColor3ubv (color);
   const GLfloat delta = 2 * M_PI / 64;
   const GLfloat width = window::width / 2.5;
   const GLfloat height = window::height / 2.5;
   const GLfloat xoffset = window::width / 2.0;
   const GLfloat yoffset = window::height / 2.0;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) 
   {
      GLfloat xpos = width * cos (theta) + xoffset;
      GLfloat ypos = height * sin (theta) + yoffset;
      glVertex2f (xpos, ypos);
   }
   glEnd();

}

//Receives coordinates for the center of the shape and
//a color to color the object.
void polygon::draw (const vertex& center, const rgbcolor& color) const 
{
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

ostream& operator<< (ostream& out, const shape& obj) 
{
   obj.show (out);
   return out;
}

