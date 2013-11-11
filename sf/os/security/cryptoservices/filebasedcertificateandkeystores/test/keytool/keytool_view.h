/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef  __KEYTOOL_VIEW_H_
#define __KEYTOOL_VIEW_H_

#include <mctkeystore.h>
#include <cctcertinfo.h>
#include <signed.h>

typedef RArray<TUid> RUidArray;

class MKeyToolView 
	{
public:
	virtual void DisplayUsage() = 0;
	virtual void BoilerPlate() = 0;
	virtual void DisplayKeyInfoL(CCTKeyInfo& aKeyInfo, TBool aIsDetailed, TBool aPageWise) = 0;
	virtual void DisplayErrorL(const TDesC& aError, TBool aPageWise) = 0;	
	virtual void DisplayErrorL(const TDesC& aError, TInt aErrorCode, TBool aPageWise) = 0;
	virtual void DisplayCertL(CCTCertInfo& aCert, CCertificate& aCertificate, RUidArray aApps, TBool aTrusted, TBool aIsDetailed, TBool aPageWise) = 0;
	};

#endif
