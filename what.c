/*==============================================================================
| what
|   coded by Graham THE Ollis <ollisg@ns.arizona.edu>
|
|   Copyright (C) 1995, 1998 Graham THE Ollis
|   Copyright (c) 2015, Graham Ollis
|
| All rights reserved.
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions are met:
|
| * Redistributions of source code must retain the above copyright notice, this
|   list of conditions and the following disclaimer.
|
| * Redistributions in binary form must reproduce the above copyright notice,
|   this list of conditions and the following disclaimer in the documentation
|   and/or other materials provided with the distribution.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
| AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
| IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
| DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
| FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
| DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
| SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
| OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
| OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
|  Date       Name	Revision
|  ---------  --------  --------
|  16 jul 95  G. Ollis	created
|  13 aug 98  G. Ollis	added correct handling of directories under linux
|  07 sep 98  G. Ollis	added documentation, made release ready.
|  29 apr 15  G. Ollis  relicensed under BSD
|=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

#define VERSION_NUMBER     "1.2"
#define TRUE                   1
#define FALSE                  0
#define BUFFER_SIZE         4096
#define LINE_LEN            1000

static const char *ID_STRING = "@(#)GUTIL version 1.2";
static const char *ID_STRING2 = "@[#]GUTIL version 1.2 alternate id";
static const char *empty = "";
char *code = "@(#)";

typedef unsigned int WORD;
typedef unsigned char BYTE;
typedef int BOOLEAN;

/*====== globals =============================================================*/

int stop_after = -1;
int num_found = 0;

/*====== prototypes ==========================================================*/

void helpmsg(void);
void searchfile(char *);
int isdirectory(char *);

/*====== M A I N =============================================================*/

int
main(int argc,char *argv[])
{
  int i, n, next_loop=0;

  for(i=1;i<argc;i+=1+next_loop) {
    next_loop = 0;
    if(argv[i][0]=='-') {
      if(argv[i][1]=='-') {
        if(!strcmp(&argv[i][2], "version"))
          printf("what version %s\n", VERSION_NUMBER);
        else if(!strcmp(&argv[i][2], "help")) 
          helpmsg();
        else {
          fprintf(stderr, "unknown long switch %s\n", argv[i]);
        }
      } else {
        for(n=1;argv[i][n];n++)
          switch(argv[i][n]) {
            case 'h' : case 'H' : case '?' :
              helpmsg();
              break;

            case 's' :
              stop_after = 1;
              break;

            case 'n' :
              stop_after = atoi(argv[i+1]);
              argv[i+1][0] = 0;
              break;

            case 'p' :
              code = argv[i+1];
              fprintf(stderr, "pattern is: \'%s\'\n", code);
              next_loop++;
              break;

            default :
              fputs("warning: option \'",stderr);
              fputc(argv[i][n],stderr);
              fputs("\' unknown\n",stderr);
              break;
        }
      }
    } else if(argv[i][0] == 0) {
    } else {
      searchfile(argv[i]);
    }
  }

  return num_found;
}

/*====== functions ===========================================================*/

void
searchfile(char *s)
{
  FILE *fp;
  char buff[BUFFER_SIZE];
  int i,n;
  char c;
  WORD br;
  char msg[LINE_LEN];

  fputs(s,stdout);
  puts(":");

  if(isdirectory(s))
    return;

  if((fp=fopen(s,"rb"))==NULL) {
    fputs("warning: error opening ",stderr);
    fputs(s,stderr);
    fputs(" for read\n",stderr);
    return;
  }

  while(!feof(fp)) {
    br = fread(buff,1,BUFFER_SIZE,fp);
    for(i=0;i<br && stop_after != 0;i++)
      if(!memcmp(&buff[i],code,4)) {
        stop_after --;
        i+=4;
        n=0;
        while(i<br && buff[i] != '\"'
                   && buff[i] != '>'
                   && buff[i] != '\0'
                   && buff[i] != '\n'
                   && buff[i] != '\r'
                   && buff[i] != '\\' && n<LINE_LEN)
          msg[n++]=buff[i++];

        if(i==br) {
          while((c=fgetc(fp))!=EOF && c != '\"'
                                   && c != '>'
                                   && c != '\0'
                                   && c != '\n'
                                   && c != '\r'
                                   && c != '\\' && n<LINE_LEN);
          msg[n++]=c;
        }

        msg[n]='\0';

        putchar('\t');
        puts(msg);
      }

  }

  if(stop_after == 0) {
    exit(num_found);
  }

  fclose(fp);
}

/*----------------------------------------------------------------------------*/

int
isdirectory(char *filename)
{
  struct stat buf;

  if(lstat(filename, &buf) == -1) {
    fprintf(stderr, "warning: cannot stat() %s\n", filename);
    return 1;
  } 

  if(buf.st_mode & S_IFDIR)
    return 1;

  return 0;
}

/*----------------------------------------------------------------------------*/

void
helpmsg(void)
{
  puts(" GUTIL program WHAT by Graham Ollis.");
  fputs(" version ",stdout);
  puts(VERSION_NUMBER);
  puts(empty);
  puts("  WHAT <options> <filenames>");
  puts(empty);
  puts("WHAT searches for SCCS version info from a file");
  puts(empty);
  puts("options available:");
  puts("-s   Stop after the first occurance of the pattern");
  puts("-n   Specify the number of occurances to stop after");
  puts("-p   Specify a different pattern");
  exit(0);
}
