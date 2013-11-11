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


#include "apmstd.h"
#include <caf/importfile.h>
#include "agentinfo.h"
#include <caf/agentinterface.h>
#include <caf/agentfactory.h>
#include <caf/caferr.h>

using namespace ContentAccess;

CImportFile* CImportFile::NewL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aMetaData, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
	{
	CImportFile* self = new (ELeave) CImportFile();
	CleanupStack::PushL(self);
	self->ConstructL(aAgentUid, aMimeType, aMetaData, aOutputDirectory, aSuggestedFileName);
	CleanupStack::Pop(self);
	return self;
	}

void CImportFile::ConstructL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aMetaData, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName)
	{
	// increment the reference count for the agent, so ECOM does not unload the agent
	iAgentFactory = CAgentFactory::NewL(aAgentUid);

	// Create a CAgentSupplier instance for the import
	iAgentImportFile = iAgentFactory->CreateImporterL(aMimeType, aMetaData, aOutputDirectory, aSuggestedFileName);
	}

CImportFile* CImportFile::NewL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aMetaData)
	{
	CImportFile* self = new (ELeave) CImportFile();
	CleanupStack::PushL(self);
	self->ConstructL(aAgentUid, aMimeType, aMetaData);
	CleanupStack::Pop(self);
	return self;
	}

void CImportFile::ConstructL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aMetaData)
	{
	// increment the reference count for the agent, so ECOM does not unload the agent
	iAgentFactory = CAgentFactory::NewL(aAgentUid);

	// Create a CAgentSupplier instance for the import
	iAgentImportFile = iAgentFactory->CreateImporterL(aMimeType, aMetaData);
	}

CImportFile ::CImportFile()
	{
	}

CImportFile ::~CImportFile()
	{
	delete iAgentImportFile;
	
	// Finished with Agent session, may unload the agent DLL
	delete iAgentFactory;
	REComSession::FinalClose();
	}

EXPORT_C TInt CImportFile::WriteData(const TDesC8& aData)
	{
	return iAgentImportFile->WriteData(aData);
	}

EXPORT_C void CImportFile::WriteData(const TDesC8& aData, TRequestStatus &aStatus)
	{
	iAgentImportFile->WriteData(aData,aStatus);
	}

EXPORT_C TInt CImportFile::WriteDataComplete()
	{
	// Tell the agent we are finished
	return iAgentImportFile->WriteDataComplete();
	}

EXPORT_C void CImportFile::WriteDataComplete(TRequestStatus &aStatus)
	{
	// Tell the agent we are finished
	iAgentImportFile->WriteDataComplete(aStatus);
	}

EXPORT_C TInt CImportFile::OutputFileCountL() const
	{
	return iAgentImportFile->OutputFileCountL();
	}

EXPORT_C CSupplierOutputFile& CImportFile::OutputFileL(TInt aIndex) const
	{
	return iAgentImportFile->OutputFileL(aIndex);
	}

EXPORT_C TImportStatus CImportFile::GetImportStatus() const
	{
	return iAgentImportFile->GetImportStatus();	
	}

EXPORT_C TInt CImportFile::GetSuggestedOutputFileExtension(TDes& aFileExtension)
	{	
	return iAgentImportFile->GetSuggestedOutputFileExtension(aFileExtension);
	}

EXPORT_C TInt CImportFile::GetSuggestedOutputFileName(TDes& aFileName)
	{	
	return iAgentImportFile->GetSuggestedOutputFileName(aFileName);
	}

EXPORT_C TInt CImportFile::ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName)
	{
	return iAgentImportFile->ContinueWithNewOutputFile(aFile, aFileName);	
	}

EXPORT_C void CImportFile::ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName, TRequestStatus& aStatus)
	{
	iAgentImportFile->ContinueWithNewOutputFile(aFile, aFileName, aStatus);	
	}

EXPORT_C void CImportFile::Cancel()
	{
	iAgentImportFile->Cancel();
	}

EXPORT_C TBool CImportFile::ContentMimeTypeL(TDes8& aContentMime)
	{
	return iAgentImportFile->ContentMimeTypeL(aContentMime);
	}

