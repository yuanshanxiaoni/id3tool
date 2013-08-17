/* config.h
 *
 *  id3tool:  An editor for ID3 tags.
 *  Copyright (C) 1999-2000  Christopher Collins
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _CONFIG_H
#define _CONFIG_H
@TOP@

/* Version Symbols */
#undef PACKAGE
#undef VERSION

/* Define if you have the ANSI C header files.  */
#undef STDC_HEADERS

/* Define if you have the strdup function.  */
#undef HAVE_STRDUP

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

@BOTTOM@
/*=======================================================================*/
/*                    DO NOT EDIT BELOW THIS POINT!!!!                   */
/*=======================================================================*/

#ifndef HAVE_STRDUP
extern char *	my_strdup (char *strin);
#define strdup(x) my_strdup(x)
#endif

#ifndef HAVE_STRCASECMP
#ifdef HAVE_STRICMP
#define strcasecmp(x,y) stricmp(x,y)
#else
#define strcasecmp(x,y) strcmp(x,y)
#endif /* HAVE_STRICMP */
#endif /* HAVE_STRCASECMP */

#ifndef HAVE_GETOPT_LONG
#include "bsd_getopt.h"
#endif

#endif
