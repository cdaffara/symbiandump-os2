// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __T_SQLBUR_H__
#define __T_SQLBUR_H__

#include <e32test.h>
#include <f32file.h>

#include "SqlSrvBurInterface.h"

class CSqlSrvTestBurInterface : public CBase, public MSqlSrvBurInterface
	{
	public:
		static CSqlSrvTestBurInterface* New();
		virtual ~CSqlSrvTestBurInterface();
		virtual RFs& Fs();
		virtual void GetBackUpListL(TSecureId aUid, TDriveNumber aDrive, RArray<HBufC*>& aFileList);
		
	private:
		CSqlSrvTestBurInterface();
		void Construct();
		
	private:
		RFs iFs;
		
	};

#endif // __T_SQLBUR_H__
