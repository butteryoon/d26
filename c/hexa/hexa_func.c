
#include <stdio.h>

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

unsigned int uiConvDecFromHexa(char *szIn, int len)
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

int main()
{
	char	buf[BUFSIZ] = "%3B05%3A29%20+%22PM";
	char	rst[BUFSIZ] = "";
	char 	temp[BUFSIZ];
	int		i, len, pos = 0, flag = 0, offset = 0;

	len = strlen(buf);

	for(i = 0; i < len; i++)
	{
		if(buf[i] == '%')	
		{
			flag = 1;
			offset = 0;
		}
		else if(flag)
		{
			temp[offset++] = buf[i];

			if(offset == 2)
			{
				flag = 0;
				rst[pos++] = uiConvDecFromHexa(temp, 2);
			}
		}
		else
		{
			rst[pos++] = buf[i];
		}
	}

	rst[pos] = 0x00;

	printf("STR=[%s]:[%s]\n", buf, rst);

	return 0;
}
