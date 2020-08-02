// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <libgen.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

static int count_1 = 0;
const string cin_name = "-";
using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) 
{
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

//Print to output
void catfile (istream& infile, const string& filename) 
{
   for(;;) 
   {
      count_1++;
      string line;
      getline (infile, line);
      if (infile.eof()) break;

      string before_eq; //Left value (or whole value if no equals)
      string after_eq;  //Right value
      int m = 0;
      for(int i = 0; i <= line.length(); i++)
         if(line[i] != '=')
         {
            before_eq.push_back(line[i]);
         }
         else
         {
            m = 1;
            for(int j = i+1; j <= line.length(); j++)
            {
               after_eq.push_back(line[j]);
            }
            break;
         }
      if(m == 1) //If an equals sign is input
      {
         cout << "-: " << count_1 << ": " << line << endl;
         cout << before_eq << " = " << after_eq << endl;
      }
      else       //If no equals sign
      {
         cout << "-: " << count_1 << ": " << line << endl;
         cout << before_eq << endl;
      }
   }

}

int main (int argc, char** argv) 
{
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   int status = 0;
   string progname (basename (argv[0]));
   vector<string> filenames (&argv[1], &argv[argc]);
   if (filenames.size() == 0) filenames.push_back (cin_name);
   //Call catfile
   for (const auto& filename: filenames) 
   {
      if (filename == cin_name) catfile (cin, filename);
      else 
      {
         ifstream infile (filename);
         if (infile.fail()) 
         {
            status = 1;
            cerr << progname << ": " << filename << ": "
                 << strerror (errno) << endl;
         }
         else 
         {
            catfile (infile, filename);
            infile.close();
         }
      }
   }

   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) 
   {
      //Create pair from input
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   cout << test.empty() << endl;
   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;


}

