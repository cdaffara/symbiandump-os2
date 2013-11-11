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


#ifndef __TESTAGENTFILE_H__
#define __TESTAGENTFILE_H__

#include <e32base.h>
#include <f32file.h>
#include "TestAgentFileInterface.h"

class CTestAgentFile: public CTestAgentFileInterface
		{
	public:
		static CTestAgentFile* NewL(RFile& aFile);

		CTestAgentFile(RFile& aFile);
		virtual ~CTestAgentFile();

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
		RFile& iFile;
		};

#endif 
