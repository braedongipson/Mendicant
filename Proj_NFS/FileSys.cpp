// CPSC 3500: File System
// Implements the file system commands that are available to the shell.

#include <cstring>
#include <iostream>
#include <unistd.h>
using namespace std;

#include "FileSys.h"
#include "BasicFileSys.h"
#include "Blocks.h"
#include <sys/socket.h>

// mounts the file system
void FileSys::mount(int sock) {
    bfs.mount();
    curr_dir = 1; //by default current directory is home directory, in disk block #1
    fs_sock = sock; //use this socket to receive file system operations from the client and send back response messages
}

// unmounts the file system
void FileSys::unmount() {
    bfs.unmount();
    close(fs_sock);
}

// make a directory
void FileSys::mkdir(const char *name) //works
{
    //directory already exists
    if(curr_dir != findFile(name)){
        sendStuff("Directory already exists");
        return;
    }
    //get current block
    struct dirblock_t parent;
    bfs.read_block(curr_dir, (void*)&parent);

    if(parent.num_entries+1 > MAX_DIR_ENTRIES){
        sendStuff("Too many directories");
        return;
    }

    if (strlen(name) > MAX_FNAME_SIZE){
        sendStuff("504 File name is too long");
        return;
    }
    struct dirblock_t block;
    block.magic = DIR_MAGIC_NUM;
    block.num_entries = 0;
    for (int i = 0; i < MAX_DIR_ENTRIES; i++) {
        block.dir_entries[i].block_num = 0;
    }
    //write the data first

    short block_num = bfs.get_free_block();
    /*
if (block_num == 0){
cout << "505 Disk is full" << endl;
return;
}
*/
    bfs.write_block(block_num, (void*) &block);
    //update the directory inode second
    parent.dir_entries[parent.num_entries].block_num = block_num;
    for (int i = 0; i <MAX_FNAME_SIZE; i++) {
        parent.dir_entries[parent.num_entries].name[i]= name[i];
    }
    parent.num_entries++;
    bfs.write_block(curr_dir, (void*) &parent);
    sendStuff("200 OK");
}

// switch to a directory
void FileSys::cd(const char *name) //works
{
    short old = curr_dir;
    cout<<name<<endl;
    short check = findFile(name);
    if (!is_directory(check)){
        sendStuff("500 File is not a directory\n");
        return;
    }
    curr_dir = check;
    cout<<curr_dir<<endl;
    if(old == curr_dir){
        sendStuff("503 File does not exist\n");
        return;
    }
    sendStuff("200 OK\n");
}

// switch to home directory
void FileSys::home() { //works lol
    curr_dir = 1;
}

// remove a directory
void FileSys::rmdir(const char *name) //works
{
    if(!exists(name)){
        sendStuff("503 File does not exist\n");
        return;
    }

    short check = findFile(name);
    if (!is_directory(check)){
        sendStuff("500 File is not a directory\n");
        return;
    }

    struct dirblock_t current;
    bfs.read_block(curr_dir, (void*)&current);
    int block_num;
    int index;
    for(int i = 0; i<current.num_entries; i++){
        if(strcmp(name, current.dir_entries[i].name) == 0){
            block_num = current.dir_entries[i].block_num;
            index = i;
            break;
        }
    }
    if(index == current.num_entries){
        return;
    }
    struct dirblock_t block;
    bfs.read_block(block_num, (void*)&block);
    if(block.num_entries != 0){
        sendStuff("507 Directory is not empty\n");
        return;
    }
    if(current.num_entries > 1){
        current.dir_entries[index] = current.dir_entries[current.num_entries-1];
    }
    current.num_entries--;
    bfs.write_block(curr_dir, (void*) &current);
    bfs.reclaim_block(block_num);
    sendStuff("200 OK\n");
}

// list the contents of current directory
void FileSys::ls() //works
{
    struct dirblock_t current;
    bfs.read_block(curr_dir, (void *) &current);
    for(int i =0; i<current.num_entries; i++){
        sendStuff(current.dir_entries[i].name);
        //differentiate directories and files
        struct dirblock_t check;
        bfs.read_block(current.dir_entries[i].block_num, (void*)&check);
        if (check.magic == DIR_MAGIC_NUM){
            sendStuff("/");
        }
        sendStuff("\n");
    }
    sendStuff("200 OK\n");
}

// create an empty data file
void FileSys::create(const char *name) //unsure
{

    if(findFile(name) != curr_dir) {
        sendStuff("503 File exists\n");
        return;
    }
    if (strlen(name) > MAX_FNAME_SIZE){
        sendStuff("504 File name is too long\n");
        return;
    }

    struct dirblock_t parent;
    bfs.read_block(curr_dir, (void*)&parent);

    if(parent.num_entries+1 > MAX_DIR_ENTRIES){
        sendStuff("506 Directory is Full\n"); //need to send back error message
        return;
    }

    //STEP 2: INODE
    struct inode_t metaData; // create inode to hold metadata about file
    short i_block = bfs.get_free_block(); //MAY FAIL CHECK RETURN
    /*
    if (i_block == 0){
        cout << "505 Disk is full" << endl;
        return;
    }
     */

    metaData.magic = INODE_MAGIC_NUM;
    metaData.size = 0;
    for (int i = 0; i < MAX_DATA_BLOCKS; i++){
        metaData.blocks[i] = 0;
    }
    bfs.write_block(i_block, (void*) &metaData);
    bfs.read_block(i_block, (void *) &metaData);
    /*
    for (int i = 0; i < 60; i++) {
        cout << metaData.blocks[i] << ' ';
    }
    cout << endl;
*/
    //STEP 3: ADD DIRECTORY ENTRY

    //update the directory inode
    parent.dir_entries[parent.num_entries].block_num = i_block;
    for (int i = 0; i <MAX_FNAME_SIZE; i++) {
        parent.dir_entries[parent.num_entries].name[i]= name[i];
    }
    parent.num_entries++;
    bfs.write_block(curr_dir, (void*) &parent);
    sendStuff("200 OK\n");
}

// append data to a data file
void FileSys::append(const char *name, const char *data) //unsure
{
    short current = findFile(name);
    if (curr_dir == current)
        sendStuff("503 File does not exist\n");

    if (is_directory(current)){
        sendStuff("501 File is a directory\n");
        return;
    }

    //search parent directory to find file (send error message if not)
    struct inode_t metaData;
    bfs.read_block(current, (void*) &metaData);

    if(metaData.size + strlen(data) > MAX_FILE_SIZE){
        sendStuff("508 Append exceeds maximum file size\n");
        return;
    }

    //inode# for file
    unsigned num_blocks = metaData.size % BLOCK_SIZE == 0? (metaData.size / BLOCK_SIZE) : (metaData.size / BLOCK_SIZE) + 1;

    unsigned offset = 0;
    unsigned space_left = BLOCK_SIZE;
    if ((metaData.blocks[num_blocks] == 0) && metaData.size == 0){ //metaData.blocks[num_blocks-1] will give you last block (this catches everything) else won't be called
        metaData.size += BLOCK_SIZE;
        metaData.blocks[num_blocks] = bfs.get_free_block();
        num_blocks = 1;
        offset = 0;
    }
    else{//current block contains data
        short last_block_index = metaData.blocks[num_blocks-1];
        struct datablock_t check_data;
        bfs.read_block(last_block_index,(void*) &check_data);
        for (int i = 0; i < BLOCK_SIZE; i++)
            if (check_data.data[i] != 0)
                offset++;
        offset += 1;//dont want to write over the last char of the last entry
        space_left = BLOCK_SIZE - offset;
        if(space_left < strlen(data)){//not enough space, need to allocate new block
            metaData.size += BLOCK_SIZE;
            metaData.blocks[num_blocks] = bfs.get_free_block();
            num_blocks += 1;
        }
    }
    int left = MAX_FILE_SIZE - metaData.size;
    cout << "NUM BLOCKS " << num_blocks << endl;
    cout << "OFFSET: " << offset << endl;
    cout << "SPACE LEFT IN BLOCK: " << space_left << endl;
    cout << "SPACE LEFT IN FILE: " <<  left << endl;

    bfs.write_block(current, (void *)&metaData);

    short index;
    num_blocks = num_blocks - 1 < 0? 0: num_blocks - 1;
    index = metaData.blocks[num_blocks];
    struct datablock_t file_data;
    bfs.read_block(index, (void *)&file_data);
    int p = 0;
    for (int k = offset; k < BLOCK_SIZE && p < strlen(data); k++){
        file_data.data[k] = data[p];
        p++;
        // cout << file_data.data[k] << ' ';
    }
    bfs.write_block(index, (void *)&file_data);
}


// display the contents of a data file
void FileSys::cat(const char *name) //unsure
{
    short current = findFile(name);

    if (curr_dir == current) {
        sendStuff("503 File does not exist\n");
        return;
    }


    struct inode_t metaData;
    bfs.read_block(current, (void*) &metaData);
    unsigned num_blocks = metaData.size % BLOCK_SIZE == 0? metaData.size / BLOCK_SIZE : (metaData.size / BLOCK_SIZE) + 1;
    cout << "NUMBLOCKS " << num_blocks << endl;

    for (int i = 0; i < num_blocks; i++){
        struct datablock_t fileData;
        cout << "block index: " << metaData.blocks[i] <<  endl;
        bfs.read_block(metaData.blocks[i], (void*) &fileData);
        for (int k = 0; k < BLOCK_SIZE; k++){
            cout << fileData.data[k];
        }
        cout << endl;
    }
    sendStuff("200 OK\n");
}

// display the first N bytes of the file
void FileSys::head(const char *name, unsigned int n)
{
    short current = findFile(name);

    if (curr_dir == current) {
        sendStuff("503 File does not exist\n");
        return;
    }

    struct inode_t metaData;
    bfs.read_block(current, (void*) &metaData);
    int lim = n;
    unsigned num_blocks;
    if (n > BLOCK_SIZE ) {
        num_blocks = n % BLOCK_SIZE == 0 ? n / BLOCK_SIZE : (n / BLOCK_SIZE) + 1;
        lim = BLOCK_SIZE;
    }
    else{
        num_blocks = 1;
        lim = n;
    }

    cout << "NUMBLOCKS " << num_blocks << endl;

    for (int i = 0; i < num_blocks; i++){
        struct datablock_t fileData;
        cout << "block index: " << metaData.blocks[i] <<  endl;
        bfs.read_block(metaData.blocks[i], (void*) &fileData);
        for (int k = 0; k < BLOCK_SIZE; k++){
            cout << fileData.data[k];
        }
        cout << endl;
    }
    cout << "200 OK" << endl;
}

// delete a data file
void FileSys::rm(const char *name) //unsure
{
    //check if file is directory or data
    if(!exists(name)){
        sendStuff("503 File does not exist\n");
        return;
    }
    struct dirblock_t current;
    bfs.read_block(curr_dir, (void*)&current);
    int block_num;
    int index;
    for(int i = 0; i<current.num_entries; i++){
        if(strcmp(name, current.dir_entries[i].name) == 0){
            block_num = current.dir_entries[i].block_num;
            index = i;
            break;
        }
    }
    if(index == current.num_entries){
        return;
    }

    if(current.num_entries > 1){
        current.dir_entries[index] = current.dir_entries[current.num_entries-1];
    }
    current.num_entries--;
    bfs.write_block(curr_dir, (void*) &current);
    bfs.reclaim_block(block_num);

    sendStuff("200 OK\n");
}

// display stats about file or directory
void FileSys::stat(const char *name)
{
    struct dirblock_t current;
    short block_num = findFile(name);
    if (curr_dir == block_num) {
        sendStuff("503 File does not exist\n");
        return;
    }

    bfs.read_block(block_num, (void*) &current);
    string out;
    if (!is_directory(block_num)){ //is an inode
        inode_t *p = (inode_t*) &current; //cast to inode
        unsigned num_blocks = p->size == 0? 1 : p->size / BLOCK_SIZE;
        num_blocks = (p->size > 0 && num_blocks < 2)? 2 : num_blocks;

        out = "Inode block: " + to_string(block_num)+ "\n";
        sendStuff(out.c_str());

        out = "Bytes in file: " + to_string(p->size) + "\n";
        sendStuff(out.c_str());

        out = "Number of Blocks: " + to_string(num_blocks)+ "\n";
        sendStuff(out.c_str());

        out = "First Block: " + to_string(p->blocks[0]) + "\n";
        sendStuff(out.c_str());

    }
    else{//is directory
        string dname = name;
        out = "Directory name: " + dname + "/" + "\n";
        sendStuff(out.c_str());

        out = "Directory block: " + to_string(block_num) + "\n";
        sendStuff(out.c_str());

    }

    sendStuff("200 OK\n");
}

// HELPER FUNCTIONS (optional)

bool FileSys::is_directory(short block_num)
{
    struct dirblock_t current;
    bfs.read_block(block_num, (void* )&current);

    if (current.magic == DIR_MAGIC_NUM)
        return true;
    else if(current.magic == INODE_MAGIC_NUM) //once you check the magic number,
        return false; //if block is inode_t *p = (inode_t *) &buff;
    else
        return false;
}

short FileSys::findFile(const char *name){
    struct dirblock_t current;
    bfs.read_block(curr_dir, (void* )&current);
    for(int i = 0; i<current.num_entries; i++){
        if(strcmp(name, current.dir_entries[i].name) == 0){
            return current.dir_entries[i].block_num;
        }
    }
    return curr_dir;
}

bool FileSys::exists(const char *name){
    struct dirblock_t current;
    bfs.read_block(curr_dir, (void* )&current);
    for(int i = 0; i<current.num_entries; i++){
        if(strcmp(name, current.dir_entries[i].name) == 0){
            return true;
        }
    }
    return false;
}

void FileSys::sendStuff(const char *message){
    //send(fs_sock, message, strlen(message), 0);
    int bytes_sent = 0;
    const char *p = message;
    while(bytes_sent < sizeof(message)){
        int x = write(fs_sock, (void*)p, sizeof(message) - bytes_sent);
        if(x == -1 || x == 0){
            perror("write");
            close(fs_sock);
            exit(1);
        }
        p += x;
        bytes_sent += x;
    }
}