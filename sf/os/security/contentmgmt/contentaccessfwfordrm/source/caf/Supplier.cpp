/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <caf/caftypes.h>
#include <caf/metadataarray.h>
#include <caf/supplier.h>
#include "resolver.h"
#include <caf/importfile.h>
#include "agentinfo.h"
#include <caf/agentinterface.h>

#ifndef REMOVE_CAF1
#include <caf/cafmimeheader.h>
#include <apmstd.h> 
#endif // REMOVE_CAF1

using namespace ContentAccess;

EXPORT_C CSupplier* CSupplier::NewLC()
	{
	CSupplier* self = new (ELeave) CSupplier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSupplier* CSupplier::NewL()
	{
	CSupplier* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSupplier::CSupplier()
	{
	}

CSupplier::~CSupplier()
	{
	delete iAgentResolver;
	delete iOutputDirectory;
	}

void CSupplier::ConstructL()
	{
	iAgentResolver = CAgentResolver::NewL(EFalse);
	iOutputDirectory=HBufC::NewL(0);
	}

EXPORT_C TBool CSupplier::IsImportSupported(const TDesC8& aMimeType)
	{
	TBuf8 <KMaxDataTypeLength> mimeLowerCase;
	if(aMimeType.Length() > KMaxDataTypeLength)
		{
		return EFalse;
		}

	// Convert supplied mime type to lower case
	mimeLowerCase.Copy(aMimeType);
	mimeLowerCase.LowerCase();

	// Search for the mime type in the agents
	for(TInt i = 0; i < iAgentResolver->SupplierMimeTypes().Count(); i++)
		{
		if(mimeLowerCase.Compare(iAgentResolver->SupplierMimeTypes()[i]) == 0)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_C void CSupplier::PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const
	{
	// Ignore errors from any particular agent because we can't
	// let one agent ruin things for everyone else.
	TInt i = 0;
	TInt err = KErrNone;
	CAgentManager* manager = NULL;
	for(i = 0; i < iAgentResolver->AgentInfoCount(); i++)
		{
		TRAP(err, manager = &iAgentResolver->AgentInfo(i).AgentManagerL());
		if(err == KErrNone)
			{
			manager->PrepareHTTPRequestHeaders(aStringPool, aRequestHeaders);
			}	
		}
	}


EXPORT_C void CSupplier::SetOutputDirectoryL(const TDesC &aOutputDirectory)
	{
	HBufC *newOutputDirectory;

	// Make sure the path has a backslash at the end
	TInt pos = aOutputDirectory.LocateReverse(TChar(KPathDelimiter));
	if(pos + 1 != aOutputDirectory.Length())
		{
		// Append a trailing backslash
		newOutputDirectory=HBufC::NewL(aOutputDirectory.Length()+1);
		newOutputDirectory->Des().Append(aOutputDirectory);
		newOutputDirectory->Des().Append(TChar(KPathDelimiter));
		}
	else
		{
		newOutputDirectory=aOutputDirectory.AllocL();
		}

	// replace the existing output path
	delete iOutputDirectory;
	iOutputDirectory = newOutputDirectory;
	}

EXPORT_C CImportFile* CSupplier::ImportFileL(const TDesC8& aMimeType, const CMetaDataArray& aImportMetaData, const TDesC& aSuggestedFileName)
	{
	CAgentInfo& agentInfo = iAgentResolver->ResolveSupplierMimeL(aMimeType);

	TUid agentUid = agentInfo.Agent().ImplementationUid();
	
	// Create import object, agent creates output files
	CImportFile* import = CImportFile::NewL (agentUid, aMimeType, aImportMetaData, *iOutputDirectory, aSuggestedFileName);
	return import;
	}

EXPORT_C CImportFile* CSupplier::ImportFileL(const TDesC8& aMimeType, const CMetaDataArray& aImportMetaData)
	{
	// Create import object, client provides output files
	CAgentInfo& agentInfo = iAgentResolver->ResolveSupplierMimeL(aMimeType);
	
	TUid agentUid = agentInfo.Agent().ImplementationUid();
	
	// Create import object, agent creates output files
	CImportFile* import = CImportFile::NewL (agentUid, aMimeType, aImportMetaData);
	return import;
	}

#ifndef REMOVE_CAF1
EXPORT_C CImportFile *  CSupplier::ImportFileL (CCafMimeHeader &aMimeHeader, const TDesC &aSuggestedFileName)
	{
	TBuf8 <KMaxDataTypeLength> mimeType;

	// string constants used to fill in CMetaDataArray
	_LIT8(KContentType, "Content-Type");
	_LIT8(KContentLength, "Content-Length");
	_LIT8(KContentId, "Content-Id");
	_LIT8(KContentTransferEncoding, "Content-Tranfer-Encoding");
	_LIT8(KBoundary, "Content-Boundary");

	// create new CMetaDataArray to hold the information previously supplied in CCafMimeHeader
	CMetaDataArray *array = CMetaDataArray::NewLC();

	// Copy "standard mime data" to the CMetaDataArray
	if(aMimeHeader.StandardMimeData(EContentType).Length() != 0)
		{
		array->AddL(KContentType(),aMimeHeader.StandardMimeData(EContentType));
		mimeType.Append(aMimeHeader.StandardMimeData(EContentType));
		}
	if(aMimeHeader.StandardMimeData(EContentLength).Length() != 0)
		{
		array->AddL(KContentLength(),aMimeHeader.StandardMimeData(EContentLength));
		}
	if(aMimeHeader.StandardMimeData(EContentLength).Length() != 0)
		{
		array->AddL(KContentLength(),aMimeHeader.StandardMimeData(EContentLength));
		}
	if(aMimeHeader.StandardMimeData(EContentId).Length() != 0)
		{
		array->AddL(KContentId(),aMimeHeader.StandardMimeData(EContentId));
		}
	if(aMimeHeader.StandardMimeData(EContentTransferEncoding).Length() != 0)
		{
		array->AddL(KContentTransferEncoding(),aMimeHeader.StandardMimeData(EContentTransferEncoding));
		}
	if(aMimeHeader.StandardMimeData(EBoundary).Length() != 0)
		{
		array->AddL(KBoundary(),aMimeHeader.StandardMimeData(EBoundary));
		}

	// Add "non-standard" mime fields to CMetaDataArray
	TInt count = aMimeHeader.NonStandardMimeCount();
	for( TInt i = 0; i < count; i++)
		{
		array->AddL(aMimeHeader.NonStandardMimeField(i), aMimeHeader.NonStandardMimeData(i));
		}

	// Create new CImportFile object
	CImportFile *importFile = ImportFileL(mimeType, *array, aSuggestedFileName);
	CleanupStack::PopAndDestroy(array);
	return importFile;
	}

#endif // REMOVE_CAF1


