// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bool isNeg = true;

bigint::bigint (long that): uvalue (that), is_negative (that < 0) 
{
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_)
{

}

//Purpose: Accepts a string object that represents a 'number'.
//Checks if the 'number' is negative (if theres an underscore)
//Result: is_negative stores the sign, and uvalue stores the
//number in int form and with its sign removed.
bigint::bigint (const string& that) 
{
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const 
{
   return *this;
}

bigint bigint::operator- () const 
{
   return {uvalue, not is_negative};
}

//If signs are same, then add.
//If signs are different, subtract the smaller value from bigger value.
//If signs are different and values are the same, the result = 0.
bigint bigint::operator+ (const bigint& that) const 
{
   ubigint result;
   if(is_negative == that.is_negative)
   {
       result = uvalue + that.uvalue;     //sign = is_negative
       isNeg = is_negative;
   }
   else
   {
       if(uvalue > that.uvalue)
       {
           result = uvalue - that.uvalue; //sign = is_negative
           isNeg = is_negative;
       }
       else if(uvalue < that.uvalue)
       {
           result = that.uvalue - uvalue; //sign = that.is_negative
           isNeg = that.is_negative;
       }
       else
       {   
           result = uvalue - uvalue;      //sign = false (positive)
           isNeg = false;
       }
   }
   return result;

}

bigint bigint::operator- (const bigint& that) const 
{
   ubigint result;
   if(is_negative == that.is_negative)
   {
       if(uvalue > that.uvalue)
       {
         result = uvalue - that.uvalue;

          isNeg = is_negative;
       }
       else if (uvalue < that.uvalue)
       {
         result = that.uvalue - uvalue;

         isNeg = that.is_negative;
       }
       else
       {
         result = uvalue - uvalue;

         isNeg = false;
       }
   }
   else
   {
       result = uvalue + that.uvalue;
       if(is_negative == true)
       {

         isNeg =  true;
       }
       else if (is_negative == false)
       {

         isNeg = false;
       }
       else
       {

         isNeg = false; 
       }
   }
   return result;
}


//If signs are different, result is negative.
//If signs are same, result is positive.
bigint bigint::operator* (const bigint& that) const 
{
  bigint result;
  result = uvalue * that.uvalue;

  if (is_negative != that.is_negative)
  {
      //result is negative
      isNeg = true;
  }
  else
  { 
      //result is positive
      isNeg = false;
  }

  return result;
}

//If signs are different, result is negative.
//If signs are same, result is positive.
bigint bigint::operator/ (const bigint& that) const 
{
  bigint result;
  result = uvalue / that.uvalue;

  if (is_negative != that.is_negative)
  {
      //result is negative
      isNeg = true;
  } 
  else
  { 
      //result is positive
      isNeg = false;
  }
   return result;
}


//The sign of the result is the sign of the left value.
bigint bigint::operator% (const bigint& that) const 
{
  bigint result;

  result = uvalue % that.uvalue;

  isNeg = is_negative;

  return result;
}

//Only returns true if both numbers have the same sign 
//and both numbers have the same value.
bool bigint::operator== (const bigint& that) const 
{
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const 
{
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) 
{
   return out << (isNeg ? "-" : "")
              << that.uvalue;
}
