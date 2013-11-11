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

#ifndef __T_RICHOOM_H__
#define __T_RICHOOM_H__

#include <test/testexecutestepbase.h>
#include <txtrich.h>

class CT_RICHOOM : public CTestStep
	{
public:
	CT_RICHOOM();
protected:
	TVerdict doTestStepL();
private:
	CRichText* LoadIntoTextL(TFileName& aFileName);
	void ConstructOOM();
	void CopyPasteOOM();
	void CopyPasteOOM2();
	void WriteInlineL(RWriteStream& aStream,CRichText* aRichText);
	void ReadInlineL(RReadStream& aStream,CRichText* aRichText);
	CStyleList* CreatePopulatedStyleList();
	void SaveLoadOOM();
	void CheckTextIntegrity(const CRichText* aText);
	void BasicEditOOM();
	void AppendTest1L();
	void AppendTest2L(CRichText* aTarget);
	void AppendTestNewL(CRichText* aTarget);
	void AppendTest3L(CRichText* aTarget);
	void AppendTakingSolePictureOwnershipOOM();
	void DoAppendParagraphOOM(CRichText* aRichText,TBool aUnderOOM,TInt aReplicas);
	void AppendParagraphOOM();
	void ApplyCharFormatOOM1();
	void ApplyCharFormatOOM2();
	void doMainL();
	void setupCleanup();
	void DeleteDataFile(const TDesC& aFullName);
	};

_LIT(KTestStep_T_RICHOOM, "T_RICHOOM");

#endif
