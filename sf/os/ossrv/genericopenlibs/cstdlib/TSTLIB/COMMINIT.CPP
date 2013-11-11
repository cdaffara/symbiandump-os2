// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Initialise Comm Server in an simple text shell environment
// 
//

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <e32hal.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#define LDD_FNAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#define LDD_FNAME _L("FCOMM")
#endif

extern "C" {

EXPORT_C int CommInit(int aEnhanced)
	{
	RFs fs;
	TInt err=fs.Connect();	// make sure the FileServer is alive (only needed for WINS test code)
	fs.Close();

	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		return(err);
	err=User::LoadLogicalDevice( aEnhanced?LDD_FNAME:LDD_NAME );
	if (err!=KErrNone && err!=KErrAlreadyExists)
		return(err);
	return KErrNone;
	}

}
