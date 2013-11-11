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

/**
 Returns the start position of the write area within the 
 intermediate buffer taking into account seeks.
 
 @param The enumerator indicates we are setting a write buffer.
 
*/
inline TUint8* RFileBuf::Limit(TWrite) const
	{
	return iWLim;
	}
/** Sets the start position of the write area within the 
 intermediate buffer taking into account seeks.

@param The enumerator indicates we are setting a write buffer.
@param aPtr The start point to set the limit to.
 */	
inline void RFileBuf::SetLimit(TWrite,TUint8* aLimit)
	{
	iWLim=aLimit;
	}
/** Sets the read position of the stream to the specified offset within the file. 

@param The enumerator indicates we are setting a read buffer.
@param aPos The offset within the file to which the read stream position
is set. By default this is zero.
@see MStreamBuf::TRead */
inline void RFileBuf::SetPos(TRead,TInt aPos)
	{
	iRPos=aPos;
	}
/** Sets the write position of the stream to the specified offset within the file.

@param The enumerator indicates we are setting a write buffer.
@param aPos The offset within the file to which the write stream position
is set. By default this is zero.
@see MStreamBuf::TWrite */
inline void RFileBuf::SetPos(TWrite,TInt aPos)
	{
	iWPos=aPos;
	}
/** Returns the read stream position.

@param The enumerator indicates we are setting a read buffer.
@see MStreamBuf::TRead */
inline TInt RFileBuf::Pos(TRead) const
	{
	return iRPos;
	}
/** Returns the write stream position.

@param The enumerator indicates we are setting a write buffer.
@see MStreamBuf::TWrite */
inline TInt RFileBuf::Pos(TWrite) const
	{
	return iWPos;
	}
/** Moves the file offset to which the read stream position is set.

@param The enumerator indicates we are setting a read buffer.
@param The number of bytes to move by.
@see MStreamBuf::TRead */
inline TInt RFileBuf::MovePos(TRead,TInt anOffset)
	{
	return iRPos+=anOffset;
	}
/** Moves the file offset to which the write stream position is set.

@param The enumerator indicates we are setting a write buffer.
@param The number of bytes to move by.
@see MStreamBuf::TWrite */
inline TInt RFileBuf::MovePos(TWrite,TInt anOffset)
	{
	return iWPos+=anOffset;
	}
/** Returns the number of bytes that are/were in the read area of the 
 intermediate buffer yet to be passed to the user. 

@param The enumerator indicates we are setting a read buffer.
@see MStreamBuf::TRead */	
inline TInt RFileBuf::Lag(TRead) const
	{
	return Ptr(ERead)-End(ERead);
	}
/** Returns the number of bytes that are/were in the write area of the 
 intermediate buffer yet to be written to the file.

@param The enumerator indicates we are setting a write buffer.
@see MStreamBuf::TWrite */	
inline TInt RFileBuf::Lag(TWrite) const
	{
	return Ptr(EWrite)-iBase;
	}
/** Returns the number of bytes of outstanding data in the intermediate 
 buffer that is yet to be written, taking into account any changes brought
 about by seeks.

@param The enumerator indicates we are using a write buffer.
@see MStreamBuf::TWrite */	
inline TInt RFileBuf::Span(TWrite) const
	{
	return Max(Ptr(EWrite),Limit(EWrite))-iBase;
	}
/** Gets the the read position within the stream taking 
 into account the data in the intermediate buffer not yet passed up
 to the user.

@param The enumerator indicates we are using a read buffer.
@see MStreamBuf::TRead */	
inline TInt RFileBuf::Mark(TRead) const
	{
	return Pos(ERead)+Lag(ERead);
	}
/** Gets the write position within the stream taking 
 into account data in the intermediate buffer not yet written.

@param The enumerator indicates we are using a write buffer.
@see MStreamBuf::TWrite */	
inline TInt RFileBuf::Mark(TWrite) const
	{
	return Pos(EWrite)+Lag(EWrite);
	}
/** Returns the possible end position of the write position in the stream 
 should the current data in the intermediate buffer be written.

@param The enumerator indicates we are using a write buffer.
@see MStreamBuf::TWrite */	
inline TInt RFileBuf::Reach(TWrite) const
	{
	return Pos(EWrite)+Span(EWrite);
	}
/** Sets the start and end points of the read area within the intermediate buffer.

@param The enumerator indicates we are setting a read buffer.
@param aPtr The start point.
@param anEnd The end point.
@see MStreamBuf::TRead 
@see TStreamBuf::SetBuf 
 */	
inline void RFileBuf::SetBuf(TRead,TUint8* aPtr,TUint8* anEnd)
	{
	TStreamBuf::SetBuf(ERead,aPtr,anEnd);
	}
/** Sets the start and end points of the read area within the intermediate buffer.

@param The enumerator indicates we are setting a write buffer.
@param aPtr The start point.
@param anEnd The end point.
@see MStreamBuf::TWrite 
@see TStreamBuf::SetBuf 
 */	
inline void RFileBuf::SetBuf(TWrite,TUint8* aPtr,TUint8* anEnd)
	{
	TStreamBuf::SetBuf(EWrite,aPtr,anEnd);
	SetLimit(EWrite,aPtr);
	}
/** Sets the start and end points of the read area within the intermediate buffer.

@param The enumerator indicates we are setting a read buffer. ???
@param aPtr The start point.
@param anEnd The end point.
@see MStreamBuf::TArea 
@see TStreamBuf::SetBuf 
 */	
inline void RFileBuf::SetBuf(TArea anArea,TUint8* aPtr,TUint8* anEnd)
	{
	TStreamBuf::SetBuf(anArea,aPtr,anEnd);
	if (anArea&EWrite) {
		SetLimit(EWrite,aPtr);
	}
	}

/** Returns a pointer to the CPermanentStoreCoord object.

@see CPermanentStoreCoord */	
inline CPermanentStoreCoord& CPermanentFileStore::Coord() const
	{
	__ASSERT_DEBUG(iCoord!=NULL,User::Invariant());
	return *iCoord;
	}

