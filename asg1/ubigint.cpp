// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $

#include <iostream> 
#include <string>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <sstream>
#include <algorithm>
using namespace std;

#include "ubigint.h"
#include "debug.h"

//DONE
//Convert unsigned long to character digits, to put into ubig_value.
ubigint::ubigint (unsigned long that): ubig_value (that) 
{
   string numstring;
   stringstream ss;
   ss << that;
   //numstring now holds the string version of 'that'.
   numstring = ss.str();
   //convert to char vector.
   for(auto c : numstring)
      ubig_value.push_back(c);
}

//DONE
//Transfers the string into the char vector ubig_value.
ubigint::ubigint (const string& that): ubig_value(0) 
{
   for (char digit : that)
   {
      if (not isdigit (digit))
      {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
   }
   for(auto c : that)
      ubig_value.push_back(c);
}

ubigint ubigint::operator+ (const ubigint& that) const 
{
   
   ubigint sum;                                
   ubigvalue_t left = ubig_value;              
   ubigvalue_t right = that.ubig_value;        
   int L_indices = ubig_value.size() - 1;      
   int R_indices = that.ubig_value.size() - 1;
   int L_intdigit = 0;                         
   int R_intdigit = 0;                         
   int digit_sum = 0;                          
   int carry = 0;                              

   //Reverse the vectors to simplify vector operations.
   reverse(left.begin(), left.end());
   reverse(right.begin(), right.end());

   //Make sure the lengths of the 2 numbers are the same.
   //Otherwise, append the shorter number with 0's.

   //If the right number is smaller...
   if(left.size() > right.size())
   {
     //Append right number with zeros.
     for(int current = R_indices; current <= L_indices; current++)
     {
        right.push_back('0');
     }
   }
   //If the left number is smaller...
   else if(left.size() < right.size())
   {
     //Append left number with zeros.
     for(int current = L_indices; current <= R_indices; current++)
     {
        left.push_back('0');
     }
   }

   //Begin adding the two vectors.

   //Run down the length of the two vectors. 
   for(int current = 0; current <= L_indices; current++)
   {
      //Convert the current digits into integers.
      L_intdigit = left[current] - 48;
      R_intdigit = right[current] - 48;

      //Add each digit.
      digit_sum = L_intdigit + R_intdigit + carry;
      if(digit_sum > 9)
      {
         digit_sum = digit_sum - 10;
         carry = 1;
      }
      else
      {
         carry = 0;
      }
      
      sum.ubig_value.push_back(digit_sum + 48);
   }

  //Append any remaining carry value.
  if(carry == 1)
  {
     sum.ubig_value.push_back('1');
  }

  //Reverse the final product.
  reverse(sum.ubig_value.begin(), sum.ubig_value.end());
  return sum;

}

ubigint ubigint::operator- (const ubigint& that) const 
{
   ubigint difference;                        
   ubigvalue_t left = ubig_value;              
   ubigvalue_t right = that.ubig_value;        
   int L_indices = ubig_value.size() - 1;      
   int R_indices = that.ubig_value.size() - 1; 
   int L_intdigit = 0;                        
   int R_intdigit = 0;                        
   int digit_sub = 0;                          
   int carry = 0;                              

   //Reverse the vectors to simplify vector operations.
   reverse(left.begin(), left.end());
   reverse(right.begin(), right.end());

   //Make sure the lengths of the 2 numbers are the same.
   //Otherwise, append the shorter number with 0's.

   //If the right number is smaller...
   if(left.size() > right.size())
   {
     //Append right number with zeros.
     for(int current = R_indices; current <= L_indices; current++)
     {
        right.push_back('0');
     }
   }
   //If the left number is smaller...
   else if(left.size() < right.size())
   {
     //Append left number with zeros.
     for(int current = L_indices; current <= R_indices; current++)
     {
        left.push_back('0');
     }
   }

   //Begin subtracting the two vectors.

   //Run down the length of the two vectors. 
   for(int current = 0; current <= L_indices; current++)
   {
      //Convert the current digits into integers.
      L_intdigit = left[current] - 48;
      R_intdigit = right[current] - 48;

      //Subtract each digit.
      digit_sub = L_intdigit - R_intdigit + carry;
      if(digit_sub < 0)
      {
         digit_sub = digit_sub + 10;
         carry = -1;
      }
      else
      {
         carry = 0;
      }
      
      difference.ubig_value.push_back(digit_sub + 48);
   }

  //Reverse the final product.
  reverse(difference.ubig_value.begin(), difference.ubig_value.end());
  return difference;
}

ubigint ubigint::operator* (const ubigint& that) const 
{
   ubigint product;                            
   ubigvalue_t left = ubig_value;              
   ubigvalue_t right = that.ubig_value;        
   int L_indices = ubig_value.size() - 1;      
   int R_indices = that.ubig_value.size() - 1; 
   int L_intdigit = 0;                         
   int R_intdigit = 0;                         
   int digit_prod = 0;                         
   int carry = 0;                              
   int Literate = 0;                           
   int Riterate = 0;                           


   //Convert the current digits into integers.
   L_intdigit = left[0] - 48;
   R_intdigit = right[0] - 48;
   
   int productt = L_intdigit * R_intdigit;

   string numstring;
   stringstream ss;
   ss << productt;
   //numstring now holds the string version of 'that'.
   numstring = ss.str();
   //convert to char vector.
   for(auto c : numstring)
      product.ubig_value.push_back(c);

  return product;
}

void ubigint::multiply_by_2() 
{
}

void ubigint::divide_by_2() 
{
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) 
{
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const 
{
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const 
{
   //return udivide (*this, that).remainder;
   
   ubigint result;
   result.ubig_value.push_back('2');
   return result;
}

bool ubigint::operator== (const ubigint& that) const 
{
   //Signs are compared in bigint.cpp
   bool result = equal(ubig_value.begin(), ubig_value.end(), 
                       that.ubig_value.begin());
   if (result)
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool ubigint::operator< (const ubigint& that) const 
{
   for(int i = 0; i < ubig_value.size(); ++i)
   {
      if((ubig_value[i] > that.ubig_value[i]) ||
         (ubig_value[i] == that.ubig_value[i]))
      {
         return false;
      }  
   }
   return true;
}

//DONE
//Print the contents of ubig_value.
ostream& operator<< (ostream& out, const ubigint& that) 
{ 
   for(int current = 0; current < that.ubig_value.size(); current++)
   {
      out << that.ubig_value[current];
   }
   return out;
}

