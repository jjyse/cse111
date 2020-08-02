// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map 
{
   //Define makes a record of the definition of a shape
   //and makes it available later for a draw command.
   {"define" , &interpreter::do_define },
   //Draw creates an object in the screen's object list,
   //to be displayed when the program starts.
   {"draw"   , &interpreter::do_draw   },
};

//Map of commands and their corresponding functions.
unordered_map<string,interpreter::factoryfn>
interpreter::factory_map 
{
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
};

//Declare a shape_map called objmap to store shapes.
//<string,shape_ptr>
interpreter::shape_map interpreter::objmap;

//Prints out all objects in the objmap.
interpreter::~interpreter() 
{
   for (const auto& itor: objmap) 
   {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

//Parse input commands (define, border, moveby, draw, whitespace).
void interpreter::interpret (const parameters& params) 
{
   //Add some functionality to ignore commands beginning with
   //a '#', or containing only spaces or empty lines.
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

//Insert a new object given its name and the shape.
void interpreter::do_define (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


//Search for a shape and set its location and color.
void interpreter::do_draw (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   window::push_back (object (itor->second, where, color));
}

//Find the function that corresponds to the shape.
shape_ptr interpreter::make_shape (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

//Set the font and message.
shape_ptr interpreter::make_text (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   return make_shared<text> (nullptr, string());
}

//Set values for ellipse shape.
shape_ptr interpreter::make_ellipse (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   return make_shared<ellipse> (GLfloat(), GLfloat());
}

//Set values for circle shape.
shape_ptr interpreter::make_circle (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (GLfloat());
}

//Set values for polygon shape.
shape_ptr interpreter::make_polygon (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   return make_shared<polygon> (vertex_list());
}

//Set values for rectangle shape.
shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<rectangle> (GLfloat(), GLfloat());
}

//Set values for square shape.
shape_ptr interpreter::make_square (param begin, param end) 
{
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (GLfloat());
}

