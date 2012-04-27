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

 
#include "chatnetclient.h"
#include <iostream>

using std::cout;
using std::cin;
using std::string;

//----------------- chatnetclient class methods -----------------//

//--------------- This function connects to chatnet Server ------------------//
//--------- On success it will return 0 ----------//
//--------- on failure -1 is returned -----------//
int chatnetclient::ConnectToServer(const char * host, const char * port)
{

  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_UNSPEC; // force ipv6 or ipv4
  hints.ai_socktype = SOCK_STREAM;
  if((rv = getaddrinfo(host,port,&hints,&servinfo)) != 0)
  {
    perror(gai_strerror(rv)); // print error to stderr
    return 1; // return 1 because of an erorr
  }
  
  for(p = servinfo; p != NULL; p = p->ai_next)
  {
    if((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
    {
      perror("socket"); // erorr with socket
      cout << "Error: socket...trying again...\n"; 
      continue;
    }
    
    // connect
    if(connect(sockfd,p->ai_addr, p->ai_addrlen) == -1)
    {
    perror("connect");
    cout << "Error: connect...trying again...\n";
    close(sockfd);
    continue;
    }
    
    // if connection is succefull break the loop
    break;
}

if(p == NULL)
{
  cout << "Unable to resolve host\n";
   return 1;
}

freeaddrinfo(servinfo);

return 0;
}

/*----------- This function breaks up messages received from chatnet server
 into individual fields so they are easy to work with ---------- */
void chatnetclient::ParseServerMsg()
{
output_counter = 0;
socket_buffer_counter = 0;

while((socket_input_buffer[socket_buffer_counter] != 0) && (socket_input_buffer[socket_buffer_counter] != ' '))
  field_1[output_counter++] = socket_input_buffer[socket_buffer_counter++];
  field_1[output_counter] = 0; // terminate the string
  
  field_2[0] = '\0';
 
if(socket_input_buffer[socket_buffer_counter])
{
 socket_buffer_counter++; // skipe space
 output_counter = 0;
 while((socket_input_buffer[socket_buffer_counter] != 0) && (socket_input_buffer[socket_buffer_counter] != ' '))
  field_2[output_counter++] = socket_input_buffer[socket_buffer_counter++];
  field_2[output_counter] = 0; // terminate the string
}
field_3[0] = '\0';
if(socket_input_buffer[socket_buffer_counter])
{
 socket_buffer_counter++; // skipe space
 output_counter = 0;
 while((socket_input_buffer[socket_buffer_counter] != 0) && (socket_input_buffer[socket_buffer_counter] != ' '))
  field_3[output_counter++] = socket_input_buffer[socket_buffer_counter++];
  field_3[output_counter] = 0; // terminate the string
}
}

//----------- This function gets the name of the client --------//
int chatnetclient::GetName()
{
  int recv_bytes; // holds the number of bytes received
 string cmp; // used to compare the received buffer
  do{
    cout << "Enter your nickname: ";
    getline(cin, name); // get users name
    sprintf(buffer, "%s\n",name.c_str());
    if(send(sockfd,buffer,strlen(buffer),0) == -1)
    {
      perror("send");
      cout << "Error sending to chatnet server.\n";
   
      return 1;
    }
   //----------- recv respond from chatnet server ------------//
  recv_bytes = recv(sockfd,buffer,sizeof(buffer) - 1,0);
  if(recv_bytes == -1 || recv_bytes == 0)
  {
  cout << "Error receiving from server.\n";
  return 1;
  }
  
  buffer[recv_bytes] = 0; // terminate the received string
   cmp = buffer;

   /*-------- If respond from chatnet server is not 'NAMEACCEPTED', then is probably
    an error. display the error to the user------------*/
  if(cmp != "NAMEACCEPTED")
  {temp_int = 0; 
    while(buffer[temp_int++] != ':')
  {};
 
    while(buffer[temp_int++] != '\0')
    cout << buffer[temp_int];
  }

  } while(cmp != "NAMEACCEPTED");
 
 cout << "Your name has been succefully accepted.\n";
 
 return 0; // no problem
}


void chatnetclient::Mainloop(const char * host, const char * port)
{
 
if(ConnectToServer(host,port) != 0)
{
  exit(EXIT_FAILURE);  
}

//get the client's name 

if(GetName() != 0)
{
 exit(EXIT_FAILURE); 
}

// now begin serving the user
FD_ZERO(&masters);
FD_ZERO(&readfds);
FD_SET(sockfd, &masters);
FD_SET(STDIN_FILENO, &masters);

inchat = false;
int len;
cout << "--- Ready to serve ---\n";
while(true)
{ 
readfds = masters;
rv = select(sockfd + 1, &readfds,NULL,NULL,NULL);
if(rv == -1)
{
  perror("select");
  exit(EXIT_FAILURE);
}

if(FD_ISSET(STDIN_FILENO,&readfds))
{
  bzero(terminal_buffer,1024); // clear the buffer up
 len = read(STDIN_FILENO,terminal_buffer,sizeof(terminal_buffer) - 2);
 
 //---------------- Check if command size is large ---------------//

 // if something is received, do checks against it.
 if(len)
 {
if(len > 546)
{
system("clear");
cout << "Input is too long!\n";
continue;
}
      
   terminal_buffer[len] = 0; // terminate the string

   // is it a command
   if(terminal_buffer[0] == '/')
   {  

       // lets use ParseServerMsg to parse input
       // Server command
       strcpy(socket_input_buffer,terminal_buffer);
       ParseServerMsg();
      
       if(!strcmp(field_1, "/chat") && field_2[0] != '\0' && field_3[0] == '\0')
       {            
	            field_2[strlen(field_2) -1] = '\0';  
	        
		   //------- No problem, not trying to chat with ur self ---------//
	           if(inchat == false)
		   {     
                person_inchat = field_2;
		if(person_inchat != name)
		  inchat = true;
		else
		{ cout << "You can't chat to your self.\n";
		continue;
		}
		   }
		   //------- Problem user is already in a chat session --------//
		   else 
		   {
		     cout <<"Your are already chating with " << person_inchat << "\n";
		     continue; // goto the top
		   }
		    
		   
               }
         
               // user is leaving
     if(!strcmp(terminal_buffer, "/exit\n"))
     {
         //if this client is in chat let the other client know he's gone
         if(inchat == true)
         { 
	   
             strcpy(message,name.c_str());
             strcat(message, " Has left the chat session.\n");
             if(SendMsg(person_inchat,message) != 0)
             {
                 cout << "Error sending to chatnet server.\n";
                 exit(EXIT_FAILURE);
             }
         }
         cout << "Exiting...\n";
         exit(EXIT_SUCCESS);
     }
     if(!strcmp(terminal_buffer, "/endchat\n"))
     {cout << "Chat session with " << person_inchat << " has ended.\n";
       strcpy(message,name.c_str());
             strcat(message, " Has ended the chat session.\n");
             if(SendMsg(person_inchat,message) != 0)
             {
                 cout << "Error sending to chatnet server.\n";
                 exit(EXIT_FAILURE);
             }
             inchat = false;
     }
       

     if(send(sockfd,terminal_buffer,strlen(terminal_buffer),0) == -1)
     {
       perror("send");

       exit(EXIT_FAILURE);
     }
     
   }


   // it's a message to a client
   
   if(inchat == true && terminal_buffer[0] != '/')
   {
       if(SendMsg(person_inchat, terminal_buffer) != 0)
       {   
           exit(EXIT_FAILURE);
       }
   
     }

   // user is trying to send a message
   if(inchat == false && terminal_buffer[0] != '/')
       cout << "Your are not in a chat session!\n";
   
   }
  
 }
 
 
if(FD_ISSET(sockfd,&readfds))
{
    bzero(socket_input_buffer, 1024);

  len = recv(sockfd,socket_input_buffer,sizeof(socket_input_buffer),0);
  if(len == -1 || len == 0)
  { 
    cout << "Host closed connection\n";
    exit(EXIT_FAILURE);
  }
  
 
  socket_input_buffer[len] = 0; // terminate the string received

  // no problem check what the received command is
  ParseServerMsg();
  if(!strcmp(field_1, "/msg"))
     {
         // a message
         // print senders name
      if(person_inchat == field_2)
      {
      cout << "<" << field_2 << "> ";
         temp_int = 0;
         while(socket_input_buffer[temp_int++] != ':')
         {}; // locate the start of the message

         // print the message
                   temp_int = temp_int - 1;
                     while(socket_input_buffer[temp_int++] != '\0')
             cout << socket_input_buffer[temp_int];
         cout << "\n";
	
         continue;
      }
     }
  // This message is sent by chatnet server if a message was
  // not sent to the user
  else if(!strcmp(socket_input_buffer, "MSGNOTSENT"))
  {

      cout << "Your message was not sent to " << person_inchat << ".\n";
      cout << "The user you are trying to send the message to might be offline.\n";
  }
  // A ping from the server. Deal with it
else if(!strcmp(socket_input_buffer, "PING"))
  { 
    strcpy(buffer,"PING\n");
      if(send(sockfd,buffer,strlen(buffer), 0) == -1)
      {  std::cerr << "Connection might have been terminated by server.\n";
          exit(EXIT_FAILURE);
      }
     

  }
  // a chat
  else if(!strcmp(field_1,"xchat"))
  {
  if(inchat == false)
  {
  person_inchat = field_2;
  cout << "Your are now in a chat session with " << person_inchat << ".\n";
  inchat = true;
  }
  else
  {
   sprintf(message,"%s %s",name.c_str(), " is already in a chat session\n");
   if(SendMsg(field_2,message) != 0)
   continue;
  }
}
else
 // just display what has been received
 cout << socket_input_buffer;
 
}

}
 
}

//----------------- This function sends a chat message to chatnet server to be sent to a client ---------//
int chatnetclient::SendMsg(const string & username, const char * msg)
{
  //------- Check the size of the msg ----------------//
  if(strlen(msg) > 512)
  { 
    std::cerr << "Message is too long!\n";
    return 2;
  }
  //---- No Problem send the message -------/
    strcpy(buffer,"/msg ");
    strcat(buffer, username.c_str());
    strcat(buffer, " :");
    strncat(buffer, msg,strlen(msg));
    if(send(sockfd, buffer, strlen(buffer), 0) == -1)
        return 1; // Error sending 

    // at success
    return 0;
}

/*
void chatnetclient::Ping()
{
    strcpy(buffer,"PING\n");
      if(send(sockfd,buffer,strlen(buffer), 0) == -1)
      {
          perror("send");
	  cerr << "Connection terminated by server.\n";
          exit(EXIT_FAILURE);
      }
}*/