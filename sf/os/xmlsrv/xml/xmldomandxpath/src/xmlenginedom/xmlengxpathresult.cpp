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
// Result of XPath expression
//

#include <xml/dom/xmlengxpathresult.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include "xmlengdomdefs.h"
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

#define RESULT_OBJ (static_cast<xmlXPathObjectPtr>(iInternal))

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathResult::RXmlEngXPathResult():iInternal(NULL)
	{
	}

// ---------------------------------------------------------------------------
// Frees any allocated memory by this instance
// ---------------------------------------------------------------------------
//
EXPORT_C void RXmlEngXPathResult::Free()
    {
	xmlXPathFreeObject(RESULT_OBJ);
    }

// ---------------------------------------------------------------------------
// Retrieves type of result
// @note Always check  type of result before use it
// ---------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathResult::TXmlEngXPathResultType RXmlEngXPathResult::Type() const
    {
	return (TXmlEngXPathResultType)RESULT_OBJ->type;
    }

// ---------------------------------------------------------------------------
// Retrieves node-set result.
// Non-RXmlEngNodeSet result will return NULL. No conversion is done from other 
// types.
// ---------------------------------------------------------------------------
//
EXPORT_C RXmlEngNodeSet RXmlEngXPathResult::AsNodeSet() const
    {
	return RXmlEngNodeSet(RESULT_OBJ->nodesetval);	
    }

// ---------------------------------------------------------------------------
// Retrieves copy of result as TXmlEngParserString. 
// In this version of API this method will make conversion from other types of 
// XPath result. In the future release there will be separate method for 
// conversion (this method will return TDOMString and conversion method - 
// TString)
// NOTE: Returns a pointer to the copy of the string, the caller must free it
// ---------------------------------------------------------------------------
//
EXPORT_C void RXmlEngXPathResult::AsStringL(RBuf8& aOutput) const
    {
    xmlChar* copy = NULL;
    if (RESULT_OBJ->type == XPATH_STRING)
        {
        if(RESULT_OBJ->stringval)
            {
            copy = xmlStrdup(RESULT_OBJ->stringval);
            OOM_IF_NULL(copy);
        	}
        }
    else
        {
        copy = xmlXPathCastToString(RESULT_OBJ);
        if(OOM_FLAG)
            {
            if(copy)
                xmlFree(copy);
            OOM_HAPPENED;
            }
        }
    xmlCharAssignToRbuf8L(aOutput,copy);
    }

// ---------------------------------------------------------------------------
// Retrieves numeric value of the result.
// Conversion occurs as XPath spec says if the result is not of ENumber type
// ---------------------------------------------------------------------------
//
EXPORT_C TReal RXmlEngXPathResult::AsNumber() const
    {
	if (RESULT_OBJ->type == XPATH_NUMBER)
		return RESULT_OBJ->floatval;
	else
		return xmlXPathCastToNumber(RESULT_OBJ);
    }

// ---------------------------------------------------------------------------
// Retrieves numeric value of the result.
// Conversion occurs as XPath spec says if the result is not of EBoolean type
// ---------------------------------------------------------------------------
//
EXPORT_C TBool RXmlEngXPathResult::AsBoolean() const
	{
	if (RESULT_OBJ->type == XPATH_BOOLEAN)
		return RESULT_OBJ->boolval;
	else
		return xmlXPathCastToBoolean(RESULT_OBJ);
	}

// -----------------------------------------------------------------------------
// Constructor 
// -----------------------------------------------------------------------------
//
RXmlEngXPathResult::RXmlEngXPathResult(void* aData):iInternal(aData)
	{
	}
