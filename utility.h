#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<ctype.h>
#include<math.h>

#define STORAGE 500
#define W_SIZE 1000
#define MAX_ENTRY 500 //!!!!
#define MAX 5242880
#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0
#define URL 1
#define MAIN 2
#define UNTAGE 3
#define SITE 4
#define URLCODE 5
#define TITLE 6
#define SIZE 7
#define KEY 8
#define BDMD 9
#define LANG 10
#define IP 11


struct data_handler {
    char start[3]; //@\n
    char url[STORAGE];
    char maintex[STORAGE];
    char untag[STORAGE];
    char sitecode[STORAGE];
    char urlcode[STORAGE];
    char title[W_SIZE]; 
    char size[STORAGE];
    char keyword[W_SIZE]; 
    char bodymd[STORAGE];
    char lang[STORAGE];
    char ip[STORAGE];
};

struct file_handler {
    size_t currentRead;
    int dataCount;
};

struct queue {
    int front;
    int rear;
    struct data_handler **array;
    unsigned int capacity;
    int size;
};



int read_file(char*, int,char**);
int para_valid(int, char**);
int convert(FILE *, int, struct file_handler*, struct data_handler**, size_t, int, int, int, int);
struct data_handler *doubling(struct data_handler*, size_t);
int tmpAssign(struct data_handler*, FILE*, int, int);
size_t tassign(struct data_handler*, FILE*, int);
void assignEmpty(struct data_handler*, int);
void assign(struct data_handler*, struct data_handler* );
void write_file(int, struct data_handler*);
void myqsort(struct data_handler*, int, int, int, int, int, int, int);
int Scmp( const void *a, const void *b);
int Urlcmp( const void *a, const void *b);
int Maincmp(const void *a, const void *b);
int Tagcmp(const void *a, const void *b);
int Sitecmp(const void *a, const void *b);
int Ucodecmp(const void *a, const void *b);
int Titlecmp(const void *a, const void *b);
int Keycmp(const void *a, const void *b);
int BDMDcmp(const void *a, const void *b);
int Langcmp(const void *a, const void *b);
int IPcmp(const void *a, const void *b);
void mergeFile(int, int, int, int, int);
struct queue* create_queue(int);
int isFull(struct queue*);
int isEmpty(struct queue*) ;
void enqueue(struct data_handler, struct queue*);
struct data_handler* dequeue(struct queue*);
int winner_tree_init( struct data_handler*, struct queue**, int, int, int, int, int);
int adjust(struct data_handler*, struct data_handler*, int, int, int, int, int, int);
void maxAssign(struct data_handler*);
void minAssign(struct data_handler*);
int empty_queues(struct queue**, int);
