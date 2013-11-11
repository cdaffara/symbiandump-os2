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
* Generate a DSA key.
*
*/




/**
 @file
*/

#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/dsa.h>
#include <openssl/bn.h>
#include "utils.h"

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

static void printDSAKey(DSA* key)
    {
    printf("static DSA* createDSAKey()\n");
    printf("\t{\n");

    printCBN("p_data", key->p);
    printCBN("q_data", key->q);
    printCBN("g_data", key->g);
    printCBN("priv_key_data", key->priv_key);
    printCBN("pub_key_data", key->pub_key);

    printf("\tDSA* key = DSA_new();\n");
    printf("\tkey->p = BN_new();\n");
    printf("\tkey->q = BN_new();\n");
    printf("\tkey->g = BN_new();\n");
    printf("\tkey->priv_key = BN_new();\n");
    printf("\tkey->pub_key = BN_new();\n\n");

    printf("\tBN_bin2bn(p_data, p_data_len, key->p);\n");
    printf("\tBN_bin2bn(q_data, q_data_len, key->q);\n");
    printf("\tBN_bin2bn(g_data, g_data_len, key->g);\n");
    printf("\tBN_bin2bn(pub_key_data, pub_key_data_len, key->pub_key);\n");
    printf("\tBN_bin2bn(priv_key_data, priv_key_data_len, key->priv_key);\n\n");
     
    printf("\treturn key;\n");

    printf("\t}\n");
    }

/*
 * This is the seed used in the openssl test code.  Using it (by
 * specifying the -use_seed option) makes this program generate the
 * same key used in the openssl test code
 *
 * It comes from the updated Appendix 5 to FIPS PUB 186.
 */

static unsigned char seed[20]={
	0xd5,0x01,0x4e,0x4b,0x60,0xef,0x2b,0xa8,0xb6,0x21,0x1b,0x40,
	0x62,0xba,0x32,0x24,0xe0,0x42,0x7d,0xd3,
	};

static const char rnd_seed[] = "string to make the random number generator think it has entropy";

static void badUsage()
    {
    printf("usage: gen_dsakey [ -use_seed ]\n");
    exit(1);
    }

int main(int argc, char **argv)
	{
	DSA *dsa=NULL;
	int counter,ret=0,i,j;
	unsigned char buf[256];
	unsigned long h;
	unsigned char sig[256];
	unsigned int siglen;
    BOOL useSeed = FALSE;

    if (argc > 2)
        badUsage();
    else if (argc == 2)
        {
        if (strcmp(argv[1], "-use_seed") != 0)
            badUsage();
        useSeed = TRUE;
        }
    
	RAND_seed(rnd_seed, sizeof rnd_seed);

	dsa=DSA_generate_parameters(512,useSeed ? seed : NULL,20,&counter,&h,NULL,NULL);

	DSA_generate_key(dsa);

    printDSAKey(dsa);

	return 0;
	}
