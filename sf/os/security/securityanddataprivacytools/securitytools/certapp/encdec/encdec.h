#ifndef __ENCDEC_H__
#define __ENCDEC_H__/*
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


#include <s32strm.h>
#include "filestream.h"
#include "logger.h"

/**
 * @file
 * @internalComponent
 */

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

TUint8 fromHex(TUint8 ch);
TUint32 ReadUnsignedNumber(std::string &aStr, size_t aSize);
class RDecodeReadStream
	{
public:
	// Create store based stream in binary mode
	RDecodeReadStream(CFileStore *aStore, RReadStream &aReadStream);
	// Create a file based stream  in human mode
	RDecodeReadStream(const std::string &aCertBaseName, RReadStream &aReadStream);

	void RawRead(void *aPtr, TUint32 aLength);

	void CheckName(const std::string &aExpected);
	TUint32 ReadUnsignedNumber(size_t aSize);

	// Return current token
	const std::string &Token() const;
	// Discard current token and read the next
	void ReadNextToken();
	
	// Return the token after the current token.
	// The current token is not updated.
	// This will only look ahead a single token.
	const std::string &PeakToken();

	bool HumanReadable() const;
	void Close();

	CFileStore *iStore;  // Only used for STORE based streams ie. not human readable
	std::string iCertBaseName; // Only used for file based streams ie. human readable

	RReadStream &iReadStream;
private:
	bool iHumanReadable; 
	std::string iToken;
	bool iPrefetchedTokenIsValid;
	std::string iPrefetchedToken;

	void GetToken(std::string &aToken);
	};


class REncodeWriteStream
	{
public:
	// Construct stream based on a store in binary mode
	REncodeWriteStream(CFileStore *aStore, RWriteStream &aWriteStream); // store stream
	// Construct stream based on a file in human mode
	REncodeWriteStream(const std::string &aCertBaseName, RWriteStream &aWriteStream); // file stream
	// Create a human readable log
	REncodeWriteStream(Log &aLog);

	/**
	   Write binary data to the output stream without escaping
	   it. This data is allowed to contain the NUL (0) character and
	   may be binary data for the store file or UTF-8 text for the
	   human readable config file output.
	*/
	void WriteBin(const void *aPtr, TUint32 aLength);
	/**
	   Write a C style string to the output stream without escaping
	   it. It is NOT safe to write a generic UTF-8 string via this
	   this function, because such a string may contain embedded 0
	   characters. A 7-bit ASCII string will work reliably.  This
	   function is intended for writing 7-but ASCII strings to the
	   human readable config file, and should not be used for writing
	   data to a store file.
	 */
	void WriteQuotedUtf8(const void *aStr, TUint32 aLength);
	/**
	   Write a single byte.
	 */
	void WriteByte(TUint8 aByte);
	/**
	   Write a UTF-8 string quoting backslash and double quote characters.

	   A backslash will be written as \\
	   A quote character will be written as \"

	   Note that all bytes in UTF-8 escape sequences have the top bit
	   set therefore the quoting technique used by this function will
	   not effect them.
	 */
	void WriteCStr(const void *aCstr);

	void WriteHexNumber(TUint32 aNumber);

	void WriteSpace();
	void WriteLineEnd();

	void WriteIndent();
	void IncIndent();
	void DecIndent();

	bool HumanReadable() const;
	bool &PemOut();
	bool &Verbose();
	void Close();

	bool Quiet() const;

	CFileStore *StoreObject();
	RWriteStream &StoreWriteStream();

	std::string CertFileName(TUint32 aFormat, TUint32 aCertNumber);
	
private:
	CFileStore *iStore; // Only used for STORE based streams ie. not human readable
	std::string iCertBaseName; // Only used for file based streams ie. human readable

	RWriteStream *iWriteStream; // STORE or file based stream, valid if iLogStream==0
	
	std::ostream *iLogStream;
private:
	bool iHumanReadable; 
	bool iPemOut; 
	bool iVerbose; 
	int iIndentLevel;
	};

/**
  A template which generates a class which can be
  internalised/externalised via the REncodeWriteStream and
  RDecodeReadStream templates.

  The constructor takes a C string constant which specifies the field
  name.
  
  The optional aCommentOnlyInHumanMode parameter changes operation in human
  mode only - The field will be written as a comment, and will not be
  accepted whilst reading.

  Typical use is something like this:-
  EncDecObject<TUint32> fieldCost("cost");

  Typically this template will not require specialisations to handle
  additional types.
*/
template <class T> class EncDecObject
	{
public:
	EncDecObject(const char *aName, bool aCommentOnlyInHumanMode = false)
		: iName(aName), iCommentOnlyInHumanMode(aCommentOnlyInHumanMode), iValue()
		{
		}

	const std::string &Name() const { return iName; }

	const T &Value() const { return iValue; }
	T &Value() { return iValue; }

	bool CommentOnlyInHumanMode() const { return iCommentOnlyInHumanMode; }
private:
	std::string iName;
	bool iCommentOnlyInHumanMode;
	T iValue;
	};

/**
   Class for handling Enum values
 */
struct EnumEntry
	{
	const char *iName;
	TUint32 iValue;
	};

// This class should be template by a type which standard store can
// internalise/externalise ie TUint8/TUin16/TUint32 (probably not an enum)
template<typename T>class EncDecEnum
	{
public:
	/**
	   Construct an object for handling an enum type.
	   aEnumEntries must be a pointer to a static array of EnumEntry
	   structs terminated by one with iName==0.
	 */
	EncDecEnum(const char *aName, const EnumEntry *aEnumEntries, bool aCommentOnlyInHumanMode = false);

	const std::string &Name() const { return iName; }

	const T &Value() const { return iValue; }
	T &Value() { return iValue; }

	const char *ValueName() const { return ValueToName(iValue); }

	bool CommentOnlyInHumanMode() const { return iCommentOnlyInHumanMode; }

	void SetValue(const T &aValue);
	void SetValue(const char *aName);
private:
	const char *ValueToName(const T &aValue) const;
	std::string iName;
	TUint8 iWidth;
	const EnumEntry *iEnumEntries; // Array terminated by entry with iName==0
	bool iCommentOnlyInHumanMode;
	T iValue;
	};

/*
  The EncDecContainer class manages a set of objects which inherit
  from the EncDecContainerItem base class. It can be
  internalised/externalised via the REncodeWriteStream and
  RDecodeReadStream templates.

  The constructor takes a C string constant which specifies the
  container name.

  The binary form is a 32 bit count followed by a sequence of
  EncDecContainerItem objects.

  In human readable form is a sequence of zero or more human readable
  representations of T bracketed by StartX and EndX. Where X is the
  container name.
*/
class EncDecContainerItem
	{
public:
	virtual ~EncDecContainerItem();

	// Get the type name for the container. If 0 then do not bracket item with StartType/EndType
	virtual const char *ItemType() const = 0;
	// If ItemType()!=0 then ItemName will be included after StartType
	virtual std::string ItemName() const;
	virtual void SetItemName(const std::string &aName);
	virtual void Encode(REncodeWriteStream &aWriteStream) = 0;
	virtual void Decode(RDecodeReadStream &aReadStream) = 0;
	};

typedef EncDecContainerItem *EncDecContainerItemFactoryFunc();

class EncDecContainer
	{
public:
	EncDecContainer(const char *aContainerName, EncDecContainerItemFactoryFunc *aFactory);
	~EncDecContainer();

	void push_back(EncDecContainerItem *aItem);
	const EncDecContainerItem &operator[](TUint32 aIndex) const;
	EncDecContainerItem &operator[](TUint32 aIndex);
	TUint32 size() const;
	void reset();

	void Encode(REncodeWriteStream &aWriteStream) const;
	void Decode(RDecodeReadStream &aReadStream);

private:
	std::string iName;
	EncDecContainerItemFactoryFunc *iFactory;
	std::vector<EncDecContainerItem *> iArray;
	};

void readContainer(const std::string &aFileName, bool aHuman, EncDecContainer &container);
void writeContainer(const char *aFileName, bool aHuman, bool aPemOut, bool aVerbose, const EncDecContainer &container);

/*
  The EncodeHuman template functions are used to convert a type to
  human readable form.

  Do NOT try and write specialisations of these templates, it probably
  will not work, instead just write a conventional function which is
  selected via the norml overloading rules. See GOTW articles on the
  web.
 */

// The basic EncodeHuman template assumes that T is an unsigned
// integer and encodes it in hex.
template <class T> void EncodeHuman(REncodeWriteStream& aStream,const T &aUnsignedIntType)
{
	aStream.WriteHexNumber(aUnsignedIntType);
}

void EncodeHuman(REncodeWriteStream& aStream,const TUid &aUid);
void EncodeHuman(REncodeWriteStream& aStream,const TName &aName);

/*
  The DecodeHuman template functions are used to read in the human
  readable form.

  Do NOT try and write specialisations of these templates, it probably
  will not work, instead just write a conventional function which is
  selected via the norml overloading rules. See GOTW articles on the
  web.
 */

// The basic DecodeHuman template assumes that T is an unsigned integer
// and decodes it from either decimal or hex (starting with 0x).  The
// code calls RDecodeReadStream::ReadUnsignedNumber which will decode
// the number (max 32bits) and check it fits into specified type.
template <class T> void DecodeHuman(RDecodeReadStream& aStream,T &aUnsignedIntType)
{
	aUnsignedIntType = (T) aStream.ReadUnsignedNumber(sizeof(aUnsignedIntType));
}

void DecodeHuman(RDecodeReadStream& aStream,TUid &aUid);
void DecodeHuman(RDecodeReadStream& aStream,TName &aName);

/*
  The following two template operators require the object which is
  being internalised or externalised to provide a const Name function
  (which returns the field name) and two Value functions (one const
  and one not) which return a reference to an instance of the type
  being handled. A function called CommentOnlyInHumanMode should
  return true if the human output should be prefixed with # and should
  be reject when reading.

  Typicaly types will be wrapped by the EncDecObject template to
  provide the Name() and Value() functions required by these
  templates.

  Do NOT try and write specialisations of these templates, it probably
  will not work, instead just write a conventional function which is
  selected via the norml overloading rules. See GOTW articles on the
  web.

  Note: You probably only need to enhance the EncodeHuman/DecodeHuman
  functions unless you are adding a new variable length container type.
 */

/*
  The externalise operator << first checks if the destination stream
  is HumanReadable. If it is, it writes the Name(), followed by a
  space, calls EncodeHuman, then WriteLineEnd. If the stream is not
  HumanReadable it simply applies the << operator to the Value().
*/
template <class T>
inline REncodeWriteStream& operator<<(REncodeWriteStream& aStream,const T& anObject);

REncodeWriteStream& operator<<(REncodeWriteStream& aStream, const EncDecContainer &aContainer);

template <typename T>
REncodeWriteStream& operator<<(REncodeWriteStream& aStream, const EncDecEnum<T> &aEncDecEnum);

/*
  The internalise operator >> first checks if the source stream is
  HumanReadable. If it is, it reads/checks the field name then calls
  DecodeHuman. If the stream is not HumanReadable
  it simply applies the >> operator to the Value().
*/
template <class T>
inline RDecodeReadStream& operator>>(RDecodeReadStream& aStream,T& anObject);

RDecodeReadStream& operator>>(RDecodeReadStream& aStream,EncDecContainer &aContainer);

template <typename T>
RDecodeReadStream& operator>>(RDecodeReadStream& aStream, EncDecEnum<T> &aEncDecEnum);

#include "encdec.inl"



#endif
