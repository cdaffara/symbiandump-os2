// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32ucmp.h>
#include "BADICTIONARYCOMPRESSION.H"
#include "BaRscImpl.h"
#include "BaAssert.h"
#include "BaCompileAssert.h"

/** TCleanupItem function.
@internalComponent
@param aFile Pointer to the RFile instance which has to be closed. */
LOCAL_C void CloseAndDeleteFile(TAny* aFile)
	{
	RFile* const file=STATIC_CAST(RFile*,aFile);
	if (file!=NULL)
		{
		file->Close();
		delete file;
		}
	}

/** TCleanupItem function.
@internalComponent
@param aArrayOfDictionaryCompressionBitStreams Pointer to an array (RArray) of 
RDictionaryCompressionBitStream objects which have to be closed. */
LOCAL_C void CloseArrayOfDictionaryCompressionBitStreams(TAny* aArray)
	{
	typedef RArray<RDictionaryCompressionBitStream> RDictComprBitStream;
	RDictComprBitStream* array = static_cast <RDictComprBitStream*> (aArray);
	for (TInt i=array->Count()-1;i>=0;--i)
		{
		array[i].Close();
		}
	array->Close();
	}

/** @internalComponent */
struct SDictionaryCompressionData
	{
 	inline SDictionaryCompressionData() :
		iStartOfDictionaryData(0),
		iStartOfDictionaryIndex(0),
		iNumberOfDictionaryEntries(0),
		iStartOfResourceData(0),
		iStartOfResourceIndex(0),
		iNumberOfBitsUsedForDictionaryTokens(0), 
		iCachedDictionaryIndex(0),
		iCachedResourceBuffer(0)// = NULL;
		{
		}
	TInt iStartOfDictionaryData;
	TInt iStartOfDictionaryIndex;
	TInt iNumberOfDictionaryEntries;
	TInt iStartOfResourceData;
	TInt iStartOfResourceIndex;
	TInt iNumberOfBitsUsedForDictionaryTokens;
	TUint16* iCachedDictionaryIndex;
	TUint8* iCachedResourceBuffer;
	
	};

/** @internalComponent */
class RResourceFileImpl::TExtra
	{
public:
	TExtra(const TBaAssert& aAssertObj);
	~TExtra();
	TBool ContainsCompressedUnicodeL(TInt aRscIdx, TBool aFirstRscIsGen) const;
private:
	TExtra(const TExtra&);//prevent default copy constructor
	TExtra& operator=(const TExtra&);//prevent default "=" operator
public:
	TUidType iUidType;
	HBufC8* iBitArrayOfResourcesContainingCompressedUnicode; // an array of bits, one for each resource in the resource file
	SDictionaryCompressionData iDictionaryCompressionData;
	TInt iFileOffset;	// offset of RSC chunk in new AIF file format
	TBaAssert iAssertObj;
	TInt iFileSize;		// RSC chunc size - in new AIF file format, file size otherwise
	};

/** @internalComponent
@param aAssertObj Object which will determine RResourceFileImpl::TExtra
instance's behaviour if something goes wrong - depending on aAssertObj state
RResourceFileImpl::TExtra methods will leave or panic. */
RResourceFileImpl::TExtra::TExtra(const TBaAssert& aAssertObj) :
	iUidType(),
	iBitArrayOfResourcesContainingCompressedUnicode(NULL),
	iDictionaryCompressionData(),
	iFileOffset(0),
	iAssertObj(aAssertObj),
	iFileSize(0)
	{
	}

/** @internalComponent */
RResourceFileImpl::TExtra::~TExtra()
	{
	delete iBitArrayOfResourcesContainingCompressedUnicode;
	}

/** @internalComponent */
TBool RResourceFileImpl::TExtra::ContainsCompressedUnicodeL(TInt aRscIdx, TBool aFirstRscIsGen) const
	{
	if (aFirstRscIsGen)
		{
		// dictionary-compressed resource files can have an automatically generated 
		//resource which is the bit-array of resources containing compressed Unicode 
		//(this automatically generated resource does not have a corresponding bit 
		//for itself in the bit-array as it would be self-referring...)
		--aRscIdx; 
		if (aRscIdx<0)
			{
			//aRscIdx is referring to the automatically generated resource 
			//(which is the bit-array of resources containing compressed Unicode)
			return EFalse; 
			}
		}
	iAssertObj.AssertDebL(aRscIdx>=0, EBafPanicNegativeResourceIndex1);
	if (iBitArrayOfResourcesContainingCompressedUnicode==NULL)
		{
		return EFalse;
		}
	TInt index = aRscIdx / 8;
	iAssertObj.AssertDebL(index < iBitArrayOfResourcesContainingCompressedUnicode->Length(), EBafPanicBadIndex);
	return (*iBitArrayOfResourcesContainingCompressedUnicode)[index]&(1<<(aRscIdx%8));
	}

/** @internalComponent */
RResourceFileImpl::RResourceFileImpl() :
	iFile(NULL),
	iSizeOfLargestResourceWhenCompletelyUncompressed(0),
	iIndex(NULL),
	iOffset(0),
	iExtra(NULL),
	iFlagsAndNumberOfResources(0)
	{
	//Fixed class size - because of the BC reasons.
	//RResourceFileImpl size must be the same as RResourceFile size.
	enum
		{
		KRscFileImplSize = 24
		};
	COMPILE_TIME_ASSERT(sizeof(RResourceFileImpl)==KRscFileImplSize);
    // MSVC++ & GCCE can't handle this template instantiation
#if !defined(__VC32__) && !defined(__GCCE__) && !defined(__X86GCC__)
	//Fixed "iOffset" position - because of the BC reasons.
	COMPILE_TIME_ASSERT(_FOFF(RResourceFileImpl, iOffset)==12);
#endif
	}

/** Closes the resource file reader.
This function is called after finishing reading all resources.
@internalComponent */
void RResourceFileImpl::Close()
	{
	if ((iFile!=NULL) && ((iFlagsAndNumberOfResources & static_cast<TUint>(EFlagIsRomFile))==0 
		&& (iFlagsAndNumberOfResources & static_cast<TUint>(EFlagIsBufferRscFile))==0))
		{
		RFile* const file=STATIC_CAST(RFile*,iFile);
		file->Close();
		delete file;
		}
	//Here it indicates that it is reading from a HBufC8 rsc file buffer so we need to free the heap here
	if ((iFile!=NULL) && ((iFlagsAndNumberOfResources & static_cast<TUint>(EFlagIsBufferRscFile))!=0))
		{
		HBufC8* buffer=STATIC_CAST(HBufC8*,iFile);
		delete buffer;
		}
	iFile=NULL;
	iSizeOfLargestResourceWhenCompletelyUncompressed=0;
	delete iIndex;
	iIndex=NULL;
	if (iExtra)
		{
		delete [] iExtra->iDictionaryCompressionData.iCachedResourceBuffer;
		iExtra->iDictionaryCompressionData.iCachedResourceBuffer = NULL;
		delete [] iExtra->iDictionaryCompressionData.iCachedDictionaryIndex;
		iExtra->iDictionaryCompressionData.iCachedDictionaryIndex = 0;
		delete iExtra;
		iExtra=NULL;
		}
	iFlagsAndNumberOfResources=0;
	iOffset=0;
	}

/** Opens the resource file reader.
The resource file reader must be opened before reading resources or
checking the signature of the resource file. This function initially
closes the resource-file object if it is currently open. If a leave
occurs during the function, the object is reverted to its closed state.
@internalComponent
@param aFs Handle to a file server session.
@param aName File to open as a resource file.
@param aAssert Object which will be used to  determine panic/leave behaviour 
of the "L" methods.
@param aFileOffset The resource file section offset from the beginning of the file.
@param aFileSize The resource file section size.
@panic Some BAFL panic codes.
@leave KErrCorrupt The file is corrupted.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
void RResourceFileImpl::OpenL(RFs &aFs, const TDesC &aName, const TBaAssert& aAssert,
							  TUint aFileOffset/* = 0 */, TInt aFileSize/* = 0 */)
	{
	Close();
	CleanupClosePushL(*this);
	RFile* const file=new(ELeave) RFile;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteFile,file));
	User::LeaveIfError(file->Open(aFs, aName, EFileStream | EFileRead | EFileShareReadersOnly));
	iExtra=new (ELeave) TExtra(aAssert);
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	iExtra->iFileOffset = aFileOffset;
	TInt fileSize = 0;
	if (aFileSize)
		{
		// With new format AIF, the RSC file is part of a larger file, so we need to set the
		// size of the RSC file here.
		fileSize = aFileSize;
		assert_obj.AssertDebL(fileSize > TInt(aFileOffset), EBafPanicBadOpenArg);
		}
	else
		{
		User::LeaveIfError(file->Size(fileSize));
		assert_obj.AssertDebL(fileSize > 0, EBafPanicFileSize2);
		}
	TUint8* romAddress=NULL;
	TInt seekRc(KErrNotSupported);

	// ESeekAddress in emulator will just get -5 and NULL romAddress.
	// On emulator there is also undesirable effect that after the call the ROM
	// resource file cannot be deleted while emulator is running. Seems base has
	// some kind of handle open on the rsc. One licensee has a use case to
	// re-compile resources while emulator is running. Hence Seek only in hw.
#ifndef __WINSCW__
	seekRc = file->Seek(ESeekAddress,REINTERPRET_CAST(TInt&,romAddress));
#endif

	if (romAddress==NULL || seekRc !=KErrNone)
		{
		iFile=file;
		// At the 1st line of this function we call Close() which sets
		// iFlagsAndNUmberOfResources to zero. Nothing between Close() and here changes
		// iFlagsAndNumberOfResources. So no need to assert EFlagIsRomFile is cleared.
//		assert_obj.AssertDebL((iFlagsAndNumberOfResources & static_cast<TUint>(EFlagIsRomFile))==0, 
//										EBafPanicBadInitialization1);
		CleanupStack::Pop(file);
		}
	else
		{
		iFile=CONST_CAST(TUint8*,romAddress);
		iFlagsAndNumberOfResources |= static_cast<TUint>(EFlagIsRomFile);
		CleanupStack::PopAndDestroy(file);
		}
	iExtra->iFileSize = fileSize;
	ReadHeaderAndResourceIndexL();
	CleanupStack::Pop(this);
	}

/** Opens the resource file reader.
The resource file reader must be opened before reading resources or
checking the signature of the resource file. This function initially
closes the resource-file object if it is currently open. If a leave
occurs during the function, the object is reverted to its closed state.
@internalComponent
@param aRscArchive buffer containing a full rsc file
@param aAssert Object which will be used to  determine panic/leave behaviour 
of the "L" methods.
@panic Some BAFL panic codes.
@leave KErrCorrupt The file is corrupted.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */

void RResourceFileImpl::OpenL(const TDesC8& aRscArchive,const TBaAssert& aAssert)
	{
	Close();
	CleanupClosePushL(*this);
	
	iExtra=new (ELeave) TExtra(aAssert);

	//Since we are getting the entire buffer of the rsc file, we can treat
	//this equivalent to a ROM rsc file by pointing the iFile to
	//the start of the aRscArchive. We can depend directly on existing ReadL
	//to retrive the bytes directly from memory. No file open is required
	//and the handling of the rest of the rsc file is an exact copy of the
	//previous OpenL().A good way to reduce ROM size is to split the reading
	//part of the resource file into one separate function.//TODO
	//We also dont care about any fileOffset as the aRscArchive contains only
	//the buffer for one entire rsc file.
	//Get the length of the rsc file
	TInt fileSize=aRscArchive.Length();
	//iFile=STATIC_CAST(TAny*,(aRscArchive.AllocL())->Des()).Ptr());
	iFile = aRscArchive.AllocL();
	iExtra->iFileSize=fileSize;
	iFlagsAndNumberOfResources |= static_cast<TUint>(EFlagIsBufferRscFile);
			
	ReadHeaderAndResourceIndexL(); 
	CleanupStack::Pop(this);
	}

/** 
Retrieve the UID tuple of the opened resource file.

@internalComponent
@pre OpenL() has been called successfully.
@return The UIDs of the loaded resource file.
@panic BAFL 36 - data members are not initialized(probably - OpenL() hasn't been called). */
TUidType RResourceFileImpl::UidType() const
	{
	__ASSERT_ALWAYS(iExtra!=NULL,Panic(EBafPanicNotOpened));
	return iExtra->iUidType;
	}

/** Reads a resource specified by resource id into the specified descriptor.
The descriptor must be long enough to contain the entire resource. The
search for the resource uses the following algorithm: A resource id in
the range 1 to 4095 is looked up in this resource file. The function
leaves if there is no matching resource. If the resource id is greater
than 4095, then the most significant 20 bits of the resource id is
treated as an offset and the least significant 12 bits is treated as
the real resource id. If the offset matches the offset value defined
for this file, then the resource is looked up in this resource file
using the real resource id (i.e. the least significant 12 bits). If
the offset does not match, then the function leaves. Note, do not call
this function until a call to ConfirmSignatureL() has
completed successfully.
@internalComponent
@pre OpenL() is called.
@param aDes On return, contains the resource that has been read.
The function leaves if the descriptor is not long
enough to contain the entire resource.
@param aResourceId The numeric id of the resource to be read. The
function leaves if this resource id is not in this resource file.
@see ConfirmSignatureL()
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted. Some other error codes
are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
void RResourceFileImpl::ReadL(TDes8 &aDes,TInt aResourceId) const
	{
	HBufC8* const resource=AllocReadLC(aResourceId);
	aDes=*resource;
	CleanupStack::PopAndDestroy(resource);
	}

/** Reads a resource into a heap descriptor, returns a pointer to that
descriptor and pushes the pointer onto the cleanup stack.

A heap descriptor of appropriate length is allocated for the resource.
Ownership of the heap descriptor passes to the caller who must destroy
it and pop its pointer off the cleanup stack when it is no longer
needed.The search for the resource uses the following algorithm:A
resource id in the range 1 to 4095 is looked up in this resource file.
The function leaves if there is no matching resource.If the resource
id is greater than 4095, then the most significant 20 bits of the
resource id is treated as an offset and the least significant 12 bits
is treated as the real resource id. If the offset matches the offset
value defined for this file, then the resource is looked up in this
resource file using the real resource id (i.e. the least significant
12 bits). If the offset does not match, then the function leaves.Note,
do not call this function until a call to
ConfirmSignatureL() has completed successfully.

@internalComponent
@pre OpenL() is called.
@param aResourceId The numeric id of the resource to be read.
@return Pointer to a heap descriptor containing the resource.
@see ConfirmSignatureL()
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
@leave KErrNotFound There is no resource with aResourceId in the file.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
HBufC8* RResourceFileImpl::AllocReadLC(TInt aResourceId) const
	{
	if (!OwnsResourceIdL(aResourceId))
		{
		User::Leave(KErrNotFound);
		}
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	//"-1" because the first resource has ID 0x*****001 (not 0x*****000)
	TInt resourceIndex=(aResourceId & EIdBits)-1; 
	if (iFlagsAndNumberOfResources & EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource)
		{
		assert_obj.AssertDebL(iFlagsAndNumberOfResources & EFlagDictionaryCompressed, 
								EBafPanicBadFlags1);
		assert_obj.AssertDebL(iFlagsAndNumberOfResources & EFlagThirdUidIsOffset, 
								EBafPanicBadFlags2);
		if (resourceIndex>0)
			{
			--resourceIndex;
			}
		else
			{
			assert_obj.AssertDebL(resourceIndex==0, EBafPanicNegativeResourceIndex2);
			HBufC8* const resourceDataFor_RSS_SIGNATURE=HBufC8::NewMaxLC(8);
			TUint* wordPointer=REINTERPRET_CAST(TUint*,CONST_CAST(
											TUint8*,resourceDataFor_RSS_SIGNATURE->Ptr()));
			wordPointer[0]=4;
			wordPointer[1]=((iExtra->iUidType[2].iUid << 12) | 1);
			return resourceDataFor_RSS_SIGNATURE;
			}
		}
	const TBool firstResourceIsGenerated=
		(iFlagsAndNumberOfResources & 
		EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode);
	if (firstResourceIsGenerated)
		{
		assert_obj.AssertDebL(iFlagsAndNumberOfResources & 
								EFlagDictionaryCompressed, EBafPanicBadFlags6);
		//dictionary-compressed resource files can have an automatically generated 
		//resource which is the bit-array of resources containing compressed Unicode 
		//(this automatically generated resource does not have a corresponding bit for 
		//itself in the bit-array as it would be self-referring...)
		++resourceIndex; 
		}
	assert_obj.AssertDebL(resourceIndex>=0, EBafPanicNegativeResourceIndex3);
	HBufC8* const dictionaryDecompressedResourceData=DictionaryDecompressedResourceDataLC(
														resourceIndex,
														iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags),
														iExtra->iDictionaryCompressionData,
														*iIndex);
	if (!iExtra->ContainsCompressedUnicodeL(resourceIndex,firstResourceIsGenerated))
		{
		return dictionaryDecompressedResourceData;
		}
	HBufC8* const finalResourceData=DecompressUnicodeL(
										*dictionaryDecompressedResourceData,
										iSizeOfLargestResourceWhenCompletelyUncompressed);
	CleanupStack::PopAndDestroy(dictionaryDecompressedResourceData);
	CleanupStack::PushL(finalResourceData);
	return finalResourceData;
	}

/** @internalComponent
@return The first resource record.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
RResourceFileImpl::SSigRecord RResourceFileImpl::FirstRecordL() const
	{
	// Added to support reading of rel 6.x resource files.
	// rel 6.x files do not have signatures!

	HBufC8* const firstResource=AllocReadLC(1);

	// Basic check to test if the signature is of the correct size.
	if (firstResource->Size() != sizeof(SSigRecord))
		{
		User::Leave(KErrCorrupt);
		}
	SSigRecord sigRecord = *reinterpret_cast<const SSigRecord*>(firstResource->Ptr());
	CleanupStack::PopAndDestroy(firstResource);
	return sigRecord;
	}

/** Initialises the offset value from the first resource.
The function assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().
@internalComponent
@pre OpenL() is called.
@param aSignature This argument value is not used by the function.
@see Offset()
@see AllocReadL()
@see AllocReadLC() 
@see ReadL()
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
void RResourceFileImpl::ConfirmSignatureL(TInt /*aSignature*/)
    {
	SSigRecord first_record=FirstRecordL();
	// test removed by SC 29/09/98 - assume that existing resource layout and structures 
	//are fixed now
//	if (first_record.signature!=aSignature)
//		Leave(KErrBaflWrongResourceFileSignature);
    iOffset=first_record.offset & EOffsetBits;
    }

/** Initialises the offset value from the first resource.

The function tests to catch cases where the first resource is not an RSS_SIGNATURE.
It assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().

@see Offset()
@see AllocReadL()
@see AllocReadLC() 
@see ReadL()
@internalComponent
@pre OpenL() is called.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
void RResourceFileImpl::ConfirmSignatureL()
	{
	// Added to support reading of rel 6.x resource files.

	SSigRecord firstRecord=FirstRecordL();

	// If the resource offset does not correspond to the first resource
	// this is not a resource signature.
	if ((firstRecord.offset & EIdBits) != 1)
		{
		User::Leave(KErrCorrupt);
		}
	iOffset=(firstRecord.offset & EOffsetBits);
	}

/** @internalComponent
@pre OpenL() is called.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
TInt RResourceFileImpl::SignatureL() const
	{
	SSigRecord first_record=FirstRecordL();
	return first_record.signature;
	}

/** Tests whether the resource file owns the specified resource id.

The resource file owns the resource id if the most significant 20 bits
of the resource id are zero or match the offset value as returned from
a call to the Offset() member function or if the resource id is not out of range.

@internalComponent
@see Offset()
@pre OpenL() is called.
@param aResourceId The resource id to test.
@return True, if the resource file owns the id, false otherwise.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
TBool RResourceFileImpl::OwnsResourceIdL(TInt aResourceId) const
	{ 
	// whether owns resource: does so if offset 0, or matches that given, 
	//and id is in index
	const TInt offset=(aResourceId & EOffsetBits);
	if ((offset!=0) && (offset!=iOffset))
		{
		return EFalse;
		}
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	const TInt resourceIndex=(aResourceId & EIdBits)-1;
	TInt numberOfResources=(iFlagsAndNumberOfResources & ~EAllFlags);
	if (iFlagsAndNumberOfResources & EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource)
		{
		assert_obj.AssertDebL(iFlagsAndNumberOfResources & EFlagDictionaryCompressed, 
								EBafPanicBadFlags3);
		assert_obj.AssertDebL(iFlagsAndNumberOfResources & EFlagThirdUidIsOffset, 
								EBafPanicBadFlags4);
		++numberOfResources;
		}
	if (iFlagsAndNumberOfResources & 
		EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode)
		{
		iExtra->iAssertObj.AssertDebL(iFlagsAndNumberOfResources & EFlagDictionaryCompressed, 
								EBafPanicBadFlags5);
		--numberOfResources;
		}
	return (resourceIndex >= 0) && (resourceIndex < numberOfResources);
	}

/** The method will decomress the unicode data (aCompressedUnicode argument) and append
the decompressed data to the end of aBuffer (aBuffer argument).

@internalComponent
@pre OpenL() is called.
@param aBuffer Destination buffer.
@param aCompressedUnicode Compressed unicode buffer.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
void RResourceFileImpl::AppendDecompressedUnicodeL(
												   TDes8& aBuffer,
												   const TDesC8& aCompressedUnicode) const
	{
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	const TInt lengthOfCompressedUnicode=aCompressedUnicode.Length();
	if (lengthOfCompressedUnicode>0)
		{
		const TUint8* startOfDecompressedUnicode=aBuffer.Ptr()+aBuffer.Length();
		if (REINTERPRET_CAST(TUint,startOfDecompressedUnicode) & 0x01)
			{
			assert_obj.AssertDebL(aBuffer.MaxLength() >= (aBuffer.Length() + 1), 
									EBafPanicMaxLength2);
			aBuffer.Append(0xab);
			++startOfDecompressedUnicode;
			}
		//works correctly, even if aBuffer.MaxLength()-aBuffer.Length() is an odd number
		const TInt maximumOutputLength=(aBuffer.MaxLength()-aBuffer.Length())/2; 
		TMemoryUnicodeSink decompressedUnicode(REINTERPRET_CAST(TUint16*,CONST_CAST(
													TUint8*,startOfDecompressedUnicode)));
		TInt lengthOfDecompressedUnicode;
		TInt numberOfInputBytesConsumed;
		TUnicodeExpander unicodeExpander;
		unicodeExpander.ExpandL(decompressedUnicode,
								aCompressedUnicode.Ptr(),
								maximumOutputLength,
								lengthOfCompressedUnicode,
								&lengthOfDecompressedUnicode,
								&numberOfInputBytesConsumed);
		TInt temp;
		unicodeExpander.FlushL(decompressedUnicode,maximumOutputLength,temp);
		lengthOfDecompressedUnicode+=temp;
		assert_obj.AssertRelL(numberOfInputBytesConsumed==lengthOfCompressedUnicode, 
										EBafPanicBadDecompression);
		aBuffer.SetLength(aBuffer.Length()+(lengthOfDecompressedUnicode*2));
		}
	}

/** The method will decompress the unicode data (aInputResourceData argument), allocate enough
memory from the heap for the decompressed data, copy the data there and return a descriptor
to the decompressed data.

The method doesn't own the allocated heap memory for the decompressed data. It's a caller
responsibility to deallocate the allocated memory.

@internalComponent
@param aInputResourceData Compressed data.
@param aSizeOfLargestResourceWhenCompletelyUncompressed The size of decomressed data.
@pre OpenL() is called.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
@leave KErrNoMemory There is not enough memory for the decompressed data.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
HBufC8* RResourceFileImpl::DecompressUnicodeL(const TDesC8& aInputResourceData,
										 TInt aSizeOfLargestResourceWhenCompletelyUncompressed) const
	{
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	const TInt numberOfBytesInInput=aInputResourceData.Length();
	assert_obj.AssertDebL(aSizeOfLargestResourceWhenCompletelyUncompressed>0, 
							EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed2);
	HBufC8* const outputResourceData=
							HBufC8::NewLC(aSizeOfLargestResourceWhenCompletelyUncompressed);
	TPtr8 asWritable(outputResourceData->Des());
	const TUint8* input=aInputResourceData.Ptr();
	TInt index=0;
	for (TBool decompressRun=ETrue; ; decompressRun=!decompressRun)
		{
		assert_obj.AssertDebL(index<numberOfBytesInInput, EBafPanicIndexOverRan);
		TInt runLength=input[index];
		if (runLength & 0x80)
			{
			++index;
			if (index>=numberOfBytesInInput)
				{
				User::Leave(KErrCorrupt);
				}
			runLength &= ~0x80;
			runLength <<= 8;
			runLength |= input[index];
			}
		++index;
		if (runLength>0)
			{
			const TPtrC8 run(input+index,runLength);
			if (decompressRun)
				{
				AppendDecompressedUnicodeL(asWritable,run);
				}
			else
				{
				assert_obj.AssertDebL(
					(asWritable.Length() + runLength) <= asWritable.MaxLength(), 
					EBafPanicOverflow);
				asWritable.Append(run);
				}
			index+=runLength;
			}
		if (index>numberOfBytesInInput)
			{
			User::Leave(KErrCorrupt);
			}
		if (index>=numberOfBytesInInput)
			{
			break;
			}
		}
	CleanupStack::Pop(outputResourceData);
	return outputResourceData;
	}

/** @internalComponent
@pre OpenL() is called.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
@leave KErrNoMemory There is not enough memory for the decompressed data.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
HBufC8* RResourceFileImpl::DictionaryDecompressedResourceDataLC(
			TInt aResourceIndex,
			TUint aFlags,
			const SDictionaryCompressionData& aDictionaryCompressionData,
			const TDesC16& aIndex) const
	{
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	if (aFlags & EFlagDictionaryCompressed)
		{
		assert_obj.AssertDebL(iSizeOfLargestResourceWhenCompletelyUncompressed>0, 
								EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed3);
		HBufC8* const outputResourceData=
							HBufC8::NewLC(iSizeOfLargestResourceWhenCompletelyUncompressed);
		TPtr8 asWritable(outputResourceData->Des());
		RArray<RDictionaryCompressionBitStream> stackOfDictionaryCompressionBitStreams;
		CleanupStack::PushL(
			TCleanupItem(CloseArrayOfDictionaryCompressionBitStreams,
						&stackOfDictionaryCompressionBitStreams));
		AppendDictionaryCompressionBitStreamL(
						stackOfDictionaryCompressionBitStreams,
						aFlags,
						aDictionaryCompressionData,
						aDictionaryCompressionData.iStartOfResourceData,
						aDictionaryCompressionData.iStartOfResourceIndex,
						aResourceIndex);
		const TBool calypsoFileFormat=(aFlags & EFlagCalypsoFileFormat);
		FOREVER
			{
			const TInt indexOfTopBitStream=stackOfDictionaryCompressionBitStreams.Count()-1;
			assert_obj.AssertDebL(indexOfTopBitStream>=-1, EBafPanicNegativeArrayIndex);
			if (indexOfTopBitStream<0)
				{
				break;
				}
			RDictionaryCompressionBitStream& dictionaryCompressionBitStream=
							stackOfDictionaryCompressionBitStreams[indexOfTopBitStream];
			FOREVER
				{
				if (dictionaryCompressionBitStream.EndOfStreamL())
					{
					dictionaryCompressionBitStream.Close();
					stackOfDictionaryCompressionBitStreams.Remove(indexOfTopBitStream);
					break;
					}
				const TInt indexOfDictionaryEntry=
								dictionaryCompressionBitStream.IndexOfDictionaryEntryL();
				if (indexOfDictionaryEntry<0)
					{
					dictionaryCompressionBitStream.ReadL(asWritable,calypsoFileFormat);
					}
				else
					{
					AppendDictionaryCompressionBitStreamL(
											stackOfDictionaryCompressionBitStreams,
											aFlags,
											aDictionaryCompressionData,
											aDictionaryCompressionData.iStartOfDictionaryData,
											aDictionaryCompressionData.iStartOfDictionaryIndex,
											indexOfDictionaryEntry);
					break;
					}
				}
			}
		CleanupStack::PopAndDestroy(&stackOfDictionaryCompressionBitStreams);
		return outputResourceData;
		}
	assert_obj.AssertDebL(aResourceIndex < aIndex.Length(), EBafPanicBadIndex2);
	const TInt positionOfResourceData=aIndex[aResourceIndex];
	const TInt numberOfBytes=aIndex[aResourceIndex+1]-positionOfResourceData;
	assert_obj.AssertDebL(numberOfBytes >= 0, EBafPanicNegativeNumber3);
	HBufC8* const outputResourceData=HBufC8::NewLC(numberOfBytes);
	TPtr8 asWritable(outputResourceData->Des());
	ReadL(aFlags,positionOfResourceData,asWritable,numberOfBytes);
	return outputResourceData;
	}

/** @internalComponent
@pre OpenL() is called.
@panic Some BAFL panic codes, if the file is corrupted.
@leave KErrCorrupt The file is corrupted.
Some other error codes are possible too.
The method could panic or leave depending on the state of
iAssertObj member of RResourceFileImpl::TExtra class. */
void RResourceFileImpl::AppendDictionaryCompressionBitStreamL(
			RArray<RDictionaryCompressionBitStream>& aStackOfDictionaryCompressionBitStreams,
			TUint aFlags,
			const SDictionaryCompressionData& aDictionaryCompressionData,
			TInt aStartOfBitData,
			TInt aStartOfIndex,
			TInt aIndexEntry) const
	{
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	const TBool isRomFile=(aFlags & static_cast<TUint>(EFlagIsRomFile));
	TBuf8<4> temp;
	assert_obj.AssertDebL(aIndexEntry>=0, EBafPanicNegativeIndexEntry);
	TInt offsetToFirstBit;
	TInt offsetOnePastLastBit;
	if (	aDictionaryCompressionData.iStartOfDictionaryIndex == aStartOfIndex
		&&	aDictionaryCompressionData.iCachedDictionaryIndex != 0)
		{
		__ASSERT_DEBUG(!isRomFile,User::Invariant());
		// indices start at 1
		offsetToFirstBit = (aIndexEntry <= 0)
			?	0
			:	aDictionaryCompressionData.iCachedDictionaryIndex[aIndexEntry-1];
		offsetOnePastLastBit = aDictionaryCompressionData.iCachedDictionaryIndex[aIndexEntry];
		}
	else
		{
		ReadL(aFlags,aStartOfIndex+((aIndexEntry-1)*2),temp,4);
		offsetToFirstBit=(aIndexEntry > 0) ? LittleEndianTwoByteIntegerL(temp,0) : 0;
		offsetOnePastLastBit=LittleEndianTwoByteIntegerL(temp,2);
		}
	TInt rsc_file_size = iExtra->iFileOffset + iExtra->iFileSize;
	TInt offset_first = offsetToFirstBit / 8 + iExtra->iFileOffset;
	assert_obj.AssertDebL(offset_first < rsc_file_size, EBafPanicBadOffset);
	TInt offset_last = offsetOnePastLastBit / 8 + iExtra->iFileOffset;
	assert_obj.AssertDebL(offset_last <= rsc_file_size, EBafPanicBadOffset2);
	TUint8* buffer = NULL;
	TInt start_pos = 0;
	if (isRomFile)
		{
		TInt startOfBitData = aStartOfBitData + iExtra->iFileOffset;
		assert_obj.AssertDebL(startOfBitData < rsc_file_size, EBafPanicStartPos);
		//for some reason, GCC gives a warning if the 
		//STATIC_CAST(const TUint8*,aFile) is used instead of STATIC_CAST(TUint8*,aFile)
		buffer = STATIC_CAST(TUint8*,iFile) + startOfBitData; 
		}
	else
		{
		const TInt offsetToByteContainingFirstBit=offsetToFirstBit/8;
		const TInt offsetToOnePastByteContainingLastBit=((offsetOnePastLastBit-1)/8)+1;
		const TInt numberOfBytesToLoad=
						offsetToOnePastByteContainingLastBit-offsetToByteContainingFirstBit;
		assert_obj.AssertDebL(numberOfBytesToLoad >= 0, EBafPanicNegativeNumber2);
		buffer=new(ELeave) TUint8[numberOfBytesToLoad];
		CleanupArrayDeletePushL(buffer);
		if( iExtra->iDictionaryCompressionData.iCachedResourceBuffer == 0)
			{
			iExtra->iDictionaryCompressionData.iCachedResourceBuffer=new(ELeave) TUint8[rsc_file_size]; // reserver buffer for whole file
				TPtr8 JKasWritable(iExtra->iDictionaryCompressionData.iCachedResourceBuffer,rsc_file_size);
				User::LeaveIfError(
					STATIC_CAST(RFile*,iFile)->Read(
										0,
										JKasWritable,
										rsc_file_size));
			}
		start_pos = aStartOfBitData + offsetToByteContainingFirstBit + iExtra->iFileOffset;
		assert_obj.AssertDebL(start_pos < rsc_file_size, EBafPanicStartPos2);
		assert_obj.AssertDebL((start_pos + numberOfBytesToLoad) <= rsc_file_size, EBafPanicFileSize4);
		const TInt numberOfBitsFromStartOfBitDataToFirstLoadedByte=
											offsetToByteContainingFirstBit*8;
		offsetToFirstBit-=numberOfBitsFromStartOfBitDataToFirstLoadedByte;
		offsetOnePastLastBit-=numberOfBitsFromStartOfBitDataToFirstLoadedByte;

		Mem::Copy( buffer, iExtra->iDictionaryCompressionData.iCachedResourceBuffer + start_pos,  numberOfBytesToLoad);
		}
	RDictionaryCompressionBitStream stream;
	stream.OpenL(
				aDictionaryCompressionData.iNumberOfBitsUsedForDictionaryTokens,
				offsetToFirstBit,
				offsetOnePastLastBit,
				!isRomFile,
				buffer,
				assert_obj);
	User::LeaveIfError(aStackOfDictionaryCompressionBitStreams.Append(stream));
	if (!isRomFile)
		{
		CleanupStack::Pop(buffer); // buffer deleted in RDictionaryCompressionBitStream::close
		}
	}

/** @internalComponent
@pre OpenL() is called. */
TInt RResourceFileImpl::LittleEndianTwoByteIntegerL(
													const TDesC8& aBuffer,
													TInt aIndexOfFirstByte) const
	{
	iExtra->iAssertObj.AssertDebL((aIndexOfFirstByte + 1) < aBuffer.Length(), EBafPanicBadIndex3);
	return aBuffer[aIndexOfFirstByte] | (aBuffer[aIndexOfFirstByte+1]<<8);
	}

/** @internalComponent
@pre OpenL() is called. */
void RResourceFileImpl::ReadL(TUint aFlags,TInt aPos,TDes8& aDes,TInt aLength) const
	{
	aPos += iExtra->iFileOffset;
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	assert_obj.AssertDebL(aPos >= iExtra->iFileOffset, EBafPanicNegativePos);
	assert_obj.AssertDebL(aLength >= 0, EBafPanicNegativeLength);
	assert_obj.AssertDebL(aLength <= aDes.MaxLength(), EBafPanicMaxLength);
	assert_obj.AssertDebL((aPos + aLength) <= (iExtra->iFileOffset + iExtra->iFileSize), EBafPanicFileSize3);
	if (aFlags & static_cast<TUint>(EFlagIsRomFile))
		{
		//for some reason, GCC gives a warning if the 
		//STATIC_CAST(const TUint8*,aFile) is used instead of STATIC_CAST(TUint8*,aFile)
		aDes.Copy(STATIC_CAST(TUint8*,iFile)+aPos,aLength); 
		}
	else if (aFlags & static_cast<TUint>(EFlagIsBufferRscFile))
		{
		HBufC8* buffer=STATIC_CAST(HBufC8*,iFile);
		aDes.Copy(CONST_CAST(TUint8*,buffer->Ptr())+aPos,aLength);
		}
	else
		{
		User::LeaveIfError(STATIC_CAST(RFile*,iFile)->Read(aPos,aDes,aLength));
		}
	}

/** @internalComponent
@pre OpenL() is called. */
void RResourceFileImpl::ReadL(TInt aPos,TDes8& aDes,TInt aLength) const
	{
	ReadL(iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags),aPos,aDes,aLength);
	}

/** Returns the offset value defined for this resource file.

This function must not be called until a call to
ConfirmSignatureL() has completed successfully, otherwise
the value returned by this function may be meaningless.

@see ConfirmSignatureL()
@internalComponent
@pre OpenL() is called.
@return The offset value defined for this resource file.
 */
TInt RResourceFileImpl::Offset() const
	{
	return iOffset;
	}

/** Function to retrieve the header information of the rsc file and all the
	resource index information in the rsc file. This function is created to
	handle the common functionality in the two OpenL() method.
@internalComponent
@pre OpenL() is called.
*/
void RResourceFileImpl::ReadHeaderAndResourceIndexL()
	{
	SDictionaryCompressionData dictionaryCompressionData;
	const TBaAssert& assert_obj = iExtra->iAssertObj;
	TUidType uidType;
	//dictionary-compressed resource files have a 21-byte header, 
	//16 bytes of checked UIDs followed by a 1-byte field and two 2-byte fields
	TBuf8<21> header; 
	if(iExtra->iFileSize >= 16)
		{
		ReadL(0,header,Min(header.MaxLength(),iExtra->iFileSize));
		uidType=TCheckedUid(header.Left(16)).UidType();
		if (uidType[0].iUid==0x101f4a6b)
			{
			iFlagsAndNumberOfResources |= EFlagPotentiallyContainsCompressedUnicode;
			assert_obj.AssertDebL(header.Length() >= 18, EBafPanicBadHeader1);
			iSizeOfLargestResourceWhenCompletelyUncompressed=
				LittleEndianTwoByteIntegerL(header,16+1);
			}
		else if (uidType[0].iUid==0x101f5010)
			{
			iFlagsAndNumberOfResources |=
				EFlagPotentiallyContainsCompressedUnicode | EFlagDictionaryCompressed;
			assert_obj.AssertDebL(header.Length() >= 18, EBafPanicBadHeader2);
			iSizeOfLargestResourceWhenCompletelyUncompressed=
				LittleEndianTwoByteIntegerL(header,16+1);
			}
		else if (uidType[0]!=TUid::Null())
			{
			User::Leave(KErrNotSupported);
			}
		//the "signature" of Calypso's resource files
 		else if (LittleEndianTwoByteIntegerL(header,0)==4) 
			{
 			iFlagsAndNumberOfResources |= EFlagDictionaryCompressed | EFlagCalypsoFileFormat;
 			iSizeOfLargestResourceWhenCompletelyUncompressed=
				LittleEndianTwoByteIntegerL(header,8);
			}
		}

	//It seems that the following AssertDebL() call never fails, 
	//because LittleEndianTwoByteIntegerL always 
	//returns zero or positive value.
	assert_obj.AssertDebL(iSizeOfLargestResourceWhenCompletelyUncompressed>=0, 
							EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed1);
	TInt numberOfResources=0;
	HBufC8* bitArrayOfResourcesContainingCompressedUnicode=NULL;
	if (iFlagsAndNumberOfResources & EFlagDictionaryCompressed)
		{
		if (iFlagsAndNumberOfResources & EFlagCalypsoFileFormat)
			{
			assert_obj.AssertDebL(header.Length() > 10, EBafPanicBadHeader3);
			numberOfResources=LittleEndianTwoByteIntegerL(header,2);
			const TInt numberOfBitsUsedForDictionaryTokens = header[10];
			const TInt numberOfDictionaryEntries =
				(1 << numberOfBitsUsedForDictionaryTokens) - header[5];
			assert_obj.AssertDebL(numberOfDictionaryEntries >= 0, EBafPanicBadHeader4);
			// "+2" because the first entry in the dictionary-index in this file format 
			//is the number of bits from the start of the dictionary data to the start 
			//of the first dictionary entry which is always zero, and thus unnecessary
			const TInt startOfDictionaryData=4+7+2; 
			// "+2" because the first entry in the resource-index in this file format is 
			//the number of bits from the start of the resource data to the start of the 
			//first resource which is always zero, and thus unnecessary
			const TInt startOfResourceIndex=LittleEndianTwoByteIntegerL(header,6)+2; 
			assert_obj.AssertDebL(startOfResourceIndex >= 0, EBafPanicBadHeader5);
			dictionaryCompressionData.iStartOfDictionaryData=
						startOfDictionaryData+(numberOfDictionaryEntries*2);
			dictionaryCompressionData.iStartOfDictionaryIndex=startOfDictionaryData;
			dictionaryCompressionData.iNumberOfDictionaryEntries=numberOfDictionaryEntries;
			dictionaryCompressionData.iStartOfResourceData=
						startOfResourceIndex+(numberOfResources*2);
			dictionaryCompressionData.iStartOfResourceIndex=startOfResourceIndex;
			dictionaryCompressionData.iNumberOfBitsUsedForDictionaryTokens=
						numberOfBitsUsedForDictionaryTokens;
						
			if ((iFlagsAndNumberOfResources & static_cast<TUint>(EFlagIsRomFile)) == 0)
				{
				// attempt to cache dictionary index
				// allocate and populate the dictionary index buffer
				dictionaryCompressionData.iCachedDictionaryIndex = new TUint16[numberOfDictionaryEntries];
				if (dictionaryCompressionData.iCachedDictionaryIndex != 0)
					{
					TInt length = numberOfDictionaryEntries * 2;
					TPtr8 ptr8((TUint8*)dictionaryCompressionData.iCachedDictionaryIndex, 0, length);
					ReadL(
						iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags),		// aFlags
						startOfDictionaryData,						// aPos
						ptr8,
						length);									// aLength
					}
				}	// if (iFlagsAndNumberOfResources & EFlagIsRomFile)
			}
		else
			{
 			assert_obj.AssertDebL(header.Length()==16+1+2+2, EBafPanicBadHeaderLength);
 			const TUint firstByteAfterUids=header[16];
 			if (firstByteAfterUids & 0x80)
				{
				// this flag is only set if the resource file is dictionary-compressed
			    iFlagsAndNumberOfResources |= EFlagThirdUidIsOffset; 
				}
			if (firstByteAfterUids & 0x40)
				{
				// this flag is only set if the resource file is dictionary-compressed
				iFlagsAndNumberOfResources |= EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource; 
				}
			if (firstByteAfterUids & 0x20)
				{
				iFlagsAndNumberOfResources |=
					EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode;
				}
			dictionaryCompressionData.iStartOfResourceData=
				LittleEndianTwoByteIntegerL(header,16+1+2);
			TBuf8<2> temp;
			ReadL((iExtra->iFileSize)-2,temp,2);
			const TInt numberOfBitsOfResourceData=LittleEndianTwoByteIntegerL(temp,0);
			dictionaryCompressionData.iStartOfResourceIndex=
						dictionaryCompressionData.iStartOfResourceData+
						((numberOfBitsOfResourceData+7)/8);
			numberOfResources=(iExtra->iFileSize-dictionaryCompressionData.iStartOfResourceIndex)/2;
			dictionaryCompressionData.iStartOfDictionaryData=16+5;
			if ((numberOfResources>0) && 
				!(iFlagsAndNumberOfResources &
				EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode))
				{
				const TInt lengthOfBitArrayInBytes=(numberOfResources+7)/8;
				bitArrayOfResourcesContainingCompressedUnicode=
									HBufC8::NewLC(lengthOfBitArrayInBytes);
				TPtr8 asWritable(bitArrayOfResourcesContainingCompressedUnicode->Des());
				ReadL(16+5,asWritable,lengthOfBitArrayInBytes);
				dictionaryCompressionData.iStartOfDictionaryData+=lengthOfBitArrayInBytes;
				}
			ReadL(dictionaryCompressionData.iStartOfResourceData-2,temp,2);
			const TInt numberOfBitsOfDictionaryData=LittleEndianTwoByteIntegerL(temp,0);
			dictionaryCompressionData.iStartOfDictionaryIndex=
						dictionaryCompressionData.iStartOfDictionaryData+
						((numberOfBitsOfDictionaryData+7)/8);
			dictionaryCompressionData.iNumberOfDictionaryEntries=
						(dictionaryCompressionData.iStartOfResourceData-
						dictionaryCompressionData.iStartOfDictionaryIndex)/2;
			//the bottom 3 bits of firstByteAfterUids stores the number of bits used for 
			//dictionary tokens as an offset from 3, e.g. if 2 is stored in these three bits 
			//then the number of bits per dictionary token would be 3+2=5 - this allows a 
			//range of 3-11 bits per dictionary token (the maximum number of dictionary 
			//tokens therefore ranging from 8-2048) - the spec currently only supports 5-9
			//bits per dictionary token, however
			dictionaryCompressionData.iNumberOfBitsUsedForDictionaryTokens=
						3 + (firstByteAfterUids & 0x07); 
			if ((numberOfResources>0) && 
				(iFlagsAndNumberOfResources &
				EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode))
				{
				bitArrayOfResourcesContainingCompressedUnicode=
					DictionaryDecompressedResourceDataLC(
											0,
											iFlagsAndNumberOfResources & static_cast<TUint>(EAllFlags),
											dictionaryCompressionData,
											KNullDesC16);
				}
			}
		}
	else
		{
		assert_obj.AssertDebL((iExtra->iFileSize + iExtra->iFileOffset) > 2, EBafPanicFileSize);
		// This format of resource file is likely to be used for non-ROM resource files, 
		//so cache the resource-index (in iIndex) to minimize disk access.
		// Ignore the flags in non-dictionary-compressed resource files - they are to 
		//be used only by a dictionary-compressing program rather than to be used directly 
		//by Bafl when reading non-dictionary-compressed resource files (as output by Rcomp).
		const TInt KMaximumNumberOfBytesCached=256;
		TBuf8<KMaximumNumberOfBytesCached> cache;
		const TInt numberOfBytesCached=Min(iExtra->iFileSize,KMaximumNumberOfBytesCached);
		ReadL(iExtra->iFileSize-numberOfBytesCached,cache,numberOfBytesCached);
		assert_obj.AssertDebL(cache.Length()==numberOfBytesCached, EBafPanicBadCacheLength);
		const TInt positionOfStartOfIndex=
					((cache[numberOfBytesCached-1]<<8) | cache[numberOfBytesCached-2]);
		const TInt numberOfBytesOfIndex=iExtra->iFileSize-positionOfStartOfIndex;
		assert_obj.AssertDebL(numberOfBytesOfIndex%2==0, EBafPanicIndexIsOddNumberOfBytes);
		assert_obj.AssertDebL(numberOfBytesOfIndex>=0, EBafPanicNegativeNumber);
		const TInt numberOfBytesOfIndexStillToRetrieve=
					numberOfBytesOfIndex-numberOfBytesCached;
		if (numberOfBytesOfIndexStillToRetrieve<=0)
			{
			const TPtrC8 indexAsBinaryBuffer(cache.Right(numberOfBytesOfIndex));
			iIndex=HBufC16::NewMaxL(numberOfBytesOfIndex/2);
			//indexAsBinaryBuffer.Ptr() may not be 2-byte aligned, hence why we can't do 
			//iIndex=TPtrC16(REINTERPRET_CAST(const TUint16*,indexAsBinaryBuffer.Ptr()),
			//numberOfBytesOfIndex/2).AllocL()
			Mem::Copy(CONST_CAST(TUint16*,iIndex->Ptr()),indexAsBinaryBuffer.Ptr(),
				numberOfBytesOfIndex); 
			}
		else
			{
			HBufC16* const index=HBufC16::NewMaxLC(numberOfBytesOfIndex/2);
			TPtr8 indexAsWritableBinaryBuffer(REINTERPRET_CAST(TUint8*,CONST_CAST
												(TUint16*,index->Ptr())),numberOfBytesOfIndex);
			ReadL(positionOfStartOfIndex,indexAsWritableBinaryBuffer,
											numberOfBytesOfIndexStillToRetrieve);
			assert_obj.AssertDebL(
				indexAsWritableBinaryBuffer.Length()==numberOfBytesOfIndexStillToRetrieve, 
				EBafPanicBadIndexLength1);
			indexAsWritableBinaryBuffer.Append(cache);
			assert_obj.AssertDebL(indexAsWritableBinaryBuffer.Length()==numberOfBytesOfIndex, 
				EBafPanicBadIndexLength2);
			assert_obj.AssertDebL(indexAsWritableBinaryBuffer.Length()==index->Length()*2, 
				EBafPanicBadIndexLength3);
			iIndex=index;
			CleanupStack::Pop(index);
			}
		//"-1" because the last thing in the index (which is in fact the last thing in the 
		//file itself) is the position of the start of the index which is therefore not 
		//pointing to a resource
		numberOfResources=(numberOfBytesOfIndex/2) - 1; 
		if ((numberOfResources>0) && 
			(iFlagsAndNumberOfResources & EFlagPotentiallyContainsCompressedUnicode))
			{
			const TInt lengthOfBitArrayInBytes=(numberOfResources+7)/8;
			bitArrayOfResourcesContainingCompressedUnicode=
								HBufC8::NewLC(lengthOfBitArrayInBytes);
			TPtr8 bitArray(bitArrayOfResourcesContainingCompressedUnicode->Des());
			//"16+1+2": 16 bytes of checked-UID + 1 byte of flags (these flags are for a 
			//dictionary-compressing program's use rather than directly for Bafl's use, 
			//so we ignore them) + 2 bytes containing the size of the largest resource when 
			//uncompressed
			ReadL(16+1+2,bitArray,lengthOfBitArrayInBytes); 
			}
		}
	assert_obj.AssertDebL((numberOfResources & EAllFlags)==0, EBafPanicBadNumberOfResources);
	assert_obj.AssertDebL((iFlagsAndNumberOfResources & ~EAllFlags)==0, 
							EBafPanicBadInitialization2);
	iFlagsAndNumberOfResources |= (numberOfResources & ~EAllFlags);
	iExtra->iUidType = uidType;
	iExtra->iBitArrayOfResourcesContainingCompressedUnicode = bitArrayOfResourcesContainingCompressedUnicode;
	iExtra->iDictionaryCompressionData = dictionaryCompressionData;
	if (bitArrayOfResourcesContainingCompressedUnicode!=NULL)
		{
		CleanupStack::Pop(bitArrayOfResourcesContainingCompressedUnicode);
		}
	//iOffset is set by calling ConfirmSignatureL
	assert_obj.AssertDebL(iOffset==0, EBafPanicBadInitialization3);
	}
