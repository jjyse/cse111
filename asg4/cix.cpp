// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstdio>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

//Client module.
//Interacts with the user and reads in commands from the terminal.
//Connects to the given host and port.
//Client exits by closing the socket.

logstream outlog (cout);

//Quit the program. An end of file marker or Control/D is
//equivalent.
struct cix_exit: public exception {};

//A mapping of cix commands.
unordered_map<string,cix_command> command_map 
{
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls", cix_command::LS},
   {"get", cix_command::GET},
   {"put", cix_command::PUT},
   {"rm", cix_command::RM}
};

//A summary of available commands is printed.
void cix_help() 
{
     cout << "\ncix_command::EXIT "
     << "Internal to cix, not used in communication."
     << "\ncix_command::GET "
     << "Request a file from the server. " 
     << "The filename is used both remotely and locally. "
     << "The payload length is 0."
     << "\ncix_command::HELP "
     << "Internal to cix, not used in communication. "
     << "\ncix_command::LS "
     << "Request file (ls) information. " 
     << "The payload length and filename are zeroed."
     << "\ncix_command::PUT "
     << "The length of the payload is the number of bytes"
     << " in the file. The contents of the file "
     << "immediately follow the header. "
     << "The bytes of the payload are unstructured and may"
     << " contain null bytes. Binary files are acceptable."
     << "\ncix_command::RM "
     << "Request to remove a file. The payload length is 0."
     << endl; 
}

//Remote server will execute the command ls -1.
//Prints a list of files in remote directory, line by line.
void cix_ls (client_socket& server) 
{
   //Create a new header.
   cix_header header;
   //The header's command is ls.
   header.command = cix_command::LS;
   //Print a message to the output.
   outlog << "sending header " << header << endl;
   //Send a packet through the socket.
   send_packet (server, &header, sizeof header);
   //Recieve a packet through the socket.
   recv_packet (server, &header, sizeof header);
   //Print a message to the output.
   outlog << "received header " << header << endl;
   //Error handling.
   if (header.command != cix_command::LSOUT) 
   {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }
   //Execute ls command.
   else 
   {
      //nbytes is the number of bytes in the payload.
      //Use ntohl to take care of endianness before sending data.
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      //Add the end of file marker (null character).  
      buffer[host_nbytes] = '\0';
      //Print out the characters read from the file.
      cout << buffer.get();
   }
}

//Copy the file named filename on the remote server and create or
//overwrite a file of the same name in the current directory.
void cix_get(client_socket& server, string file_name)
{
   //Create a new header.
   cix_header header;
   //The header's command is get.
   header.command = cix_command::GET;
   //Add the header's filename.
   strcpy(header.filename, file_name.c_str());
   //Print a message to the output.
   outlog << "sending header " << header << endl;
   //Send a packet through the socket.
   send_packet (server, &header, sizeof header);
   //Recieve a packet through the socket.
   recv_packet (server, &header, sizeof header);
   //Print a message to the output.
   outlog << "received header " << header << endl;
   //Error handling.
   if (header.command != cix_command::GET) 
   {
      outlog << "sent GET, server did not return GET" << endl;
      outlog << "server returned " << header << endl;
   }
   //Execute get command.
   else 
   {
      //nbytes is the number of bytes in the payload.
      //Use ntohl to take care of endianness before sending data.
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      //Add the end of file marker (null character).  
      buffer[host_nbytes] = '\0';
      //Print out the characters read from the file.
      cout << buffer.get();
   }
}

//Copies a local file into the socket and causes the remote
//server to create that file in its directory.
void cix_put(client_socket& server, string file_name)
{
   //Create a new header.
   cix_header header;
   //The header's command is get.
   header.command = cix_command::PUT;
   //Add the header's filename.
   strcpy(header.filename, file_name.c_str());
   //Print a message to the output.
   outlog << "sending header " << header << endl;
   //Send a packet through the socket.
   send_packet (server, &header, sizeof header);
   //Recieve a packet through the socket.
   recv_packet (server, &header, sizeof header);
   //Print a message to the output.
   outlog << "received header " << header << endl;
   //Error handling.
   if (header.command != cix_command::PUT) 
   {
      outlog << "sent PUT, server did not return PUT" << endl;
      outlog << "server returned " << header << endl;
   }
   //Execute get command.
   else 
   {
      //nbytes is the number of bytes in the payload.
      //Use ntohl to take care of endianness before sending data.
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      //Add the end of file marker (null character).  
      buffer[host_nbytes] = '\0';
      //Print out the characters read from the file.
      cout << buffer.get();
   }
}

//Causes the remote server to remove the file.
void cix_rm(client_socket& server, string file_name)
{
   //Create a new header.
   cix_header header;
   //The header's command is ls.
   header.command = cix_command::RM;
   //Add the header's filename.
   strcpy(header.filename, file_name.c_str());
   //Print a message to the output.
   outlog << "sending header " << header << endl;
   //Send a packet through the socket.
   send_packet (server, &header, sizeof header);
   //Recieve a packet through the socket.
   recv_packet (server, &header, sizeof header);
   //Print a message to the output.
   outlog << "received header " << header << endl;
   //Error handling.
   if (header.command != cix_command::RM) 
   {
      outlog << "sent RM, server did not return RM" << endl;
      outlog << "server returned " << header << endl;
   }
   //Execute ls command.
   else 
   {
      //nbytes is the number of bytes in the payload.
      //Use ntohl to take care of endianness before sending data.
      size_t host_nbytes = ntohl (header.nbytes);

      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      //Add the end of file marker (null character).  
      buffer[host_nbytes] = '\0';
      //Print out the characters read from the file.
      cout << buffer.get();
   }
}


void usage() 
{
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) 
{
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) 
      {
         //Store the input command.
         string line;
         //Store the input filename.
         string file_name;
         string space = " ";
         //Get user input.
         getline(cin, line);
         size_t found = line.find(space); 
         //If we find a space, split the string.
         if (found != string::npos) 
         {
            file_name = line.substr(found+1, line.length()-1);
            line = line.substr(0, found);
         }

         if(file_name.size() > 59)
         {
            outlog << file_name << ": invalid filename" << endl;
            break;
         }                             
         if (cin.eof()) throw cix_exit();
         outlog << "command " << line << endl;
         const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) 
         {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               line.erase();
               file_name.erase();
               break;
            case cix_command::LS:
               cix_ls(server);
               line.erase();
               file_name.erase();
               break;
            case cix_command::GET:
               cix_get (server, file_name);
               line.erase();
               file_name.erase();
               break;
            case cix_command::PUT:
               cix_put (server, file_name);
               line.erase();
               file_name.erase();
               break;
            case cix_command::RM:
               cix_rm (server, file_name);
               line.erase();
               file_name.erase();
               break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

