// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 


#include "dummyswi.h"
#include <e32base.h>
#include <eikenv.h>
#include <e32property.h>
#include <sacls.h>
#include <bacline.h>	//CCommandLineArguments

// This exe is used to simulate the Software Installer behaviour of setting and resetting
// the P&S KSAUidSoftwareInstallKeyValue property.

LOCAL_C void DoStartL() 
	{
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	TInt i = args->Count();

	if ( args->Count() == 2 )
		{
		TLex tlex(args->Arg(1));
		TInt argVal = 0;
		if ( tlex.Val(argVal) == KErrNone )
			{
			RProperty propertyHndl;
			User::LeaveIfError(propertyHndl.Attach(KUidSystemCategory, KSAUidSoftwareInstallKeyValue));
			TInt initialVal = 0;

			// Get initial value
			TInt err = propertyHndl.Get(initialVal);
			
			// 1 => Successful installation 
			// 4 => Successful uninstallation 
			if(argVal == 1 || argVal == 4 )
				{
				// Imitate the start of SWI
				TInt val = initialVal;
				if(argVal == 1)
					{
					val |= ESASwisInstall;
					}
				else
					{
					val |= ESASwisUninstall;
					}
				err = propertyHndl.Set(val);

				// Wait for the test case to finish and for the P&S value to be set
				User::After(1200000);
				
				// Imitate the successful completion of SWI
				val |= ESASwisStatusSuccess;
				err = propertyHndl.Set(val);

				// wait for RProperty::Set() to complete
				User::After(100000);
				}
			// 2 => Aborted installation
			// 3 => Hanging installation
			// 5 => Aborted uninstallation
			// 6 =? Hanging uninstallation
			else if(argVal == 2 || argVal == 3 || argVal == 5 || argVal == 6)
				{
				// Imitate the start of SWI
				TInt val = initialVal;
				if( argVal == 2 || argVal == 3)
					{
					val |= ESASwisInstall;
					}
				else
					{
					val |= ESASwisUninstall;
					}
				err = propertyHndl.Set(val);

				// Wait for the test case to finish
				if( argVal == 3 || argVal == 6 )
					{
					User::After(16500000);
					}
				else
					{
					User::After(1200000);
					}

				// Imitate the successful completion of SWI
				val |= ESASwisStatusAborted;
				err = propertyHndl.Set(val);

				// wait for RProperty::Set() to complete
				User::After(100000);
				}
			// Reset initial value for P&S install property
			err = propertyHndl.Set(initialVal);
			propertyHndl.Close();
			}
		}
	CleanupStack::PopAndDestroy(); //args
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD( err, DoStartL() );
	delete cleanup;
    __UHEAP_MARKEND;
    return err;
	}
