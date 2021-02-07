#include "ezport.h"

#include <string.h>
#include "jr/string.h"

char *jr_TTY_LettersFromDeviceName(dev_name)
	char *	dev_name ;
{
	char *tty_string ;
	char *tty_letters ;

	if (tty_string = strstr(dev_name, "ttydf")) {
		tty_letters = tty_string + 5 ;
	}
	else if (tty_string = strstr(dev_name, "ttyd")) {
		tty_letters = tty_string + 4 ;
	}
	else if (tty_string = strstr(dev_name, "tty")) {
		tty_letters = tty_string + 3 ;
	}
	else if (tty_string = strstr(dev_name, "cuf")) {
		tty_letters = tty_string + 3 ;
	}
	else if (tty_string = strstr(dev_name, "cu")) {
		tty_letters = tty_string + 2 ;
	}
	else {
		tty_letters = dev_name ;
	}
	return(tty_letters) ;
}

