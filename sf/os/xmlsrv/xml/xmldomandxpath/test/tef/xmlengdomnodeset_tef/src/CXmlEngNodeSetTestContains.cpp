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
 @file CXmlEngNodeSetTestContains.cpp
 @internalTechnology
*/
#include "CXmlEngNodeSetTestContains.h"


#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengnodeset.h>
#include <xml/dom/xmlengnode.h>


#include <xml/dom/xmlengxpathextensionfunction.h>
#include <xml/dom/xmlengxpathevaluationcontext.h>
#include <xml/dom/xmlengxpathconfiguration.h>
#include <xml/dom/xmlengxpathevaluator.h>
#include <xml/dom/xmlengxpathexpression.h>
#include <xml/dom/xmlengxpatherrors.h>

#include <xml/parser.h>
#include <xml/parserfeature.h>

//#include <string.h>
#include <charconv.h>
#include <utf.h>
#include <xml/dom/xmlengxpathutils.h>



/**
 * Class CXmlEngNodeSetTestContains Implementation
 */
CXmlEngNodeSetTestContains::CXmlEngNodeSetTestContains(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngNodeSetTestContains::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngNodeTestSetContains)
		{
		INFO_PRINTF1(KXmlEngNodeTestSetContains);
		SetTestStepResult(TestKXmlEngNodeSetContains());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngNodeSetTestContains::TestKXmlEngNodeSetContains()
	{
	
	//Name of the XML source file name
	//_LIT (KLitXMLSrcFileName, "c:\\testing\\data\\xmleng\\efute\\input\\dom\\xpath_02.xml");
	
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
			
	//Lets parse the XML file, and fill it to the RXMLDocument
	//TRAP(err, iDoc = iParser.ParseFileL(KLitXMLSrcFileName));
	
	_LIT8(KDataContext, "<doc>D<test>10</test><test2/></doc>");
	RXmlEngDocument contextDoc;
	
	TRAP(err, contextDoc = iParser.ParseL(TPtrC8(KDataContext)));
	if (KErrNone != err)
		{
			return EFail;
		}
		
	 // register instance doc trees
    CXmlEngXFormsInstanceMap* map = CXmlEngXFormsInstanceMap::NewLC();
	
   _LIT8(KInstA, "<data>A<test>5</test><test att=\"10\">6</test></data>");
   _LIT8(KInstB, "<data>B<test attr='345'>TestText</test><test/></data>");
   _LIT8(KInstC, "<data>C<test3>345</test3></data>");
      
   RXmlEngDocument instA;
   instA = iParser.ParseL(TPtrC8(KInstA));
   if(KErrNone != err)
	{
		return EFail;
	}
   
   RXmlEngDocument instB; 
   instB = iParser.ParseL(TPtrC8(KInstB)); 
   if(KErrNone != err)
   	{
   		return EFail;
   	}
   
   
   RXmlEngDocument instC; 
   instC = iParser.ParseL(TPtrC8(KInstC)); 
   if(KErrNone != err)
   	{
   		return EFail;
   	}
   
   
   TRAP(err, map->AddEntryL(instA, _L8("A")));
   if(KErrNone != err)
   	{
   		return EFail;
   	}
   
   TRAP(err, map->AddEntryL(instB, _L8("B")));
   if(KErrNone != err)
   	{
   		return EFail;
   	}
   
   TRAP(err, map->AddEntryL(instC, _L8("C")));
   if(KErrNone != err)
   	{
   		return EFail;
   	}

    // TC1
    TXmlEngXPathEvaluator eval;
    eval.SetInstanceMap(map);
    
    RXmlEngXPathResult result; 
    TRAP(err, result = eval.EvaluateL(_L8("instance('A')"), contextDoc, NULL));
    if(KErrNone != err)
    	{
    		return EFail;
    	} 
    
    //Get the node set
    RXmlEngNodeSet set = result.AsNodeSet();
    
    
    _LIT8(KLit1, "A");
    _LIT8(KLit2, "B");
    _LIT8(KLit3, "C");
    TBufC8<30> aLocalName(KLit1);
    TBufC8<30> aNamespaceUri(KLit2);
    TBufC8<30> aPrefix(KLit3);
    
    
    TXmlEngElement aElement; 
    TRAP(err, aElement = contextDoc.CreateElementL(aLocalName, aNamespaceUri, aPrefix));
    if(KErrNone != err)
      	{
      		return EFail;
      	} 
    	
    //Actual API call
    TBool isContained = set.Contains(aElement);
		
	contextDoc.Close();
	instA.Close();
	instB.Close();
	instC.Close();
	
    iParser.Close();
    iDOM_impl.Close();
    
    CleanupStack::PopAndDestroy(map);
    
	if (KErrNone != err)
	{
		return EFail;
	}
	
	return EPass;
	}
