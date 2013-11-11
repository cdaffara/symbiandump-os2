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

#ifndef __T_TRTCUSTM_H__
#define __T_TRTCUSTM_H__

#include <test/testexecutestepbase.h>

class CT_TRTCUSTM : public CTestStep
	{
public:
	CT_TRTCUSTM();
protected:
	TVerdict doTestStepL();
private:
	void WriteInlineL(RWriteStream& aStream,CRichText* aRichText);
	void ReadInlineL(RReadStream& aStream,CRichText* aRichText);
	TStreamId PerformSaveL(CRichText* aRichText);
	CRichText* PerformLoadL(TStreamId aId);
	CStyleList* CreatePopulatedStyleList();
	void CustomLoadSave();
	void TestDEF101757();
	void TestINC054540();
	void doMainL();
	void setupCleanup();
	void DeleteDataFile(const TDesC& aFullName);
	};

_LIT(KTestStep_T_TRTCUSTM, "TRTCUSTM");

#endif
