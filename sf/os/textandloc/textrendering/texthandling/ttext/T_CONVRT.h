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

#ifndef __T_CONVRT_H__
#define __T_CONVRT_H__

#include <test/testexecutestepbase.h>

class CT_CONVRT : public CTestStep
	{
public:
	CT_CONVRT();
protected:
	TVerdict doTestStepL();
private:
	template <class T> void testStoreRestoreL(T& aCopy,const T& aOriginal);
	TInt IsEqual(const CEditableText* aCopy,const CEditableText* aOriginal);
	void DoTestRichTextL();
	void LoadIntoText(TFileName& aFileName);
	void KillText();
	void KillLayers();
	void Reset();
	void GoL();
	void setupCleanup();
	void DeleteDataFile(const TDesC& aFullName);
	};

_LIT(KTestStep_T_CONVRT, "T_CONVRT");

#endif
