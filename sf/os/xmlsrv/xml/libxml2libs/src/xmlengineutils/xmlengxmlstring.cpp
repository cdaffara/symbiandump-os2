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
// This file contains declarations of string methods used by
// XmlEngine's modules.
//

#include <xml/utils/xmlengxestrings.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengutils.h>

#include <stdapis/libxml2/libxml2_xmlstring.h>
#include <stdapis/libxml2/libxml2_globals.h>

#include <utf.h>
#include <escapeutils.h>

//----------------------------------------------------------- TXmlEngConstString

// ------------------------------------------------------------------------------
// Compares two strings.
//
// @return Result as <b>strcmp()</b>
// ------------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngConstString::Compare(
    TXmlEngConstString aString ) const
    {
    char* str2 = aString.iString;
    char* str1 = iString;
    if (str1 == str2)
        {
        return(0);
        }
    if (!str1 && *str2 == '\x0')
        {
        return(0);
        }
    if (!str2 && *str1 == '\x0')
        {
        return(0);
        }
    if (!str1 )
        {
        return(-1);
        }
    if (!str2 )
        {
        return(1);
        }

    register int tmp;
    do  {
        tmp = *str1++ - *str2;
        if (tmp != 0) 
            return(tmp);
        } while (*str2++ != 0);
    return 0;
    }

// ------------------------------------------------------------------------------
// Returns copy of the TXmlEngConstString contents
// ------------------------------------------------------------------------------
//
EXPORT_C char* TXmlEngConstString::CopyL() const
    {

    xmlChar* copy = xmlStrdup((xmlChar*)iString);
    OOM_IF(iString && !copy);

    return (char*) copy;
    }


// ------------------------------------------------------------------------------
// Returns size of string in bytes
// ------------------------------------------------------------------------------
//
EXPORT_C TUint TXmlEngConstString::Size() const 
    {
	if (iString)
		{
		return (TUint)strlen(iString);
		}
    return 0;
    }

// ------------------------------------------------------------------------------
// @return Size of a string: a number of characters
// @note for non-ASCII contents <b>Size() != Length()</b>
// ------------------------------------------------------------------------------
//
EXPORT_C TUint TXmlEngConstString::Length()  const
    {
    // Note: return 0 for NULL strings
    if(!iString)
        return 0;
    TInt len = xmlUTF8Strlen((const xmlChar*)iString);
    return len != -1 ? (TUint)len : 0;
    }

//---------------------------------------------------------------------- TXmlEngString

// ------------------------------------------------------------------------------
// Frees the string
// ------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngString::Free()
    {xmlFree(iString); iString = 0;}

// ------------------------------------------------------------------------------
// Transfers string and its ownership between TXmlEngString objects;
//
// Any previous content is freed. The source becomes empty (NULL).
// This allows reusing TXmlEngString variables and free them only once.
//
// @note
// This is only a convenience method: it allows avoiding many TXmlEngString variables
// and corresponding calls of Free() method for each. In practice, having a separate
// variable for string values makes code more clearer [No performance impact too].
// ------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngString::Set(TXmlEngString& aSrc)
    {
    if(iString)
        {
        if(iString == aSrc.iString)
            {
            return;
            }
        else
            {
            delete iString;
            }
        }
    iString = aSrc.iString;
    aSrc.iString = 0;
    }

inline HBufC* HBufCFromXmlCharLC(
    const void* aUtf8String )
    {
    if (!aUtf8String)
        return KNullDesC().AllocLC();

    HBufC* res = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                        TPtrC8((TUint8*)aUtf8String,
                        strlen((char*)aUtf8String)));		
    CleanupStack::PushL(res);
    return res;
    }

inline HBufC* HBufCFromXmlCharL(
    const void* aUtf8String ) 
    {
    if(!aUtf8String)
        return KNullDesC().AllocL();
    else
        return CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                    TPtrC8((TUint8*)aUtf8String,
                    strlen((char*)aUtf8String)));
    }

/**
Allocates UTF-16 heap descriptor from the TDOMString contents

Typical use:
@code
    ...
    HBufC* attrValue = attr.Value().AllocL();
    ...
@endcode

AllocLC() does the same, but returned HBufC* is already on the cleanup stack
after a call to this method.
*/
EXPORT_C HBufC* TXmlEngConstString::AllocL(void) const
    {
    return HBufCFromXmlCharL(iString); // Note: it's inlined
    }

/**
Allocates UTF-16 heap descriptor from the TXmlEngConstString contents and
pushes the descriptor to cleanup stack.

Typical use:
@code
    ...
    HBufC* attrValue = attr.Value().AllocLC();
    ...
@endcode
*/
EXPORT_C HBufC* TXmlEngConstString::AllocLC(void)const
    {
    return HBufCFromXmlCharLC(iString);  // Note: it's inlined
    }

// ------------------------------------------------------------------------------
// Initialize TDOMString from Symbian descriptor.
//
// UTF-16 content of descriptor is converted into UTF-8 zero-terminated
// c-string, for which memory is allocated.
// ------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngString::SetL(const TDesC& aDes)
    {
    if(iString)
        PushL();
    char* str = const_cast<char*> (XmlEngXmlCharFromDesL(aDes));
    if (iString)    
        CleanupStack::PopAndDestroy(iString);
    iString = str;
    }

//
// ------------------------------------------------------------------------------
// Initialize TDOMString from Symbian descriptor.
//
// Byte content of descriptor is taken as UTF-8 string,
// and zero-terminated c-string is created from it.
// ------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngString::SetL(const TDesC8& aDes8)
    {
    if(iString)
        PushL();
    char* str = const_cast<char*> (XmlEngXmlCharFromDes8L(aDes8));
    if (iString)    
        CleanupStack::PopAndDestroy(iString);
    iString = str;
    }

// ------------------------------------------------------------------------------
// Allocates heap descriptor as AllocL() and frees internal c-string.
//
// Typical use:
// @code
//    ...
//    HBufC* attrValue = attr.WholeValueCopyL().AllocAndFreeL();
//    ...
// @endcode
//
// AllocAndFreeLC() does the same, but returned HBufC* is already on the cleanup stack
// after a call to this method.
// ------------------------------------------------------------------------------
//
EXPORT_C HBufC* TXmlEngString::AllocAndFreeL()
{
    CleanupStack::PushL((TAny*)iString);  // PushL()  
    HBufC* ret = HBufCFromXmlCharL(iString); // inlined AllocL();
    CleanupStack::PopAndDestroy();
    iString = NULL;
    return ret;
}

/**
Allocates heap descriptor as AllocLC() and frees internal c-string.

More details in AllocAndFreeL() description.
*/
EXPORT_C HBufC* TXmlEngString::AllocAndFreeLC()
{
    // inlined AllocAndFreeL():
    CleanupStack::PushL((TAny*)iString);  // PushL()
    HBufC* hbuf = HBufCFromXmlCharL(iString); // inlined AllocL();
    CleanupStack::PopAndDestroy(); // iString
    iString = NULL;
    //
    CleanupStack::PushL(hbuf);
    return hbuf;
}


EXPORT_C void TXmlEngString::AppendL(TXmlEngString str)
    {
    if(str.iString)
        {
        TUint len1 = iString ? Size(): 0;
        TUint len2 = str.Size();
        char* res = new (ELeave) char[len1 + len2 + 1];
        OOM_IF_NULL(res);
        if (iString)
            memcpy(res, iString, len1);
        memcpy(res+len1, str.iString, len2);
        res[len1+len2] = 0;
        delete iString;
        iString = res;
        }
    }

EXPORT_C void TXmlEngString::AppendL(TXmlEngString str1, TXmlEngString str2)
    {   
        AppendL(str1);
        AppendL(str2);
    }

