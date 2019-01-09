#!/usr/bin/env python

#############################
#** Program Filename: ftclient.py
#** Author: Terezie Schaller
#** Date:11/25/2018
#** Description:Client half of a python program using socket API to make a simple file transfer program.
#** Input:needs server host, server port number, command (-l or -r), filename(optional), data port number
#** Output:depending on command, with -l will list directories in ftserver's current directory
#** Output: with -g and valid filename will create a new file wih contents of file in ftserver's directory
#############################

# run the scrip from the shell
# making it executable (chmod +x ftclient.py) and then running it directly: ./ftclient.py

#References
#CS372 Intro to Computer Networks lecture example
#https://stackabuse.com/command-line-arguments-in-python/
#https://www.tutorialspoint.com/python/python_functions.htm
#https://stackoverflow.com/questions/15587877/run-a-python-script-in-terminal-without-the-python-command
#https://stackoverflow.com/questions/2429511/why-do-people-write-the-usr-bin-env-python-shebang-on-the-first-line-of-a-pyt/2429517
#https://www.youtube.com/watch?v=LJTaPaFGmM4
#https://realpython.com/python-sockets/
#https://docs.python.org/2/library/socket.html
#DUH
#https://www.tutorialspoint.com/python/python_if_else.htm
#https://stackoverflow.com/questions/82831/how-do-i-check-whether-a-file-exists-without-exceptions

import sys
from socket import *
import os.path

#validate command line parameters
################################
#** Function: validateCmdLn()
#** Description: checks basic command line inputs
#** Parameters: none
#** Pre-Conditions: valid number of arguments and port number must be specified on command line
#** Post-Conditions: program allowed to continue
################################
def validateCmdLn():
        numargs = len(sys.argv)
        if numargs < 5 or numargs > 6:
            print "Invalid number of arguments. Goodbye."
            sys.exit()

        if (sys.argv[3] == "-g" and numargs == 5):
            print "Enter a filename with the -g command. Goodbye"
            sys.exit()


        if (int(sys.argv[-1]) < 1024 or int(sys.argv[-1]) > 65535):
            print "Invalid portnumber"
            print "Try again and ener a port number from 1024 to 65535"
            sys.exit()


#create dataSocket and begin listening
################################
#** Function: makeDataSocket()
#** Description: starts the server listening on the port specified by the command line
#** Parameters: none
#** Pre-Conditions: valid port number must be specified on command line
#** Post-Conditions: socket open and listening on the given port number, socket object is returned
################################
def makeDataSocket():
        dataPort = int(sys.argv[-1])
        dataSocket = socket(AF_INET, SOCK_STREAM)
        dataSocket.bind(('',dataPort))
        dataSocket.listen(5)
        print "Now listening on dataport: " + str(dataPort)
        newdataSocket, addr = dataSocket.accept() #https://realpython.com/python-sockets/
        return newdataSocket

#initiates the TCP control connection
################################
#** Function: initControl()
#** Description: contacts the server listening on the control port specified by the command line
#** Parameters: none
#** Pre-Conditions: valid port number must be specified on command line, server must be listening
#** Post-Conditions: connection established, socket object returned
################################

def initControl():
    print "Initiates TCP control connection."
    #initiates control connection with ftserver and sends data port number
    serverName = sys.argv[1]
    serverPort = int(sys.argv[2])
    controlSocket = socket(AF_INET, SOCK_STREAM)
    controlSocket.connect((serverName, serverPort))
    controlSocket.send(sys.argv[-1]) #send the dataport to the server
    reply = controlSocket.recv(1024)
    if "good" == reply:
        print "TCP control connection established: " + str(controlSocket)
    return controlSocket

#sends command and processes
#error message if applicable
def makeRequest():
    pass
#receives data and performs
#correct actions according to
#command
def receiveData():
    pass
#main function
def main():
    print "ftclient started"

    #validate command line parameters
    validateCmdLn()

    #set up socket to server
    controlSocket = initControl()

    #receive message on dataSocket
    dataSocket = makeDataSocket()
    newData = dataSocket.recv(1024)
    print "Recieving on dataSocket: " + newData

    #send command to ftserver.c on control connection
    print "Sending command on control socket."
    controlSocket.send(sys.argv[3])


    #if command is -l prepare to recv and
    #print the directory listening on Data Socket
    if (sys.argv[3] == "-l"):
        print "-l was sent to ftserver."
        count = 0
        while (len(newData)):
            newData = dataSocket.recv(1024)
            print newData
    #if command is -g send filename
    elif (sys.argv[3] == "-g"):
        print "-g was sent to the ftserver."
        print "sending filename to ftserver: " + sys.argv[4]
        #try sending filename on the control socket
        controlSocket.send(sys.argv[4])
        #get response on data socket
        response = dataSocket.recv(1024)
        print "Does file exist: " + response
        #if file exisits (check for EXISTS in response):
            #open new file and begin writing data to file
        if (response == "EXISTS"):
            print "Copying file"
            #open a new file and begin writing the data
            #print it out just for testing
            #avoid accidentally overwriting an already existing file
            #by checking for file and asking user if they want to proceed
            if (os.path.exists(sys.argv[4])):
                print "*** Caution, file already exits. ***"
                ans = raw_input("Do you want to continue and overwrite existing file? (y/n):")
                if (ans == "y"):
                    newfile = sys.argv[4]
                else:
                    sys.exit()
            else: newfile = sys.argv[4]
            myfile = open(newfile, "w") #open a new file for writing
            fileData = dataSocket.recv(1024) #receive some data
            myfile.write(fileData) #begin writing data to file
            while(len(fileData)): #continue as long as there is incomming data
                fileData = dataSocket.recv(1024) #receive new data
                myfile.write(fileData) #add it to file
            myfile.close()
        #else if file does not exist (check for NOPE) print error message
        elif (response == "NOPE"):
            print "File not found"

    #else if not -l or -g, then receive and print error msg
    #and quit

    #receive done message on control connection
    finalMsg = controlSocket.recv(1024)
    print "From control socket: " + finalMsg

    #this is the server for the data connection
    #close data connecion and listening sockets
    dataSocket.close()

    #close the control socket(P)
    controlSocket.close()

#end main function

#call main function

if __name__ == '__main__':
    main()
