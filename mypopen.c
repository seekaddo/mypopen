
#include "mypopen.h"


/*\brief
 * \param
 * Starting pointing of the program
 * */

int main(int argc, char *argv[]) {
    argc = argc+0;
    argv[0] = *argv;



    FILE *p = mypopen ("cd .. && grep --color \"addo\" ./* ","r");
    char buffer[1024];
    while (fgets(buffer, 1024, p)) {
        printf (" => %s", buffer);
    }
    fclose(p);



    return 0;

}

FILE *mypopen(const char *command, const char *type){

    int fd[2];
    int read_fd, write_fd, pid;

    pipe(fd);

    //split the pipe for read and write
    read_fd = fd[0];
    write_fd = fd[1];

    pid = fork();

    if(pid == 0){
        // we got a child here
        printf("Child was here\n");

        close(read_fd);

        //now we set the child to write and remove the original write

        dup2(write_fd,1);

        close(write_fd);

        execl("/usr/bin/sh","sh","-c",command,NULL);

        return NULL;

    } else{

        close(write_fd);
        if(type[0] == 'r'){
            printf("We are reading ......\n");
            return fdopen(read_fd,"r");
        } else{
            printf("We are writing ......\n");
            return fdopen(read_fd,"w");
        }



    }


}