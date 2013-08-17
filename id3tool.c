/* id3tool.c
 *
 * Tool for manipulating ID3 Tags in MP3 files
*/

/*  id3tool:  An editor for ID3 tags.
 *  Copyright (C) 1999-2005  Christopher Collins
 *
 * This program was authored principly by Christopher Collins (aka
 * Crossfire).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author, Christopher Collins, nor any of his
 *    aliases may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CHRISTOPHER COLLINS ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL CHRISTOPHER COLLINS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif

#include "id3.h"

struct option id3options[] = {
	{"set-title", required_argument, NULL, 't'},
	{"set-album", required_argument, NULL, 'a'},
	{"set-artist", required_argument, NULL, 'r'},
	{"set-year", required_argument, NULL, 'y'},
	{"set-note", required_argument, NULL, 'n'},
	{"set-genre", required_argument, NULL, 'g'},
	{"set-genre-word", required_argument, NULL, 'G'},
	{"set-track", required_argument, NULL, 'c'},
	{"genre-list", no_argument, NULL, 'l'}, 
	{"version", no_argument, NULL, 'v'},
	{"help", no_argument, NULL, 'h'},
	{0, 0, 0, 0}
};
#define ID3TOOL_GETOPT_ARGS "t:a:r:y:n:g:G:c:lvh"

#ifdef NEED_STRDUP
extern char *
my_strdup (char *strin)
{
	int	len;
	char	*newloc;
	len = strlen (strin) + 1;
	newloc = malloc (len);
	if (newloc != NULL) {
		strcpy (newloc, strin);
	}
	return (newloc);
}
#endif

void
showblurb(void)
{
	printf ("id3tool version %s\n", VERSION);
	printf ("Copyright (C) 1999-2005, Christopher Collins\n");
	printf ("id3tool comes with ABSOLUTELY NO WARRANTY.  This is free "
		"software, and you are\n");
	printf ("welcome to redistribute it under certain conditions.  For "
		"details refer to the\n");
	printf ("file \"COPYING\" included with this program.\n");

#ifndef HAVE_GETOPT_LONG
	/* only display if we're using BSD getopt */
	printf ("This product includes software developed by the NetBSD "
		"Foundation, Inc. and\n");
	printf ("its contributors.\n");       
#endif
}

void
showusage(int argc, char **argv)
{
	printf ("usage:\n");
	printf ("%s [<options>] <filename>\n", argv[0]);
	printf ("  -t, --set-title=WORD\t\tSets the title to WORD\n");
	printf ("  -a, --set-album=WORD\t\tSets the album to WORD\n");
	printf ("  -r, --set-artist=WORD\t\tSets the artist to WORD\n");
	printf ("  -y, --set-year=YEAR\t\tSets the year to YEAR [4 digits]\n");
	printf ("  -n, --set-note=WORD\t\tSets the note to WORD\n");
	printf ("  -g, --set-genre=INT\t\tSets the genre code to INT\n");
	printf ("  -G, --set-genre-word=WORD\tSets the genre to WORD\n");
	printf ("  -c, --set-track=INT\t\tSets the track number to INT\n");
	printf ("  -l, --genre-list\t\tShows the Genre's and their codes\n");
	printf ("  -v, --version\t\t\tDisplays the version\n");
	printf ("  -h, --help\t\t\tDisplays this message\n");
	printf ("\nReport bugs to Chris Collins <ccollins@pcug.org.au>\n");
}

int
main (int argc, char **argv)
{
	int		retval;
	char *		newtitle = NULL;
	char *		newalbum = NULL;
	char *		newartist = NULL;
	char *		newnote = NULL;
	long		newyear = -1;
	long		newgenre = -1;
	long		newtrack = -1;
	id3tag_t	mytag;
	FILE		*fptr = NULL;
	int		newtag = 0;
	char		strbuf[31];
	int		opt_index = 0;
	int		ctr;

	retval = getopt_long (argc, argv, ID3TOOL_GETOPT_ARGS, 
			      id3options, &opt_index);
	while (retval != EOF) {
		switch (retval) {
		case 't': /* set title */
			if (NULL != newtitle) {
				fprintf (stderr, "%s: title already set\n",
					 argv[0]);
				exit (1);
			}
			newtitle = strdup(optarg);
			break;
		case 'a': /* set album */
			if (NULL != newalbum) {
				fprintf (stderr, "%s: album already set\n", 
					 argv[0]);
				exit (1);
			}
			newalbum = strdup(optarg);
			break;
		case 'r': /* set artist */
			if (NULL != newartist) {
				fprintf (stderr, "%s: artist already set\n", 
					 argv[0]);
				exit (1);
			}
			newartist = strdup(optarg);
			break;
		case 'y': /* set year */
			if (newyear >= 0) {
				fprintf (stderr, "%s: year already set\n", 
					 argv[0]);
				exit (1);
			}
			newyear = atoi(optarg);
			break;
		case 'n': /* set note */
			if (NULL != newnote) {
				fprintf (stderr, "%s: note already set\n", 
					 argv[0]);
				exit (1);
			}
			newnote = strdup(optarg);
			break;
		case 'g': /* set genre */
			if (newgenre >= 0) {
				fprintf (stderr, "%s: genre already set\n",
					 argv[0]);
				exit (1);
			}
			newgenre = atoi(optarg);
			break;
		case 'c': /* set track */
			if (newtrack >= 0) {
				fprintf (stderr, "%s: track already set\n",
					 argv[0]);
				exit (1);
			}
			newtrack = atoi(optarg);
			break;
		case 'G': /* try to set a genre by name */
			for (ctr = 0; id3_styles[ctr].name != NULL; ctr++) {
				if (!strcasecmp(optarg, 
						id3_styles[ctr].name)) {
					/* we found it! */
					break;
				}
			}
			if (id3_styles[ctr].name == NULL) {
				fprintf (stderr, 
					 "%s: Couldn't find genre \"%s\"\n", 
					 argv[0], optarg);
				exit (1);
			}
			newgenre = id3_styles[ctr].styleid;
			break;
		case 'l': /* show a list of the genres */
			printf ("%-40s | %s\n", "Style", "ID");
			printf ("------------------------------"
				"-----------+-----\n");
			for (ctr = 0; id3_styles[ctr].name != NULL; ctr++) {
				printf ("%-40s | 0x%02X\n", 
					id3_styles[ctr].name, 
					id3_styles[ctr].styleid);
			}
			exit (0);     
		case 'v': /* get version */
			showblurb();
			
			exit (0);
		case 'h': /* show usage */
			showblurb();
			printf("\n");
			showusage(argc, argv);
			exit (0);
		case '?':
			exit (1);
		default:
			break;
		}
		retval = getopt_long (argc, argv, ID3TOOL_GETOPT_ARGS,
				      id3options, &opt_index);
	}
	if (optind + 1 > argc) {
		fprintf (stderr, "%s: Not enough arguments\n", argv[0]);
		showblurb();
		printf("\n");
		showusage(argc, argv);
		exit (1);
	}
	for (ctr = optind; ctr < argc; ctr++) {
		fptr = fopen(argv[ctr], "rb");
		if (NULL == fptr) {
			fprintf (stderr, "%s: Can't open file \"%s\" "
				 "for read.\n", argv[0], argv[ctr]);
			exit (1);
		}
		if (id3_readtag(fptr, &mytag)) {
			newtag = 1;
			id3_cleartag(&mytag);
		} else {
			newtag = 0;
		}
		fclose(fptr);
		if (!newtitle && !newartist && !newalbum && !newnote &&
		    (newyear < 0) && (newgenre < 0) && (newtrack < 0)) {
			/* print id3 tag */
			printf ("Filename: %s\n", argv[ctr]);
			if (newtag) {
				printf ("No ID3 Tag\n");
			} else {
				if (mytag.songname[0] != '\0') {
					strncpy(strbuf, mytag.songname, 30);
					strbuf[30] = '\0';
					printf ("Song Title:\t%-30s\n", strbuf);
				}
				if (mytag.artist[0] != '\0') {
					strncpy(strbuf, mytag.artist, 30);
					strbuf[30] = '\0';
					printf ("Artist:\t\t%-30s\n", strbuf);
				}
				if (mytag.album[0] != '\0') {
					strncpy(strbuf, mytag.album, 30);
					strbuf[30] = '\0';
					printf ("Album:\t\t%-30s\n", strbuf);
				}
				if (mytag.note.v11.marker == '\0') {
					/* use v1.1 symantics */
					if (mytag.note.v11.note[0] != '\0') {
						strncpy(strbuf, mytag.note.v11.note, 28);
						strbuf[28] = '\0';
						printf ("Note:\t\t%-28s\n", strbuf);
					}
					if (mytag.note.v11.track != 0) {
						printf ("Track:\t\t%d\n", mytag.note.v11.track);
					}
				} else {
					if (mytag.note.v10.note[0] != '\0') {
						strncpy(strbuf, mytag.note.v10.note, 30);
						strbuf[30] = '\0';
						printf ("Note:\t\t%-30s\n", strbuf);
					}
				}
				if (mytag.year[0] != '\0') {
					strncpy(strbuf, mytag.year, 4);
					strbuf[4] = '\0';
					printf ("Year:\t\t%-4s\n", strbuf);
				}
				if (mytag.style != 0xFF) {
					printf ("Genre:\t\t%s (0x%X)\n", 
						id3_findstyle(mytag.style),
						mytag.style);
				}
			}
			printf ("\n");
		} else {
			/* we have something to set! :) */
			if (newtitle) {
				memset (mytag.songname, 0, 30);
				strncpy (mytag.songname, newtitle, 30);
			}
			if (newartist) {
				memset (mytag.artist, 0, 30);
				strncpy (mytag.artist, newartist, 30);
			}
			if (newalbum) {
				memset (mytag.album, 0, 30);
				strncpy (mytag.album, newalbum, 30);
			}
			if (newnote) {
				/* check for existing v11 data */
				if (mytag.note.v11.marker == '\0' 
				    && mytag.note.v11.track != 0
				    && (newtrack != 0)) {
					memset (mytag.note.v10.note, 0, 28);
					strncpy (mytag.note.v11.note, newnote, 28);
				} else {
					memset (mytag.note.v10.note, 0, 30);
					strncpy (mytag.note.v10.note, newnote, 30);
				}
			}
			/* basic epoch selection */
			if (newyear >= 0) {
				if (newyear < 100) {
					if (newyear > 70) {
						newyear += 1900;
					} else {
						newyear += 2000;
					}
				}
				sprintf (strbuf, "%4.4ld", newyear);
				memset (mytag.year, 0, 4);
				strncpy (mytag.year, strbuf, 4);
			}
			if (newgenre >= 0) {
				mytag.style = newgenre;
			}
			if (newtrack > 0) {
				mytag.note.v11.marker = '\0';
				mytag.note.v11.track = newtrack;
			}
			
			fptr = fopen(argv[ctr], "r+b");
			if (NULL == fptr) {
				fprintf (stderr, "%s: Can't open file \"%s\" "
					 "for write.\n",  argv[0], argv[ctr]);
				exit (1);
			}
			if (newtag) {
				id3_appendtag(fptr, &mytag);
			} else {
				id3_replacetag(fptr, &mytag);
			}
			fclose(fptr);
		}
	}	

	/* we have something to set! :) */
	if (newtitle) {
		free(newtitle);
	}
	if (newartist) {
		free(newartist);
	}
	if (newalbum) {
		free (newalbum);
	}
	if (newnote) {
		free (newnote);
	}
	return (0);
}
