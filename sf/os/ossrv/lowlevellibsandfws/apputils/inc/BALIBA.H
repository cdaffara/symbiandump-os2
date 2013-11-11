// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Written by Brendan, Dec 1996
// Library associated files for pluggable components
// 
//

#if !defined(__LIBASSOC_H__)
#define __LIBASSOC_H__
#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif




class TLibAssocBase
/**
@publishedAll
@released

This is an implementation base class for TLibAssoc.
*/
	{
protected:
	inline TLibAssocBase();
	IMPORT_C TLibAssocBase(const RLibrary& aLib,TAny* aPtr);
	IMPORT_C void Set(const RLibrary& aLib,TAny* aPtr);
	IMPORT_C static void DoUnload(TAny* aThis);
public:
	inline TBool IsNull() const;
private:
	RLibrary iLibrary;
protected:

    /**
    A Pointer to the class instance.
    */
	TAny* iPtr;
	};




//	class TLibAssoc inlines
inline TLibAssocBase::TLibAssocBase()
	: iPtr(NULL)
/**
Default constructor.

It sets the member TLibAssocBase::iPtr to NULL.
*/	
	{}




inline TBool TLibAssocBase::IsNull() const
/**
Tests whether the pointer to the class instance is NULL.

@return ETrue, if the pointer is NULL; EFalse, otherwise.
*/
	{return iPtr==NULL;}


//
//	class TLibAssoc
//
template <class T>
class TLibAssoc : public TLibAssocBase
/**
@publishedAll
@released

Associates a dynamically loadable DLL and an instance of a class that,
typically, will have been created using the ordinal 1 function of that DLL.

An object of this type is useful when cleanup behaviour requires that
a class instance be deleted and the associated DLL be closed.

The DLL is expected to be already open when the TLibAssoc object is created.
*/
	{
public:
	inline TLibAssoc();
	inline TLibAssoc(const RLibrary& aLib,T* aClass);
	inline void Set(const RLibrary& aLib,T* aClass);
	//
	inline void Unload();
	//
	inline operator TCleanupItem();
	operator TLibAssoc*(); // undefined, but here to prevent accidental delete(TLibAssoc)
	//
	inline T* Ptr();
	inline const T* PtrC() const;
	//
	inline operator T*();
	inline operator const T*() const;
	//
	inline T* operator->();
	inline const T* operator->() const;
private:
	static void Cleanup(TAny* aThis); // for TCleanupOperation
	};




template <class T>
inline TLibAssoc<T>::TLibAssoc()
/**
Default constructor.

An association between a DLL and a class instance can be made
after construction using the Set() function.
*/
	{}



template <class T>
inline TLibAssoc<T>::TLibAssoc(const RLibrary& aLib,T* aClass)
	: TLibAssocBase(aLib,aClass)
/**
Constructs the object taking the specified DLL and an instance of
the specified class.

@param aLib   A reference to a DLL that has already been opened.
@param aClass A pointer to an object to be associated with the DLL.
              Typically, this object will have been created using
              the ordinal 1 function from that DLL.
*/	
	{}




template <class T>
inline void TLibAssoc<T>::Set(const RLibrary& aLib,T* aClass)
/**
Makes an association between the specified DLL and an instance of
the specified class.

@param aLib   A reference to a DLL that has already been opened.
@param aClass A pointer to an object to be associated with the DLL.
              Typically, this object will have been created using
              the ordinal 1 function from that DLL.
*/
	{TLibAssocBase::Set(aLib,aClass);}




template <class T>
inline T* TLibAssoc<T>::Ptr()
/**
Gets a pointer to the class instance.

@return A pointer to the class instance.
*/
	{return (T*)iPtr;}




template <class T>
inline const T* TLibAssoc<T>::PtrC() const
/**
Gets a pointer to the const class instance.

@return A pointer to the const class instance.
*/
	{return (const T*)iPtr;}




template <class T>
inline TLibAssoc<T>::operator T*()
/**
Conversion operator.

Invoked by the compiler when a TLibAssoc<T> type is passed to a function that
is prototyped to take a T* type.
*/
	{return (T*)iPtr;}




template <class T>
inline TLibAssoc<T>::operator const T*() const
/**
Const conversion operator.

Invoked by the compiler when a TLibAssoc<T> type is passed to a function that
is prototyped to take a const T* type.
*/
	{return (const T*)iPtr;}




template <class T>
inline T* TLibAssoc<T>::operator->()
/**
Dereferencing operator.

@return A pointer to the class instance.
*/
	{return (T*)iPtr;}




template <class T>
inline const T* TLibAssoc<T>::operator->() const
/**
Const dereferencing operator.

@return A pointer to the const class instance.
*/
	{return (const T*)iPtr;}




template <class T>
inline TLibAssoc<T>::operator TCleanupItem()
/**
The TCleanupItem conversion operator.

Invoked by the compiler when a TLibAssoc<T> type is passed to a function that
is prototyped to take a TCleanupItem type.

The most common usage is to put a cleanup item onto the cleanup stack using
CleanupStack::PushL(). The cleanup operation is represented by the private
static function Cleanup().

For example, if we declare

@code
TLibAssoc<A> a;
@endcode 

then we can simply put a cleanup item onto the cleanup stack
by calling

@code
CleanupStack::PushL(a);
@endcode

@see TCleanupItem
@see CleanupStack::PushL
*/
	{return(TCleanupItem(Cleanup,this));}




template <class T>
inline void TLibAssoc<T>::Unload()
/**
Deletes the class instance and calls Close() on the associated DLL.

@see RLibrary::Close
*/
	{Cleanup(this);}




template <class T>
void TLibAssoc<T>::Cleanup(TAny* aThis)
/**
The cleanup operation.

The implementation deletes the class instance and calls Close() on
the associated DLL.

Note that this function is internal and is not intended for use; it is documented
for completeness.
*/
	{
	delete (T*)(((TLibAssoc<T>*)aThis)->iPtr);
	TLibAssocBase::DoUnload(aThis);
	}

#endif
