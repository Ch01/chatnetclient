 /* Copyright (C) 2012 Chol Nhial <ch01.cout@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
 
#ifndef CHATNETCLIENT_H_INCLUDED
#define CHATNETCLIENT_H_INCLUDED

#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <string>
#include <netinet/in.h>
#include <netdb.h>

using std::string;
//--------- chatnetclient main class ------------//
class chatnetclient
{
private:
int sockfd,rv;
struct addrinfo hints, *servinfo, *p;
char buffer[4096];
char socket_input_buffer[1024];
char terminal_buffer[1024];
char field_1[256], field_2[256], field_3[256];
int output_counter;
int socket_buffer_counter,terminal_input_counter;
int temp_int;
bool inchat;
std::string person_inchat;
std::string name;
char message[64];
fd_set readfds;
fd_set masters;
public:
void ParseServerMsg();
int ConnectToServer(const char * host, const char * port);
int SendMsg(const string & username, const char * msg);
int GetName();
void Mainloop(const char * host, const char * port);
void Ping();
};
#endif // CHATNETCLIENT_H_INCLUDED

