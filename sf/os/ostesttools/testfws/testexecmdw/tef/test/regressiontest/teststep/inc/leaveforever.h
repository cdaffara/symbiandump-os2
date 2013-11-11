/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 @file leaveforever.h
*/
#ifndef CLEAVEFOREVER_H_
#define CLEAVEFOREVER_H_
#include <test/testexecutestepbase.h>

_LIT(KLeaveForever,"LeaveForever");

class CLeaveForever : public CTestStep
	{
	public:
		CLeaveForever();
		virtual ~CLeaveForever();
	
		virtual TVerdict doTestStepL();
	};

#endif /*CLEAVEFOREVER_H_*/
