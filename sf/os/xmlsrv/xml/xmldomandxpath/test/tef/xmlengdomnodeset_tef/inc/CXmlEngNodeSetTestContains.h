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
 @file CXmlEngNodeSetTestContains.h
 @internalTechnology
*/
#ifndef CXMLENGNODESETTESTCONTAINS_H_
#define CXMLENGNODESETTESTCONTAINS_H_

#include <test/testexecutestepbase.h>
#include <xml/dom/xmlengdomparser.h>
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengerrors.h>
#include <xml/dom/xmlengxpathresult.h>


//Names of the sub steps of the test case
_LIT(KXmlEngNodeTestSetContains, "XmlEngNodeTestSetContains");



class CXmlEngNodeSetTestContains : public CTestStep
{
public:
	//The constructor is passed the step name 
	CXmlEngNodeSetTestContains(const TDesC& aStepName);

	//This method is impelemented and from here various sub steps of the tests are invoked
	virtual TVerdict doTestStepL(void);
	
	//These are the methods that are the sub steps for the testing
	TVerdict TestKXmlEngNodeSetContains();
	
private:
    // DOM implementation for dom tests
	RXmlEngDOMImplementation iDOM_impl;

	// Dom parser
	RXmlEngDOMParser iParser;
	
	
	// XML document ( dom tree )
	RXmlEngDocument iDoc;
	
	// XML node ( dom tree )
	//TXmlEngNode iNode;
	
	// XML element
	//TXmlEngElement iElement;
	
	// XPath result
	RXmlEngXPathResult iResult;
	
};

#endif /*CXMLENGNODESETTESTCONTAINS_H_*/
