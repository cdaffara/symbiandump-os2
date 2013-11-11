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

#ifndef __BARSCIMPL_H__
#define __BARSCIMPL_H__

#include <e32std.h>
#include <f32file.h>

//Forward declarations
struct SDictionaryCompressionData;
class RDictionaryCompressionBitStream;
class TBaAssert;

/** Accesses a resource file and reads the resource data into a buffer.

It is the implementation class for RResourceFile and CResourceFile classes.

Because of BC reasons, the size of RResourceFileImpl must be the same as 
the size of RResourceFile class.

It is expected that all resources deallocation must be done in Close() method.
(BC reasons - RResourceFile reserves enough space for the RResourceFileImpl
instance and uses placement new operator to create it. But RResourceFile doesn't have a 
destructor, so there is no predictable way how the RResourceFileImpl 
destructor might be called, even if the class has user defined destructor).

Expected behaviour when assignment operator or copy constructor is called:
The class doesn't have assignment operator and copy constructor, so the compiler generated
ones will be used. The heap buffers used by the source class instance will be shared with the 
destination class instance.

Methods panic/leave behaviour: "L" methods panics/leave behaviour depends on iAssertObj
member (iExtra data member actually). If iAssertObj internal state is TBaAssert::EPanic - 
"L" methods will panic, otherwise they will leave if something goes wrong.

@internalComponent 
@see RResourceFile 
@see CResourceFile */
class RResourceFileImpl
	{
public:
	RResourceFileImpl();
	~RResourceFileImpl();
	void OpenL(
		RFs& aFs, 
		const TDesC& aName, 
		const TBaAssert& aAssert,
		TUint aFileOffset = 0, 
		TInt aFileSize = 0);
	void OpenL(const TDesC8& aRscArchive,const TBaAssert& aAssert);
	void Close();
	TUidType UidType() const;
	void ReadL(TDes8 &aDes, TInt aResourceId) const;
	HBufC8 *AllocReadLC(TInt aResourceId) const;
	void ConfirmSignatureL(TInt aSignature);
	void ConfirmSignatureL();
	TInt SignatureL() const;
	TBool OwnsResourceIdL(TInt aResourceId) const;
	TInt Offset() const;
private:
	struct SSigRecord
		{
		TInt signature;
		TInt offset;
		};
	enum
		{
		EFlagIsRomFile																=0x80000000,
		EFlagPotentiallyContainsCompressedUnicode									=0x40000000,
		EFlagDictionaryCompressed													=0x20000000,
		EFlagThirdUidIsOffset														=0x10000000,
		EFlagGenerate_RSS_SIGNATURE_ForFirstUserResource							=0x08000000,
		EFlagFirstResourceIsGeneratedBitArrayOfResourcesContainingCompressedUnicode	=0x04000000,
 		EFlagCalypsoFileFormat														=0x02000000,
 		EFlagIsBufferRscFile														=0x01000000,
		EAllFlags																	=0xff000000
		};
	enum
		{
		EOffsetBits	=0xfffff000,
		EIdBits		=0x00000fff
		};
	class TExtra;
private:
	void ReadL(TInt aPos,TDes8& aDes,TInt aLength) const;
	SSigRecord FirstRecordL() const;

	void AppendDecompressedUnicodeL(TDes8& aBuffer,const TDesC8& aCompressedUnicode) const;
	HBufC8* DecompressUnicodeL(
		const TDesC8& aDictionaryDecompressedResourceData,
		TInt aSizeOfLargestResourceWhenCompletelyUncompressed) const;
	HBufC8* DictionaryDecompressedResourceDataLC(
		TInt aResourceIndex,
		TUint aFlags,
		const SDictionaryCompressionData& aDictionaryCompressionData,
		const TDesC16& aIndex) const;
	void AppendDictionaryCompressionBitStreamL(
		RArray<RDictionaryCompressionBitStream>& aStackOfDictionaryCompressionBitStreams,
		TUint aFlags,
		const SDictionaryCompressionData& aDictionaryCompressionData,
		TInt aStartOfBitData,
		TInt aStartOfIndex,
		TInt aIndexEntry) const;
	TInt LittleEndianTwoByteIntegerL(const TDesC8& aBuffer,TInt aIndexOfFirstByte) const;
	void ReadL(TUint aFlags,TInt aPos,TDes8& aDes,TInt aLength) const;
	void ReadHeaderAndResourceIndexL();

private:
	TAny* iFile; // either an RFile*, or a const TUint8* pointing into ROM
	TInt iSizeOfLargestResourceWhenCompletelyUncompressed;
	//basically an array of (unsigned) 16-bit file-positions - 
	//this is only used for non-dictionary-compressed resource files
	HBufC16* iIndex; 
	//the position of this member in the class is exposed because RResourceFile::Offset() is 
	//an inline function accessing "iOffset". RResourceFileImpl is an implementation class for
	//RResourceFile class. The "iOffset" offset from the beginning of the class must be exactly
	//12 bytes.
	TInt iOffset; 
	TExtra* iExtra;
	TUint iFlagsAndNumberOfResources;
	};

#endif//__BARSCIMPL_H__
