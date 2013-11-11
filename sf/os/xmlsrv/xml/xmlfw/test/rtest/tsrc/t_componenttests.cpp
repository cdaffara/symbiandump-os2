// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <ecom/ecom.h>
#include <charconv.h>

#include <xml/attribute.h>
#include <xml/documentparameters.h>
#include <xml/stringdictionarycollection.h>
#include <xml/taginfo.h>
#include <xml/xmlframeworkerrors.h>
#include <xml/plugins/dictionarycodepage.h>
#include <xml/plugins/charsetconverter.h>

#include "t_componenttests.h"
#include "t_testconstants.h"
#include "t_stringdictionary00.h"
#include "t_stringdictionary00tagtable.h"

using namespace Xml;


_LIT(KTest,"t_ComponentTests");

static RTest test(KTest);

_LIT8 (KTestString1, "This is a test string1.");
_LIT8 (KTestString2, "This is a test string2.");
_LIT8 (KTestString3, "This is a test string3.");
_LIT8 (KTestString4, "This is a test string4.");
_LIT8 (KTestString5, "This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text.");

_LIT8 (KCharSetName, "Test Charset name.");
_LIT8 (KDictionaryUri, "-//TestStringDictionary//DTD TestStringDictionary 1.1//EN~0");
_LIT8 (KAddElement, "Add");

TBuf16<600> largebuf16 = _L16("This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text.");
TBuf8<600> largebuf8 =   _L8("This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text. This is a very large test string containing lots and lots and lots and lots of text.");

_LIT8 (KEncoding, "UTF-8");
const TInt KMibEnum = 106;

const TInt KUnloadedCodePage = 999;
const TInt KUnknownToken = 999;



CComponentTests* CComponentTests::NewL()
	{
	CComponentTests* self = new(ELeave) CComponentTests();
	return self;
	}


CComponentTests::CComponentTests()
	{
	// do nothing;
	}


CComponentTests::~CComponentTests()
	{
	// do nothing;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1271
@SYMTestCaseDesc	    Tests for RTagInfo::Uri(),RTagInfo::Prefix(),RTagInfo::LocalName() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for heap leak,and open handles
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test1L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1271 Test1L "));

	// RTagInfo Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString string1 = iStringPool.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = iStringPool.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = iStringPool.OpenStringL(KTestString3);

	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&iStringPool);

	RTagInfo tagInfo;
	tagInfo.Open(string1, string2, string3);

	const RString& uri = tagInfo.Uri();
	const RString& prefix = tagInfo.Prefix();
	const RString& localName = tagInfo.LocalName();

	test (KTestString1() == uri.DesC());
	test (KTestString2() == prefix.DesC());
	test (KTestString3() == localName.DesC());

	tagInfo.Close();
	iStringPool.Close();

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1272
@SYMTestCaseDesc	    Test for RTagInfo copy constructor,assignment operator,Copy functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create three RTagInfo objects using assignment operator,copy constructor,and Copy function with the same information that describes an element.
                        Tests for the integrity of the data of all the three objects.
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1272 Test2L "));

	// RTagInfo Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString string1 = iStringPool.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = iStringPool.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = iStringPool.OpenStringL(KTestString3);

	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&iStringPool);

	RTagInfo tagInfo;
	tagInfo.Open(string1, string2, string3);
	RTagInfo tagInfo2 = tagInfo;
	RTagInfo tagInfo3 (tagInfo);
	RTagInfo tagInfo4 = tagInfo.Copy();

	const RString& uri = tagInfo.Uri();
	const RString& prefix = tagInfo.Prefix();
	const RString& localName = tagInfo.LocalName();

	test (KTestString1() == uri.DesC());
	test (KTestString2() == prefix.DesC());
	test (KTestString3() == localName.DesC());

	test (KTestString1() == tagInfo2.Uri().DesC());
	test (KTestString2() == tagInfo2.Prefix().DesC());
	test (KTestString3() == tagInfo2.LocalName().DesC());

	test (KTestString1() == tagInfo3.Uri().DesC());
	test (KTestString2() == tagInfo3.Prefix().DesC());
	test (KTestString3() == tagInfo3.LocalName().DesC());

	test (KTestString1() == tagInfo4.Uri().DesC());
	test (KTestString2() == tagInfo4.Prefix().DesC());
	test (KTestString3() == tagInfo4.LocalName().DesC());

	tagInfo.Close();
	tagInfo4.Close();
	iStringPool.Close();

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1273
@SYMTestCaseDesc	    Tests for RTagInfo created from RAttribute class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the information of RTagInfo objects.
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test3L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1273 Test3L "));

	// RAttribute Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString string1 = iStringPool.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = iStringPool.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = iStringPool.OpenStringL(KTestString3);
	CleanupClosePushL(string3);

	RString string4 = iStringPool.OpenStringL(KTestString4);

	CleanupStack::Pop(&string3);
	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&iStringPool);

	RAttribute attribute;
	attribute.Open(string1, string2, string3);
	attribute.SetValue(string4);

	const RTagInfo& tagInfo = attribute.Attribute();
	const RString& value = attribute.Value();
	TAttributeType type = attribute.Type();

	test (KTestString1() == tagInfo.Uri().DesC());
	test (KTestString2() == tagInfo.Prefix().DesC());
	test (KTestString3() == tagInfo.LocalName().DesC());
	test (KTestString4() == value.DesC());
	test (EAttributeType_NONE == type);

	attribute.Close();
	iStringPool.Close();

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1274
@SYMTestCaseDesc	    Tests for RTagInfo created from RAttribute::Uri,Prefix,LocalName functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create three RAttribute objects using copy constructor,assignment operator and Copy function.Test for information of RAttribute objects
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test4L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1274 Test4L "));

	// RAttribute Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString string1 = iStringPool.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = iStringPool.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = iStringPool.OpenStringL(KTestString3);
	CleanupClosePushL(string3);

	RString string4 = iStringPool.OpenStringL(KTestString4);

	CleanupStack::Pop(&string3);
	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&iStringPool);

	RAttribute attribute;
	attribute.Open(string1, string2, string3, string4);
	RAttribute attribute2 = attribute;
	RAttribute attribute3 (attribute);
	RAttribute attribute4 = attribute.Copy();

	const RTagInfo& tagInfo = attribute.Attribute();
	const RString& value = attribute.Value();
	TAttributeType type = attribute.Type();

	test (KTestString1() == tagInfo.Uri().DesC());
	test (KTestString2() == tagInfo.Prefix().DesC());
	test (KTestString3() == tagInfo.LocalName().DesC());
	test (KTestString4() == value.DesC());
	test (EAttributeType_NONE == type);

	test (KTestString1() == attribute2.Attribute().Uri().DesC());
	test (KTestString2() == attribute2.Attribute().Prefix().DesC());
	test (KTestString3() == attribute2.Attribute().LocalName().DesC());
	test (KTestString4() == attribute2.Value().DesC());
	test (EAttributeType_NONE == attribute2.Type());

	test (KTestString1() == attribute3.Attribute().Uri().DesC());
	test (KTestString2() == attribute3.Attribute().Prefix().DesC());
	test (KTestString3() == attribute3.Attribute().LocalName().DesC());
	test (KTestString4() == attribute3.Value().DesC());
	test (EAttributeType_NONE == attribute3.Type());

	test (KTestString1() == attribute4.Attribute().Uri().DesC());
	test (KTestString2() == attribute4.Attribute().Prefix().DesC());
	test (KTestString3() == attribute4.Attribute().LocalName().DesC());
	test (KTestString4() == attribute4.Value().DesC());
	test (EAttributeType_NONE == attribute4.Type());

	attribute.Close();
	attribute4.Close();
	iStringPool.Close();

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1275
@SYMTestCaseDesc	    Tests for RTagInfo created from RAttribute::Uri,Prefix,LocalName functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create RAttribute Test for information of RAttribute objects
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1275 Test5L "));

	// RAttribute Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString string1 = iStringPool.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = iStringPool.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = iStringPool.OpenStringL(KTestString3);
	CleanupClosePushL(string3);

	RString string4 = iStringPool.OpenStringL(KTestString4);

	CleanupStack::Pop(&string3);
	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&iStringPool);

	RAttribute attribute;
	attribute.Open(string1, string2, string3, string4);

	const RTagInfo& tagInfo = attribute.Attribute();
	const RString& value = attribute.Value();
	TAttributeType type = attribute.Type();

	test (KTestString1() == tagInfo.Uri().DesC());
	test (KTestString2() == tagInfo.Prefix().DesC());
	test (KTestString3() == tagInfo.LocalName().DesC());
	test (KTestString4() == value.DesC());
	test (EAttributeType_NONE == type);

	attribute.Close();
	iStringPool.Close();

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1276
@SYMTestCaseDesc	    Tests for information of the document to be passed to client RDocumentParameters::CharacterSetName() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the character set name of document.
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test6L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1276 Test6L "));

	// RDocumentParameters Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString characterSetName = iStringPool.OpenStringL(KCharSetName);

	CleanupStack::Pop(&iStringPool);

	RDocumentParameters docParam;
	docParam.Open(characterSetName);

	test (KCharSetName() == docParam.CharacterSetName().DesC());

	docParam.Close();
	iStringPool.Close();


	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1277
@SYMTestCaseDesc	    Tests for information of the document to be passed to client RDocumentParameters::CharacterSetName functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the character set name of document
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test7L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1277 Test7L "));

	// RDocumentParameters Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	iStringPool.OpenL();
	CleanupClosePushL(iStringPool);

	RString characterSetName = iStringPool.OpenStringL(KCharSetName);
	CleanupStack::Pop(&iStringPool);

	RDocumentParameters docParam;
	docParam.Open(characterSetName);

	test (KCharSetName() == docParam.CharacterSetName().DesC());

	docParam.Close();
	iStringPool.Close();

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1278
@SYMTestCaseDesc	    Tests for RStringDictionaryCollection::CompareThisDictionary(),SwitchCodePage,
                        ElementL,AttributeL,AttributeValuePairL,AttributeValueL functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for no error,unsupported XML value,XML unsupported Element,
                        XML unsupported Attribute,XML unsupported AttributeValue
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test8L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1278 Test8L "));

	// RStringDictionaryCollection Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	RStringDictionaryCollection nsCollection;
	CleanupClosePushL(nsCollection);

	nsCollection.OpenL();

	nsCollection.OpenDictionaryL (KDictionaryUri);

	RString nsUri = nsCollection.StringPool().OpenStringL(KDictionaryUri);
	CleanupClosePushL(nsUri);

	RString element;
	RString attribute;
	RString value;

	CleanupClosePushL(element);
	CleanupClosePushL(attribute);
	CleanupClosePushL(value);


	test (nsCollection.CurrentDictionaryL().CompareThisDictionary(nsUri) == (TInt) ETrue);

	test (nsCollection.CurrentDictionaryL().SwitchCodePage(KUnloadedCodePage) == KErrXmlMissingStringDictionary);

	TRAPD(err, nsCollection.CurrentDictionaryL().ElementL(CTestStringDictionary00::ETokenValueAdd, element));
	test (err == KErrNone);
	test (element.DesC() == KAddElement);
	element.Close(); // as we are reassigning

	TRAP (err, nsCollection.CurrentDictionaryL().ElementL(KUnknownToken, element));
	test (err == KErrXmlUnsupportedElement);

	TRAP (err, nsCollection.CurrentDictionaryL().AttributeL(KUnknownToken, attribute));
	test (err == KErrXmlUnsupportedAttribute);
	attribute.Close(); // as we are reassigning

	TRAP (err, nsCollection.CurrentDictionaryL().AttributeValuePairL(KUnknownToken, attribute, value));
	test (err == KErrXmlUnsupportedAttribute);
	attribute.Close(); // as we are reassigning
	value.Close(); // as we are reassigning

	TRAP (err, nsCollection.CurrentDictionaryL().AttributeValueL(KUnknownToken, value));
	test (err == KErrXmlUnsupportedAttributeValue);
	value.Close(); // as we are reassigning

	CleanupStack::PopAndDestroy(&value);
	CleanupStack::PopAndDestroy(&attribute);
	CleanupStack::PopAndDestroy(&element);
	CleanupStack::PopAndDestroy(&nsUri); // on the stringpool

	nsCollection.Close();

	CleanupStack::Pop(&nsCollection);


	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1279
@SYMTestCaseDesc	    Tests for CCharSetConverter functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for UTF-8 to Unicode and Unicode to UTF-8 character set conversions.
                        Tests for no error , no memory error
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CComponentTests::Test9L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1279 Test9L "));

	// CCharSetConverter Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	CCharSetConverter* charSet = CCharSetConverter::NewL();
	CleanupStack::PushL(charSet);

	TInt err = KErrNone;
	TUint charSetUid = 0;

	TRAP (err, charSet->PrepareCharConvL(charSetUid, KEncoding));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	TRAP (err, charSet->PrepareCharConvL(charSetUid, KMibEnum));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	// convert to unicode from utf-8
	HBufC16* unicodeConversion = NULL;
	err = charSet->ConvertToUnicodeL(charSetUid, KTestString1, unicodeConversion);
	CleanupStack::PushL(unicodeConversion);
	test (err == KErrNone);
	TBuf16<100> compare;
	compare.Copy(KTestString1);
	test (*unicodeConversion == compare);

	// convert to utf-8 from unicode
	HBufC8* utf8Conversion = NULL;
	err = charSet->ConvertFromUnicodeL(*unicodeConversion, charSetUid, utf8Conversion);
	CleanupStack::PushL(utf8Conversion);
	test (err == KErrNone);
	test (*utf8Conversion == KTestString1());

	CleanupStack::PopAndDestroy(utf8Conversion);
	CleanupStack::PopAndDestroy(unicodeConversion);

	// really a test of CharConv as we call it directly.
	TRAP (err, charSet->PrepareToConvertToOrFromL(charSetUid));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);


	const TInt KUcs4Count = 23;
	TUint32 ucs4 [KUcs4Count] = {0};
	ucs4 [0] = 'T';
	ucs4 [1] = 'h';
	ucs4 [2] = 'i';
	ucs4 [3] = 's';
	ucs4 [4] = ' ';
	ucs4 [5] = 'i';
	ucs4 [6] = 's';
	ucs4 [7] = ' ';
	ucs4 [8] = 'a';
	ucs4 [9] = ' ';
	ucs4 [10] = 't';
	ucs4 [11] = 'e';
	ucs4 [12] = 's';
	ucs4 [13] = 't';
	ucs4 [14] = ' ';
	ucs4 [15] = 's';
	ucs4 [16] = 't';
	ucs4 [17] = 'r';
	ucs4 [18] = 'i';
	ucs4 [19] = 'n';
	ucs4 [20] = 'g';
	ucs4 [21] = '1';
	ucs4 [22] = '.';

	utf8Conversion = NULL;
	err = charSet->ConvertUcs4CharactersToEncodingL(ucs4, KUcs4Count, KCharacterSetIdentifierUtf8, utf8Conversion);
	CleanupStack::PushL(utf8Conversion);
	test (err == KErrNone);
	test (*utf8Conversion == KTestString1());
	CleanupStack::PopAndDestroy(utf8Conversion);

	HBufC8* Output8 = NULL;
	err = charSet->ConvertFromUnicodeL(largebuf16, charSetUid, Output8);
	CleanupStack::PushL(Output8);
	test (err == KErrNone);
	test(largebuf8 == *Output8);

	HBufC16* Output16 = NULL;
	err = charSet->ConvertToUnicodeL(charSetUid, largebuf8, Output16);
	CleanupStack::PushL(Output16);
	test (err == KErrNone);
	test(largebuf16 == *Output16);

	CleanupStack::PopAndDestroy(Output16);
	CleanupStack::PopAndDestroy(Output8);
	CleanupStack::PopAndDestroy(charSet);

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-UT-3718
@SYMTestCaseDesc		    CDictionaryCodePage tests
@SYMTestPriority 		    Medium
@SYMTestActions  		    Creating a link between indicies and tokens. Checking a token returns an index from the string pool and then
							checking if an index return a token.
@SYMTestExpectedResults 	Indicies and tokens are returns (and not returned) when expected.
@SYMDEF 		 		 	DEF071977
*/
void CComponentTests::Test10L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-UT-3718 Test10L "));

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	TInt index;
	TInt token;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);



	iStringPool.OpenL(t_StringDictionary00TagTable::Table);
	CleanupClosePushL(iStringPool);

	CDictionaryCodePage* codepage = CDictionaryCodePage::NewL(&t_StringDictionary00TagTable::Table, &t_StringDictionary00TagTable::Table, &t_StringDictionary00TagTable::Table, 0);
	CleanupStack::PushL(codepage);

	// Construct the correlation mapping
	codepage->ConstructIndexMappingL(iCodePage00, CDictionaryCodePage::EStringTypeElement);
	codepage->ConstructIndexMappingL(iCodePage00, CDictionaryCodePage::EStringTypeAttribute);
	codepage->ConstructIndexMappingL(iCodePage00, CDictionaryCodePage::EStringTypeAttributeValue);

	index = codepage->StringPoolIndexFromToken(CTestStringDictionary00::ETokenValueAdd, CDictionaryCodePage::EStringTypeElement);
	test(index!=KErrXmlStringPoolTableNotFound);

	index = codepage->StringPoolIndexFromToken(KUnknownToken, CDictionaryCodePage::EStringTypeElement);
	test(index==KErrXmlStringPoolTableNotFound);

	index = codepage->StringPoolIndexFromToken(CTestStringDictionary00::ETokenValueAdd,CDictionaryCodePage::EStringTypeAttribute);
	test(index!=KErrXmlStringPoolTableNotFound);

	index = codepage->StringPoolIndexFromToken(KUnknownToken, CDictionaryCodePage::EStringTypeAttribute);
	test(index==KErrXmlStringPoolTableNotFound);

	index = codepage->StringPoolIndexFromToken(CTestStringDictionary00::ETokenValueAdd,CDictionaryCodePage::EStringTypeAttributeValue);
	test(index!=KErrXmlStringPoolTableNotFound);

	index = codepage->StringPoolIndexFromToken(KUnknownToken, CDictionaryCodePage::EStringTypeAttributeValue);
	test(index==KErrXmlStringPoolTableNotFound);

	token = codepage->TokenFromStringPoolIndex(0,CDictionaryCodePage::EStringTypeElement);
	test(token==CTestStringDictionary00::ETokenValueAdd);

	token = codepage->TokenFromStringPoolIndex(0,CDictionaryCodePage::EStringTypeAttribute);
	test(token==CTestStringDictionary00::ETokenValueAdd);

	token = codepage->TokenFromStringPoolIndex(0,CDictionaryCodePage::EStringTypeAttributeValue);
	test(token==CTestStringDictionary00::ETokenValueAdd);

	test(codepage->CodePage()==0);


	const RString& aPubId = iStringPool.String(t_StringDictionary00TagTable::EFormalPublicId,
							*(codepage->StringTable(CDictionaryCodePage::EStringTypeElement)));

	TPtrC8 ades = aPubId.DesC();
	test(ades.Compare(KDictionaryUri)==0);

	const RString& aPubId1 = iStringPool.String(t_StringDictionary00TagTable::EFormalPublicId,
							*(codepage->StringTable(CDictionaryCodePage::EStringTypeAttribute)));
	TPtrC8 ades1 = aPubId1.DesC();
	test(ades1.Compare(KDictionaryUri)==0);

	const RString& aPubId2 = iStringPool.String(t_StringDictionary00TagTable::EFormalPublicId,
							*(codepage->StringTable(CDictionaryCodePage::EStringTypeAttributeValue)));
	TPtrC8 ades2 = aPubId2.DesC();
	test(ades1.Compare(KDictionaryUri)==0);


	CleanupStack::PopAndDestroy(codepage);

	CleanupStack::PopAndDestroy(&iStringPool);



	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...
	__UHEAP_MARKEND;

	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-UT-1577
@SYMTestCaseDesc	    Tests the TPtr Overloaded CCharSetConverter functions, making sure they
						function correctly.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for UTF-8 to Unicode and Unicode to UTF-8 character set conversions.
						Tests for heap leak and for open handles at the end of the test.
@SYMTestExpectedResults Test must not fail
@SYMDEF 				DEF075739
*/
void CComponentTests::Test11L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-UT-1577 Test11L "));

	// CCharSetConverter Test
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	TPtr8 conversionOutput8(0,0);
	TPtr16 conversionOutput16(0,0);

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	CCharSetConverter* charSet = CCharSetConverter::NewL();
	CleanupStack::PushL(charSet);

	TInt err = KErrNone;
	TUint charSetUid = 0;

	TRAP (err, charSet->PrepareCharConvL(charSetUid, KEncoding));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	TRAP (err, charSet->PrepareCharConvL(charSetUid, KMibEnum));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	// convert to unicode from utf-8
	err = charSet->ConvertToUnicodeL(charSetUid, KTestString1, conversionOutput16);
	test (err == KErrNone);
	TBuf16<100> compare;
	compare.Copy(KTestString1);
	test (conversionOutput16 == compare);

	// convert to utf-8 from unicode
	err = charSet->ConvertFromUnicodeL(conversionOutput16, charSetUid, conversionOutput8);
	test (err == KErrNone);
	test (conversionOutput8 == KTestString1());

	// really a test of CharConv as we call it directly.
	TRAP (err, charSet->PrepareToConvertToOrFromL(charSetUid));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	//Try converting a large amount of data, will require the output buffer to increase in size
	//Using the TPtr overload
	err = charSet->ConvertFromUnicodeL(largebuf16, charSetUid, conversionOutput8);
	test (err == KErrNone);
	test(largebuf8 == conversionOutput8);

	//Try converting a large amount of data, will require the output buffer to increase in size
	//Using the TPtr overload
	err = charSet->ConvertToUnicodeL(charSetUid, largebuf8, conversionOutput16);
	test (err == KErrNone);
	test(largebuf16 == conversionOutput16);

	CleanupStack::PopAndDestroy(charSet);

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-UT-1662
@SYMTestCaseDesc	    Tests the CCharSetConverter::ConvertToUnicodeL function to ensure that an inputBuffer which is
						larger than 32kB doesn't cause the CCharSetConverter::iConversionBufferSize to overflow.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests CCharSetConverter::ConvertToUnicodeL(TUint32 aSrcCharset, const TDesC8& aInput, TPtr16& aOutput) with aInput > 32kB.
						Tests for heap leak and for open handles at the end of the test
@SYMTestExpectedResults Test must not fail
@SYMDEF 				DEF081549
*/
void CComponentTests::Test12L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-UT-1662 Test12L "));

	// Testing CCharSetConverter::ConvertToUnicodeL(TUint32 aSrcCharset, const TDesC8& aInput, TPtr16& aOutput)
	// with aInput > 32kB.
	//
	// ===========================================

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Starts...

	CCharSetConverter* charSet = CCharSetConverter::NewL();
	CleanupStack::PushL(charSet);

	TInt err = KErrNone;
	TUint charSetUid = 0;

	TRAP (err, charSet->PrepareCharConvL(charSetUid, KEncoding));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	TRAP (err, charSet->PrepareCharConvL(charSetUid, KMibEnum));
	// extra clause for OOM tests
	test (err == KErrNone || err == KErrNoMemory);

	// convert large string (>32kB) to unicode
	TPtr16 largeUnicodeConversion(0,0);
	err = charSet->ConvertToUnicodeL(charSetUid, KTestString5, largeUnicodeConversion);
	test (err == KErrNone);

	CleanupStack::PopAndDestroy(charSet);

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


class CTest : public CBase
	{
	public:
		void MainL();
	};


typedef void (CComponentTests::*ClassFuncPtrL) (void);


//----------------------------------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1280
@SYMTestCaseDesc	    Tests for out of memory conditions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests all basic components for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void DoOomTestL(CComponentTests* aObjectPtr, ClassFuncPtrL testFuncL)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1280 OOM test "));
	TInt err, tryCount = 0;
	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++tryCount);
		User::__DbgMarkStart(RHeap::EUser);
		TRAP(err, (aObjectPtr->*testFuncL)());
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(err==KErrNoMemory);

	if(err==KErrNone)
		{
		// Reset
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
		}
	else
		{
		User::Panic(_L("Unexpected leave reason"),err);
		}

	test.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	test.End();
	}


//----------------------------------------------------------------------------


static void BasicComponentTestsL()
	{
	test.Next(_L("BasicComponentTests..."));

	__UHEAP_MARK;

	CComponentTests* client1 = CComponentTests::NewL();
	CleanupStack::PushL(client1);
	client1->Test1L();
	CleanupStack::PopAndDestroy(client1);

	CComponentTests* client2 = CComponentTests::NewL();
	CleanupStack::PushL(client2);
	client2->Test2L();
	CleanupStack::PopAndDestroy(client2);

	CComponentTests* client3 = CComponentTests::NewL();
	CleanupStack::PushL(client3);
	client3->Test3L();
	CleanupStack::PopAndDestroy(client3);

	CComponentTests* client4 = CComponentTests::NewL();
	CleanupStack::PushL(client4);
	client4->Test4L();
	CleanupStack::PopAndDestroy(client4);

	CComponentTests* client5 = CComponentTests::NewL();
	CleanupStack::PushL(client5);
	client5->Test5L();
	CleanupStack::PopAndDestroy(client5);

	CComponentTests* client6 = CComponentTests::NewL();
	CleanupStack::PushL(client6);
	client6->Test6L();
	CleanupStack::PopAndDestroy(client6);

	CComponentTests* client7 = CComponentTests::NewL();
	CleanupStack::PushL(client7);
	client7->Test7L();
	CleanupStack::PopAndDestroy(client7);

	CComponentTests* client8 = CComponentTests::NewL();
	CleanupStack::PushL(client8);
	client8->Test8L();
	CleanupStack::PopAndDestroy(client8);

	CComponentTests* client9 = CComponentTests::NewL();
	CleanupStack::PushL(client9);
	client9->Test9L();
	CleanupStack::PopAndDestroy(client9);

	CComponentTests* client10 = CComponentTests::NewL();
	CleanupStack::PushL(client10);
	client10->Test10L();
	CleanupStack::PopAndDestroy(client10);

	CComponentTests* client11 = CComponentTests::NewL();
	CleanupStack::PushL(client11);
	client11->Test11L();
	CleanupStack::PopAndDestroy(client11);

	CComponentTests* client12 = CComponentTests::NewL();
	CleanupStack::PushL(client12);
	client12->Test12L();
	CleanupStack::PopAndDestroy(client12);

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


static void OomTestsL()
	{
	test.Next(_L("OOM Tests ..."));

	__UHEAP_MARK;

	CComponentTests* client1 = CComponentTests::NewL();
	CleanupStack::PushL(client1);
	DoOomTestL(client1, &CComponentTests::Test1L);
	CleanupStack::PopAndDestroy(client1);

	CComponentTests* client2 = CComponentTests::NewL();
	CleanupStack::PushL(client2);
	DoOomTestL(client2, &CComponentTests::Test2L);
	CleanupStack::PopAndDestroy(client2);

	CComponentTests* client3 = CComponentTests::NewL();
	CleanupStack::PushL(client3);
	DoOomTestL(client3, &CComponentTests::Test3L);
	CleanupStack::PopAndDestroy(client3);

	CComponentTests* client4 = CComponentTests::NewL();
	CleanupStack::PushL(client4);
	DoOomTestL(client4, &CComponentTests::Test4L);
	CleanupStack::PopAndDestroy(client4);

	CComponentTests* client5 = CComponentTests::NewL();
	CleanupStack::PushL(client5);
	DoOomTestL(client5, &CComponentTests::Test5L);
	CleanupStack::PopAndDestroy(client5);

	CComponentTests* client6 = CComponentTests::NewL();
	CleanupStack::PushL(client6);
	DoOomTestL(client6, &CComponentTests::Test6L);
	CleanupStack::PopAndDestroy(client6);

	CComponentTests* client7 = CComponentTests::NewL();
	CleanupStack::PushL(client7);
	DoOomTestL(client7, &CComponentTests::Test7L);
	CleanupStack::PopAndDestroy(client7);

	CComponentTests* client8 = CComponentTests::NewL();
	CleanupStack::PushL(client8);
	DoOomTestL(client8, &CComponentTests::Test8L);
	CleanupStack::PopAndDestroy(client8);

	CComponentTests* client9 = CComponentTests::NewL();
	CleanupStack::PushL(client9);
	DoOomTestL(client9, &CComponentTests::Test9L);
	CleanupStack::PopAndDestroy(client9);

	CComponentTests* client10 = CComponentTests::NewL();
	CleanupStack::PushL(client10);
	DoOomTestL(client10, &CComponentTests::Test10L);
	CleanupStack::PopAndDestroy(client10);

	CComponentTests* client11 = CComponentTests::NewL();
	CleanupStack::PushL(client11);
	DoOomTestL(client11, &CComponentTests::Test11L);
	CleanupStack::PopAndDestroy(client11);

	CComponentTests* client12 = CComponentTests::NewL();
	CleanupStack::PushL(client12);
	DoOomTestL(client12, &CComponentTests::Test12L);
	CleanupStack::PopAndDestroy(client12);

	__UHEAP_MARKEND;
	}


//----------------------------------------------------------------------------


static void MainL()
	{
	BasicComponentTestsL();
	OomTestsL();
	}


//----------------------------------------------------------------------------


TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test.End();
	test (r==KErrNone);
	delete c;
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}
