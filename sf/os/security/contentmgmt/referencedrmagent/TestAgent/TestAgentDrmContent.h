/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TESTAGENTDRMFILE_H__
#define __TESTAGENTDRMFILE_H__

#include <e32base.h>
#include <f32file.h>
#include "TestAgentFileInterface.h"

class CTestAgentDrmContent : public CTestAgentFileInterface
		{
	public:
		static CTestAgentDrmContent* NewL(RFile& aFile);

		CTestAgentDrmContent(RFile& aFile);
		virtual ~CTestAgentDrmContent();

		virtual TInt Read(TDes8& aDes,TInt aLength);
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
		virtual void ReadCancel(TRequestStatus& aStatus);
		virtual TInt Seek(TSeek aMode,TInt& aPos);
		virtual TPtrC8 ContentMimeType();
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		virtual void DataSizeL(TInt &aSize);
		virtual void Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);	
	private:
		void ConstructL();
		TInt ReadTUintvarL(TDes8& aBuffer, TInt Offset, TInt& aNumBytes) const;

	private:
		RFile& iFile;

		/* OMA DRM Specific */
		TInt iHeaderLength;
		TInt iHeaderOffset;
		TInt iDataSize;
		TInt iDataOffset;

		HBufC8 *iContentMimeType;
		};

#endif 
