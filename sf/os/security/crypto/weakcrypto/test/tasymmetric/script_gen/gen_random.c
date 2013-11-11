/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Generates C code that initialises a variable to hold some random data.
*
*/




/**
 @file
*/

#include <openssl/rand.h>

static void gen_random(int len)
    {
    unsigned char data[len];
    int i, j;

    RAND_bytes(data, len);

    printf("unsigned char random[] =");
    for (i = 0 ; i < sizeof(data) ; i += 16)
        {
        printf("\n\t\"");
        for (j = i ; j < sizeof(data) && j < (i + 16) ; ++j)
            {
            printf("\\x%02x", data[j]);
            }
        printf("\"");
        }

    printf(";\n");
    }

static void badUsage()
    {
    printf("gen_random [BYTES]\n");
    exit(1);
    }

int main(int argc, char* argv[])
    {
    int bytes = 256;

    if (argc > 2)
        badUsage();
    else if (argc == 2)
        {
        bytes = atoi(argv[1]);
        if (bytes < 1)
            badUsage();
        }

    gen_random(bytes);
    return 0;
    }
