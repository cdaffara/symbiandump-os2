// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @fileCXmlEngXpathCreation.cpp
 @internalTechnology
*/

#include "CXmlEngXpathCreation.h"
#include <xml/dom/xmlengxpathevaluator.h>
#include "libxml2_tree.h"



/**
 * Class CXmlEngNodeSetTestContains Implementation
 */
CXmlEngXpathCreation::CXmlEngXpathCreation(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngXpathCreation::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngXPathCreation)
		{
		INFO_PRINTF1(KXmlEngXPathCreation);
		SetTestStepResult(TestKXmlEngXPathCreation());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngXpathCreation::TestKXmlEngXPathCreation()
	{
	//Create the dom utils object
	TInt err = KErrNone; 
		
	//Open the DOM implementation
	TRAP(err, iDOM_impl.OpenL());
	
	if (KErrNone != err)
		{
			return EFail;
		}
	
	//Open the Dom Parser
	iParser.Open(iDOM_impl);
	
	_LIT8(KInstA, "<data>A<test>5</test><test att=\"10\">6</test></data>");
	_LIT8(KInstB, "<data>B<test attr='345'>TestText</test><test/></data>");
	_LIT8(KInstC, "<data>C<test3>345</test3></data>");

	RXmlEngDocument instA;
	TRAP(err, instA = iParser.ParseL(TPtrC8(KInstA))); 
	if (KErrNone != err)
		{
			iParser.Close();
		    iDOM_impl.Close();
	    
			return EFail;
		}
	
	
	RXmlEngDocument instB;
	TRAP(err, instB = iParser.ParseL(TPtrC8(KInstB)));
	if (KErrNone != err)
		{
			instA.Close();
			iParser.Close();
		    iDOM_impl.Close();
		    
			return EFail;
		}
	
	RXmlEngDocument instC;
	TRAP(err, instC = iParser.ParseL(TPtrC8(KInstC)));
	
	if (KErrNone != err)
		{
			instA.Close();
			instB.Close();
			iParser.Close();
		    iDOM_impl.Close();
		    
			return EFail;
		}

	CXmlEngXFormsInstanceMap* map = CXmlEngXFormsInstanceMap::NewLC();
	
	TRAP(err, map->AddEntryL(instA, _L8("A")));
	if (KErrNone != err)
		{
			instA.Close();
			instB.Close();
			instC.Close();
			
		    iParser.Close();
		    iDOM_impl.Close();
		    
			return EFail;
		}
	
	TRAP(err, map->AddEntryL(instB, _L8("B")));
	if (KErrNone != err)
		{
			instA.Close();
			instB.Close();
			instC.Close();
			
		    iParser.Close();
		    iDOM_impl.Close();
		    
			return EFail;
		}
	
	TRAP(err, map->AddEntryL(instC, _L8("C")));
	if (KErrNone != err)
		{
			instA.Close();
			instB.Close();
			instC.Close();
			
		    iParser.Close();
		    iDOM_impl.Close();
		    
			return EFail;
		}
	TXmlEngXPathEvaluator eval;
	eval.SetInstanceMap(map);

	//Actual API to cover
	RXmlEngXPathExpression expr;
	TRAP(err, expr = eval.CreateExpressionL( _L8("/doc/test + instance('A')/test[position()=2]/@att + instance('C')/test3"), NULL));
	if (KErrNone != err)
		{
			instA.Close();
			instB.Close();
			instC.Close();
			
		    iParser.Close();
		    iDOM_impl.Close();
		    
			return EFail;
		}
	
	//2nd Actual API to cover
	void *context = expr.ExtendedContext();
	
	//3rd Actual API to cover
	expr.SetExtendedContext(context);
	
	instA.Close();
	instB.Close();
	instC.Close();
	
    iParser.Close();
    iDOM_impl.Close();
    
    CleanupStack::PopAndDestroy(map);
    	
	return EPass;
	}
