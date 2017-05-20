/**
 * @file mypopen.c
 *
 * Beispiel 2
 *@brief This is a simple GNU-like popen and fclose Library
 *
 * @author Dennis Addo ic16b026
 * @author Robert Niedermaier ic16b089
 * @date 24/03/2017
 *
 * @details More information about the project can be found here
 * @see https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel2.html
 *
 *
 * Projekt Beispiel 2
 * @version 1.0
 *
 *
 */


/*\include
 * -------------------------------------------------------------- includes --
 */
#include <zconf.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mypopen.h"






/*!
* @def  tpsr
*       for setting STDIN/STDOUT base on passed type
*/
#define tpsr(a, b) ( (*type == 'r') ? (b) : (a))


/**\brief
 * static global variable for giving mypclose access to file stream  from fdopen @c fp_stream
 * and the child's process id @c child_pid.
 *
 * bash sh path @c shell_path
 * */
static FILE *fp_stream = NULL;
static pid_t child_pid = (pid_t) -1;
// ### FB BP: Sowas besser als 
//static const char shell_path[] = "/bin/sh";
//            definieren.
static const char *const shell_path = "/bin/sh";


/**\brief the shell sh is invoked, passing it the command string as arguement. The output of th is
 *        is returned or redirected as a FILE stream based on the the passed parameter option to mypopen
 *        when r type is specified for the stream then the standard output of the
 *        process should be piped/connected to the write part {the input part} of our pipe[1]
 *        and if w-->write is specified for the stream then the standard input of
 *        the process should be connected/piped to the read part { the output part} of our pipe[0]
 *
 *
 *        A call to mypopen must specify either “r” or “w”; no other option is supported
 *        in a standard implementa- tion of mypopen.
 *
 *
 *
 * \param command a given command to executive
 * \param type a specified output stream r-> read, w -> write mode/type
 *
 * \return @c FILE*  on success or  @c NULL --> on fail and errno is set
 * 
 * PNB:
 *  pid cast was used allover for comparing pid and signed int.....because pid is normally on
 *  linux system a typdef int but on other systems BSD, Mac os darwin (uses darwin_pid_t)
 *  so pid cast was considered to prevent any incompatible type errors in the future
 * */

FILE *mypopen(const char *command, const char *type) {

    int fileDis[2], parent_fd, child_fd;


    if (child_pid != (pid_t) -1) {
        errno = EAGAIN;
        return NULL;
    }

    if (type == NULL || type[1] != '\0') {
        errno = EINVAL;
        return NULL;
    }


    if (type[0] != 'w' && type[0] != 'r') {
        errno = EINVAL;
        return NULL;
    }


    if (pipe(fileDis) == -1) {
        return NULL;  /*errno will be set*/
    }


#if 1

    child_fd = tpsr(STDIN_FILENO, STDOUT_FILENO);
    parent_fd = tpsr(STDOUT_FILENO, STDIN_FILENO);

    /*
     *       Expansion here
     *             |
     *             |
     *             v
     * */

#else

    if type[0] == 'r' :

        child_fd = STDOUT_FILENO; /*1 */
        parent_fd = STDIN_FILENO; /*0 */
   
     else :

        child_fd = STDIN_FILENO; /* 0i */
        parent_fd = STDOUT_FILENO;/*1  */


#endif


    if ((child_pid = fork()) == (pid_t) -1) {
        const int err = errno;
        close(fileDis[0]);
        close(fileDis[1]);
        errno = err;
        return NULL;
    }

    if (child_pid == 0) {

        (void) close(fileDis[parent_fd]);

        if (fileDis[child_fd] != child_fd) {

            if (dup2(fileDis[child_fd], child_fd) == -1) {
                (void) close(fileDis[child_fd]);
                _Exit(EXIT_FAILURE);

                
            }
#if 0
            if (child_fd == STDOUT_FILENO) {   /*not necessary*/
                if (dup2(fileDis[child_fd], STDERR_FILENO) == -1){
                    (void) close(fileDis[child_fd]);
                     _exit(EXIT_FAILURE);
                }

            }
#endif
            (void) close(fileDis[child_fd]);
        }


        (void) execl(shell_path, "sh", "-c", command, (char *) NULL);
        _Exit(127);


    } else {

        (void) close(fileDis[child_fd]);

        if ((fp_stream = fdopen(fileDis[parent_fd], type)) == NULL) {
            child_pid = -1;
            (void) close(fileDis[parent_fd]);
            return NULL;
        }


    }

    return fp_stream;
}


/**\brief when processes started with mypopen have finished, mypclose is used to close
 *        the FILE stream associated with it.The mypclose call will return only when
 *        the process started with mypopen finishes.If it’s still running when mypclose
 *        is called, the mypclose call will wait for the process to finish. It is assumed
 *        that multiple calls to mypopen follows mypclose.
 *
 *
 * \param stream the passed FILE stream to be closed
 * \return exit status of command on success or -1 on fail and errno is set
 *
 *
 * PNB:
 * pid cast was used allover for comparing pid.....because pid is normally on
 * linux system a typdef int but on other systems BSD, Mac os darwin (uses darwin_pid_t)
 * so pid cast was considered to prevent any incompatible type errors in the future
 * */
int mypclose(FILE *stream) {
    int status;
    pid_t wpid;

    if (child_pid == (pid_t) -1) {
        errno = ECHILD;
        return -1;
    }


    if (fp_stream != stream) {/*|| fileno(stream) != fileno(fp_stream) ){*/
        errno = EINVAL;
        return -1;
    }


    if (fclose(stream) == EOF) {/*errno is set by fclose*/
        fp_stream = NULL;
        child_pid = -1;
        return -1;
    }


    do {
        wpid = waitpid(child_pid, &status, 0);

    } while ((wpid == (pid_t) -1) && errno == EINTR);


    if (wpid == (pid_t)-1){
        fp_stream = NULL;
        child_pid = -1;
        errno = ECHILD;
        return -1;
    }

    if (WIFEXITED(status)) {
        child_pid = -1;
        fp_stream = NULL;
        return WEXITSTATUS(status);
    }

    child_pid = -1;
    fp_stream = NULL;
    errno = ECHILD;

    return -1;
}
