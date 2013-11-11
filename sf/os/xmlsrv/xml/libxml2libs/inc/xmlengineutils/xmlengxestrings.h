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
// This file contains definitions of string used by
// XmlEngine's modules.
//



/**
 @file
 @publishedPartner
 @released
*/

#ifndef XESTRINGS_H
#define XESTRINGS_H

#include <e32base.h>

/**
Defines a constant string to be used as an argument value for arguments of type
TDOMString and TDOMStringArg.

No '\0' character can appear in the string.

Usage:
@code
    _XMLLIT(KLitName, "<some ASCII literal to be used as UTF-8 string>");
	obj.SomeMethod(KLitName);   // argument type is TXmlEngConstString or TDOMStringArg
@endcode
*/
#define _XMLLIT(LitName, utf8String) const TUint8* const LitName = (const TUint8*) utf8String;

/**
Implements a constant string used within libxml2-based API's.

TXmlEngConstString contains a pointer to a zero-terminated UTF-8 string.  The
contents of the string is not supposed to be changed. From this point of view
TXmlEngConstString instances are close to the concept of immutable strings.

Ownership of the string is not defined for TXmlEngConstString. Thus, in
different contexts (APIs), different string-handling policies may be applied.
For instance, in XML Engine SAX API all reported via callbacks strings of
TXmlEngConstString type are assumed owned by the parser.

TXmlEngConstString is a base class for other specialized string classes in XML
Engine APIs.
*/
class TXmlEngConstString
    {
    friend class Libxml2_XmlAttributes;

public:
    /** Default constructor */
    inline TXmlEngConstString();

    /**
    Constructor
	@param aString The string for initialization.  Ownership is not transferred
	and the string must stay in scope for the lifetime of this object.
    */
    inline TXmlEngConstString(const char* aString);

    /**
    Check if the string is not NULL
    @return ETrue if the string is not null, EFalse otherwise
    */
    inline TBool NotNull() const;

    /**
    Check if the string is NULL
    @return ETrue if the string is null, EFalse otherwise
    */
    inline TBool IsNull()  const;

    /**
    Gets a c string
    @return The string as a C string or NULL if null
    */
    inline const char* Cstring() const;

    /**
    Gets a c string or if NULL, return a default value.
    @param aDefaultValue default value
    @return If not NULL, the c string, otherwise aDefaultValue
    */
    inline const char* CstringDef(const char* aDefaultValue) const;

	/**
	Compares with another string.  NULL and "" are considered equal.
    
    @param aString The string to compare
    @return ETrue if equal, EFalse otherwise
    */
    inline TBool Equals(TXmlEngConstString aString) const;

    /**
    Compares with another string.  NULL and "" are considered equal.
    
    @param aString The string to compare
    @return 0 if equal, -1 otherwise
    */
    IMPORT_C TInt Compare(TXmlEngConstString aString) const;

	/**
	Allocates a HBufC from the string contents.  Ownership is transferred to
	the caller.

    Typical use:
    @code
       ...
       HBufC* attrValue = attr.Value().AllocL();
       ...
    @endcode
    
	@see AllocLC()
	@leave KErrNoMemory Memory allocation failure
	@leave KUriUtilsCannotConvert String cannot be converted
    */
    IMPORT_C HBufC* AllocL() const;

	/**
	Allocates a HBufC from the string contents and pushes the descriptor to
	cleanup stack.  Ownership is transferred to the caller.
    
    Typical use:
    @code
       ...
       HBufC* attrValue = attr.Value().AllocLC();
       ...
    @endcode

	@see AllocL()
	@leave KErrNoMemory Memory allocation failure
	@leave KUriUtilsCannotConvert String cannot be converted
    */
    IMPORT_C HBufC* AllocLC() const;

    /**
    Get a TPtrC8 that contains the string
    @return A point descriptor that holds the string
    */
    inline TPtrC8 PtrC8() const;

	/**
	Creates a new copy of the string contents.  Ownership for the new string is
	transferred to the caller.

    @return A c string copy of the string contents
	@leave KErrNoMemory Memory allocation failure
    */
    IMPORT_C char* CopyL()  const;

    /**
    Returns the size of the string in bytes
    @return The size
    */
    IMPORT_C TUint Size()   const;
	
	/**
	Get the length of the string.  For non-ASCII strings, Size() != Length() due
	to the UTF-8 encoding used for non-ASCII characters.
    @return The number of characters in the string
    */
    IMPORT_C TUint Length() const;

protected:
	/**
	Set a new string value.  The old string is not freed.  Ownership is not
	transferred.
    @param aString The new value
    */
    inline void Set(char* aString);

protected:
    /** String value */
    char* iString;
};

/**
General-purpose string used in libxml2-based API's.

This class holds a zero-terminated sequence of bytes (c-style string).  It is
commonly used to represent a sequence of UTF-8 characters.

TXmlEngString objects should be treated as if they were pointers. The
destructor for TXmlEngString does nothing.  This means that the developer needs
to free the memory allocated by TXmlEngString by calling Free() or Close()
(Close() is an alias for Free()) and to do so only once.

The reason for this design is that the goal is to have a string class that is
as small as a one-pointer sized object that is mostly stored on the stack, not
on the heap (and therefore no additional memory allocation is required).  It is
also as flexible as possible to allow developers to create their own derived
classes which may provide auto_ptr-like or reference counting solutions.

The contents of this class can be replaced, destroyed, copied and converted to
c strings and Symbian descriptor types. 

The contents of TXmlEngString may also be modified with AppendL().
  
When TXmlEngString is the return type in libxml2 based API's, this usually
indicates that a string has been newly allocated, which should be freed by the
method caller.
*/
class TXmlEngString: public TXmlEngConstString
{
public:
    /** Default constructor */
    inline TXmlEngString();

	/**
	Constructs a new string from a given c string.  Ownership is transferred
	and the string must be freed with Free() or Close().

    @param aString A heap-based c string
    */
    inline TXmlEngString(char* aString);

    /** Frees the string */
    IMPORT_C void Free();

	/**
	Sets a new value.  The old string is freed.  Ownership is transferred and
	the string must be freed with Free() or Close().

    @param aStr The new string
    */
    inline void Set(char* aStr);

    /**
	Transfers a string and its ownership from another TXmlEngString object.  If
	this object currently stores a string it is freed.

	@param aSrc The source string
    */
    IMPORT_C void Set(TXmlEngString& aSrc);   
    
    /** Frees the string */
    inline void Close();

    /**
	Pushes this string to the cleanup stack.  This must be matched with a
	corresponding call to CleanupStack::Pop() or CleanupStack::PopAndDestroy().
	@return This object
	@leave KErrNoMemory Memory allocation failure
    */
    inline TXmlEngString PushL();

    /**
    Initializes the string, converting from a UTF-16 descriptor to
	a UTF-8 zero-terminated string.
    @param aDes The new value
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetL(const TDesC& aDes);

    /**
    Initializes the string from a descriptor
    @param aDes The new value
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetL(const TDesC8& aDes);

	/**
	Creates a new UTF-16 HBufC from the UTF-8 string contents and transfers
	ownership of the string.  The string held by this object is freed.

    Typical use:
    @code
      ...
      HBufC* attrValue = attr.WholeValueCopyL().AllocAndFreeL();
      ...
    @endcode
    
	@see AllocAndFreeLC()
    @return The copied string
	@leave KErrNoMemory Memory allocation failure
	@leave KUriUtilsCannotConvert String cannot be converted
    */
    IMPORT_C HBufC* AllocAndFreeL();

    /**
	Creates a new UTF-16 HBufC from the UTF-8 string contents and transfers
	ownership of the string.  The result is placed on the cleanup stack.  The
	string held by this object is freed.

    Typical use:
    @code
      ...
      HBufC* attrValue = attr.WholeValueCopyL().AllocAndFreeL();
      ...
    @endcode
    
	@see AllocAndFreeL()
    @return The copied string
	@leave KErrNoMemory Memory allocation failure
	@leave KUriUtilsCannotConvert String cannot be converted
    */
    IMPORT_C HBufC* AllocAndFreeLC();

    /**
	Appends new text to the string
    @param aStr The string to add
	@leave KErrNoMemory Memory allocation failure
    */
    IMPORT_C void AppendL(TXmlEngString aStr);

    /**
	Appends new text to the string.
    @param aStr1 The first string to append
    @param aStr2 The second string to append
	@leave KErrNoMemory Memory allocation failure
    */
    IMPORT_C void AppendL(TXmlEngString  aStr1, TXmlEngString aStr2);

protected:
    /** 
    Constructs the object from a TXmlEngConstString
    @param aStr The string to initialize from
    */
    inline TXmlEngString(const TXmlEngConstString& aStr): TXmlEngConstString(aStr.Cstring()) {}
};

#include <xml/utils/xmlengxestrings.inl>

#endif // XESTRINGS_H

