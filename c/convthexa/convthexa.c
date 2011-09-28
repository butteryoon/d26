
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECFROMHEXA(out, in)                                \
    {                                                       \
        switch(in)                                          \
        {                                                   \
        case '0': out = 0; break;                           \
        case '1': out = 1; break;                           \
        case '2': out = 2; break;                           \
        case '3': out = 3; break;                           \
        case '4': out = 4; break;                           \
        case '5': out = 5; break;                           \
        case '6': out = 6; break;                           \
        case '7': out = 7; break;                           \
        case '8': out = 8; break;                           \
        case '9': out = 9; break;                           \
        case 'A': out = 10; break;                          \
        case 'a': out = 10; break;                          \
        case 'B': out = 11; break;                          \
        case 'b': out = 11; break;                          \
        case 'C': out = 12; break;                          \
        case 'c': out = 12; break;                          \
        case 'D': out = 13; break;                          \
        case 'd': out = 13; break;                          \
        case 'E': out = 14; break;                          \
        case 'e': out = 14; break;                          \
        case 'F': out = 15; break;                          \
        case 'f': out = 15; break;                          \
        }                                                   \
    }

unsigned int convDecFromHexa(char *szIn, int len)
{
    unsigned int    i;
    unsigned int    tmp = 0, rst = 0;

    if(len > 8) return 0;

    for(i = 0; i < len; i++)
    {
        rst = rst << 4;
        DECFROMHEXA(tmp, szIn[i]);
        rst += tmp;
    }

    return rst;
}

void convtHexa(char *s, char *out)
{
    char    temp[4];
    int     i, len, pos = 0, flag = 0, offset = 0;

    len = strlen(s);

    for(i = 0; i < len; i++)
    {
        if(s[i] == '%')
        {
            flag = 1;
            offset = 0;
        }
        else if(flag)
        {
            temp[offset++] = s[i];

            if(offset == 2)
            {
                flag = 0;
                out[pos++] = convDecFromHexa(temp, 2);
            }
        }
        else
        {
            out[pos++] = s[i];
        }
    }

    out[pos] = 0x00;
}


int main(int argc, char *argv[])
{
	char	buf[BUFSIZ];
	char	rst[BUFSIZ];

	sprintf(buf, "www.google.com/index.jsp?test%%20code");
	convtHexa(buf, rst);
	printf("CONVT=%s\n", rst);

	return 0;
}
