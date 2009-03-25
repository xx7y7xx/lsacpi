#include <sys/param.h>
#include <sys/stat.h>
/* #ifndef NO_UDOM_SUPPORT
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#endif */

#include <ctype.h>
//#include <err.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

int bflag, eflag, nflag, sflag, tflag, vflag;

int rval;
/*---------------------------------------------------------------------------------------
 *      返回值，定??全局?量，允?被所有函?修改
 *---------------------------------------------------------------------------------------
 */
const char *filename;

/*---------------------------------------------------------------------------------------
 *      ?前正在?理的文件名，定??全局?量，避免在函????
 *---------------------------------------------------------------------------------------
 */
static void usage(void);
//static void scanfiles(char *argv[], int cooked);
//static void cook_cat(FILE *);
//static void raw_cat(int);

/*---------------------------------------------------------------------------------------
 *      函??明
 *---------------------------------------------------------------------------------------
 */
#ifndef NO_UDOM_SUPPORT
static int udom_open(const char *path, int flags);
#endif

/*---------------------------------------------------------------------------------------
 *      ?函?可能与NFS文件系?有?
 *---------------------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	int ch;

	setlocale(LC_CTYPE, "");

/*---------------------------------------------------------------------------------------
 *      ?置locale，?入??""代表使用系??境?量定?的locale，例如zh_CN.eucCN
 *---------------------------------------------------------------------------------------
 */
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

/*---------------------------------------------------------------------------------------
 *      使用getopt函?分析命令行??，?置??的????，???取完?后，argv指向下一??，即第一?文件
 *      的文件名。 ?是?取命令行??的?准做法，?于getopt、optind的?容??man getopt。
 *---------------------------------------------------------------------------------------
 */
	if (bflag || eflag || nflag || sflag || tflag || vflag)
		//scanfiles(argv, 1);
		printf("scanfiles(argv, 1);");
	else
		//scanfiles(argv, 0);
		printf("scanfiles(argv, 0);");
	if (fclose(stdout))
		//err(1, "stdout");
	exit(rval);
	/* NOTREACHED */
}

/*---------------------------------------------------------------------------------------
 *      ?用scanfiles函??理??表中的所有文件，如果cat?有????，?scanfiles函?的cooked??被
 *      ?置?1（意思是需要??出?容再加工），否?cooked被?置?0。
 *      ?理完成后，???准?出，此?所有???中的?容都?被?出。注意cat的?准?出?常被重定向。
 *---------------------------------------------------------------------------------------
 */
static void
usage(void)
{
	fprintf(stderr, "usage: cat [-benstuv] [file ...]\n");
	exit(1);
	/* NOTREACHED */
}
