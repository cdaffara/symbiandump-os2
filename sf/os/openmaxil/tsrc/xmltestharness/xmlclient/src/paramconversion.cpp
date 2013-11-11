/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
*/

#include "paramconversion.h"
#include <uri8.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

OMX_PARAM_CONTENTURITYPE* ParamConversion::FilenameAsContentUriStructV1_0L(const TDesC& aFileName)
	{
	CUri8* aFileUri = CUri8::CreateFileUriL(aFileName);
	CleanupStack::PushL(aFileUri);
	const TDesC8& aUriDes = aFileUri->Uri().UriDes();
	if(aUriDes.Length() > 255)
		{
		User::Leave(KErrTooBig);
		}
	OMX_PARAM_CONTENTURITYPE* aUriStruct = reinterpret_cast<OMX_PARAM_CONTENTURITYPE*>(new(ELeave) TUint8[264]);
	aUriStruct->nSize = 264;
	aUriStruct->nVersion.s.nVersionMajor = 1;
	aUriStruct->nVersion.s.nVersionMinor = 0;
	aUriStruct->nVersion.s.nRevision = 0;
	aUriStruct->nVersion.s.nStep = 0;
	TPtr8 aUriStructDes(aUriStruct->contentURI, aUriDes.Length()+1);
	aUriStructDes = aUriDes;
	aUriStructDes.Append('\0');
	CleanupStack::PopAndDestroy(aFileUri);
	return aUriStruct;
	}

OMX_PARAM_CONTENTURITYPE* ParamConversion::FilenameAsContentUriStructV1_1L(const TDesC& aFileName)
	{
	CUri8* aFileUri = CUri8::CreateFileUriL(aFileName);
	CleanupStack::PushL(aFileUri);
	const TDesC8& aUriDes = aFileUri->Uri().UriDes();
	// sizeof(OMX_PARAM_CONTENTURITYPE) includes 1 byte for the URI name. counting this as well leaves room for a null terminator.
	TInt aStructSize = 9 + aUriDes.Length();
	TUint8* aUriBytes = new(ELeave) TUint8[aStructSize];
	OMX_PARAM_CONTENTURITYPE* aUriStruct = reinterpret_cast<OMX_PARAM_CONTENTURITYPE*>(aUriBytes);
	aUriStruct->nSize = aStructSize;
	aUriStruct->nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
	aUriStruct->nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
	aUriStruct->nVersion.s.nRevision = OMX_VERSION_REVISION;
	aUriStruct->nVersion.s.nStep = OMX_VERSION_STEP;
	TPtr8 aUriStructDes(aUriStruct->contentURI, aUriDes.Length()+1);
	aUriStructDes = aUriDes;
	aUriStructDes.Append('\0');
	CleanupStack::PopAndDestroy(aFileUri);
	return aUriStruct;
	}
