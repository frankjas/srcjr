#include "ezport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "jr/syscalls.h"

jr_int jr_UnixModeToPerms (
	mode_t				unix_mode)
{
	jr_int				perm_flags			= 0;

	if (unix_mode & S_ISUID) {
		perm_flags		|= jr_PERM_S_OWNER;
	}
	if (unix_mode & S_ISGID) {
		perm_flags		|= jr_PERM_S_GROUP;
	}

	if (unix_mode & S_IRUSR) {
		perm_flags		|= jr_PERM_R_OWNER;
	}
	if (unix_mode & S_IWUSR) {
		perm_flags		|= jr_PERM_W_OWNER;
	}
	if (unix_mode & S_IXUSR) {
		perm_flags		|= jr_PERM_X_OWNER;
	}

	if (unix_mode & S_IRGRP) {
		perm_flags		|= jr_PERM_R_GROUP;
	}
	if (unix_mode & S_IWGRP) {
		perm_flags		|= jr_PERM_W_GROUP;
	}
	if (unix_mode & S_IXGRP) {
		perm_flags		|= jr_PERM_X_GROUP;
	}

	if (unix_mode & S_IROTH) {
		perm_flags		|= jr_PERM_R_OTHER;
	}
	if (unix_mode & S_IWOTH) {
		perm_flags		|= jr_PERM_W_OTHER;
	}
	if (unix_mode & S_IXOTH) {
		perm_flags		|= jr_PERM_X_OTHER;
	}

	return perm_flags;
}

mode_t jr_PermsToUnixMode (
	unsigned jr_int		perm_flags)
{
	mode_t				unix_mode			= 0;

	if (perm_flags & jr_PERM_S_OWNER) {
		unix_mode		|= S_ISUID;
	}
	if (perm_flags & jr_PERM_S_GROUP) {
		unix_mode		|= S_ISGID;
	}

	if (perm_flags & jr_PERM_R_OWNER) {
		unix_mode		|= S_IRUSR;
	}
	if (perm_flags & jr_PERM_W_OWNER) {
		unix_mode		|= S_IWUSR;
	}
	if (perm_flags & jr_PERM_X_OWNER) {
		unix_mode		|= S_IXUSR;
	}

	if (perm_flags & jr_PERM_R_GROUP) {
		unix_mode		|= S_IRGRP;
	}
	if (perm_flags & jr_PERM_W_GROUP) {
		unix_mode		|= S_IWGRP;
	}
	if (perm_flags & jr_PERM_X_GROUP) {
		unix_mode		|= S_IXGRP;
	}

	if (perm_flags & jr_PERM_R_OTHER) {
		unix_mode		|= S_IROTH;
	}
	if (perm_flags & jr_PERM_W_OTHER) {
		unix_mode		|= S_IWOTH;
	}
	if (perm_flags & jr_PERM_X_OTHER) {
		unix_mode		|= S_IXOTH;
	}

	return unix_mode;
}

unsigned jr_int jr_PermsFromString (
	const char *		perms_string)
{
	unsigned jr_int		perm_flags			= 0;
	jr_int				q;

	for (q=0;  perms_string[q];  q++) {

		switch (q) {
			case 0			: {
				if (perms_string[q] == 'r') {
					perm_flags	|= jr_PERM_R_OWNER;
				}
				break;
			}
			case 1			: {
				if (perms_string[q] == 'w') {
					perm_flags	|= jr_PERM_W_OWNER;
				}
				break;
			}
			case 2			: {
				if (perms_string[q] == 'x') {
					perm_flags	|= jr_PERM_X_OWNER;
				}
				if (perms_string[q] == 's') {
					perm_flags	|= jr_PERM_X_OWNER | jr_PERM_S_OWNER;
				}
				break;
			}
			case 3			: {
				if (perms_string[q] == 'r') {
					perm_flags	|= jr_PERM_R_GROUP;
				}
				break;
			}
			case 4			: {
				if (perms_string[q] == 'w') {
					perm_flags	|= jr_PERM_W_GROUP;
				}
				break;
			}
			case 5			: {
				if (perms_string[q] == 'x') {
					perm_flags	|= jr_PERM_X_GROUP;
				}
				if (perms_string[q] == 's') {
					perm_flags	|= jr_PERM_X_GROUP | jr_PERM_S_GROUP;
				}
				break;
			}
			case 6			: {
				if (perms_string[q] == 'r') {
					perm_flags	|= jr_PERM_R_OTHER;
				}
				break;
			}
			case 7			: {
				if (perms_string[q] == 'w') {
					perm_flags	|= jr_PERM_W_OTHER;
				}
				break;
			}
			case 8			: {
				if (perms_string[q] == 'x') {
					perm_flags	|= jr_PERM_X_OTHER;
				}
				break;
			}
		}
	}

	return perm_flags;
}


jr_int jr_PermsAddFromString (
	jr_int				curr_perms,
	const char *		perms_string)
{
	jr_int				q;
	jr_int				do_add				= 0;
	jr_int				do_subtract			= 0;
	jr_int				for_user			= 0;
	jr_int				for_group			= 0;
	jr_int				for_other			= 0;


	for (q=0;  perms_string[q];  q++) {

		switch (perms_string[q]) {
			case '+'		: {
				do_add		= 1;
				break;
			}
			case '-'		: {
				do_subtract	= 1;
				break;
			}
			case '='		: {
				do_add		= 1;
				curr_perms	= 0;
				break;
			}
			case 'a'		: {
				for_user	= 1;
				for_group	= 1;
				for_other	= 1;
				break;
			}
			case 'u'		: {
				for_user	= 1;
				break;
			}
			case 'g'		: {
				for_group	= 1;
				break;
			}
			case 'o'		: {
				for_other	= 1;
				break;
			}
			case 'r'		: {
				if (for_user) {
					if (do_add) {
						curr_perms	|= jr_PERM_R_OWNER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_R_OWNER;
					}
				}
				if (for_group) {
					if (do_add) {
						curr_perms	|= jr_PERM_R_GROUP;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_R_GROUP;
					}
				}
				if (for_other) {
					if (do_add) {
						curr_perms	|= jr_PERM_R_OTHER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_R_OTHER;
					}
				}
				break;
			}
			case 'w'		: {
				if (for_user) {
					if (do_add) {
						curr_perms	|= jr_PERM_W_OWNER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_W_OWNER;
					}
				}
				if (for_group) {
					if (do_add) {
						curr_perms	|= jr_PERM_W_GROUP;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_W_GROUP;
					}
				}
				if (for_other) {
					if (do_add) {
						curr_perms	|= jr_PERM_W_OTHER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_W_OTHER;
					}
				}
				break;
			}
			case 'x'		: {
				if (for_user) {
					if (do_add) {
						curr_perms	|= jr_PERM_X_OWNER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_X_OWNER;
					}
				}
				if (for_group) {
					if (do_add) {
						curr_perms	|= jr_PERM_X_GROUP;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_X_GROUP;
					}
				}
				if (for_other) {
					if (do_add) {
						curr_perms	|= jr_PERM_X_OTHER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_X_OTHER;
					}
				}
				break;
			}
			case 's'		: {
				if (for_user) {
					if (do_add) {
						curr_perms	|= jr_PERM_X_OWNER | jr_PERM_S_OWNER;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_S_OWNER;
					}
				}
				if (for_group) {
					if (do_add) {
						curr_perms	|= jr_PERM_X_GROUP | jr_PERM_S_GROUP;
					}
					if (do_subtract) {
						curr_perms	&= ~jr_PERM_S_GROUP;
					}
				}
				break;
			}
		}
	}

	return curr_perms;
}


void jr_PermsToString (
	char *				perm_string,
	unsigned jr_int		perm_flags)
{
	perm_string[0]		= 0;

	if (perm_flags & jr_PERM_R_OWNER) {
		strcat (perm_string, "r");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_W_OWNER) {
		strcat (perm_string, "w");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_S_OWNER) {
		strcat (perm_string, "s");
	}
	else if (perm_flags & jr_PERM_X_OWNER) {
		strcat (perm_string, "x");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_R_GROUP) {
		strcat (perm_string, "r");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_W_GROUP) {
		strcat (perm_string, "w");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_S_GROUP) {
		strcat (perm_string, "s");
	}
	else if (perm_flags & jr_PERM_X_GROUP) {
		strcat (perm_string, "x");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_R_OTHER) {
		strcat (perm_string, "r");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_W_OTHER) {
		strcat (perm_string, "w");
	}
	else {
		strcat (perm_string, "-");
	}

	if (perm_flags & jr_PERM_X_OTHER) {
		strcat (perm_string, "x");
	}
	else {
		strcat (perm_string, "-");
	}
}

#ifndef ostype_winnt

unsigned jr_int jr_umask(
	unsigned jr_int				perm_flags)
{
	mode_t						mode_flags	= jr_PermsToUnixMode( perm_flags);
	mode_t						old_mode;

	old_mode	= umask( mode_flags);

	return jr_UnixModeToPerms( old_mode);
}
#endif

