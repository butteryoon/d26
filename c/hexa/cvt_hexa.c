/**********************************************************
                 AVIS Project

   Author   :
   Section  :
   SCCS ID  : %W%
   Date     : %G%
   Revision History :


   Description:

   Copyright (c) ABLEX 2004
***********************************************************/

/**A.1*  File Inclusion ***********************************/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**B.1*  Definition of New Constants *********************/

/**B.2*  Definition of New Type  **************************/

/**C.1*  Declaration of Variables  ************************/

/**D.1*  Definition of Functions  *************************/

/**D.2*  Definition of Functions  *************************/

char cConv(char a, char b)
{
    char   cRst;

    if((a >= 48) && (a <= 57))
        cRst = (a - 48) << 4;
    else if((a >= 65) && (a <= 90))
        cRst = ((a - 65) + 10) << 4;
    else
        cRst = ((a - 97) + 10) << 4;

    if((b >= 48) && (b <= 57))
        cRst = cRst + (b - 48);
    else if((b >= 65) && (b <= 90))
        cRst = cRst + ((b - 65) + 10);
    else
        cRst = cRst + ((b - 97) + 10);

    return cRst;
}



int dConvFromHexaStr(char *szOut, char *szIn, int dSize)
{
    int     dLen;
    int     i, j;

	dLen = dSize / 2;

    for(i = 0; i < dLen; i++)
    {
        j = i * 2;
        szOut[i] = cConv(szIn[j], szIn[j+1]);
    }

    return dLen;
}



int dConvToHexaStr(unsigned char *szOut, unsigned char *szIn, int dLen)
{
    int             i;
    int             dPos;

    dPos = 0;

    for(i = 0; i < dLen; i++)
    {
        sprintf(&szOut[dPos], "%02X", szIn[i]);
        dPos += 2;
    }

	return dPos;
}



