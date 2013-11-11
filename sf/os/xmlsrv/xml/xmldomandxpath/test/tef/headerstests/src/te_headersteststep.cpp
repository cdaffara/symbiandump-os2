// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// te_s60HeadersTestStep.cpp
// 
//


#include <e32std.h>
#include "te_headersteststep.h"
#include <xml/utils/xmlengxestrings.h>


//test S60_header_files
//dom
#include <xml/dom/xmlengattr.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengcdatasection.h>
#include <xml/dom/xmlengcharacterdata.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengcomment.h>
#include <xml/dom/xmlengdatacontainer.h>
#include <xml/dom/xmlengdataserializer.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengdocumentfragment.h>
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdomparser.h>
#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengentityreference.h>
#include <xml/dom/xmlengerrors.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengnamespace.h>
#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengnodefilter.h>
#include <xml/dom/xmlengnodelist.h>
#include <xml/dom/xmlengnodelist_impl.h>
#include <xml/dom/xmlengoutputstream.h>
#include <xml/dom/xmlengprocessinginstruction.h>
#include <xml/dom/xmlengserializationoptions.h>
#include <xml/dom/xmlengtext.h>
#include <xml/dom/xmlenguserdata.h>
//serializer
#include <xml/dom/xmlengdeserializer.h>
#include <xml/dom/xmlengdeserializerdom.h>
#include <xml/dom/xmlengextendedcontenthandler.h>
#include <xml/dom/xmlengparsingoptions.h>
#include <xml/dom/xmlengserializeerrors.h>
#include <xml/dom/xmlengserializer.h>
//xpath
#include <xml/dom/xmlengnamespaceresolver.h>
#include <xml/dom/xmlengnodeset.h>
#include <xml/dom/xmlengxformsinstancemap.h>
#include <xml/dom/xmlengxpath.h>
#include <xml/dom/xmlengxpathconfiguration.h>
#include <xml/dom/xmlengxpatherrors.h>
#include <xml/dom/xmlengxpathevaluationcontext.h>
#include <xml/dom/xmlengxpathevaluator.h>
#include <xml/dom/xmlengxpathexpression.h>
#include <xml/dom/xmlengxpathextensionfunction.h>
#include "xmlengxpathobject.h"   // is now xml xmlengxpathutils.h
#include <xml/dom/xmlengxpathresult.h>
#include <xml/dom/xmlengxpathutils.h>

//test .inl files
//dom
#include <xml/dom/xmlengattr.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengcdatasection.h>
#include <xml/dom/xmlengcharacterdata.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengcomment.h>
#include <xml/dom/xmlengdatacontainer.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengdocumentfragment.h>
#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengentityreference.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengnamespace.h>
#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengnodelist.h>
#include <xml/dom/xmlengprocessinginstruction.h>
#include <xml/dom/xmlengtext.h>
//xpath
#include <xml/dom/xmlengnodeset.h>
#include <xml/dom/xmlengxpathexpression.h>
#include <xml/dom/xmlengxpathresult.h>
#include <xml/dom/xmlengxpathutils.h>


/**
 * A compiler test to ensure that our headerfiles will be included as the 
 *   customer(s) expect them to.  Since these header files are not used
 *   by our code, but only by our customer's, this may be the only way 
 *   to verify any changes to them.
 * 
 * SYSLIB-XML-UT-4003, DEF129353
 */ 
TVerdict CXmlDomHeadersTestStep::doTestStepL()
	{
	SetTestStepResult(EPass);
	
	INFO_PRINTF1(_L("Compiler Test for s60 .h & xml .inl redirection. -- SYSLIB-XML-UT-4003"));

	// make sure the typdef in the S60 header file works.
	// will throw a compiler error if it can't find the class.
	TBuf8<4> str(_L8("true"));
	TBool aBool = TXmlEngXPathObject::ToBoolean(str);
	
	return TestStepResult();
	}

