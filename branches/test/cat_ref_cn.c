/*---------------------------------------------------------------------------------------
 *      路?：  /usr/src/bin/cat.c:
 *      程序：  /bin/cat
 *      定?：  #define NO_UDOM_SUPPORT
 *      ?注：  未描述有?udom_open的?容，可能与NFS文件系?有?
 *
 *      ???明（??手?）：     
 *              -b      ?非空行?行?
 *              -e      ?示不可打印字符并在行尾?示?束符$
 *              -n      ?行?
 *              -s      ???的空行???一?空行
 *              -t      ?示不可打印字符并?tab字符?示?^I
 *              -u      禁用?出???
 *              -v      ?示不可打印字符
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
 *      FreeBSD版?信息
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
 *      ?文件
 *---------------------------------------------------------------------------------------
 */
    67  int bflag, eflag, nflag, sflag, tflag, vflag;
/*---------------------------------------------------------------------------------------
 *      ????，分?与-benstv??，??值?1表示?????
 *---------------------------------------------------------------------------------------
 */
    68  int rval;
/*---------------------------------------------------------------------------------------
 *      返回值，定??全局?量，允?被所有函?修改
 *---------------------------------------------------------------------------------------
 */
    69  const char *filename;
    70 
/*---------------------------------------------------------------------------------------
 *      ?前正在?理的文件名，定??全局?量，避免在函????
 *---------------------------------------------------------------------------------------
 */
    71  static void usage(void);
    72  static void scanfiles(char *argv[], int cooked);
    73  static void cook_cat(FILE *);
    74  static void raw_cat(int);
    75 
/*---------------------------------------------------------------------------------------
 *      函??明
 *---------------------------------------------------------------------------------------
 */
    76  #ifndef NO_UDOM_SUPPORT
    77  static int udom_open(const char *path, int flags);
    78  #endif
    79 
/*---------------------------------------------------------------------------------------
 *      ?函?可能与NFS文件系?有?
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
 *      ?置locale，?入??""代表使用系??境?量定?的locale，例如zh_CN.eucCN
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
 *      使用getopt函?分析命令行??，?置??的????，???取完?后，argv指向下一??，即第一?文件
 *      的文件名。 ?是?取命令行??的?准做法，?于getopt、optind的?容??man getopt。
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
 *      ?用scanfiles函??理??表中的所有文件，如果cat?有????，?scanfiles函?的cooked??被
 *      ?置?1（意思是需要??出?容再加工），否?cooked被?置?0。
 *      ?理完成后，???准?出，此?所有???中的?容都?被?出。注意cat的?准?出?常被重定向。
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
 *      ?出cat的用法和??，然后退出程序。
 *---------------------------------------------------------------------------------------
 */
   133  static void
   134  scanfiles(char *argv[], int cooked)
   135  {
/*---------------------------------------------------------------------------------------
 *      scanfiles依次?取argv??中的每一?文件，根据???理?入的?容，然后?到?准?出。
 *---------------------------------------------------------------------------------------
 */
   136          int i = 0;
   137          char *path;
   138          FILE *fp;
   139 
   140          while ((path = argv[i]) != NULL || i == 0) {
/*---------------------------------------------------------------------------------------
 *      第一种情?：argv??不?空，?遍?argv??，?argv[i]指向空串?，循??束。
 *      第二种情?：argv???空，即argv[0] == NULL，此?cat需要??准?入复制到?准?出，循?体??
 *      行一次。
 *---------------------------------------------------------------------------------------
 */
   141                  int fd;
   142 
   143                  if (path == NULL || strcmp(path, "-") == 0) {
   144                          filename = "stdin";
   145                          fd = STDIN_FILENO;
/*---------------------------------------------------------------------------------------
 *      ?path?空或"-"?，??是??准?入中?取
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
 *      否?，以只?方式打?path指定的文件，若open失?，?有可能?文件不支持open函?，??udom_open。
 *---------------------------------------------------------------------------------------
 */
   154                  if (fd < 0) {
   155                          warn("%s", path);
   156                          rval = 1;
/*---------------------------------------------------------------------------------------
 *      打?文件??，?示警告信息，?置返回值?1，程序不退出，???理下一?文件。
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
 *      若cat?有??，?用cook_cat?理文件，否?用raw_cat?理文件。
 *      注意到cook_cat需要?入一?文件指?，而raw_cat要求文件描述符，?何采取不同的方式？raw_cat只是
 *      ??地?入一??据，然后?出，用read和write系??用即可。而cook_cat需要一?一?地?入字符并分
 *      析?容，read和write系??用用于?理??字符，效率很低，由于C?准?IO?有??机制，故此?使用?
 *      函?能??得更好的性能。
 *      最后?要注意?准?入和其它文件必????待，不能???准?入。
 *---------------------------------------------------------------------------------------
 */
   170                  if (path == NULL)
   171                          break;
/*---------------------------------------------------------------------------------------
 *      ?cat?有指定?入文件?，循????里退出。
 *---------------------------------------------------------------------------------------
 */
   172                  ++i;
   173          }
   174  }
   175 
/*---------------------------------------------------------------------------------------
 *      scanfiles函?定??束。
 *---------------------------------------------------------------------------------------
 */
   176  static void
   177  cook_cat(FILE *fp)
   178  {
/*---------------------------------------------------------------------------------------
 *      cook_cat逐字符地?入文件?容，按照??分析后再?出。
 *---------------------------------------------------------------------------------------
 */
   179          int ch, gobble, line, prev;
   180 
   181          /* Reset EOF condition on stdin. */
   182          if (fp == stdin && feof(stdin))
   183                  clearerr(stdin);
   184 
/*---------------------------------------------------------------------------------------
 *      若?准?入中含有上次?留的文件?束符，?其清除。?是可能?生的，?考??种情?："cat - 1.c -"。
 *---------------------------------------------------------------------------------------
 */
   185          line = gobble = 0;
   186          for (prev = '\n'; (ch = getc(fp)) != EOF; prev = ch) {
/*---------------------------------------------------------------------------------------
 *      循??入字符。
 *---------------------------------------------------------------------------------------
 */
   187                  if (prev == '\n') {
/*---------------------------------------------------------------------------------------
 *      如果前一?字符是'\n'，?明?前字符ch是新行的??。
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
 *      若指定了-s??，?需要?多???空行???一行。如果此?ch是'\n'（和prev一?），?明?前行是空
 *      行，再考察gobble?量。若gobble?0，?明前一行不是空行，需要?出?前的空行，此??gobble置?1；
 *      若gobble?1，?之前已?出?空行，不用再?出，于是???取下一?字符。
 *---------------------------------------------------------------------------------------
 */
   196                          if (nflag && (!bflag || ch != '\n')) {
   197                                  (void)fprintf(stdout, "%6d\t", ++line);
   198                                  if (ferror(stdout))
   199                                          break;
   200                          }
/*---------------------------------------------------------------------------------------
 *      此?，由于ch是新行的??，所以如果指定了-n或-b???，???出行?。注意-b???含了-n??，在
 *      用getopt分析命令行???已?确保?bflag?1?，nflag也?1。
 *---------------------------------------------------------------------------------------
 */
   201                  }
   202                  if (ch == '\n') {
   203                          if (eflag && putchar('$') == EOF)
   204                                  break;
/*---------------------------------------------------------------------------------------
 *      遇到?行符?，若指定了-e??，??出行?束符$，注意-e???含了-v??。
 *---------------------------------------------------------------------------------------
 */
   205                  } else if (ch == '\t') {
   206                          if (tflag) {
   207                                  if (putchar('^') == EOF || putchar('I') == EOF)
   208                                          break;
   209                                  continue;
   210                          }
/*---------------------------------------------------------------------------------------
 *      遇到tab字符?，若指定了-t??，??出^I，注意-t???含了-v??。
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
 *      若指定了-v??，?按照cat手?中?明的方式?示不可打印字符和控制字符。需要特?指出，字符'\177'表
 *      示退格字符，不能直接?出，否???除前一?字符，所以?出'?'代替之。
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
 *      若?理?程中?入文件出?，?清除??，???理下一?文件；若?出文件出?，?程序?法??，此?只
 *      能退出程序。
 *---------------------------------------------------------------------------------------
 */
   236 
   237  static void
   238  raw_cat(int rfd)
   239  {
/*---------------------------------------------------------------------------------------
 *      raw_cat一次?取一整??据，再整??出，?出前不做任何?理。
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
 *      用fstat函??得文件信息，st_blksize表示???文件?最合适的?大小，根据??信息分配大小最合适的
 *      ???。注意?里malloc分配的?存并?有用free函??放，可能作者?得程序退出?自?回收空?，不?放
 *      反而更具效率吧。
 *---------------------------------------------------------------------------------------
 */
   254          while ((nr = read(rfd, buf, bsize)) > 0)
   255                  for (off = 0; nr; nr -= nw, off += nw)
   256                          if ((nw = write(wfd, buf + off, (size_t)nr)) < 0)
   257                                  err(1, "stdout");
/*---------------------------------------------------------------------------------------
 *      ?取文件?并?入?出文件，注意?里?理write的方式，cat?有假定write能?一次性完成，因?cat并不
 *      能???准?出?被重定向到哪里。如果?准?出被重定向到一?允?中?write的位置，那么?里所做的?理
 *      就是必需的。
 *---------------------------------------------------------------------------------------
 */
   258          if (nr < 0) {
   259                  warn("%s", filename);
   260                  rval = 1;
   261          }
   262  }
/*---------------------------------------------------------------------------------------
 *      如果?生??，?打印警告信息，程序???理。
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
 *      udom_open函?使用了unix域套接字，??方面我?一?所知，深感?愧。
 *      ?便提一句，代?前面的行?正是使用cat加上的。
 *---------------------------------------------------------------------------------------
 */