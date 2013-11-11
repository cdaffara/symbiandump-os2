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


#include <e32base.h>
#include "FSResources.h"
#include <fstokenserver.rsg>

_LIT(KResourceFile, "Z:\\RESOURCE\\FSTOKENSERVER.RSC");

CDesCArray* FSResources::iStrings;

/// If the resources aren't initialised, initialise them
void FSResources::InitialiseL()
	{
	if (iStrings != NULL)
		return;

	RFs fs;
	User::LeaveIfError(fs.Connect());

	TFileName fileName;
	fileName.Copy(KResourceFile);

	CleanupClosePushL(fs);
	BaflUtils::NearestLanguageFile(fs,fileName);
	if (!BaflUtils::FileExists(fs,fileName))
		{
		User::Leave(KErrNotFound);
		}
	
	RResourceFile resourceFile;
	resourceFile.OpenL(fs,fileName);
	CleanupClosePushL(resourceFile);
	resourceFile.ConfirmSignatureL(0);
	resourceFile.Offset();
	TResourceReader reader;
	HBufC8* resource = resourceFile.AllocReadLC(R_FSSERVER_STRINGS);
	reader.SetBuffer(resource);

	iStrings = reader.ReadDesCArrayL();


	CleanupStack::PopAndDestroy(3); // Close reader, file and fs	
	}

/// Free all resources.
void FSResources::Cleanup()
	{
	delete iStrings;
	iStrings = NULL;
	}

/// Returns an array of strings. The strings are identified by
/// enums in TStrings
const RCPointerArray<HBufC>& Strings();

