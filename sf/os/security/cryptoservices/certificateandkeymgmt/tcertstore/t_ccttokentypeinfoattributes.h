/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CCTTokenTypeInfoAttributes class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_CCTTOKENTYPEINFOATTRIBUTES_H__
#define __T_CCTTOKENTYPEINFOATTRIBUTES_H__

#include "t_testhandler.h"
#include "t_certstoreactionshandle.h"

class CCTTokenTypeInfoAttributes : public CCertStoreTestAction
	{
	public:
		static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
			Output& aOut, const TTestActionSpec& aTestActionSpec);
		~CCTTokenTypeInfoAttributes();
		virtual void PerformAction(TRequestStatus& aStatus);
		virtual void PerformCancel();
		virtual void Reset();

	private:
		CCTTokenTypeInfoAttributes(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
		void ConstructL(const TTestActionSpec& aTestActionSpec);
		void DoPerformPrerequisite(TRequestStatus& aStatus);
		void DoPerformPostrequisite(TRequestStatus& aStatus);
		void DoReportAction();
		void DoCheckResult(TInt aError);

	private:
		enum TState
			{
			EFinished
			};

	private:
		TState iState;
		RCPointerArray<CCTTokenTypeInfo> iTokenTypes;
	};

#endif

