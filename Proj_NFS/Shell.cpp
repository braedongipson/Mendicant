// CPSC 3500: Shell
// Implements a basic shell (command line interface) for the file system

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
using namespace std;

#include "Shell.h"

static const string PROMPT_STRING = "NFS> ";	// shell prompt

// Mount the network file system with server name and port number in the format of server:port
void Shell::mountNFS(string fs_loc) {
    string host = fs_loc.substr(0,fs_loc.find(':'));
    string port = fs_loc.substr(fs_loc.find(':') + 1, fs_loc.length());
    //create the socket cs_sock and connect it to the server and port specified in fs_loc
    struct addrinfo hints, *servinfo, *p;
    cout << host << " " << port << endl;
    memset(&hints, 0 , sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //host = htonl((uint32_t*)&host);
    if ((getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo)) != 0){
        return;
    }
    for(p = servinfo; p; p = p->ai_next) {
        cs_sock = socket(p->ai_family, p->ai_socktype, 0);
        if (cs_sock == -1) {
            cout << "error with socket" << endl;
            continue;
        }
        cout << "made it here" << endl;
        if(connect(cs_sock, p->ai_addr, p->ai_addrlen) == -1){
            cout << "error with connection" << endl;
            perror("error: ");
            close(cs_sock);
            continue;
        }
        //if all the above operations are completed successfully, set is_mounted to true
        is_mounted = true;
        freeaddrinfo(servinfo);

        return;
    }


}

// Unmount the network file system if it was mounted
void Shell::unmountNFS() {
    if(is_mounted){
        is_mounted = false;
        close(cs_sock);
    }
}

// Remote procedure call on mkdir
void Shell::mkdir_rpc(string dname) {
    //send command to server
    dname = "mkdir " + dname;

    send(cs_sock, dname.c_str(), strlen(dname.c_str()), 0);
    //receive response code
    char msg[512];

    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on cd
void Shell::cd_rpc(string dname) {
    dname = "cd " + dname;
    //char *out = new char[dname.length()+1];
    //strcpy(out, dname.c_str());

    send(cs_sock, dname.c_str(), strlen(dname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on home
void Shell::home_rpc() {
    string dname = "home ";

    send(cs_sock, dname.c_str(), strlen(dname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on rmdir
void Shell::rmdir_rpc(string dname) {
    dname = "rmdir " + dname;
    send(cs_sock, dname.c_str(), strlen(dname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on ls
void Shell::ls_rpc() {
    string name = "ls ";
    //char *out = new char[dname.length()+1];
    //strcpy(out, dname.c_str());

    send(cs_sock, name.c_str(), strlen(name.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on create
void Shell::create_rpc(string fname) {
    fname = "create " + fname;
    send(cs_sock, fname.c_str(), strlen(fname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on append
void Shell::append_rpc(string fname, string data) {
    fname = "append " + fname + " " + data;
    send(cs_sock, fname.c_str(), strlen(fname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procesure call on cat
void Shell::cat_rpc(string fname) {
    fname = "cat " + fname;
    send(cs_sock, fname.c_str(), strlen(fname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on head
void Shell::head_rpc(string fname, int n) {
    fname = "head " + fname + " " + to_string(n);

    send(cs_sock, fname.c_str(), strlen(fname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on rm
void Shell::rm_rpc(string fname) {
    fname = "rm " + fname;
    send(cs_sock, fname.c_str(), strlen(fname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Remote procedure call on stat
void Shell::stat_rpc(string fname) {
    fname = "stat " + fname;
    send(cs_sock, fname.c_str(), strlen(fname.c_str()), 0);
    //receive response code
    char msg[512];
    int bytes = recv(cs_sock, msg, sizeof(msg), 0);
    msg[bytes] = '\0';
    //output result
    cout << msg << endl;
}

// Executes the shell until the user quits.
void Shell::run()
{
    // make sure that the file system is mounted
    if (!is_mounted)
        return;
    // continue until the user quits
    bool user_quit = false;
    while (!user_quit) {

        // print prompt and get command line
        string command_str;
        cout << PROMPT_STRING;
        getline(cin, command_str);

        // execute the command
        user_quit = execute_command(command_str);
    }

    // unmount the file system
    unmountNFS();
}

// Execute a script.
void Shell::run_script(char *file_name)
{
    // make sure that the file system is mounted
    if (!is_mounted)
        return;
    // open script file
    ifstream infile;
    infile.open(file_name);
    if (infile.fail()) {
        cerr << "Could not open script file" << endl;
        return;
    }


    // execute each line in the script
    bool user_quit = false;
    string command_str;
    getline(infile, command_str, '\n');
    while (!infile.eof() && !user_quit) {
        cout << PROMPT_STRING << command_str << endl;
        user_quit = execute_command(command_str);
        getline(infile, command_str);
    }

    // clean up
    unmountNFS();
    infile.close();
}


// Executes the command. Returns true for quit and false otherwise.
bool Shell::execute_command(string command_str)
{
    // parse the command line
    struct Command command = parse_command(command_str);

    // look for the matching command
    if (command.name == "") {
        return false;
    }
    else if (command.name == "mkdir") {
        mkdir_rpc(command.file_name);
    }
    else if (command.name == "cd") {
        cd_rpc(command.file_name);
    }
    else if (command.name == "home") {
        home_rpc();
    }
    else if (command.name == "rmdir") {
        rmdir_rpc(command.file_name);
    }
    else if (command.name == "ls") {
        ls_rpc();
    }
    else if (command.name == "create") {
        create_rpc(command.file_name);
    }
    else if (command.name == "append") {
        append_rpc(command.file_name, command.append_data);
    }
    else if (command.name == "cat") {
        cat_rpc(command.file_name);
    }
    else if (command.name == "head") {
        errno = 0;
        unsigned long n = strtoul(command.append_data.c_str(), NULL, 0);
        if (0 == errno) {
            head_rpc(command.file_name, n);
        } else {
            cerr << "Invalid command line: " << command.append_data;
            cerr << " is not a valid number of bytes" << endl;
            return false;
        }
    }
    else if (command.name == "rm") {
        rm_rpc(command.file_name);
    }
    else if (command.name == "stat") {
        stat_rpc(command.file_name);
    }
    else if (command.name == "quit") {
        return true;
    }

    return false;
}

// Parses a command line into a command struct. Returned name is blank
// for invalid command lines.
Shell::Command Shell::parse_command(string command_str)
{
    // empty command struct returned for errors
    struct Command empty = {"", "", ""};

    // grab each of the tokens (if they exist)
    struct Command command;
    istringstream ss(command_str);
    int num_tokens = 0;
    if (ss >> command.name) {
        num_tokens++;
        if (ss >> command.file_name) {
            num_tokens++;
            if (ss >> command.append_data) {
                num_tokens++;
                string junk;
                if (ss >> junk) {
                    num_tokens++;
                }
            }
        }
    }

    // Check for empty command line
    if (num_tokens == 0) {
        return empty;
    }

    // Check for invalid command lines
    if (command.name == "ls" ||
        command.name == "home" ||
        command.name == "quit")
    {
        if (num_tokens != 1) {
            cerr << "Invalid command line: " << command.name;
            cerr << " has improper number of arguments" << endl;
            return empty;
        }
    }
    else if (command.name == "mkdir" ||
             command.name == "cd"    ||
             command.name == "rmdir" ||
             command.name == "create"||
             command.name == "cat"   ||
             command.name == "rm"    ||
             command.name == "stat")
    {
        if (num_tokens != 2) {
            cerr << "Invalid command line: " << command.name;
            cerr << " has improper number of arguments" << endl;
            return empty;
        }
    }
    else if (command.name == "append" || command.name == "head")
    {
        if (num_tokens != 3) {
            cerr << "Invalid command line: " << command.name;
            cerr << " has improper number of arguments" << endl;
            return empty;
        }
    }
    else {
        cerr << "Invalid command line: " << command.name;
        cerr << " is not a command" << endl;
        return empty;
    }

    return command;
}
