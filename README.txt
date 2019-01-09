Terezie (Tera) Schaller
Nov 25, 2018
CS 372 - project 2

Description
This is a simple file transfer protocol program. Files in the same directory as ftserver.c can be transferred to the directory with ftclient.py. 

Running Instructions
1. To begin make sure ftserver.c and ftclient.py are in separate folders. Have 2 windows open running different flip servers. This program was tested with ftserver.c on flip2 and ftclient.py on flip3.

2. In the window/directory with ftclient.py, enter the command:

chmod +x ftclient.py

to give execution permissions.

3. Compile ftserver.c with the following command:

gcc -o ftserver ftserver.c

There may be a warning error about "passing argument 2 of ‘accept’ ... " which can be ignored for the purposes of this assignment. 

4. Begin running the ftserver with the following command:

./ftserver <server port number> <host of ftclient.py>

server port number should be between 1024 to 65535
host of ftclient is the flip server where the python ftclient will be running

Example: 

./ftserver 12520 flip3

will begin the server listening on port 12520 for a client running on flip3

5. Use the ftclient.py to request data from the server with the command:

./ftclient.py <host of ftserver> <ftserver port> <command> <optional: filename> <data port>

host of ftserver - this is the version of flip where ftserver is running
ftserver port - this is the port that ftserver is listening on (the port you gave to ftserver when it started)
command - -l to list directories or -g to get files
optional: filename - for use only if -g was the command, this is the filename you wish to request. 
data port - is the port where you wish the client to listen for incoming data. This should be between 1024 to 65535 and different from the server port.

Example 1(if server is already running on flip 2 at port 12520):

./ftclient.py flip2 12520 -l 12530

This will list all files in the same directory as the ftserver, and receives the data over port number 12530. 

Example 2(if server is already running on flip 2 at port 12520 and contains a file called test1.txt):

./ftclient.py flip2 12520 -g test1.txt 12530

This will result in a file in the same directory as ftclient.py named test1.txt and identical contents

6. Client will exit when transfer is complete. Client may be re-run to make additional requests of the server. Shut down server with Ctrl-C.

Notes to the grader
1. All requirements were met on the first run/loop of the server program. The server continues listening and accepting additional commands after the client has shut down and been re-run; however, some bugs may appear on subsequent loops. I corrected as many as I could, but some may still remain. If having trouble with a request/command, please try stopping and re-running the server. 

2. Testing notes: I wrote the code and did preliminary testing by transferring small files and printing the results to the console while the data was being written (print statements removed in final submission), checking that a file had been made, and using cat (for small files) or head and tail for large files to view the contents of the file. Both large (~6MB) and small files appeared readable and the same as original during preliminary testing. Using ls -l filename, I confirmed that transferred file was the same size as the original file. Using diff showed that the files were the same. Additional tests may be needed to confirm that the files are identical; however, preliminary testing suggests that the transfer is successful. Files used in testing are included with assignment submission as test1.txt and big.txt.

3. Files should have spaces and newlines at intervals similar to readable text to transfer. Behavior on files with no spaces or newlines is undefined. 

4. I apologize for the minimalist modularization and overall lack of readability of my code. I worked incredibly hard to get a version of this program working. I find it hard to modularize when programming with sockets. I didn't have much time to go back and clean it up and make it readable. This will be a great portfolio piece for future job interviews so I will most definitely go do that over the break.  

Thank you for your time and effort in reviewing my code! 



