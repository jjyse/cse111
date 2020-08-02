// $Id: ubigint.h,v 1.4 2020-01-06 13:39:55-08 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <vector>
#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"

class ubigint 
{
   friend ostream& operator<< (ostream&, const ubigint&);
   private:
      //using unumber = unsigned long;
      //unumber uvalue {};
      using udigit_t = unsigned char;
      using ubigvalue_t = vector<udigit_t>;
      //ubig_value is a vector of unsigned chars.
      ubigvalue_t ubig_value;
   public:
      void multiply_by_2();
      void divide_by_2();
      // Need default ctor as well.
      ubigint() = default;
      ubigint (unsigned long);
      ubigint (const string&);

      ubigint operator+ (const ubigint&) const;
      ubigint operator- (const ubigint&) const;
      ubigint operator* (const ubigint&) const;
      ubigint operator/ (const ubigint&) const;
      ubigint operator% (const ubigint&) const;

      bool operator== (const ubigint&) const;
      bool operator<  (const ubigint&) const;
};

#endif

