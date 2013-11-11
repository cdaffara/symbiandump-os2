/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_CONVS1_H__
#define __T_CONVS1_H__

#include <test/testexecutestepbase.h>

class CT_CONVS1 : public CTestStep
	{
public:
	CT_CONVS1();
protected:
	TVerdict doTestStepL();
private:
	template <class T> void testStoreRestoreL(T& aCopy,const T& aOriginal);
	TInt IsEqual(const CPlainText* aCopy,const CPlainText* aOriginal);
	TInt DocsEqual(const CGlobalText* aCopy,const CGlobalText* aOrig);
	void GenerateGlobalTextL();
	void setupCleanup();
	void DeleteDataFile(const TDesC& aFullName);
	};

_LIT(KTestStep_T_CONVS1, "T_CONVS1");

#endif
