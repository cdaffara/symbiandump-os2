/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the definitions of X509 certificate objects.
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __X509TIME_H__
#define __X509TIME_H__

#include <e32base.h>
#include <e32std.h>
#include <asn1dec.h>

class TASN1DecX509Time

	{
public:
	TASN1DecX509Time();			
	TTime DecodeDERL(const TDesC8& aSource,TInt& aPos);
	TTime DecodeDERL(const TASN1DecGeneric& aSource);	
	TInt EncodeDER(TDes8& aDestination,TInt& aPos);
	TTime& GetTimeL(void);
	TInt LengthDERContent(void) const;
private:
	TTime DecodeContentsL(const TASN1DecGeneric& aSource);
	};

#endif
