/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __CDCINTERFACEBASE_H__
#define __CDCINTERFACEBASE_H__

#include <e32base.h>
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif

NONSHARABLE_CLASS(CCdcInterfaceBase) : public CBase
/**
 * Base class for ACM interface subclasses.
 */
	{
public:
	~CCdcInterfaceBase();

public:
	TInt GetInterfaceNumber(TUint8& aIntNumber);

protected:
	CCdcInterfaceBase(const TDesC16& aIfcName);
	void BaseConstructL();

private:
	virtual TInt SetUpInterface() = 0;

protected: // owned
	RDevUsbcClient iLdd;			///< The Ldd to use.
	TPtrC16 iIfcName;
	};

#endif // __CDCINTERFACEBASE_H__
