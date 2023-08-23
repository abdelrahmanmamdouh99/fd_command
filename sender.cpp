//writter

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <fstream>
#include <iostream>


using namespace std;

//specify the size and some info for shared memory func
#define ByteSize 1024
#define BackingFile currentDir
#define currentDir "/home/abdel/sharedMemTask/"
#define FIFO_FILE "/home/abdel/sharedMemTask/pipe.txt"

void report_and_exit(const char* msg){
	perror(msg);
	exit(-1);
}

int main(){


/**********************************/
int fd_pipe;
int end_process;
int stringlen;
int read_bytes;
char readbuf[80];
char end_str[5];
printf("FIFO_CLIENT: Send messages, infinitely, to end enter \"end\"\n");
fd_pipe = open(FIFO_FILE, O_CREAT|O_RDWR);

read_bytes = read(fd_pipe, readbuf, sizeof(readbuf));
cout<<readbuf<<endl;
/********************************************/

/**************shared memory and semaphore****************/
int            fd;
const char     *shmpath;
shmpath="sharedMem";
const char* SemaphoreName ="Sema";
fd = shm_open(shmpath, O_CREAT | O_RDWR, 0600);
cout<<fd;
if(fd<0) report_and_exit("CAN't open shared mem");  		//cause if it fails it return -1
ftruncate(fd, ByteSize);
char* memptr =(char*) mmap(NULL,ByteSize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
if ((caddr_t) -1 ==memptr) report_and_exit("can't get segment...");
fprintf(stderr,"shared mem add: %p [0.....%d]\n", memptr, ByteSize -1);
fprintf(stderr,"backing file: /dev/shm%s\n",BackingFile);
sem_t* semptr = sem_open(SemaphoreName,O_CREAT,0600,0);
if(semptr==(void*) -1) report_and_exit("sem_open");
/**************************************************/

//with the help of chatgpt ^_^
// Open the source file
std::ifstream inputFile(readbuf, std::ios::binary);
if (!inputFile) {
	std::cerr << "Error opening source file." << std::endl;
	return 1;
}

// Get the file size
inputFile.seekg(0, std::ios::end);
std::streampos fileSize = inputFile.tellg();
inputFile.seekg(0);

// Read file content and copy to shared memory
inputFile.read(static_cast<char*>(memptr), fileSize);

/*********************************************************/

if(sem_post(semptr)<0) report_and_exit("sem_post");

sleep(12);

munmap(memptr,ByteSize);
close(fd);
sem_close(semptr);
shm_unlink(BackingFile);
return 0;
}
