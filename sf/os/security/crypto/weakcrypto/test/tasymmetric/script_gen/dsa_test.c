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
* Generates DSA test vectors.
*
*/




/**
 @file
*/

#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/dsa.h>
#include "utils.h"

/*
 * The DSA keys have been choosen from randomly generated ones so that either
 * the 2nd or 3rd most significant bit of q is set - this is so that a
 * particular path is followed when the signing procedure generates k, for which
 * our function that generates a random number in a certain range is equivalent
 * to the openssl one.
 *
 * This is nasty, but it means we don't have to change the openssl code.
 */

static DSA* createDSAKey1()
	{
	unsigned char p_data[] =
		"\x8d\xf2\xa4\x94\x49\x22\x76\xaa\x3d\x25\x75\x9b\xb0\x68\x69\xcb"
		"\xea\xc0\xd8\x3a\xfb\x8d\x0c\xf7\xcb\xb8\x32\x4f\x0d\x78\x82\xe5"
		"\xd0\x76\x2f\xc5\xb7\x21\x0e\xaf\xc2\xe9\xad\xac\x32\xab\x7a\xac"
		"\x49\x69\x3d\xfb\xf8\x37\x24\xc2\xec\x07\x36\xee\x31\xc8\x02\x91";

	int p_data_len = 64;

	unsigned char q_data[] =
		"\xc7\x73\x21\x8c\x73\x7e\xc8\xee\x99\x3b\x4f\x2d\xed\x30\xf4\x8e"
		"\xda\xce\x91\x5f";

	int q_data_len = 20;

	unsigned char g_data[] =
		"\x62\x6d\x02\x78\x39\xea\x0a\x13\x41\x31\x63\xa5\x5b\x4c\xb5\x00"
		"\x29\x9d\x55\x22\x95\x6c\xef\xcb\x3b\xff\x10\xf3\x99\xce\x2c\x2e"
		"\x71\xcb\x9d\xe5\xfa\x24\xba\xbf\x58\xe5\xb7\x95\x21\x92\x5c\x9c"
		"\xc4\x2e\x9f\x6f\x46\x4b\x08\x8c\xc5\x72\xaf\x53\xe6\xd7\x88\x02";

	int g_data_len = 64;

	unsigned char priv_key_data[] =
		"\xc2\x5f\xb4\x81\xd7\x9c\x07\x30\x97\xc2\x1d\x0c\xb1\xd7\x07\x2f"
		"\x04\x1d\xbb\x89";

	int priv_key_data_len = 20;

	unsigned char pub_key_data[] =
		"\x2d\x52\x1e\x27\x37\xe7\xdb\xdb\xd1\xe4\x7a\xb6\x79\xc5\xae\x69"
		"\xe1\x25\x37\xe9\x96\xb1\x81\xba\xf0\xb8\x85\x0f\x84\xc8\x2b\x9e"
		"\xf5\xb8\xb6\xc2\xb2\x22\x1b\xb5\xa9\xd0\x58\x66\x32\x5a\xb1\x62"
		"\xae\xb7\x13\x68\xbd\xe4\xca\xa0\xf6\x68\x96\x3c\x7d\x1a\x43\x52";

	int pub_key_data_len = 64;

	DSA* key = DSA_new();
	key->p = BN_new();
	key->q = BN_new();
	key->g = BN_new();
	key->priv_key = BN_new();
	key->pub_key = BN_new();

	BN_bin2bn(p_data, p_data_len, key->p);
	BN_bin2bn(q_data, q_data_len, key->q);
	BN_bin2bn(g_data, g_data_len, key->g);
	BN_bin2bn(pub_key_data, pub_key_data_len, key->pub_key);
	BN_bin2bn(priv_key_data, priv_key_data_len, key->priv_key);

	return key;
	}

static DSA* createDSAKey2()
	{
	unsigned char p_data[] =
		"\x8d\xf2\xa4\x94\x49\x22\x76\xaa\x3d\x25\x75\x9b\xb0\x68\x69\xcb"
		"\xea\xc0\xd8\x3a\xfb\x8d\x0c\xf7\xcb\xb8\x32\x4f\x0d\x78\x82\xe5"
		"\xd0\x76\x2f\xc5\xb7\x21\x0e\xaf\xc2\xe9\xad\xac\x32\xab\x7a\xac"
		"\x49\x69\x3d\xfb\xf8\x37\x24\xc2\xec\x07\x36\xee\x31\xc8\x02\x91";

	int p_data_len = 64;

	unsigned char q_data[] =
		"\xc7\x73\x21\x8c\x73\x7e\xc8\xee\x99\x3b\x4f\x2d\xed\x30\xf4\x8e"
		"\xda\xce\x91\x5f";

	int q_data_len = 20;

	unsigned char g_data[] =
		"\x62\x6d\x02\x78\x39\xea\x0a\x13\x41\x31\x63\xa5\x5b\x4c\xb5\x00"
		"\x29\x9d\x55\x22\x95\x6c\xef\xcb\x3b\xff\x10\xf3\x99\xce\x2c\x2e"
		"\x71\xcb\x9d\xe5\xfa\x24\xba\xbf\x58\xe5\xb7\x95\x21\x92\x5c\x9c"
		"\xc4\x2e\x9f\x6f\x46\x4b\x08\x8c\xc5\x72\xaf\x53\xe6\xd7\x88\x02";

	int g_data_len = 64;

	unsigned char priv_key_data[] =
		"\x0f\x36\x53\xf7\x51\xa7\x04\x6e\x0b\x52\x30\xfe\x15\x7c\xa3\x3e"
		"\x03\xf0\x84\x73";

	int priv_key_data_len = 20;

	unsigned char pub_key_data[] =
		"\x7f\x9a\xd6\x6d\x4d\xa9\xb0\x0e\x71\x76\x04\xc4\xdb\x3b\x96\x93"
		"\x6b\x6c\xa6\x16\xa5\x6b\xb6\xe6\x23\x26\xc7\xf5\xd4\xd6\x5b\x06"
		"\x6c\x10\x47\x0a\xc2\xf2\x1d\xc1\x7b\x39\x54\x6d\x84\x99\x40\xd1"
		"\x7e\xb0\xb5\x17\xc1\x17\xcd\xa0\x78\x0c\xc1\x67\xf1\x57\x83\x2c";

	int pub_key_data_len = 64;

	DSA* key = DSA_new();
	key->p = BN_new();
	key->q = BN_new();
	key->g = BN_new();
	key->priv_key = BN_new();
	key->pub_key = BN_new();

	BN_bin2bn(p_data, p_data_len, key->p);
	BN_bin2bn(q_data, q_data_len, key->q);
	BN_bin2bn(g_data, g_data_len, key->g);
	BN_bin2bn(pub_key_data, pub_key_data_len, key->pub_key);
	BN_bin2bn(priv_key_data, priv_key_data_len, key->priv_key);

	return key;
	}

static DSA* createDSAKey3()
	{
	unsigned char p_data[] =
		"\xcf\x62\x62\xfb\x7e\x7d\xc9\xc3\x7b\x4b\xa3\x62\x02\x1c\xfd\xb0"
		"\xcc\xd7\xbe\x39\xba\x89\xfc\x86\xf4\x9b\x29\xee\xe8\xee\x47\x23"
		"\xa8\x36\xd5\xbc\x3f\x31\xfe\x75\xc9\xcc\xc2\xa4\x49\x1d\x44\xc2"
		"\xb2\x7a\x71\x45\xd8\xcd\x84\x41\xe5\x8a\x94\xaa\x02\x0f\xd9\x03";

	int p_data_len = 64;

	unsigned char q_data[] =
		"\xfe\xf0\xae\xe8\x2a\x1c\x4b\xdb\xbb\xd7\xdf\x3b\xfa\xc9\x63\xfb"
		"\x6d\xfc\x1e\xe7";

	int q_data_len = 20;

	unsigned char g_data[] =
		"\x3b\x48\xa9\x2d\x82\xde\x8c\x21\x81\x09\xf8\x78\xae\xfc\x6a\xf5"
		"\x0e\x2a\x90\x3d\xfb\x64\x90\x6f\x0d\xbc\xfd\x67\x4e\x2c\x56\x00"
		"\xa3\x80\x25\x8a\x00\x26\x62\x10\xdc\xb6\xaa\x11\xa6\x36\x8c\x19"
		"\xb9\x2a\xcc\x00\xca\x84\xd5\xeb\x8f\x9b\x50\x08\x2d\x56\x4b\x37";

	int g_data_len = 64;

	unsigned char priv_key_data[] =
		"\x8f\xf4\xb0\xe8\x16\x94\xcb\x45\xaf\x26\xfa\x3b\x52\x96\xb3\xe9"
		"\xc9\x41\xce\x18";

	int priv_key_data_len = 20;

	unsigned char pub_key_data[] =
		"\x4e\x31\xab\x0e\x9b\xd5\x16\xaf\xf5\xc7\xf8\xe3\x1d\x01\x20\x90"
		"\x3e\x11\x11\x5f\xf2\x22\x46\x5f\x08\xbb\x9e\x36\x82\xdc\xfb\x33"
		"\xf9\xaa\xa5\xe9\xa0\x12\x16\x65\xc4\xbc\x7c\x6a\xf8\xb3\xc8\x8c"
		"\x2e\x45\xcc\x00\xaa\x7b\x4d\xae\xae\x9a\x1d\x37\x75\xb2\xe6\x86";

	int pub_key_data_len = 64;

	DSA* key = DSA_new();
	key->p = BN_new();
	key->q = BN_new();
	key->g = BN_new();
	key->priv_key = BN_new();
	key->pub_key = BN_new();

	BN_bin2bn(p_data, p_data_len, key->p);
	BN_bin2bn(q_data, q_data_len, key->q);
	BN_bin2bn(g_data, g_data_len, key->g);
	BN_bin2bn(pub_key_data, pub_key_data_len, key->pub_key);
	BN_bin2bn(priv_key_data, priv_key_data_len, key->priv_key);

	return key;
	}

static int digest_len = 20;

static unsigned char digest1[]="12345678901234567890";

static unsigned char digest2[] =
        "\xb1\xcd\x52\x66\x6d\x22\x60\x16\xd3\x75\x8e\x96\xa1\xff\xfa\xbc"
        "\x1b\x9a\xe2\xd7";

static unsigned char digest3[] =
        "\x39\x86\x53\xec\xae\xf6\xd7\x7a\x8a\x67\x8a\x74\x20\x30\x3b\xc9"
        "\xda\xc9\xf3\xa7";
/**
 * Print DSA public key.
 */

void printDSAPublicKey(DSA* key)
    {    
    printBNElement("keyP", key->p);
    printBNElement("keyQ", key->q);
    printBNElement("keyG", key->g);
    printBNElement("keyY", key->pub_key);
    }

/**
 * Print DSA private key.
 */

void printDSAPrivateKey(DSA* key)
    {    
    printBNElement("keyP", key->p);
    printBNElement("keyQ", key->q);
    printBNElement("keyG", key->g);
    printBNElement("keyX", key->priv_key);
    }

/**
 * Sign a message - DSA deals with generating a digest for us.
 */

static void generateSignatureVector(DSA* key, unsigned char* mess, int len, BOOL passes)
    {
    unsigned char buf[len];
	unsigned int siglen;
    int err;
    DSA_SIG* sig;
 
	Mem::Copy(buf, mess, len);

    setOurRandom();
    sig = DSA_do_sign(buf, len, key);
    if (sig == NULL)
        processError();

    if (!passes)
        scramble(buf, len);

    printActionHeader("DSA test vector", "DSASignVector");
    printDSAPrivateKey(key);
    printHexElement("digest", buf, len);
    printBNElement("sigR", sig->r);
    printBNElement("sigS", sig->s);
    printActionFooter(passes);

    printActionHeader("DSA test vector", "DSAVerifyVector");
    printDSAPublicKey(key);
    printHexElement("digest", buf, len);
    printBNElement("sigR", sig->r);
    printBNElement("sigS", sig->s);
    printActionFooter(passes);

    DSA_SIG_free(sig);
    }


int main(int argc, char **argv)
	{
	DSA* key1 = createDSAKey1();
	DSA* key2 = createDSAKey2();
	DSA* key3 = createDSAKey3();

    setOurRandom();
    testOurRandom();

    generateSignatureVector(key1, digest1, digest_len, TRUE);
    generateSignatureVector(key2, digest1, digest_len, TRUE);
    generateSignatureVector(key3, digest1, digest_len, TRUE);

    generateSignatureVector(key1, digest2, digest_len, TRUE);
    generateSignatureVector(key2, digest2, digest_len, TRUE);
    generateSignatureVector(key3, digest2, digest_len, TRUE);

    generateSignatureVector(key1, digest3, digest_len, TRUE);
    generateSignatureVector(key2, digest3, digest_len, TRUE);
    generateSignatureVector(key3, digest3, digest_len, TRUE);

    generateSignatureVector(key1, digest1, digest_len, FALSE);
    generateSignatureVector(key2, digest1, digest_len, FALSE);
    generateSignatureVector(key3, digest1, digest_len, FALSE);

	return 0;
	}
 
