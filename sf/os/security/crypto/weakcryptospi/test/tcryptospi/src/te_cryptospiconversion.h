/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#if (!defined __TE_CRYPTOSPI_CONVERSION_H__)
#define __TE_CRYPTOSPI_CONVERSION_H__

#include "te_cryptospistepbase.h"
#include <cryptospi/romlit.h>

class TVariantPtrC : public TPtrC
	{
	public:

	operator TUid();
	operator TInt();
	operator TRomLitC16() ;
	operator const TInt32*() ;
	
	HBufC* HexStringLC();
	
	};
	
#endif
