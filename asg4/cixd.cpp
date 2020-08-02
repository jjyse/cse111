// $Id: cixd.cpp,v 1.8 2019-04-05 15:04:28-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

//Server/daemon module. 
//Listens for client connection requests in an infinite loop.
//Uses fork to create a child process, listens for connections
//on the given port.
//Requests recieved are followed by a send.
//Server exits when the client disconnects.

logstream outlog (cout);
struct cix_exit: public exception {};

//Server's response to client ls request.
void reply_ls (accepted_socket& client_sock, cix_header& header) 
{
   const char* ls_cmd = "ls -l 2>&1";
   FILE* ls_pipe = popen (ls_cmd, "r");
   if (ls_pipe == NULL) { 
      outlog << "ls -l: popen failed: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   int status = pclose (ls_pipe);
   if (status < 0) outlog << ls_cmd << ": " << strerror (errno) << endl;
              else outlog << ls_cmd << ": exit " << (status >> 8)
                          << " signal " << (status & 0x7F)
                          << " core " << (status >> 7 & 1) << endl;
   header.command = cix_command::ACK;
   header.command = cix_command::LSOUT;
   header.nbytes = htonl (ls_output.size());
   memset (header.filename, 0, FILENAME_SIZE);
   outlog << "sending header " << header << endl;
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str(), ls_output.size());
   outlog << "sent " << ls_output.size() << " bytes" << endl;
}

void reply_get (accepted_socket& client_sock, cix_header& header)
{
   //Get cwd
   const char* pwd_cmd = "pwd";
   //Use popen to execute pwd. pwd_pipe is the stream that contains
   //data returned by pwd. "r" to read from stream. "w" to write to
   //stream.
   FILE* pwd_pipe = popen (pwd_cmd, "r");
   //If popen returns null (error), exit out of the program.
   if (pwd_pipe == NULL) 
   {      
      outlog << "ls -l: popen failed: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   //The output of the ls command will go in pwd_output.
   string pwd_output;
   //Initialize a character array.
   char buffer1[0x1000];
   //Loop to extract characters from the stream into pwd_output.
   for (int i = 0; i < 60; i++) 
   {
      //fgets() reads characters from the stream and stores them 
      //in buffer. Let rcg be a pointer to buffer, which is returned
      //by fgets().
      char* rcg = fgets (buffer1, sizeof buffer1, pwd_pipe);
      //Break once we encounter a nullptr.
      if (rcg == nullptr) break;
      //Add each non-null character into pwd_output.
      pwd_output.append (buffer1);
   }
   //Close the pipe and check for errors.
   int status1 = pclose (pwd_pipe);
   if (status1 < 0) outlog << pwd_cmd 
      << ": " << strerror (errno) << endl;
              else outlog << pwd_cmd << ": exit " << (status1 >> 8)
                          << " signal " << (status1 & 0x7F)
                          << " core " << (status1 >> 7 & 1) << endl; 
   header.command = cix_command::ACK;
   pwd_output.erase(pwd_output.length()-7);
   pwd_output.append("local");

   //Concatenate the whole command. c_str adds null terminators.
   char str[150];
   str[0] = '\0';
   string comm = "cp ";
   string space = " ";
   string comm2 = " 2>&1";
   strcat(str, comm.c_str());
   strcat(str, header.filename);
   strcat(str, space.c_str());
   strcat(str, pwd_output.c_str());
   //Direct the output to the client.
   strcat(str, comm2.c_str());
   const char* cp_cmd = str;
   //Use popen to execute cp. cp_pipe is the stream that contains
   //data returned by cp. "r" to read from stream. "w" to write to
   //stream.
   FILE* cp_pipe = popen (cp_cmd, "r");
   //If popen returns null (error), exit out of the program.
   if (cp_pipe == NULL) 
   { 
      outlog << "popen failed: " << strerror (errno) << endl;
      //Set the header command = NAK
      header.command = cix_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   //The output of the ls command will go in cp_output.
   string cp_output;
   //Initialize a character array.
   char buffer[0x1000];
   //Loop to extract characters from the stream into cp_output.
   for (;;) 
   {
      //fgets() reads characters from the stream and stores them 
      //in buffer. Let rc be a pointer to buffer, which is returned
      //by fgets().
      char* rc = fgets (buffer, sizeof buffer, cp_pipe);
      //Break once we encounter a nullptr.
      if (rc == nullptr) break;
      //Add each non-null character into cp_output.
      cp_output.append (buffer);
   }
   //Close the pipe and check for errors.
   int status = pclose (cp_pipe);
   if (status < 0) outlog << cp_cmd << ": " << strerror (errno) << endl;
              else outlog << cp_cmd << ": exit " << (status >> 8)
                          << " signal " << (status & 0x7F)
                          << " core " << (status >> 7 & 1) << endl;
   header.command = cix_command::ACK;
   //Set the header command to be GET (if this line is executed, this
   //means that there were no errors).
   header.command = cix_command::GET;
   //Set the header's nbytes value (payload size).
   header.nbytes = htonl (cp_output.size());
   //Use memset to fill the entire filename with 0's.
   memset (header.filename, 0, FILENAME_SIZE);
   //Print out the status message.
   outlog << "sending header " << header << endl;
   //Send the header response.
   send_packet (client_sock, &header, sizeof header);
   //Send the requested data.
   send_packet (client_sock, cp_output.c_str(), cp_output.size());
   //Print out the status message.
   outlog << "sent " << cp_output.size() << " bytes" << endl;
}

void reply_put (accepted_socket& client_sock, cix_header& header)
{
   //Get cwd
   const char* pwd_cmd = "pwd";
   //Use popen to execute pwd. pwd_pipe is the stream that contains
   //data returned by pwd. "r" to read from stream. "w" to write to
   //stream.
   FILE* pwd_pipe = popen (pwd_cmd, "r");
   //If popen returns null (error), exit out of the program.
   if (pwd_pipe == NULL) 
   {      
      outlog << "ls -l: popen failed: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   //The output of the ls command will go in pwd_output.
   string pwd_output;
   //Initialize a character array.
   char buffer1[0x1000];
   //Loop to extract characters from the stream into pwd_output.
   for (int i = 0; i < 60; i++) 
   {
      //fgets() reads characters from the stream and stores them 
      //in buffer. Let rcg be a pointer to buffer, which is returned
      //by fgets().
      char* rcg = fgets (buffer1, sizeof buffer1, pwd_pipe);
      //Break once we encounter a nullptr.
      if (rcg == nullptr) break;
      //Add each non-null character into pwd_output.
      pwd_output.append (buffer1);
   }
   //Close the pipe and check for errors.
   int status1 = pclose (pwd_pipe);
   if (status1 < 0) outlog << pwd_cmd << ": " 
      << strerror (errno) << endl;
              else outlog << pwd_cmd << ": exit " << (status1 >> 8)
                          << " signal " << (status1 & 0x7F)
                          << " core " << (status1 >> 7 & 1) << endl;
   header.command = cix_command::ACK; 
   pwd_output.erase(pwd_output.length()-7);
   string local_direct = pwd_output;
   string remote_direct = pwd_output;
   local_direct.append("local/");
   remote_direct.append("remote");

   //Concatenate the whole command. c_str adds null terminators.
   char str[150];
   str[0] = '\0';
   string comm = "rsync ";
   string space = " ";
   string comm2 = " 2>&1";
   strcat(str, comm.c_str());
   strcat(str, local_direct.c_str());
   strcat(str, header.filename);
   strcat(str, space.c_str());
   strcat(str, remote_direct.c_str());
   strcat(str, space.c_str());
   strcat(str, comm2.c_str());

   const char* cp_cmd = str;
   //Use popen to execute cp. cp_pipe is the stream that contains
   //data returned by cp. "r" to read from stream. "w" to write to
   //stream.
   FILE* cp_pipe = popen (cp_cmd, "r");
   //If popen returns null (error), exit out of the program.
   if (cp_pipe == NULL) 
   { 
      outlog << "popen failed: " << strerror (errno) << endl;
      //Set the header command = NAK
      header.command = cix_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   //The output of the ls command will go in cp_output.
   string cp_output;
   //Initialize a character array.
   char buffer[0x1000];
   //Loop to extract characters from the stream into cp_output.
   for (;;) 
   {
      //fgets() reads characters from the stream and stores them 
      //in buffer. Let rc be a pointer to buffer, which is returned
      //by fgets().
      char* rc = fgets (buffer, sizeof buffer, cp_pipe);
      //Break once we encounter a nullptr.
      if (rc == nullptr) break;
      //Add each non-null character into cp_output.
      cp_output.append (buffer);
   }
   //Close the pipe and check for errors.
   int status = pclose (cp_pipe);
   if (status < 0) outlog << cp_cmd << ": " << strerror (errno) << endl;
              else outlog << cp_cmd << ": exit " << (status >> 8)
                          << " signal " << (status & 0x7F)
                          << " core " << (status >> 7 & 1) << endl;
   header.command = cix_command::ACK;
   //Set the header command to be PUT (if this line is executed, this
   //means that there were no errors).
   header.command = cix_command::PUT;
   //Set the header's nbytes value (payload size).
   header.nbytes = htonl (cp_output.size());
   //Use memset to fill the entire filename with 0's.
   memset (header.filename, 0, FILENAME_SIZE);
   //Print out the status message.
   outlog << "sending header " << header << endl;
   //Send the header response.
   send_packet (client_sock, &header, sizeof header);
   //Send the requested data.
   send_packet (client_sock, cp_output.c_str(), cp_output.size());
   //Print out the status message.
   outlog << "sent " << cp_output.size() << " bytes" << endl;
}

//Use unlink for rm.
void reply_rm (accepted_socket& client_sock, cix_header& header)
{
   int x = 0;
   x = unlink(header.filename);
   if(x == -1)
    {      
      outlog << "Cannot remove file : " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   else
      header.command = cix_command::ACK;
   send_packet(client_sock, &header, sizeof header);
}


void run_server (accepted_socket& client_sock) 
{
   outlog.execname (outlog.execname() + "-server");
   outlog << "connected to " << to_string (client_sock) << endl;
   try {   
      for (;;) {
         cix_header header; 
         recv_packet (client_sock, &header, sizeof header);
         outlog << "received header " << header << endl;
         switch (header.command) {
            case cix_command::LS: 
               reply_ls(client_sock, header);
               break;
            case cix_command::GET: 
               reply_get (client_sock, header);
               break;
            case cix_command::PUT: 
               reply_put (client_sock, header);
               break;
            case cix_command::RM: 
               reply_rm (client_sock, header);
               break;
            default:
               outlog << "invalid client header:" << header << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   throw cix_exit();
}

//Each connection to the client causes the server to fork a child
//process to serve the client. This function is invoked once
//a client requests a connection.
void fork_cixserver (server_socket& server, accepted_socket& accept) 
{
   pid_t pid = fork();
   if (pid == 0) { // child
      server.close();
      run_server (accept);
      throw cix_exit();
   }else {
      accept.close();
      if (pid < 0) {
         outlog << "fork failed: " << strerror (errno) << endl;
      }else {
         outlog << "forked cixserver pid " << pid << endl;
      }
   }
}


void reap_zombies() 
{
   for (;;) 
   {
      int status;
      pid_t child = waitpid (-1, &status, WNOHANG);
      if (child <= 0) break;
      outlog << "child " << child
             << " exit " << (status >> 8)
             << " signal " << (status & 0x7F)
             << " core " << (status >> 7 & 1) << endl;
   }
}

void signal_handler (int signal) {
   outlog << "signal_handler: caught " << strsignal (signal) << endl;
   reap_zombies();
}

void signal_action (int signal, void (*handler) (int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigfillset (&action.sa_mask);
   action.sa_flags = 0;
   int rc = sigaction (signal, &action, nullptr);
   if (rc < 0) outlog << "sigaction " << strsignal (signal)
                      << " failed: " << strerror (errno) << endl;
}


int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   signal_action (SIGCHLD, signal_handler);
   in_port_t port = get_cix_server_port (args, 0);
   try {
      server_socket listener (port);
      for (;;) {
         outlog << to_string (hostinfo()) << " accepting port "
             << to_string (port) << endl;
         accepted_socket client_sock;
         for (;;) {
            try {
               listener.accept (client_sock);
               break;
            }catch (socket_sys_error& error) {
               switch (error.sys_errno) {
                  case EINTR:
                     outlog << "listener.accept caught "
                         << strerror (EINTR) << endl;
                     break;
                  default:
                     throw;
               }
            }
         }
         outlog << "accepted " << to_string (client_sock) << endl;
         try {
            fork_cixserver (listener, client_sock);
            reap_zombies();
         }catch (socket_error& error) {
            outlog << error.what() << endl;
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

