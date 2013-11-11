// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : wsdDll2.cpp
// Part of     : wsdDll2 DLL
// Contains the code for the wsdDll2 DLL
//



#include <e32uid.h>
#include "wsdDll2.h"
#include "CommonDefs.h"
#include <pls.h>

const TUid KWsdDll2Uid3 = {0x094E40B4};

// DLL entry point
GLDEF_C TInt E32Dll()  
	{
	return(KErrNone);
	}

// WSD object type for this DLL
struct TWsdData2
{
	TInt iDll2Int;
	TBuf<10> iDll2Buf;
};

// Initialisation function for WSD objects - supplied to Pls()
LOCAL_C TInt InitializeWsd(TWsdData2* aArg)
{
	aArg->iDll2Int = KDll2InitialIntValue;
	aArg->iDll2Buf.Copy(KDll2InitialBufValue);
	return KErrNone;
}

// Function to fetch the WSD object for this process
LOCAL_C TWsdData2* GetGlobals()
{
	// Access the PLS of this process
	TWsdData2 *p = Pls<TWsdData2>(KWsdDll2Uid3, &InitializeWsd);
	return p;
}

EXPORT_C TInt GetDll2Int()
{
	TWsdData2* p = GetGlobals();	
	return p->iDll2Int;
}

EXPORT_C void GetDll2Buf(TDes& aBuf)
{
	TWsdData2* p = GetGlobals();	
	aBuf.Copy(p->iDll2Buf);
}

EXPORT_C void SetDll2Int(const TInt& aArg)
{
	TWsdData2* p = GetGlobals();	
	p->iDll2Int = aArg;
}

EXPORT_C void SetDll2Buf(TDesC& aArg)
{
	TWsdData2* p = GetGlobals();	
	p->iDll2Buf.Copy(aArg);
}

