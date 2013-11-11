/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A wrapper for bio.h in openssl for Symbian.
*
*/


/* A wrapper for bio.h in openssl */

#ifndef __SYMBIANCRYPTO_BIO_H__
#define __SYMBIANCRYPTO_BIO_H__    


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include "xmlsecc_config.h"

typedef struct ScBIOSt
{
	char*					mem;
	unsigned int			len;
	char*					name;
} BIO; 


BIO* BIO_new_file(const char *filename, const char *mode, const char *name);

BIO* BIO_new_buffer(const char *aData, unsigned int aSize, const char *aName);

void BIO_free(BIO *bio);

BIO* BIO_new();

int BIO_write(BIO *bio, const unsigned char *buf, unsigned int size);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYMBIANCRYPTO_BIO_H__ */

#define __SYMBIANCRYPTO_BIO_H__    
