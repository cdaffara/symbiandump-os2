// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "TestContentHandler.h"
#include "XMLOutputContentHandler.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <xml/dom/xmlengxpathextensionfunction.h>
#include <xml/dom/xmlengxpathevaluationcontext.h>
#include <xml/dom/xmlengxpathconfiguration.h>
#include <xml/dom/xmlengxpathevaluator.h>
#include <xml/dom/xmlengxpathexpression.h>
#include <xml/dom/xmlengxpatherrors.h>

#include <Xml\Parser.h>
#include <Xml\ParserFeature.h>

#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>
#include <xml/dom/xmlengxpathutils.h>

using namespace Xml;

/************************************** XPATH **************************************/

// -----------------------------------------------------------------------------
// CLibxml2Tester::EvaluateExpressionL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::EvaluateExpressionL( CStifItemParser& aItem)
	{
	RXmlEngNodeList <TXmlEngElement> noLi;
	iDoc.DocumentElement().GetChildElements(noLi);
	noLi.Next();
	iNode = noLi.Next();
	noLi.Close();
		
	TPtrC expr;
	TBuf8<256> exp;
	TInt i = 0;
	while (aItem.GetNextString(expr) == KErrNone)
		{
		if (i > 0)
			exp.Append(_L(" "));
		exp.Append(expr);
		i++;
		}
	
    if ( i == 0 )
        {
        return KErrGeneral;
        }
    
    while(TRUE)
    	{
    	TInt i = exp.Find(_L8("\\"));
    	if (i == KErrNotFound)
    		break;
    	exp.Replace(i,1,_L8("/"));
    	}    
    
    TXmlEngXPathEvaluator eval;
    iResult = eval.EvaluateL(exp,iNode);
    
	return KErrNone;    
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::ToNumberL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ToNumberL( CStifItemParser& /*aItem*/)
	{
	
	TXmlEngXPathEvaluator eval;
	
	_LIT8(str1,"number(/test/test1[@attr1='1'])");
	_LIT8(str2,"/test/test1[@attr1='1']");
    RXmlEngXPathResult res1 = eval.EvaluateL(str1,iDoc.DocumentElement());
    RXmlEngXPathResult res2 = eval.EvaluateL(str2,iDoc.DocumentElement());
    
    _LIT(KNodeToNumber,"Node to number checking error.");
    // Jigs - PREQ 2051 - Change
  	//if(res1.AsNumber() != TXmlEngXPathObject::ToNumberL(res2.AsNodeSet()[0]))
		if(res1.AsNumber() != XmlEngXPathUtils::ToNumberL(res2.AsNodeSet()[0]))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KNodeToNumber);
		iLog->Log((TDesC)KNodeToNumber);
    	return KErrGeneral;
		}
    
    res1.Close();
    res2.Close();
    
    _LIT8(str3,"number(/test/test1)");
	_LIT8(str4,"/test/test1");
    res1 = eval.EvaluateL(str3,iDoc.DocumentElement());
    res2 = eval.EvaluateL(str4,iDoc.DocumentElement());
    
    _LIT(KSetToNumber,"NodeSet to number checking error.");
    // Jigs - PREQ 2051 - Change
  	if(res1.AsNumber() != XmlEngXPathUtils::ToNumberL(res2.AsNodeSet()))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KSetToNumber);
		iLog->Log((TDesC)KSetToNumber);
    	return KErrGeneral;
		}
    
    res1.Close();
    res2.Close();
    
    _LIT8(str5,"number(\"12345.6\")");
	_LIT8(str6,"12345.6");
    res1 = eval.EvaluateL(str5,iDoc.DocumentElement());
 
    _LIT(KStringToNumber,"String to number checking error.");
    // Jigs - PREQ 2051 - Change
  	if(res1.AsNumber() != XmlEngXPathUtils::ToNumberL(str6))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KStringToNumber);
		iLog->Log((TDesC)KStringToNumber);
		return KErrGeneral;
		}
		 
    res1.Close();
    
    _LIT(KBoolToNumber,"Boolean to number checking error.");
    // Jigs - PREQ 2051 - Change
  	if(!(XmlEngXPathUtils::ToNumberL(TRUE) == 1.0 && XmlEngXPathUtils::ToNumberL(FALSE) == 0.0))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KBoolToNumber);
		iLog->Log((TDesC)KBoolToNumber);
		return KErrGeneral;
		}

	return KErrNone;    
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::ToBoolL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ToBoolL( CStifItemParser& /*aItem*/)
	{
	
	TXmlEngXPathEvaluator eval;
	
	_LIT8(str1,"boolean(/test/test1[@attr1='1'])");
	_LIT8(str2,"/test/test1[@attr1='1']");
	
    RXmlEngXPathResult res1 = eval.EvaluateL(str1,iDoc.DocumentElement());
    RXmlEngXPathResult res2 = eval.EvaluateL(str2,iDoc.DocumentElement());
    
    _LIT(KNodeToNumber,"Node to bool checking error.");
  	//if(!(res1.AsBoolean() && TXmlEngXPathObject::ToBoolean(res2.AsNodeSet()[0])))
	if(!(res1.AsBoolean() && XmlEngXPathUtils::ToBoolean(res2.AsNodeSet()[0])))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KNodeToNumber);
		iLog->Log((TDesC)KNodeToNumber);
    	return KErrGeneral;
		}
    
    res1.Close();
    res2.Close();

    _LIT8(str3,"boolean(/test/test4)");
	_LIT8(str4,"/test/test4");
    res1 = eval.EvaluateL(str3,iDoc.DocumentElement());
    res2 = eval.EvaluateL(str4,iDoc.DocumentElement());
    
    _LIT(KSetToNumber,"NodeSet to bool checking error.");
  	//if(!(!res1.AsBoolean() && !TXmlEngXPathObject::ToBoolean(res2.AsNodeSet())))
		if(!(!res1.AsBoolean() && !XmlEngXPathUtils::ToBoolean(res2.AsNodeSet())))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KSetToNumber);
		iLog->Log((TDesC)KSetToNumber);
    	return KErrGeneral;
		}
    
    res1.Close();
    res2.Close();
    
    _LIT8(str5,"boolean(12345.6)");
	res1 = eval.EvaluateL(str5,iDoc.DocumentElement());
    
    _LIT(KStringToNumber,"String to number checking error.");
  	//if(!(res1.AsBoolean() && TXmlEngXPathObject::ToBoolean((TReal)12345.6)))
		if(!(res1.AsBoolean() && XmlEngXPathUtils::ToBoolean((TReal)12345.6)))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KStringToNumber);
		iLog->Log((TDesC)KStringToNumber);
		return KErrGeneral;
		}
		 
    res1.Close();
    
    _LIT(KBoolToNumber,"String to boolean checking error.");
  	//if(TXmlEngXPathObject::ToBoolean(KNullDesC8))
		if(XmlEngXPathUtils::ToBoolean(KNullDesC8))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KBoolToNumber);
		iLog->Log((TDesC)KBoolToNumber);
		return KErrGeneral;
		}

	return KErrNone;    
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::ToStringL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ToStringL( CStifItemParser& /*aItem*/)
	{
	
	TXmlEngXPathEvaluator eval;
	
	_LIT8(str1,"string(/test/test1[@attr1='1'])");
	_LIT8(str2,"/test/test1[@attr1='1']");
    RXmlEngXPathResult res1 = eval.EvaluateL(str1,iDoc.DocumentElement());
    RXmlEngXPathResult res2 = eval.EvaluateL(str2,iDoc.DocumentElement());

    _LIT(KNodeToNumber,"Node to string checking error.");
  	RBuf8 tmp;
  	res1.AsStringL( tmp );
  	CleanupClosePushL( tmp );
  	RBuf8 tmp2;
  	//TXmlEngXPathObject::ToStringL(res2.AsNodeSet()[0],tmp2);
		XmlEngXPathUtils::ToStringL(res2.AsNodeSet()[0],tmp2);
  	CleanupClosePushL( tmp2 );
  	
  	if(tmp.Compare(tmp2))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KNodeToNumber);
		iLog->Log((TDesC)KNodeToNumber);
		CleanupStack::PopAndDestroy( &tmp2 );
		CleanupStack::PopAndDestroy( &tmp );
    	return KErrGeneral;
		}
    
    tmp.Close();
    tmp2.Close();
    res1.Close();
    res2.Close();
    
    _LIT8(str3,"string(/test/test1)");
	_LIT8(str4,"/test/test1");
    res1 = eval.EvaluateL(str3,iDoc.DocumentElement());
    res2 = eval.EvaluateL(str4,iDoc.DocumentElement());
    
    _LIT(KSetToNumber,"NodeSet to string checking error.");
  	res1.AsStringL(tmp);
  	//TXmlEngXPathObject::ToStringL(res2.AsNodeSet(),tmp2);
		XmlEngXPathUtils::ToStringL(res2.AsNodeSet(),tmp2);
  	
  	if(tmp.Compare(tmp2))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KSetToNumber);
		iLog->Log((TDesC)KSetToNumber);
		CleanupStack::PopAndDestroy( &tmp2 );
		CleanupStack::PopAndDestroy( &tmp );
    	return KErrGeneral;
		}
    
    tmp.Close();
    tmp2.Close();
    res1.Close();
    res2.Close();
    
    _LIT8(str5,"string(12345.6)");
	res1 = eval.EvaluateL(str5,iDoc.DocumentElement());
    
    _LIT(KStringToNumber,"Number to string checking error.");
  	res1.AsStringL(tmp);
  	//TXmlEngXPathObject::ToStringL((TReal)12345.6,tmp2);
		XmlEngXPathUtils::ToStringL((TReal)12345.6,tmp2);
  	
  	if(tmp.Compare(tmp2))
		{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KStringToNumber);
		iLog->Log((TDesC)KStringToNumber);
		CleanupStack::PopAndDestroy( &tmp2 );
		CleanupStack::PopAndDestroy( &tmp );
		return KErrGeneral;
		}
	res1.Close();
	tmp2.Close();
	CleanupStack::PopAndDestroy( &tmp2 );
	tmp.Close();
    
    //TXmlEngXPathObject::ToStringL(TRUE,tmp);
		XmlEngXPathUtils::ToStringL(TRUE,tmp);
    _LIT(KBoolToNumber,"Boolean checking error.");
  	if(tmp.Compare(_L8("true")))
		{
		CleanupStack::PopAndDestroy( &tmp );
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KBoolToNumber);
		iLog->Log((TDesC)KBoolToNumber);
		return KErrGeneral;
		}
    
    CleanupStack::PopAndDestroy(&tmp);
	return KErrNone;    
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::SaveResultL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::SaveResultL( CStifItemParser& aItem)
	{
	_LIT(KOutputFileErrorName,"No output file name.");
	_LIT(KOutputFileError,"Output file error: %d.");
	_LIT8(KBoolean,"XPathResult: %b.");
	_LIT8(KNumber,"XPathResult: %f.");
	_LIT8(KString,"XPathResult: %S.");
	_LIT8(KNode,"XPathResult:\r\n");
	    
    TPtrC OutputFile;
    if ( aItem.GetNextString(OutputFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileErrorName);
     	iLog->Log((TDesC)KOutputFileErrorName);
        return KErrGeneral;
        }
    RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile outputFile;
	TFileName outFileName;
	outFileName.Copy(OutputFile);
	
	// Open output file
	TInt err = outputFile.Replace(fs,outFileName,EFileShareAny|EFileWrite);
	
	// If output file is correct
	if (err == KErrNone)
    {
    	TBuf8<256> info;
		TBuf8<256> info2;
    	if(iResult.Type() == RXmlEngXPathResult::EBoolean)
    		{
    		info.Format(KBoolean,iResult.AsBoolean());
			outputFile.Write(info);
    		}
    	else if(iResult.Type() == RXmlEngXPathResult::ENumber)
    		{
    		info.Format(KNumber,iResult.AsNumber());
			outputFile.Write(info);
    		} 
    	else if(iResult.Type() == RXmlEngXPathResult::EString)
    		{
    		RBuf8 str;
			iResult.AsStringL(str);
			info2.Copy(str);
			info.Format(KString,&info2);
			outputFile.Write(info);
			str.Close();
			}
		else
			{
			RXmlEngNodeSet nodeSet = iResult.AsNodeSet();
			outputFile.Write(KNode);
			for(int i=0; i<nodeSet.Length();i++)
				{
				TXmlEngNode node = nodeSet[i];
				SaveNode(outputFile,node);
				}
			}
    	outputFile.Close();
	}
	else
	{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KOutputFileError, err);
		iLog->Log(KOutputFileError, err);
		return err;	
	}

	iResult.Close();
	CleanupStack::PopAndDestroy(&fs);
	return KErrNone;    
	}

TInt CLibxml2Tester::InstanceL( CStifItemParser& /*aItem*/)
	{
    _LIT(KTestFail,"Test failed: %d.");
    
    _LIT8(KInstA, "<data>A<test>5</test><test att=\"10\">6</test></data>");
    _LIT8(KInstB, "<data>B<test attr='345'>TestText</test><test/></data>");
    _LIT8(KInstC, "<data>C<test3>345</test3></data>");
    _LIT8(KDataContext, "<doc>D<test>10</test><test2/></doc>");
    
    RXmlEngDocument instA = parser.ParseL(TPtrC8(KInstA)); 
    RXmlEngDocument instB = parser.ParseL(TPtrC8(KInstB)); 
    RXmlEngDocument instC = parser.ParseL(TPtrC8(KInstC)); 
    RXmlEngDocument contextDoc = parser.ParseL(TPtrC8(KDataContext)); 

    // register instance doc trees
    CXmlEngXFormsInstanceMap* map = CXmlEngXFormsInstanceMap::NewLC();
    map->AddEntryL(instA, _L8("A"));
    map->AddEntryL(instB, _L8("B"));
    map->AddEntryL(instC, _L8("C"));

    // TC1
    TXmlEngXPathEvaluator eval;
    eval.SetInstanceMap(map);
    RXmlEngXPathResult result = eval.EvaluateL(_L8("instance('A')"), contextDoc, NULL);
	
    if(result.Type() != RXmlEngXPathResult::ENodeset)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        }        
	RXmlEngNodeSet set = result.AsNodeSet();
    if(set.Length() != 1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        } 
    TXmlEngNode node = set[0];
    if(node.NodeType() != TXmlEngNode::EElement)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        } 
    TPtrC8 name = node.Name();
    if(name.Compare(_L8("data")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        } 
    RBuf8 content;
    node.WholeTextContentsCopyL(content);
    if( content.Compare(_L8("A56")) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        } 
    if(!node.IsSameNode(instA.DocumentElement()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        } 
    content.Close();
	result.Free();

    // TC2
    result = eval.EvaluateL(_L8("/doc/test + instance('A')/test[position()=2]/@att + instance('C')/test3"), contextDoc, NULL);
    if(result.Type() != RXmlEngXPathResult::ENumber)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 2);
		iLog->Log(KTestFail, 2);
		return KErrGeneral;
        } 
    TReal value = result.AsNumber();
    if(value != 365)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 2);
		iLog->Log(KTestFail, 2);
		return KErrGeneral;
        } 
    result.Free();

    // TC3
    RXmlEngNodeSet dependents;
    dependents.InitializeL();
    RXmlEngXPathExpression expr = eval.CreateExpressionL( _L8("/doc/test + instance('A')/test[position()=2]/@att + instance('C')/test3"), NULL);
    result = expr.EvaluateWithDependenciesL(contextDoc, dependents);
    if(result.Type() != RXmlEngXPathResult::ENumber)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		return KErrGeneral;
        } 
    value = result.AsNumber();
    if(value != 365)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		return KErrGeneral;
        } 
    
    if(dependents.Length() != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		return KErrGeneral;
        } 
    
    for(int i = 0; i < 3; i++) {
        node = dependents[i];
        TPtrC8 name1 = node.Name();
        RBuf8 content1;
        node.WholeTextContentsCopyL( content1 );
        
        if ((!name1.Compare(_L8("test")) && !content1.Compare(_L8("10"))) &&
                (!name1.Compare(_L8("test3")) && !content1.Compare(_L8("345"))) &&
                (!name1.Compare(_L8("att")) && !content1.Compare(_L8("10"))))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
	    	iLog->Log(KTestFail, 3);
	    	content1.Close();
		    return KErrGeneral;    
            }
        
        content1.Close();
    }
    result.Free();
    expr.Destroy();
    dependents.Free();

    // TC4
    dependents.InitializeL();
    expr = eval.CreateExpressionL(_L8("instance('B')/test[@attr=instance('C')/test3]") , NULL);
    result = expr.EvaluateWithDependenciesL(contextDoc, dependents);
    RBuf8 strValue;
    result.AsStringL( strValue );
    if(strValue.Compare(_L8("TestText")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 4);
		iLog->Log(KTestFail, 4);
		strValue.Close();
		return KErrGeneral;
        }
        
    strValue.Close();

    if(dependents.Length() != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 4);
		iLog->Log(KTestFail, 4);
		return KErrGeneral;
        }
        
    TInt match = 0;
    
    for(int i = 0; i < 3; i++) {
        node = dependents[i];
        TPtrC8 name2 = node.Name();
        RBuf8 content2;
        node.WholeTextContentsCopyL( content2 );
        
            if (!name2.Compare(_L8("test")) && !content2.Compare(_L8("TestText"))) {
                match++;
            } else if (!name2.Compare(_L8("test3")) && !content2.Compare(_L8("345"))) {
                match++;
            } else if (!name2.Compare(_L8("attr")) && !content2.Compare(_L8("345"))) {
                match++;
            }
        content2.Close();
    }
    if (match != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 4);
	    iLog->Log(KTestFail, 4);
	    return KErrGeneral;
        }
        
    dependents.Free();
    expr.Destroy();
    result.Free();

    // TC5
    map->RemoveEntryL(_L8("A"));
    eval.SetInstanceMap(map);
    result = eval.EvaluateL(_L8("instance('A')"), contextDoc, NULL);
    if(result.Type() != RXmlEngXPathResult::ENodeset)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 5);
		iLog->Log(KTestFail, 5);
		return KErrGeneral;
        }        
	set = result.AsNodeSet();
    if(set.Length() != 0)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 5);
		iLog->Log(KTestFail, 5);
		return KErrGeneral;
        } 

    // TC6
    if(map != eval.InstanceMap())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 6);
		iLog->Log(KTestFail, 6);
		return KErrGeneral;
        } 
        
    // TC7
    dependents.InitializeL();
    expr = eval.CreateExpressionL( _L8("instance('B')/test[@attr='x']") , NULL);
    result = expr.EvaluateWithDependenciesL(contextDoc, dependents);
    if(result.Type() != RXmlEngXPathResult::ENodeset)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 7);
		iLog->Log(KTestFail, 7);
		return KErrGeneral;
        }        
	set = result.AsNodeSet();
    if(set.Length() != 0)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 7);
		iLog->Log(KTestFail, 7);
		return KErrGeneral;
        } 
    if(dependents.Length() != 1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 7);
		iLog->Log(KTestFail, 7);
		return KErrGeneral;
        }

    for(int i = 0; i < 1; i++) {
        node = dependents[i];
        TPtrC8 name3 = node.Name();
        RBuf8 content3;
        node.WholeTextContentsCopyL( content3 );
        
        if (!(!name3.Compare(_L8("attr")) && !content3.Compare(_L8("345"))))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 7);
	    	iLog->Log(KTestFail, 7);
	    	content3.Close();
		    return KErrGeneral;    
            }
        content3.Close();
    }
    dependents.Free();
    expr.Destroy();
    result.Free();

    // cleanup 
    instA.Close();   
    instB.Close();   
    instC.Close();   
    contextDoc.Close(); 
    CleanupStack::PopAndDestroy(); // map

	return KErrNone;    
	}

class XPathDate: public MXmlEngXPathExtensionFunction
    {
public:
    TInt MinArity(){return 1;}
    TXmlEngEvaluationStatus Evaluate(MXmlEngXPathEvaluationContext* aEvalCtxt)
        {
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::ENodeset)
            aEvalCtxt->SetResultL( _L8("Nodeset") );
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::ENumber)
            aEvalCtxt->SetResultL( _L8("Number") );
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::EString)
            aEvalCtxt->SetResultL( _L8("String") );
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::EBoolean)
            return EError;
        return ESucceeded;
        }
    };
    
class XPathDate2: public MXmlEngXPathExtensionFunction
    {
public:
    TInt MinArity(){return 1;}
    TXmlEngEvaluationStatus Evaluate(MXmlEngXPathEvaluationContext* aEvalCtxt)
        {
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::ENodeset)
            {
            aEvalCtxt->InitializeNodeSetResult();
            aEvalCtxt->AppendToResult(aEvalCtxt->Argument(0).AsNodeSet());
            if(aEvalCtxt->Result().Type() != RXmlEngXPathResult::ENodeset)
                return EError;
            }
            
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::ENumber)
            aEvalCtxt->SetResult(aEvalCtxt->Argument(0).AsNumber());
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::EString)
            aEvalCtxt->SetResultL( _L8("String") );
        if (aEvalCtxt->Argument(0).Type() == RXmlEngXPathResult::EBoolean)
            aEvalCtxt->SetResult(ETrue);
        return ESucceeded;
        }
    };

class XPathDate3: public MXmlEngXPathExtensionFunction
    {
public:
    TInt MinArity(){return 2;}
    TXmlEngEvaluationStatus Evaluate(MXmlEngXPathEvaluationContext* aEvalCtxt)
        {
        if(aEvalCtxt->ArgCount() != 2)
            {
            return EInvalidArgumentNumber;
            }
        aEvalCtxt->SetResult(aEvalCtxt->Argument(0).AsNumber() + aEvalCtxt->Argument(1).AsNodeSet().Length());
        return ESucceeded;
        }
    };

// -----------------------------------------------------------------------------
// CLibxml2Tester::ExtensionFunctionL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ExtensionFunctionL( CStifItemParser& /*aItem*/)
	{
	XPathDate myDate;

	_LIT8(KDataContext, "<doc><test>10</test><test/><test/><test2>data</test2></doc>");
	_LIT(KTestFail,"Test failed: %d.");
	
	XPathDate myFunct;
	//TXmlEngExtensionFunctionDescriptor tExt = {&myFunct,"MyFunction"};
	TXmlEngExtensionFunctionDescriptor tExt;
	tExt.iFunc = &myFunct;
	tExt.iName = "MyFunction";
		// jigs
    XmlEngXPathConfiguration::AddExtensionFunctionL(tExt);
    // jigs
    if(!XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyFunction"),KNullDesC8))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 5);
		iLog->Log(KTestFail, 5);
		return KErrGeneral;
        }
	
	RXmlEngDocument contextDoc = parser.ParseL(TPtrC8(KDataContext)); 
	
	TXmlEngXPathEvaluator eval;
    RXmlEngXPathResult result = eval.EvaluateL( _L8("MyFunction(/doc/test)"), contextDoc, NULL);
	
    if(result.Type() != RXmlEngXPathResult::EString)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        }
    
    RBuf8 str;
    result.AsStringL( str );
    _LIT8(str2,"Nodeset");
    _LIT8(str3,"String");
    _LIT8(str4,"Number");
    if(str.Compare(str2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		str.Close();
		return KErrGeneral;
        }
    str.Close();
    result.Free();
    
	result = eval.EvaluateL( _L8("MyFunction(10)"), contextDoc, NULL);
	if(result.Type() != RXmlEngXPathResult::EString)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 2);
		iLog->Log(KTestFail, 2);
		return KErrGeneral;
        }
	result.AsStringL(str);
	if(str.Compare(str4))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 2);
		iLog->Log(KTestFail, 2);
		str.Close();
		return KErrGeneral;
        }
    str.Close();
    result.Free();
    
	result = eval.EvaluateL(_L8("MyFunction(\"data\")"), contextDoc, NULL);
	if(result.Type() != RXmlEngXPathResult::EString)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		return KErrGeneral;
        }
	result.AsStringL(str);
	if(str.Compare(str3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		str.Close();
		return KErrGeneral;
        }
    str.Close();
    result.Free();
    
    TRAPD(err,result = eval.EvaluateL(_L8("MyFunction(boolean(10))"), contextDoc, NULL);)
    if(err != KXmlEngErrXPathResult)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 4);
		iLog->Log(KTestFail, 4);
		return KErrGeneral;
        }
        // jigs
    if(!XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyFunction"),KNullDesC8))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 5);
		iLog->Log(KTestFail, 5);
		return KErrGeneral;
        }
        // jigs
    XmlEngXPathConfiguration::RemoveExtensionFunction(tExt);
    	// jigs
    if(XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyFunction"),KNullDesC8))
        {
        TestModuleIf().Printf(infoNum++,KTEST_NAME, KTestFail, 6);
		iLog->Log(KTestFail, 6);
		return KErrGeneral;
        }

    contextDoc.Close();
	return KErrNone;    
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::ExtensionVectorL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ExtensionVectorL( CStifItemParser& /*aItem*/)
	{
	XPathDate myDate;
	XPathDate2 myDate2;
	XPathDate3 myDate3;
	
	TXmlEngExtensionFunctionDescriptor extFunc;
	extFunc.iFunc = &myDate;
	extFunc.iName = "MyDate";
	
	TXmlEngExtensionFunctionDescriptor extFunc2;
	extFunc2.iFunc = &myDate2;
	extFunc2.iName = "MyDate2";
	
	TXmlEngExtensionFunctionDescriptor extFunc3;
	extFunc3.iFunc = &myDate3;
	extFunc3.iName = "MyDate3";
	
	RArray<TXmlEngExtensionFunctionDescriptor> extVect;
	extVect.Append(extFunc);
    extVect.Append(extFunc2);
    extVect.Append(extFunc3);


	_LIT8(KDataContext, "<doc><test>10</test><test/><test/><test2>data</test2></doc>");
	_LIT(KTestFail,"Test failed: %d.");
	
	XPathDate myFunct;
    //TXmlEngXPathConfiguration::AddExtensionFunctionVectorL(extVect,extVect.Count());
    XmlEngXPathConfiguration::AddExtensionFunctionVectorL(extVect,extVect.Count());
    
    if(!XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyDate"),KNullDesC8))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 6);
		iLog->Log(KTestFail, 6);
		return KErrGeneral;
        }
	
	RXmlEngDocument contextDoc = parser.ParseL(TPtrC8(KDataContext)); 
	
	TXmlEngXPathEvaluator eval;
    RXmlEngXPathResult result = eval.EvaluateL(_L8("MyDate2(/doc/test)"), contextDoc, NULL);
	
    if(result.Type() != RXmlEngXPathResult::ENodeset)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        }
    
    RXmlEngNodeSet set = result.AsNodeSet();
    if(set.Length() != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 1);
		iLog->Log(KTestFail, 1);
		return KErrGeneral;
        }
    result.Free();
    
	result = eval.EvaluateL(_L8("MyDate2(10)"), contextDoc, NULL);
	if(result.Type() != RXmlEngXPathResult::ENumber)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 2);
		iLog->Log(KTestFail, 2);
		return KErrGeneral;
        }
	if(result.AsNumber() != 10)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 2);
		iLog->Log(KTestFail, 2);
		return KErrGeneral;
        }
    result.Free();
    
	result = eval.EvaluateL(_L8("MyDate2(\"data\")"), contextDoc, NULL);
	if(result.Type() != RXmlEngXPathResult::EString)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		return KErrGeneral;
        }
	RBuf8 str;
	result.AsStringL(str);
	_LIT8(str3,"String");
	if(str.Compare(str3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 3);
		iLog->Log(KTestFail, 3);
		str.Close();
		return KErrGeneral;
        }
    str.Close();
    result.Free();
    
    result = eval.EvaluateL(_L8("MyDate2(boolean(10))"), contextDoc, NULL);
	if(result.Type() != RXmlEngXPathResult::EBoolean)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 4);
		iLog->Log(KTestFail, 4);
		return KErrGeneral;
        }
	if(!result.AsBoolean())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 4);
		iLog->Log(KTestFail, 4);
		return KErrGeneral;
        }
    result.Free();
    
    result = eval.EvaluateL(_L8("MyDate3(3,/doc/test)"), contextDoc, NULL);
	if(result.Type() != RXmlEngXPathResult::ENumber)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 5);
		iLog->Log(KTestFail, 5);
		return KErrGeneral;
        }
	if(result.AsNumber() != 6)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 5);
		iLog->Log(KTestFail, 5);
		return KErrGeneral;
        }
    result.Free();
    // jigs
    if(!XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyDate"),KNullDesC8))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME, KTestFail, 6);
		iLog->Log(KTestFail, 6);
		return KErrGeneral;
        }
    // jigs    
    XmlEngXPathConfiguration::RemoveExtensionFunctionVector(extVect,extVect.Count());
		// jigs
    if(XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyDate2"),KNullDesC8))
        {
        TestModuleIf().Printf(infoNum++,KTEST_NAME, KTestFail, 7);
		iLog->Log(KTestFail, 7);
		return KErrGeneral;
        }
        // jigs
    if(XmlEngXPathConfiguration::IsFunctionSupportedL(_L8("MyDate"),KNullDesC8))
        {
        TestModuleIf().Printf(infoNum++,KTEST_NAME, KTestFail, 7);
		iLog->Log(KTestFail, 7);
		return KErrGeneral;
        }

    contextDoc.Close();
    extVect.Close();
	return KErrNone;    
	}

