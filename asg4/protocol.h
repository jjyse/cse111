// $Id: protocol.h,v 1.7 2019-05-15 15:26:36-07 - - $

#ifndef __PROTOCOL__H__
#define __PROTOCOL__H__

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

#include "sockets.h"

//Enum class: user defined data types.
//Must use scope qualifier to access the enumerator. 
//error = 0, exit = 1, get = 2, ...
enum class cix_command : uint8_t 
{
   ERROR = 0, EXIT, GET, HELP, LS, PUT, RM, FILEOUT, LSOUT, ACK, NAK,
};
constexpr size_t FILENAME_SIZE = 59;

//The header consists of a struct of 64 bytes.
constexpr size_t HEADER_SIZE = 64;
//A header is created each time a connection to the server is 
//made. A header contains the size of the payload, a
//command, and the filename. 
struct cix_header 
{
   //nbytes is the number of bytes in the payload if
   //there is any payload. Otherwise = 0. This field
   //loads and stores data with ntohl and htonl.
   uint32_t nbytes {0};
   //Let command = 0;
   cix_command command {cix_command::ERROR};
   //The name of the file being tranferred or
   //removed. The filename may not have any slash
   //characters, must be null terminated with
   //'\0', and must have < 59 characters. All bytes 
   //following the null termination must also be null.
   char filename[FILENAME_SIZE] {};
};

//Check if true when the code is compiled, if not true,
//stop the compilation process and issue an error message.
static_assert (sizeof (cix_header) == HEADER_SIZE);

void send_packet (base_socket& socket,
                  const void* buffer, size_t bufsize);

void recv_packet (base_socket& socket, void* buffer, size_t bufsize);

ostream& operator<< (ostream& out, const cix_header& header);

string get_cix_server_host (const vector<string>& args, size_t index);

in_port_t get_cix_server_port (const vector<string>& args,
                               size_t index);

#endif

