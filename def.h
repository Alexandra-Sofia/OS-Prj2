/*def.h*/
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/siginfo.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#define READ 0
#define WRITE 1
#define SIZE32 32
#define SIZE64 64
#define SIZE128 128
#define SIZE256 256

typedef enum { false, true } bool;

typedef struct w workers;

struct w{
	int fd;
	char pipename[8];
	int pid;
	bool free;
	workers *next; 
};


typedef struct ur urls;

struct ur{
	char domain[SIZE64];
	int f;
	urls *next;
};

void handler(int);
