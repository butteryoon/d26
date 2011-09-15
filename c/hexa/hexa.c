#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>


#define FPRINTF     fprintf
#define LOG_LEVEL   stdout

//#define FPRINTF	dAppLog
//#define LOG_LEVEL	LOG_CRI

//#define iscntrl(c) __isctype((c), _IScntrl)
#define WIDTH   16
int nifo_dump_DebugString(char *debug_str, char *s, int len)
{
    char buf[BUFSIZ],lbuf[BUFSIZ],rbuf[BUFSIZ];
    unsigned char *p;
    int line,i;

    FPRINTF(LOG_LEVEL,"### %s\n",debug_str);
    p =(unsigned char *) s;
    for(line = 1; len > 0; len -= WIDTH,line++) {
        memset(lbuf,0,BUFSIZ);
        memset(rbuf,0,BUFSIZ);

        for(i = 0; i < WIDTH && len > i; i++,p++) {
            sprintf(buf,"%02x ",(unsigned char) *p);
            strcat(lbuf,buf);
            sprintf(buf,"%c",(!iscntrl(*p) && *p <= 0x7f) ? *p : '.');
            strcat(rbuf,buf);
        }
        FPRINTF(LOG_LEVEL,"%04x: %-*s    %s\n",line - 1,WIDTH * 3,lbuf,rbuf);
    }
    return line;
}

int main()
{
    int k = 0xFFFFFFFF;
    char    aaa[BUFSIZ] = "abcdefskdfsa";

    nifo_dump_DebugString("TEST 1", (char *)&k, 4);
    nifo_dump_DebugString("TEST 2", aaa, strlen(aaa));
    return 0;
}

