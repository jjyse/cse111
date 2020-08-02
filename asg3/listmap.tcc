// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

// listmap destructor - deletes the object upon ending the program.
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() 
{
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// iterator listmap::insert (const value_type&)
// inserts a given pair.
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) 
{
    DEBUGF ('l', &pair << "->" << pair);
/*
    //Create a new node newNode
    //Set set next node = nullptr, previous node = nullptr
    node* newNode = new node(nullptr, nullptr, pair);

    anchor()->next = newNode;  //Mark the head of list
    anchor()->prev = newNode;  //Mark the tail of list


    return iterator(newNode);  //Iterator is set to newNode
*/
    return iterator();
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) 
{
   DEBUGF ('l', that);
   return iterator();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) 
{
   DEBUGF ('l', &*position);
   return iterator();
}


