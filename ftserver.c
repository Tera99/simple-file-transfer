/*********************************************************************
 * ** Program Filename: ftserver.c
 * ** Author: Terezie (Tera) Schaller
 * ** Date: 11/25/2018
 * ** Description: server half (in C) of a simple file transfer program using socket API.
 * ** Input: port number and client host name on command line
 * ** Output: listens for a TCP connection on a control socket, when control connections
 * ** is established, opens a second TCP connection for data transfer
 * *********************************************************************/

//references
//CS 344 lecture material, Spring 2018, Benjamin Brewster
//CS 372 assignment 1 code, see references there as well
//https://ubuntuforums.org/showthread.php?t=1449331
//list programs in directory in C
//https://www.sanfoundry.com/c-program-list-files-directory/
//comparing C strings
//https://www.techonthenet.com/c_language/standard_library_functions/string_h/strcmp.php
//copying strings
//http://www.cplusplus.com/reference/cstring/strcpy/
//C file access
//CS344 lectures, Benjamin Brewster, Spring 2018
//https://www.tutorialspoint.com/cprogramming/c_file_io.htm
//https://stackoverflow.com/questions/9434207/reading-from-file-using-fgets
//https://www.tutorialspoint.com/cprogramming/c_file_io.htm
//test large text file
//https://norvig.com/big.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>

/*********************************************************************
 * ** Function: Error function
 * ** REFERENCE: Benjamin Brewster, CS 344 lecture examples
 * ** Description: prints error message and exits
 * ** Parameters: error message
 * ** Pre-Conditions: none
 * ** Post-Conditions: program exits
 * *********************************************************************/

void error(const char *msg) { perror(msg); exit(1); }//error function



int main( int argc, char *argv[])
{
  //define variables
  int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
  socklen_t sizeOfClientInfo;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;

  //define variables for dataSocket
  int dataPortNum;
  int dataSocketFD;
  int dataWritten;
  struct sockaddr_in dataAddr;
  struct hostent* dataClientInfo;
  char dataBuffer[1024];

  //more variables
  //parsing commands -l or -g
  int result; //for strcmp function
  int result2; //for strcmp function
  char myCommand[256]; //for strcmp function
  char myCommand2[256]; //for strcmp function
  //reading files in directory
  DIR *d;//for reading files in a direcory
  struct dirent *dir;//for reading files in a directory
  int dirsent; //sending directory info
  int size;
  //checking if a file exists or not
  int fileExists = 0; //0 if file does not exist, 1 if it does
  //opening and reading file
  FILE *fp;
  char myfile[256];

  //if (argc < 2) { fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); } //check args and usage

  //set up the address struct for the server
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear the address struct
  portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; //create a network capable socket
  //htons converts from host byte order to network order
  serverAddress.sin_port = htons(portNumber); //store the port number
  serverAddress.sin_addr.s_addr = INADDR_ANY; //allow any address for this connection to process

  //set up the socket
  listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); //create the socket
  if (listenSocketFD < 0) error("ERROR opening socket");

  //enable the socket to begin listening
  //connect socket to port
  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    error("ERROR on binding");
  //flip the socket on
  //it can now receive up to 5 connections
  listen(listenSocketFD, 5);

  //try putting the while loop here??
  while(1){

    //accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); //get the size of the address for the expected client
    //accept the connection
    establishedConnectionFD = accept(listenSocketFD, (struct sockAddr *)&clientAddress, &sizeOfClientInfo);
    if (establishedConnectionFD < 0) error("ERROR on accept");

    //get the dataport number from the client and display it
    memset(buffer, '\0', 256);
    charsRead = recv(establishedConnectionFD, buffer, 255, 0);//read the client's message from the socket
    if (charsRead < 0) error("ERROR reading from the socket");
    //printf("SERVER: I received this from the client: \"%s\"\n", buffer);
    //dataPort number is in the buffer as a string
    //convert it to int and save to variable
    dataPortNum = atoi(buffer);
    printf("Data Port Number: %d\n", dataPortNum);

    //send a success message back to the client
    charsRead = send(establishedConnectionFD, "good", 4, 0); //send success message back
    if(charsRead < 0) error("ERROR writing to socket");

    //set up address struct with dataport on ftclient.py
    //act as client to ftclient.py dataPort
    //relies heavily on B. Brewster CS344 lecture examples
    printf("Setting up address struct for data connection.\n");
    memset((char*)&dataAddr, '\0', sizeof(dataAddr)); //clear out the address struct
    dataAddr.sin_family = AF_INET; //create a network capable socket
    dataAddr.sin_port = htons(dataPortNum); //store the port portNumber
    dataClientInfo = gethostbyname(argv[2]); //convert the machine name into the special form of address
    if (dataClientInfo == NULL) { fprintf(stderr, "FTSERVER: ERROR, no such host\n"); exit(0); }
    //copy in the address
    memcpy((char*)&dataAddr.sin_addr.s_addr, (char*)dataClientInfo->h_addr, dataClientInfo->h_length);

    //open up a socket to dataport on ftclient.py
    printf("Opening up a data socket.\n");
    dataSocketFD = socket(AF_INET, SOCK_STREAM,0);
    if(dataSocketFD < 0) error("FTSERVER: ERROR opening socket");

  //connect to the dataPort/ftclient
    printf("Connecting to ftclient.");
    if(connect(dataSocketFD, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) error("FTSERVER: ERROR connecting");

  //send basic message to dataPort/ftclient
    //printf("sending to ftclient: Q is good\n");
    charsRead = send(dataSocketFD, "Q is good ", 9, 0); //send success message back
    if(charsRead < 0) error("ERROR writing to socket");

    //receive command from ftclient.py on control connection
    memset(buffer, '\0', 256);
    charsRead = recv(establishedConnectionFD, buffer, 255, 0);//read the client's message from the socket
    if (charsRead < 0) error("ERROR reading from the socket");
    printf("SERVER (control socket): I received this from the client: \"%s\"\n", buffer);

    //test if command is -l
    //trying str compare
    memset(myCommand, '\0', 256);
    strcpy(myCommand, "-l");
    result = strcmp(buffer, myCommand);
    //printf("result of strcmp: %d\n", result);

    //check if command is -g
    memset(myCommand2, '\0', 256);
    strcpy(myCommand2, "-g");
    result2 = strcmp(buffer, myCommand2);
    //printf("result2 is: %d\n", result2);

    //reset fileExists
    fileExists = 0;

    if (result == 0){ //if command is -l
      //send directory listing
      printf("-l command received, sending directory listing.\n");
      //reference
      //https://www.sanfoundry.com/c-program-list-files-directory/
      d = opendir(".");
      if (d)
      {
        while ((dir = readdir(d)) != NULL)
        {
          printf("%s\n", dir->d_name);
          dirsent = send(dataSocketFD, dir->d_name, strlen(dir->d_name), 0);
          usleep(200000); //need a slight pause so python can catch up and will print on seperate lines
        }
      closedir(d);
      }
    } else if (result2 == 0){ //if command is g
      printf("-g command received, getting filename.\n");
      memset(buffer, '\0', 256); //always clear the buffer
      charsRead = recv(establishedConnectionFD, buffer, 255, 0);//read the filename from the socket
      if (charsRead < 0) error("ERROR reading from the socket");
      printf("SERVER (control socket): I received this from the client: \"%s\"\n", buffer);
      //check if file exists
      //compare filename sent by ftclient to files in the directory
      d = opendir(".");
      if (d)
      {
        while ((dir = readdir(d)) != NULL)//read through files in directory
        {
          //printf("%s\n", dir->d_name);
          if (strcmp(buffer, dir->d_name) == 0){ //if a match is found
            fileExists = 1; //set file exits to 1/true
            strcpy(myfile, dir->d_name);//save filename
            printf("File found.\n");
          } else {
            //printf("File does not exist.\n");
          }

        }
      closedir(d);
      }
      //send result/response on dataSocket
      //printf("*** fileExists: %d ***\n", fileExists);
      memset(buffer, '\0', 256);
      if (fileExists == 1){
        strcpy(buffer, "EXISTS");
      } else {
        strcpy(buffer, "NOPE");
      }
      charsRead = send(dataSocketFD, buffer, strlen(buffer), 0); //send message back
      if(charsRead < 0) error("ERROR writing to socket");
    }

    //if file EXISTS
    //begin sending data to ftclient
    if (fileExists == 1){
      //open file for reading
      fp = fopen(myfile, "r");
      //readfile into buffer
      memset(buffer, '\0', 256);
      fgets(buffer, 255, (FILE*)fp); //reading the file line by line
      //send file
      charsRead = send(dataSocketFD, buffer, strlen(buffer), 0); //send success message back
      if(charsRead < 0) error("ERROR writing to socket");
      //keep reading and sending as long as there is data in the file
      while(fgets(buffer, 255, (FILE*)fp)){
        charsRead = send(dataSocketFD, buffer, strlen(buffer), 0); //send success message back
        if(charsRead < 0) error("ERROR writing to socket");
      }
      printf("done reading/sending\n");
      //close the file
      fclose(fp);

    }

    //send message on control connection again notifiying file is finished
    charsRead = send(establishedConnectionFD, "Data Transfer Complete", 22, 0); //send success message back
    if(charsRead < 0) error("ERROR writing to socket");

    close(dataSocketFD); //close the dataSocket(Q)
  }



  return 0;
} //end of main
