/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ttesthandlersettings.h"
#include "t_bacline.h"

_LIT(KWaitForKeyPressAtEndSwitch, "-w");
_LIT(KOOMSwitch, "-o");
_LIT(KInteractiveSwitch, "-i");
_LIT(KCancelSwitch, "-c");
_LIT(KSkippedSwitch, "-s");
_LIT(KExhaustiveSwitch, "-e");

EXPORT_C CTestHandlerSettings* CTestHandlerSettings::NewLC()
	{
	CTestHandlerSettings* self = new(ELeave) CTestHandlerSettings();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CTestHandlerSettings* CTestHandlerSettings::NewL(const CTestHandlerSettings& aOther)
	{
	CTestHandlerSettings* self = new(ELeave) CTestHandlerSettings();
	self->iWaitForKeyPressAtEnd = aOther.iWaitForKeyPressAtEnd;
	self->iOOM = aOther.iOOM;
	self->iCancel = aOther.iCancel;
	self->iSkip = aOther.iSkip;
	self->iExhaust = aOther.iExhaust;
	self->iInt = aOther.iInt;
	return self;
	}

EXPORT_C CTestHandlerSettings::~CTestHandlerSettings()
	{
	}

CTestHandlerSettings::CTestHandlerSettings()
	{
	iWaitForKeyPressAtEnd = iOOM = iCancel = iSkip = iExhaust = iInt = EFalse;
	}

void CTestHandlerSettings::ConstructL()
	{
	CCommandLineArguments* cmdLine = CCommandLineArguments::NewL();

	TInt argTotal=cmdLine->Count();

	for (TInt loop=0 ; loop < argTotal ; ++loop)
		{
		TPtrC arg(cmdLine->Arg(loop));
		if (arg==KWaitForKeyPressAtEndSwitch)
			{
			iWaitForKeyPressAtEnd = ETrue;
			}
		else if (arg==KOOMSwitch)
			{
			iOOM = ETrue;
			}
		else if (arg==KInteractiveSwitch)
			{
			iInt = ETrue;
			}
		else if (arg==KCancelSwitch)
			{
			iCancel = ETrue;
			}
		else if (arg==KSkippedSwitch)
			{
			iSkip = ETrue;
			}
		else if (arg==KExhaustiveSwitch)
			{
			iExhaust = ETrue;
			}
		}

	delete cmdLine;
	cmdLine = NULL;
	}
