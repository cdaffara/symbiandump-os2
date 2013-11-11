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
// This file contains methods allow to change XmlChar to TDesC
// and TDesC to XmlChar
//

#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_entities.h>

#include <xml/utils/xmlengmem.h>

#include <utf.h>
#include <escapeutils.h>

EXPORT_C char* XmlEngEscapeForXmlValueL(
    const char* aUnescapedString )
    {
    xmlChar* res = xmlEncodeSpecialChars(NULL, (const xmlChar*)aUnescapedString);
    if(aUnescapedString && !res)
        {
        OOM_HAPPENED;
        }
    return (char*)res;
    }

EXPORT_C char* XmlEngXmlCharFromDesL(
    const TDesC& aDes ) // UTF-16 descriptor to convert into UTF-8 zero-terminated string
    {
    TUint len = ((aDes.Length() + 4) | 0x1F);
    char* str = new(ELeave) char[len + 1];
    TPtr8 utf8buf((TUint8*)str, 0, len);
    TInt res = CnvUtfConverter::ConvertFromUnicodeToUtf8(utf8buf, aDes);
    if (res == 0)
        {
        str[utf8buf.Size()] = 0;
        return str;
        }
    //the buffer size was not large enough:
    //reallocate the larger buffer
    const TInt maxSizeOfSymbol = 4;
    len = len + res*maxSizeOfSymbol; // res contains number of non-converted UTF-16 characters
    delete str;
    str = new(ELeave) char[len + 1];
    TPtr8 utf8buf2((TUint8*)str, 0, len);
    res = CnvUtfConverter::ConvertFromUnicodeToUtf8(utf8buf2, aDes);
    if (res == 0)
        {
        str[utf8buf2.Size()] = 0;
        return str;
        }
    delete str;
    str = NULL;
    User::Leave(KErrGeneral); // this should never happen;
    return NULL;
    }

EXPORT_C char* XmlEngXmlCharFromDes8L(
    const TDesC8& aDes )
    {
    TUint len = aDes.Length();
    char* str = new(ELeave) char[len + 1];
    memcpy(str, aDes.Ptr(), len);
    str[len] = 0;
    return str;
    }
