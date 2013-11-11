// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BAARCHIVEIMPL_H__
#define __BAARCHIVEIMPL_H__

#include <e32std.h>
#include <f32file.h>

//Some useful constant
const TUid KSpiFileUid={0x10205C2B};
const TUint KSpiFirstRscOffset=32;

//Forward declarations
class CResourceFile;

/**
Internal class to store the RSC name and the pointer to data
@internalComponent
*/
NONSHARABLE_CLASS(TRscEntry)
{
public:
	TRscEntry(){}
public:
	TPtrC8 iRscName;
	TPtrC8 iRscData;
#ifdef __BASPITEST__
	//used for testing purpose to check file association of each resource entry
	TPtrC iFileNamePtr;
#endif	
};

/** Accesses a resource file and reads the resource data into a buffer.
It is the implementation class for RResourceArchive
@internalComponent 
@see RResourceArchive
*/
NONSHARABLE_CLASS(CResourceArchiveImpl) : public CBase
	{
	friend class RStaticPluginInfoTest;
public:
	static CResourceArchiveImpl* NewL(RFs& aFs,const TDesC& aName);
	static CResourceArchiveImpl* NewL(RFs& aFs,const TDesC& aPath,const TDesC& aPattern);
	~CResourceArchiveImpl();
	CResourceFile* NextL(HBufC*& aRscFileName);
	void Reset();
	TUid Type();
	TBool NextResourceExist() const;
	
private:
	void ConstructL(RFs& aFs,const TDesC& aName);
	void ConstructL(RFs& aFs,const TDesC& aPath,const TDesC& aPattern);
	//helper functions
	void OpenFileL(RFs& aFs,const TDesC& aName,TPtr8& aBufferPtr);
	void ValidateHeaderL(TPtr8& aBufferPtr);
	void ProcessEntryL(TPtr8& aBufferPtr,RArray<TPtrC8>& aHiddenList);
	CResourceArchiveImpl();
	
	//determine the order in which entry is inserted	
	static TInt EntryOrder(const TRscEntry& aSource,const TRscEntry& aTarget)
		{
		return (aSource.iRscName.CompareF(aTarget.iRscName));
		}

	//determine the matching criteria for TPtrC8
	static TBool MatchDescriptor(const TPtrC8& aSource,const TPtrC8& aTarget)
		{
		if (aSource.CompareF(aTarget)==0)
			return ETrue;
		return EFalse;
		}
	TInt32 LittleEndianFourByteInteger(const TDesC8& aBuffer,TInt aIndexOfFirstByte) const;	
private:
	//current rsc entry index to iRscList
	TInt iCurrentIndex;
	//type of spi file
	TUid iSpiFileType;
	//combined array of resources
	RArray<TRscEntry> iRscList;
	//list of all opened archive buffer
	RPointerArray<HBufC8> iSpiBufferArray;
	//list of all opened RChunk buffer
	RPointerArray<RChunk> iSpiChunkBufferArray;
	
#ifdef __BASPITEST__
	//used for testing purpose to check file association of each resource entry
	RArray<TFileName> iSpiFileNameArray;
#endif	
	};
	
#endif//__BAARCHIVEIMPL_H_
