/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#if !defined(__CONVDATA_H__)
#define __CONVDATA_H__

/**
This structure doesn't intend to be used by external developers.
For plugin creators, all they need is to include this header file in order to compile. All the plugin data are generated from provided perl scripts. 
 
Any use of this structure externally are not proved to be compatible in future. 
@internalComponent
*/
struct SCnvConversionData
	{
/** Specifies the default endian-ness of the current character set. Used by SetDefaultEndiannessOfForeignCharacters(). */
	enum TEndianness // the values of these *must not* change they are used hard-coded in CNVTOOL.PL
		{
		EUnspecified		=0,
		EFixedLittleEndian	=1,
		EFixedBigEndian		=2
		};
	struct SVariableByteData
		{
		struct SRange
			{
			TUint8 iFirstInitialByteValueInRange;
			TUint8 iLastInitialByteValueInRange;
			TUint8 iNumberOfSubsequentBytes;
			TUint8 iSpare;
			};
		TInt iNumberOfRanges;
		const SRange* iRangeArray;
		};
	struct SOneDirectionData
		{
		struct SRange
			{
			enum TAlgorithm
				{
				// each of these, with the exception of EDirect, has a corresponding member of UData
				EDirect,
				EOffset,
				EIndexedTable16,
				EKeyedTable1616,
				EKeyedTable16OfIndexedTables16,
				EIndexedTable32,
				EKeyedTable3232,
				EKeyedTable32OfIndexedTables32
				};
			union UData
				{
				struct SIndexedTable16
					{
					struct SEntry
						{
						TUint16 iOutputCharacterCode;
						};
					const SEntry* iEntryArray;
					};
				struct SKeyedTable1616
					{
					struct SEntry
						{
						TUint16 iKey;
						TUint16 iOutputCharacterCode;
						};
					TInt iNumberOfEntries;
					const SEntry* iEntryArray;
					};
				struct SKeyedTable16OfIndexedTables16
					{
					struct SKeyedEntry
						{
						TUint16 iFirstInputCharacterCodeInIndexedTable;
						TUint16 iLastInputCharacterCodeInIndexedTable;
						const TUint16* iIndexedEntryArray;
						};
					TInt iNumberOfKeyedEntries;
					const SKeyedEntry* iKeyedEntryArray;
					};
				struct SIndexedTable32
					{
					struct SEntry
						{
						TUint32 iOutputCharacterCode;
						};
					const SEntry* iEntryArray;
					};
				struct SKeyedTable3232
					{
					struct SEntry
						{
						TUint32 iKey;
						TUint32 iOutputCharacterCode;
						};
					TInt iNumberOfEntries;
					const SEntry* iEntryArray;
					};
				struct SKeyedTable32OfIndexedTables32
					{
					struct SKeyedEntry
						{
						TUint32 iFirstInputCharacterCodeInIndexedTable;
						TUint32 iLastInputCharacterCodeInIndexedTable;
						const TUint32* iIndexedEntryArray;
						};
					TInt iNumberOfKeyedEntries;
					const SKeyedEntry* iKeyedEntryArray;
					};
				TInt iOffset;
				SIndexedTable16 iIndexedTable16;
				SKeyedTable1616 iKeyedTable1616;
				SKeyedTable16OfIndexedTables16 iKeyedTable16OfIndexedTables16;
				SIndexedTable32 iIndexedTable32;
				SKeyedTable3232 iKeyedTable3232;
				SKeyedTable32OfIndexedTables32 iKeyedTable32OfIndexedTables32;
				};
			TUint iFirstInputCharacterCodeInRange;
			TUint iLastInputCharacterCodeInRange;
			TUint8 iAlgorithm; // one of the TAlgorithm values
			TUint8 iSizeOfOutputCharacterCodeInBytesIfForeign; // only used in SCnvConversionData::iUnicodeToForeignData
			TUint16 iSpare;
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
			UData iData;
#else
			struct
				{
				TUint iWord1;
				TUint iWord2;
				} iData;
#endif
			};
		TInt iNumberOfRanges;
		const SRange* iRangeArray;
		};
	TEndianness iEndiannessOfForeignCharacters;
	SVariableByteData iForeignVariableByteData;
	SOneDirectionData iForeignToUnicodeData;
	SOneDirectionData iUnicodeToForeignData;
	TAny* iSpareForFutureUse_1;
	TAny* iSpareForFutureUse_2;
	};

/**
@internalComponent
*/
#define UData_SIndexedTable16(a) \
	reinterpret_cast<TUint>(const_cast<SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16::SEntry*>(a)) 
/**
@internalComponent
*/
#define UData_SKeyedTable1616(a) \
	static_cast<TUint>ARRAY_LENGTH(a), \
	reinterpret_cast<TUint>(const_cast<SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry*>(a))
/**
@internalComponent
*/
#define UData_SKeyedTable16OfIndexedTables16(a)	\
	static_cast<TUint>ARRAY_LENGTH(a), \
	reinterpret_cast<TUint>(const_cast<SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable16OfIndexedTables16::SKeyedEntry *>(a)) 
/**
@internalComponent
*/
#define UData_SIndexedTable32(a) \
	reinterpret_cast<TUint>(const_cast<SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable32::SEntry*>(a)) 
/**
@internalComponent
*/
#define UData_SKeyedTable3232(a) \
	static_cast<TUint>ARRAY_LENGTH(a), \
	reinterpret_cast<TUint>(const_cast<SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable3232::SEntry*>(a))
/**
@internalComponent
*/
#define UData_SKeyedTable32OfIndexedTables32(a)	\
	static_cast<TUint>ARRAY_LENGTH(a), \
	reinterpret_cast<TUint>(const_cast<SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable32OfIndexedTables32::SKeyedEntry *>(a)) 
#endif

