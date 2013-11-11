#ifndef __e32base_h__
#define __e32base_h__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent
*/

#include <string>
#include <vector>
#include <stdint.h>
#define __ASSERT_ALWAYS(cond, func) if(!(cond)) func
#define __ASSERT_DEBUG(cond, func) if(!(cond)) func
#define REINTERPRET_CAST(type,val) reinterpret_cast<type>(val)
#define CleanupClosePushL(xx) 
#define _LIT(name,str)  TLIT16 name(str)
#define _LIT16(name,str)  TLIT16 name(str)
#define _LIT8(name,str)  TLIT8 name(str)
#define _L8(str) TLIT8(str)
#define _L16(str) TLIT16(str)
//#define _L(str) TLIT16(str)
#define LOCAL_C static
#define GLDEF_C
#define IMPORT_C
#define EXPORT_C
#define __UHEAP_MARK
#define __UHEAP_MARKEND
#define __LEAVE(err) FatalError()	

extern void FatalError();

typedef uint64_t TUint64;
typedef int64_t TInt64;

typedef	int	TInt32;
typedef	unsigned int TUint32;

typedef	short TInt16;
typedef	unsigned short TUint16;

typedef	signed char	TInt8;
typedef	unsigned char TUint8;

typedef	TUint32 TChar;

typedef	int TBool;

typedef void TAny;

#define _MAKE_TINT64_ZX(x)	((TInt64)((TUint32)(x)))
#define _MAKE_TUINT64_ZX(x)	((TUint64)((TUint32)(x)))
#define MAKE_TINT64(h,l)	( (_MAKE_TINT64_ZX(h)<<32) | _MAKE_TINT64_ZX(l) )
#define MAKE_TUINT64(h,l)	( (_MAKE_TUINT64_ZX(h)<<32) | _MAKE_TUINT64_ZX(l) )


#define	I64HIGH(x)			( (TUint32)((x)>>32) )
#define	I64LOW(x)			( (TUint32)(x) )
#define	I64INT(x)			( (TInt)(x) )


typedef unsigned short int TText16;
typedef TText16 TText;
typedef int TInt;
typedef unsigned int TUint;

#define __Size (sizeof(TUint)/sizeof(TUint16))
#define __Align(s) ((((s)+__Size-1)/__Size)*__Size)

/** The largest integer value which can be stored as a TCardinality type. */
const TInt KMaxCardinality=0x1fffffff;
const TInt KMaxName=0x80;

#define FALSE false
#define TRUE true
enum TFalse {
            EFalse=FALSE
            };
enum TTrue {
           ETrue=TRUE
           };

#ifndef _FOFF
#if __GNUC__ < 4
#define _FOFF(c,f)			(((TInt)&(((c *)0x1000)->f))-0x1000)
#else
#define _FOFF(c,f)			(((TInt)&(((c *)0x1000)->f))-0x1000)
// __builtin_offset will issue warning of there are any member functions...
//#define _FOFF(c,f)			__builtin_offsetof(c,f)
#endif
#endif


const TInt KMaxTInt8=0x7f;
const TInt KMinTInt8=(-128);
const TUint KMaxTUint8=0xffu;
const TInt KMaxTInt16=0x7fff;
const TInt KMinTInt16=(-32768);
const TUint KMaxTUint16=0xffffu;
const TInt KMaxTInt32=0x7fffffff;
const TInt KMinTInt32=(TInt)0x80000000;
const TUint KMaxTUint32=0xffffffffu;
const TInt KMaxTInt=0x7fffffff;
const TInt KMinTInt=(TInt)0x80000000;
const TUint KMaxTUint=0xffffffffu;
//const TInt64 KMaxTInt64 = I64LIT(0x7fffffffffffffff);
//const TInt64 KMinTInt64 = UI64LIT(0x8000000000000000);
//const TUint64 KMaxTUint64 = UI64LIT(0xffffffffffffffff);




class TSize
	{
public:
	TSize() 
		{
		iWidth=0;
		iHeight=0;
		}
	TSize(TInt aWidth, TInt aHeight)
		: iWidth(aWidth), iHeight(aHeight)
		{
		}
private:
	TInt iWidth;
	TInt iHeight;
	};

class TDesC8
	{
public:
	
	TInt Length() const
		{
			return iCurrentLength;
		}
	
	TInt Size() const
		{
			return iCurrentLength;
		}

	virtual const TUint8 *Ptr() const = 0;
	virtual const TUint8 &operator[](TInt anIndex) const = 0;

	TBool operator==(const TDesC8 &aDes) const;
	
protected:
	TDesC8();
	TDesC8(const TDesC8 &aRef);
	TDesC8(TInt aLength);
	TInt iCurrentLength;


private:
	// Assignment is not publicly available
	TDesC8 &operator=(const TDesC8 &aRhs);
	};

class TDesC16;
class TDes8 : public TDesC8
	{
public:

	void Copy(const TDesC16 &aDes);
	virtual void Append(TChar aChar) = 0;
	TInt MaxLength() const
		{
			return iMaxLength;
		}

	TInt MaxSize() const
		{
			return iMaxLength;
		}
	
	void SetLength(TInt aLength);

protected:
	TDes8();
	TDes8(const TDes8 &aRef);
	TDes8(TInt aLength,TInt aMaxLength);
	TInt iMaxLength;

private:
	// Copy construction and assignment not supported
	TDes8 &operator=(const TDes8 &aRhs);
	};

class TPtr8 : public TDes8
	{
public:
	TPtr8(TUint8 *aBuf, TInt aLength, TInt aMaxLength)
		: TDes8(aLength, aMaxLength), iPtr(aBuf)
		{
		}
	TPtr8(TUint8 *aBuf, TInt aMaxLength)
		: TDes8(0, aMaxLength), iPtr(aBuf)
		{
		}

	TPtr8(const TPtr8 &aRhs); // Points at same data
	TPtr8 &operator=(const TPtr8 &aRhs); // copies data

	virtual const TUint8 *Ptr() const;
	virtual const TUint8 &operator[](TInt anIndex) const;

	virtual void Append(TChar aChar);

protected:
	TUint8 *iPtr;
	};

class TPtrC8 : public TDesC8
	{
public:
	TPtrC8(const TUint8 *aBuf, TInt aLength)
		: TDesC8(aLength), iPtr(aBuf)
		{
		}
	TPtrC8(const TPtrC8 &aRhs); // Points at same data
	TPtrC8 &operator=(const TPtrC8 &aRhs); // copies data
	void Set(TUint8 *aBuf, TInt aLength);

	virtual const TUint8 *Ptr() const;
	virtual const TUint8 &operator[](TInt anIndex) const;

protected:
	const TUint8 *iPtr;
	};

class TDesC16
	{
public:
	TInt Length() const
		{
			return iCurrentLength;
		}
	
	TInt Size() const
		{
			return iCurrentLength * sizeof(TUint16);
		}

	virtual const TUint16 *Ptr() const = 0;
	virtual const TUint16 &operator[](TInt anIndex) const = 0;

	TBool operator==(const TDesC8 &aDes) const;

protected:
	TDesC16();
	TDesC16(const TDesC16 &aRef);
	TDesC16( TInt aLength);
	TInt iCurrentLength;

private:
	// Disable assignment
	TDesC16 &operator=(const TDesC16 &aRhs);
	};

class TDes16 : public TDesC16
	{
public:
	TInt MaxLength() const 
		{
			return iMaxLength;
		}
	TInt MaxSize() const 
		{
			return iMaxLength * sizeof(TUint16);
		}
	void SetLength(TInt aLength);

	virtual void Append(TChar aChar) = 0;

protected:
	TDes16();
	TDes16(const TDes16 &aRef);
	TDes16(TInt aLength, TInt aMaxLength);
	TInt iMaxLength;
private:
	// Disabled assignment
	TDes16 &operator=(const TDes16 &aRhs);
	};

class TPtr16 : public TDes16
	{
public:
	TPtr16(TUint16 *aBuf, TInt aLength, TInt aMaxLength)
		: TDes16(aLength, aMaxLength), iPtr(aBuf)
		{
		}
	TPtr16(TUint16 *aBuf, TInt aMaxLength)
		: TDes16(0, aMaxLength), iPtr(aBuf)
		{
		}
	TPtr16(const TPtr16 &aRhs); // Points at same data
	TPtr16 &operator=(const TPtr16 &aRhs); // copies data

	void Copy(const TDesC8 &aDes);

	virtual const TUint16 *Ptr() const;
	virtual const TUint16 &operator[](TInt anIndex) const;
	virtual void Append(TChar aChar);
	
protected:
	TUint16 *iPtr;
	};

class TPtrC16 : public TDesC16
	{
public:
	TPtrC16(const TUint16 *aBuf, TInt aLength)
		: TDesC16(aLength), iPtr(aBuf)
		{
		}
	TPtrC16(const TPtrC16 &aRhs); // Points at same data
	TPtrC16 &operator=(const TPtrC16 &aRhs); // copies data

	void Set(TUint16 *aBuf, TInt aLength);

	virtual const TUint16 *Ptr() const;

	virtual const TUint16 &operator[](TInt anIndex) const;
	
protected:
	const TUint16 *iPtr;
	};


typedef TDesC16 TDesC;

/**
Packages a modifiable pointer descriptor which represents an object of specific 
type.

The template parameter defines the type of object.

The object represented by the packaged pointer descriptor is accessible through 
the package.
*/
template <class T>
class TPckg : public TPtr8
	{
public:
	inline TPckg(const T& aRef);
	inline T& operator()();
private:
	TPckg<T>& operator=(const TPckg<T>& aRef);
	};

// Template class TPckg
template <class T>
inline TPckg<T>::TPckg(const T &aRef)
	: TPtr8((TUint8 *)&aRef,sizeof(T),sizeof(T))
/**
Constructs a packaged modifiable pointer descriptor to represent the specified 
object whose type is defined by the template parameter.

@param aRef The object to be represented by this packaged modifiable pointer 
            descriptor.
*/
	{}

template <class T>
inline T &TPckg<T>::operator()()
/**
Gets a reference to the object represented by this packaged
modifiable pointer descriptor.

@return The packaged object.
*/
	{return(*((T *)iPtr));}





// TLIT16 is an internal port class
class TLIT16
	{
public:
	TLIT16(const char *aStr);
	TLIT16(const TLIT16 &);
	virtual ~TLIT16();
	const TDesC16* operator&() const;
	operator const TDesC16&() const;
	const TDesC16& operator()() const;
private:
	TLIT16 &operator=(const TLIT16 &);
	TPtrC16 iDes;
	};

// TLIT8 is an internal port class
class TLIT8 : public TPtrC8
	{
public:
	TLIT8(const char *aStr);
	TLIT8(const TLIT8 &);
	};



template <TInt S>
class TBuf : public TPtr16
	{
public:
	inline TBuf();
	inline explicit TBuf(TInt aLength);
	inline TBuf(const TDesC& aDes);
private:
	TText iBuf[__Align(S)];
	};

template <TInt S>TBuf<S>::TBuf()
	: TPtr16(iBuf, 0, S)
{
}

template <TInt S>TBuf<S>::TBuf(TInt aLength)
	: TPtr16(iBuf, aLength, S)
{
}

template <TInt S>TBuf<S>::TBuf(const TDesC& aDes)
	: TPtr16(iBuf, aDes.Length(), S)
{
	if(aDes.Length() > S) FatalError();
	memcpy(iBuf, aDes.Ptr(), aDes.Length()*2); // *2 for 16 bit copy
}


template <TInt S>
class TBuf8 : public TPtr8
	{
public:
	inline TBuf8();
	inline explicit TBuf8(TInt aLength);
	inline TBuf8(const TDesC8& aDes);
private:
	TUint8 iBuf[__Align(S)];
	};

template <TInt S>TBuf8<S>::TBuf8()
	: TPtr8(iBuf, 0, S)
{
}

template <TInt S>TBuf8<S>::TBuf8(TInt aLength)
	: TPtr8(iBuf, aLength, S)
{
}

template <TInt S>TBuf8<S>::TBuf8(const TDesC8& aDes)
	: TPtr8(iBuf, aDes.Length(), S)
{
	if(aDes.Length() > S) FatalError();
	memcpy(iBuf, aDes.Ptr(), aDes.Length());
}


typedef TBuf<KMaxName> TName;

class TUid
	{
public:
	enum { KNullUidValue };
	
	static TUid Uid(TInt aUid)
		{TUid uid={aUid};return uid;}

	static TUid Null()
		{TUid uid={KNullUidValue};return uid;}

	TBool operator==(const TUid &aUid) const
		{
		return iUid == aUid.iUid;
		}

	TBool operator!=(const TUid &aUid) const
		{
		return iUid != aUid.iUid;
		}
	

	TInt32 iUid;
	};

const TInt KMaxCheckedUid=3;


class TUidType
	{
public:
#ifndef __KERNEL_MODE__
	IMPORT_C TUidType();
	IMPORT_C TUidType(TUid aUid1);
	IMPORT_C TUidType(TUid aUid1,TUid aUid2);
	IMPORT_C TUidType(TUid aUid1,TUid aUid2,TUid aUid3);
	IMPORT_C TBool operator==(const TUidType& aUidType) const;
	IMPORT_C TBool operator!=(const TUidType& aUidType) const;
	IMPORT_C const TUid& operator[](TInt anIndex) const;
	IMPORT_C TUid MostDerived() const;
	IMPORT_C TBool IsPresent(TUid aUid) const;
	IMPORT_C TBool IsValid() const;
private:
#endif
	TUid iUid[KMaxCheckedUid];
	};

class TCheckedUid
	{
public:
	IMPORT_C TCheckedUid();
	IMPORT_C TCheckedUid(const TUidType& aUidType);
	IMPORT_C TCheckedUid(const TDesC8& aPtr);
	IMPORT_C void Set(const TUidType& aUidType);
	IMPORT_C void Set(const TDesC8& aPtr);
	IMPORT_C TPtrC8 Des() const;
	const TUidType& UidType() const {return iType;};
protected:
	IMPORT_C TUint Check() const;
private:
	TUidType iType;
	TUint iCheck;
	};



class RArrayBase
	{
public:
	virtual ~RArrayBase() {}
	virtual void Close() = 0;
	};

template<typename T> class RArray : public RArrayBase
	{
public:
	RArray();
	explicit RArray(TInt);
	TInt Count() const;
	const T& operator[](TInt aIndex) const;
	T& operator[](TInt aIndex);
	void Remove(TInt aIndex);
	void AppendL(const T &aEntry);
	virtual void Close() {vec.clear();};
private:
	typedef std::vector<T> VectorT;
	VectorT vec;
	};

class Mem
	{
public:
	static void FillZ(TAny *aTrg, TInt aLength);
	static void Crc(TUint16& aCrc,const TAny* aPtr,TInt aLength);
	static void Crc32(TUint32& aCrc, const TAny* aPtr, TInt aLength);
	static TUint8* Move(TAny *aTrg, const TAny *aSrc, TInt aLength);
	};

void Panic(TInt aCode);


#define KNullUid TUid::Null()

class RWriteStream;
class RReadStream;
class CFileStore;
class CConsoleBase;
class RFs;
class CleanupStack
	{
public:
	static void PopAndDestroy(RWriteStream *aStream);
	static void PopAndDestroy(RReadStream *aStream);
	static void PopAndDestroy(CFileStore *aStore);
	static void PopAndDestroy(RArrayBase *aRArray);
	static void PopAndDestroy(CConsoleBase *aCons);
	static void PopAndDestroy(RFs *aFs);
	static void PushL(void *)
		{
		}
	
	
	};


class User
	{
public:
	static void LeaveIfError(TInt aError);
	static void Leave(TInt aError); // Not supported!!!!
	static void Invariant();
	static void Panic(const TDesC &aCategory, TInt aReason);
	};



template <typename T> RArray<T>::RArray()
	{
	}

template<typename T> RArray<T>::RArray(TInt)
	{
	}
	
template<typename T> TInt RArray<T>::Count() const
	{
	return vec.size();
	}


template<typename T> const T& RArray<T>::operator[](TInt aIndex) const
	{
	return vec[aIndex];
	}

template<typename T> T& RArray<T>::operator[](TInt aIndex)
	{
	return vec[aIndex];
	}


template<typename T> void RArray<T>::Remove(TInt aIndex)
	{
	typename std::vector<T>::itrator it = vec.begin();
	while(aIndex) ++it;
	vec.erase(it);
	}

template<typename T> void RArray<T>::AppendL(const T &aEntry)
	{
	vec.push_back(aEntry);
	}

#endif
