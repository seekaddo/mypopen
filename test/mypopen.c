#include "mypopen.h"


/**\brief
 * \param command
 * \param type
 * */

#if 1
#define tpsr(a,b) (*type == 'r' ? (b) : (a))
#define tpsw(a,b) (*type == 'w' ? (b) : (a))
#endif
/** Robert use this to help you in your implementation
 * */
static FILE *fp_stream = NULL;
static pid_t popen_pid = -1;
static const char * const shell_path = "/bin/sh";


FILE *mypopen(const char *command, const char *type) {

    int pipe_list[2];
    //int read_fd = 0;
    //int write_fd = 0;
    int parent_fd, child_fd;


    if (type == NULL || type[1] != 0) {
        errno = EINVAL;
        return NULL;
    }

    /* if (type[1] != 0) {
         errno = E2BIG;
         return NULL;
     }*/

    if (type[0] != 'w' && type[0] != 'r') {
        errno = EINVAL;
        return NULL;
    }


    if (pipe(pipe_list) == -1) {
        return NULL;  //errno will be set
    }
#if 1
    child_fd = tpsr(STDIN_FILENO,STDOUT_FILENO);
    parent_fd = tpsw(STDIN_FILENO,STDOUT_FILENO);
#else   
 if (type[0] == 'r') {
        child_fd = STDOUT_FILENO; //1 child woll be doing the writing
        parent_fd = STDIN_FILENO; //0 parent read
    } else {
        child_fd = STDIN_FILENO; //0 child doing the reading
        parent_fd = STDOUT_FILENO;//1 parent do the writing
    }

#endif
    //split the pipe for read and write
    //read_fd = pipe_list[0];
    //write_fd = pipe_list[1];

    if ((popen_pid = fork()) == -1) {
        close(pipe_list[0]);
        close(pipe_list[1]);
        return NULL;
    }

    if (popen_pid == 0) {
        // we got a child here
       	(void)close(pipe_list[parent_fd]);

        if (pipe_list[child_fd] != child_fd) {

            if (dup2(pipe_list[child_fd], child_fd) == -1) {
                (void) close(pipe_list[child_fd]);
                _exit(EXIT_FAILURE);
            }
#if 0
            if (child_fd == STDOUT_FILENO) {
                if (dup2(filedes[child_fd], STDERR_FILENO) == -1){
                    (void) close(filedes[child_fd]);
                    _exit(EXIT_FAILURE);
                }

            }
#endif
            (void) close(pipe_list[child_fd]);
        }
      

        /* if (type[0] == 'r') {
             (void) close(pipe_list[0]);
             if (pipe_list[1] != STDOUT_FILENO) {

                 if (dup2(pipe_list[1], STDOUT_FILENO) == -1) {
                     (void) close(pipe_list[1]);
                     exit(EXIT_FAILURE);
                 }
             }

         } else {
             (void) close(pipe_list[1]);
             if (pipe_list[0] != STDIN_FILENO) {

                 if (dup2(pipe_list[0], STDIN_FILENO) == -1) {
                     (void) close(pipe_list[0]);
                     exit(EXIT_FAILURE);
                 }
             }
         }*/

        //(void) close(pipe_list[1]);


        (void) execl(shell_path, "sh", "-c", command, (char *) NULL);
        _exit(EXIT_FAILURE); //exit(127) required by man page

    } else {

        (void) close(pipe_list[child_fd]);
        if ((fp_stream = fdopen(pipe_list[parent_fd], type)) == NULL) {
            (void) close(pipe_list[parent_fd]);
            return NULL;
        }


    }

    return fp_stream;
}


/**\brief
 * \param stream
 *
 * Robert is working on it
 * */
int mypclose(FILE *stream) {
    return pclose(stream);
}
