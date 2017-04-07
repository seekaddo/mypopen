
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
    //int read_fd = 0;
    //int write_fd = 0;
    int pid;

    if (type == NULL || type[1] != 0)
        return NULL;

   /* if (type[1] != 0) {
        errno = E2BIG;
        return NULL;
    }*/

    if (type[0] != 'w' && type[0] != 'r') {
        errno = EINVAL;
        return NULL;
    }


    if (pipe(fd) == -1) {
        return NULL;  //errno will be set
    }

    //split the pipe for read and write
    //read_fd = fd[0];
    //write_fd = fd[1];

    if ((pid = fork()) == -1) {
        close(fd[0]);
        close(fd[1]);
        return NULL;
    }

    if (pid == 0) {
        // we got a child here

        if (type[0] == 'r') {
            (void) close(fd[0]);
            if (fd[1] != STDOUT_FILENO) {

                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    (void) close(fd[1]);
                    exit(EXIT_FAILURE);
                }
            }

        } else {
            (void) close(fd[1]);
            if (fd[0] != STDIN_FILENO) {

                if (dup2(fd[0], STDIN_FILENO) == -1) {
                    (void) close(fd[0]);
                    exit(EXIT_FAILURE);
                }
            }
        }


        //(void) close(fd[1]);


        (void) execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        exit(EXIT_FAILURE);

    } else {

        if (type[0] == 'r') {
            fp = fdopen(fd[0], type);
            (void) close(fd[1]);
        } else {
            fp = fdopen(fd[1], type);
            (void) close(fd[0]);
        }


    }

    if (fp == NULL) {
        (void) close(fd[0]);
        (void) close(fd[1]);
        return NULL;
    }


    return fp;
}


/**\brief
 * \param stream
 *
 * Robert is working on it
 * */
int mypclose(FILE *stream) {
    return pclose(stream);
}