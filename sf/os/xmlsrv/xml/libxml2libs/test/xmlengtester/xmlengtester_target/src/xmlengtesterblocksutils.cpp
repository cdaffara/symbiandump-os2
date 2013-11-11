/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestContentHandler.h"
#include "XMLOutputContentHandler.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <Xml\Parser.h>
#include <Xml\ParserFeature.h>

#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>
#include <xml/utils/xmlengutils.h>
using namespace Xml;

/************************************** Utils **************************************/

// -----------------------------------------------------------------------------
// CLibxml2Tester::UtilsEscapeForXmlValueL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::UtilsEscapeForXmlValueL( CStifItemParser& /*aItem*/ )
{
	_LIT(KFunctionError,"Error in fuction: EscapeForXmlValueL.");
	_LIT(KEscapeSuc,"Function use succesfull.");
   
    const char * tekst = "Test < & > text.";
    const char * tekst3 = "Test &lt; &amp; &gt; text.";
   
    char *tekst2 = XmlEngEscapeForXmlValueL(tekst);
   
    if (this->CompareChars(tekst2,tekst3))
    {
    	TestModuleIf().Printf( infoNum++,KTEST_NAME, KEscapeSuc);
		iLog->Log((TDesC)KEscapeSuc);
    }
    else
    {
    	TestModuleIf().Printf( infoNum++,KTEST_NAME, KFunctionError);
		iLog->Log((TDesC)KFunctionError);
		return KErrGeneral;
    }
   
    return KErrNone;
   
    }
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::UtilsXmlCharFromDesL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::UtilsXmlCharFromDesL( CStifItemParser& /*aItem*/ )
{
	_LIT(KFunctionError,"Error in fuction: XmlCharFromDesL.");
	_LIT(KEscapeSuc,"Function use succesfull.");
    _LIT(KText, "Test Data");
	
	TBufC<10> buf ( KText );
	TPtrC desc = buf.Des();

    char * tekst = XmlEngXmlCharFromDesL(desc);

	TBuf8<10> desc8;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(desc8,desc);
	
    if (this->CompareDescAndChar(tekst,desc8))
    {
    	TestModuleIf().Printf( infoNum++,KTEST_NAME, KEscapeSuc);
		iLog->Log((TDesC)KEscapeSuc);
    }
    else
    {
    	TestModuleIf().Printf( infoNum++,KTEST_NAME, KFunctionError);
		iLog->Log((TDesC)KFunctionError);
		return KErrGeneral;
    }
        
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::UtilsXmlCharFromDes8L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::UtilsXmlCharFromDes8L( CStifItemParser& /*aItem*/ )
{
	_LIT(KFunctionError,"Error in fuction: XmlCharFromDesL.");
	_LIT(KEscapeSuc,"Function use succesfull.");
    _LIT8(KText, "Test Data");
	
	TBufC8<10> buf ( KText );
	TPtrC8 desc = buf.Des();
    
    char * tekst = XmlEngXmlCharFromDes8L(desc);
    
	if (this->CompareDescAndChar(tekst,desc))
    {
    	TestModuleIf().Printf( infoNum++,KTEST_NAME, KEscapeSuc);
		iLog->Log((TDesC)KEscapeSuc);
    }
    else
    {
    	TestModuleIf().Printf( infoNum++,KTEST_NAME, KFunctionError);
		iLog->Log((TDesC)KFunctionError);
		return KErrGeneral;
    }
   
    return KErrNone;
    
    }
