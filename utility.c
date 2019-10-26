#include"utility.h"
// 1. assign strcpy data->bd
int para_valid(int gc,char **com) {
    int index = -1;

    if(gc == 2) return URL;

    for(int iter = 0; iter< gc; iter++) {
        if(strcmp(com[iter],"-rb") == 0 ){
            index = iter+1;
        }
    }
    if(index == -1) {
        fprintf(stderr,"ERROE: unrecognized error at para_valid\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(com[index],"@url") == 0) return URL;
    else if(strcmp(com[index],"@MainTextMD5") == 0) return MAIN;
    else if(strcmp(com[index],"@UntagMD5") == 0) return UNTAGE;
    else if(strcmp(com[index],"@SiteCode") == 0) return SITE;
    else if(strcmp(com[index],"@UrlCode") == 0) return URLCODE;
    else if(strcmp(com[index],"@title") == 0) return TITLE;
    else if(strcmp(com[index],"@Size") == 0) return SIZE;
    else if(strcmp(com[index],"@keyword") == 0) return KEY;
    else if(strcmp(com[index],"@BodyMD5") == 0) return BDMD;
    else if(strcmp(com[index],"@Lang") == 0) return LANG;
    else if(strcmp(com[index],"@IP") == 0) return IP;
    else return FALSE;
}

int para_check(char check[],int argc,char **argv) {
    for(int iter = 0; iter<argc; iter++) {
        if(strcmp(argv[iter],check) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void assignEmpty(struct data_handler *tmp, int index) {
    if(index == 0) strcpy(tmp->start,"@\n\0");
    if(index == 1) strcpy(tmp->url,"\0");
    else if(index == 2) strcpy(tmp->maintex, "\0");
    else if(index == 3) strcpy(tmp->untag, "\0");
    else if(index == 4) strcpy(tmp->sitecode, "\0");
    else if(index == 5) strcpy(tmp->urlcode, "\0");
    else if(index == 6) strcpy(tmp->title, "\0");
    else if(index == 7) strcpy(tmp->size, "\0");
    else if(index == 8) strcpy(tmp->keyword, "\0");
    else if(index == 9) strcpy(tmp->bodymd, "\0");
    else if(index == 10) strcpy(tmp->lang, "\0");
    else if(index == 11) strcpy(tmp->ip, "\0");

}

void assign(struct data_handler *data,struct data_handler *tmp) {

    strcpy(data->start,tmp->start);
    strcpy(data->url,tmp->url);
    strcpy(data->maintex,tmp->maintex);
    strcpy(data->untag,tmp->untag);
    strcpy(data->sitecode,tmp->sitecode);
    strcpy(data->urlcode,tmp->urlcode);
    strcpy(data->title,tmp->title);
    strcpy(data->size,tmp->size);
    strcpy(data->keyword,tmp->keyword);
    strcpy(data->bodymd,tmp->bodymd);
    strcpy(data->lang,tmp->lang);
    strcpy(data->ip,tmp->ip);
}

void write_file(int file, struct data_handler *tmp) {
        write(file,tmp->start,strlen(tmp->start));
        write(file, tmp->url,strlen(tmp->url));
        write(file,tmp->maintex,strlen(tmp->maintex));  
        write(file,tmp->untag,strlen(tmp->untag));       
        write(file,tmp->sitecode,strlen(tmp->sitecode));
        write(file,tmp->urlcode,strlen(tmp->urlcode));       
        write(file,tmp->title,strlen(tmp->title));
        write(file,tmp->size,strlen(tmp->size));    
        write(file,tmp->keyword,strlen(tmp->keyword));
        write(file,tmp->bodymd,strlen(tmp->bodymd));
        write(file,tmp->lang,strlen(tmp->lang));
        write(file,tmp->ip,strlen(tmp->ip));
}


int read_file(char* filename,int argc, char **argv) {
    /* rb processs file base on whether command have rb
    if have rb convet process only take rb and element benath rb
    else convert intake all element */
    int r_com = FALSE, i_com = FALSE, s_com = FALSE, rb_com = FALSE;
    FILE *fp = fopen(filename,"r+");
    int read_now = 0;
    struct file_handler *ptr = (struct file_handler*)malloc(sizeof(struct file_handler)*1);
    struct data_handler *head = (struct data_handler*)malloc(sizeof(struct data_handler)*MAX_ENTRY);
    size_t maxEntry = MAX_ENTRY;
    int startPattern = FALSE;
    int op = open("debug.rec",O_WRONLY|O_APPEND);
    int merge = FALSE;

    if(fp == NULL) {
        return -1; // because already print in main
    }
    //check command
    r_com = para_check("-r",argc,argv);
    i_com = para_check("-i",argc,argv);
    s_com = para_check("-s",argc,argv);
    rb_com = para_check("-rb",argc,argv);
    // calculate file size
    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp);

    fseek(fp ,0, SEEK_SET);
    int readFile = 4; // ctrl winner tree's node
    size_t expected_read = 0;
    ptr->currentRead = 0;
    ptr -> dataCount = -1;
    // deal rb pattern
    if(s_com == TRUE) startPattern = URL;
    else startPattern = para_valid(argc,argv);
    if( startPattern == SIZE) {
        fprintf(stderr,"ERROR: size pattern can be sort using -s no need to -rb @Size\n");
    }
    else if(startPattern == FALSE) {
        fprintf(stderr,"ERROR: Invalid Format\n");
        exit(EXIT_FAILURE);
    }
    // write out sorted tmp file
    while(readFile != 0) {
        expected_read = fileSize/readFile;
        maxEntry =  convert(fp, expected_read, ptr, &head, maxEntry, s_com, rb_com,startPattern, merge);
        read_now += ptr->currentRead; // debug
        fileSize = fileSize - ptr->currentRead;
        myqsort(head,ptr->dataCount,readFile, startPattern, s_com, rb_com, r_com, i_com); // rb; i; s; r
        ptr->dataCount = -1;
        ptr->currentRead = 0;
        readFile -= 1;
    }

    //merge file
    mergeFile(maxEntry, s_com, rb_com, r_com, startPattern);
    free(head);
    free(ptr);
    head = NULL;
    ptr = NULL;
    return 1;

}

int convert(FILE *fp, int expected_read, struct file_handler *ptr, struct data_handler **head,  size_t maxEntry, int s, int rb,int startPattern, int merge) {

    struct data_handler tmp[1];
    size_t readRecord = 0;
    struct data_handler *data = *head;

    while((ptr->currentRead)<expected_read) {
        if(ptr->dataCount == maxEntry-1) {
            *head = doubling(*head,maxEntry);
            maxEntry = maxEntry << 1;
            data = *head+(ptr->dataCount+1); //jump to empty
        }
        readRecord = tmpAssign(tmp,fp,startPattern,merge);
        if(readRecord == -1 ) return maxEntry;
        ptr->currentRead += readRecord;
        assign(data,tmp); //free tmp's body
        data = data+1;
        ptr->dataCount++;
    }

    return maxEntry;
}

int tmpAssign(struct data_handler *tmp, FILE *fp, int startPattern,int merge) {
    int recordSize = 0;
    size_t len = 0;
    char *line = NULL;
    int iter = 0;

    if(merge == TRUE) {
        getline(&line,&len,fp);
        for(iter = 0; iter< startPattern; iter++)
            assignEmpty(tmp,iter);
        free(line);
        line = NULL;
    }


    for(; iter<12; iter++) {
        if( iter < startPattern)  {
            getline(&line,&len,fp);
            recordSize += strlen(line);
            assignEmpty(tmp,iter);
            free(line);
            line = NULL;
            continue;
        }
        recordSize += tassign(tmp,fp,iter);
        if(recordSize == -1) return -1;
    }
    return recordSize;
}

size_t tassign( struct data_handler *tmp, FILE *fp, int index) {
    size_t len = 0;
    char *line = NULL;
    size_t lineSize = 0;
    int check = 0;

    check = getline(&line,&len,fp);
    if(check == -1) return -1;
    lineSize += strlen(line);

    if(index == 1) strcpy(tmp->url,line);
    else if(index == 2) strcpy(tmp->maintex, line);
    else if(index == 3) strcpy(tmp->untag, line);
    else if(index == 4) strcpy(tmp->sitecode, line);
    else if(index == 5) strcpy(tmp->urlcode, line);
    else if(index == 6) strcpy(tmp->title, line);
    else if(index == 7) strcpy(tmp->size, line);
    else if(index == 8) strcpy(tmp->keyword, line);
    else if(index == 9) strcpy(tmp->bodymd, line);
    else if(index == 10) strcpy(tmp->lang, line);
    else if(index == 11) strcpy(tmp->ip, line);

    free(line);
    line = NULL;
    return lineSize;
}

struct data_handler *doubling(struct data_handler *oldArray, size_t maxEntry) {
    int newEntry = maxEntry<<1;
    int iter = 0;
    struct data_handler *newArray = (struct data_handler*)malloc(sizeof(struct data_handler)*newEntry);
    for(iter = 0; iter<maxEntry; iter++) {
        assign(newArray+iter,oldArray+iter);
    }
    free(oldArray);
    oldArray = NULL;
    return newArray;
}

void myqsort(struct data_handler *ptr, int dataCount, int fileNum, int startPattern, int s_com, int rb_com, int r_com, int i_com) {
    char outFileName[10] = {0};
    sprintf(outFileName,"tmp%d.rec",fileNum);
    int outFile = open(outFileName,O_RDWR|O_CREAT|O_EXCL|O_APPEND,S_IRWXU);

    if(outFile == -1) {
        fprintf(stderr,"ERROR: Unable to write file\n");
        exit(EXIT_FAILURE);
    }

    if(s_com == TRUE)
        qsort(ptr,dataCount+1,sizeof(struct data_handler), Scmp);

    else if(rb_com == TRUE) {
        if(startPattern == URL)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Urlcmp);

        else if(startPattern == MAIN)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Maincmp);

        else if(startPattern == UNTAGE)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Tagcmp);

        else if(startPattern == SITE)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Sitecmp);

        else if(startPattern == URLCODE)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Ucodecmp);
        
        else if(startPattern == TITLE)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Titlecmp);

        else if(startPattern == KEY)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Keycmp);

        else if(startPattern == BDMD)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), BDMDcmp);

        else if(startPattern == LANG)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), Langcmp);
        
        else if(startPattern == IP)
            qsort(ptr,dataCount+1,sizeof(struct data_handler), IPcmp);

    }

    if(r_com == FALSE) {
        for(int iter = 0; iter < dataCount+1; iter++) {
            write_file(outFile,ptr+iter);
        }
    }
    else {
        for(int iter = dataCount; iter >= 0 ; iter--) {
            write_file(outFile,ptr+iter);
        }
    }
     
}

int Scmp( const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->size;
    char *string2 = ((struct data_handler *)b)->size;
    int num1 = atoi(string1+6);
    int num2 = atoi(string2+6);
    return num1-num2;
}

int Urlcmp( const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->url;
    char *string2 = ((struct data_handler *)b)->url;
    char *ptr = string1;
    
    return strcmp(string1,string2);
}

int Maincmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->maintex;
    char *string2 = ((struct data_handler *)b)->maintex;
    return strcmp(string1,string2);
}

int Tagcmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->untag;
    char *string2 = ((struct data_handler *)b)->untag;
    return strcmp(string1,string2);
}

int Sitecmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->sitecode;
    char *string2 = ((struct data_handler *)b)->sitecode;
    return strcmp(string1,string2);
}

int Ucodecmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->urlcode;
    char *string2 = ((struct data_handler *)b)->urlcode;
    return strcmp(string1,string2);
}

int Titlecmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->title;
    char *string2 = ((struct data_handler *)b)->title;
    return strcmp(string1,string2);
}

int Keycmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->keyword;
    char *string2 = ((struct data_handler *)b)->keyword;
    return strcmp(string1,string2);
}

int BDMDcmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->bodymd;
    char *string2 = ((struct data_handler *)b)->bodymd;
    return strcmp(string1,string2);
}

int Langcmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->lang;
    char *string2 = ((struct data_handler *)b)->lang;
    return strcmp(string1,string2);
}

int IPcmp(const void *a, const void *b) {
    char *string1 = ((struct data_handler *)a)->ip;
    char *string2 = ((struct data_handler *)b)->ip;
    return strcmp(string1,string2);
}


void mergeFile(int maxEntry, int s_com, int rb_com, int r_com, int startPattern) {
     struct queue **buffer = NULL;
     struct file_handler *ptr = (struct file_handler*)malloc(sizeof(struct file_handler)*1);
     int readFile = 4;
     int adjustedIndex = -1;
     char filename[10] = {0};
     FILE *fp = NULL;
     int mergeFile = 0;
     int expectedRead = -1;
     struct data_handler *head = (struct data_handler*)malloc(sizeof(struct data_handler)*maxEntry);
     struct data_handler *dataPtr = head;
     struct data_handler *root = NULL;
     struct data_handler *value = NULL;

    // queue buffer line
     buffer = (struct queue**)malloc(sizeof(struct queue*)*4);
    // get data from temporary file and enqueue into buffer
     for(; readFile>0; readFile--) {
         sprintf(filename,"tmp%d.rec",readFile);
         fp = fopen(filename,"r+");
         if(fp == NULL) {
             fprintf(stderr,"ERROR: Unable to open temporary file\n");
             exit(EXIT_FAILURE);
         }
         // get file size
         fseek(fp,0,SEEK_END);
         expectedRead = ftell(fp);
         fseek(fp,0,SEEK_SET);
         ptr->currentRead = 0;
         ptr->dataCount = -1;
         convert( fp, expectedRead, ptr, &head, maxEntry, s_com, rb_com, startPattern, TRUE);
         dataPtr = head;
         buffer[readFile-1] = create_queue(ptr->dataCount+1);
         for(int iter = 0; iter<=ptr->dataCount; iter++) {
             enqueue(dataPtr[iter],buffer[readFile-1]);
         }
     }
    readFile = 4;
     // create empty array as an abstraction to tree
    root = (struct data_handler*)malloc(sizeof(struct data_handler)*(2*readFile-1));
    adjustedIndex = winner_tree_init(root, buffer, startPattern, readFile, rb_com, s_com, r_com);
    mergeFile = open("sorted.rec",O_RDWR|O_CREAT|O_EXCL|O_APPEND,S_IRWXU);
    write_file( mergeFile, &root[0]);

    while(!(empty_queues( buffer, readFile))) {
        value = dequeue(buffer[adjustedIndex-readFile+1]);
        if(value == NULL) {
            value = (struct data_handler*)malloc(sizeof(struct data_handler)*1);
            if( r_com == TRUE) minAssign(value);
            else maxAssign(value);
            buffer[adjustedIndex-readFile+1]->size = buffer[adjustedIndex-readFile+1]->size -1;
            adjustedIndex = adjust( root, value, adjustedIndex, readFile, rb_com, s_com, r_com, startPattern);
            if(!(empty_queues( buffer, readFile)) ) write_file( mergeFile, &root[0]);
            free(value);
            value = NULL;
            continue;
        }
        adjustedIndex = adjust( root, value, adjustedIndex, readFile ,rb_com, s_com, r_com, startPattern);
        write_file( mergeFile, &root[0]);
    }

    // free area
     free(buffer);
     free(ptr);
     ptr = NULL;
     buffer = NULL;
     // free buffer[iter]
}

struct queue* create_queue(int d_count) {
    struct queue *myqueue = (struct queue*)malloc(sizeof(struct queue)*1);
    myqueue->front = 0;
    myqueue->rear = d_count-1;
    myqueue->capacity = d_count;
    myqueue->size = 0;
    myqueue->array = (struct data_handler**)malloc(sizeof(struct data_handler*)*(d_count+1)); //add one to avoid memory error
    return myqueue;
}

int isFull(struct queue *myqueue) {
    if(myqueue->size == myqueue->capacity) return TRUE;
    return FALSE;
}

int isEmpty(struct queue *myqueue ) {
    if(myqueue->size == 0) return TRUE;
    return FALSE;
}

void enqueue(struct data_handler value,struct queue *myqueue) {
    if(isFull(myqueue)) {
        fprintf(stderr,"ERROR: queue is full\n");
        return;
    }
    myqueue->rear = (myqueue->rear+1) % myqueue->capacity;
    myqueue->array[myqueue->rear] = (struct data_handler*)malloc(sizeof(struct data_handler)*1);
    struct data_handler *tmp = &value;
    assign(myqueue->array[myqueue->rear],tmp);
    myqueue->size += 1;
}

struct data_handler* dequeue(struct queue *myqueue) {
    if(isEmpty(myqueue)) {
        fprintf(stderr,"ERROR: queue is empty\n");
        return NULL;
    }
    struct data_handler *item =  myqueue->array[myqueue->front];
    myqueue->front = (myqueue->front+1) % myqueue->capacity;
    myqueue->size -= 1;
    return item;
}

int winner_tree_init( struct data_handler *root, struct queue **buffer, int startPattern, int fileNum, int rb_com, int s_com, int r_com) {
    int adjusted_index = -1;
    struct data_handler *ptr = NULL;
    struct data_handler *tmp = NULL;
    struct data_handler *tmp1 = NULL;
    struct data_handler *tmp2 = NULL;
    int *record_winner = (int*)malloc(sizeof(int)*(fileNum-1));
    int parent = fileNum-2;
    int recordIndex = 0;
    int treeHigh = log2(fileNum);

    // push element to leaf
    for(int iter = 0; iter<fileNum; iter++) {
        ptr = &(root[iter+fileNum-1]);
        assign(ptr,dequeue(buffer[iter]));
    }

    // tournament round
    while( parent!=-1 ) {
        // cmp
        tmp1 = &(root[ parent*2+1 ]);
        tmp2 = &(root[ parent*2+2 ]);
        int a;
        if( s_com == TRUE) {

            if( r_com == TRUE) {

                if(Scmp(tmp1,tmp2) > 0 ){
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+1];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+1;
                }
                
                else {
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+2];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+2;
                }
            }

            else {

                if(Scmp(tmp1,tmp2) > 0){
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+2];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+2;
                }
                
                else {
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+1];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+1;
                }
            }
        }

        else if( rb_com == TRUE) {

            if(startPattern == URL) {

                if(r_com== TRUE) {
                    if(Urlcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Urlcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }

            }
            else if(startPattern == MAIN) {

                if(r_com== TRUE) {
                    if(Maincmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Maincmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == UNTAGE) {

                if(r_com== TRUE) {
                    if(Tagcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Tagcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == SITE) {
                if(r_com== TRUE) {
                    if(Sitecmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Sitecmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == URLCODE) {
                if(r_com== TRUE) {
                    if(Ucodecmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Ucodecmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }

            else if(startPattern == TITLE) {
                if(r_com== TRUE) {
                    if(Titlecmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Titlecmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == KEY) {
                if(r_com== TRUE) {
                    if(Keycmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Keycmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == BDMD) {
                if(r_com== TRUE) {
                    if(BDMDcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(BDMDcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == LANG) {
                if(r_com== TRUE) {
                    if(Langcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Langcmp(tmp1,tmp2)> 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == IP) {
                if(r_com== TRUE) {
                    if(IPcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(IPcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
        }
        // cmp end
        recordIndex++;
        parent--;
    }
    // find winner's position
    adjusted_index = record_winner[recordIndex-1]*2+1;

    for( int iter = 0; iter < treeHigh-1; iter++) {

        for( int inner = 0; inner< fileNum-2; inner++) {

            if(adjusted_index == record_winner[inner] || adjusted_index+1 == record_winner[inner]) {
                if( iter == treeHigh -2) {
                    adjusted_index = record_winner[inner];
                    break;
                }
                adjusted_index = record_winner[inner]*2+1;
                break;
            }
        }
    }
    // free area
    free(record_winner);

    return adjusted_index;
}


void maxAssign(struct data_handler *data) {
    strcpy(data->start,"~");
    strcpy(data->url,"~");
    strcpy(data->maintex,"~");
    strcpy(data->untag,"~");
    strcpy(data->sitecode,"~");
    strcpy(data->urlcode,"~");
    strcpy(data->title,"~");
    strcpy(data->size,"@Size:2147000000");
    strcpy(data->keyword,"~");
    strcpy(data->bodymd,"~");
    strcpy(data->lang,"~");
    strcpy(data->ip,"~");
}

void minAssign(struct data_handler *data) {
    strcpy(data->start,"\0");
    strcpy(data->url,"\0");
    strcpy(data->maintex,"\0");
    strcpy(data->untag,"\0");
    strcpy(data->sitecode,"\0");
    strcpy(data->urlcode,"\0");
    strcpy(data->title,"\0");
    strcpy(data->size,"@Size:0");
    strcpy(data->keyword,"\0");
    strcpy(data->bodymd,"\0");
    strcpy(data->lang,"\0");
    strcpy(data->ip,"\0");
}


int empty_queues(struct queue **buffer,int size) {
    int iter = 0;
    int inner_size = 0;
    for( iter = 0;iter<size;iter++) {
        inner_size = buffer[iter]->size;
        if(inner_size == -1) {
            continue;
        }
        else break;
    }

    if(iter == size) return TRUE;
    return FALSE;
}

int adjust( struct data_handler *root, struct data_handler *value, int index, int fileNum, int rb_com, int s_com, int r_com, int startPattern) {

    struct data_handler *tmp1;
    struct data_handler *tmp2;
    struct data_handler *ptr;
    struct data_handler *tmp = &(root[index]);
    int *record_winner = (int*)malloc(sizeof(int)*(fileNum-1));
    int recordIndex = 0;
    int winnerIndex = -1;
    int parent = fileNum-2; // have to see through all tree every time since function have to record eveytime's winner
    int treeHigh = log2(fileNum);
    int adjusted_index = -1;
    // assign dequeue value into nore
    assign(tmp, value);

    // tournament round
    while( parent!=-1 ) {
        // cmp
        tmp1 = &(root[ parent*2+1 ]);
        tmp2 = &(root[ parent*2+2 ]);
        if( s_com == TRUE) {

            if( r_com == TRUE) {

                if(Scmp(tmp1,tmp2) > 0){
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+1];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+1;
                }
                
                else {
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+2];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+2;
                }
            }

            else {

                if(Scmp(tmp1,tmp2) > 0){
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+2];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+2;
                }
                
                else {
                    ptr = &root[parent];
                    tmp = &root[ 2*parent+1];
                    assign( ptr, tmp);
                    record_winner[recordIndex] = 2*parent+1;
                }
            }
        }

        else if( rb_com == TRUE) {

            if(startPattern == URL) {

                if(r_com== TRUE) {
                    if(Urlcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Urlcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }

            }
            else if(startPattern == MAIN) {

                if(r_com== TRUE) {
                    if(Maincmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Maincmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == UNTAGE) {

                if(r_com== TRUE) {
                    if(Tagcmp(tmp1,tmp2)) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Tagcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == SITE) {
                if(r_com== TRUE) {
                    if(Sitecmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Sitecmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == URLCODE) {
                if(r_com== TRUE) {
                    if(Ucodecmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Ucodecmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }

            else if(startPattern == TITLE) {
                if(r_com== TRUE) {
                    if(Titlecmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Titlecmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == KEY) {
                if(r_com== TRUE) {
                    if(Keycmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Keycmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == BDMD) {
                if(r_com== TRUE) {
                    if(BDMDcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(BDMDcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == LANG) {
                if(r_com== TRUE) {
                    if(Langcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(Langcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
            else if(startPattern == IP) {
                if(r_com== TRUE) {
                    if(IPcmp(tmp1,tmp2) > 0) {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }
                }

                else {

                    if(IPcmp(tmp1,tmp2) > 0){
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+2];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+2;
                    }

                    else {
                        ptr = &root[parent];
                        tmp = &root[ 2*parent+1];
                        assign( ptr, tmp);
                        record_winner[recordIndex] = 2*parent+1;
                    }
                }
            }
        }
        // cmp end
        winnerIndex++;
        recordIndex++;
        parent--;
    }

    // find winner's position
    adjusted_index = record_winner[winnerIndex]*2+1;

    for( int iter = 0; iter < treeHigh-1; iter++) {

        for( int inner = 0; inner< fileNum-2; inner++) {

            if(adjusted_index == record_winner[inner] || adjusted_index+1 == record_winner[inner]) {
                if( iter == treeHigh -2) {
                    adjusted_index = record_winner[inner];
                    break;
                }
                adjusted_index = record_winner[inner]*2+1;
                break;
            }
        }
    }
    // free area
    free(record_winner);

    return adjusted_index;
}