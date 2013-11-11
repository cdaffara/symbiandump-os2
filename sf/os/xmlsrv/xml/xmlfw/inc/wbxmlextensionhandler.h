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
// Plugin interface
// 
//

#ifndef __WBXMLEXTENSIONHANDLER_H__
#define __WBXMLEXTENSIONHANDLER_H__

#include <e32std.h>


class RString;

namespace Xml
{

class MWbxmlExtensionHandler
/**
The Wbxml api extension class.
Inherited by classes wishing to cater for this interface, e.g. WbxmlParser.

@publishedAll
@released
*/
	{
public:

/**
The uid identifying this extension interface.
*/
	enum TExtInterfaceUid {EExtInterfaceUid = 0x101FE212};
	
/**
This method is a receive notification of an extension instruction.
The type of extension is specified in aToken and is based on the global tokens
specified in WBXML documents.
@see http://www.w3.org/TR/wbxml/
@param				aData is the extension data.
@param				aToken is the global unambiguous token value.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode) = 0;

	};

}

#endif //__WBXMLEXTENSIONHANDLER_H__
