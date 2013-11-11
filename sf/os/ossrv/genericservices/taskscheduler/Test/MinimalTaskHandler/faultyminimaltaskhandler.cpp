// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <schtask.h>
#include <s32file.h>


//***********************************************************************************
LOCAL_D TInt Execute()
	{
	TInt err = KErrNoMemory;

	RFile file;
			
	// Adopt the task file from the Task Scheduler
	err = file.AdoptFromCreator(TScheduledTaskFile::FsHandleIndex(),
								TScheduledTaskFile::FileHandleIndex());
	
	//Panic the client to test server robustness							
	User::Panic(_L("Test Panic"),0);
	
	file.Close();
	return err;

	}


//***********************************************************************************
GLDEF_C TInt E32Main()
	{
	return Execute();
	}
