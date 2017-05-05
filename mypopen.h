#ifndef MYPOPEN_MYPOPEN_H
#define MYPOPEN_MYPOPEN_H
/**
 * @file mypopen.h
 *
 * Beispiel 2
 *@brief This is a simple GNU-like popen and fclose Library
 *
 * @author Dennis Addo ic16b026
 * @author Robert Niedermaier ic16b089
 * @details More information about the project can be found here
 * @see URL:https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel2.html
 *
 * @date 24/03/2017
 *
 * @version 1.0
 *
 * @todo All implentations must be contained in one method structure unless these functions
 * @     are required by other programs/methods
 * @todo ALL errors must be the GNU C EXIT_SUCCESS and FAILURE macros
 * @todo perror and sterror is used to display error informations, functions and error-types
 *
 *
 */

/*\include
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h>
#include <zconf.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>



/**\fn
 * ------------------------------------------------------------- functions --
 * */
extern FILE *mypopen(const char *command, const char *type);
extern int mypclose(FILE *stream);


#endif //MYPOPEN_MYPOPEN_H
