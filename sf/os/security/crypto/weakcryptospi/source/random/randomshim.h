/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* random shim class definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __RANDOMSHIM_H__
#define __RANDOMSHIM_H__

#include <e32def.h>
#include <random.h>
#include <cryptospi/cryptorandomapi.h>

//
// CRandom shim class
//
NONSHARABLE_CLASS(CRandomShim) : public CBase
	{
public:
	static CRandomShim* NewL();
	static CRandomShim* NewLC();
	~CRandomShim();
	virtual void GenerateBytesL(TDes8& aDest);
	
private:
	CRandomShim();
	CRandomShim(const CRandomShim&);
	CRandomShim& operator=(const CRandomShim&);	
	void ConstructL();

private:
	CryptoSpi::CRandom* iRandomImpl;
	};

class TRandomShim
	{
public:
	static void Random(TDes8& aDestination);
	static void RandomL(TDes8& aDestination);
	static void SecureRandomL(TDes8& aDestination);
	};

#endif // __RANDOMSHIM_H__
