/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains the list of error values that may be returned. 
*
*/


/**
 @file 
 @publishedAll
 @released
*/

#ifndef CRYPTO_SPI_ERRS_H
#define CRYPTO_SPI_ERRS_H

#include <e32def.h>


/** 
	If the key size is larger than that allowed 
	by the cipher strength restrictions of the crypto 
	library.
**/
const TInt KErrKeyNotWeakEnough				= -11002;	


/**
	If the padding on a block does not conform to the
	specifications of the algorithm.	 
**/
const TInt KErrInvalidPadding				= -11003;


/** 
	If the key is weaker than that allowed 
	by the cipher strength restrictions of the crypto 
	library.
 **/
const TInt KErrWeakKey						= -11004;


/**
	If the requested key size is not supported.
**/
const TInt KErrKeySize						= -11009;


#endif	// #ifndef CRYPTO_SPI_ERRS_H

