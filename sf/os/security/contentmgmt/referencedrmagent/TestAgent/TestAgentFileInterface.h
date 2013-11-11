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


#ifndef __TESTAGENTFILEINTERFACE_H__
#define __TESTAGENTFILEINTERFACE_H__

#include <e32base.h>
#include <f32file.h>

class CTestAgentFileInterface: public CBase
		{
	public:
		virtual TInt Read(TDes8& aDes,TInt aLength) = 0;
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus) = 0;
		virtual void ReadCancel(TRequestStatus& aStatus) = 0;
		virtual TInt Seek(TSeek aMode,TInt& aPos) = 0;
		virtual TPtrC8 ContentMimeType() = 0;
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue) = 0;
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue) = 0;
		virtual void DataSizeL(TInt &aSize) = 0;
		virtual void Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)=0;
		};

#endif 
