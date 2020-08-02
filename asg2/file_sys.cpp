// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"


map<string,inode_ptr> inode_state::directories;
size_t inode::next_inode_nr {1};
string inode_state::filepath;
map<string,string> inode_state::fileContents;
map<string,inode_ptr> inode_state::fileNames;

struct file_type_hash 
{
   size_t operator() (file_type type) const 
   {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) 
{
   static unordered_map<file_type,string,file_type_hash> hash 
   {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() 
{
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");

   inode newinode(file_type::DIRECTORY_TYPE);
   directory dire;
   //Make root
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   //cwd is now the root
   cwd = root;
   dire.getDirents().insert({".", root});
   dire.getDirents().insert({"..", root});
   //Directories now contains root name and root ptr.
   directories.insert({"/", root});
   fileNames.insert({". ..", root});
   //File contents of root initialized to empty.
   fileContents.insert({". .. ", ". .."});
   filepath = "/";
}

const string& inode_state::prompt() const
{
   return prompt_;
}

void inode_state::prompt(string newPrompt) 
{
  prompt_ = newPrompt + " ";
}

ostream& operator<< (ostream& out, const inode_state& state) 
{
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) 
{
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const 
{
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const 
{
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) 
{
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string& filename) 
{
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) 
{
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const 
{
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) 
{
   DEBUGF ('i', filename);

  if(dirents.empty())
  {
    throw file_error ("is a " + error_file_type());
  }
  else
  {
    map<string,inode_ptr>::iterator it;

    if(dirents.count(filename) == 0)
    {
      throw file_error ("is a " + error_file_type());
    }
    else
    {
      it = dirents.find(filename);
      dirents.erase(it);
      cout << filename << " has been removed. \n";
    }

  }
}

inode_ptr directory::mkdir (const string& dirname) 
{
   DEBUGF ('i', dirname);
   return nullptr;
   
}

inode_ptr directory::mkfile (const string& filename) 
{
   DEBUGF ('i', filename);
   return nullptr;
   //inode newInode(file_type::PLAIN_TYPE);
   //dirents.insert({filename, newInode.getContents()});
   //return newInode;
}

void inode_state::setCwd(inode_ptr newCwd)
{
  cwd = newCwd;
}

