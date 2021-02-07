#include "ezport.h"

#include <sys/time.h>
#include <sys/resource.h>

main ()
{
	jr_int i, t;
	struct rusage rusage_before[1];
	struct rusage rusage_after[1];

	getrusage (RUSAGE_SELF, rusage_before);
	for (i=0; i < 1000000; i ++) {
		t = 60 * 60 * 24 + i;
		myfunc (t);
	}
	getrusage (RUSAGE_SELF, rusage_after);

	printf ("Time for '60 * 60 * 24 + i': %d seconds, %d micro-seconds\n",
		rusage_after->ru_utime.tv_sec - rusage_before->ru_utime.tv_sec,
		rusage_after->ru_utime.tv_usec - rusage_before->ru_utime.tv_usec
	);

	getrusage (RUSAGE_SELF, rusage_before);
	for (i=0; i < 1000000; i ++) {
		t = 86400 + i;
		myfunc (t);
	}
	getrusage (RUSAGE_SELF, rusage_after);

	printf ("Time for '86400 + i': %d seconds, %d micro-seconds\n",
		rusage_after->ru_utime.tv_sec - rusage_before->ru_utime.tv_sec,
		rusage_after->ru_utime.tv_usec - rusage_before->ru_utime.tv_usec
	);
}

myfunc (t)
{
	return (t);
}
