// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"
#include <algorithm>
#include <string>

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

//Enter file name to get back the node_ptr for that file.
inode_ptr getValue(inode_state& state, string str)
{
   inode_ptr value;
   for (auto &i : state.fileNames) 
   {
      if (i.first == str) 
      {
         value = i.second;
         break;
      }
   }
   return value;
}

//Enter the node_ptr to get the file name.
string getKey(inode_state& state, inode_ptr ptr)
{
   string key;
   for (auto &i : state.fileNames) 
   {
      if (i.second == ptr) 
      {
         key = i.first;
         break;
      }
   }
   return key;
}

string convertVec(const wordvec& words)
{   wordvec myvec = words;
    myvec.erase (myvec.begin());
    string result;
    for (auto const& s : myvec) { result += s; }
      return result;
}

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string str = convertVec(words);
   string mesge = "cat: " + str + ": No such file or directory.";

   map<string, string>::iterator it;
   if((it = state.fileContents.find(str)) != state.fileContents.end())
      {
         mesge = it->second;
      }

   cout << mesge << endl;
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string rt = convertVec(words);

   //If words size = 1, print out all the file names under cwd.
   //If words size <= 1, then print out contents of cwd.
   if(words.size() == 1)
   {
      string mesge;
      for(auto it=state.fileNames.begin(); 
         it!=state.fileNames.end(); it++)
      {
         if(it->second == state.getCwd())
         {
            mesge = mesge + " " + it->first;
         }
         else
            continue;
      }
      cout << mesge << endl;
   }
   else if(rt[0] == '/')
   {
      //Print out the fileNames under the root directory.
      inode_ptr itsRoot = getValue(state, ". ..");
      string mesge;
      for(auto it=state.fileNames.begin(); 
         it!=state.fileNames.end(); it++)
      {
         if(it->second == itsRoot)
         {
            mesge = mesge + " " + it->first;
         }
         else
            continue;
      }
      cout << mesge << endl;

   }
   else
   {
      string input = convertVec(words);
      if(input[input.size()-1] == '/')
         input.pop_back();

      string file_name;
      for(int i = input.size()-1; i >= 0; i--)
      {
         if(input[i] == '/')
            break;
         else
         {
           file_name += input[i];
         }
      }
      reverse(file_name.begin(), file_name.end());

      inode_ptr thisPtr= getValue(state, file_name);
      string mesge;

      for(auto it=state.fileNames.begin(); 
         it!=state.fileNames.end(); it++)
      {
         if(it->second == thisPtr)
         {
            mesge = mesge + " " + it->first;
         }
         else
            continue;
      }
      cout << mesge << endl;
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   //Adds a new file under the current working directory.

   if(words.size() <= 1)
   {
      throw command_error("make: *** No targets specified. \n");
   }
   else
   {
      string str;
      for (int i = 2; i < words.size(); i++)
      {
         if(i == words.size() - 1)
            str += words[i];
         else
            str += words[i] + " ";
      }
      state.fileContents.insert({words[1], str});
      state.fileNames.insert({words[1], state.getCwd()});
   }

}

void fn_mkdir (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() <= 1)
   {
      throw command_error("mkdir: *** No targets specified. \n");
   }

   string direc_name = convertVec(words);
   inode_ptr new_inode = make_shared<inode>(file_type::DIRECTORY_TYPE);
   state.directories.insert({direc_name, new_inode});

   //set cwd
   state.setCwd(new_inode);
   state.filepath = state.filepath + direc_name + "/";

}

void fn_prompt (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   wordvec thePrompt = words;
   string theStringPrompt;
   theStringPrompt= convertVec(thePrompt);
   state.prompt(theStringPrompt);
}

void fn_pwd (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   cout << state.filepath << endl;
}

void fn_rm (inode_state& state, const wordvec& words)
{
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string file_name = convertVec(words);
   state.fileNames.erase (file_name);

   inode_ptr fileptr = getValue(state, file_name);

   state.directories.erase(file_name);
   state.fileContents.erase(file_name);

   for(auto it=state.directories.begin(); 
      it!=state.directories.end(); it++)
   {
      if(it->second == fileptr)
      {
         state.directories.erase(it->first);
      }
      else
         continue;
   }

}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

