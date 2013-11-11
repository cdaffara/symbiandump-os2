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
* testagentconsumer.h
* Test agent implementation of the CAgentConsumer class
*
*/




#ifndef __TESTAGENTDATA_H__
#define __TESTAGENTDATA_H__

#include <caf/caf.h>
#include "agentinterface.h"
#include "TestAgentFileInterface.h"

class CTestAgentData : public ContentAccess::CAgentData
		{
	public:
		static CTestAgentData* NewL(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);
		static CTestAgentData* NewLC(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);

		static CTestAgentData* NewL(RFile& aFile, const TDesC& aUniqueId);
		static CTestAgentData* NewLC(RFile& aFile, const TDesC& aUniqueId);

		virtual ~CTestAgentData();

	public: // From CAgentConsumer
		virtual TInt Read(TDes8& aDes);
		virtual TInt Read(TDes8& aDes,TInt aLength);
		virtual void Read(TDes8& aDes, TRequestStatus& aStatus);
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
		virtual void ReadCancel(TRequestStatus& aStatus);
		virtual void DataSizeL(TInt& aSize);
		virtual TInt Seek(TSeek aMode,TInt& aPos);
		virtual TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
		virtual TInt EvaluateIntent(ContentAccess::TIntent aIntent);
		virtual TInt ExecuteIntent(ContentAccess::TIntent aIntent);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue);
		virtual TInt GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		virtual TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet);
		virtual TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual void DataSize64L(TInt64 &aSize);
		virtual TInt Seek64(TSeek aMode, TInt64& aPos);
		virtual TInt Read64(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	private:
		CTestAgentData();
		void ConstructL(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);
		void ConstructL(RFile& aFile, const TDesC& aUniqueId);
		
	private:
		/* Calculate Data Size and Data offset */
		void CalculateDataSizeAndOffsetL();

		/* Format of a TUintvar is defined by the Open Mobile alliance in 
		*   Wireless Session Protocol OMA-WAP-WSP-v1_0-20020920-C
		*   @param aBuffer [in] buffer containing a TUintvar
		*   @param Offset [in] location of the first byte of the TUintvar within the buffer
		*   @param aNumBytes [out] number of bytes read from the buffer to decode the the TUintvar
		*   @return the value of the TUnitvar
		*/  
		TInt ReadTUintvarL(TDes8& aBuffer, TInt Offset, TInt& aNumBytes) const;

		// encapsulates and parses a DCF file
		CTestAgentFileInterface *iTestFileObject;

		// File session used when checking attributes
		RFs iFs;
		// File handle
		RFile iFile;

		ContentAccess::CVirtualPath *iVirtualPath;
		};

#endif // __TESTAGENTCONSUMER_H__
