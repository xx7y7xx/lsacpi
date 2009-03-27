/*-
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kevin Fall.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys/param.h>
#include <sys/stat.h>
#ifndef NO_UDOM_SUPPORT
//b1nggou	# include <sys/socket.h>
//b1nggou	# include <sys/un.h>
# include <errno.h>
#endif

#include <ctype.h>
//b1nggou	#include <err.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

int bflag, eflag, nflag, sflag, tflag, vflag;
int rval;
const char *filename;

static void usage(void);
static void scanfiles(char *argv[], int cooked);
static void cook_cat(FILE *);
static void raw_cat(int);

//b1nggou	#ifndef NO_UDOM_SUPPORT
//b1nggou	static int udom_open(const char *path, int flags);
//b1nggou	#endif

int
main(int argc, char *argv[])
{
	int ch;

	setlocale(LC_CTYPE, "");

	while ((ch = getopt(argc, argv, "benstuv")) != -1)
		switch (ch) {
		case 'b':
			bflag = nflag = 1;      /* -b implies -n */
			break;
		case 'e':
			eflag = vflag = 1;      /* -e implies -v */
			break;
		case 'n':
			nflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		case 't':
			tflag = vflag = 1;      /* -t implies -v */
			break;
		case 'u':
			setbuf(stdout, NULL);
			break;
		case 'v':
			vflag = 1;
			break;
		default:
			usage();
		}
	argv += optind;

	if (bflag || eflag || nflag || sflag || tflag || vflag)
		scanfiles(argv, 1);
	else
		scanfiles(argv, 0);
	if (fclose(stdout))
		printf("err(1, \"stdout\");");
	exit(rval);
	/* NOTREACHED */
}

static void
usage(void)
{
//b1nggou	fprintf(stderr, "usage: cat [-benstuv] [file ...]\n");
	fprintf(stderr, " ");
	printf("\
Concatenate FILE(s), or standard input, to standard output.\n\
\n\
  -A, --show-all           equivalent to -vET\n\
  -b, --number-nonblank    number nonblank output lines\n\
  -e                       equivalent to -vE\n\
  -E, --show-ends          display $ at end of each line\n\
  -n, --number             number all output lines\n\
  -s, --squeeze-blank      never more than one single blank line\n\
  -t                       equivalent to -vT\n\
  -T, --show-tabs          display TAB characters as ^I\n\
  -u                       (ignored)\n\
  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n\
      --help               display this help and exit\n\
      --version            output version information and exit\n\
\n\
With no FILE, or when FILE is -, read standard input.\n\
");
	exit(1);
	/* NOTREACHED */
}

static void
scanfiles(char *argv[], int cooked)
{
	int i = 0;
	char *path;
	FILE *fp;

	while ((path = argv[i]) != NULL || i == 0) {
		int fd;

		if (path == NULL || strcmp(path, "-") == 0) {
		/* b1nggou: Without INPUT file path. */
			filename = "stdin";
			fd = STDIN_FILENO;
		} else {
		/* b1nggou: With INPUT file path. */
			filename = path;
			fd = open(path, O_RDONLY);
//b1nggou	#ifndef NO_UDOM_SUPPORT
//b1nggou		if (fd < 0 && errno == EOPNOTSUPP)
//b1nggou			fd = udom_open(path, O_RDONLY);
//b1nggou	#endif
		}

		if (fd < 0) {
		/* b1nggou: open file error. */
			printf("warn(\"%s\", path);");
			rval = 1;
		} else if (cooked) {
		/* b1nggou: [cooked]=1, cat.exe with para. */
			if (fd == STDIN_FILENO)
				cook_cat(stdin);
			else {
				fp = fdopen(fd, "r");
				cook_cat(fp);
				fclose(fp);
			}
		} else {
			raw_cat(fd);
			if (fd != STDIN_FILENO)
				close(fd);
		}
		if (path == NULL)
			break;
		++i;
	}
}


static void
cook_cat(FILE *fp)
{
	int ch, gobble, line, prev;

	/* Reset EOF condition on stdin. */
	if (fp == stdin && feof(stdin))
		clearerr(stdin);

	line = gobble = 0;
	for (prev = '\n'; (ch = getc(fp)) != EOF; prev = ch) {
		if (prev == '\n') {
			if (sflag) {
				if (ch == '\n') {
					if (gobble)
						continue;
					gobble = 1;
				} else
					gobble = 0;
			}
			if (nflag && (!bflag || ch != '\n')) {
				(void)fprintf(stdout, "%6d\t", ++line);
				if (ferror(stdout))
					break;
			}
		}
		if (ch == '\n') {
			if (eflag && putchar('$') == EOF)
				break;
		} else if (ch == '\t') {
			if (tflag) {
				if (putchar('^') == EOF || putchar('I') == EOF)
					break;
				continue;
			}
		} else if (vflag) {
			if (!isascii(ch) && !isprint(ch)) {
				if (putchar('M') == EOF || putchar('-') == EOF)
					break;
				ch = toascii(ch);
			}
			if (iscntrl(ch)) {
				if (putchar('^') == EOF ||
				    putchar(ch == '\177' ? '?' :
				    ch | 0100) == EOF)
					break;
				continue;
			}
		}
		if (putchar(ch) == EOF)
			break;
	}
	if (ferror(fp)) {
		printf("warn(\"%s\", filename);");
		rval = 1;
		clearerr(fp);
	}
	if (ferror(stdout))
		printf("err(1, \"stdout\");");
}

static void
raw_cat(int rfd)
{
	int off, wfd;
	ssize_t nr, nw;
	static size_t bsize;
	static char *buf = NULL;
	struct stat sbuf;

	wfd = fileno(stdout);
	if (buf == NULL) {
		if (fstat(wfd, &sbuf))
			printf("err(1, \"%s\", filename);");
//b1nggou	#ifdef _MSC_VER
		bsize = 1024;
//b1nggou	#else
//b1nggou	bsize = MAX(sbuf.st_blksize, 1024);
//b1nggou	#endif
		if ((buf = malloc(bsize)) == NULL)
			printf("err(1, \"buffer\");");
	}
	while ((nr = read(rfd, buf, bsize)) > 0) {
		for (off = 0; nr; nr -= nw, off += nw)
			if ((nw = write(wfd, buf + off, (size_t)nr)) < 0)
				printf("err(1, \"stdout\");");
	}
	if (nr < 0) {
		printf("warn(\"%s\", filename);");
		rval = 1;
	}
}


//b1nggou	#ifndef NO_UDOM_SUPPORT
//b1nggou	
//b1nggou	static int
//b1nggou	udom_open(const char *path, int flags)
//b1nggou	{
//b1nggou		struct sockaddr_un sou;
//b1nggou		int fd;
//b1nggou		unsigned int len;
//b1nggou	
//b1nggou		bzero(&sou, sizeof(sou));
//b1nggou	
//b1nggou		/*
//b1nggou		 * Construct the unix domain socket address and attempt to connect
//b1nggou		 */
//b1nggou		fd = socket(AF_UNIX, SOCK_STREAM, 0);
//b1nggou		if (fd >= 0) {
//b1nggou			sou.sun_family = AF_UNIX;
//b1nggou			if ((len = strlcpy(sou.sun_path, path,
//b1nggou			    sizeof(sou.sun_path))) >= sizeof(sou.sun_path)) {
//b1nggou				errno = ENAMETOOLONG;
//b1nggou				return (-1);
//b1nggou			}
//b1nggou			len = offsetof(struct sockaddr_un, sun_path[len+1]);
//b1nggou	
//b1nggou			if (connect(fd, (void *)&sou, len) < 0) {
//b1nggou				close(fd);
//b1nggou				fd = -1;
//b1nggou			}
//b1nggou		}
//b1nggou	
//b1nggou		/*
//b1nggou		 * handle the open flags by shutting down appropriate directions
//b1nggou		 */
//b1nggou		if (fd >= 0) {
//b1nggou			switch(flags & O_ACCMODE) {
//b1nggou			case O_RDONLY:
//b1nggou				if (shutdown(fd, SHUT_WR) == -1)
//b1nggou					warn(NULL);
//b1nggou				break;
//b1nggou			case O_WRONLY:
//b1nggou				if (shutdown(fd, SHUT_RD) == -1)
//b1nggou					warn(NULL);
//b1nggou				break;
//b1nggou			default:
//b1nggou				break;
//b1nggou			}
//b1nggou		}
//b1nggou		return(fd);
//b1nggou	}
//b1nggou	
//b1nggou	#endif

