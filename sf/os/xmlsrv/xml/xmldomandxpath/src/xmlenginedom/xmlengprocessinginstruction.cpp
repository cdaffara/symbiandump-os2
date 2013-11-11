// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Processing instruction node functions
//

#include <xml/dom/xmlengprocessinginstruction.h>
#include <xml/dom/xmlengcomment.h>
#include "xmlengdomdefs.h"
#include <xml/utils/xmlengxestrings.h>	//pjj18

// ---------------------------------------------------------------
// @return "Target" part of a processing instruction
// 
// @code
//     <?target data?>
// @endcode
//
// @see http://www.w3.org/TR/2004/REC-xml-20040204/#sec-pi
// ---------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngProcessingInstruction::Target() const
    {
    XE_ASSERT_DEBUG(LIBXML_NODE);
    //
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_NODE->name)).PtrC8();
    }

// ---------------------------------------------------------------
// @return "Data" part of a processing instruction
//
// @code
//    <?target data?>
// @endcode
//
// @see http://www.w3.org/TR/2004/REC-xml-20040204/#sec-pi
// ---------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngProcessingInstruction::Data() const
    {
    XE_ASSERT_DEBUG(LIBXML_NODE);
    //
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_NODE->content)).PtrC8();
    }

// ---------------------------------------------------------------
// Sets data part of processing instruction
//
// @note PI contents should not contain "?>" sequence
// ---------------------------------------------------------------
//
EXPORT_C void TXmlEngProcessingInstruction::SetDataL(const TDesC8& aData)
    {
    AsComment().SetContentsL(aData);
    }

