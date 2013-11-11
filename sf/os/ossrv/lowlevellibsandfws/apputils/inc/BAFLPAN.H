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
// Panics
// 
//

#ifndef __BAFLPAN_H__
#define __BAFLPAN_H__

//If you add/change TBafPanic enum - don't forget to add/change
//related "leave" error code to KErrorMapTbl array.
//Enum item value is used as an index to access related "leave"
//error code into KErrorMapTbl array.
/**
@publishedPartner
@released
*/
enum TBafPanic
	{
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicResourceLongerThanBuffer				=1,
	/** Out of range index passed to RIncrMatcherBase::FirstMatchingIndex()
	*/
	EBafPanicMatcherOutOfRange						=2,
	/** Maximum length passed to RTextBuf::SetMaxLengthL() was <=0
	*/
	EBafPanicTextBufOutOfRange						=3,
	/** This panic is raised by the Read...() member functions and the Advance() member function of TResourceReader, when the current position in the resource buffer is moved beyond the end of the buffer.
	*/
	EBafPanicResourceReaderEndExceeded				=4,
	/** This panic is raised by TResourceReader::Rewind(), when the current position in the resource buffer is moved before the start of the buffer.
	*/
	EBafPanicResourceReaderStartExceeded			=5,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicBulkDataWriting						=6,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicBulkDataNotWriting						=7,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicBulkDataCannotSaveToFile				=8,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicBulkDataNotEmbedded					=9,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicSoundDataCannotChangeCompressorType	=10,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicSoundPlaying							=11,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicSoundSamplerActive						=12,
	/** This panic is raised when either iLibrary.Handle() != KNullHandle or iPtr != NULL, indicating that an assosciation has already been set
	*/
	EBafPanicLibAssocAlreadySet						=13,
	/** No path specified in sound info passed to CBaSystemSoundArray::SetSoundL()
	*/
	EBafPanicSystemSoundNoPath						=14,
	/** This panic is raised by TResourceReader::ReadTPtrC16(), when it detects that the alignment of Unicode strings within the resource is incorrect, i.e. the buffer is of an incorrect length for reading a Unicode string.
	*/
	EBafPanicUnicodeTextPaddingError				=15,
	/** no longer used
	*/
	EBafPanicOldResourceFormat						=16,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicInvalidSoundCategory					=17,
	/** Invalid system sound priority set
	*/
	EBafPanicInvalidSoundPriority					=18,
	/** NULL UID passed to CBaNamedPlugins::IndexOfUid()
	*/
	EBafPanicNullUid								=19,
	/** Bad resource file format found when constructing a CBaNamedPlugins object
	*/
	EBafPanicBadResourceFileFormat					=20,
	/** The array position at which the text representing the choice of no plug-in is to be inserted, is invalid. The panic is raised by CBaNamedPlugins::ConstructL()
	*/
	EBafPanicBadArrayPosition						=21,
	/** An invalid comparison algorithm or no comparison algorithm supplied when sorting the plug-in names in the implementation of CBaNamedPlugins::ConstructL()
	*/
	EBafPanicBadCompareNames						=22,
	/** This panic is raised when the drive specified to be a clipboard drive is invalid and non-writable
	*/
	EBafPanicBadDrive								=23,
	/** This panic is raised when there is a mismatch between the number of input bytes which are consumed and the length of the compressed unicode
	*/
	EBafPanicBadDecompression						=24,
	/** This panic is raised when the number of bytes for an index is not even
	*/
	EBafPanicIndexIsOddNumberOfBytes				=25,
	/** This panic is raised when every flag is set in iFlagsAndNumberOfResources
	*/
	EBafPanicBadNumberOfResources					=26,
	/** This panic is raised when the number of bytes cached is not equal to the size of the cache
	*/
	EBafPanicBadCacheLength							=27,
	/** This panic is raised when the number of bytes still to retrieve is not equal to the length of the buffer
	*/
	EBafPanicBadIndexLength1						=28,
	/** Length of the buffer does not equal number of bytes of index
	*/
	EBafPanicBadIndexLength2						=29,
	/** Inconsistency check failed when caching the resource file index
	*/
	EBafPanicBadIndexLength3						=30,
	/** This panic is raised if RResourceFileImpl::OpenL is called and the flag EFlagIsRomFile is not set in iFlagsAndNumberOfResources
	*/
	EBafPanicBadInitialization1						=31,
	/** This panic is raised if RResourceFileImpl::ReadHeaderAndResourceIndexL is called and no flags are set in iFlagsAndNumberOfResources
	*/
	EBafPanicBadInitialization2						=32,
	/** The value of iOffset is not equal to zero
	*/
	EBafPanicBadInitialization3						=33,
	/** Index is greater or equal to the number of bytes of the input, causing index to be out of bounds
	*/
	EBafPanicIndexOverRan							=34,
	/** __ASSERT_ALWAYS This panic is raised if a pointer is null
	*/
	EBafPanicNullPointer							=35,
	/** Data members are not initialized(probably - OpenL() hasn't been called)
	*/
	EBafPanicNotOpened								=36,
	/** Index to an array is negative, out of bounds
	*/
	EBafPanicNegativeArrayIndex						=37,
	/** Attempt to access past the end of the stream from RDictionaryCompressionBitStream::IndexOfDictionaryEntryL()
	*/
	EBafPanicEndOfStream1							=38,
	/** Attempt to access past the end of the stream from RDictionaryCompressionBitStream::ReadL()
	*/
	EBafPanicEndOfStream2							=39,
	/** This panic is raised when there are no consecutive prefix bits
	*/
	EBafPanicBadNumberOfConsecutivePrefixBits1		=40,
	/** This panic is raised when the number of consecutive prefix bits is greater than 4
	*/
	EBafPanicBadNumberOfConsecutivePrefixBits2		=41,
	/** This panic is raised if the number of bits off byte boundary is negative
	*/
	EBafPanicBadNumberOfBitsOffByteBoundary1		=42,
	/** Inconsistency check failed when decompressing a resource file
	*/
	EBafPanicBadNumberOfBitsOffByteBoundary2		=43,
	/** Panic raised when the offset value is less than zero in RDictionaryCompressionBitStream::OpenL
	*/
	EBafPanicNegativeOffsetToFirstBit1				=44,
	/** Panic raised when the offset value is less than zero in RDictionaryCompressionBitStream::EndOfStreamL
	*/
	EBafPanicNegativeOffsetToFirstBit2				=45,
	/** This panic is raised if the offset to the current bit is less than the offset to the first bit
	*/
	EBafPanicBadCurrentBitPosition1					=46,
	/** This panic is raised if the offset to the current bit is more than the offset to the last bit
	*/
	EBafPanicBadCurrentBitPosition2					=47,
	/** No bits were read from the current byte
	*/
	EBafPanicBadNumberOfBitsReadFromCurrentByte		=48,
	/** Read operation has gone over the length of the buffer to read, number of bits left to read is negative
	*/
	EBafPanicBadNumberOfBitsLeftToRead				=49,
	/** This panic is raised when the buffer has not been initialised and is NULL when a call to RDictionaryCompressionBitStream::EndOfStreamL is made
	*/
	EBafPanicNotConstructed1						=50,
	/** This panic is raised when the buffer has not been initialised and is NULL when a call to RDictionaryCompressionBitStream::IndexOfDictionaryEntryL is made
	*/
	EBafPanicNotConstructed2						=51,
	/** This panic is raised when the buffer has not been initialised and is NULL when a call to RDictionaryCompressionBitStream::ReadL is made
	*/
	EBafPanicNotConstructed3						=52,
	/** This panic is raised when the buffer has not been initialised and is NULL when a call to RDictionaryCompressionBitStream::CurrentBitIsOn is made
	*/
	EBafPanicNotConstructed4						=53,
	/** This panic is raised when the buffer has not been initialised and is NULL when a call to RDictionaryCompressionBitStream::ReadIntegerL is made
	*/
	EBafPanicNotConstructed5						=54,
	/** The length of the bit buffer is negative
	*/
	EBafPanicNegativeLengthOfBitBuffer				=55,
	/** Index to the resource's bit-array is negative
	*/
	EBafPanicNegativeResourceIndex1					=56,
	/** Index to the resource's bit-array is negative
	*/
	EBafPanicNegativeResourceIndex2					=57,
	/** Index to the resource's bit-array is negative
	*/
	EBafPanicNegativeResourceIndex3					=58,
	/** The flag EFlagDictionaryCompressed is not set in iFlagsAndNumberOfResources in call to RResourceFileImpl::AllocReadLC and flag EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource is set
	*/
	EBafPanicBadFlags1								=59,
	/** The flag EFlagThirdUidIsOffset is not set in iFlagsAndNumberOfResources in call to RResourceFileImpl::AllocReadLC and flag EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource is set
	*/
	EBafPanicBadFlags2								=60,
	/** The flag EFlagDictionaryCompressed is not set in iFlagsAndNumberOfResources in call to RResourceFileImpl::OwnsResourceIdL and flag EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource is set
	*/
	EBafPanicBadFlags3								=61,
	/** The flag EFlagThirdUidIsOffset is not set in iFlagsAndNumberOfResources in call to RResourceFileImpl::OwnsResourceIdL and flag EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource is set
	*/
	EBafPanicBadFlags4								=62,
	/** The flag EFlagDictionaryCompressed is not set in iFlagsAndNumberOfResources in call to RResourceFileImpl::OwnsResourceIdL and flag EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode is set
	*/
	EBafPanicBadFlags5								=63,
	/** The flag EFlagDictionaryCompressed is not set in iFlagsAndNumberOfResources in call to RResourceFileImpl::AllocReadLC and flag EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode is set
	*/
	EBafPanicBadFlags6								=64,
	/** Index parameter specified is less than zero, out of bounds
	*/
	EBafPanicNegativeIndexEntry						=65,
	/** This panic is raised if the size of the largest resource when completely uncompressed is not positive
	*/
	EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed1=66,
	/** This panic is raised if the size of the largest resource when completely uncompressed is not greater than 0 in RResourceFileImpl::DecompressUnicodeL
	*/
	EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed2=67,
	/** This panic is raised if the size of the largest resource when completely uncompressed is not greater than 0
	*/
	EBafPanicBadSizeOfLargestResourceWhenCompletelyUncompressed3=68,
	/** The header length is not the correct length (16+1+2+2)
	*/
	EBafPanicBadHeaderLength						=69,
	/** This panic is raised when iBuffer is NULL in call to TResourceReaderImpl::MovePtrL. DEBUG build only.
	*/
	EBafPanicNullPtr1								=70,
	/** This panic is raised when aPtr is NULL in call to TResourceReaderImpl::MovePtrL. DEBUG build only.
	*/
	EBafPanicNullPtr2								=71,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadTPtrC8L. DEBUG build only.
	*/
	EBafPanicNullPtr3								=72,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadTPtrC16L. DEBUG build only.
	*/
	EBafPanicNullPtr4								=73,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadInt8L. DEBUG build only.
	*/
	EBafPanicNullPtr5								=74,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadUint8L. DEBUG build only.
	*/
	EBafPanicNullPtr6								=75,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadInt16L. DEBUG build only.
	*/
	EBafPanicNullPtr7								=76,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadUint16L. DEBUG build only
	*/
	EBafPanicNullPtr8								=77,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadInt32L. DEBUG build only.
	*/
	EBafPanicNullPtr9								=78,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadUint32L. DEBUG build only.
	*/
	EBafPanicNullPtr10								=79,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::ReadL. DEBUG build only.
	*/
	EBafPanicNullPtr11								=80,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::RewindL. DEBUG build only.
	*/
	EBafPanicNullPtr12								=81,
	/** This panic is raised when iCurrentPtr is NULL in call to TResourceReaderImpl::AdvanceL. DEBUG build only.
	*/
	EBafPanicNullPtr13								=82,
	/** Obsolete, panic code not raised anymore
	*/
	EBafPanicNullPtr14								=83,
	/** This panic is raised when the number of bytes for an index is negative in RResourceFileImpl::ReadHeaderAndResourceIndexL
	*/
	EBafPanicNegativeNumber							=84,
	/** Panic caused by a header with length less than 18
	*/
	EBafPanicBadHeader1								=85,
	/** Panic caused by a header with length less than 18
	*/
	EBafPanicBadHeader2								=86,
	/** Panic caused by a header with length less than or equal to 10
	*/
	EBafPanicBadHeader3								=87,
	/** This panic is raised when there is a negative number of dictionary entries
	*/
	EBafPanicBadHeader4								=88,
	/** This panic is raised if the index for the start of the resource is negative
	*/
	EBafPanicBadHeader5								=89,
	/** This panic is raised if the filesize is not equal to the expected value
	*/
	EBafPanicFileSize								=90,
	/** This panic is raised if appending data to an object would cause the length to exceed the maximum length for that object
	*/
	EBafPanicOverflow								=91,
	/** This panic is raised when an index into an array is over the size of the array
	*/
	EBafPanicBadIndex								=92,
	/** This panic is raised when the number of bytes for an index is negative in RResourceFileImpl::AppendDictionaryCompressionBitStreamL
	*/
	EBafPanicNegativeNumber2						=93,
	/** This panic is raised when the filesize is not greater than in RResourceFileImpl::OpenL
	*/
	EBafPanicFileSize2								=94,
	/** This panic is raised when the number of bytes for an index is negative in RResourceFileImpl::DictionaryDecompressedResourceDataLC
	*/
	EBafPanicNegativeNumber3						=95,
	/** This panic is raised if the aPos argument to RResourceFileImpl::ReadL is less than the file offset
	*/
	EBafPanicNegativePos							=96,
	/** This panic is raised if the aLength argument to RResourceFileImpl::ReadL is negative
	*/
	EBafPanicNegativeLength							=97,
	/** This panic is raised if the aLength argument to RResourceFileImpl::ReadL is greater than the maximum length of aDes descriptor argument
	*/
	EBafPanicMaxLength								=98,
	/** This panic is raised if the index to a resource is greater than the length of the object containing the resource
	*/
	EBafPanicBadIndex2								=99,
	/** This panic is raised if the index to a resource is greater than the length of the object containing the resource
	*/
	EBafPanicBadIndex3								=100,
	/** This panic is raised if the buffer length + 1 equals or exceeds the maximum length of the buffer
	*/
	EBafPanicMaxLength2								=101,
	/** Inconsistency check failed when resding a resource file
	*/
	EBafPanicFileSize3								=102,
	/** This panic is raised when the start position is equal to or greater than the file size
	*/
	EBafPanicStartPos								=103,
	/** This panic is raised when the start position is equal to or greater than the file size
	*/
	EBafPanicStartPos2								=104,
	/** This panic is raised if the number of bytes to load will exceed the filesize from the start position
	*/
	EBafPanicFileSize4								=105,
	/** This panic is raised when the first offset is not smaller than the file size
	*/
	EBafPanicBadOffset								=106,
	/** This panic is raised when the last offset is not smaller than or equal to the file size
	*/
	EBafPanicBadOffset2								=107,
	/** Arguments inconsistent i.e reference an invalid drive or offset position 
	*/
	EBafPanicBadOpenArg								=108,
	/** This panic is raised when the buffer to append the data to will not fit that much data
	*/
	EBafPanicBufLength								=109,
	/** This panic is raised when the RFs argument to SysUtil::FFSSpaceBelowCriticalLevel, SysUtil::DiskSpaceBelowCriticalLevelL and SysUtil::MMCSpaceBelowCriticalLevelL is passed without being "connected".
	*/
	EBafPanicRFsConnectArg								=110
	};

/**
@internalComponent
*/
GLREF_C void Panic(TBafPanic aPanic);

#endif
