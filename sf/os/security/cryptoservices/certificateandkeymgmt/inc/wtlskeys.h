/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
 
#if !defined (__WTLSKEYS_H__)
#define __WTLSKEYS_H__

#include <e32base.h>
#include <e32std.h>
#include <asymmetric.h>

class CWTLSRSAPublicKey : public CRSAPublicKey
	{
public:
	IMPORT_C static CWTLSRSAPublicKey* NewL(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSRSAPublicKey* NewLC(const TDesC8& aBinaryData);
	IMPORT_C static CWTLSRSAPublicKey* NewL(const TDesC8& aBinaryData, TInt& aPos);
	IMPORT_C static CWTLSRSAPublicKey* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CWTLSRSAPublicKey();
	};

#endif
