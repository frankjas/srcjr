#ifndef __mailproj_h___
#define __mailproj_h___

#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#include "jr/io.h"
#include "jr/inetlib.h"
#include "jr/mail.h"
#include "jr/error.h"

#define SMTP_PORT						25

#define	SMTP_INIT_STATUS				220
#define	SMTP_OK_STATUS					250
#define	SMTP_WILL_FORWARD_STATUS		251
#define	SMTP_QUIT_OK_STATUS				221
#define	SMTP_DATA_OK_STATUS				354

#define SMTP_MAX_RESPONSE_LENGTH		2048



#endif
