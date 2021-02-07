#include "ezport.h"

#ifdef has_buggy_inet_ntoa

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char *jr_inet_ntoa (sin_addr)
	struct in_addr sin_addr;
{
	static char inet_str [32];


	sprintf (inet_str, "%d.%d.%d.%d",
		sin_addr.s_net,
		sin_addr.s_host,
		sin_addr.s_lh,
		sin_addr.s_impno
	) ;

	return (inet_str);
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}

#endif
