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
*
*/




/**
 @file
*/

#include "utils.h"
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>

void printBN(BIGNUM* bn)
    {
    char* text = BN_bn2hex(bn);
    printf("%s", text);
    OPENSSL_free(text);
    }

void printBin(char* data, int len)
    {
    BIGNUM* bn = BN_new();
    bn = BN_bin2bn(data, len, bn);
    printBN(bn);
    BN_free(bn);
    }

static int vectorNumber = 1;

/**
 * Print the first few lines of the action block.
 */

void printActionHeader(char* name, char* type)
    {
    printf("<action>\n");
    printf("\t<actionname>%s %i (%s)</actionname>\n", name, vectorNumber++, type);
    printf("\t<actiontype>%s</actiontype>\n", type);
    printf("\t<actionbody>\n");
    } 

/**
 * Print the last few lines of the action block.
 */

void printActionFooter(BOOL passes)
    {
    printf("\t</actionbody>\n");
    printf("\t<actionresult>\n");
    printf("\t\t<result>%s</result>\n", passes ? "ETrue" : "EFalse");
    printf("\t</actionresult>\n");
    printf("</action>\n");
    }

/**
 * Print an element containg hex data.
 */

void printHexElement(char* name, unsigned char* data, int len)
    {
    printf("\t\t<%s>", name);
    printBin(data, len);
    printf("</%s>\n", name);
    }

/**
 * Print an element containg hex data.
 */

void printBNElement(char* name, BIGNUM* num)
    {
    printf("\t\t<%s>", name);
    printBN(num);
    printf("</%s>\n", name);
    }

/**
 * Scramble some data - used for generating tests that we expect to fail.
 */

void scramble(unsigned char* data, int len)
    {
    int i;
    for (i = 0 ; i < len ; ++ i)
        data[i] ^= i;
    }

/**
 * Print an openssl error and exit.
 */

void processError()
    {
    unsigned long err = ERR_get_error();
    ERR_load_crypto_strings();
    printf("Openssl error: %s\n", ERR_error_string(err, NULL));
    exit(1);
    }

////////////////////////////////////////////////////////////////////////////////
// Random stuff
////////////////////////////////////////////////////////////////////////////////

int random_value = 1;

void random_seed(const void* buf, int num)
    {
    }

int random_bytes(unsigned char *buf, int num)
    {
    int i;
    for (i=0; i<num; i++)
			buf[i]=random_value++;

    return(1);
    }

void random_cleanup(void)
    {
    }

void random_add(const void *buf, int num, double entropy)
    {
    }

int random_pseudorand(unsigned char *buf, int num)
    {
    return 1;
    }

int random_status(void)
    {
    return 1;
    }

RAND_METHOD ourRNG = {random_seed, random_bytes, random_cleanup, random_add, random_pseudorand, random_status};

void testOurRandom()
    {
    char data[16];
    int i;

    RAND_bytes(data, 16);

    for (i = 0 ; i < 16 ; ++i)
        {
        if (data[i] != i + 1)
            {
            printf("Random number generator not crippled\n");
            exit(1);
            }
        }
    }

void setOurRandom()
    {
    random_value = 1;
    RAND_set_rand_method(&ourRNG);
    }

/** Print C source for assinging binary data to a variable. */
void printCBin(char* varname, unsigned char* data, int len)
    {
    int i, j;

    printf("\tunsigned char %s[] =", varname);
    for (i = 0 ; i < len ; i += 16)
        {
        printf("\n\t\t\"");
        for (j = i ; j < len && j < (i + 16) ; ++j)
            {
            printf("\\x%02x", data[j]);
            }
        printf("\"");
        }

    printf(";\n\n");

    printf("\tint %s_len = %i;\n\n", varname, len);
    }

/** Print C source for assigning the binary form of a BIGNUM to a variable. */
void printCBN(char* varname, BIGNUM* bignum)
    {
    int len = BN_num_bytes(bignum);
    unsigned char buffer[len];

    BN_bn2bin(bignum, buffer);
    printCBin(varname, buffer, len);
    }
