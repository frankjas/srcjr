#include "ezport.h"

#include "jr/syscalls.h"

void main()
{
	char result[1024] ;

	jr_FileNameToRelativePath(result, sizeof(result),
		"$TEST/natsci/software/purchases/test.html",
		"~ftp"
	) ;
	fprintf(stdout, "%s", result) ;
}
