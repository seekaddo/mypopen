
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "mypopen.h"


/**\brief
 * \param command
 * \param type
 * */

FILE *mypopen(const char *command, const char *type) {

    FILE *fp = NULL;
    int fd[2];
    int read_fd = 0;
    int write_fd = 0, pid;

    if (type[1] != 0) {
        errno = E2BIG;
        return NULL;
    }

    if (type[0] != 'w' || type[1]) {
        errno = EINVAL;
        return NULL;
    }


    if (pipe(fd) == -1) {
        return NULL;  //errno will be set
    }

    //split the pipe for read and write
    read_fd = fd[0];
    write_fd = fd[1];

    if ((pid = fork()) == -1) {
        close(read_fd);
        close(write_fd);
        return NULL;
    }

    if (pid == 0) {
        // we got a child here

        if (type[0] == 'r') {
            (void) close(read_fd);
            if (write_fd != STDOUT_FILENO) {

                if (dup2(write_fd, STDOUT_FILENO) == -1) {
                    (void) close(write_fd);
                    exit(EXIT_FAILURE);
                }
                (void)close(write_fd);
            }

        } else{
            (void) close(write_fd);
            if (read_fd != STDIN_FILENO) {

                if (dup2(read_fd, STDIN_FILENO) == -1) {
                    (void) close(read_fd);
                    exit(EXIT_FAILURE);
                }
                (void)close(read_fd);
            }
        }


        //(void) close(fd[1]);


        (void) execl("/usr/bin/sh", "sh", "-c", command, (char *) NULL);
        exit(EXIT_FAILURE);

    } else {

        if (type[0] == 'r') {
            (void) close(write_fd);
            fp = fdopen(read_fd, type);
        } else {
            close(read_fd);
            fp = fdopen(write_fd, type);
        }


    }

    if(fp == NULL){
        (void)close(read_fd);
        (void)close(write_fd);
        return NULL;
        if(pid == 0) exit(EXIT_FAILURE);
    }


    return fp;
}


/**\brief
 * \param stream
 * */
int mypclose(FILE *stream) {
    return pclose(stream);
}