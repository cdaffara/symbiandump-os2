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
// Name        : wsdDll1.cpp
// Part of     : wsdDll1 DLL
// Contains the code for the wsdDll1 DLL
//


 
#include <e32cons.h>
#include <e32uid.h>
#include "wsdDll1.h"
#include "commonDefs.h"
#include <pls.h>

const TUid KWsdDll1Uid3 = {0x0BE43AC9};

// DLL entry point
GLDEF_C TInt E32Dll()
	{
	return(KErrNone);
	}

// WSD object type for this DLL
struct TWsdData1
{
	TInt iDll1Int;
	TBuf<10> iDll1Buf;
};

// Initialisation function for WSD objects - supplied to Pls()
LOCAL_C TInt InitializeWsd(TWsdData1* aArg)
{
	aArg->iDll1Int = KDll1InitialIntValue;
	aArg->iDll1Buf.Copy(KDll1InitialBufValue);
	return KErrNone;
}

// Function to fetch the WSD object for this process
LOCAL_C TWsdData1* GetGlobals()
{
	// Access the PLS of this process
	TWsdData1* p = Pls<TWsdData1>(KWsdDll1Uid3, &InitializeWsd);
	return p;
}

EXPORT_C TInt GetDll1Int()
{
	TWsdData1* p = GetGlobals();	
	return p->iDll1Int;
}

EXPORT_C void GetDll1Buf(TDes& aBuf)
{
	TWsdData1* p = GetGlobals();	
	aBuf.Copy(p->iDll1Buf);
}

EXPORT_C void SetDll1Int(const TInt& aArg)
{
	TWsdData1* p = GetGlobals();	
	p->iDll1Int = aArg;
}

EXPORT_C void SetDll1Buf(TDesC& aArg)
{
	TWsdData1* p = GetGlobals();	
	p->iDll1Buf.Copy(aArg);
}

