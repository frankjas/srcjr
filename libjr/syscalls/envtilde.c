#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include "jr/vector.h"
#include "jr/malloc.h"
#include "jr/string.h"

char *  jr_NewStringFromDollarAndTildeExpansion(word_arg)
	const char *  word_arg ;
{
	char			directory_name[2048] ;
	const char * 	temp_word_value = 0 ;

	if (*word_arg == '$') {
		const char *	environment_variable_name = word_arg + 1 ;

		if ((strcmp(environment_variable_name, "CWD") == 0) ||
			(strcmp(environment_variable_name, "PWD") == 0)) {

			getcwd(directory_name, sizeof(directory_name)) ;
			directory_name[sizeof(directory_name)-1] = 0 ;

			temp_word_value = directory_name ;
		}
		else {
			temp_word_value = getenv(environment_variable_name) ;
		}
	}
	else if (*word_arg == '~') {
		const char *		username = word_arg + 1 ;
		struct passwd *		password_info ;
		
		if (*username == 0) {
			password_info = getpwuid(getuid()) ;
		}
		else {
			password_info = getpwnam(username) ;
		}

		if (password_info != 0) {
			temp_word_value = password_info->pw_dir ;	/* home directory of username or self */
		}
	}

	/* allocate result if necessary and return */ {
		char *	new_word_result ;
		
		if (temp_word_value != 0) {
			new_word_result = jr_strdup(temp_word_value) ;
		}
		else {
			errno = 0 ;
			new_word_result = 0 ;
		}

		return(new_word_result) ;
	}
}
			
jr_int jr_PathStringCopyWithDollarAndTildeExpansion(target_str, source_str)
	char *  		target_str ;
	const char *	source_str ;
{
	if (strcmp(source_str, "/") == 0) {
		strcpy(target_str, source_str) ;
		return(0) ;
	}
	else {
		char *  fullpathv[128] ;
		char *  cur_component ;
		jr_int    maxdirs = sizeof(fullpathv) / sizeof(char *) ;
		jr_int    i, n ;
		jr_int    last_char ;
		jr_int    first_char ;

		{
			char *  end_ptr = strchr(source_str, 0) ;

			last_char  = end_ptr[-1] ;
			first_char = source_str[0] ;
		}

		n = jr_VectorizeIntoWords(source_str, "/", fullpathv, maxdirs) ;
		if (n == maxdirs) {
			for (i=0; i < n; i++) {
				jr_free(fullpathv[i]) ;
			}
			errno = 0 ;
			return(-1) ;
		}

		for (i=0 ; i < n; i++) {
			cur_component = fullpathv[i] ;

			if ((*cur_component == '$') ||
				(*cur_component == '~')) {
				char *  new_component ;
				
				new_component = jr_NewStringFromDollarAndTildeExpansion(fullpathv[i]) ;
				if (new_component == 0) {
					jr_VectorUndo(fullpathv) ;
					return(-1) ;
				}

				jr_free(cur_component) ;
				fullpathv[i] = new_component ;
			}
		}

		if (first_char == '/') {
			strcpy(target_str, "/") ;
		}
		else {
			*target_str = 0 ;
		}

		for (i=0; i < n; i++) {
			strcat(target_str, fullpathv[i]) ;
			jr_free(fullpathv[i]) ;
			if (fullpathv[i+1]) {
				strcat(target_str, "/") ;
			}
		}

		if (last_char == '/') {
			char *  end_ptr = strchr(target_str,0) ;

			if (end_ptr[-1] != '/') {
				strcat(target_str, "/") ;
			}
		}
	}
	return(0) ;
}
