#include "ezport.h"

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "jr/syscalls.h"
#include "jr/error.h"

jr_int jr_SetTerminalMode (fd, mode_constant, turn_on_mode, error_buf)
	jr_int					fd;
	jr_int					mode_constant ;
	jr_int					turn_on_mode ;
	char *					error_buf;
{
	struct termios			tty_info [1];

	jr_int					status;

	status	= tcgetattr (fd, tty_info);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get input attributes: %s", strerror (errno));
		status			= -1;
		goto return_status;
	}

	switch (mode_constant) {

		case jr_CBREAK_MODE		: {
			if (turn_on_mode) {
				tty_info->c_lflag	&=  ~ICANON;
			}
			else {
				tty_info->c_lflag	|=  ICANON;	/* enable line buffering, ctl char processing */
			}
			break;
		}

		default					: {
			jr_esprintf (error_buf, "unrecognized mode constant %d", mode_constant);
			status			= -1;
			goto return_status;
		}
	}

	status	= tcsetattr (fd, TCSANOW, tty_info);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't set input attributes: %s", strerror (errno));
		status			= -1;
		goto return_status;
	}

	status = 0;

	return_status : {
	}

	return status;
}


#ifdef use_old_BSD_tty_functions

#include <sgtty.h>
#include <signal.h>
#include <sys/ioctl.h>

/*
 * Should rewrite this using POSIX tc functions
 */

void jr_SetEchoMode(b)
	jr_int b ;
{
	struct sgttyb otty ;
	ioctl(0,TIOCGETP,&otty) ;
	if (b) otty.sg_flags |=  ECHO ;
	else   otty.sg_flags &= ~ECHO ;
	ioctl(0,TIOCSETP,&otty) ;
}

void jr_SetRawNoEchoMode(b)
	jr_int b ;
{
	struct sgttyb otty ;
	ioctl(0,TIOCGETP,&otty) ;
	if (b) {
		otty.sg_flags |=  CBREAK ;
		otty.sg_flags |=  RAW ;
		otty.sg_flags &= ~ECHO ;
	}
	else {
		otty.sg_flags &= ~CBREAK ;
		otty.sg_flags &= ~RAW ;
		otty.sg_flags |=  ECHO ;
	}
	ioctl(0,TIOCSETP,&otty) ;
}

void jr_SetRawMode(b)
	jr_int b ;
{
	struct sgttyb otty ;
	ioctl(0,TIOCGETP,&otty) ;
	if (b) {
		otty.sg_flags |=  CBREAK ;
		otty.sg_flags |=  RAW ;
	}
	else {
		otty.sg_flags &= ~CBREAK ;
		otty.sg_flags &= ~RAW ;
	}
	ioctl(0,TIOCSETP,&otty) ;
}

void jr_SetCbreakMode(b)
	jr_int b ;
{
	struct sgttyb otty ;
	ioctl(0,TIOCGETP,&otty) ;
	if (b) otty.sg_flags |=  CBREAK ;
	else   otty.sg_flags &= ~CBREAK ;
	ioctl(0,TIOCSETP,&otty) ;
}

void jr_SetEraseAndKillDefaults()
{
	struct sgttyb otty ;

	ioctl(0,TIOCGETP,&otty) ;
	otty.sg_erase = '\010' ; /*  erase character == ^H */
	otty.sg_kill  = '\030' ; /* cancel character == ^X */
	ioctl(0,TIOCSETP,&otty) ;
}

#endif
