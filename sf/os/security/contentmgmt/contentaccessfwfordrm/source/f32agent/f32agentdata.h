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
 @internalComponent
 @released
*/

#ifndef __F32AGENTDATA_H__
#define __F32AGENTDATA_H__

#include <f32file.h>
#include <caf/agentinterface.h>

namespace ContentAccess
	{
	class TVirtualPathPtr;
	class CF32AgentUi;

	/**
	 F32 agent implementation of the CAgentData class used to read
	 plaintext files
	 
	 @internalComponent
	 @released
	 */
	class CF32AgentData : public CAgentData
		{
	public:
		static CF32AgentData* NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		static CF32AgentData* NewL(RFile& aFile, const TDesC& aUniqueId);
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		static CF32AgentData* NewL(const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		~CF32AgentData();

	public: // From CAgentData
		virtual TInt Read(TDes8& aDes);
		virtual TInt Read(TDes8& aDes,TInt aLength);
		virtual void Read(TDes8& aDes, TRequestStatus& aStatus);
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
		virtual void ReadCancel(TRequestStatus& aStatus);
		virtual void DataSizeL(TInt& aSize);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual void DataSize64L(TInt64& aSize);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual TInt Seek(TSeek aMode,TInt& aPos);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual TInt Seek64(TSeek aMode,TInt64& aPos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue);
		virtual TInt EvaluateIntent(TIntent aIntent);
		virtual TInt ExecuteIntent(TIntent aIntent);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue);
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet);
		virtual TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API		
		virtual TInt Read64(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		virtual TInt Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket);		
		virtual void Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket, TRequestStatus& aStatus);
#endif	//SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	private:
		CF32AgentData();
		void ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		void ConstructL(RFile& aFile, const TDesC& aUniqueId);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		void ConstructL(const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		CF32AgentUi& AgentUiL();
		
	private:
		/** Handle to the filesystem */
		RFs iFs;
		
		/** RFile object */
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64 iFile;
#else
		RFile   iFile;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API 

		CVirtualPath* iVirtualPath;
		CF32AgentUi* iAgentUi;

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		HBufC8* iHeaderData;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		};
} // namespace ContentAccess
#endif // __F32AGENTDATA_H__
