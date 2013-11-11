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
// tef_efm.iby
// 
//

#include "helping_exe.h"
#include <e32base.h>
#include <eikenv.h>
#include <featmgr/featurecontrol.h>
#include <featmgr/featmgr.h>
#include "efm_test_consts.h"
#include <bacline.h>	//CCommandLineArguments

// This exe is used to add a new feature to FeatMgr whilst the SWI is running and its launched
// exe has called SWIStart and has added some features at the same time as this exe.

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
			// 
			if(argVal == 1)
				{
				RFeatureControl control;
				TInt err = control.Connect();

				TBitFlags32 flags( 0 );
				flags.Set( EFeatureSupported);
				flags.Set( EFeatureModifiable );

				// Adding features
				TFeatureEntry entry1( KNewUid1, flags, KDefaultData1 );
				err = control.AddFeature(entry1);

				control.Close();
				}
			else if( argVal == 2)
				{
				RFeatureControl control;
				TInt err = control.Connect();

				err = control.SWIEnd();

				control.Close();
				}
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
