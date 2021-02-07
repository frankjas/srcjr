#include "ezport.h"

#if defined (USG) && !defined (Xenix)
#	if !defined (USGr3) && ! defined (USGr4)
#		define USGr3
#	endif /* USGr3 */
#endif /* USG && !Xenix */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <errno.h>
#include <sys/param.h>

#if defined (_POSIX_VERSION) || defined (USGr3) || defined (USGr4) || defined (DIRENT)
#	include <dirent.h>
#	define direct dirent
#	define	D_NAMLEN(d) strlen((d)->d_name)
#else
#	define D_NAMLEN(d) ((d)->d_namlen)
#	if defined (Xenix)
#		include <sys/ndir.h>
#	else
#		if defined (USG)
#			include "ndir.h"
#		else
#			include <sys/dir.h>
#		endif
#	endif
#endif	/* USGr3 || DIRENT.  */

#if defined (_POSIX_SOURCE)
	/* Posix does not require that the d_ino field be present, and some systems do not provide it. */
#	define REAL_DIR_ENTRY(dp) 	1	/* TRUE ALWAYS */
#else
#	define REAL_DIR_ENTRY(dp)  (dp->d_ino != 0)
#endif /* _POSIX_SOURCE */

#include <string.h>

/*
 * If the opendir () on your system lets you open non-directory files,
 * then we consider that not robust.  Define OPENDIR_NOT_ROBUST in the
 * SYSDEP_CFLAGS for your machines entry in machines.h.
 */

#ifndef NULL
#	define NULL 0
#endif

#include <pwd.h>

#include "jr/malloc.h"
#include "jr/string.h"
#include "jr/syscalls.h"

/*
 * Global variable which controls whether or not * matches .*.
 */

/*
 *  Return a vector of names of files in directory DIR
 *  whose names match glob pattern PAT.
 *  The names are not in any particular order.
 *  Wildcards at the beginning of PAT do not match an initial period.
 *  The vector is terminated by an element that is a null pointer.
 *  To jr_free the space allocated, first jr_free the vector's elements,
 *  then jr_free the vector.
 *  Return 0 if cannot get enough jr_malloc_stats to hold the pointer
 *  and the names.
 *  Return -1 if cannot access directory DIR.
 *  Look in errno for more information.
 */
const char **  jr_VectorCreateFromDirectoryNameAndPattern (
	const char *	directory_name_string,
	const char *	pattern_string,
	jr_int			match_slashes,
	jr_int			match_leading_dot)
{
	struct globval
		{
			struct globval *next;
			char *name;
		};

	DIR *d;
	register struct direct *dp;
	struct globval *lastlink;
	register struct globval *nextlink;
	register char *nextname;
	unsigned jr_int count;
	jr_int lose;
	register const char **name_vector;
	register unsigned jr_int i;
	jr_int is_match ;

#	if defined (OPENDIR_NOT_ROBUST)
		/* test dir for being a directory first */ {
			struct stat finfo;

			if (stat (directory_name_string, &finfo) < 0) {
				return ((char **) -1) ;
			}

			if (!S_ISDIR (finfo.st_mode)) {
				return ((char **) -1) ;
			}
		}
#	endif /* OPENDIR_NOT_ROBUST */

	d = opendir (directory_name_string);
	if (d == NULL) {
		/*
		 * opendir failed
		 */
		return ((const char **)-1) ;
	}

	lastlink = 0;
	count = 0;
	lose = 0;

	/* Scan the directory, finding all names that match.
		 For each name that matches, allocate a struct globval
		 on the stack and store the name in it.
		 Chain those structs together; lastlink is the front of the chain.  */
	while (1) {
		dp = readdir (d);
		if (dp == NULL) break;

		/* If this directory entry is not to be used, try again. */
		if (!REAL_DIR_ENTRY (dp)) continue;

		/* If a dot must be explicity matched, check to see if they do. */
		if	(	dp->d_name[0] == '.'
			&&	pattern_string[0] != '.'
			&& !match_leading_dot) {
			continue;
		}

		is_match	=	jr_IsGlobMatch (
							dp->d_name,
							pattern_string,
							match_slashes,
							match_leading_dot
						) ;

		if (is_match) {
			nextlink = (struct globval *) alloca (sizeof (struct globval)); /* stack allocation */
			nextlink->next = lastlink;
			nextname = (char *) jr_malloc (strlen(dp->d_name) + 1);
			if (nextname == NULL) {
				lose = 1;
				break;
			}
			lastlink = nextlink;
			nextlink->name = nextname;
			memcpy (nextname, dp->d_name, strlen(dp->d_name) + 1);
			++count;
		}
	}
	(void) closedir (d);

	/* Have we run out of malloced space */
	if (lose) {
		/* Here jr_free the strings we have got.  */
		while (lastlink) {
			jr_free (lastlink->name);
			lastlink = lastlink->next;
		}
		return 0;
	}
	else {
		name_vector = (const char **) jr_malloc ((count + 1) * sizeof (char *));
		lose |= (name_vector == NULL) ;
	}


	/* Copy the name pointers from the linked list into the vector.  */
	for (i = 0; i < count; ++i) {
		name_vector[i] = lastlink->name;
		lastlink = lastlink->next;
	}

	name_vector[count] = 0;
	return name_vector;
}

/*
 * Return a new array which is the concatenation
 * of each string in ARRAY to DIR.
 */

static const char **  glob_dir_to_array (directory_name_string, array)
		 const char *   directory_name_string ;
		 const char **  array;
{
	register unsigned jr_int i, l;
	jr_int     add_slash;
	const char **  result = 0 ;

	l = strlen (directory_name_string);
	if (l == 0) return array;

	add_slash = directory_name_string[l - 1] != '/';

	i = 0;
	while (array[i] != 0) ++i;

	result = (const char **) jr_malloc ((i + 1) * sizeof (char *));
	if (result == NULL) return 0;

	for (i = 0; array[i] != 0; i++) {
		result[i] = (char *) jr_malloc (l + (add_slash ? 1 : 0) + strlen (array[i]) + 1);
		if (result[i] == NULL) return 0;
		sprintf ((char *) result[i], "%s%s%s", directory_name_string, add_slash ? "/" : "", array[i]);
	}
	result[i] = 0;

	/* Free the input array.  */
	for (i = 0; array[i] != 0; i++) {
		jr_free (array[i]);
	}
	jr_free ((char *) array);

	return (result) ;
}

/*
 *  Do globbing on PATHNAME.  Return an array of pathnames that match,
 *  marking the end of the array with a null-pointer as an element.
 *  If no pathnames match, then the array is empty (first element is null).
 *  If there isn't enough jr_malloc_stats, then return 0.
 *  If a file system error occurs, return 0; `errno' has the error code.
 */

const char **  jr_VectorCreateFromCshLikeNameExpansion(
	const char *	pathname_arg,
	jr_int			match_slashes,
	jr_int			match_leading_dot)
{
	char			pathname[1024] ;
	char *			directory_name ;
	char *			filename;
	unsigned jr_int	directory_len;
	unsigned jr_int	result_size;
	const char **	result = 0 ;

	if (jr_PathStringCopyWithDollarAndTildeExpansion(pathname, pathname_arg) != 0) {
		return(0) ;
	}

	result = (const char **) jr_malloc (sizeof (char *));
	if (result == NULL) return(0);
	result_size = 1;
	result[0] = 0;

	/* Find the filename.  */
	filename = strrchr (pathname, '/');
	if (filename == 0) {
		filename = pathname;
		directory_name = "";
		directory_len = 0;
	}
	else {
		directory_len = (filename - pathname) + 1;
		directory_name = (char *) alloca (directory_len + 1); /* stack allocation */

		memcpy (directory_name, pathname, directory_len);
		directory_name[directory_len] = '\0';
		++filename;
	}

	/*
	 * If directory_name contains globbing characters, then we
	 * have to expand the previous levels.  Just recurse.
	 */

	if (jr_HasGlobPatternChars (directory_name)) {
		const char **directories;
		register unsigned jr_int i;

		if (directory_name[directory_len - 1] == '/') {
			directory_name[directory_len - 1] = '\0';
		}

		directories = jr_VectorCreateFromCshLikeNameExpansion (
						directory_name, match_slashes, match_leading_dot
					);

		if (directories == 0) {
			goto cleanup_error;
		}
		else if (*directories == 0) {
			/*
			** 12/20/07: no entries, return a null vector, the loop below
			** expects some values
			*/
			return(directories) ;
		}

		/*
		 * We have successfully globbed the preceding directory name.
	 	 * For each name in DIRECTORIES, call glob_vector on it and
	 	 * FILENAME.  Concatenate the results together.
		 */
		for (i = 0; directories[i] != 0; ++i) {
			const char **temp_results;

			temp_results = jr_VectorCreateFromDirectoryNameAndPattern (
							directories[i], filename, match_slashes, match_leading_dot
						);

			/* Handle error cases. */
			if (temp_results == 0) {
				goto cleanup_error;
			}
			else if (temp_results == (const char **)-1) {
				/* This filename is probably not a directory.  Ignore it.  */
				;
			}
			else {
				const char **array = glob_dir_to_array (directories[i], temp_results);
				register unsigned jr_int l;

				l = 0;
				while (array[l] != 0) ++l;

				result = (const char **)jr_realloc (result, (result_size + l) * sizeof (char *));
				if (result == NULL) goto cleanup_error;

				for (l = 0; array[l] != 0; ++l) {
					result[result_size++ - 1] = array[l];
				}

				result[result_size - 1] = 0;

				/* Note that the elements of ARRAY are not jr_freed.  */
				jr_free ((char *) array);
			}
		} /* for */

		/* Free the directories.  */
		for (i = 0; directories[i]; i++) {
			jr_free (directories[i]);
		}

		jr_free ((char *) directories);

		return(result) ;
	}

	/* If there is only a directory name, return it. */
	if (*filename == '\0') {
		result = (const char **) jr_realloc ((char *) result, 2 * sizeof (char *));
		if (result == NULL) return 0;

		result[0] = (char *) jr_malloc (directory_len + 1);
		if (result[0] == NULL) goto cleanup_error;

		memcpy ((char *) result[0], directory_name, directory_len + 1);
		result[1] = 0;
		return result;
	}
	else {
		/*
		 * Otherwise, just return what jr_VectorCreateFromCshLikeNameExpansion
		 * returns appended to the directory name.
		 */
		const char **temp_results ;
		
		temp_results	=	jr_VectorCreateFromDirectoryNameAndPattern (
								(directory_len == 0 ? "." : directory_name),
								filename, match_slashes, match_leading_dot
							);

		if (temp_results == 0 || temp_results == (const char **)-1) {
			if (result != 0) {
				jr_free(result) ;
			}
			return(0);
		}

		if (result != 0) {
			jr_free(result) ;
		}

		return (glob_dir_to_array (directory_name, temp_results));
	}

	/*
	 * We get to jr_malloc_stats error if the program has run out of jr_malloc_stats
	 */

	cleanup_error:
	if (result != 0) {
		register unsigned jr_int i;

		for (i = 0; result[i] != 0; ++i) {
			jr_free (result[i]);
		}
		jr_free ((char *) result);
	}
	return (0) ;
}
