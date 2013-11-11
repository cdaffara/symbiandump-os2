/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#ifndef _UTILS_H__
#define _UTILS_H__

#include <math.h>

#define LEN 15

int comparefloats(double symbian_op, double linux_op)
    {
    double fractpart1=0, intpart1=0, fractpart2=0, intpart2=0;
    int ret = 0;
    char str_int1[LEN+1] = "\0";
    char str_fract1[LEN+1+2] = "\0";
    char str_fract2[LEN+1+2] = "\0";
    char format_str[10] = "\0";
    int len1=0, len_to_comp=0;
    
    if((symbian_op < 0) && (linux_op < 0))
        {
        symbian_op = symbian_op * -1;
        linux_op = linux_op * -1;
        }
    
    fractpart1 = modf(symbian_op, &intpart1);
    fractpart2 = modf(linux_op, &intpart2);
    
    if(intpart1 == intpart2)
        {
        sprintf(str_int1, "%d", (int)intpart1);
        len1 = strlen(str_int1);
        
        len_to_comp = LEN - len1;
        sprintf(format_str, "%s%d%s", "%0.", len_to_comp, "f");
    
        sprintf(str_fract1, format_str, fractpart1);
        sprintf(str_fract2, format_str, fractpart2);
        
        if(strcmp(str_fract1, str_fract2) != 0)
            ret = 1;
        }
    else
        ret = 1;
    
    return ret;
    }

#endif //_UTILS_H__

