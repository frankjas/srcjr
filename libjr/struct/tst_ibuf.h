#ifndef __tst_ibuf_h_
#define __tst_ibuf_h_

#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/inputbuf.h"


typedef struct {
	jr_int numeric;

	char printer_name [128];

	unsigned jr_int diag		: 1;
	unsigned jr_int other		: 1;
} CommandLineInfo;

extern char *ProgramName;
extern CommandLineInfo ArgvFlags [1];

extern char **ArgInterp			PROTO ((char **argv));
static char **DashFlagInterp	PROTO ((char *curarg, char **argv)) ;
extern void UsageError			PROTO ((char *error));

#endif
