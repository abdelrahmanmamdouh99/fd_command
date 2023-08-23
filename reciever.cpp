#include <iostream>
#include <fstream>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
 
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

#define ByteSize 1024
#define BackingFile DirectoryOfSharedMem
#define DirectoryOfSharedMem "/home/abdel/sharedMemTask/sharedmem"



#define pipe_file "pipe.txt"


void report_and_exit(const char* msg){
perror(msg);
exit(-1);

}

int main(){
/***************create pipe****************/
int fd_pipe;

/* Create the FIFO if it does not exist */
mkfifo(pipe_file, S_IFIFO|0640);

fd_pipe = open(pipe_file, O_RDWR);

write(fd_pipe, "tast.txt", strlen("tast.txt"));
	/*
	sleep - This is to make sure other process reads this, otherwise this
	process would retrieve the message
	*/
	sleep(2);
// Close the write end of the pipe in the sender process
/*******************************************************************/

/*************create shared memory and semaphore**************/
int            fd;
const char     *shmpath;

shmpath="sharedMem";
const char* SemaphoreName ="Sema";
sem_t* semptr = sem_open(SemaphoreName,O_CREAT,0600,0);

fd = shm_open(shmpath, O_RDWR, 0600);
char* memptr=(char*)mmap(NULL,ByteSize,PROT_READ,MAP_SHARED,fd,0);
if((char*) -1 ==memptr) report_and_exit("can't access segment");
/***************************************************************/

/**********open the whole file from shared memory*********/
//used the help of chatgpt here
 // Get the file size of the shared memory segment
struct stat shmInfo;
fstat(fd, &shmInfo);
off_t sharedMemorySize = shmInfo.st_size;

// Write shared memory content to an output file
std::ofstream outputFile("recieved.txt", std::ios::binary);
outputFile.write(static_cast<char*>(memptr), sharedMemorySize);
/*****************************************/




munmap(memptr, ByteSize);
close(fd);
sem_close(semptr);
unlink(BackingFile);

return 0;

}
