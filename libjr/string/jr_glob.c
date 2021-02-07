#include "ezport.h"

#include <string.h>

#include "jr/string.h"

static jr_int jr_glob_match_after_star (
	const char *  	text,
	const char *  	pattern,
	jr_int			match_slashes);

/*
 * Return nonzero if PATTERN has any special jr_IsGlobMatch chars in it.
 */
jr_int jr_HasGlobPatternChars (pattern)
	const char *	pattern;
{
	return jr_GlobPatternOffset( pattern ) >= 0;
}

jr_int jr_GlobPatternOffset (pattern)
	const char *	pattern;
{
	const char *	p ;
	const char *	p_start = 0;		/* 5-16-11: shutup MS compiler warning */
	char			c;
	jr_int			open = 0;

	p = pattern ;
	for (; *p; p++) {
		c = *p ;

		switch (c) {
			case '?':
			case '*': return p - pattern;

			case '[':
				/*
				** Only accept an open brace if there is a close
				** brace to match it.  Bracket expressions must be
				** complete, according to Posix.2
				*/
				if (open == 0) {
					p_start	= p;
				}
				open++;
				continue;
			case ']':
				if (open) {
					open--;
					if (open == 0) {
						return p_start - pattern;
					}
				}
				continue;      

			case '\\':
				/*
				** 7/13/08: Note: if it's a backslash, skip over the next char.
				*/
				if (*p++ == '\0') return -1;
				continue;
		}
	}

	return -1;
}
/*
 * Match the pattern PATTERN against the string TEXT;
 * return 1 if it matches, 0 otherwise.
 *
 *  A match means the entire string TEXT is used up in matching.
 *
 *  In the pattern string, `*' matches any sequence of characters,
 *  `?' matches any character, [SET] matches any character in the specified set,
 *  [!SET] matches any character not in the specified set.
 *
 *  A set is composed of characters or ranges; a range looks like
 *  character hyphen character (as in 0-9 or A-Z).
 *  [0-9a-zA-Z_] is the set of characters allowed in C identifiers.
 *  Any other character in the pattern must be matched exactly.
 *
 *  To suppress the special syntactic significance of any of `[]*?!-\',
 *  and match the character exactly, precede it with a `\'.
 *
 *  If DOT_SPECIAL is nonzero,
 *  `*' and `?' do not match `.' at the beginning of TEXT.
 */

jr_int jr_IsGlobMatch (text, pattern, match_slashes, match_leading_dot)
	const char *	text;
	const char *	pattern ;
	jr_int    		match_slashes;
	jr_int    		match_leading_dot;
{
	register const char *p = pattern ;
	register const char *t = text;
	register char c;

	while ((c = *p++) != '\0') {
		if (*t == '/'  &&  c != '/'  &&  !match_slashes) {
			/*
			 * JR 7/20/2005: '/' is not matched by any pattern.
			 */
			return  0;
		}
		switch (c) {
			case '?':
				if (*t == '\0' || (t == text && *t == '.' && !match_leading_dot)) {
					return 0;
				}
				else {
					++t;
				}
				break;

			case '\\':
				if (*p++ != *t++) {
					return 0;
				}
				break;

			case '*':
				if (t == text && *t == '.' && !match_leading_dot) {
					return 0;
				}
				return (jr_glob_match_after_star (t, p, match_slashes)) ;

			case '[': {
				register char c1 = *t++;
				jr_int          invert;

				if (!c1) {
					return (0);
				}

				invert = ((*p == '!') || (*p == '^'));
				if (invert) p++;

				c = *p++;
				while (1) {
					register char cstart = c, cend = c;

					if (c == '\\') {
						cstart = *p++;
						cend = cstart;
					}

					if (c == '\0')
					return 0;

					c = *p++;
					if (c == '-' && *p != ']') {
						cend = *p++;
						if (cend == '\\') cend = *p++;
						if (cend == '\0') return 0;
						c = *p++;
					}
					if (c1 >= cstart && c1 <= cend)
					goto match;
					if (c == ']') break;
				}
				if (!invert) return 0;
				break;

				match:
				/* Skip the rest of the [...] construct that already matched.  */
				while (c != ']') { 
					if (c == '\0') return 0;

					c = *p++;
					if (c == '\0') return 0;
					else if (c == '\\') ++p;
				}
				if (invert) return 0;
				break;
			}

			default:
				if (c != *t++)
				return 0;
		}	/* switch */
	}	/* while */

	return (*t == '\0') ;
}


/*
 * Similar to jr_IsGlobMatch, but match PATTERN against any final segment of TEXT.
 */
static jr_int jr_glob_match_after_star (
	const char *  	text,
	const char *  	pattern,
	jr_int			match_slashes)
{
	register const char *p = pattern ;
	register const char *t = text;
	register char c, c1;

	while ((c = *p++) == '?' || c == '*') {
		if (c == '?' && *t++ == '\0') return 0;
	}

	if (c == '\0') {
		if (strchr (text, '/')  &&  !match_slashes) {
			/*
			 * JR 7/20/2005: '*' can't match '/'
			 */
			return 0;
		}
		return 1;
	}

	if (c == '\\') c1 = *p;
	else           c1 = c;

	while (1) {
		if ((c == '[' || *t == c1) && jr_IsGlobMatch (t, p - 1, match_slashes, 0)) {
			return 1;
		}
		if (*t == '/'  &&  !match_slashes) {
			/*
			 * JR 7/20/2005: '*' can't match '/'
			 */
			return 0;
		}
		if (*t++ == '\0') {
			return 0;
		}
	}
}

