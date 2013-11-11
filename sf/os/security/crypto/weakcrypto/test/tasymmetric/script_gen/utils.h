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

#ifndef __utils_h
#define __utils_h

#include <openssl/bn.h>

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

void printBN(BIGNUM* bn);
void printBin(char* data, int len);
void printActionHeader(char* name, char* type);
void printActionFooter(BOOL passes);
void printHexElement(char* name, unsigned char* data, int len);
void printBNElement(char* name, BIGNUM* num);
void scramble(unsigned char* data, int len);
void processError();
void setOurRandom();
void testOurRandom();
void printCBin(char* varname, unsigned char* data, int len);
void printCBN(char* varname, BIGNUM* bignum);

#endif
