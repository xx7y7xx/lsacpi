/*---------------------------------------------------------------------------------------
 *      ��?�G  /usr/src/bin/cat.c:
 *      �{�ǡG  /bin/cat
 *      �w?�G  #define NO_UDOM_SUPPORT
 *      ?�`�G  ���y�z��?udom_open��?�e�A�i���ONFS���t?��?
 *
 *      ???���]??��?�^�G     
 *              -b      ?�D�Ŧ�?��?
 *              -e      ?�ܤ��i���L�r�Ŧ}�b���?��?����$
 *              -n      ?��?
 *              -s      ???���Ŧ�???�@?�Ŧ�
 *              -t      ?�ܤ��i���L�r�Ŧ}?tab�r��?��?^I
 *              -u      �T��?�X???
 *              -v      ?�ܤ��i���L�r��
 *---------------------------------------------------------------------------------------
 */
     1  /*-
     2   * Copyright (c) 1989, 1993
     3   *      The Regents of the University of California.  All rights reserved.
     4   *
     5   * This code is derived from software contributed to Berkeley by
     6   * Kevin Fall.
     7   *
     8   * Redistribution and use in source and binary forms, with or without
     9   * modification, are permitted provided that the following conditions
    10   * are met:
    11   * 1. Redistributions of source code must retain the above copyright
    12   *    notice, this list of conditions and the following disclaimer.
    13   * 2. Redistributions in binary form must reproduce the above copyright
    14   *    notice, this list of conditions and the following disclaimer in the
    15   *    documentation and/or other materials provided with the distribution.
    16   * 4. Neither the name of the University nor the names of its contributors
    17   *    may be used to endorse or promote products derived from this software
    18   *    without specific prior written permission.
    19   *
    20   * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
    21   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    22   * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    23   * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
    24   * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    25   * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    26   * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    27   * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    28   * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    29   * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    30   * SUCH DAMAGE.
    31   */
    32 
    33  #if 0
    34  #ifndef lint
    35  static char const copyright[] =
    36  "@(#) Copyright (c) 1989, 1993\n\
    37          The Regents of the University of California.  All rights reserved.\n";
    38  #endif /* not lint */
    39  #endif
    40 
    41  #ifndef lint
    42  #if 0
    43  static char sccsid[] = "@(#)cat.c       8.2 (Berkeley) 4/27/95";
    44  #endif
    45  #endif /* not lint */
    46  #include <sys/cdefs.h>
    47  __FBSDID("$FreeBSD: src/bin/cat/cat.c,v 1.32 2005/01/10 08:39:20 imp Exp $");
    48 
/*---------------------------------------------------------------------------------------
 *      FreeBSD��?�H��
 *---------------------------------------------------------------------------------------
 */
    49  #include <sys/param.h>
    50  #include <sys/stat.h>
    51  #ifndef NO_UDOM_SUPPORT
    52  #include <sys/socket.h>
    53  #include <sys/un.h>
    54  #include <errno.h>
    55  #endif
    56 
    57  #include <ctype.h>
    58  #include <err.h>
    59  #include <fcntl.h>
    60  #include <locale.h>
    61  #include <stdio.h>
    62  #include <stdlib.h>
    63  #include <string.h>
    64  #include <unistd.h>
    65  #include <stddef.h>
    66 
/*---------------------------------------------------------------------------------------
 *      ?���
 *---------------------------------------------------------------------------------------
 */
    67  int bflag, eflag, nflag, sflag, tflag, vflag;
/*---------------------------------------------------------------------------------------
 *      ????�A��?�O-benstv??�A??��?1���?????
 *---------------------------------------------------------------------------------------
 */
    68  int rval;
/*---------------------------------------------------------------------------------------
 *      ��^�ȡA�w??����?�q�A��?�Q�Ҧ���?�ק�
 *---------------------------------------------------------------------------------------
 */
    69  const char *filename;
    70 
/*---------------------------------------------------------------------------------------
 *      ?�e���b?�z�����W�A�w??����?�q�A�קK�b��????
 *---------------------------------------------------------------------------------------
 */
    71  static void usage(void);
    72  static void scanfiles(char *argv[], int cooked);
    73  static void cook_cat(FILE *);
    74  static void raw_cat(int);
    75 
/*---------------------------------------------------------------------------------------
 *      ��??��
 *---------------------------------------------------------------------------------------
 */
    76  #ifndef NO_UDOM_SUPPORT
    77  static int udom_open(const char *path, int flags);
    78  #endif
    79 
/*---------------------------------------------------------------------------------------
 *      ?��?�i���ONFS���t?��?
 *---------------------------------------------------------------------------------------
 */
    80  int
    81  main(int argc, char *argv[])
    82  {
    83          int ch;
    84 
    85          setlocale(LC_CTYPE, "");
    86 
/*---------------------------------------------------------------------------------------
 *      ?�mlocale�A?�J??""�N��ϥΨt??��?�q�w?��locale�A�Ҧpzh_CN.eucCN
 *---------------------------------------------------------------------------------------
 */
    87          while ((ch = getopt(argc, argv, "benstuv")) != -1)
    88                  switch (ch) {
    89                  case 'b':
    90                          bflag = nflag = 1;      /* -b implies -n */
    91                          break;
    92                  case 'e':
    93                          eflag = vflag = 1;      /* -e implies -v */
    94                          break;
    95                  case 'n':
    96                          nflag = 1;
    97                          break;
    98                  case 's':
    99                          sflag = 1;
   100                          break;
   101                  case 't':
   102                          tflag = vflag = 1;      /* -t implies -v */
   103                          break;
   104                  case 'u':
   105                          setbuf(stdout, NULL);
   106                          break;
   107                  case 'v':
   108                          vflag = 1;
   109                          break;
   110                  default:
   111                          usage();
   112                  }
   113          argv += optind;
   114 
/*---------------------------------------------------------------------------------------
 *      �ϥ�getopt��?���R�R�O��??�A?�m??��????�A???����?�Z�Aargv���V�U�@??�A�Y�Ĥ@?���
 *      �����W�C ?�O?���R�O��??��?�㰵�k�A?�_getopt�Boptind��?�e??man getopt�C
 *---------------------------------------------------------------------------------------
 */
   115          if (bflag || eflag || nflag || sflag || tflag || vflag)
   116                  scanfiles(argv, 1);
   117          else
   118                  scanfiles(argv, 0);
   119          if (fclose(stdout))
   120                  err(1, "stdout");
   121          exit(rval);
   122          /* NOTREACHED */
   123  }
   124 
/*---------------------------------------------------------------------------------------
 *      ?��scanfiles��??�z??�����Ҧ����A�p�Gcat?��????�A?scanfiles��?��cooked??�Q
 *      ?�m?1�]�N��O�ݭn??�X?�e�A�[�u�^�A�_?cooked�Q?�m?0�C
 *      ?�z�����Z�A???��?�X�A��?�Ҧ�???����?�e��?�Q?�X�C�`�Ncat��?��?�X?�`�Q���w�V�C
 *---------------------------------------------------------------------------------------
 */
   125  static void
   126  usage(void)
   127  {
   128          fprintf(stderr, "usage: cat [-benstuv] [file ...]\n");
   129          exit(1);
   130          /* NOTREACHED */
   131  }
   132 
/*---------------------------------------------------------------------------------------
 *      ?�Xcat���Ϊk�M??�A�M�Z�h�X�{�ǡC
 *---------------------------------------------------------------------------------------
 */
   133  static void
   134  scanfiles(char *argv[], int cooked)
   135  {
/*---------------------------------------------------------------------------------------
 *      scanfiles�̦�?��argv??�����C�@?���A���u???�z?�J��?�e�A�M�Z?��?��?�X�C
 *---------------------------------------------------------------------------------------
 */
   136          int i = 0;
   137          char *path;
   138          FILE *fp;
   139 
   140          while ((path = argv[i]) != NULL || i == 0) {
/*---------------------------------------------------------------------------------------
 *      �Ĥ@����?�Gargv??��?�šA?�M?argv??�A?argv[i]���V�Ŧ�?�A�`??���C
 *      �ĤG����?�Gargv???�šA�Yargv[0] == NULL�A��?cat�ݭn??��?�J�`���?��?�X�A�`?�^??
 *      ��@���C
 *---------------------------------------------------------------------------------------
 */
   141                  int fd;
   142 
   143                  if (path == NULL || strcmp(path, "-") == 0) {
   144                          filename = "stdin";
   145                          fd = STDIN_FILENO;
/*---------------------------------------------------------------------------------------
 *      ?path?�ũ�"-"?�A??�O??��?�J��?��
 *---------------------------------------------------------------------------------------
 */
   146                  } else {
   147                          filename = path;
   148                          fd = open(path, O_RDONLY);
   149  #ifndef NO_UDOM_SUPPORT
   150                          if (fd < 0 && errno == EOPNOTSUPP)
   151                                  fd = udom_open(path, O_RDONLY);
   152  #endif
   153                  }
/*---------------------------------------------------------------------------------------
 *      �_?�A�H�u?�覡��?path���w�����A�Yopen��?�A?���i��?��󤣤��open��?�A??udom_open�C
 *---------------------------------------------------------------------------------------
 */
   154                  if (fd < 0) {
   155                          warn("%s", path);
   156                          rval = 1;
/*---------------------------------------------------------------------------------------
 *      ��?���??�A?��ĵ�i�H���A?�m��^��?1�A�{�Ǥ��h�X�A???�z�U�@?���C
 *---------------------------------------------------------------------------------------
 */
   157                  } else if (cooked) {
   158                          if (fd == STDIN_FILENO)
   159                                  cook_cat(stdin);
   160                          else {
   161                                  fp = fdopen(fd, "r");
   162                                  cook_cat(fp);
   163                                  fclose(fp);
   164 
   165                  } else {
   166                          raw_cat(fd);
   167                          if (fd != STDIN_FILENO)
   168                                  close(fd);
   169                  }
                        }
/*---------------------------------------------------------------------------------------
 *      �Ycat?��??�A?��cook_cat?�z���A�_?��raw_cat?�z���C
 *      �`�N��cook_cat�ݭn?�J�@?����?�A��raw_cat�n�D���y�z�šA?��������P���覡�Hraw_cat�u�O
 *      ??�a?�J�@??�u�A�M�Z?�X�A��read�Mwrite�t??�ΧY�i�C��cook_cat�ݭn�@?�@?�a?�J�r�Ŧ}��
 *      �R?�e�Aread�Mwrite�t??�ΥΤ_?�z??�r�šA�Ĳv�ܧC�A�Ѥ_C?��?IO?��??���A�G��?�ϥ�?
 *      ��?��??�o��n���ʯ�C
 *      �̦Z?�n�`�N?��?�J�M�䥦���????�ݡA����???��?�J�C
 *---------------------------------------------------------------------------------------
 */
   170                  if (path == NULL)
   171                          break;
/*---------------------------------------------------------------------------------------
 *      ?cat?�����w?�J���?�A�`????���h�X�C
 *---------------------------------------------------------------------------------------
 */
   172                  ++i;
   173          }
   174  }
   175 
/*---------------------------------------------------------------------------------------
 *      scanfiles��?�w??���C
 *---------------------------------------------------------------------------------------
 */
   176  static void
   177  cook_cat(FILE *fp)
   178  {
/*---------------------------------------------------------------------------------------
 *      cook_cat�v�r�Ŧa?�J���?�e�A����??���R�Z�A?�X�C
 *---------------------------------------------------------------------------------------
 */
   179          int ch, gobble, line, prev;
   180 
   181          /* Reset EOF condition on stdin. */
   182          if (fp == stdin && feof(stdin))
   183                  clearerr(stdin);
   184 
/*---------------------------------------------------------------------------------------
 *      �Y?��?�J���t���W��?�d�����?���šA?��M���C?�O�i��?�ͪ��A?��??����?�G"cat - 1.c -"�C
 *---------------------------------------------------------------------------------------
 */
   185          line = gobble = 0;
   186          for (prev = '\n'; (ch = getc(fp)) != EOF; prev = ch) {
/*---------------------------------------------------------------------------------------
 *      �`??�J�r�šC
 *---------------------------------------------------------------------------------------
 */
   187                  if (prev == '\n') {
/*---------------------------------------------------------------------------------------
 *      �p�G�e�@?�r�ŬO'\n'�A?��?�e�r��ch�O�s�檺??�C
 *---------------------------------------------------------------------------------------
 */
   188                          if (sflag) {
   189                                  if (ch == '\n') {
   190                                          if (gobble)
   191                                                  continue;
   192                                          gobble = 1;
   193                                  } else
   194                                          gobble = 0;
   195                          }
/*---------------------------------------------------------------------------------------
 *      �Y���w�F-s??�A?�ݭn?�h???�Ŧ�???�@��C�p�G��?ch�O'\n'�]�Mprev�@?�^�A?��?�e��O��
 *      ��A�A�ҹ�gobble?�q�C�Ygobble?0�A?���e�@�椣�O�Ŧ�A�ݭn?�X?�e���Ŧ�A��??gobble�m?1�F
 *      �Ygobble?1�A?���e�w?�X?�Ŧ�A���ΦA?�X�A�_�O???���U�@?�r�šC
 *---------------------------------------------------------------------------------------
 */
   196                          if (nflag && (!bflag || ch != '\n')) {
   197                                  (void)fprintf(stdout, "%6d\t", ++line);
   198                                  if (ferror(stdout))
   199                                          break;
   200                          }
/*---------------------------------------------------------------------------------------
 *      ��?�A�Ѥ_ch�O�s�檺??�A�ҥH�p�G���w�F-n��-b???�A???�X��?�C�`�N-b???�t�F-n??�A�b
 *      ��getopt���R�R�O��???�w?�̫O?bflag?1?�Anflag�]?1�C
 *---------------------------------------------------------------------------------------
 */
   201                  }
   202                  if (ch == '\n') {
   203                          if (eflag && putchar('$') == EOF)
   204                                  break;
/*---------------------------------------------------------------------------------------
 *      �J��?���?�A�Y���w�F-e??�A??�X��?����$�A�`�N-e???�t�F-v??�C
 *---------------------------------------------------------------------------------------
 */
   205                  } else if (ch == '\t') {
   206                          if (tflag) {
   207                                  if (putchar('^') == EOF || putchar('I') == EOF)
   208                                          break;
   209                                  continue;
   210                          }
/*---------------------------------------------------------------------------------------
 *      �J��tab�r��?�A�Y���w�F-t??�A??�X^I�A�`�N-t???�t�F-v??�C
 *---------------------------------------------------------------------------------------
 */
   211                  } else if (vflag) {
   212                          if (!isascii(ch) && !isprint(ch)) {
   213                                  if (putchar('M') == EOF || putchar('-') == EOF)
   214                                          break;
   215                                  ch = toascii(ch);
   216                          }
   217                          if (iscntrl(ch)) {
   218                                  if (putchar('^') == EOF ||
   219                                      putchar(ch == '\177' ? '?' :
   220                                      ch | 0100) == EOF)
   221                                          break;
   222                                  continue;
   223                          }
   224                  }
/*---------------------------------------------------------------------------------------
 *      �Y���w�F-v??�A?����cat��?��?�����覡?�ܤ��i���L�r�ũM����r�šC�ݭn�S?���X�A�r��'\177'��
 *      �ܰh��r�šA���ઽ��?�X�A�_???���e�@?�r�šA�ҥH?�X'?'�N�����C
 *---------------------------------------------------------------------------------------
 */
   225                  if (putchar(ch) == EOF)
   226                          break;
   227          }
   228          if (ferror(fp)) {
   229                  warn("%s", filename);
   230                  rval = 1;
   231                  clearerr(fp);
   232          }
   233          if (ferror(stdout))
   234                  err(1, "stdout");
   235  }
/*---------------------------------------------------------------------------------------
 *      �Y?�z?�{��?�J���X?�A?�M��??�A???�z�U�@?���F�Y?�X���X?�A?�{��?�k??�A��?�u
 *      ��h�X�{�ǡC
 *---------------------------------------------------------------------------------------
 */
   236 
   237  static void
   238  raw_cat(int rfd)
   239  {
/*---------------------------------------------------------------------------------------
 *      raw_cat�@��?���@��??�u�A�A��??�X�A?�X�e��������?�z�C
 *---------------------------------------------------------------------------------------
 */
   240          int off, wfd;
   241          ssize_t nr, nw;
   242          static size_t bsize;
   243          static char *buf = NULL;
   244          struct stat sbuf;
   245 
   246          wfd = fileno(stdout);
   247          if (buf == NULL) {
   248                  if (fstat(wfd, &sbuf))
   249                          err(1, "%s", filename);
   250                  bsize = MAX(sbuf.st_blksize, 1024);
   251                  if ((buf = malloc(bsize)) == NULL)
   252                          err(1, "buffer");
   253          }
/*---------------------------------------------------------------------------------------
 *      ��fstat��??�o���H���Ast_blksize���???���?�̦X�쪺?�j�p�A���u??�H�����t�j�p�̦X�쪺
 *      ???�C�`�N?��malloc���t��?�s�}?����free��??��A�i��@��?�o�{�ǰh�X?��?�^����?�A��?��
 *      �Ϧӧ��Ĳv�a�C
 *---------------------------------------------------------------------------------------
 */
   254          while ((nr = read(rfd, buf, bsize)) > 0)
   255                  for (off = 0; nr; nr -= nw, off += nw)
   256                          if ((nw = write(wfd, buf + off, (size_t)nr)) < 0)
   257                                  err(1, "stdout");
/*---------------------------------------------------------------------------------------
 *      ?�����?�}?�J?�X���A�`�N?��?�zwrite���覡�Acat?�����wwrite��?�@���ʧ����A�]?cat�}��
 *      ��???��?�X?�Q���w�V������C�p�G?��?�X�Q���w�V��@?��?��?write����m�A���\?���Ұ���?�z
 *      �N�O���ݪ��C
 *---------------------------------------------------------------------------------------
 */
   258          if (nr < 0) {
   259                  warn("%s", filename);
   260                  rval = 1;
   261          }
   262  }
/*---------------------------------------------------------------------------------------
 *      �p�G?��??�A?���Lĵ�i�H���A�{��???�z�C
 *---------------------------------------------------------------------------------------
 */
   263 
   264  #ifndef NO_UDOM_SUPPORT
   265 
   266  static int
   267  udom_open(const char *path, int flags)
   268  {
   269          struct sockaddr_un sou;
   270          int fd;
   271          unsigned int len;
   272 
   273          bzero(&sou, sizeof(sou));
   274 
   275          /*
   276           * Construct the unix domain socket address and attempt to connect
   277           */
   278          fd = socket(AF_UNIX, SOCK_STREAM, 0);
   279          if (fd >= 0) {
   280                  sou.sun_family = AF_UNIX;
   281                  if ((len = strlcpy(sou.sun_path, path,
   282                      sizeof(sou.sun_path))) >= sizeof(sou.sun_path)) {
   283                          errno = ENAMETOOLONG;
   284                          return (-1);
   285                  }
   286                  len = offsetof(struct sockaddr_un, sun_path[len+1]);
   287 
   288                  if (connect(fd, (void *)&sou, len) < 0) {
   289                          close(fd);
   290                          fd = -1;
   291                  }
   292          }
   293 
   294          /*
   295           * handle the open flags by shutting down appropriate directions
   296           */
   297          if (fd >= 0) {
   298                  switch(flags & O_ACCMODE) {
   299                  case O_RDONLY:
   300                          if (shutdown(fd, SHUT_WR) == -1)
   301                                  warn(NULL);
   302                          break;
   303                  case O_WRONLY:
   304                          if (shutdown(fd, SHUT_RD) == -1)
   305                                  warn(NULL);
   306                          break;
   307                  default:
   308                          break;
   309                  }
   310          }
   311          return(fd);
   312  }
   313 
   314  #endif
/*---------------------------------------------------------------------------------------
 *      udom_open��?�ϥΤFunix��M���r�A??�譱��?�@?�Ҫ��A�`�P?�\�C
 *      ?�K���@�y�A�N?�e������?���O�ϥ�cat�[�W���C
 *---------------------------------------------------------------------------------------
 */