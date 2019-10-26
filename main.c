#include"utility.h"

int main(int argc,char **argv) {

    if(argc<2) {
        fprintf(stderr,"rsort filename\n");
        exit(EXIT_FAILURE);
    }

    else if( argc == 2 ) {

        if(strcmp(argv[1],"-h") == 0) {
            fprintf(stderr,"USAGE:rsort -rb/-s [rb_option] [-i/-r] filename\n\
            paramemter usage\n\
            -rb = record begin patter\n\
            -s = sizeorder\n\
            -i = incensitive\n\
            -r = reverse order\n\
            *****notice******\n\
            1. -s cannot pair with -i\n\
            2. -rb cannot pair with -s\n");
            return 0;
        }
        int size = read_file(argv[1],argc,argv);
        if(size<=0) {
            fprintf(stderr,"ERROR: unable to process %s\n",argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    else if( argc == 3) {

        if(strcmp(argv[1],"-s") == 0) {
           int size = read_file(argv[2],argc,argv);
           if(size<0) {
               fprintf(stderr,"ERROR: unable tp porcess %s\n", argv[2]);
               exit(EXIT_FAILURE);
           }
        }
        else if(strcmp(argv[1],"-i") == 0) {
            fprintf(stderr,"ERROR: Give only incensitve parameter will result the same file\n");
            exit(EXIT_FAILURE);
        }
        else {
            fprintf(stderr,"ERROR: Invalid format\n");
            exit(EXIT_FAILURE);
        }

    }

    else if( argc == 4) {
        if(strcmp(argv[1],"-rb") == 0) {
            int check = para_valid(argc,argv);
            if(check == FALSE) {
                fprintf(stderr,"ERROR: Not exist record heading %s\n",argv[2]);
                exit(EXIT_FAILURE);
            }
            int size  = read_file(argv[3],argc,argv);
            if(size<0) {
                fprintf(stderr,"ERROR: unable to process %s\n",argv[3]);
                exit(EXIT_FAILURE);
            }
        }

        else if(strcmp(argv[1],"-s") == 0) {

            if(strcmp(argv[2],"-i") == 0) {
                fprintf(stderr,"ERROR: Unable to sort size incenstive\n");
                exit(EXIT_FAILURE);
            }
            int size = read_file(argv[3],argc,argv);
            if(size<0) {
                fprintf(stderr,"ERROR: unable to process %s\n",argv[3]);
                exit(EXIT_FAILURE);
            }
        }

        else if(strcmp(argv[1],"-i") == 0) {

            if(strcmp(argv[2],"-s") == 0) {
                fprintf(stderr,"ERROR: Unable to sort size incenstive\n");
                exit(EXIT_FAILURE);
            }
            int size = read_file(argv[3],argc,argv);
            if(size<0) {
                fprintf(stderr,"ERROR: unable to process %s\n",argv[3]);
                exit(EXIT_FAILURE);
            }
            
        }

        else {

            fprintf(stderr,"ERROR: Unrecognized symbol %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    else if( argc == 5) {

        int warning = OFF;
        // check wehter rb and s coexist
        for(int iter = 0; iter< argc; iter++) {
            if(strcmp(argv[iter],"-rb")==0 || strcmp(argv[iter],"-s") == 0) {
                warning = ON;
            }
            else if(warning == ON && ( strcmp(argv[iter],"-rb") ==0 || strcmp(argv[iter],"-s") == 0 )) {
                fprintf(stderr,"ERROR: Invalid format\n");
                exit(EXIT_FAILURE);
            }
        }
        int size = read_file(argv[4],argc,argv); 
    }

    else {
        fprintf(stderr,"Please rsort --help for more information\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}