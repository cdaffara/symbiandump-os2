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

#ifndef __XMLFRAMEWORKERRORS_H__
#define __XMLFRAMEWORKERRORS_H__

/**
@file

This file contains allocated error codes and
generic Panic error messages and the Panic function.
The XmlFramework has been allocated error codes in the range: -17550 to -17599 

@publishedAll
@released
*/

#include <e32base.h>

namespace Xml
{

// Leave Codes

// ECom related

const TInt KErrXmlStringDictionaryPluginNotFound	= -17550;
const TInt KErrXmlParserPluginNotFound				= -17551;
const TInt KErrXmlGeneratorPluginNotFound			= -17552;
const TInt KErrXmlPluginNotFound					= -17553;

// CCharSetConverter

const TInt KErrXmlBadCharacterConversion			= -17554;
const TInt KErrXmlUnsupportedCharacterSet			= -17555;
const TInt KErrXmlUnavailableCharacterSet			= -17556;

// MStringDictionary

const TInt KErrXmlUnsupportedElement				= -17557;
const TInt KErrXmlUnsupportedAttribute				= -17558;
const TInt KErrXmlUnsupportedAttributeValue			= -17559;
const TInt KErrXmlMissingStringDictionary			= -17560;

// General

const TInt KErrXmlUnsupportedDocumentVersion		= -17561;
const TInt KErrXmlDocumentCorrupt					= -17562;
const TInt KErrXmlStringPoolTableNotFound			= -17563;
const TInt KErrXmlBadIndex							= -17564;
const TInt KErrXmlUnsupportedExtInterface			= -17566;

// Custom resolver related
/**
Indicates the parser query is matched to more than one parser. 
This error can be returned only if the request is to leave in such a case - LeaveOnMany flag is set.
*/
const TInt KErrXmlMoreThanOneParserMatched			= -17567;
const TInt KErrXmlStringTooBig                      = -17568;
const TInt KErrXmlFirst                             = -17550;
const TInt KErrXmlLast                              = -17599;




}

 

#endif // __XMLFRAMEWORKERRORS_H__
