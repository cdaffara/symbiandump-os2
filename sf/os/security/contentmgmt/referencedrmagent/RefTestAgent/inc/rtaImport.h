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


/**
 @file
 @internalAll
 @released
*/

#ifndef __RTAIMPORT_H__
#define __RTAIMPORT_H__


#include <caf/caf.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include "rtaArchive.h"

_LIT(KRtaTempOutFileName, "tmpout");
 
class RFile;

namespace ReferenceTestAgent
	{
	class CDrmRights;
	class CDrmRightsParser;
	
	/**
 	Defines the types of mime types supported by the reference test agent.
 	This types are needed to separate the post acquisition cases from the
 	regular imports.
	 */
	enum TImportFileMimeType
		{
		EMimeContentOrRights=0,
		EMimePostAcquisitionRights,
		EMimePostAcquisitionContent
		};

	class CRefTestAgentImport : public CBase
		{
	public:
		IMPORT_C static CRefTestAgentImport* NewL(const TDesC& aClientSuggestedFileName, RFs& aFs, const TImportFileMimeType& aMimeType);
		virtual ~CRefTestAgentImport();

		// obtain the mime type of the content in the file
		IMPORT_C void WriteL(const TDesC8& aData);
		IMPORT_C void ContinueWithNewOutputFileL(RFile& aFile, const TDesC& aFileName);
		IMPORT_C TInt WriteComplete();
		IMPORT_C TInt GetSuggestedOutputFileExtension(TDes& aFileExtension);
		IMPORT_C TInt GetSuggestedOutputFileName(TDes& aFileName);
		IMPORT_C void SetMetaDataL(const ContentAccess::CMetaDataArray& aMetaDataArray);
		IMPORT_C void GetCurrentFileNameAndBuffer(TDes& aFileName, TDes8& aBuffer);
		// process the rights received during the import and save them in the rights server
		void ProcessRightsL(RFile& aFile);
		
	private:
		CRefTestAgentImport(RFs& aFs, const TImportFileMimeType& aMimeType);
		void ConstructL(const TDesC& aClientSuggestedFileName);
	
		void WriteHeaderWithNoRightsL();
		void WriteCompleteL();
		void GetSuggestedOutputFileNameL(TDes& aFileName);
		void OpenTempOutFileL();
		void OutputContentL(RFile& aFile);
		void WriteIntL(TInt aLen);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		void WriteInt64L(TInt64 aLen);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		void WriteDescriptorL(const TDesC8& aBuffer);
		void WriteFileDataL(const TDesC8& aData);
		void WriteStreamDataL(const TDesC8& aData);
		void ReadBuffer(TDes8& aBuffer);
		
	private:		
		RFs&  iFs;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64 iFile;
#else
		RFile iFile;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TBool iOutputFileOpened;

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TInt64 iBytesReceived;
		
		TInt64 iRightsOffset;
		TPckg <TInt64> iRightsOffsetPckg;

		TInt64 iDrmFilesOffset;
		TPckg <TInt64> iDrmFilesOffsetPckg;
#else
		TInt iBytesReceived;
		
		TInt iRightsOffset;
		TPckg <TInt> iRightsOffsetPckg;

		TInt iDrmFilesOffset;
		TPckg <TInt> iDrmFilesOffsetPckg;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		TInt iRightsSize;
		TPckg <TInt> iRightsSizePckg;

		TInt iDrmFilesSize;
		TPckg <TInt> iDrmFilesSizePckg;

		HBufC *iTempOutFileName;
	
		enum TImportStage 
			{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API 
			EReceivingHeaderMark,
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
			EReceivingRightsOffset,
			EReceivingDrmFilesOffset,
			EReceivingContent,
			EReceivingDrmFilesSize,
			EReceivingDrmFiles,
			EReceivingRightsSize,
			EReceivingRights,
			EFinished
			} iImportStage;		

		CDrmRightsParser* iDrmRightsParser;

		/** A client (JMI) requested that the DeleteFile API doubles
		 as API to delete rights objects. Clients will pass the 
		 suggested output filename at rights import time to specify
		 the rights to delete.
		 */
		HBufC8 *iClientSuggestedFileName;
		TImportFileMimeType iMimeType;
		HBufC8* iRightObjectCid; ///< Content ID of the rights object. Sent as meta-data.
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TInt64 iDataOffset; 		 ///< Saves the offset of drm data.
#else
		TInt iDataOffset; 		 ///< Saves the offset of drm data.
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TInt	iOffsetSize;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TBuf8<KRTAArchiveHeaderSize> iHeaderBuff;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		HBufC* iRtaTempOutDir; /* default is "c:\\rta\\" */
		};
	}

#endif

