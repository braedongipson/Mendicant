#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "FileSys.h"
#include <unistd.h>

#include <strings.h>
#include <cstring>

#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./nfsserver port#\n";
        return -1;
    }
    int port = atoi(argv[1]);

    //networking part: create the socket and accept the client connection
    int sock;
    int server = socket(AF_INET, SOCK_STREAM, 0); //change this line when necessary!
    if (server <= 0) {
        cout << "server error " << server << endl;
        return -1;
    }
    sockaddr_in myAddr;
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(port);
    socklen_t myAddrLen = sizeof(myAddr);


    if (bind(server, (struct sockaddr*)&myAddr, myAddrLen)){
        cout << "Bind error" << endl;
        perror("Error: ");
        return -2;
    }
    cout << server << endl;
    if(listen(server, 3) < 0) {
        cout << "Listen error" << endl;
        return -3;
    }
    if((sock = accept(server, (struct sockaddr*)&myAddr, (socklen_t*)&myAddrLen)) < 0){
        cout << "accept" << endl;
        return -4;
    }

    //mount the file system
    FileSys fs;
    fs.mount(sock); //assume that sock is the new socket created
    //for a TCP connection between the client and the server.

    //loop: get the command from the client and invoke the file
    char buff[20];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, 20 );

        // read the message from client and copy it in buffer
        int x = read(sock, buff, sizeof(buff));
        /*
        if ( x > 0)
            cout << x << endl;
*/
        stringstream s(buff);
        string comm;
        string name;
        getline(s, comm, ' ');
        getline(s, name, ' ');
/*
        cout << "Command: " << comm << endl;
        cout << "Name: " << name << endl;
*/
        if (comm == "mkdir")
            fs.mkdir(name.c_str());
        else if (comm == "cd")
            fs.cd(name.c_str());
        else if (comm == "home")
            fs.home();
        else if (comm == "rmdir")
            fs.rmdir(name.c_str());
        else if (comm == "ls")
            fs.ls();
        else if (comm == "create")
            fs.create(name.c_str());
        else if (comm == "cat")
            fs.cat(name.c_str());
        else if (comm == "append"){
            string data;
            getline(s, data);
            fs.append(name.c_str(), data.c_str());
        }
        else if (comm == "head"){
            string data;
            getline(s, data);
            fs.head(name.c_str(), stoul(data));
        }
        else if (comm == "rm")
            fs.rm(name.c_str());
        else if (comm == "stat")
            fs.stat(name.c_str());

        bzero(buff, 20);
        n = 0;

    }



    //system operation which returns the results or error messages back to the clinet
    //until the client closes the TCP connection.



    //close the listening socket
    close(sock);
    //unmout the file system
    fs.unmount();

    return 0;
}