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




#ifndef __TESTAGENTIMPORTFILE_H__
#define __TESTAGENTIMPORTFILE_H__

#include "agentinterface.h"

class CTestAgentImportFile : public ContentAccess::CAgentImportFile
		{
	public:
		static CTestAgentImportFile* NewL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		static CTestAgentImportFile* NewL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray);
		virtual ~CTestAgentImportFile();

	public:
		// From CAgentImportFile
		virtual TInt WriteData(const TDesC8& aData);
		virtual void WriteData(const TDesC8& aData, TRequestStatus& aStatus);
		virtual TInt WriteDataComplete();
		virtual void WriteDataComplete(TRequestStatus& aStatus);
		virtual TInt OutputFileCountL() const;
		virtual ContentAccess::CSupplierOutputFile& OutputFileL(TInt aIndex);
		virtual ContentAccess::TImportStatus GetImportStatus() const;
		virtual TInt GetSuggestedOutputFileExtension(TDes& aFileExtension);
		virtual TInt GetSuggestedOutputFileName(TDes& aFileName);
		virtual TInt ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName);
		virtual void ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName, TRequestStatus& aStatus);
		virtual TBool ContentMimeTypeL(TDes8& aContentMime); 
	private:
		CTestAgentImportFile(TBool aAgentCreatesOutputFiles);
		void ConstructL(const TDesC8& aMimeType, const ContentAccess::CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);

		TInt OpenOutputFile();
		void WriteDataCompleteL();
		void ContinueWithNewOutputFileL(RFile& aFile, const TDesC& aFileName);

	private:

		HBufC *iOutputDirectory;
		HBufC *iSuggestedFileName;
		HBufC *iOutputFileName;
		HBufC8 *iLastWriteData;
		RFs iFs;
		RFile iFile;
		RPointerArray <ContentAccess::CSupplierOutputFile> iOutputFiles;
		ContentAccess::TImportStatus iImportStatus;
		TBool iAgentCreatesOutputFiles;
		TBool iFileOpen;
		TBool iWaitingForReceiptHandle;
		};
#endif // __TESTAGENTIMPORTFILE_H__
