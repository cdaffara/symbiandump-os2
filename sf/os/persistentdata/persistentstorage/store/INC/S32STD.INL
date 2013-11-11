// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Class TStreamId
inline TStreamId::TStreamId(TUint32 aValue)
	: iVal(aValue)
/** Constructs the object with the specified stream ID value.

Users of stores do not normally use this function.

In debug mode, the function checks that the supplied stream ID value is not 
greater than the maximum permitted value, and raises a STORE-Store 2 panic 
if it is. In release mode, no checking on the supplied value is done.

The checking of the supplied value is done by a private function implemented 
in estor. dll. This means that in debug mode, a call is made into estor.dll, 
but in release mode it is not.

@param aValue The stream ID value. */
	{
#if defined (_DEBUG)
	__DbgChkRange(aValue);
#endif
	}
inline TBool TStreamId::operator==(TStreamId anId) const
/** Equality comparison operator. Use this operator to determine whether this stream 
id is equal to the specified stream id.

@param anId The stream id to be compared with this stream id.
@return True, if the stream ids are equal; false otherwise */
	{return iVal==anId.iVal;}
inline TBool TStreamId::operator!=(TStreamId anId) const
/** Inequality comparison operator. Use this operator to determine whether this 
stream id is unequal to the specified stream id.

@param anId The stream id to be compared with this stream id.
@return True, if the two stream ids are unequal, false otherwise. */
	{return iVal!=anId.iVal;}
inline void TStreamId::ExternalizeL(RWriteStream& aStream) const
/** Externalises an object of this class to a write stream.

The presence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class.

@param aStream Stream to which the object should be externalised. */
	{
#if defined (_DEBUG)
	__DbgChkRange(iVal);
#endif
	aStream<<iVal;
	}
inline TUint32 TStreamId::Value() const
/** Gets the stream ID value encapsulated by this object.

@return The stream ID value. */
	{
#if defined (_DEBUG)
	__DbgChkRange(iVal);
#endif
	return iVal;
	}

// Class RStoreWriteStream
inline RStoreWriteStream::RStoreWriteStream(const MExternalizer<TStreamRef>& anExter)
	: RWriteStream(anExter)
/** Constructs an object with an externaliser. The store map CStoreMap is an example 
of an externalizer.

@param anExter Specifies an externaliser */
	{}

// Class TSwizzleCBase
inline TSwizzleCBase::TSwizzleCBase(TStreamRef aRef)
	: iPtr(aRef.Ptr())
	{
#if defined (_DEBUG)
	__DbgChkRef(aRef);
#endif
    }
inline TBool TSwizzleCBase::operator==(const TSwizzleCBase& aSwizzle) const
/** Compares for equality with another swizzle.

Use this operator to determine whether this swizzle represents the same object 
as that represented by the specified swizzle.

Both this swizzle and the specified swizzle must maintain the representation 
of their respective objects as either pointers or stream ids.

If one swizzle maintains the representation of its object as a pointer while 
the other swizzle maintains the representation of its object as a stream id, 
the comparison is meaningless and always returns false.

@param aSwizzle A reference to the swizzle to be compared with this swizzle.
@return True, if the represented objects are the same; False, otherwise. */
	{return iPtr==aSwizzle.iPtr;}
inline TBool TSwizzleCBase::operator==(const TAny* aPtr) const
/** Compares for equality with an in-memory object.

Use this operator to determine whether this swizzle represents the same in-memory 
object as that represented by the specified pointer.

This swizzle must maintain the representation of its object as a pointer. 
If it maintains the representation of its object as a swizzle, the comparison 
is meaningless and always returns false.

@param aPtr A pointer to the object to be compared with this swizzle.
@return True, if the represented objects are the same; False, otherwise. */
	{return iPtr==aPtr;}
inline TBool TSwizzleCBase::operator!=(const TSwizzleCBase& aSwizzle) const
/** Compares for inequality with another swizzle.

Use this operator to determine whether this swizzle represents a different 
object to that represented by the specified swizzle.

Both this swizzle and the specified swizzle must maintain the representation 
of their respective objects as either pointers or stream ids.

If one swizzle maintains the representation of its object as a pointer while 
the other swizzle maintains the representation of its object as a stream id, 
the comparison is meaningless and always returns true.

@param aSwizzle A reference to the swizzle to be compared with this swizzle.
@return True, if the represented objects are not the same; False, otherwise */
	{return iPtr!=aSwizzle.iPtr;}
inline TBool TSwizzleCBase::operator!=(const TAny* aPtr) const
/** Compares for inequality with an in-memory object.

Use this operator to determine whether this swizzle represents a different 
in-memory object to that represented by the specified pointer.

This swizzle must maintain the representation of its object as a pointer. 
If it maintains the representation of its object as a swizzle, the comparison 
is meaningless and always returns true.

@param aPtr A pointer to the object to be compared with this swizzle.
@return True, if the represented objects are not the same; False, otherwise */
	{return iPtr!=aPtr;}
inline TBool TSwizzleCBase::IsPtr() const
/** Tests whether this swizzle currently represents an object as a pointer.

@return True, if this swizzle currently maintains the object representation 
as a pointer; False, otherwise. Note that if the swizzle is uninitialised, 
the value returned by this function is not defined. */
	{return IsPtrRep(iPtr);}
inline TBool TSwizzleCBase::IsId() const
/** Tests whether this swizzle currently represents an object as a stream id.

@return True, if this swizzle currently maintains the representation of the 
object as a stream id; False, otherwise. Note that if the swizzle is uninitialised, 
the value returned by this function is not defined. */
	{return IsIdRep(iPtr);}
inline TSwizzleCBase::TSwizzleCBase(const TAny* aPtr)
	: iPtr(aPtr)
	{
#if defined (_DEBUG)
	__DbgChkPtr(aPtr);
#endif
    }
inline const TAny* TSwizzleCBase::Ptr() const
	{
#if defined (_DEBUG)
	__DbgChkPtr(iPtr);
#endif
	return iPtr;
	}
inline TBool operator==(const TAny* aPtr,const TSwizzleCBase& aSwizzle)
	{return aSwizzle==aPtr;}
inline TBool operator!=(const TAny* aPtr,const TSwizzleCBase& aSwizzle)
	{return aSwizzle!=aPtr;}

// Class TSwizzleBase
inline TSwizzleBase::TSwizzleBase(TAny* aPtr)
	: TSwizzleCBase(aPtr)
	{}
inline TSwizzleBase::TSwizzleBase(TStreamId anId)
	: TSwizzleCBase(anId)
	{}
inline TAny* TSwizzleBase::Ptr() const
	{return (TAny*)TSwizzleCBase::Ptr();}

// Template class TSwizzle
template <class T>
inline TSwizzle<T>::TSwizzle(T* aPtr)
	: TSwizzleBase(aPtr)
/** Constructs a swizzle for a class T type object, represented by a pointer.

@param aPtr A pointer to a class T type object. */
	{}
template <class T>
inline TSwizzle<T>::TSwizzle(TStreamId anId)
	: TSwizzleBase(anId)
/** Constructs a swizzle for a class T type object, represented as a stream id.

@param anId The id of a stream containing the external representation of a 
class T type object. */
	{}
template <class T>
inline TSwizzle<T>& TSwizzle<T>::operator=(T* aPtr)
/** Sets this swizzle to represent the in-memory type T object, pointed to by the 
specified T* pointer.

@param aPtr A pointer to an object of type T which the swizzle is to represent.
@return A reference to this swizzle representing the object of type T. */
	{return *this=TSwizzle<T>(aPtr);}
template <class T>
inline T* TSwizzle<T>::AsPtr() const
/** Returns a pointer to the object which this swizzle represents.

Note that the indirect component selector operator->() can be used to access 
members of the represented object.

@return A pointer to the class T type object represented by this swizzle. */
	{return (T*)Ptr();}
template <class T>
inline TSwizzle<T>::operator T*() const
	{return AsPtr();}
template <class T>
inline T& TSwizzle<T>::operator*() const
/** Returns a reference to the type T object which this swizzle represents.

@return A reference to the type T object represented by this swizzle. */
	{return *AsPtr();}
template <class T>
inline T* TSwizzle<T>::operator->() const
/** Gives access to members of the type T object which this swizzle represents.

Note, use the AsPtr() member function to return a pointer to the object itself.

@return A pointer to the T type object; not explicitly accessible. */
	{return AsPtr();}
template <class T>
inline void TSwizzle<T>::ExternalizeL(RWriteStream& aStream) const
/** Externalises the stream id of the stream associated with this swizzle.

This function presupposes that: 

a store map, i.e. an object of type CStoreMap, has been constructed

the id of the stream containing the external representation of the represented 
type T object has been bound to this swizzle and added to the store map.

the concrete stream referenced by aStream has been constructed, specifying 
the store map as an externaliser.

The presence of this function means that the standard templated operator<<() 
can be used.

@param aStream Stream to which the stream id should be externalised.
@see CStoreMap */
	{TSwizzleBase::DoExternalizeL(aStream,TExternalizer<T>::Function());}
inline TSwizzle<TAny>::TSwizzle(TAny* aPtr)
	: TSwizzleBase(aPtr)
/** Constructs the swizzle representing the specified untyped object as a pointer.
	
@param aPtr A pointer to an untyped object which this swizzle is to represent. */
	{}
inline TSwizzle<TAny>::TSwizzle(TStreamId anId)
	: TSwizzleBase(anId)
/** Constructs the swizzle for an untyped object, represented as a stream id.
	
@param anId The id of a stream containing the external representation of the 
	untyped object which this swizzle is to represent. */
	{}
inline TSwizzle<TAny>::TSwizzle(const TSwizzleBase& aSwizzle)
	: TSwizzleBase(aSwizzle)
/** Constructs the swizzle to represent the untyped object currently represented 
	by the specified swizzle.
	
@param aSwizzle A reference to a swizzle whose representation of an object 
	is to be copied to this swizzle */
	{}
inline TSwizzle<TAny>& TSwizzle<TAny>::operator=(TAny* aPtr)
/** Sets this swizzle to represent the in-memory object, pointed to by the specified 
	pointer.
	
@param aPtr A pointer to the untyped object which the swizzle is to represent.
@return A reference to this swizzle. */
	{return *this=TSwizzle<TAny>(aPtr);}
inline TSwizzle<TAny>& TSwizzle<TAny>::operator=(const TSwizzleBase& aSwizzle)
	/** Sets the swizzle to represent the untyped object currently represented by the 
	specified swizzle.
	
	@param aSwizzle The swizzle whose representation of an object is to be copied 
	to this swizzle.
	@return A reference to this swizzle. */
	{return *this=TSwizzle<TAny>(aSwizzle);}
inline TAny* TSwizzle<TAny>::AsPtr() const
/** Returns a pointer to the untyped object that this swizzle represents.
	
	@return A pointer to the untyped object represented by this swizzle. */
	{return Ptr();}
inline TSwizzle<TAny>::operator TAny*() const
	{return AsPtr();}

// Template class TSwizzleC
template <class T>
inline TSwizzleC<T>::TSwizzleC(const T* aPtr)
	: TSwizzleCBase(aPtr)
/** Constructs a swizzle for a class T type object and represents the object by 
pointer.

@param aPtr A pointer to a class T type object. */
	{}
template <class T>
inline TSwizzleC<T>::TSwizzleC(TStreamId anId)
	: TSwizzleCBase(anId)
/** Constructs a swizzle for a class T type object and represents the object as 
a stream id.

@param anId The stream id of a stream containing the external representation 
of an object of type class T. */
	{}
template <class T>
inline TSwizzleC<T>::TSwizzleC(TSwizzle<T> aSwizzle)
	: TSwizzleCBase(aSwizzle)
/** Constructs a swizzle for a class T type object that is currently represented 
by another swizzle.

The representation of the class T type object is the same as its representation 
by the specified swizzle, i.e. if the specified swizzle represents the object 
as a pointer, then this newly constructed swizzle also represents the object 
as a pointer.

@param aSwizzle The swizzle whose representation of the class T type object 
is to be copied to this swizzle. */
	{}
template <class T>
inline TSwizzleC<T>& TSwizzleC<T>::operator=(const T* aPtr)
/** Sets this swizzle to represent the in-memory type T object, pointed to by the 
specified T* pointer.

@param aPtr A pointer to an object of type T which the swizzle is to represent.
@return A reference to this swizzle representing the object of type T.
@see TSwizzle::operator=() */
	{return *this=TSwizzleC<T>(aPtr);}
template <class T>
inline const T* TSwizzleC<T>::AsPtr() const
/** Returns a constant pointer to the object which this swizzle represents.

Note:

The in memory object cannot be changed through this swizzle.

To access members of the object, the indirect component selector operator->() 
can be used.

@return A const pointer to the class T type object represented by this swizzle. */
	{return (const T*)Ptr();}
template <class T>
inline TSwizzleC<T>::operator const T*() const
	{return AsPtr();}
template <class T>
inline const T& TSwizzleC<T>::operator*() const
/** Returns a const reference to the type T object which this swizzle represents.

@return A const reference to the type T object represented by this swizzle.
@see TSwizzle::operator*() */
	{return *AsPtr();}
template <class T>
inline const T* TSwizzleC<T>::operator->() const
/** Gives access to members of the type T object which this swizzle represents.

Note:

use the AsPtr() member function to return a pointer to the object itself.

the type T object cannot be changed through this operator.

@return A const pointer to the T type object; not explicitly accessible.
@see TSwizzle::operator->() */
	{return AsPtr();}
template <class T>
inline void TSwizzleC<T>::ExternalizeL(RWriteStream& aStream) const
/** Externalises the stream id of the stream associated with this swizzle.

This function presupposes that: 

a store map, i.e. an object of type CStoreMap, has been constructed

the id of the stream containing the external representation of the <class T> 
object has been bound to this swizzle and added to the store map.

the concrete stream referenced by aStream has been constructed, specifying 
the store map as an externalizer.

The presence of this function means that the standard templated operator<<() 
can be used.

@param aStream Stream to which the stream id should be externalised
@see CStoreMap */
	{TSwizzleCBase::DoExternalizeL(aStream,TExternalizer<T>::Function());}
inline TSwizzleC<TAny>::TSwizzleC(const TAny* aPtr)
	: TSwizzleCBase(aPtr)
/** Constructs the swizzle representing the specified untyped object as a pointer.
	
@param aPtr A pointer to an untyped object which this swizzle is to represent. */
	{}
inline TSwizzleC<TAny>::TSwizzleC(TStreamId anId)
	: TSwizzleCBase(anId)
/** Constructs the swizzle for an untyped object, represented as a stream id.
	
@param anId The id of a stream containing the external representation of the 
	untyped object which this swizzle is to represent. */
	{}
inline TSwizzleC<TAny>::TSwizzleC(const TSwizzleCBase& aSwizzle)
	: TSwizzleCBase(aSwizzle)
/** Constructs the swizzle to represent the untyped object currently represented 
	by the specified swizzle.
	
@param aSwizzle The swizzle whose representation of an object is to be copied 
	to this swizzle. */
	{}
inline TSwizzleC<TAny>::TSwizzleC(TStreamRef aRef)
	: TSwizzleCBase(aRef)
/** Constructs the swizzle from a stream reference.
	
@param aRef The stream reference. */
 	{}
inline TSwizzleC<TAny>& TSwizzleC<TAny>::operator=(const TAny* aPtr)
/** Sets this swizzle to represent the specified in-memory untyped object.
	
@param aPtr A pointer to the untyped object that the swizzle is to represent.
@return A reference to this swizzle representing the untyped object. */
	{return *this=TSwizzleC<TAny>(aPtr);}
inline TSwizzleC<TAny>& TSwizzleC<TAny>::operator=(const TSwizzleCBase& aSwizzle)
/** Sets this swizzle to represent the untyped object currently represented by the 
	specific swizzle.
	
@param aSwizzle A pointer to the untyped object that the swizzle is to represent.
@return A reference to a swizzle whose representation of an object is to be 
	copied to this swizzle. */
	{return *this=TSwizzleC<TAny>(aSwizzle);}
inline const TAny* TSwizzleC<TAny>::AsPtr() const
/** Gets a constant pointer to the untyped object that this swizzle represents.
	
@return A const pointer to the untyped object represented by this swizzle. */
	{return Ptr();}
inline TSwizzleC<TAny>::operator const TAny*() const
	{return AsPtr();}

