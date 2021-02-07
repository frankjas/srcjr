#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include "jr/io.h"
#include "jr/alist.h"
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/inetlib.h"
#include "jr/mail.h"
#include "jr/misc.h"
#include "jr/nettype.h"

#include "mailproj.h"

#define MAX_RESPONSE_SIZE	8192


#ifndef HFIXEDSZ
#	define HFIXEDSZ			12		/* sizeof (HEADER) */
#endif

#ifndef INT16SZ
#	define INT16SZ			2		/* from sendmail code */
#endif

#ifndef INT32SZ
#	define INT32SZ			4		/* from sendmail code */
#endif


jr_int jr_MailGetMailExchanger (host_name, smtp_server_name, smtp_server_name_length, error_buf)
	const char *			host_name;
	char *					smtp_server_name;
	jr_int					smtp_server_name_length;
	char *					error_buf;
{
	jr_MX_EntryListType		mx_entry_list[1];
	jr_MX_EntryType *		mx_entry_ptr;

	jr_int					status;


	jr_MX_EntryListInit (mx_entry_list);

	status	= jr_MX_EntryListLoadHostNameInfo (mx_entry_list, host_name, error_buf);

	if (status != 0) {

		goto return_status;
	}

	jr_MX_EntryListSortByPreference (mx_entry_list);

	mx_entry_ptr	= jr_MX_EntryListFirstEntryPtr (mx_entry_list);


	strncpy (smtp_server_name, jr_MX_EntryName (mx_entry_ptr), smtp_server_name_length);

	smtp_server_name [smtp_server_name_length - 1] = 0;

	status			= 0;


	return_status : {
		jr_MX_EntryListUndo (mx_entry_list);
	}

	return status;
}


void jr_MX_EntryListInit (mx_entry_list)
	jr_MX_EntryListType *	mx_entry_list;
{
	jr_AListInit (mx_entry_list->list_field, sizeof (jr_MX_EntryType));
}

void jr_MX_EntryListUndo (mx_entry_list)
	jr_MX_EntryListType *	mx_entry_list;
{
	jr_MX_EntryType *		mx_entry_ptr;

	jr_AListForEachElementPtr (mx_entry_list->list_field, mx_entry_ptr) {
		jr_MX_EntryUndo (mx_entry_ptr);
	}
	jr_AListUndo (mx_entry_list->list_field);
}

void jr_MX_EntryListAddEntry (mx_entry_list, mx_name, mx_preference)
	jr_MX_EntryListType *	mx_entry_list;
	const char *			mx_name;
	jr_int					mx_preference;
{
	jr_MX_EntryType *		mx_entry_ptr;

	mx_entry_ptr	= jr_AListNewTailPtr (mx_entry_list->list_field);

	jr_MX_EntryInit (mx_entry_ptr, mx_name, mx_preference);
}

void jr_MX_EntryListSetInetAddress (mx_entry_list, mx_name, inet_addr_number)
	jr_MX_EntryListType *	mx_entry_list;
	const char *			mx_name;
	jr_int					inet_addr_number;
{
	jr_MX_EntryType *		mx_entry_ptr;

	jr_MX_EntryListForEachEntryPtr (mx_entry_list, mx_entry_ptr) {
		if (strcasecmp (mx_name, jr_MX_EntryName (mx_entry_ptr))  ==  0) {
			jr_MX_EntrySetInetAddrNumber (mx_entry_ptr, inet_addr_number);
		}
	}
}


jr_int jr_MX_EntryListLoadHostNameInfo (mx_entry_list, host_name, error_buf)
	jr_MX_EntryListType *	mx_entry_list;
	const char *			host_name;
	char *					error_buf;
{
	unsigned char *			response_buffer				= 0;
	unsigned jr_int			max_response_length			= MAX_RESPONSE_SIZE;
	unsigned jr_int			response_length;

	char *					tmp_name_ptr				= 0;

	HEADER *				header_ptr;

	unsigned char *			curr_ptr;
	unsigned char *			end_ptr;

	jr_int					answer_type;
	jr_int					answer_length;
	jr_int					num_question_entries;
	jr_int					num_answer_entries;
	jr_int					mx_preference;
	jr_int					j;

	jr_int					status;


	if (max_response_length  <  sizeof (HEADER)) {
		max_response_length	= sizeof (HEADER);
	}

	response_buffer			= jr_malloc (max_response_length);
	tmp_name_ptr			= jr_malloc (max_response_length);


	status		= res_query (host_name,  C_IN, T_MX, response_buffer, max_response_length);

	if (status < 0) {
		jr_esprintf (error_buf, "couldn't get MX info for '%.32s", host_name);
		goto return_status;
	}

	response_length	= status;


	if (response_length  >  max_response_length) {
		jr_esprintf (error_buf, "res_query() response length %d > max. of %d passed in!",
			response_length, max_response_length
		);

		status	= jr_INET_SYSTEM_ERROR;

		goto return_status;
	}


	header_ptr				= (void *) response_buffer;

	num_question_entries	= jr_ntohs (header_ptr->qdcount);
	num_answer_entries		= jr_ntohs (header_ptr->ancount);

	curr_ptr				= response_buffer + HFIXEDSZ;
	end_ptr					= response_buffer + response_length;


	for (j=0; j < num_question_entries; j++) {

		status = dn_skipname(curr_ptr, end_ptr);

		if (status < 0) {
			status = jr_INET_HOST_NOT_FOUND;
			jr_esprintf (error_buf, "no MX entries found");
			goto return_status;
		}
		curr_ptr	+= status + QFIXEDSZ;
	}


	for (j=0;  j < num_answer_entries;  j++) {

		if (curr_ptr >= end_ptr) {
			break;
		}

		status	= dn_expand (response_buffer, end_ptr, curr_ptr, tmp_name_ptr, max_response_length);

		if (status < 0) {
			break;
		}

		curr_ptr	+= status;

		GETSHORT (answer_type, curr_ptr);

 		curr_ptr	+= INT16SZ + INT32SZ;
					/*
					 * Skip over the 'class' value and the 'ttl' value
					 * Class is: C_IN, C_CHAOS, C_HS, C_HESIOD, C_ANY, C_NONE.
					 * (from 'bind' source, lib/resolv/res_debug.c,
					 * def. for 'struct res_sym __p_class_syms[]' array.
					 */

		GETSHORT (answer_length, curr_ptr);

		switch (answer_type) {
			/*
			 * Look in source for 'bind', function Print_rr() in bin/nslookup/debug.c
			 * for code to interpret each type of nameserver record.
			 */

			case T_A				: {
				jr_int				inet_addr_number;

				GETLONG (inet_addr_number, curr_ptr);

				jr_MX_EntryListSetInetAddress (mx_entry_list, tmp_name_ptr, inet_addr_number);
				/*
				 * tmp_name_ptr contains the name of the mx entry with this address
				 */

				curr_ptr += answer_length;
				break;
			}
			case T_MX				: {

				GETSHORT (mx_preference, curr_ptr);

				status	= dn_expand (
							response_buffer, end_ptr, curr_ptr, tmp_name_ptr, max_response_length
						);

				if (status < 0) {
					goto return_list;
				}

				curr_ptr	+= status;

				jr_MX_EntryListAddEntry (mx_entry_list, tmp_name_ptr, mx_preference);

				break;
			}

			default					: {
				curr_ptr += answer_length;
				break;
			}
		}
	}

	return_list	: 

	if (jr_MX_EntryListIsEmpty (mx_entry_list)) {
		status = jr_INET_HOST_NOT_FOUND;
		jr_esprintf (error_buf, "no MX entries found");
		goto return_status;
	}


	status = 0;

	return_status : {


		if (response_buffer) {
			jr_free (response_buffer);
		}
		if (tmp_name_ptr) {
			jr_free (tmp_name_ptr);
		}
	}
	return status;
}


jr_int jr_MX_EntryListSetInetAddrNumbers (mx_entry_list, error_buf)
	jr_MX_EntryListType *	mx_entry_list;
	char *					error_buf;
{
	jr_MX_EntryType *		mx_entry_ptr;
	struct sockaddr_in  	sockaddr_info[1] ;
	jr_int					status ;



	jr_MX_EntryListForEachEntryPtr (mx_entry_list, mx_entry_ptr) {

		if (jr_MX_EntryInetAddrNumber (mx_entry_ptr)  ==  0) {

			status = jr_InetAddressInit (
						sockaddr_info, jr_MX_EntryName (mx_entry_ptr), SMTP_PORT, error_buf
					);

			if (status != 0) {
				jr_esprintf (error_buf, "couldn't get inet address for '%.32s': %s",
					jr_MX_EntryName (mx_entry_ptr), error_buf
				);
				return status;
			}

			jr_MX_EntrySetInetAddrNumber (
				mx_entry_ptr, sockaddr_info->sin_addr.s_addr
			);
		}
	}
	return 0;
}

void jr_MX_EntryInit (mx_entry_ptr, mx_name, mx_preference)
	jr_MX_EntryType *		mx_entry_ptr;
	const char *			mx_name;
	jr_int					mx_preference;
{
	memset (mx_entry_ptr, 0, sizeof (*mx_entry_ptr));

	mx_entry_ptr->mx_name		= jr_strdup (mx_name);
	mx_entry_ptr->mx_preference	= mx_preference;
}

void jr_MX_EntryUndo (mx_entry_ptr)
	jr_MX_EntryType *		mx_entry_ptr;
{
	jr_free (mx_entry_ptr->mx_name);
}

jr_int jr_MX_EntryPreferenceCmp (void_ptr_1, void_ptr_2)
	const void *			void_ptr_1;
	const void *			void_ptr_2;
{
	const jr_MX_EntryType *	mx_entry_ptr_1				= void_ptr_1;
	const jr_MX_EntryType *	mx_entry_ptr_2				= void_ptr_2;

	
	if (mx_entry_ptr_1->mx_preference  <  mx_entry_ptr_2->mx_preference) {
		return -1;
	}
	if (mx_entry_ptr_1->mx_preference  >  mx_entry_ptr_2->mx_preference) {
		return 1;
	}

	return 0;
}


#ifdef __sendmail_original_code

	/*
	 * This is what the above mail exchanger code was based on.
	 * Stolen from sendmail's getmxrr()
	 */


#include <sysexits.h>
#include <netinet/in.h>
/*
 * for EX_OK, etc. constants
 */

#include <netdb.h>


#define RES_UNC_T			char *
#define SIZE_T				size_t
#define FALSE				0
#define TRUE				(! FALSE)


#define MAXPACKET			8192		/* max packet size used internally by BIND */

#define MAXMXHOSTS			100
#define MXHOSTBUFSIZE		(128 * MAXMXHOSTS)

#define MAXSHORTSTR			203

#define tTd(flag, level)	0			/* JR - for diagnostic levels */

#define sm_gethostbyname(n)	gethostbyname (n)


#define UseNameServer		1
#define HasWildcardMX		0	/* don't use MX records when canonifying */
#define ConfigLevel			8	/* JR - the config file level, refers to sendmail version? */

static char *				FallBackMX;
static char *				MyHostName;
static jr_int				TryNullMXList	= 0;

static char					MXHostBuf[MXHOSTBUFSIZE];

#define syserr				printf

typedef union
{
	HEADER	qb1;				/* JR - declared in arpa/nameser.h */
	u_char	qb2[MAXPACKET];
} querybuf;

typedef jr_int bool;

extern int h_errno;


extern int res_query(), res_search();
extern int mxrand __P((char *));

jr_int getmxrr(host, mxhosts, droplocalhost, rcode)
	char *					host;
	char **					mxhosts;
	jr_int					droplocalhost;
	jr_int *				rcode;
{
	unsigned char *			eom;
	unsigned char *			cp;
	jr_int					i;
	jr_int					j;
	jr_int					n;
	jr_int					nmx = 0;
	char *					bp;
	HEADER *				hp;
	querybuf				answer;
	jr_int					ancount;
	jr_int					qdcount;
	jr_int					buflen;
	jr_int					seenlocal = FALSE;
	unsigned jr_short		pref;
	unsigned jr_short		type;
	unsigned jr_short		localpref = 256;
	char *					fallbackMX = FallBackMX;
	jr_int					trycanon = FALSE;
	jr_int					(*resfunc)();
	unsigned jr_short		prefer[MAXMXHOSTS];
	jr_int					weight[MAXMXHOSTS];

	if (tTd(8, 2)) {
		printf("getmxrr(%s, droplocalhost=%d)\n", host, droplocalhost);
	}

	if (	fallbackMX != NULL
		&&	droplocalhost
		&&	wordinclass(fallbackMX, 'w')) {

		/* don't use fallback for this pass */
		fallbackMX = NULL;
	}

	*rcode = EX_OK;

	/* efficiency hack -- numeric or non-MX lookups */
	if (host[0] == '[') {
		goto punt;
	}

	/*
	**  If we don't have MX records in our host switch, don't
	**  try for MX records.  Note that this really isn't "right",
	**  since we might be set up to try NIS first and then DNS;
	**  if the host is found in NIS we really shouldn't be doing
	**  MX lookups.  However, that should be a degenerate case.
	*/

	if (!UseNameServer) {
		goto punt;
	}

	if (HasWildcardMX && ConfigLevel >= 6) {
		resfunc = res_query;
	}
	else {
		resfunc = res_search;
	}

	errno = 0;

	n = (*resfunc)(host, C_IN, T_MX, (u_char *) &answer, sizeof(answer));

	if (n < 0) {
		if (tTd(8, 1)) {
			printf("getmxrr: res_search(%s) failed (errno=%d, h_errno=%d)\n",
			    (host == NULL) ? "<NULL>" : host, errno, h_errno
			);
		}

		switch (h_errno) {
			case NO_DATA:
				trycanon = TRUE;
				/* fall through */

			case NO_RECOVERY:
				/* no MX data on this host */
				goto punt;

			case HOST_NOT_FOUND:

#			if BROKEN_RES_SEARCH
			case 0:
				/* Ultrix resolver retns failure w/ h_errno=0 */
#			endif
				/* host doesn't exist in DNS; might be in /etc/hosts */
				trycanon = TRUE;
				*rcode = EX_NOHOST;
				goto punt;

			case TRY_AGAIN:
			case -1:
				/* couldn't connect to the name server */
				if (fallbackMX != NULL) {
					/* name server is hosed -- push to fallback */
					mxhosts[nmx++] = fallbackMX;
					return nmx;
				}
				/* it might come up later; better queue it up */
				*rcode = EX_TEMPFAIL;
				break;

			default:
				syserr("getmxrr: res_search (%s) failed with impossible h_errno (%d)\n",
					host, h_errno
				);
				*rcode = EX_OSERR;
				break;
		}

		/* irreconcilable differences */
		return (-1);
	}

	/* avoid problems after truncation in tcp packets */
	if (n > sizeof(answer)) {
		n = sizeof(answer);
	}

#	ifdef _garbage__
	unsigned	qdcount :16;	/* number of question entries */
	unsigned	ancount :16;	/* number of answer entries */
	unsigned	nscount :16;	/* number of authority entries */
	unsigned	arcount :16;	/* number of resource entries */
#	endif

	/* find first satisfactory answer */
	hp		= (HEADER *)&answer;
	cp		= (u_char *)&answer + HFIXEDSZ;
	eom		= (u_char *)&answer + n;

	qdcount	= jr_ntohs(hp->qdcount);
	ancount	= jr_ntohs(hp->ancount);

	for (j=0; j < qdcount; j++; cp += n + QFIXEDSZ) {
		if ((n = dn_skipname(cp, eom)) < 0) {
			goto punt;
		}
	}

	buflen	= sizeof(MXHostBuf) - 1;
	bp		= MXHostBuf;


	for (j=0;  j < ancount;  j++) {

		if (cp >= eom  ||  nmx >= MAXMXHOSTS - 1) {
			break;
		}

		if ((n = dn_expand((u_char *)&answer, eom, cp, (RES_UNC_T) bp, buflen)) < 0) {
			break;
		}

		cp += n;

		GETSHORT(type, cp);
 		cp += INT16SZ + INT32SZ;
		GETSHORT(n, cp);

		if (type != T_MX) {
			if (tTd(8, 8) || _res.options & RES_DEBUG) {
				printf("unexpected answer type %d, size %d\n", type, n);
			}
			cp += n;
			continue;
		}

		GETSHORT(pref, cp);

		if ((n = dn_expand((u_char *)&answer, eom, cp, (RES_UNC_T) bp, buflen)) < 0) {
			break;
		}

		cp += n;

		if (wordinclass(bp, 'w')) {
			if (tTd(8, 3)) {
				printf("found localhost (%s) in MX list, pref=%d\n", bp, pref);
			}
			if (droplocalhost) {
				if (!seenlocal || pref < localpref) {
					localpref = pref;
				}
				seenlocal = TRUE;
				continue;
			}
			weight[nmx] = 0;
		}
		else {
			weight[nmx] = mxrand(bp);
		}

		prefer[nmx]		= pref;
		mxhosts[nmx++]	= bp;

		n = strlen(bp);
		bp += n;

		if (bp[-1] != '.') {
			*bp++ = '.';
			n++;
		}
		*bp++ = '\0';
		buflen -= n + 1;
	}

	/* sort the records */
	for (i = 0; i < nmx; i++) {
		for (j = i + 1; j < nmx; j++) {
			if (	prefer[i] > prefer[j]
				||	(prefer[i] == prefer[j] && weight[i] > weight[j])) {

				jr_int			temp;
				char *			temp1;

				temp		= prefer[i];
				prefer[i]	= prefer[j];
				prefer[j]	= temp;
				temp1		= mxhosts[i];
				mxhosts[i]	= mxhosts[j];
				mxhosts[j]	= temp1;
				temp		= weight[i];
				weight[i]	= weight[j];
				weight[j]	= temp;
			}
		}

		if (seenlocal && prefer[i] >= localpref) {
			/* truncate higher preference part of list */
			nmx = i;
		}
	}

	/* delete duplicates from list (yes, some bozos have duplicates) */
	for (i = 0; i < nmx - 1; ) {
		if (strcasecmp(mxhosts[i], mxhosts[i + 1]) != 0) {
			i++;
		}
		else {
			/* compress out duplicate */
			for (j = i + 1; j < nmx; j++) {
				mxhosts[j] = mxhosts[j + 1];
			}
			nmx--;
		}
	}

	if (nmx == 0) punt: {
		if (	seenlocal
			&&	(!TryNullMXList || sm_gethostbyname(host) == NULL)) {

			/*
			**  If we have deleted all MX entries, this is
			**  an error -- we should NEVER send to a host that
			**  has an MX, and this should have been caught
			**  earlier in the config file.
			**
			**  Some sites prefer to go ahead and try the
			**  A record anyway; that case is handled by
			**  setting TryNullMXList.  I believe this is a
			**  bad idea, but it's up to you....
			*/

			*rcode = EX_CONFIG;
			syserr("MX list for %s points back to %s", host, MyHostName);
			return -1;
		}

		if (strlen(host) >= (SIZE_T) sizeof MXHostBuf) {
			*rcode = EX_CONFIG;
			syserr("Host name %s too long", shortenstring(host, MAXSHORTSTR));
			return -1;
		}

		snprintf(MXHostBuf, sizeof MXHostBuf, "%s", host);

		mxhosts[0] = MXHostBuf;

		if (host[0] == '[') {
			register char *p;

			/* this may be an MX suppression-style address */
			p = strchr(MXHostBuf, ']');
			if (p != NULL) {
				*p = '\0';
				if (inet_addr(&MXHostBuf[1]) != INADDR_NONE) {
					nmx++;
					*p = ']';
				}
				else {
					trycanon = TRUE;
					mxhosts[0]++;
				}
			}
		}
		if (trycanon && getcanonname(mxhosts[0], sizeof MXHostBuf - 2, FALSE)) {
			bp = &MXHostBuf[strlen(MXHostBuf)];
			if (bp[-1] != '.') {
				*bp++ = '.';
				*bp = '\0';
			}
			nmx = 1;
		}
	}

	/* if we have a default lowest preference, include that */
	if (fallbackMX != NULL && !seenlocal) {
		mxhosts[nmx++] = fallbackMX;
	}

	return (nmx);
}

#endif
