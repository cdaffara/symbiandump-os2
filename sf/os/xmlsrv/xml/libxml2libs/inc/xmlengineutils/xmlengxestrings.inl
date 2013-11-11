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
// Declaration of methods from string classes.
//

inline TXmlEngConstString::TXmlEngConstString()
        : iString(NULL) {}

inline TXmlEngConstString::TXmlEngConstString(const char* const aString) 
        :iString(const_cast<char*>(aString)) {}

inline TBool TXmlEngConstString::NotNull() const 
    {return iString!=NULL;}

inline TBool TXmlEngConstString::IsNull()  const 
    {return iString==NULL;}

inline const char* TXmlEngConstString::Cstring() const 
    {return iString;}

inline const char* TXmlEngConstString::CstringDef(const char* aDefaultValue) const 
    {return iString ? iString : aDefaultValue;}

inline TBool TXmlEngConstString::Equals(TXmlEngConstString aString) const
    {return !Compare(aString);}

inline TPtrC8 TXmlEngConstString::PtrC8() const 
    {return TPtrC8((TUint8*)iString, Size());}

inline void TXmlEngConstString::Set(char* aString) 
    {iString = aString;}

// ---------------------------------------------------- TXmlEngString

// --------------------------------------------------------------------------------------
// Default constructor
// --------------------------------------------------------------------------------------
//
inline TXmlEngString::TXmlEngString()
    :TXmlEngConstString(NULL) {}

// --------------------------------------------------------------------------------------
//Constructor for TDOMString initialized with given c-string
// --------------------------------------------------------------------------------------
//
inline TXmlEngString::TXmlEngString(char* aString)
        :TXmlEngConstString(aString) {}

// --------------------------------------------------------------------------------------
// Sets new value; old contents is freed
// --------------------------------------------------------------------------------------
//
inline void TXmlEngString::Set(char* aStr) 
    {delete iString; iString = aStr;}

inline void TXmlEngString::Close() 
    {Free();}

// --------------------------------------------------------------------------------------
// Pushes string pointer to CleanupStack. 
// 
// DO NOT forget to call CleanupStack::PopAndDestroy() for it
// (or CleanupStack::Pop() and then Free())
// --------------------------------------------------------------------------------------
//
inline TXmlEngString TXmlEngString::PushL() 
    {CleanupStack::PushL((TAny*)iString); return *this;}
