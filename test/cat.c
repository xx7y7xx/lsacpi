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
 *      ��^�ȡA�w??����?�q�A��?�Q�Ҧ���?�ק�
 *---------------------------------------------------------------------------------------
 */
const char *filename;

/*---------------------------------------------------------------------------------------
 *      ?�e���b?�z�����W�A�w??����?�q�A�קK�b��????
 *---------------------------------------------------------------------------------------
 */
static void usage(void);
//static void scanfiles(char *argv[], int cooked);
//static void cook_cat(FILE *);
//static void raw_cat(int);

/*---------------------------------------------------------------------------------------
 *      ��??��
 *---------------------------------------------------------------------------------------
 */
#ifndef NO_UDOM_SUPPORT
static int udom_open(const char *path, int flags);
#endif

/*---------------------------------------------------------------------------------------
 *      ?��?�i���ONFS���t?��?
 *---------------------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	int ch;

	setlocale(LC_CTYPE, "");

/*---------------------------------------------------------------------------------------
 *      ?�mlocale�A?�J??""�N��ϥΨt??��?�q�w?��locale�A�Ҧpzh_CN.eucCN
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
 *      �ϥ�getopt��?���R�R�O��??�A?�m??��????�A???����?�Z�Aargv���V�U�@??�A�Y�Ĥ@?���
 *      �����W�C ?�O?���R�O��??��?�㰵�k�A?�_getopt�Boptind��?�e??man getopt�C
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
 *      ?��scanfiles��??�z??�����Ҧ����A�p�Gcat?��????�A?scanfiles��?��cooked??�Q
 *      ?�m?1�]�N��O�ݭn??�X?�e�A�[�u�^�A�_?cooked�Q?�m?0�C
 *      ?�z�����Z�A???��?�X�A��?�Ҧ�???����?�e��?�Q?�X�C�`�Ncat��?��?�X?�`�Q���w�V�C
 *---------------------------------------------------------------------------------------
 */
static void
usage(void)
{
	fprintf(stderr, "usage: cat [-benstuv] [file ...]\n");
	exit(1);
	/* NOTREACHED */
}
