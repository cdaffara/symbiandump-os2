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
* Generates RSA test vectors.
*
*/




/**
 @file
*/

#include <stdio.h>
#include <string.h>

#include "openssl/e_os.h"

#include <openssl/crypto.h>
#include <openssl/rsa.h>

#include "utils.h"
#include "keys.h"

void printPublicKey(RSA* key)
    {    
    printf("\t\t<modulus>");
    printBN(key->n);
    printf("</modulus>\n");
    printf("\t\t<publicExponent>");
    printBN(key->e);
    printf("</publicExponent>\n");
    }

void printPrivateKey(RSA* key)
    {    
    printf("\t\t<modulus>");
    printBN(key->n);
    printf("</modulus>\n");
    printf("\t\t<privateExponent>");
    printBN(key->d);
    printf("</privateExponent>\n");
    }

/**
 * Generate encrypt and decrypt vectors for a plaintext.
 */

static void generateEncryptionVector(RSA* key, unsigned char* ptext_ex, int plen, BOOL passes)
    {
    unsigned char ctext[RSA_size(key)];
    int num;

    setOurRandom();
	num = RSA_public_encrypt(plen, ptext_ex, ctext, key, RSA_PKCS1_PADDING);
    if (num == -1)
        processError();

    if (!passes)
        scramble(ctext, num);

    printActionHeader("RSA test vector", "RSAEncryptVector");
    printPublicKey(key);
    printHexElement("plaintext", ptext_ex, plen);
    printHexElement("ciphertext", ctext, num);
    printActionFooter(passes);

    printActionHeader("RSA test vector", "RSADecryptVector");
    printPrivateKey(key);
    printHexElement("ciphertext", ctext, num);
    printHexElement("plaintext", ptext_ex, plen);
    printActionFooter(passes);
    }

/**
 * Sign a digest - the digest is unformatted, ie we're not dealing with
 * algotrithm identifiers here.
 */

static void generateSignatureVector(RSA* key, unsigned char* ptext_ex, int plen, BOOL passes)
    {
    unsigned char ctext[RSA_size(key)];
    int num;

	num = RSA_private_encrypt(plen, ptext_ex, ctext, key, RSA_PKCS1_PADDING);
    if (num == -1)
        processError();

    if (!passes)
        scramble(ctext, num);

    printActionHeader("RSA test vector", "RSASignVector");
    printPrivateKey(key);
    printHexElement("digestInfo", ptext_ex, plen);
    printHexElement("signature", ctext, num);
    printActionFooter(passes);

    printActionHeader("RSA test vector", "RSAVerifyVector");
    printPublicKey(key);
    printHexElement("digestInfo", ptext_ex, plen);
    printHexElement("signature", ctext, num);
    printActionFooter(passes);
    }

/* Plaintext from openssl test code. */
static unsigned char ptext1[] = "\x54\x85\x9b\x34\x2c\x49\xea\x2a";
static int plen1 = sizeof(ptext1) - 1;

/* 16 byte random plaintext. */
static unsigned char ptext2[] =
        "\x47\xab\x92\x76\x09\xfd\x75\xa7\xe2\x08\x85\xeb\x7e\x4c\xff\x0a";
static int plen2 = sizeof(ptext2) - 1;

/* 32 byte random plaintext. */
static unsigned char ptext3[] =
        "\x0b\x0a\x7c\xeb\x6c\x17\x45\x53\x1d\xa7\x24\xad\x43\x8b\xf7\x46"
        "\x89\xc3\x9f\x09\x5e\x88\x3e\xd8\x8e\x04\x36\x38\x49\xc0\x0f\x41";
static int plen3 = sizeof(ptext3) - 1;

/* One byte plaintext. */
static unsigned char short_ptext[] = "\x23";
static int short_plen = sizeof(short_ptext) - 1;

/* Longest possible plaintexts, one for each key. */
static unsigned char long_ptext1[] =
        "\x66\x79\xf3\x84\x82\x06\x99\x06\xcd\xf1\xdf\x3f\xdd\xb5\x37\x74"
        "\x46\x76\xba\x0d\xb8\xd6\x82\xb6\x82\x6f\x31\xb1\xd8\x23\x0c\xca"
        "\x4e\x39\x28\x77\x05\x3f\xac\x5a\x13\xff\x3a\x39\x35\x2e\xaf\xb1"
        "\x85\xe4\xd0\x60\xf4";

static int long_plen1 = sizeof(long_ptext1) - 1;

static unsigned char long_ptext2[] =
        "\xcd\xa2\x2c\x4b\x6a\x20\x00\x0e\xad\xad\x74\xbd\xb3\x04\xbd\xc5"
        "\x72\x73\x02\x11\x9d\x6d\x37\x75\x66\x5a\xf2\xe6\x47\x65\x79\x80"
        "\x7c\x92\xec\x09\xf5\x33\xea";

static int long_plen2 = sizeof(long_ptext2) - 1;

static unsigned char long_ptext3[] =
        "\x0e\x25\x61\xaf\x55\xeb\x9c\x10\x90\x4f\xd4\x27\xfd\x0d\x1d\xf4"
        "\x38\xbd\x9e\xd0\xc7\x1c\x48\x0b\x50\xa1\xd3\xf1\xb4\xdb\xba\x2d"
        "\x00\x81\x59\x6e\x61\x43\x35\x50\xf9\x5f\x70\x20\xb2\x47\x48\x7f"
        "\x32\xf7\xe8\x2e\x50\xc1\x80\x45\x4b\x5c\xf8\x45\x6a\xa0\x0f\x33"
        "\xf1\xec\x9a\xb1\x79\xf5\xcc\x92\x1c\x30\x12\xb0\x55\x7b\x49\x06"
        "\x93\xa8\x30\x5a\x68\x79\x8a\x21\x9a\xd7\x68\x70\xf8\xa1\xf1\x0a"
        "\x52\x85\x75\xf9\x2d\x26\xd3\x1b\x37\xdc\xdc\x60\x87\x77\xcb\x97"
        "\x57\x00\x4f\xf1\x81";

static int long_plen3 = sizeof(long_ptext3) - 1;

int main(int argc, char *argv[])
    {
    initKeys();

    setOurRandom();
    testOurRandom();

    /** Public encryption: */

    /** Encrypt openssl test plaintext with each key. */
    generateEncryptionVector(key1, ptext1, plen1, TRUE);
    generateEncryptionVector(key2, ptext1, plen1, TRUE);
    generateEncryptionVector(key3, ptext1, plen1, TRUE);

    /** Encrypt 16 byte test plaintext with each key. */
    generateEncryptionVector(key1, ptext2, plen2, TRUE);
    generateEncryptionVector(key2, ptext2, plen2, TRUE);
    generateEncryptionVector(key3, ptext2, plen2, TRUE);

    /** Encrypt 32 byte test plaintext with each key. */
    generateEncryptionVector(key1, ptext3, plen3, TRUE);
    generateEncryptionVector(key2, ptext3, plen3, TRUE);
    generateEncryptionVector(key3, ptext3, plen3, TRUE);

    /** Encypt one byte plaintext with each key. */
    generateEncryptionVector(key1, short_ptext, short_plen, TRUE);
    generateEncryptionVector(key2, short_ptext, short_plen, TRUE);
    generateEncryptionVector(key3, short_ptext, short_plen, TRUE);

    /** Encrypt longest possible plaintext for each key. */
    generateEncryptionVector(key1, long_ptext1, long_plen1, TRUE);
    generateEncryptionVector(key2, long_ptext2, long_plen2, TRUE);
    generateEncryptionVector(key3, long_ptext3, long_plen3, TRUE);

    /** Negative encryption vectors. */
    generateEncryptionVector(key1, ptext1, plen1, FALSE);
    generateEncryptionVector(key2, ptext1, plen1, FALSE);
    generateEncryptionVector(key3, ptext1, plen1, FALSE);

    /** Signing: */

    /** Sign openssl test plaintext with each key. */
    generateSignatureVector(key1, ptext1, plen1, TRUE);
    generateSignatureVector(key2, ptext1, plen1, TRUE);
    generateSignatureVector(key3, ptext1, plen1, TRUE);

    /** Sign 16 byte digest with each key. */
    generateSignatureVector(key1, ptext2, plen2, TRUE);
    generateSignatureVector(key2, ptext2, plen2, TRUE);
    generateSignatureVector(key3, ptext2, plen2, TRUE);

    /** Sign 32 byte digest with each key. */
    generateSignatureVector(key1, ptext3, plen3, TRUE);
    generateSignatureVector(key2, ptext3, plen3, TRUE);
    generateSignatureVector(key3, ptext3, plen3, TRUE);

    /** Sign one byte digest with each key. */
    generateSignatureVector(key1, short_ptext, short_plen, TRUE);
    generateSignatureVector(key2, short_ptext, short_plen, TRUE);
    generateSignatureVector(key3, short_ptext, short_plen, TRUE);

    /** Sign longest possible digests for each key. */
    generateSignatureVector(key1, long_ptext1, long_plen1, TRUE);
    generateSignatureVector(key2, long_ptext2, long_plen2, TRUE);
    generateSignatureVector(key3, long_ptext3, long_plen3, TRUE);

    /** Negative signature vectors. */
    generateSignatureVector(key1, ptext1, plen1, FALSE);
    generateSignatureVector(key2, ptext1, plen1, FALSE);
    generateSignatureVector(key3, ptext1, plen1, FALSE);

    return 0;
    }
