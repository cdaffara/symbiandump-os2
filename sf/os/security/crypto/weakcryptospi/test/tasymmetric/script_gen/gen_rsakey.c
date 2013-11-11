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
* Generate an RSA key.
*
*/




/**
 @file
*/

#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include "utils.h"

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

static void printRSAKey(RSA* key)
    {
    printf("static RSA* createRSAKey()\n");
    printf("\t{\n");

    printCBN("n_data", key->n);
    printCBN("e_data", key->e);
    printCBN("d_data", key->d);

    printf("\tRSA* key = RSA_new();\n");
    printf("\tkey->n = BN_new();\n");
    printf("\tkey->e = BN_new();\n");
    printf("\tkey->d = BN_new();\n");

    printf("\tBN_bin2bn(n_data, n_data_len, key->n);\n");
    printf("\tBN_bin2bn(e_data, e_data_len, key->e);\n");
    printf("\tBN_bin2bn(d_data, d_data_len, key->d);\n");
     
    printf("\treturn key;\n");

    printf("\t}\n");
    }

static const char rnd_seed[] = "string to make the random number generator think it has entropy";

static void badUsage()
    {
    printf("usage: gen_rsakey\n");
    exit(1);
    }

int main(int argc, char **argv)
	{
	RSA *rsa;
	int modulus_size = 1024;
	int exponent = 65537;	

    if (argc > 1)
        badUsage();
    
	RAND_seed(rnd_seed, sizeof rnd_seed);

	rsa = RSA_generate_key(modulus_size, exponent, NULL, NULL);

    printRSAKey(rsa);

	return 0;
	}
