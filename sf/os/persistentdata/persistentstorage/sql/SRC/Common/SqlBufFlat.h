// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLBUFFLAT_H__
#define __SQLBUFFLAT_H__

#include <e32base.h>
#include "SqlAssert.h"

//Forward declarations
class TSqlBufRIterator;
class CSqlSecurityPolicy;
class TSqlSecurityPolicyIterator;

#ifdef _DEBUG
const TUint64 KSqlBufFlatMagicValue = 0xAA11BB44221199EEULL;//TInt64 for 8-byte alignment
#define SQLFLATBUF_INVARIANT() Invariant()
#else
#define SQLFLATBUF_INVARIANT() void(0)
#endif

/**
RSqlBufFlat class manages a flat memory buffer with fixed elements count.
Each element (or field) of the flat buffer has a type and variable length data and is accessible using an
index. The data may be NULL.
The RSqlBufFlat objects are used for sending/receiving data between client dlls and servers, 
because once the buffer filled, a pointer to the flat buffer data can be used to send the whole buffer in a single
IPC call, which is more effective than using a stream like transfer.

RSqlBufFlat public functions are not very convenient for storing/retrieving values based on the column type 
and there are two additional classes which may be used instead of working with the flat buffer directly: 
TSqlBufRIterator and TSqlBufWIterator.

The physical structure of the flat buffer is:
---------------------------
| SysData | Header | Data |
---------------------------
- "SysData" has fixed length and contains fields with about the buffer used size, elements count, header size;
- "Header"  has fixed length, which depends of the number of elements and contains cells, one per element.
            Each cell contains information about: element type, element data length, the start position of the 
            element data in the flat buffer, "present"/"not present" flag. "Not present" means that no memory
            is reserved for the element data, but the buffer "knows" what is the element type and element data length;
            (In the current design there is no "Present"/"Not present flag". If the data position is 0, that indicates
            the element is "Not present"). See RSqlBufFlat::TCell comments for details.
- "Data"    Is the dynamic part of the buffer which may grow during the operations with the flat buffer (setting
            element data);
            
"Present"/"Not present" attribute has a key role in the large data transfers between the client and the server.
In order to optimize memory usage, usually the client specifies what can be the max field size. The server
knows that and when it fills a particular flat buffer, which has to be sent to the client, it puts in the
flat buffer only the pieces of data whose size is less than the specified "large data" size. Any data piece whose
size is larger will be set as "Not present" in the flat buffer. The "Not present" field will have all the
attributes, like type and size, but won't have any data or any allocated flat buffer memory.
So the client, when it receives the flat buffer, will know the field type and size and if it wants to get the field data,
will have to make an additional call to the server. 

Note that the fields in the buffer are 8-byte aligned.

Typical examples how the buffer can be used:

Case 1 - a dll client wants to receive from the server a flat buffer data. 

@code
RSqlBufFlat bufFlat;
bufFlat.SetCount(N);								//N is the field count
....
bufFlat.Reset();									//Clears the content of the buffer without shrinking it.
ipcArgs.Set(0, bufFlat.MaxSize());					//Tell the server what is the flat buffer max size
ipcArgs.Set(1, &bufFlat.BufPtr());
TInt rc = session.SendReceive(funcNum, ipcArgs);
if(rc > KSqlClientBufOverflowCode)
	{												//the client buffer is not big enough and has to be resized
	rc = bufFlat.ReAlloc(err - KSqlClientBufOverflowCode);
	if(rc != KErrNone)
		{
		return rc;
		}
	ipcArgs.Set(0, bufFlat.MaxSize());
	ipcArgs.Set(1, &bufFlat.BufPtr());
	rc = session.SendReceive(funcNum, ipcArgs);
	}
@endcode

Case 2 - a dll client wants to send to the server a flat buffer data. 

@code
RSqlBufFlat bufFlat;
bufFlat.SetCount(N);								//N is the field count
....
TPtrC8 ptr(bufFlat.BufDes());
TInt err = session.SendReceive(funcNum, TIpcArgs(ptr.Length(), &ptr));
@endcode

Case 3 - the server wants to return to the client a flat buffer data.
@code
RSqlBufFlat bufFlat;
bufFlat.SetCount(N);								//N is the field count
....
TInt maxCliBufSize = msg.Int0();					//The max size of the client buffer
if(maxCliBufSize < bufFlat.Size())
	{
	return bufFlat.Size() + KSqlClientBufOverflowCode;//Tell the client that its buffer is too small
	}
msg.WriteL(1, bufFlat.BufDes());
@endcode

Case 4 - the server wants to receive from the client a flat buffer data.
@code
RSqlBufFlat bufFlat;
bufFlat.SetCount(N);								//N is the field count
....
TInt cliBufFlatLen = aMessage.Int0();
TInt err = bufFlat.ReAlloc(cliBufFlatLen);			//Reallocate memory for the flat buffer
if(err != KErrNone)
	{
	return err;
	}
msg.ReadL(1, bufFlat.BufPtr());
@endcode

Case 5 - the server (or the client) wants to fill the flat buffer with some data.
@code
RSqlBufFlat bufFlat;
bufFlat.SetCount(N);								//N is the field count
....
TInt err = flatBuf.SetCount(M);						//If the field count has to be changed to M
if(err != KErrNone)
	{
	return err;
	}
//use the TSqlBufWIterator iterator to fill the buffer
@endcode

@see TSqlBufRIterator
@see TSqlBufWIterator
@see RSqlBufFlat::TCell
@see RSqlBufFlat::TBufFlat

@internalComponent
*/
class RSqlBufFlat
	{
	friend class TSqlBufRIterator;
	friend class CSqlSecurityPolicy;
	friend class TSqlSecurityPolicyIterator;

public:	
	//This enum has to be in the "public" section because it is used by TCell (declared as "private" in RSqlBufFlat)
	enum 
		{
		EWidthType = 3, 					//Bit width of the "Type" field of the header cell
		EWidthLen = 29						//Bit width of the "Length" field of the header cell
		};
	//This enum has to be in the "public" section because it is used by TCell (declared as "private" in RSqlBufFlat)
	enum 
		{
		EMaxType = 1 << EWidthType, 		//Max allowed flat buffer field type
		EMaxLength = 1 << EWidthLen			//Max allowed flat buffer field length
		};

	RSqlBufFlat();
	TInt SetCount(TInt aCount);
	TInt ReAlloc(TInt aSize);
	void ResetAndMinimize();
	void Reset();
	void Close();
	inline TInt Count() const;
	inline TInt Size() const;
	inline TInt MaxSize() const;
	
	TInt SetField(TInt aIndex, TInt aType, const void* aData, TInt aDataLength);
	
	inline const TDesC8& BufDes() const;
	inline TPtr8& BufPtr();
	
private:	//Data type declarations
	/**
	TCell represents the structure of header cells.
	Each cells contains the following fields:
	- Flat buffer field type;
	- Flat buffer field length;
	- Flat buffer field data position;
	  = if 0 then the field is "Not present" (no memory is reserved for the field data, the field data is missing);
	  = if positive - the field data position in the flat buffer (counting from the beginning of the header);
	  = if negative - the field is "Not present", but memory has been reserved for it;
	  
	RSqlBufFlat class offers fast, indexed access to the header cells.
	
	@see RSqlBufFlat
	*/
	struct TCell
		{
		inline TInt Type() const {return (iBits >> RSqlBufFlat::EWidthLen) & (RSqlBufFlat::EMaxType - 1);}
		inline TInt Size() const {return (iBits & (RSqlBufFlat::EMaxLength - 1));}
		TUint32	iBits;						//element type: EWidthType bits & element length: EWidthLen bits.
		TInt32	iPos;						//element data position, relative to the beginning of the header.
											//                       Zero if the element is not present.
											//                       Negative if the element is not present but memory is reserved.
		};

	enum {EExpandSize = 256};				//iBuf min expansion size - it must be a power of 2 and 
											//                          (EExpandSize % 8) should be 0.
	/**
	TBufFlat structure represents the "SysData" part of the flat buffer (see RSqlBufFlat comments), beyond which
	begins the flat buffer header and "Data" part.

	TBufFlat structure contains information about:
	- Flat buffer elements count;
	- Flat buffer header size;
	- The size of used part of the flat buffer;

	@see RSqlBufFlat
	*/
	struct TBufFlat
		{
		TInt	iCount;			//element count
		TInt	iHeaderSize;	//buffer header size	
		TInt	iSize;			//used buffer size
		TInt	iReserved;		//maintains TBufFlat 8-byte aligned
		};
	
private:	//Method declarations
	void DoInit();
	TInt DoReAlloc(TInt aSize);
	inline TInt Available() const;
	void DoSet(TInt aIndex, TInt aType, const void* aData, TInt aDataLength);
	TInt Reserve(TInt aLength);
	void Invariant() const;
	inline const TCell* Header() const;
	inline TCell* Header();
	inline TInt SysDataSize() const;
	
private:	//Data declarations
	// IPC data
	TBufFlat*		iBuf;
	// non-IPC data
	TInt			iMaxSize;	//max buffer size (allocated memory), not part of RSqlBufFlat data (IPC - sent/received).
								//iMaxSize can't be part of the IPC data, because it may be overwritten, which leads to 
								//"hard to detect" problems. For example:
								//  - client sends to the server in/out flat buffer with max size 256 bytes (allocated);
								//  - the server has a flat buffer which max size is 512, but the used size is 200;
								//  - the client flat buffer has enough space for those 200 bytes, so the server will
								//    copy its flat buffer content to the client flat buffer. The client's flat buffer
								//    iMaxSize data member will lose its original value (256) and will have new value assigned -
								//    512. But the client flat buffer does not have 512 bytes allocated!
	mutable TPtrC8	iBufPtrC;	//it is set to point to iBuf. Not part of RSqlBufFlat data (IPC - sent/received)
	mutable TPtr8	iBufPtr;	//it is set to point to iBuf. Not part of RSqlBufFlat data (IPC - sent/received)
	};

#include "SqlBufFlat.inl"

#endif//__SQLBUFFLAT_H__
