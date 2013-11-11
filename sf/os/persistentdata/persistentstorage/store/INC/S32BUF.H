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

#if !defined(__S32BUF_H__)
#define __S32BUF_H__
#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

/** Defines the type of location within a stream on which the calculation 
of a new seek position is based.

The type is used by the stream buffer SeekL() functions.

@see MStreamBuf::SeekL() */
enum TStreamLocation 
	/** The seek position is calculated relative to the beginning of the 
	stream.*/
	{EStreamBeginning,
	/** The seek position is calculated relative to the end of the stream.*/
	EStreamMark,
	/** The seek position is calculated relative to the existing read or 
	write mark in the stream. */
	EStreamEnd};

/**
 * @publishedAll 
 * @released
 * Holds the position of the read or write mark within a seekable stream.

Position is the offset from the beginning of a seekable stream. The class 
provides convenient operators for performing arithmetic on the offset value 
and for doing comparisons between stream position objects.

It can be considered as an absolute stream position.

Objects of this type are usually created as a result of calling 
MStreamBuf::SeekL() or MStreamBuf::TellL(); they allow a program to remember 
the current read position in a stream buffer and reset it to the same 
location later.

@see MStreamBuf::SeekL()
@see MStreamBuf::TellL()  
*/
class TStreamPos
	{
public:
	/** Constructs an empty stream position object. */
	TStreamPos() {}
	inline TStreamPos(TInt anOffset);
//
	inline TBool operator==(TStreamPos aPos) const;
	inline TBool operator!=(TStreamPos aPos) const;
	inline TBool operator<(TStreamPos aPos) const;
	inline TBool operator<=(TStreamPos aPos) const;
	inline TBool operator>(TStreamPos aPos) const;
	inline TBool operator>=(TStreamPos aPos) const;
//
	inline TInt operator-(TStreamPos aPos) const;
	inline TStreamPos operator+(TInt anOffset) const;
	inline TStreamPos operator-(TInt anOffset) const;
//
	inline TStreamPos& operator+=(TInt anOffset);
	inline TStreamPos& operator-=(TInt anOffset);
//
	inline TInt Offset() const;
private:
	TInt iOff;
	};
inline TStreamPos operator+(TInt anOffset,TStreamPos aPos);
#if defined(__NO_CLASS_CONSTS__)
/** Constructs a TStreamPos object that denotes the beginning of any seekable 
stream.

@see TStreamPos */
#define KStreamBeginning TStreamPos(0)
#else
const TStreamPos KStreamBeginning=TStreamPos(0);
#endif

/**
 * @publishedAll 
 * @released
 * Stream transfer object.

Holds and maintains a value that represents how much data is to be transferred, 
or remains to be transferred, between streams.

Objects of this type are used by ReadL() and WriteL() functions of MStreamBuf.

@see MStreamBuf
@see TStreamBuf
@see TStreamMark
@see TStreamExchange
@see RShareBuf  
*/
class TStreamTransfer
	{
public:
	/** An enumerator type passed to a constructor of this class to indicate 
	that there is no explicit limit to the amount of data that can be 
	transferred between streams. The enumeration is not used. */
	enum TUnlimited {EUnlimited};
public:
	/** Constructs a stream transfer object specifying that there is no 
	explicit limit to the amount of data that can be transferred between 
	streams.
	
	The amount of data to be transferred is only limited by the streams 
	themselves.
	
	The arithmetical operators do not change the state of an unlimited stream 
	transfer object. */
	TStreamTransfer() {}
	inline TStreamTransfer(TInt aMaxLength);
	inline TStreamTransfer(TUnlimited);
//
	inline TBool operator==(TInt aLength) const;
	inline TBool operator>(TInt aLength) const;
	inline TStreamTransfer operator-(TInt aLength) const;
	inline TInt operator[](TInt aMaxLength) const;
//
	inline TStreamTransfer& operator-=(TInt aLength);
//
	inline TInt Left() const;
private:
	TInt iVal;
private:
	IMPORT_C static void __DbgChkNonNegative(TInt aLength);
	};
inline TBool operator==(TInt aLength,TStreamTransfer aTransfer);
inline TBool operator<(TInt aLength,TStreamTransfer aTransfer);
#if defined(__NO_CLASS_CONSTS__)
/** Constructs a TStreamTransfer object indicating that no explicit limit is 
imposed on transfers between streams.

@see TStreamTransfer
@see MStreamBuf::ReadL()
@see MStreamBuf::WriteL() */
#define KStreamUnlimited TStreamTransfer(TStreamTransfer::EUnlimited)
#else
const TStreamTransfer KStreamUnlimited=TStreamTransfer::EUnlimited;
#endif
//
class MStreamInput;
class MStreamOutput;

/**
 * @publishedAll 
 * @released
 * A stream buffer that provides a generic I/O interface for streamed data.

A stream buffer:

may be buffered or unbuffered

may provide read-only, write-only or read/write capability

may be seekable, or unseekable.

A seekable stream buffer maintains independent read and write pointers, which 
can be manipulated separately. This is unlike the RFile interface which 
maintains a single current position in the file. For example, file streams 
and memory streams are seekable streams, but socket, serial-comms, and filtered 
streams are not.

Objects of this type are used by the stream interface classes derived from 
RReadStream and RWriteStream.

The class has no member data.

Derive from this class, if the stream has no intermediate buffering 
capabilities, otherwise derive from TStreamBuf.

Get a reference to the stream buffer used by a read stream by calling 
RReadStream::Source(). Get a reference to the stream buffer used by a write 
stream by calling RWriteStream::Sink()

@see RReadStream
@see RWriteStream
@see TStreamBuf 
*/
class MStreamBuf
	{
public:
	/** Indicates that an operation applies to the read mark in a stream or to 
	the read area in an stream buffer. */
	enum TRead {ERead=0x01};

	/** Indicates that an operation applies to the write mark in a stream or 
	to the write area in an stream buffer. */
	enum TWrite {EWrite=0x02};

	/** Used to identify the type of mark in a stream.

	The type is used by functions of this class and derived classes that perform 
	seek operations to marks within a stream.

	The type uses the ERead and EWrite enumeration values, as bit flags, to 
	identify the read and write marks respectively.

	ERead is an MStreamBuf::TRead enumerator. EWrite is an MStreamBuf::EWrite 
	enumerator.

	@see MStreamBuf::TRead
	@see MStreamBuf::TWrite */
	typedef TInt TMark;
public:
	IMPORT_C void Close();
	inline void Release();
	IMPORT_C TInt Synch();
	inline void SynchL();
//
	IMPORT_C void PushL();
//
	inline TInt ReadL(TAny* aPtr,TInt aMaxLength);
	IMPORT_C TInt Read(TDes8& aDes,TRequestStatus& aStatus);
	IMPORT_C TInt Read(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	IMPORT_C TInt ReadL(TDes8& aDes,TRequestStatus& aStatus);
	inline TInt ReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	inline TStreamTransfer ReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	IMPORT_C TInt ReadL(MStreamInput& anInput,TInt aMaxLength);
	inline void ReadL(MStreamInput& anInput);
//
	inline void WriteL(const TAny* aPtr,TInt aLength);
	IMPORT_C TInt Write(const TDesC8& aDes,TRequestStatus& aStatus);
	IMPORT_C TInt Write(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	IMPORT_C TInt WriteL(const TDesC8& aDes,TRequestStatus& aStatus);
	inline TInt WriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	inline TStreamTransfer WriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
	IMPORT_C TInt WriteL(MStreamOutput& anOutput,TInt aMaxLength);
	inline void WriteL(MStreamOutput& anOutput);
//
	inline void SeekL(TMark aMark,TStreamPos aPos);
	inline TStreamPos SeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset=0);
	inline TStreamPos SeekL(TRead,TStreamLocation aLocation,TInt anOffset=0);
	inline TStreamPos SeekL(TWrite,TStreamLocation aLocation,TInt anOffset=0);
	inline TStreamPos SeekL(TRead,TInt anOffset);
	inline TStreamPos SeekL(TWrite,TInt anOffset);
//
	inline TStreamPos TellL(TRead) const;
	inline TStreamPos TellL(TWrite) const;
	inline TInt SizeL() const;
protected:
	MStreamBuf() {}
private:
	MStreamBuf(const MStreamBuf&);
	MStreamBuf& operator=(const MStreamBuf&);
//
	virtual IMPORT_C void DoRelease();
	virtual IMPORT_C void DoSynchL();
	virtual IMPORT_C TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	virtual IMPORT_C TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	virtual IMPORT_C TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	virtual IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
	virtual IMPORT_C TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	virtual IMPORT_C TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
	virtual IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
	};

/**
 * @publishedAll 
 * @released
 * An interface to an object that acts as a target for read operations from 
a stream. The object behaves as a generic data sink.

A stream input object can act as an intelligent buffer, and is useful for 
performing filtering, compression or any other general kind of conversion 
operation that might be needed after reading from a stream.

The class is pure interface and requires an implementation.

@see MStreamBuf::ReadL()  
*/
class MStreamInput
	{
public:
	/** Reads data from an intermediate buffer into this stream input object.
	
	This function is called by the default implementation of 
	TStreamBuf::DoReadL(MStreamInput&,TStreamTransfer) 
	and assumes that the source is a stream buffer's intermediate buffer.
	
	@param aPtr A pointer into the intermediate buffer from which the read 
	operation starts.
	@param aMaxLength The maximum amount of data to be read.
	@return The amount of data read.
	@see TStreamBuf::DoReadL()
	@see TStreamBuf */
	virtual TInt PushL(const TAny* aPtr,TInt aMaxLength)=0;

	/** Reads data from the specified stream into this stream input object.
	
	This function is called by the default implementation of
	MStreamBuf::DoReadL(MStreamInput&,TStreamTransfer). 
	It may also be called by TStreamBuf::DoReadL(MStreamInput&,TStreamTransfer), 
	depending on the amount of data to be transferred and the nature of the 
	buffering scheme.
	
	@param aSource The stream from which data is to be read.
	@param aTransfer Defines the amount of data available to be read.
	@return The amount of data that was not consumed.
	@see MStreamBuf::DoReadL()
	@see TStreamBuf::DoReadL() */
	virtual	TStreamTransfer ReadFromL(MStreamBuf& aSource,TStreamTransfer aTransfer)=0;
	};

/**
 * @publishedAll 
 * @released
 * An interface to an object that acts as source for write operations to a 
stream. The object behaves as a generic data source.

A stream output object can act as an intelligent buffer, and is useful for 
performing filtering, compression or any other general kind of conversion 
operation that might be needed before writing to a stream.

The class is pure interface and requires an implementation.

@see MStreamBuf::WriteL()  
*/
class MStreamOutput
	{
public:
	/** Writes data to an intermediate buffer from this stream output object.
	
	This function is called by the default implementation of 
	TStreamBuf::DoWriteL(MStreamOutput&,TStreamTransfer) 
	and assumes that the target is a stream buffer's intermediate buffer.
	
	@param aPtr A pointer into the intermediate buffer where the write operation 
	starts.
	@param aMaxLength The maximum amount of data to be written.
	@return The amount of data written.
	@see TStreamBuf::DoWriteL()
	@see TStreamBuf */
	virtual TInt PullL(TAny* aPtr,TInt aMaxLength)=0;

	/** Writes data to the specified stream from this stream output object.
	
	This function is called by the default implementation of 
	MStreamBuf::DoWriteL(MStreamOutput&,TStreamTransfer). 
	It may also be called by TStreamBuf::DoWriteL(MStreamOutput&,TStreamTransfer), 
	depending on the amount of data to be transferred and the nature of the 
	buffering scheme.
	
	@param aSink The stream to which data is to be written.
	@param aTransfer Defines the amount of data available to be written.
	@return The amount of data that was not consumed.
	@see MStreamBuf::DoWriteL()
	@see TStreamBuf::DoWriteL() */
	virtual TStreamTransfer WriteToL(MStreamBuf& aSink,TStreamTransfer aTransfer)=0;
	};

/**
 * @publishedAll 
 * @released
 * Adds buffering capabilities to a stream buffer

The class provides pointers to mark out the current read and write areas within 
the intermediate buffer. The class also defines the pure virtual functions 
UnderflowL() and OverflowL() which must be provided by a derived class.

Streams which have buffering capabilities derive from this class, otherwise 
they derive from MStreamBuf.

Note that the class does not provide the buffer; this is left to the class 
derived from it. For example, the memory buffer classes use the memory area 
directly, the file buffer class allocate a heap cell to use as a buffer.

@see UnderflowL()
@see OverflowL()  
*/
class TStreamBuf : public MStreamBuf
	{
protected:
	/** Used to identify the type of area within an intermediate buffer.

	The type is used by functions of this class that set or get pointers into 
	the intermediate buffer.

	The type uses the ERead and EWrite enumeration values, as bit flags, to 
	identify the read and write areas respectively.

	ERead is an MStreamBuf::TRead enumerator. EWrite is an MStreamBuf::EWrite 
	enumerator.

	@see MStreamBuf::TRead
	@see MStreamBuf::TWrite */
	typedef TInt TArea;
protected:
	IMPORT_C TStreamBuf();
//
	IMPORT_C void SetBuf(TArea anArea,TUint8* aPtr,TUint8* anEnd);
	IMPORT_C void SetPtr(TArea anArea,TUint8* aPtr);
	IMPORT_C void SetEnd(TArea anArea,TUint8* anEnd);
	IMPORT_C TUint8* Ptr(TArea anArea) const;
	IMPORT_C TUint8* End(TArea anArea) const;
	IMPORT_C TInt Avail(TArea anArea) const;
//
	IMPORT_C TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	IMPORT_C TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
	IMPORT_C TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
//
	inline void SetBuf(TRead,TUint8* aPtr,TUint8* anEnd);
	inline void SetBuf(TWrite,TUint8* aPtr,TUint8* anEnd);
	inline void SetPtr(TRead,TUint8* aPtr);
	inline void SetPtr(TWrite,TUint8* aPtr);
	inline void SetEnd(TRead,TUint8* anEnd);
	inline void SetEnd(TWrite,TUint8* anEnd);
	inline TUint8* Ptr(TRead) const;
	inline TUint8* Ptr(TWrite) const;
	inline TUint8* End(TRead) const;
	inline TUint8* End(TWrite) const;
	inline TInt Avail(TRead) const;
	inline TInt Avail(TWrite) const;
private:
	/** Re-fills the intermediate buffer and resets the start and end points 
	of the read area.
	
	The implementation of this function depends on the way the stream itself is 
	implemented. For example, the in-memory streams have simple implementations.
	
	@param aMaxLength The maximum amount of data required for the intermediate 
	buffer.
	@return The amount of data available in the intermediate buffer. */
	virtual TInt UnderflowL(TInt aMaxLength)=0;

	/** Empties the intermediate buffer and resets the start and end points 
	of the write area.
	
	The implementation of this function depends on the way the stream itself is 
	implemented. For example, the in-memory streams have simple implementations. */
	virtual void OverflowL()=0;
private:
	TUint8* iRPtr;
	TUint8* iREnd;
	TUint8* iWPtr;
	TUint8* iWEnd;
	};

/**
 * @publishedAll 
 * @released
 * Interface to a stream filter.

A stream filter is an object that allows stream data to be filtered after 
retrieval from a host or filtered before being written to a host.

The class is abstract and a derived class must be defined an implemented.  
*/
class TStreamFilter : public MStreamBuf
	{
public:
	enum {EAttached=0x10};
protected:
	IMPORT_C TStreamFilter();
	inline void Set(MStreamBuf* aHost,TInt aMode);
	inline void Committed();
	inline TBool IsCommitted() const;
	IMPORT_C void EmitL(const TAny* aPtr,TInt aLength);
//
	IMPORT_C void DoRelease();
	IMPORT_C void DoSynchL();
	IMPORT_C TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
private:
	/** Calculates the maximum size of unfiltered data necessary to give the 
	specified amount of filtered data.
	
	@param aMaxLength The amount of filtered data required.
	@return The maximum amount of unfiltered data guaranteed not to generate 
	more than aMaxLength bytes of filtered output. */
	virtual TInt Capacity(TInt aMaxLength)=0;

	/** Performs the filtration process.
	
	@param aPtr Pointer to the output location for the filtered data.
	@param aMaxLength The maximum amount of space available for the filtered 
	data.
	@param aFrom A reference to a pointer to the unfiltered data source. This 
	pointer should be advanced as the source is consumed.
	@param anEnd Pointer to the first byte beyond the end of the unfiltered 
	data source.
	@return The size of the filtered data. */
	virtual TInt FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd)=0;
private:
	MStreamBuf* iHost;
	TInt iMode;
private:
	friend class TFilterInput;
	friend class TFilterOutput;
private:
	IMPORT_C static void __DbgChkMode(TInt aMode);
	};

#include <s32buf.inl>
#endif
