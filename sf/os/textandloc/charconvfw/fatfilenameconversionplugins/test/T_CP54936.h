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


#ifndef __T_CP54936_H__
#define __T_CP54936_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_CP54936 : public CTestStep
    {
public:
	CT_CP54936();
protected:  
    TVerdict doTestStepL();
private:
    void TestOut(const TDesC16 &des);
    void TestOut(const TDesC8 &des);
	void TestL();
	void TestINC090073L();
	void OOMTestL();
	void PerformanceTest1L();
	void PerformanceTest2L();
	void TestConversionL( 
	        const TDesC16& aUnicode, 
	        const TDesC8& a54936, 
	        TBool aZero1=ETrue, 
	        TBool aZero2=ETrue );
	void TestGbConversionL();
	void TestShortNameCharacterL();
    };

_LIT(KTestStep_T_CP54936, "T_CP54936");

#endif
