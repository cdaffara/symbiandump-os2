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

#ifndef __T_CONVS_H__
#define __T_CONVS_H__

#include <test/testexecutestepbase.h>

class CT_CONVS : public CTestStep
	{
public:
	CT_CONVS();
protected:
	TVerdict doTestStepL();
private:
	template <class T> void testCopy(T &aCopy,const T &anOriginal);
	template <class T> void testStoreRestoreL(T& aCopy,const T& aOriginal);
	template <class T> void testCopyChain(T &aCopy,const T &anOriginal,TInt aExcludeCount,const CFormatLayer* aBase);
	TInt IsEqual(const CPlainText* aCopy,const CPlainText* aOriginal);
	void testPlainTextL(CEditableText::TDocumentStorage aStorage);
	void testGlobalTextL(CEditableText::TDocumentStorage aStorage);
	TInt LayerIsEqual(const CParaFormatLayer* aRestored,const CParaFormatLayer* aOriginal);
	TInt LayerIsEqual(const CCharFormatLayer* aRestored,const CCharFormatLayer* aOriginal);
	void testFmtLayerStoreL();
	TInt ChainIsEqual(const CParaFormatLayer* aCopy,const CParaFormatLayer* aOriginal);
	TInt ChainIsEqual(const CCharFormatLayer* aCopy,const CCharFormatLayer* aOriginal);
	void DoParaChainL();
	void DoCharChainL();
	void DoCharChainVariant1();
	void testFmtLayerStoreChainL();
	void testFmtLayerL();
	void setupCleanup();
	void DeleteDataFile(const TDesC& aFullName);
	};

_LIT(KTestStep_T_CONVS, "T_CONVS");

#endif
