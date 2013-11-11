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
// Reads feature config file, checks it is valid, defines feature property from it and quits.
// Debug builds panic if config file could not be read or is invalid, but only after all
// processing is complete so debug and release executables still work roughly the same.
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include <e32cmn.h>
#include <e32property.h>
#include <f32file.h>
#include "featregpan.h"
#include "featregcmn.h"

static TBool IsFeatureDataValid(const TDesC8& aFeatureDes)
	{
	// size from number of entries and ranges in header must match descriptor size
	const TFeatureHeader* header = reinterpret_cast<const TFeatureHeader*>(aFeatureDes.Ptr());
	if (header->IsInvalidOrBadSize(aFeatureDes.Size()))
		{
		return EFalse;
		}

	// check feature entries are non-repeating and sorted from lowest to highest UID
	const TFeatureEntry* entry = reinterpret_cast<const TFeatureEntry*>(header + 1);
	TInt i = header->iFeatureEntryCount;
	TUint32 previousEntryUid = 0;
	if (i > 0)
		{
		previousEntryUid = entry->iUid;
		++entry;
		--i;
		}
	for (; i > 0; --i)
		{
		if (previousEntryUid >= entry->iUid)
			{
			return EFalse;
			}
		previousEntryUid = entry->iUid;
		++entry;
		}

	// check default=supported ranges are in low-high UID pairs
	const TFeatureRange* range = reinterpret_cast<const TFeatureRange*>(entry);
	for (i = header->iFeatureRangeCount; i > 0; --i)
		{
		if (range->iLowUid > range->iHighUid)
			{
			return EFalse;
			}
		++range;
		}

	return ETrue;
	}

/**
 * Reads contents of a file into aBuf, reallocating if needed
 * Afterwards aBuf may own memory even in case of failure - up to calling code to clean it up
 * @return KErrNone on success, KErrCorrupt if invalid file size, otherwise other system-wide error code
 */
static TInt ReadFeatureFileToBuf(RFs &aFs, const TDesC& aFileName, RBuf8& aBuf)
	{
	RFile inFile;
	TInt result = inFile.Open(aFs, aFileName, EFileRead|EFileShareReadersOnly|EFileStream);
	if (result != KErrNone)
		{
		return result;
		}
	TInt fileSize;
	result = inFile.Size(fileSize);
	if ((fileSize < sizeof(TFeatureHeader)) || (fileSize > RProperty::KMaxLargePropertySize))
		{
		result = KErrCorrupt;
		}
	if (result == KErrNone)
		{
 		result = aBuf.ReAlloc(fileSize);
		}
	if (result == KErrNone)
		{
		result = inFile.Read(aBuf);
		}
	inFile.Close();
	if (result == KErrNone && !IsFeatureDataValid(aBuf))
		{
		result = KErrCorrupt;
		}	
	return result;
	}

static TInt CompareFeatureEntry(const TFeatureEntry& aEntry1,const TFeatureEntry& aEntry2)
	{
	return (aEntry1.iUid-aEntry2.iUid);
	}
	
static TInt CompareFeatureRange(const TFeatureRange& aEntry1,const TFeatureRange& aEntry2)
	{
	//the comparison when both default supported range are the same
	if (aEntry1.iLowUid==aEntry2.iLowUid && aEntry1.iHighUid==aEntry2.iHighUid)
		{
		return 0;		
		}
	return (aEntry1.iLowUid<aEntry2.iLowUid?-1:1);
	}	

static void ResetRBufArray(RArray<RBuf8>& aRBuf8Array)
	{
	TInt count=aRBuf8Array.Count();
	for (TInt i=0;i<count;i++)
		{
		aRBuf8Array[i].Close();
		}
	aRBuf8Array.Reset();
	aRBuf8Array.Close();
	}

/**
This function will try to find and open all the "featreg.cfg" files from the path specified
process them in the order of increasing rom image id before copying the aggregate content
into the buffer aBuf. Validation of each of the file is also performed inside this function.
@param aFs an opened file server session
@param aPath the path to look for the featreg files
@param aBuf the buffer to contain the aggregated file content
@return KErrNoMemory if no memory, KErrCorrupt if any of the featreg.cfg files it found in the path
		is corrupt.
*/
GLDEF_C TInt ReadMultipleFeatureFileToBuf(RFs& aFs,const TDesC& aPath,RBuf8& aBuf)
	{
	CDir* featregList=NULL;
	HBufC* matchPattern=HBufC::New(aPath.Length()+KFeatregMatchPattern().Length());
	if (!matchPattern)
		{
		return KErrNoMemory;
		}
	TPtr matchPatternPtr(matchPattern->Des());		
	matchPatternPtr.Append(aPath);
	matchPatternPtr.Append(KFeatregMatchPattern);
	TInt ret=aFs.GetDir(*matchPattern,KEntryAttReadOnly | KEntryAttHidden | KEntryAttSystem | KEntryAttArchive,ESortByName,featregList);
	delete matchPattern;
	if (ret!=KErrNone)
		{
		return ret;
		}
	TInt fileCount=featregList->Count();
	//if only one file we do not need the array
	if (fileCount==1)
		{
		TFileName fullName(aPath);
		fullName.Append((*featregList)[0].iName);
		delete featregList;
		return ReadFeatureFileToBuf(aFs,fullName,aBuf);
		}
		
	//else if there are more than one file we need to open and validate each file
	//before appending each file content to the buffer array
	RArray<RBuf8> featregFileBufArray;
	for (TInt i=0;i<fileCount;i++)
		{
		TFileName fullName(aPath);
		fullName.Append((*featregList)[i].iName);
		RBuf8 fileBuffer;
		//first try to read the file into buffer and if successful append to the array
		if ((ret=ReadFeatureFileToBuf(aFs,fullName,fileBuffer))!=KErrNone || (ret=featregFileBufArray.Append(fileBuffer))!=KErrNone)
			{
			//something wrong here reset the buffer and return the error code
			fileBuffer.Close();
			delete featregList;
			ResetRBufArray(featregFileBufArray);
			return ret;
			}
		}
	//the list of files no longer needed we can delete now
	delete featregList;

	//at this stage we are sure all the featreg files are not corrupt
	//we need to read the single feature entries and the default
	//supported range before reconstructing the output buffer
	RArray<TFeatureEntry> singleArray;
	RArray<TFeatureRange> rangeArray;
	//process it in the reverse order as we read it as rom image with higher id will be mounted first
	for (TInt idx=fileCount-1;idx>=0;idx--)
		{
		const TFeatureHeader* header = reinterpret_cast<const TFeatureHeader*>(featregFileBufArray[idx].Ptr());
		const TFeatureEntry* entry=reinterpret_cast<const TFeatureEntry*>(header+1);
		const TFeatureRange* range=reinterpret_cast<const TFeatureRange*>(entry+header->iFeatureEntryCount);		
		for (TInt s=0;s<header->iFeatureEntryCount;s++)
			{
			ret=singleArray.InsertInOrder(*(reinterpret_cast<const TFeatureEntry*>(entry+s)),TLinearOrder<TFeatureEntry>(CompareFeatureEntry));
			//ignore KErrAlreadyExists as we purposely do not want duplicate entries
			if (ret!=KErrNone && ret!=KErrAlreadyExists)
				{
				singleArray.Close();
				rangeArray.Close();
				ResetRBufArray(featregFileBufArray);
				return ret;			
				}
			}
		//for range, we just insert them but ignore duplicate
		for (TInt r=0;r<header->iFeatureRangeCount;r++)
			{
			ret=rangeArray.InsertInOrder(*(reinterpret_cast<const TFeatureRange*>(range+r)),TLinearOrder<TFeatureRange>(CompareFeatureRange));
			if (ret!=KErrNone && ret!=KErrAlreadyExists)
				{
				singleArray.Close();
				rangeArray.Close();
				ResetRBufArray(featregFileBufArray);
				return ret;	
				}
			}
		}
	//now the final step is to construct the aggregate RBuf
	RBuf8 buf8;
	TInt singleCount=singleArray.Count();
	TInt rangeCount=rangeArray.Count();
	ret=buf8.Create(sizeof(TFeatureHeader)+singleCount*sizeof(TFeatureEntry)+rangeCount*sizeof(TFeatureRange));
	if (ret==KErrNone)
		{
		TFeatureHeader header={validTypePrefix,0,singleCount,rangeCount};
		buf8.Append(reinterpret_cast<const TUint8*>(&header),sizeof(TFeatureHeader));
		for (TInt s=0;s<singleCount;s++)
			{
			buf8.Append(reinterpret_cast<const TUint8*>(&(singleArray[s])),sizeof(TFeatureEntry));
			}
		for (TInt r=0;r<rangeCount;r++)
			{
			buf8.Append(reinterpret_cast<const TUint8*>(&(rangeArray[r])),sizeof(TFeatureRange));
			}	
		//transfer ownership now
		aBuf.Assign(buf8);	
		}
	//perform cleanup
	singleArray.Close();
	rangeArray.Close();
	ResetRBufArray(featregFileBufArray);	
	return ret;	
	}


/**
 * @return KErrNone (0) if successful, KErrNoMemory or KErrCorrupt if file read failed,
 * otherwise error result from calling RProperty::Define/Set
 */
#ifndef FEATREGSETUPTEST
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	// if the following RProperty is defined, look for config file on the C: private
	// data cage instead of the normal Z:. Used for testing only.
	TInt testPropertyValue;
	TInt testResult = RProperty::Get(KFeaturePropCat, KFeatRegConfigTestKey, testPropertyValue);
	const TBool useCDataCage = testResult != KErrNotFound;

	RBuf8 featureBuf;
	RFs fs;
	TInt readResult = KErrNone;
	TInt publishResult = KErrNone;
	readResult = fs.Connect();
	if (readResult == KErrNone)
		{
		if (useCDataCage)
			{
			TConfigFileName filename;
			GetSystemDrivePath(filename);
			readResult = ReadFeatureFileToBuf(fs, filename, featureBuf);
			}
		else
			{
			readResult=ReadMultipleFeatureFileToBuf(fs,KFeatregRomPrivatePath,featureBuf);
			}
		fs.Close();
		}

	// lack of memory may be temporary, so end rather than publishing invalid property now
	if (readResult != KErrNoMemory)
		{
		RProcess thisProcess;
		// sanity check that feature property category in common header equals SID of this process
		ASSERT(KFeaturePropCat == thisProcess.SecureId());
		TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);
		TSecurityPolicy writePolicy(thisProcess.SecureId());
		publishResult = RProperty::Define(KFeaturePropKey, RProperty::ELargeByteArray, readPolicy, writePolicy);

		if ((publishResult == KErrNone) || (publishResult == KErrAlreadyExists))
			{
			if (readResult == KErrNone)
				{
				publishResult = RProperty::Set(KFeaturePropCat, KFeaturePropKey, featureBuf);
				}
			else
				{
				// bad config file: set up invalid property with number of features/ranges not consistent
				// with size of property, so Feature Registry API always returns KErrCorrupt
				TFeatureHeader corruptHeader;
				corruptHeader.SetInvalid();
				TPckg<TFeatureHeader> corruptFeaturePckg(corruptHeader);
				publishResult = RProperty::Set(KFeaturePropCat, KFeaturePropKey, corruptFeaturePckg);
				}
			}

		if (readResult != KErrNone)
			{
			readResult = KErrCorrupt;
			// panic in debug mode alerts system integrators that config file is missing,
			// unreadable or invalid in this OS configuration: a serious problem
			__ASSERT_DEBUG(EFalse, Panic(EFeatRegBadConfig));
			}
		}

	featureBuf.Close();
	delete trapCleanup;
	__UHEAP_MARKEND;
	return (readResult != KErrNone) ? readResult : publishResult;
	}
#endif
