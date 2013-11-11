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


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include <convdata.h>
#include <utf.h>
#include <ecom/ecom.h>
#include <charactersetconverter.h>
#include "chcnvpanic.h"
#include "charconv_tls.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "charconvTraces.h"
#endif


#if defined(_DEBUG)
#define NON_DEBUG_INLINE
#else
#define NON_DEBUG_INLINE inline
#endif

GLREF_D const SCnvConversionData codePage1252ConversionData;
GLREF_D const SCnvConversionData iso88591ConversionData;
GLREF_D const SCnvConversionData asciiConversionData;
GLREF_D const SCnvConversionData sms7BitConversionData;
GLREF_D const SCnvConversionData unicodeConversionDataLittle;
GLREF_D const SCnvConversionData unicodeConversionDataBig;

GLREF_C void IsCharacterSetAscii(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetSMS7Bit(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetISO88591(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetCP1252(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetUTF8(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetUTF7(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetUnicodeLittle(TInt& aConfidenceLevel, const TDesC8& aSample);
GLREF_C void IsCharacterSetUnicodeBig(TInt& aConfidenceLevel, const TDesC8& aSample);

_LIT(KLitWildCard, "*");

_LIT(KLitSystemCharconvDirectory, "\\resource\\charconv\\");
_LIT(KLitROMSystemCharconvDirectory, "z:\\resource\\charconv\\");

_LIT(KLitCharacterSetNameUtf7, "UTF-7");
_LIT(KLitCharacterSetNameUtf8, "UTF-8");
_LIT(KLitCharacterSetNameImapUtf7, "IMAP UTF-7");
_LIT(KLitCharacterSetNameJavaConformantUtf8, "Java UTF-8");
_LIT(KLitCharacterSetNameCodePage1252, "Code Page 1252");
_LIT(KLitCharacterSetNameIso88591, "ISO-8859-1");
_LIT(KLitCharacterSetNameAscii, "ASCII");
_LIT(KLitCharacterSetNameUnicodeLittle, "Little-Endian UNICODE");
_LIT(KLitCharacterSetNameUnicodeBig, "Big-Endian UNICODE");
_LIT(KLitCharacterSetNameSms7Bit, "SMS 7-bit");
_LIT8(KLit8AsciiSubstituteCharacter, "\x1a");
_LIT8(KLit8Sms7BitSubstituteCharacter, "\x3f");

const TUint KNoConversionAvailable=KMaxTUint;

const TUint KCharacterSetIdentifierWin1252=0x101F873B; // same as built-in cp1252, just to be compliance with S60 CP1252

enum
	{
	EConversionDataFilePositionOfName=24,
	EConversionDataFileMaximumLengthOfName=KMaxTUint8
	};

LOCAL_C TBool StandardNamesMatch(const TDesC8& aStandardName1, const TDesC8& aStandardName2)
	{
	return aStandardName1.CompareF(aStandardName2)==0; // "no distinction is made between use of upper and lower case letters" as stated by ftp.isi.edu/in-notes/iana/assignments/character-sets
	}
	
LOCAL_C void ResetAndDestroyRImplInfoPtrArray(TAny* aPtr)
	{
	RImplInfoPtrArray* array = reinterpret_cast <RImplInfoPtrArray*> (aPtr);
	array->ResetAndDestroy();
	}

LOCAL_C void CloseECOMSession(TAny*)
	{
	REComSession::FinalClose();
	}

static void IsBuiltInCharacterSet(const TUint& aCharacterSetIdentifier, 
								  TInt& aConfidenceLevel, 
								  const TDesC8& aSample)
    {
	switch(aCharacterSetIdentifier)
	    {
		case KCharacterSetIdentifierUtf7:
		    IsCharacterSetUTF7(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierUtf8:
			IsCharacterSetUTF8(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierImapUtf7:
		case KCharacterSetIdentifierJavaConformantUtf8:
			break;
		case KCharacterSetIdentifierCodePage1252:
		case KCharacterSetIdentifierWin1252:
			IsCharacterSetCP1252(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierIso88591:
			IsCharacterSetISO88591(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierAscii:
			IsCharacterSetAscii(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierSms7Bit:
			IsCharacterSetSMS7Bit(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierUnicodeLittle:
			IsCharacterSetUnicodeLittle(aConfidenceLevel,aSample);
			break;
		case KCharacterSetIdentifierUnicodeBig:
			IsCharacterSetUnicodeBig(aConfidenceLevel,aSample);
			break;
#if defined(_DEBUG)
		default:
		    OstTraceExt3( TRACE_DUMP, _ISBUILTINCHARACTERSET, "::IsBuiltInCharacterSet;aCharacterSetIdentifier=%u;aConfidenceLevel=%d;aSample=%x", aCharacterSetIdentifier, aConfidenceLevel, ( TUint )&( aSample ) );	    
			Panic(EPanicCharacterSetNotPresent);
			break;
#endif
		}
    }
    

void TTlsData::CharacterSetConverterIsBeingCreatedL()
	{
	TTlsData* tlsData=STATIC_CAST(TTlsData*, Dll::Tls());
	if (tlsData!=NULL)
		{
		++tlsData->iReferenceCount;
		}
	else
		{
		tlsData=new(ELeave) TTlsData;
		CleanupStack::PushL(tlsData);
		User::LeaveIfError(Dll::SetTls(tlsData));
		CleanupStack::Pop(); // tlsData
		}
	}

void TTlsData::CharacterSetConverterIsBeingDestroyed()
	{
	TTlsData* tlsData=STATIC_CAST(TTlsData*, Dll::Tls());
	if (tlsData!=NULL)
		{
		if ( tlsData->iCurrentCharacterSetConverter!=NULL )
		    {
		    OstTrace0( TRACE_DUMP, TTLSDATA_CHARACTERSETCONVERTERISBEINGDESTROYED, "EPanicDestructionDuringConversion" );	    
		    }
		__ASSERT_DEBUG(tlsData->iCurrentCharacterSetConverter==NULL, Panic(EPanicDestructionDuringConversion));
		--tlsData->iReferenceCount;
		if ( tlsData->iReferenceCount < 0 )
		    {
		    OstTrace0( TRACE_DUMP, DUP1_TTLSDATA_CHARACTERSETCONVERTERISBEINGDESTROYED, "EPanicBadTlsDataReferenceCount" );
		    }
		__ASSERT_DEBUG(tlsData->iReferenceCount>=0, Panic(EPanicBadTlsDataReferenceCount));
		if (tlsData->iReferenceCount<=0)
			{
			delete tlsData;
			Dll::FreeTls();
			}
		}
	}

void TTlsData::SetCurrentCharacterSetConverter(const CCnvCharacterSetConverter* aCharacterSetConverter)
	{
	TTlsData* tlsData=STATIC_CAST(TTlsData*, Dll::Tls());
	if ( tlsData==NULL )
	    {
	    OstTrace0( TRACE_FATAL, DUP1_TTLSDATA_SETCURRENTCHARACTERSETCONVERTER, "No Tls Data in TTlsData::SetCurrentCharacterSetConverter" );
	    }
	__ASSERT_ALWAYS(tlsData!=NULL, Panic(EPanicNoTlsData));
    if ( (tlsData->iCurrentCharacterSetConverter==NULL)==(aCharacterSetConverter==NULL) )
        {
        OstTrace0( TRACE_FATAL, TTLSDATA_SETCURRENTCHARACTERSETCONVERTER, "Bad Toggle of current characater set converter in TTlsData::SetCurrentCharacterSetConverter" );  
        }
	__ASSERT_ALWAYS((tlsData->iCurrentCharacterSetConverter==NULL)!=(aCharacterSetConverter==NULL), Panic(EPanicBadToggleOfCurrentCharacterSetConverter));
	tlsData->iCurrentCharacterSetConverter=aCharacterSetConverter;
	}

EXPORT_C const CCnvCharacterSetConverter* TTlsData::CurrentCharacterSetConverter()
	{
	TTlsData* tlsData=STATIC_CAST(TTlsData*, Dll::Tls());
	return (tlsData!=NULL)? tlsData->iCurrentCharacterSetConverter: NULL;
	}

// CDeepDestructingArrayOfCharactersSets

NONSHARABLE_CLASS(CDeepDestructingArrayOfCharactersSets) : public CArrayFixFlat<CCnvCharacterSetConverter::SCharacterSet>
	{
public:
	static CDeepDestructingArrayOfCharactersSets* NewLC(TInt aGranularity);
	virtual ~CDeepDestructingArrayOfCharactersSets();
private:
	CDeepDestructingArrayOfCharactersSets(TInt aGranularity);
	};

CDeepDestructingArrayOfCharactersSets* CDeepDestructingArrayOfCharactersSets::NewLC(TInt aGranularity)
	{
	CDeepDestructingArrayOfCharactersSets* deepDestructingArrayOfCharactersSets=new(ELeave) CDeepDestructingArrayOfCharactersSets(aGranularity);
	CleanupStack::PushL(deepDestructingArrayOfCharactersSets);
	return deepDestructingArrayOfCharactersSets;
	}

CDeepDestructingArrayOfCharactersSets::~CDeepDestructingArrayOfCharactersSets()
	{
	for (TInt i=Count()-1; i>=0; --i)
		{
		delete (*this)[i].iName;
		}
	}

CDeepDestructingArrayOfCharactersSets::CDeepDestructingArrayOfCharactersSets(TInt aGranularity)
	:CArrayFixFlat<CCnvCharacterSetConverter::SCharacterSet>(aGranularity)
	{
	}

// CFileReader

NONSHARABLE_CLASS(CFileReader) : public CBase
	{
public:
	static CFileReader* NewLC(RFile& aFile);
	static CFileReader* NewLC(const TUint8* aRomFile, TInt aLengthOfRomFile);
	virtual ~CFileReader();
	void SkipL(TInt aNumberOfBytes);
	TInt ReadUint8L();
	TInt ReadUint16L();
	TUint ReadUint32L();
	TInt ReadPositiveIntegerCompacted15L();
	TInt ReadPositiveIntegerCompacted30L();
	TInt ReadSignedIntegerCompacted29L();
	void ReadBufferL(TDes8& aBuffer, TInt aBufferLength);
	HBufC8* ReadBufferL(TInt aBufferLength);
	HBufC8* ReadBufferLC(TInt aBufferLength);
	inline TBool IsEndOfFile() const {if ( iNextByteToConsume>iOnePastEndOfBuffer ) {OstTrace0( TRACE_DUMP, _NONSHARABLE_CLASS, "EPanicPastEndOfFile" );}__ASSERT_DEBUG(iNextByteToConsume<=iOnePastEndOfBuffer, Panic(EPanicPastEndOfFile)); return iNextByteToConsume>=iOnePastEndOfBuffer;}
private:
	enum {ENumberOfBytesToConsumeBetweenEachReAllocation=1000};
private:
	CFileReader();
	CFileReader(const TUint8* aRomFile, TInt aLengthOfRomFile);
	void ConstructForNonRomFileL(RFile& aFile);
	NON_DEBUG_INLINE void ReAllocateTheBuffer();
#if defined(_DEBUG)
	inline void CheckPointers(TPanic aPanic) {__ASSERT_DEBUG((iNextByteToConsume!=NULL) && (iOnePastEndOfBuffer!=NULL) && (iNextByteToConsume<=iOnePastEndOfBuffer), Panic(aPanic)); if (iBuffer==NULL) {__ASSERT_DEBUG(iFlagPoleForReAllocation==NULL, Panic(aPanic));} else {__ASSERT_DEBUG(iNextByteToConsume<iFlagPoleForReAllocation, Panic(aPanic));}}
#else
	inline void CheckPointers(TPanic) {}
#endif
private:
	TUint8* iBuffer;
	const TUint8* iNextByteToConsume;
	const TUint8* iOnePastEndOfBuffer;
	const TUint8* iFlagPoleForReAllocation;
	};

void CFileReader::ReAllocateTheBuffer() // put this function first so that the compiler does actually inline it for non-DEBUG builds
	{
	if ( (iBuffer==NULL) || (iNextByteToConsume<iFlagPoleForReAllocation ) )
	    {
	    OstTrace0( TRACE_DUMP, CFILEREADER_REALLOCATETHEBUFFER, "EPanicNotPastFlagPoleForReAllocation" );
	    }
	
	__ASSERT_DEBUG((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation), Panic(EPanicNotPastFlagPoleForReAllocation));
	const TInt lengthOfBuffer=iOnePastEndOfBuffer-iNextByteToConsume;
	Mem::Copy(STATIC_CAST(TAny*, iBuffer), iNextByteToConsume, lengthOfBuffer);
	const TAny* reAllocatedCell=
	        User::ReAlloc(STATIC_CAST(TAny*, iBuffer), lengthOfBuffer);
	if ( reAllocatedCell!=iBuffer )
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CFILEREADER_REALLOCATETHEBUFFER, "EPanicReAllocatedCellMoved" );
	    }
	__ASSERT_DEBUG(reAllocatedCell==iBuffer, Panic(EPanicReAllocatedCellMoved));
	iNextByteToConsume=iBuffer;
	iOnePastEndOfBuffer=iBuffer+lengthOfBuffer;
	// iFlagPoleForReAllocation can stay as it is
	}

CFileReader* CFileReader::NewLC(RFile& aFile)
	{
	CFileReader* fileReader=new(ELeave) CFileReader;
	CleanupStack::PushL(fileReader);
	fileReader->ConstructForNonRomFileL(aFile);
	return fileReader;
	}

CFileReader* CFileReader::NewLC(const TUint8* aRomFile, TInt aLengthOfRomFile)
	{
	CFileReader* fileReader=new(ELeave) CFileReader(aRomFile, aLengthOfRomFile);
	CleanupStack::PushL(fileReader);
	return fileReader;
	}

CFileReader::~CFileReader()
	{
	User::Free(iBuffer);
	}

void CFileReader::SkipL(TInt aNumberOfBytes)
	{
	if ( aNumberOfBytes<0 )
	    {
	    OstTrace0( TRACE_DUMP, CFILEREADER_SKIPL, "EPanicNegativeNumberOfBytes" );
	    }
	__ASSERT_DEBUG(aNumberOfBytes>=0, Panic(EPanicNegativeNumberOfBytes));
	CheckPointers(EPanicInconsistentFileReader1);
	const TUint8* newNextByteToConsume=iNextByteToConsume+aNumberOfBytes;
	if (newNextByteToConsume>iOnePastEndOfBuffer)
		{
		OstTrace0( TRACE_FATAL, DUP2_CFILEREADER_SKIPL, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	iNextByteToConsume=newNextByteToConsume;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader2);
	}

TInt CFileReader::ReadUint8L()
	{
	CheckPointers(EPanicInconsistentFileReader3);
	const TUint8* newNextByteToConsume=iNextByteToConsume+sizeof(TUint8);
	if (newNextByteToConsume>iOnePastEndOfBuffer)
		{
		User::Leave(KErrCorrupt);
		}
	const TInt integer=*iNextByteToConsume;
	iNextByteToConsume=newNextByteToConsume;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader4);
	return integer;
	}

TInt CFileReader::ReadUint16L() // little-endian
	{
	CheckPointers(EPanicInconsistentFileReader5);
	const TUint8* newNextByteToConsume=iNextByteToConsume+sizeof(TUint16);
	if (newNextByteToConsume>iOnePastEndOfBuffer)
		{
		OstTrace0( TRACE_FATAL, CFILEREADER_READUINT16L, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	const TInt integer=(*iNextByteToConsume|(*(iNextByteToConsume+1)<<8));
	iNextByteToConsume=newNextByteToConsume;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader6);
	return integer;
	}

TUint CFileReader::ReadUint32L() // little-endian
	{
	CheckPointers(EPanicInconsistentFileReader7);
	const TUint8* newNextByteToConsume=iNextByteToConsume+sizeof(TUint32);
	if (newNextByteToConsume>iOnePastEndOfBuffer)
		{
		User::Leave(KErrCorrupt);
		}
	const TInt integer=(*iNextByteToConsume|(*(iNextByteToConsume+1)<<8)|(*(iNextByteToConsume+2)<<16)|(*(iNextByteToConsume+3)<<24));
	iNextByteToConsume=newNextByteToConsume;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader8);
	return integer;
	}

TInt CFileReader::ReadPositiveIntegerCompacted15L() // big-endian
	{
	CheckPointers(EPanicInconsistentFileReader9);
	const TUint8* bytePointer=iNextByteToConsume;
	if (bytePointer>=iOnePastEndOfBuffer)
		{
		User::Leave(KErrCorrupt);
		}
	TInt integer=*bytePointer;
	if (integer&0x00000080)
		{
		integer&=~0x00000080;
		++bytePointer;
		if (bytePointer>=iOnePastEndOfBuffer)
			{
			User::Leave(KErrCorrupt);
			}
		integer<<=8;
		integer|=*bytePointer;
		}
	++bytePointer;
	iNextByteToConsume=bytePointer;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader10);
	return integer;
	}

TInt CFileReader::ReadPositiveIntegerCompacted30L() // big-endian
	{
	CheckPointers(EPanicInconsistentFileReader11);
	const TUint8* bytePointer=iNextByteToConsume;
	if (bytePointer>=iOnePastEndOfBuffer)
		{
		OstTrace0( TRACE_FATAL, CFILEREADER_READPOSITIVEINTEGERCOMPACTED30L, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	TInt integer=*bytePointer;
	if (integer&0x00000080)
		{
		integer&=~0x00000080;
		++bytePointer;
		if (bytePointer>=iOnePastEndOfBuffer)
			{
			OstTrace0( TRACE_FATAL, DUP1_CFILEREADER_READPOSITIVEINTEGERCOMPACTED30L, "KErrCorrupt" );
			User::Leave(KErrCorrupt);
			}
		integer<<=8;
		integer|=*bytePointer;
		if (integer&0x00004000)
			{
			integer&=~0x00004000;
			if (bytePointer+2>=iOnePastEndOfBuffer)
				{
				OstTrace0( TRACE_FATAL, DUP2_CFILEREADER_READPOSITIVEINTEGERCOMPACTED30L, "KErrCorrupt" );
				User::Leave(KErrCorrupt);
				}
			++bytePointer;
			integer<<=8;
			integer|=*bytePointer;
			++bytePointer;
			integer<<=8;
			integer|=*bytePointer;
			}
		}
	++bytePointer;
	iNextByteToConsume=bytePointer;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader12);
	return integer;
	}

TInt CFileReader::ReadSignedIntegerCompacted29L() // big-endian
	{
	CheckPointers(EPanicInconsistentFileReader13);
	const TUint8* bytePointer=iNextByteToConsume;
	if (bytePointer>=iOnePastEndOfBuffer)
		{
		OstTrace0( TRACE_FATAL, CFILEREADER_READSIGNEDINTEGERCOMPACTED29L, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	TInt integer=*bytePointer;
	const TBool isNegative=(integer&0x00000080);
	integer&=~0x00000080;
	if (integer&0x00000040)
		{
		integer&=~0x00000040;
		++bytePointer;
		if (bytePointer>=iOnePastEndOfBuffer)
			{
			OstTrace0( TRACE_FATAL, DUP1_CFILEREADER_READSIGNEDINTEGERCOMPACTED29L, "KErrCorrupt" );
			User::Leave(KErrCorrupt);
			}
		integer<<=8;
		integer|=*bytePointer;
		if (integer&0x00002000)
			{
			integer&=~0x00002000;
			if (bytePointer+2>=iOnePastEndOfBuffer)
				{
				OstTrace0( TRACE_FATAL, DUP2_CFILEREADER_READSIGNEDINTEGERCOMPACTED29L, "KErrCorrupt" );
				User::Leave(KErrCorrupt);
				}
			++bytePointer;
			integer<<=8;
			integer|=*bytePointer;
			++bytePointer;
			integer<<=8;
			integer|=*bytePointer;
			}
		}
	++bytePointer;
	if (isNegative)
		{
		integer=-integer;
		}
	iNextByteToConsume=bytePointer;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader14);
	return isNegative? -integer: integer;
	}

void CFileReader::ReadBufferL(TDes8& aBuffer, TInt aBufferLength)
	{
	if ( aBufferLength<0 )
	    {
	    OstTrace0( TRACE_DUMP, CFILEREADER_READBUFFERL, "EPanicNegativeBufferLength1" );
	    }
	__ASSERT_DEBUG(aBufferLength>=0, Panic(EPanicNegativeBufferLength1));
	CheckPointers(EPanicInconsistentFileReader15);
	const TUint8* newNextByteToConsume=iNextByteToConsume+aBufferLength;
	if (newNextByteToConsume>iOnePastEndOfBuffer)
		{
		OstTrace0( TRACE_FATAL, DUP1_CFILEREADER_READBUFFERL, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	aBuffer=TPtrC8(iNextByteToConsume, aBufferLength);
	iNextByteToConsume=newNextByteToConsume;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader16);
	}

HBufC8* CFileReader::ReadBufferL(TInt aBufferLength)
	{
	if ( aBufferLength<0 )
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CFILEREADER_READBUFFERL, "EPanicNegativeBufferLength2" );
	    }
	__ASSERT_DEBUG(aBufferLength>=0, Panic(EPanicNegativeBufferLength2));
	CheckPointers(EPanicInconsistentFileReader17);
	const TUint8* newNextByteToConsume=iNextByteToConsume+aBufferLength;
	if (newNextByteToConsume>iOnePastEndOfBuffer)
		{
		OstTrace0( TRACE_FATAL, DUP3_CFILEREADER_READBUFFERL, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	HBufC8* buffer=TPtrC8(iNextByteToConsume, aBufferLength).AllocL();
	iNextByteToConsume=newNextByteToConsume;
	if ((iBuffer!=NULL) && (iNextByteToConsume>=iFlagPoleForReAllocation))
		{
		ReAllocateTheBuffer();
		}
	CheckPointers(EPanicInconsistentFileReader18);
	return buffer;
	}

HBufC8* CFileReader::ReadBufferLC(TInt aBufferLength)
	{
	HBufC8* buffer=ReadBufferL(aBufferLength);
	CleanupStack::PushL(buffer);
	return buffer;
	}

CFileReader::CFileReader()
	:iBuffer(NULL),
	 iNextByteToConsume(NULL),
	 iOnePastEndOfBuffer(NULL),
	 iFlagPoleForReAllocation(NULL)
	{
	}

CFileReader::CFileReader(const TUint8* aRomFile, TInt aLengthOfRomFile)
	:iBuffer(NULL),
	 iNextByteToConsume(aRomFile),
	 iOnePastEndOfBuffer(aRomFile+aLengthOfRomFile),
	 iFlagPoleForReAllocation(NULL)
	{
	}

void CFileReader::ConstructForNonRomFileL(RFile& aFile)
	{
	TInt lengthOfBuffer;
	if ( aFile.Size(lengthOfBuffer) < 0 )
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CFILEREADER_CONSTRUCTFORNONROMFILEL, "aFile.Size(lengthOfBuffer) < 0" );
	    }
	User::LeaveIfError(aFile.Size(lengthOfBuffer));
	iBuffer=STATIC_CAST(TUint8*, User::AllocL(lengthOfBuffer+1));
	TPtr8 buffer(iBuffer, 0, lengthOfBuffer);
	User::LeaveIfError(aFile.Read(buffer));
	if ((buffer.Length()!=lengthOfBuffer) || (lengthOfBuffer<=0))
		{
		OstTrace0( TRACE_FATAL, CFILEREADER_CONSTRUCTFORNONROMFILEL, "KErrCorrupt" );
		User::Leave(KErrCorrupt);
		}
	iNextByteToConsume=iBuffer;
	iOnePastEndOfBuffer=iBuffer+lengthOfBuffer;
	iFlagPoleForReAllocation=iBuffer+ENumberOfBytesToConsumeBetweenEachReAllocation;
	}

// CStandardNamesAndMibEnums

NONSHARABLE_CLASS(CStandardNamesAndMibEnums) : public CBase
	{
public:
	static CStandardNamesAndMibEnums* NewLC();
	virtual ~CStandardNamesAndMibEnums();
	void AddFromFileL(RFs& aFileServerSession, const TDesC& aFileName);
	TUint Identifier(const TDesC8& aStandardName) const;
	TUint Identifier(TInt aMibEnum) const;
	HBufC8* StandardNameL(TUint aIdentifier) const;
	TInt MibEnum(TUint aIdentifier) const;
private:
	/** Stores information about a non-Unicode character set. The information 
	is used to locate the conversion information required by ConvertFromUnicode() 
	and	ConvertToUnicode().

	An array of these structs, which contains all available character sets, 
	can be generated by CreateArrayOfCharacterSetsAvailableLC() and 
	CreateArrayOfCharacterSetsAvailableL(), 
	and is used by one of the overloads of PrepareToConvertToOrFromL(). */
	struct SCharacterSet
		{
		inline SCharacterSet(TUint aIdentifier, TInt aNumberOfStandardNames, TInt aNumberOfMibEnums) :iIdentifier(aIdentifier), iArrayOfStandardNames(Max(aNumberOfStandardNames, 1)), iArrayOfMibEnums(Max(aNumberOfMibEnums, 1)) {}
		const TUint iIdentifier;
		RPointerArray<HBufC8> iArrayOfStandardNames;
		RArray<TInt> iArrayOfMibEnums;
		};
private:
	CStandardNamesAndMibEnums();
	TBool CharacterSetExists(TUint aIdentifier, TInt& aIndex) const;
	static TInt CompareFunction(const SCharacterSet& aCharacterSet1, const SCharacterSet& aCharacterSet2);
private:
	RArray<SCharacterSet> iArrayOfCharacterSets;
	};

CStandardNamesAndMibEnums* CStandardNamesAndMibEnums::NewLC()
	{
	CStandardNamesAndMibEnums* standardNamesAndMibEnums=new(ELeave) CStandardNamesAndMibEnums;
	CleanupStack::PushL(standardNamesAndMibEnums);
	return standardNamesAndMibEnums;
	}

CStandardNamesAndMibEnums::~CStandardNamesAndMibEnums()
	{
	for (TInt i=iArrayOfCharacterSets.Count()-1; i>=0; --i)
		{
		SCharacterSet& characterSet=iArrayOfCharacterSets[i];
		characterSet.iArrayOfStandardNames.ResetAndDestroy();
		characterSet.iArrayOfStandardNames.Close();
		characterSet.iArrayOfMibEnums.Close();
		}
	iArrayOfCharacterSets.Close();
	}

void CStandardNamesAndMibEnums::AddFromFileL(RFs& aFileServerSession, const TDesC& aFileName)
	{
	const TUint8* const romFile=aFileServerSession.IsFileInRom(aFileName);
	RFile file;
	CFileReader* fileReader=NULL;
	if (romFile!=NULL)
		{
		TEntry entry;
		User::LeaveIfError(aFileServerSession.Entry(aFileName, entry));
		fileReader=CFileReader::NewLC(romFile, entry.iSize);
		}
	else
		{
		CleanupClosePushL(file);
		User::LeaveIfError(file.Open(aFileServerSession, aFileName, EFileShareReadersOnly|EFileStream|EFileRead));
		fileReader=CFileReader::NewLC(file);
		}
	fileReader->SkipL(16); // skip the UIDs
	while (!fileReader->IsEndOfFile())
		{
		const TUint identifier=fileReader->ReadUint32L();
		const TInt numberOfStandardNames=fileReader->ReadPositiveIntegerCompacted15L();
		const TInt numberOfMibEnums=fileReader->ReadPositiveIntegerCompacted15L();
		SCharacterSet* characterSet=NULL;
		TInt indexOfCharacterSet;
		if (CharacterSetExists(identifier, indexOfCharacterSet))
			{
			characterSet=&iArrayOfCharacterSets[indexOfCharacterSet];
			}
		else
			{
			User::LeaveIfError(iArrayOfCharacterSets.Insert(SCharacterSet(identifier, numberOfStandardNames, numberOfMibEnums), indexOfCharacterSet));
			characterSet=&iArrayOfCharacterSets[indexOfCharacterSet];
			}
		TInt i;
		for (i=0; i<numberOfStandardNames; ++i)
			{
			const TInt lengthOfStandardName=fileReader->ReadPositiveIntegerCompacted15L();
			User::LeaveIfError(characterSet->iArrayOfStandardNames.Append(fileReader->ReadBufferLC(lengthOfStandardName)));
			CleanupStack::Pop(); // fileReader->ReadBufferLC(lengthOfStandardName)
			}
		for (i=0; i<numberOfMibEnums; ++i)
			{
			User::LeaveIfError(characterSet->iArrayOfMibEnums.Append(fileReader->ReadPositiveIntegerCompacted30L()));
			}
		}
	CleanupStack::PopAndDestroy((romFile!=NULL)? 1: 2); // fileReader and (possibly) file
	}


TUint CStandardNamesAndMibEnums::Identifier(const TDesC8& aStandardName) const
	{
	for (TInt i=iArrayOfCharacterSets.Count()-1; i>=0; --i)
		{
		const SCharacterSet& characterSet=iArrayOfCharacterSets[i];
		for (TInt j=characterSet.iArrayOfStandardNames.Count()-1; j>=0; --j)
			{
			if (StandardNamesMatch(*characterSet.iArrayOfStandardNames[j], aStandardName))
				{
				return characterSet.iIdentifier;
				}
			}
		}
	return 0;
	}

TUint CStandardNamesAndMibEnums::Identifier(TInt aMibEnum) const
	{
	for (TInt i=iArrayOfCharacterSets.Count()-1; i>=0; --i)
		{
		const SCharacterSet& characterSet=iArrayOfCharacterSets[i];
		for (TInt j=characterSet.iArrayOfMibEnums.Count()-1; j>=0; --j)
			{
			if (characterSet.iArrayOfMibEnums[j]==aMibEnum)
				{
				return characterSet.iIdentifier;
				}
			}
		}
	return 0;
	}

HBufC8* CStandardNamesAndMibEnums::StandardNameL(TUint aIdentifier) const
	{
	TInt indexOfCharacterSet;
	if (CharacterSetExists(aIdentifier, indexOfCharacterSet))
		{
		const RPointerArray<HBufC8>& arrayOfStandardNames=iArrayOfCharacterSets[indexOfCharacterSet].iArrayOfStandardNames;
		if (arrayOfStandardNames.Count()>0)
			{
			return arrayOfStandardNames[0]->AllocL();
			}
		}
	return NULL;
	}

TInt CStandardNamesAndMibEnums::MibEnum(TUint aIdentifier) const
	{
	TInt indexOfCharacterSet;
	if (CharacterSetExists(aIdentifier, indexOfCharacterSet))
		{
		const RArray<TInt>& arrayOfMibEnums=iArrayOfCharacterSets[indexOfCharacterSet].iArrayOfMibEnums;
		if (arrayOfMibEnums.Count()>0)
			{
			return arrayOfMibEnums[0];
			}
		}
	return 0;
	}

CStandardNamesAndMibEnums::CStandardNamesAndMibEnums()
	:iArrayOfCharacterSets(5)
	{
	}

TBool CStandardNamesAndMibEnums::CharacterSetExists(TUint aIdentifier, TInt& aIndex) const
	{
	TUint characterSet[sizeof(SCharacterSet)/sizeof(TUint)]; // use this as a pretend SCharacterSet object (the only field of it that will be used is iIdentifier)
	characterSet[_FOFF(SCharacterSet, iIdentifier)/sizeof(TUint)]=aIdentifier;
#if defined(_DEBUG)
	for (TInt i=iArrayOfCharacterSets.Count()-1; i>0; --i) // i>0 (rather than i>=0) is correct as we are comparing the character set identifier at [i] with the one at [i-1]
		{
		__ASSERT_DEBUG(CompareFunction(iArrayOfCharacterSets[i-1], iArrayOfCharacterSets[i])<0, Panic(EPanicCharacterSetsNotSorted));
		}
#endif
	return iArrayOfCharacterSets.FindInOrder(*REINTERPRET_CAST(const SCharacterSet*, characterSet), aIndex, TLinearOrder<SCharacterSet>(CompareFunction))==KErrNone;
	}

TInt CStandardNamesAndMibEnums::CompareFunction(const SCharacterSet& aCharacterSet1, const SCharacterSet& aCharacterSet2)
	{
	return aCharacterSet2.iIdentifier-aCharacterSet1.iIdentifier;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CCharsetCnvCache

/**
CCharsetCnvCache is used to accelerate the access to character set converter implementations.
On a client request the CCharsetCnvCache instance will load the required character set converter 
implementation if it is not loaded yet and give it to the client. If the requested 
character set converter is in the cache, it will be used and no time will be spend for loading it.
The type of the used caching algorithm is LRU.
The implementation UID of the converter is used as a key, uniquely identifying cache entries.
GetConverterL()/Converter() return results should not be stored across the calls. The cache
has limited max size. It will remove the last entries if it reaches its max limit - that may
make stored GetConverterL()/Converter() return results invalid.
@internalComponent
*/
NONSHARABLE_CLASS(CCharsetCnvCache) : public CBase
	{
public:
	enum 
		{
		/** Default cache size*/
		KDefaultMaxCacheSize = 32
		};
	enum
		{
		/** Min allowed cache size*/
		KMinCacheSize = 4
		};
		
public:
	static CCharsetCnvCache* NewL();
	virtual ~CCharsetCnvCache();

	CCharacterSetConverterPluginInterface* GetConverterL(TUid aImplUid);
	CCharacterSetConverterPluginInterface* Converter(TUid aImplUid);
	void SetMaxSize(TInt aSize);

private:
	/**
	The class represents an entry in CCharsetCnvCache cache.
	@internalComponent
	*/
	NONSHARABLE_CLASS(TCharsetCnvEntry)
		{
	public:
		static TInt Offset();
	public:
		TUid									iImplUid;
		CCharacterSetConverterPluginInterface*	iCharsetCnv;
		TSglQueLink								iLink;
		};
	TInt						iMaxCacheSize;
	TInt						iCacheSize;
	TSglQue<TCharsetCnvEntry>	iCache;

private:
	CCharsetCnvCache();
	TCharsetCnvEntry* Find(TUid aImplUid);
	TCharsetCnvEntry* CreateNewL(TUid aImplUid);
	void RemoveLast();
    
	};

/**
@return The offset of iLink data member from the beginning of CCharsetCnvCache::TCharsetCnvEntry
object. The method is used internally - the character set converter cache implementation 
uses TSglQue class.
Note: Static method.
*/
TInt CCharsetCnvCache::TCharsetCnvEntry::Offset()
	{
	return (_FOFF(TCharsetCnvEntry, iLink));
	}

/**
Standard factory method for CCharsetCnvCache objects creation.
Note: Static method.
@return A pointer to the created CCharsetCnvCache instance.
@leave KErrNoMemory
*/
CCharsetCnvCache* CCharsetCnvCache::NewL()
	{
	CCharsetCnvCache* self = new (ELeave) CCharsetCnvCache;
	return self;
	}

/**
*/
CCharsetCnvCache::~CCharsetCnvCache()
	{
	//Destroy all cache entries - destroy the implementation, delete the entry.
	TSglQueIter<TCharsetCnvEntry> it(iCache);
    it.SetToFirst();
	TCharsetCnvEntry* entry;
	while((entry = it++) != NULL)
		{
		delete entry->iCharsetCnv;
		delete entry;
		--iCacheSize;
		}
	//If iCacheSize is not 0, then there is something wrong with adding/removing cache entry functionality
	if ( iCacheSize != 0)
	    {
	    OstTrace0( TRACE_DUMP, CCHARSETCNVCACHE_CCHARSETCNVCACHE, "iCacheSize not zero in CCharsetCnvCache::~CCharsetCnvCache" );
	    }
	__ASSERT_DEBUG(iCacheSize == 0, User::Invariant());
	}

/**
The method searches the cache for a character set converter, which implementation UID matches 
aImplUid argument. If such converter does not exist in the cache - its implementaion will 
be loaded and cached. If the cache has more than iMaxCacheSize entries, the last cache entry 
will be removed. The found/loaded library will be moved to be the first entry in the cache.
The implementation UID of the converter is used as a key, uniquely identifying the cache entries.
@param aImplUid Implementation UID of the required character set converter.
@return A pointer to CCharacterSetConverterPluginInterface instance.
@leave KErrNoMemory
@leave System-wide error codes as well - loading converter implementation related problems.
@panic User::Invariant() The returned cache entry is NULL.
*/
CCharacterSetConverterPluginInterface* CCharsetCnvCache::GetConverterL(TUid aImplUid)
	{
	TCharsetCnvEntry* entry = Find(aImplUid);
	if(entry)
		{//The requested entry exists in the cache.
		 //1) Remove the entry (iCache object maintains a list of pointers).
		 //2) Add it as a first entry in the cache. It guarantees that if a cache overflow 
		 //happens, the last used/loaded cache entry won't be removed and destroyed.
		iCache.Remove(*entry);
		iCache.AddFirst(*entry);
		}
	else
		{//The requested entry is not in the cache - load it.
		 //If the cache gets too big - remove and destroy the last cache entry.
		TCharsetCnvEntry* newEntry = CreateNewL(aImplUid);
		iCache.AddFirst(*newEntry);
		entry = iCache.First();
		if(++iCacheSize > iMaxCacheSize)
			{
			RemoveLast();
			--iCacheSize;
			}
		}
	if ( !entry )
	    {
	    OstTrace0( TRACE_FATAL, CCHARSETCNVCACHE_GETCONVERTERL, "entry NULL in CCharsetCnvCache::GetConverterL" );   
	    }
	__ASSERT_ALWAYS(entry, User::Invariant());
	return entry->iCharsetCnv;
	}

/**
The method searches the cache for a character set converter, which implementation UID matches 
the supplied UID argument. If such converter does not exist in the cache - the method panics.
The implementation UID of the converter is used as a key, uniquely identifying the cache entries.
@param aImplUid Implementation UID of the required character set converter.
@return A pointer to CCharacterSetConverterPluginInterface instance.
@panic EPanicCharacterSetConverterNotLoaded There is no converter implementation in the cache, which 
											implementation UID is equal to aImplUid.
*/
CCharacterSetConverterPluginInterface* CCharsetCnvCache::Converter(TUid aImplUid)
	{
	TCharsetCnvEntry* entry = Find(aImplUid);
	if ( entry == NULL || entry->iCharsetCnv == NULL )
	    {
	    OstTrace0( TRACE_FATAL, CCHARSETCNVCACHE_CONVERTER, "entry or entry->iCharsetCnv NULL in CCharsetCnvCache::Converter" );
	    }
	__ASSERT_ALWAYS(entry != NULL && entry->iCharsetCnv != NULL, Panic(EPanicCharacterSetConverterNotLoaded));
	iCache.Remove(*entry);
	iCache.AddFirst(*entry);
	return entry->iCharsetCnv;	
	}

/**
The method sets the max size of the character set converter cache.
By default (if SetMaxSize() is never called) the max cache size is limited to 
KDefaultMaxCacheSize entries.
Note: Setting very small cache size will impact the overall performance of CHARCONV functions.
@param aSize The new max cache size. It must be bigger or equal to KMinCacheSize.
@panic User::Invariant() if the new max cache size is less than KMinCacheSize.
@see KDefaultMaxCacheSize
@see KMinCacheSize
*/
void CCharsetCnvCache::SetMaxSize(TInt aSize)
	{
	if ( aSize < KMinCacheSize )
	    {
	    OstTrace0( TRACE_FATAL, CCHARSETCNVCACHE_SETMAXSIZE, "aSize < KMinCacheSize in CCharsetCnvCache::SetMaxSize" );
	    }
	__ASSERT_ALWAYS(aSize >= KMinCacheSize, User::Invariant());
	//Remove and destroy the last cache entries, if iCacheSize > aSize.
	for(;iCacheSize>aSize;--iCacheSize)
		{
		RemoveLast();
		}
	iMaxCacheSize = aSize;
	}

/**
*/
CCharsetCnvCache::CCharsetCnvCache() :
	iMaxCacheSize(KDefaultMaxCacheSize),
	iCacheSize(0),
	iCache(TCharsetCnvEntry::Offset())
	{
	}

/**
The method searches the cache for a converter which implementation UID matches aImplUid argument.
If such entry exists, a pointer to it will be returned.
@param aImplUid Implementation UID of the searched character set converter.
@return A pointer to the found cache entry or NULL if there is no such entry.
*/
CCharsetCnvCache::TCharsetCnvEntry* CCharsetCnvCache::Find(TUid aImplUid)
	{
	TSglQueIter<TCharsetCnvEntry> it(iCache);
    it.SetToFirst();
	TCharsetCnvEntry* entry = NULL;
	while((entry = it++) != NULL && entry->iImplUid != aImplUid)
		{
		}
	return entry;
	}

/**
The method creates a new CCharsetCnvCache::TCharsetCnvEntry instance in the heap. It will load
the required character set converter plugin.
@param aImplUid Implementation UID of the character set converter to be loaded.
@return A pointer to the created CCharsetCnvCache::TCharsetCnvEntry instance.
@leave KErrNoMemory
@leave System-wide error codes as well - loading converter plugin related problems.
@panic User::Invariant(), if the loaded library does not export all expected functions.
*/
CCharsetCnvCache::TCharsetCnvEntry* CCharsetCnvCache::CreateNewL(TUid aImplUid)
	{
	CCharsetCnvCache::TCharsetCnvEntry* newEntry = new (ELeave) CCharsetCnvCache::TCharsetCnvEntry;
	CleanupStack::PushL(newEntry);

	newEntry->iCharsetCnv = CCharacterSetConverterPluginInterface::NewL(aImplUid);
	newEntry->iImplUid = aImplUid;

	CleanupStack::Pop(newEntry);
	return newEntry;
	}

/**
The method removes and destroys the last cache entry.
The related converter implementation will be destroyed, the entry - deleted.
@panic User::Invariant() If the last cache entry is NULL.
*/
void CCharsetCnvCache::RemoveLast()
	{
	TCharsetCnvEntry* lastEntry = iCache.Last();
	if ( !lastEntry )
	    {
	    OstTrace0( TRACE_FATAL, CCHARSETCNVCACHE_REMOVELAST, "lastEntry NULL in CCharsetCnvCache::RemoveLast" );
	    }
	__ASSERT_ALWAYS(lastEntry, User::Invariant());
	iCache.Remove(*lastEntry);
	delete lastEntry->iCharsetCnv;
	delete lastEntry;
	}
	
// CCnvCharacterSetConverter

/** Allocates and constructs a CCnvCharacterSetConverter object. If there is 
insufficient memory to create the object, the function leaves.

Since the memory is allocated on the heap, objects of this type should be 
destroyed using the delete operator when the required conversions are complete.

@return The newly created object. */
EXPORT_C CCnvCharacterSetConverter* CCnvCharacterSetConverter::NewL()
	{
	CCnvCharacterSetConverter* characterSetConverter=NewLC();
	CleanupStack::Pop(); // characterSetConverter
	return characterSetConverter;
	}

/** Allocates and constructs a CCnvCharacterSetConverter object, and leaves the 
object on the cleanup stack. If there is insufficient memory to create the 
object, the function leaves.

Since the memory is allocated on the heap, objects of this type should be 
destroyed using either the CleanupStack::Pop() function and then the delete 
operator, or the CleanupStack::PopAndDestroy() function.

@return The newly created object. */
EXPORT_C CCnvCharacterSetConverter* CCnvCharacterSetConverter::NewLC()
	{
	CCnvCharacterSetConverter* characterSetConverter=new(ELeave) CCnvCharacterSetConverter;
	characterSetConverter->iTlsDataConstructed=EFalse;
	CleanupStack::PushL(characterSetConverter);
	characterSetConverter->ConstructL();
	return characterSetConverter;
	}


/** The destructor frees all resources owned by the object, prior to its 
destruction. */
EXPORT_C CCnvCharacterSetConverter::~CCnvCharacterSetConverter()
	{
	delete iStandardNamesAndMibEnums;
	if (iTlsDataConstructed)
		{
		TTlsData::CharacterSetConverterIsBeingDestroyed();		
		}
	delete iCharsetCnvCache;
	REComSession::FinalClose();
	}


/** Creates an array identifying all the character sets for which conversion is 
available. These can be character sets for which conversion is built into 
Symbian OS, or they may be character sets for which conversion is implemented 
by a plug-in DLL.

The array returned can be used by one of the PrepareToConvertToOrFromL() 
overloads to provide a list of all the character sets available for conversion. 
The caller of this function is responsible for deleting the array, and should 
not modify it.

Not all encoders returned will be suitable for conversion from Unicode. 
Such encoders have no name and no MIB enum and so will generally not be understood by 
a receiving process. The function ConvertCharacterSetIdentifierToMibEnumL can be used 
to determine whether this is the case or not.

@param aFileServerSession A file server session. This is no longer used, but is kept for Binary Compatibility.
@return An array identifying all supported character sets. */
EXPORT_C CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* 
CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(RFs& aFileServerSession)
	{
	
	CArrayFix<SCharacterSet>* arrayOfCharacterSetsAvailable=CreateArrayOfCharacterSetsAvailableLC(aFileServerSession);
	CleanupStack::Pop(); // arrayOfCharacterSetsAvailable
	return arrayOfCharacterSetsAvailable;
	
	}


/** Creates an array identifying all the character sets for which conversion is 
available and pushes a pointer to it onto the cleanup stack. These can be 
character sets for which conversion is built into Symbian OS, or they may 
be character sets for which conversion is implemented by a plug-in DLL.

The array returned can be used by one of the PrepareToConvertToOrFromL() 
overloads to provide a list of all the character sets available for conversion. 
The caller of this function is responsible for deleting the array, and should 
not modify it.

Not all encoders returned will be suitable for conversion from Unicode. 
Such encoders have no name and no MIB enum and so will generally not be understood by 
a receiving process. The function ConvertCharacterSetIdentifierToMibEnumL can be used 
to determine whether this is the case or not.

This is a static function which uses ECOM functionality. It cleans up ECOM by calling FinalClose()

@param aFileServerSession A file server session. This is no longer required, kept for Binary Compatibilty.
@return An array of references to all supported character sets. */
EXPORT_C CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* 
CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(RFs& aFileServerSession)
	{
	
	CleanupStack::PushL(TCleanupItem(CloseECOMSession, NULL));

	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* result = 
		DoCreateArrayOfCharacterSetsAvailableLC(aFileServerSession, 0);

	CleanupStack::Pop();//CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* result
	CleanupStack::PopAndDestroy(); //CloseECOMSession
	CleanupStack::PushL(result);

	return result;
	}


/** Gets the UID of a character set identified by its Internet-standard name 
(the matching is case-insensitive).

If the character set specified is not one for which Symbian OS provides 
built-in conversion, the function searches the file system for plug-ins 
which implement the conversion and which provide the name-to-UID mapping 
information.

@param aStandardNameOfCharacterSet Internet-standard name of a character set.
@param aFileServerSession Connection to a file server session.
@return The UID for the character set if the character set with a Internet-standard name
		has a known name and MIB enum.Otherwise zero is returned. */ 
EXPORT_C TUint 
CCnvCharacterSetConverter::ConvertStandardNameOfCharacterSetToIdentifierL(
								const TDesC8& aStandardNameOfCharacterSet, 
								RFs& aFileServerSession)
	{
	if (iStandardNamesAndMibEnums==NULL)
		{
		ScanForStandardNamesAndMibEnumsROMOnlyL(aFileServerSession);
		}
	TUint result=iStandardNamesAndMibEnums->Identifier(aStandardNameOfCharacterSet);
	if(result)
		{
		return result;
		}
	if(!iIsSystemStandardNamesAndMibEnumsScanned)
		{
		//If SNM files in system haven't been scan yet, scan them
		ScanForStandardNamesAndMibEnumsL(aFileServerSession);
		iIsSystemStandardNamesAndMibEnumsScanned = ETrue;
		return iStandardNamesAndMibEnums->Identifier(aStandardNameOfCharacterSet);
		}
	else
		{
		//iStandardNamesAndMibEnums has included those snm files in system
		return 0;
		}
	}


/** Returns the Internet-standard name of a character set identified in Symbian 
OS by a UID.

If the character set specified is not one for which Symbian OS provides 
built-in conversion, the file system is searched for plug-ins which implement 
the conversion, hence the need for a file server session.

@param aCharacterSetIdentifier The UID of the character set.
@param aFileServerSession A file server session.
@return The Internet-standard name for the character set if the character set with an identifier
		has a known name and MIB enum.Otherwise NULL is returned. */ 
EXPORT_C HBufC8* 
CCnvCharacterSetConverter::ConvertCharacterSetIdentifierToStandardNameL(
								TUint aCharacterSetIdentifier, 
								RFs& aFileServerSession)
	{
	if (iStandardNamesAndMibEnums==NULL)
		{
		ScanForStandardNamesAndMibEnumsROMOnlyL(aFileServerSession);
		}
	HBufC8* result = iStandardNamesAndMibEnums->StandardNameL(aCharacterSetIdentifier);
	if (result!=NULL)
		{
		return result;
		}
	if(!iIsSystemStandardNamesAndMibEnumsScanned)
		{
		//If SNM files in system haven't been scan yet, scan them
		ScanForStandardNamesAndMibEnumsL(aFileServerSession);
		iIsSystemStandardNamesAndMibEnumsScanned = ETrue;
		return iStandardNamesAndMibEnums->StandardNameL(aCharacterSetIdentifier);
		}
	else
		{
		//iStandardNamesAndMibEnums has included those snm files in system
		return NULL;
		}
	}


/** Converts a MIB enum value to the UID value of the character set.

If the character set identified is not one for which Symbian OS provides 
built-in conversion, the function searches the file system for plug-ins 
which implement the conversion and which provide the MIB enum-to-UID mapping 
information.

@param aMibEnumOfCharacterSet The MIB enum value of the character set.
@param aFileServerSession Connection to a file server session.
@return TThe UID of the character set if the character set with a Mib enum 
		has a known name and MIB enum.Otherwise zero is returned. */ 
EXPORT_C TUint 
CCnvCharacterSetConverter::ConvertMibEnumOfCharacterSetToIdentifierL(
								TInt aMibEnumOfCharacterSet, 
								RFs& aFileServerSession)
	{
	if (iStandardNamesAndMibEnums==NULL)
		{
		ScanForStandardNamesAndMibEnumsROMOnlyL(aFileServerSession);
		}
	TUint result = iStandardNamesAndMibEnums->Identifier(aMibEnumOfCharacterSet);
	if (result)
		{
		return result;
		}
	if(!iIsSystemStandardNamesAndMibEnumsScanned)
		{
		//If SNM files in system haven't been scan yet, scan them
		ScanForStandardNamesAndMibEnumsL(aFileServerSession);
		iIsSystemStandardNamesAndMibEnumsScanned = ETrue;
		return iStandardNamesAndMibEnums->Identifier(aMibEnumOfCharacterSet);
		}
	else
		{
		//iStandardNamesAndMibEnums has included those snm files in system
		return 0;
		}
	}


/** Converts the UID of a character set to its MIB enum value.

If the character set identified is not one for which Symbian OS provides 
built-in conversion, the function searches the file system for plug-ins 
which implement the conversion and which provide the UID-to-MIB enum mapping 
information.

@param aCharacterSetIdentifier The UID of the character set.
@param aFileServerSession Connection to a file server session.
@return The MIB enum value of the character set if the character set with Identifier 
		has a known name and MIB enum.Otherwise zero is returned. */ 
EXPORT_C TInt 
CCnvCharacterSetConverter::ConvertCharacterSetIdentifierToMibEnumL(
								TUint aCharacterSetIdentifier, 
								RFs& aFileServerSession)
	{
	if (iStandardNamesAndMibEnums==NULL)
		{
		ScanForStandardNamesAndMibEnumsROMOnlyL(aFileServerSession);
		}
	TInt result = iStandardNamesAndMibEnums->MibEnum(aCharacterSetIdentifier);
	if (result)
		{
		return result;
		}
	if(!iIsSystemStandardNamesAndMibEnumsScanned)
		{
		//If SNM files in system haven't been scan yet, scan them
		ScanForStandardNamesAndMibEnumsL(aFileServerSession);
		iIsSystemStandardNamesAndMibEnumsScanned = ETrue;
		return iStandardNamesAndMibEnums->MibEnum(aCharacterSetIdentifier);
		}
	else
		{
		//iStandardNamesAndMibEnums has included those snm files in system
		return 0;
		}
	}

/** Specifies the character set to convert to or from. aCharacterSetIdentifier 
is a UID which identifies a character set. It can be one of the character 
sets for which conversion is built into Symbian OS, or it may be a character 
set for which the conversion is implemented by a plug-in DLL.

The function searches the character set array specified 
(aArrayOfCharacterSetsAvailable). This is an array containing all of the 
character sets for which conversion is available. It is created by calling 
CreateArrayOfCharacterSetsAvailableL() or 
CreateArrayOfCharacterSetsAvailableLC(). You should be sure that conversion 
is available for aCharacterSetIdentifier, because if not, a panic occurs. 
Otherwise, use the other overload of this function.

Either this function or its overload, must be called before using the conversion 
functions ConvertFromUnicode() or ConvertToUnicode().

Unlike the other overload, this function does not search the file system for 
plug-in conversion DLLs, (unless aArrayOfCharacterSetsAvailable is NULL). 
This function should be used if conversions are to be performed often, or 
if the conversion character set is to be selected by the user. Generating 
the array of all the available character sets once and searching though it 
is more efficient than the method used by the other overload, in which the 
file system may be searched every time it is invoked.

Notes:

The file server session argument is used to open the required character set 
conversion data file.

The array passed to this function can also be used to provide a list from 
which a user can select the desired conversion character set.

@param aCharacterSetIdentifier The UID of the non-Unicode character set from 
or to which to convert. Must not be zero, or a panic occurs.
@param aArrayOfCharacterSetsAvailable Array of all character sets for which 
conversion is available, created by either 
CreateArrayOfCharacterSetsAvailableLC() or 
CreateArrayOfCharacterSetsAvailableL().
@param aFileServerSession A file server session. No longer used, kept for Binary Compatibility */
EXPORT_C void 
CCnvCharacterSetConverter::PrepareToConvertToOrFromL(
				TUint aCharacterSetIdentifier, 
				const CArrayFix<SCharacterSet>& aArrayOfCharacterSetsAvailable, 
				RFs& aFileServerSession)
	{
	const TAvailability availability=DoPrepareToConvertToOrFromL(aCharacterSetIdentifier, 
																 &aArrayOfCharacterSetsAvailable, 
																 aFileServerSession);
	if (availability!=EAvailable)
		{
		OstTrace0( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_PREPARETOCONVERTTOORFROML, "Conversion Not found in CCnvCharacterSetConverter::PrepareToConvertToOrFromL" );
		User::Leave(KErrNotFound);
		}
	}

/** Specifies the character set to convert to or from. aCharacterSetIdentifier 
is a UID which identifies a character set. It can be one of the character 
sets for which conversion is built into Symbian OS, or it may be a character 
set for which conversion is implemented by a plug-in DLL. In the latter case, 
the function searches through the file system for the DLL which implements 
the character conversion.

Either this function or its overload must be called before using the conversion 
functions ConvertFromUnicode() or ConvertToUnicode().

This overload of the function is simpler to use than the other and does not 
panic if the character set with the specified UID is not available at run 
timeÂ, it simply returns ENotAvailable. It should be used when the conversion 
character set is specified within the text object being converted, e.g. an 
email message, or an HTML document. If the character set is not specified, 
the user must be presented with a list of all available sets, so it makes 
sense to use the other overload.

The function may need to search the file system each time it is called. If 
conversion takes place repeatedly over a short period, it may be more efficient 
to use the other overload.

Notes:

Although the other overload of this function is more efficient, if the 
character set is one for which conversion is built into Symbian OS, the 
difference in speed is negligible.

@param aCharacterSetIdentifier The UID of the non-Unicode character set from 
or to which to convert. Must not be zero, or a panic occurs.
@param aFileServerSession A file server session. No longer used, kept for Binary Compatibility
@return The availability of the specified character set. If EAvailable is 
returned, then the conversion functions ConvertToUnicode() and 
ConvertFromUnicode() will use aCharacterSetIdentifier as the foreign character 
set. If ENotAvailable is returned, then the foreign character set will either 
be undefined (and trying to use the conversion functions will cause a panic), 
or if it has previously been set, it will remain unchanged. */
EXPORT_C CCnvCharacterSetConverter::TAvailability 
CCnvCharacterSetConverter::PrepareToConvertToOrFromL(TUint aCharacterSetIdentifier, 
													 RFs& aFileServerSession)
	{
	return DoPrepareToConvertToOrFromL(aCharacterSetIdentifier, NULL, aFileServerSession);
	}


/** Sets the default endian-ness used by the ConvertFromUnicode() and 
ConvertToUnicode() functions to convert between Unicode and non-Unicode 
character sets.

The endian-ness of a multi-byte character set may be defined in the character 
set definition or, as in the case of UCS-2, be operating system dependent. 
If the endian-ness of the current character set is defined by the character 
set itself, then the default endian-ness specified by this function is ignored.

Notes:

The issue of endian-ness does not apply to single byte character sets 
as there is no byte order.

This function should be called (if at all) after calling 
PrepareToConvertToOrFromL() and before calling ConvertFromUnicode() and/or 
ConvertToUnicode().

@param aEndianness The default endian-ness of the current character set. */
EXPORT_C void 
CCnvCharacterSetConverter::SetDefaultEndiannessOfForeignCharacters(
											TEndianness aDefaultEndiannessOfForeignCharacters)
    {
    if ( (aDefaultEndiannessOfForeignCharacters!=ELittleEndian) && (aDefaultEndiannessOfForeignCharacters!=EBigEndian) )
        {
        OstTrace0( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_SETDEFAULTENDIANNESSOFFOREIGNCHARACTERS, "Bad Default Endianness Of Foreign Characters in CCnvCharacterSetConverter::SetDefaultEndiannessOfForeignCharacters" );        
        }
	__ASSERT_ALWAYS((aDefaultEndiannessOfForeignCharacters==ELittleEndian) || (aDefaultEndiannessOfForeignCharacters==EBigEndian), Panic(EPanicBadDefaultEndiannessOfForeignCharacters));
	iDefaultEndiannessOfForeignCharacters=aDefaultEndiannessOfForeignCharacters;
	}


/** Sets whether the Unicode 'line separator' and 'paragraph separator' 
characters (0x2028 and 0x2029 respectively) should be converted into a 
carriage return / line feed pair, or into a line feed only when converting 
from Unicode into a foreign character set. This applies to all foreign 
character sets that do not contain a direct equivalent of these Unicode 
character codes.

By default, line and paragraph separators are converted into a CR/LF pair. 
This function should be called (if at all) after calling 
PrepareToConvertToOrFromL() and before calling ConvertFromUnicode() and/or 
ConvertToUnicode().

@param aDowngradeForExoticLineTerminatingCharacters Specify 
EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed if 
line/paragraph separators should be converted into a carriage return and 
line feed combination and 
EDowngradeExoticLineTerminatingCharactersToJustLineFeed if they should be 
converted into line feeds only. Any other value causes the function to panic. */
EXPORT_C void 
CCnvCharacterSetConverter::SetDowngradeForExoticLineTerminatingCharacters(
	TDowngradeForExoticLineTerminatingCharacters aDowngradeForExoticLineTerminatingCharacters)
	{
    if ( (aDowngradeForExoticLineTerminatingCharacters!=EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed) && (aDowngradeForExoticLineTerminatingCharacters!=EDowngradeExoticLineTerminatingCharactersToJustLineFeed) )
    {
    OstTrace0( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_SETDOWNGRADEFOREXOTICLINETERMINATINGCHARACTERS, "Bad Downgrade For Exotic Line Terminating Characters1 in CCnvCharacterSetConverter::SetDowngradeForExoticLineTerminatingCharacters" );
    }
	__ASSERT_ALWAYS((aDowngradeForExoticLineTerminatingCharacters==EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed) || (aDowngradeForExoticLineTerminatingCharacters==EDowngradeExoticLineTerminatingCharactersToJustLineFeed), Panic(EPanicBadDowngradeForExoticLineTerminatingCharacters1));
	iDowngradeForExoticLineTerminatingCharacters=aDowngradeForExoticLineTerminatingCharacters;
	}

/** Sets the character used to replace unconvertible characters in the output 
descriptor, when converting from Unicode into another character set.

The default replacement for unconvertible Unicode characters is specified 
in the conversion data for the character set. The replacement text which is 
set using this function overrides the default value.

Notes:

If the replacement character is multi-byte, and its endian-ness is undefined 
in the character set, then its byte order is taken by default to be 
little-endian.

PrepareToConvertToOrFromL() undoes the effect of any previous calls to this 
function. So, to have any effect, this function should be called between the 
PrepareToConvertToOrFromL() call and the subsequent ConvertFromUnicode() call 
or calls.

The value only applies when converting from Unicode to another character set. 
In Unicode, the code for  'unknown character'is always 0xFFFD.

@param aReplacementForUnconvertibleUnicodeCharacters The single character 
which is to be used to replace unconvertible characters. */
EXPORT_C void 
CCnvCharacterSetConverter::SetReplacementForUnconvertibleUnicodeCharactersL(
								const TDesC8& aReplacementForUnconvertibleUnicodeCharacters)
	{
	iReplacementForUnconvertibleUnicodeCharacters=aReplacementForUnconvertibleUnicodeCharacters;
	}

/** Converts text encoded in the Unicode character set (UCS-2) into other
character sets.

The first overload of the function simply performs the conversion. The
second overload converts the text and gets the number of characters
that could not be converted. The third overload converts the text,
gets the number of characters that could not be converted, and also
gets the index of the first character that could not be converted. A
fourth overload was introduced in v6.0 see below.All overloads cause a
panic if no target character set has been selected to convert to (i.e.
either overload of PrepareToConvertToOrFromL() must have
been successfully called beforehand). You may also need to call
SetDefaultEndiannessOfForeignCharacters() to define the
endian-ness of the output descriptor.Notes:A sixteen-bit descriptor is
used to hold the source Unicode encoded text, and an eight-bit
descriptor is used to hold the converted non-Unicode text. Eight-bit
descriptors are used because non-Unicode character sets may use a
single byte per character (e.g. Code Page 1252) or more than one byte
per character (e.g. GB 2312-80) or even a variable number of bytes per
character (e.g. Shift-JIS).The function will fail to convert all the
input descriptor if the output descriptor is not long enough to hold
all the text.Unicode characters cannot be converted if there is no
equivalent for them in the target character set. This does not stop
the conversion, the missing character is simply replaced by the
character in the target character set which represents unknown
characters. This default unknown character can be changed using
SetReplacementForUnconvertibleUnicodeCharactersL().

@param aForeign  On return, contains the converted text in a non-Unicode 
character set.
@param aUnicode The source Unicode text to be converted.
@param aNumberOfUnconvertibleCharacters On return contains the number of 
characters which could not be converted.
@param aIndexOfFirstUnconvertibleCharacter On return, contains the index 
of the first character in the input text that could not be converted. The
value is negative if all characters were converted.
@return The number of unconverted characters left at the end of the input 
descriptor (e.g. because the output descriptor is not long enough to hold 
all the text), or one of the error values defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::ConvertFromUnicode(TDes8& aForeign,
															const TDesC16& aUnicode) const
 	{
	TArrayOfAscendingIndices notUsed;
	return ConvertFromUnicode(aForeign, aUnicode, notUsed);
	}

EXPORT_C TInt CCnvCharacterSetConverter::ConvertFromUnicode(TDes8& aForeign, const TDesC16& aUnicode, TInt& aNumberOfUnconvertibleCharacters) const
	{
	TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;
	const TInt returnValue=ConvertFromUnicode(aForeign, aUnicode, indicesOfUnconvertibleCharacters);
	aNumberOfUnconvertibleCharacters=indicesOfUnconvertibleCharacters.NumberOfIndices();
	return returnValue;
	}

/** Converts text encoded in the Unicode character set (UCS-2) into other 
character sets.

The first overload of the function simply performs the conversion. The second 
overload converts the text and gets the number of characters that could not 
be converted. The third overload converts the text, gets the number of 
characters that could not be converted, and also gets the index of the first 
character that could not be converted. A fourth overload was introduced in v6,
see below.

All overloads cause a panic if no target character set has been selected to 
convert to (i.e. either overload of PrepareToConvertToOrFromL() must have 
been successfully called beforehand). You may also need to call 
SetDefaultEndiannessOfForeignCharacters() to define the endian-ness of the 
output descriptor.

Notes:

A sixteen-bit descriptor is used to hold the source Unicode encoded text, 
and an eight-bit descriptor is used to hold the converted non-Unicode text. 
Eight-bit descriptors are used because non-Unicode character sets may use 
a single byte per character (e.g. Code Page 1252) or more than one byte per 
character (e.g. GB 2312-80) or even a variable number of bytes per character 
(e.g. Shift-JIS).

The function will fail to convert all the input descriptor if the output 
descriptor is not long enough to hold all the text.

Unicode characters cannot be converted if there is no equivalent for them 
in the target character set. This does not stop the conversion, the missing 
character is simply replaced by the character in the target character set 
which represents unknown characters. This default unknown character can be 
changed using SetReplacementForUnconvertibleUnicodeCharactersL().

@param aForeign On return, contains the converted text in a non-Unicode 
character set.
@param aUnicode The source Unicode text to be converted.
@param aNumberOfUnconvertibleCharacters On return contains the number of 
characters which could not be converted.
@param aIndexOfFirstUnconvertibleCharacter On return, contains the index of 
the first character in the input text that could not be converted. The value 
is negative if all characters were converted.
@return The number of unconverted characters left at the end of the input 
descriptor (e.g. because the output descriptor is not long enough to hold all 
the text), or one of the error values defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::ConvertFromUnicode(
												TDes8& aForeign,
												const TDesC16& aUnicode,
												TInt& aNumberOfUnconvertibleCharacters, 
												TInt& aIndexOfFirstUnconvertibleCharacter) const
	{
	TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;
	const TInt returnValue=ConvertFromUnicode(aForeign, aUnicode, indicesOfUnconvertibleCharacters);
	aNumberOfUnconvertibleCharacters=indicesOfUnconvertibleCharacters.NumberOfIndices();
	aIndexOfFirstUnconvertibleCharacter=(aNumberOfUnconvertibleCharacters==0)? -1: indicesOfUnconvertibleCharacters[0];
	return returnValue;
	}

/** Converts Unicode text into another character set.

Differs from the other overloads of this function by returning the indices 
of all of the characters in the source Unicode text which could not be converted.

@param aForeign On return, contains the converted text in a non-Unicode 
character set.
@param aUnicode The source Unicode text to be converted.
@param aIndicesOfUnconvertibleCharacters On return, holds the indices of each 
Unicode character in the source text which could not be converted.
@return The number of unconverted characters left at the end of the input 
descriptor (e.g. because the output descriptor is not long enough to hold all 
the text), or one of the error values defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::ConvertFromUnicode(
						TDes8& aForeign, 
						const TDesC16& aUnicode, 
						TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters) const
	{
	if ( iCharacterSetIdentifierOfLoadedConversionData==0 )
	    {
	    OstTrace0( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_CONVERTFROMUNICODE, "NullCharacterSetIdentifier1 in CCnvCharacterSetConverter::ConvertFromUnicode" );
	    }
	__ASSERT_ALWAYS(iCharacterSetIdentifierOfLoadedConversionData!=0, Panic(EPanicNullCharacterSetIdentifier1));
	if (aUnicode.Length()==0)
		{
		aForeign.SetLength(0);
		return 0;
		}
	if (aForeign.MaxLength()==0)
		{
		return aUnicode.Length();
		}
	TTlsData::SetCurrentCharacterSetConverter(this);
	TInt returnValue;
	if (iStoredFlags&EStoredFlagConversionPlugInLibraryIsLoaded)
		{
		
		TUid implUid;
		implUid.iUid = iCharacterSetIdentifierOfLoadedConversionData;
		returnValue = (iCharsetCnvCache->Converter(implUid))->ConvertFromUnicode(
														iDefaultEndiannessOfForeignCharacters, 
														iReplacementForUnconvertibleUnicodeCharacters, 
														aForeign, 
														aUnicode, 
														aIndicesOfUnconvertibleCharacters);
		}
	else
		{
		switch (iCharacterSetIdentifierOfLoadedConversionData)
			{
		case KCharacterSetIdentifierUtf7:
			returnValue=CnvUtfConverter::ConvertFromUnicodeToUtf7(aForeign, aUnicode, ETrue);
			break;
		case KCharacterSetIdentifierUtf8:
			returnValue=CnvUtfConverter::ConvertFromUnicodeToUtf8(aForeign, aUnicode);
			break;
		case KCharacterSetIdentifierImapUtf7:
			returnValue=CnvUtfConverter::ConvertFromUnicodeToUtf7(aForeign, aUnicode, ETrue, EFalse);
			break;
		case KCharacterSetIdentifierJavaConformantUtf8:
			returnValue=CnvUtfConverter::ConvertFromUnicodeToUtf8(aForeign, aUnicode, ETrue);
			break;
		default:
			__ASSERT_ALWAYS(iConversionData!=NULL, Panic(EPanicNoConversionData1));
			returnValue=DoConvertFromUnicode(*iConversionData, iDefaultEndiannessOfForeignCharacters, iReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters);
			break;
			}
		}
	TTlsData::SetCurrentCharacterSetConverter(NULL);
	return returnValue;
	}



/**  Converts text encoded in a non-Unicode character set into the Unicode
character set (UCS-2).

The first overload of the function simply performs the conversion. The
second overload converts the text and gets the number of bytes in the
input string that could not be converted. The third overload converts
the text, gets the number of bytes that could not be converted, and
also gets the index of the first byte that could not be converted.All
overloads cause a panic if no source character set has been selected
to convert from (i.e. either overload of PrepareToConvertToOrFromL() 
must have been successfully called beforehand). You may also need to call
SetDefaultEndiannessOfForeignCharacters() to define the
endian-ness of the input descriptor.Notes: Since Unicode is intended to
be the superset of all character sets, the function should usually
report zero unconverted characters. Unconvertible characters will
exist if the input descriptor contains illegal characters, i.e. values
not in the selected non-Unicode character set.The presence of illegal
characters does not stop the conversion. The missing character is
simply replaced by the Unicode character which represents unknown
characters (0xFFFD).If the source text consists solely of a
character that is not complete, the function returns
EErrorIllFormedInput. The reason for this is to prevent
the possibility of the calling code getting into a infinite loop.

@param aUnicode  On return, contains the converted text in the Unicode
character aForeign The non-Unicode source text to be converted.
@param aState This is used to save state information across multiple
calls to ConvertToUnicode(). You should initialise the value 
to KStateDefault, and then do not change it in a series of 
related calls.
@param aNumberOfUnconvertibleCharacters  On return, contains the number of 
bytes which were not converted.
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if 
the first character in the input descriptor (aForeign) could 
not be converted, then this parameter is set to the first byte of that
character, i.e. zero. A negative value is returned if  all the characters 
were converted.
@return The number of unconverted bytes left at the end of the  input 
descriptor (e.g. because the output descriptor is not long enough to hold 
all the text), or one of the error values defined in TError.*/
EXPORT_C TInt CCnvCharacterSetConverter::ConvertToUnicode(TDes16& aUnicode, 
														  const TDesC8& aForeign, 
														  TInt& aState) const
 	{
	TInt notUsed1;
	TInt notUsed2;
	return ConvertToUnicode(aUnicode, aForeign, aState, notUsed1, notUsed2);
	}

EXPORT_C TInt CCnvCharacterSetConverter::ConvertToUnicode(TDes16& aUnicode, const TDesC8& aForeign, TInt& aState, TInt& aNumberOfUnconvertibleCharacters) const
	{
	TInt notUsed;
	return ConvertToUnicode(aUnicode, aForeign, aState, aNumberOfUnconvertibleCharacters, notUsed);
	}

/** Converts text encoded in a non-Unicode character set into the Unicode 
character set (UCS-2).

The first overload of the function simply performs the conversion. The second 
overload converts the text and gets the number of bytes in the input string 
that could not be converted. The third overload converts the text, gets the 
number of bytes that could not be converted, and also gets the index of the 
first byte that could not be converted.

All overloads cause a panic if no source character set has been selected to 
convert from (i.e. either overload of PrepareToConvertToOrFromL() must have 
been successfully called beforehand). You may also need to call 
SetDefaultEndiannessOfForeignCharacters() to define the endian-ness of the 
input descriptor.

Notes:

Since Unicode is intended to be the superset of all character sets, the function 
should usually report zero unconverted characters. Unconvertible characters 
will exist if the input descriptor contains illegal characters, i.e. values 
not in the selected non-Unicode character set.

The presence of illegal characters does not stop the conversion. The missing 
character is simply replaced by the Unicode character which represents unknown 
characters (0xFFFD).

If the source text consists solely of a character that is not complete, the function 
returns EErrorIllFormedInput. The reason for this is to prevent the possibility 
of the calling code getting into a infinite loop.

@param aUnicode On return, contains the converted text in the Unicode character 
set.
@param aForeign The non-Unicode source text to be converted.
@param aState This is used to save state information across multiple calls 
to ConvertToUnicode(). You should initialise the value to KStateDefault, and 
then do not change it in a series of related calls.
@param aNumberOfUnconvertibleCharacters On return, contains the number of bytes 
which were not converted.
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted.
@return The number of unconverted bytes left at the end of the input descriptor 
(e.g. because the output descriptor is not long enough to hold all the text), 
or one of the error values defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::ConvertToUnicode(
								TDes16& aUnicode, 
								const TDesC8& aForeign, 
								TInt& aState, 
								TInt& aNumberOfUnconvertibleCharacters, 
								TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter) const
	{
	if ( iCharacterSetIdentifierOfLoadedConversionData==0 )
	    {
	    OstTrace0( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_CONVERTTOUNICODE, "Null CharacterSetIdentifier2 in CCnvCharacterSetConverter::ConvertToUnicode" );    
	    }
	__ASSERT_ALWAYS(iCharacterSetIdentifierOfLoadedConversionData!=0, Panic(EPanicNullCharacterSetIdentifier2));
	aNumberOfUnconvertibleCharacters=0;
	aIndexOfFirstByteOfFirstUnconvertibleCharacter=-1;
	if (aForeign.Length()==0)
		{
		aUnicode.SetLength(0);
		return 0;
		}
	if (aUnicode.MaxLength()==0)
		{
		return aForeign.Length();
		}
	TTlsData::SetCurrentCharacterSetConverter(this);
	TInt returnValue;
	if (iStoredFlags&EStoredFlagConversionPlugInLibraryIsLoaded)
		{
		TUid implUid;
		implUid.iUid = iCharacterSetIdentifierOfLoadedConversionData;
		returnValue = (iCharsetCnvCache->Converter(implUid))->ConvertToUnicode(
														iDefaultEndiannessOfForeignCharacters,  
														aUnicode, 
							  							aForeign, 
							  							aState, 
							  							aNumberOfUnconvertibleCharacters, 
							  							aIndexOfFirstByteOfFirstUnconvertibleCharacter);
		}
	else
		{
		switch (iCharacterSetIdentifierOfLoadedConversionData)
			{
		case KCharacterSetIdentifierUtf7:
			returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf7(aUnicode, aForeign, aState);
			break;
		case KCharacterSetIdentifierUtf8:
			returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf8(aUnicode, aForeign, EFalse, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
			break;
		case KCharacterSetIdentifierJavaConformantUtf8:
			returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf8(aUnicode, aForeign, ETrue, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
			break;
		case KCharacterSetIdentifierImapUtf7:
			returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf7(aUnicode, aForeign, ETrue, aState);
			break;
		default:
		    if ( iConversionData==NULL )
		        {
		        OstTrace0( TRACE_FATAL, DUP1_CCNVCHARACTERSETCONVERTER_CONVERTTOUNICODE, "No ConversionData2 in CCnvCharacterSetConverter::ConvertToUnicode" );
		        }
			__ASSERT_ALWAYS(iConversionData!=NULL, Panic(EPanicNoConversionData2));
			returnValue=DoConvertToUnicode(*iConversionData, iDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
			break;
			}
		}
	TTlsData::SetCurrentCharacterSetConverter(NULL);
	return returnValue;
	}

/**
Deprecated

@see			AutoDetectCharSetL
@since			8.1
@pre 			
@post			
*/
EXPORT_C void CCnvCharacterSetConverter::AutoDetectCharacterSetL(
					TInt& aConfidenceLevel, 
					TUint& aCharacterSetIdentifier, 
					const CArrayFix<SCharacterSet>& aArrayOfCharacterSetsAvailable, 
					const TDesC8& aSample)
	{
	CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();
    converter->AutoDetectCharSetL(aConfidenceLevel,
                                        aCharacterSetIdentifier,
                                        aArrayOfCharacterSetsAvailable,
                                        aSample);
    CleanupStack::Pop(converter);
	delete converter;
	converter = NULL;                                       
	}

/** Attempts to determine the character set of the sample text from 
those supported on the phone.

For each of the available character sets, its implementation of 
IsInThisCharacterSetL() is called. The character set which returns the highest 
confidence level (i.e. which generates the fewest 0xFFFD Unicode replacement 
characters) is returned in aCharacterSetIdentifier.

This function merely determines if the sample text is convertible with this 
converter: it does no textual analysis on the result. Therefore, this function 
is not capable of differentiating between very similar encodings (for example 
the different ISO 8859 variants).

Any code making use of this function should provide a way for the user to 
override the selection that this function makes.

Please note that the operation of this function is slow.It takes no account of the usual 
ontext that would be used in guessing a character set (for example, the language that 
is expected to be encoded or the transport used). For situations where such context is known, 
a faster, more accurate solution is advisable.  

To improve a performance of autodetection, a size (default is one) of interface proxy cache 
should be increased (see SetCharacterSetCacheSize()).However a boost of performance 
will not be visible within a first funtion call because during this first call character sets 
are loaded to a cache. Once created it will be preserved until CCnvCharacterSetConverter 
object is destroyed.


This is a static function which uses ECOM functionality. 
It cleans up ECOM by calling FinalClose()

@param aConfidenceLevel Set by the function to a value between 0 and 100. 
0 indicates the function has no idea what character set aSample is encoded 
in. In this case, aCharacterSetIdentifier is undefined. 100 indicates total 
confidence that aCharacterSetIdentifier is the character set of aSample.
@param aCharacterSetIdentifier On return, the UID of the best available 
character set for the sample text aSample. Character set UIDs are defined 
in charconv.h.
@param aArrayOfCharacterSetsAvailable The array of character sets available 
on the phone. If this is not already available, it can be created using 
CreateArrayOfCharacterSetsAvailableL() or 
CreateArrayOfCharacterSetsAvailableLC().
@param aSample The non-Unicode sample text string. */
EXPORT_C void CCnvCharacterSetConverter::AutoDetectCharSetL(
					TInt& aConfidenceLevel, 
					TUint& aCharacterSetIdentifier, 
					const CArrayFix<SCharacterSet>& aArrayOfCharacterSetsAvailable, 
					const TDesC8& aSample)
	{
	
	CleanupStack::PushL(TCleanupItem(CloseECOMSession, NULL));
	
	aConfidenceLevel=0;
	aCharacterSetIdentifier=0;
	// loop through the aArrayofCharacterSetAvailable 
	TInt previousConfidenceLevel = 0;
	RArray<TUint> chid(25);
	CleanupClosePushL(chid);
	
	for (TInt i=aArrayOfCharacterSetsAvailable.Count()-1; i>=0; --i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& charactersSet=aArrayOfCharacterSetsAvailable[i];
		
		if (charactersSet.Identifier() == KCharacterSetIdentifierWin1252)
			continue; // Win1252 is same as CP1252, so just ignore it 
		
		if (charactersSet.FileIsConversionPlugInLibrary())
			{
		
			TBool plugInImplementsAutoDetect=EFalse;
			
			TUid implUid;
 			implUid.iUid = charactersSet.Identifier();
			TBool isInThisCharSet = (iCharsetCnvCache->GetConverterL(implUid))->IsInThisCharacterSetL(
																plugInImplementsAutoDetect, 
																aConfidenceLevel, 
																aSample);
			OstTraceExt3( TRACE_DUMP, CCNVCHARACTERSETCONVERTER_AUTODETECTCHARSETL, "detect identifier 0x%x, return isInThisCharSet=%d, aConfidenceLevel=%d",  implUid.iUid, isInThisCharSet, aConfidenceLevel);
			if ((!plugInImplementsAutoDetect) || !isInThisCharSet)
				{
				continue;
				}
			}
		else if (charactersSet.NameIsFileName())
			{
			continue;
			}
		else
			{
			TUint characterIdentifier = charactersSet.Identifier();
			::IsBuiltInCharacterSet(characterIdentifier,aConfidenceLevel,aSample);
			}
		if (aConfidenceLevel > previousConfidenceLevel)
			{
			previousConfidenceLevel = aConfidenceLevel;
			chid.Reset();
			User::LeaveIfError(chid.Append(charactersSet.Identifier()));
			}
		else if ((aConfidenceLevel == previousConfidenceLevel) && (previousConfidenceLevel != 0))
			{
			User::LeaveIfError(chid.Append(charactersSet.Identifier()));
			}
		} // for 
	TInt result=0;
	TInt min = KMaxTInt;;
	const TInt numberOfCandidateCharacterSets=chid.Count();
	if (numberOfCandidateCharacterSets ==0)
 		{
        OstTrace0( TRACE_DUMP, DUP2_CCNVCHARACTERSETCONVERTER_AUTODETECTCHARSETL, "We donot find any candidate in first run, so add all plugin as candidate." );
 		// all the charcterset returned 0, so take all and find then one with least unconvertible
 		// characters
 		for (TInt i=aArrayOfCharacterSetsAvailable.Count()-1; i>=0; --i)
 			{
 			const CCnvCharacterSetConverter::SCharacterSet& charactersSettoAppend=aArrayOfCharacterSetsAvailable[i];
 			User::LeaveIfError(chid.Append(charactersSettoAppend.Identifier()));
 			}
 		}
	if (numberOfCandidateCharacterSets>1) 
		{
		// convert and check for the number of unconvertible characters
		CCnvCharacterSetConverter* const charconverter=NewLC();
		TBuf<256>* const convertedToUnicode=new(ELeave) TBuf<256>;
		CleanupStack::PushL(convertedToUnicode);
		RFs dummyFileServerSession; // PrepareToConvertToOrFromL will not actually use this parameter
		for (TInt i=0; i<numberOfCandidateCharacterSets; ++i)
			{
			charconverter->PrepareToConvertToOrFromL(chid[i], aArrayOfCharacterSetsAvailable, dummyFileServerSession); 
			TInt state=KStateDefault;
			TInt unconvertibleChars;
			charconverter->ConvertToUnicode(*convertedToUnicode,aSample,state,unconvertibleChars);
			OstTraceExt2( TRACE_DUMP, DUP1_CCNVCHARACTERSETCONVERTER_AUTODETECTCHARSETL, "Plugin 0x%x has %d unconvertibleChars", chid[i], unconvertibleChars);
			
			if (unconvertibleChars < min)
				{
				min = unconvertibleChars;
				result = i;
				}
			}
		CleanupStack::PopAndDestroy(2, charconverter);
		}

	aConfidenceLevel=previousConfidenceLevel;

	//aConfidenceLevel=another;
	aCharacterSetIdentifier = chid[result];
	OstTrace1( TRACE_DUMP, DUP3_CCNVCHARACTERSETCONVERTER_AUTODETECTCHARSETL, "Use 0x%x as the auto detected plugin", aCharacterSetIdentifier);
	
	if (aConfidenceLevel <= ELowestThreshold) 
		{
		// go through all the charset available again and start converting the sample test
		// the one with the least 0xfffd is the winner !! 
		}
	CleanupStack::PopAndDestroy(&chid);
	CleanupStack::PopAndDestroy(); //CloseECOMSession
	}
	
/**
Deprecated

@see			ConvertibleToCharSetL
@since			8.1
@pre 			
@post			
*/
EXPORT_C void CCnvCharacterSetConverter::ConvertibleToCharacterSetL(
						TInt& aConfidenceLevel, 
						const TUint aCharacterSetIdentifier,
						const CArrayFix<SCharacterSet>& aArrayOfCharacterSetsAvailable, 
						const TDesC8& aSample)
	{
	CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();
    converter->ConvertibleToCharSetL(aConfidenceLevel,
                                     aCharacterSetIdentifier,
                                     aArrayOfCharacterSetsAvailable,
                                     aSample);
    CleanupStack::Pop(converter);
	delete converter;
	converter = NULL;    
	}

/**  Given a character set UID aCharacterSetIdentifier, 
ConvertibleToCharacterSetL returns the likelihood that aSample 
is encoded in that character set. It goes through the array of character sets 
aArrayOfCharacterSetsAvailable and searches for the character set
matching aCharacterSetIdentifier. The character sets 
IsInThisCharacterSetL function is called to determine the probability
of it being encoded in that character set. 

This is a static function which uses ECOM functionality. It cleans up ECOM by calling FinalClose()

@since     7.0 
@param  aConfidenceLevel  Set by the function to a value between 0 and 100. It
indicates the likelihood that aSample is encoded in aCharacterSetIdentifier.
@param	 aCharacterSetIdentifier  the likelihood of aSample being 
encoded in that character set.
@param aArrayOfCharacterSetsAvailable The array of character sets available on 
the device. If this is not already available, it can be created using
CreateArrayOfCharacterSetsAvailableL() or
CreateArrayOfCharacterSetsAvailableLC().
@param   aSample   The non-Unicode sample text string. */
EXPORT_C void CCnvCharacterSetConverter::ConvertibleToCharSetL(
						TInt& aConfidenceLevel, 
						const TUint aCharacterSetIdentifier,
						const CArrayFix<SCharacterSet>& aArrayOfCharacterSetsAvailable, 
						const TDesC8& aSample)
	{
	CleanupStack::PushL(TCleanupItem(CloseECOMSession, NULL));

	aConfidenceLevel = 0;

	// for each charcater set in the array of character set see if it matches 
	// aCharacterSetIdentifier

	TInt charsetsInArray = aArrayOfCharacterSetsAvailable.Count();
	for (TInt i=0; i<charsetsInArray; ++i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& charactersSet=aArrayOfCharacterSetsAvailable[i];
		if (charactersSet.Identifier()==aCharacterSetIdentifier)
			{
			// found the Charset matching the UID, Use this to find the confidence Level
			if ((charactersSet.FileIsConversionPlugInLibrary()) && (charactersSet.NameIsFileName()))
				{ 
				
				TBool plugInConvertibleTo=EFalse;
				TUid implUid;
				implUid.iUid = charactersSet.Identifier();
				TBool isThisCharSet = (iCharsetCnvCache->GetConverterL(implUid))->IsInThisCharacterSetL(
																		plugInConvertibleTo, 
											  							aConfidenceLevel, 
											  							aSample);
				if ((!plugInConvertibleTo) || !isThisCharSet)
					{
					aConfidenceLevel=0;
					}
				}
			else
				{
                ::IsBuiltInCharacterSet(aCharacterSetIdentifier,aConfidenceLevel,aSample);
				}
			}
		} 
	CleanupStack::PopAndDestroy(); //CloseECOMSession
	}
	
LOCAL_C TUint OutputCharacterCode(TUint aInputCharacterCode, 
								  const SCnvConversionData::SOneDirectionData::SRange& aRange)
	{
	if ( (aInputCharacterCode<aRange.iFirstInputCharacterCodeInRange) || (aInputCharacterCode>aRange.iLastInputCharacterCodeInRange) )
	    {
	    OstTrace0( TRACE_FATAL, _OUTPUTCHARACTERCODE, "Input Character Code Not In Range in ::OutputCharacterCode" );
	    }
	__ASSERT_DEBUG((aInputCharacterCode>=aRange.iFirstInputCharacterCodeInRange) && (aInputCharacterCode<=aRange.iLastInputCharacterCodeInRange), Panic(EPanicInputCharacterCodeNotInRange));
	switch (aRange.iAlgorithm)
		{
	case SCnvConversionData::SOneDirectionData::SRange::EDirect:
		return aInputCharacterCode;
	case SCnvConversionData::SOneDirectionData::SRange::EOffset:
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
	    if (aRange.iData.iOffset==0)
	        {
	        OstTrace0( TRACE_DUMP, DUP1__OUTPUTCHARACTERCODE, "EPanicZeroOffset1" );
	        }
		__ASSERT_DEBUG(aRange.iData.iOffset!=0, Panic(EPanicZeroOffset1));
		return aInputCharacterCode+aRange.iData.iOffset;
#else
		if (STATIC_CAST(TInt, aRange.iData.iWord1)==0)
		    {
		    OstTrace0( TRACE_DUMP, DUP2__OUTPUTCHARACTERCODE, "EPanicZeroOffset2" );
		    }
		__ASSERT_DEBUG(STATIC_CAST(TInt, aRange.iData.iWord1)!=0, Panic(EPanicZeroOffset2));
		return aInputCharacterCode+STATIC_CAST(TInt, aRange.iData.iWord1);
#endif
	case SCnvConversionData::SOneDirectionData::SRange::EIndexedTable16:
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
		return aRange.iData.iIndexedTable16.iEntryArray
#else
		return REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16::SEntry*, aRange.iData.iWord1)
#endif
										[aInputCharacterCode-aRange.iFirstInputCharacterCodeInRange].iOutputCharacterCode;
	case SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616:
		{
		TInt leftIndex=0;
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
		TInt rightIndex=aRange.iData.iKeyedTable1616.iNumberOfEntries;
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry* const entryArray=aRange.iData.iKeyedTable1616.iEntryArray;
#else
		TInt rightIndex=STATIC_CAST(TInt, aRange.iData.iWord1);
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry* const entryArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry*, aRange.iData.iWord2);
#endif
		if(rightIndex<=0)
		    {
		    OstTrace0( TRACE_DUMP, DUP3__OUTPUTCHARACTERCODE, "EPanicEmptyKeyedTable1616" );
		    }
		__ASSERT_DEBUG(rightIndex>0, Panic(EPanicEmptyKeyedTable1616));
		FOREVER
			{
			if (leftIndex>rightIndex)
			    {
			    OstTrace0( TRACE_DUMP, DUP4__OUTPUTCHARACTERCODE, "EPanicBadIndices1" );
			    }
			__ASSERT_DEBUG(leftIndex<=rightIndex, Panic(EPanicBadIndices1));
			if (leftIndex==rightIndex)
				{
				return KNoConversionAvailable;
				}
			const TInt middleIndex=(leftIndex+rightIndex)>>1;
			const TUint key=entryArray[middleIndex].iKey;
			if (aInputCharacterCode<key)
				{
				rightIndex=middleIndex;
				}
			else if (aInputCharacterCode>key)
				{
				leftIndex=middleIndex+1;
				}
			else
				{
				return entryArray[middleIndex].iOutputCharacterCode;
				}
			}
		}
	case SCnvConversionData::SOneDirectionData::SRange::EKeyedTable16OfIndexedTables16:
		{
		TInt leftIndex=0;
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
		TInt rightIndex=aRange.iData.iKeyedTable16OfIndexedTables16.iNumberOfKeyedEntries;
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable16OfIndexedTables16::SKeyedEntry* const keyedEntryArray=aRange.iData.iKeyedTable16OfIndexedTables16.iKeyedEntryArray;
#else
		TInt rightIndex=STATIC_CAST(TInt, aRange.iData.iWord1);
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable16OfIndexedTables16::SKeyedEntry* const keyedEntryArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable16OfIndexedTables16::SKeyedEntry*, aRange.iData.iWord2);
#endif
		if (rightIndex<=0)
		    {
		    OstTrace0( TRACE_DUMP, DUP5__OUTPUTCHARACTERCODE, "EPanicEmptyKeyedTable16OfIndexedTables16" );
		    }
		__ASSERT_DEBUG(rightIndex>0, Panic(EPanicEmptyKeyedTable16OfIndexedTables16));
		FOREVER
			{
			if (leftIndex>rightIndex)
			    {
			    OstTrace0( TRACE_DUMP, DUP6__OUTPUTCHARACTERCODE, "EPanicBadIndices2" );
			    }
			__ASSERT_DEBUG(leftIndex<=rightIndex, Panic(EPanicBadIndices2));
			if (leftIndex==rightIndex)
				{
				return KNoConversionAvailable;
				}
			const TInt middleIndex=(leftIndex+rightIndex)>>1;
			const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable16OfIndexedTables16::SKeyedEntry& keyedEntry=keyedEntryArray[middleIndex];
			if (aInputCharacterCode<keyedEntry.iFirstInputCharacterCodeInIndexedTable)
				{
				rightIndex=middleIndex;
				}
			else if (aInputCharacterCode>keyedEntry.iLastInputCharacterCodeInIndexedTable)
				{
				leftIndex=middleIndex+1;
				}
			else
				{
				return keyedEntry.iIndexedEntryArray[aInputCharacterCode-keyedEntry.iFirstInputCharacterCodeInIndexedTable];
				}
			}
		}
		
//new 32 bit algorithms start
	case SCnvConversionData::SOneDirectionData::SRange::EIndexedTable32:
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
		return aRange.iData.iIndexedTable32.iEntryArray
#else
		return REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable32::SEntry*, aRange.iData.iWord1)
#endif
										[aInputCharacterCode-aRange.iFirstInputCharacterCodeInRange].iOutputCharacterCode;
	case SCnvConversionData::SOneDirectionData::SRange::EKeyedTable3232:
		{
		TInt leftIndex=0;
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
		TInt rightIndex=aRange.iData.iKeyedTable3232.iNumberOfEntries;
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable3232::SEntry* const entryArray=aRange.iData.iKeyedTable3232.iEntryArray;
#else
		TInt rightIndex=STATIC_CAST(TInt, aRange.iData.iWord1);
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable3232::SEntry* const entryArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable3232::SEntry*, aRange.iData.iWord2);
#endif
		if (rightIndex<=0)
		    {
		    OstTrace0( TRACE_DUMP, DUP7__OUTPUTCHARACTERCODE, "EPanicEmptyKeyedTable3232" );
		    }
		__ASSERT_DEBUG(rightIndex>0, Panic(EPanicEmptyKeyedTable3232));
		FOREVER
			{
			if (leftIndex>rightIndex)
			    {
			    OstTrace0( TRACE_DUMP, DUP8__OUTPUTCHARACTERCODE, "EPanicBadIndices1" );
			    }
			__ASSERT_DEBUG(leftIndex<=rightIndex, Panic(EPanicBadIndices1));
			if (leftIndex==rightIndex)
				{
				return KNoConversionAvailable;
				}
			const TInt middleIndex=(leftIndex+rightIndex)>>1;
			const TUint key=entryArray[middleIndex].iKey;
			if (aInputCharacterCode<key)
				{
				rightIndex=middleIndex;
				}
			else if (aInputCharacterCode>key)
				{
				leftIndex=middleIndex+1;
				}
			else
				{
				return entryArray[middleIndex].iOutputCharacterCode;
				}
			}
		}
	case SCnvConversionData::SOneDirectionData::SRange::EKeyedTable32OfIndexedTables32:
		{
		TInt leftIndex=0;
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
		TInt rightIndex=aRange.iData.iKeyedTable32OfIndexedTables32.iNumberOfKeyedEntries;
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable32OfIndexedTables32::SKeyedEntry* const keyedEntryArray=aRange.iData.iKeyedTable32OfIndexedTables32.iKeyedEntryArray;
#else
		TInt rightIndex=STATIC_CAST(TInt, aRange.iData.iWord1);
		const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable32OfIndexedTables32::SKeyedEntry* const keyedEntryArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable32OfIndexedTables32::SKeyedEntry*, aRange.iData.iWord2);
#endif
		if (rightIndex<=0)
		    {
		    OstTrace0( TRACE_DUMP, DUP9__OUTPUTCHARACTERCODE, "EPanicEmptyKeyedTable32OfIndexedTables32" );
		    }
		__ASSERT_DEBUG(rightIndex>0, Panic(EPanicEmptyKeyedTable32OfIndexedTables32));
		FOREVER
			{
			if (leftIndex>rightIndex)
			    {
			    OstTrace0( TRACE_DUMP, DUP10__OUTPUTCHARACTERCODE, "EPanicBadIndices2" );
			    }
			__ASSERT_DEBUG(leftIndex<=rightIndex, Panic(EPanicBadIndices2));
			if (leftIndex==rightIndex)
				{
				return KNoConversionAvailable;
				}
			const TInt middleIndex=(leftIndex+rightIndex)>>1;
			const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable32OfIndexedTables32::SKeyedEntry& keyedEntry=keyedEntryArray[middleIndex];
			if (aInputCharacterCode<keyedEntry.iFirstInputCharacterCodeInIndexedTable)
				{
				rightIndex=middleIndex;
				}
			else if (aInputCharacterCode>keyedEntry.iLastInputCharacterCodeInIndexedTable)
				{
				leftIndex=middleIndex+1;
				}
			else
				{
				return keyedEntry.iIndexedEntryArray[aInputCharacterCode-keyedEntry.iFirstInputCharacterCodeInIndexedTable];
				}
			}
		}
//new 32 bit algorithms end		
		
#if defined(_DEBUG)
	default:
	    OstTrace0( TRACE_DUMP, DUP11__OUTPUTCHARACTERCODE, "EPanicBadAlgorithm1" );
		Panic(EPanicBadAlgorithm1);
#endif
		}
	return 0; // dummy return to prevent compiler error
	}

LOCAL_C TBool 
ConvertsToForeignCharacterSet(
			TInt& aSizeOfOutputForeignCharacterCodeInBytes, 
			TUint aInputUnicodeCharacterCode, 
			const SCnvConversionData::SOneDirectionData::SRange* aFirstUnicodeToForeignRange, 
			const SCnvConversionData::SOneDirectionData::SRange* aLastUnicodeToForeignRange)
	{
	if (aFirstUnicodeToForeignRange==NULL)
	    {
	    OstTrace0( TRACE_DUMP, _CONVERTSTOFOREIGNCHARACTERSET, "EPanicNullPointer1" );
	    }
	__ASSERT_DEBUG(aFirstUnicodeToForeignRange!=NULL, Panic(EPanicNullPointer1));
	if (aLastUnicodeToForeignRange==NULL)
	    {
	    OstTrace0( TRACE_DUMP, DUP1__CONVERTSTOFOREIGNCHARACTERSET, "EPanicNullPointer2" );
	    }
	__ASSERT_DEBUG(aLastUnicodeToForeignRange!=NULL, Panic(EPanicNullPointer2));
	if (aFirstUnicodeToForeignRange>aLastUnicodeToForeignRange)
	    {
	    OstTrace0( TRACE_DUMP, DUP2__CONVERTSTOFOREIGNCHARACTERSET, "EPanicCrossedPointers" );
	    }
	__ASSERT_DEBUG(aFirstUnicodeToForeignRange<=aLastUnicodeToForeignRange, Panic(EPanicCrossedPointers));
	for (const SCnvConversionData::SOneDirectionData::SRange* currentUnicodeToForeignRange=aFirstUnicodeToForeignRange; ; ++currentUnicodeToForeignRange)
		{
		if ((aInputUnicodeCharacterCode>=currentUnicodeToForeignRange->iFirstInputCharacterCodeInRange) &&
			(aInputUnicodeCharacterCode<=currentUnicodeToForeignRange->iLastInputCharacterCodeInRange))
			{
			if (OutputCharacterCode(aInputUnicodeCharacterCode, *currentUnicodeToForeignRange)!=KNoConversionAvailable)
				{
				aSizeOfOutputForeignCharacterCodeInBytes=currentUnicodeToForeignRange->iSizeOfOutputCharacterCodeInBytesIfForeign;
				return ETrue;
				}
			}
		if (currentUnicodeToForeignRange>aLastUnicodeToForeignRange)
		    {
		    OstTrace0( TRACE_DUMP, DUP3__CONVERTSTOFOREIGNCHARACTERSET, "EPanicPointerPastUpperLimit21" );
		    }
		__ASSERT_DEBUG(currentUnicodeToForeignRange<=aLastUnicodeToForeignRange, Panic(EPanicPointerPastUpperLimit21));
		if (currentUnicodeToForeignRange>=aLastUnicodeToForeignRange)
			{
			return EFalse;
			}
		}
	}


/** Converts Unicode text into another character set. The Unicode text specified 
in aUnicode is converted using the conversion data object (aConversionData) 
provided by the plug-in for the foreign character set, and the converted text 
is returned in aForeign.

Note

This is a utility function that should only be called from a plug-in conversion 
library's implementation of ConvertFromUnicode(). Users of the Character 
Conversion API should use one of the overloads of ConvertFromUnicode() instead.

@param aConversionData The conversion data object. Typically, you should specify 
conversionData, as declared in convgeneratedcpp.h. This is the 
SCnvConversionData object which is created in the cnvtool-generated .cpp file 
(although for some complex character sets you may want to pass other 
SCnvConversionData objects into this parameter).
@param aDefaultEndiannessOfForeignCharacters The default endian-ness to use 
when writing the characters in the foreign character set. If an endian-ness 
for foreign characters is specified in aConversionData (i.e. not 
SCnvConversionData::EUnspecified), then that value is used and the value of 
aDefaultEndiannessOfForeignCharacters is ignored.
@param aReplacementForUnconvertibleUnicodeCharacters The single character which 
is to be used to replace unconvertible characters.
@param aForeign On return, contains the converted textÂ in a non-Unicode 
character set.
@param aUnicode The source Unicode text to be converted.
@param aIndicesOfUnconvertibleCharacters On return holds the indices of each 
Unicode character in the source text which could not be converted (because 
the target character set does not have an equivalent character).
@return The number of unconverted characters left at the end of the input 
descriptor (e.g. because aForeign was not long enough to hold all the text), 
or a negative error value, as defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::DoConvertFromUnicode(
					const SCnvConversionData& aConversionData, 
					TEndianness aDefaultEndiannessOfForeignCharacters, 
					const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
					TDes8& aForeign, 
					const TDesC16& aUnicode, 
					TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TUint notUsed;
	return DoConvertFromUnicode(aConversionData, aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, notUsed, 0);
	}

/** Converts Unicode text into another character set. The Unicode text specified 
in aUnicode is converted using the conversion data object (aConversionData) 
provided by the plug-in for the foreign character set, and the converted text 
is returned in aForeign.

This overload differs from the previous one in that it allows the caller to 
specify flags which give more control over the conversion.

Note

This is a utility function that should only be called from a plug-in conversion 
library's implementation of ConvertFromUnicode(). Users of the Character 
Conversion API should use one of the overloads of ConvertFromUnicode() instead.

@param aConversionData The conversion data object. Typically, you should specify 
conversionData, as declared in convgeneratedcpp.h. This is the 
SCnvConversionData object which is created in the cnvtool-generated .cpp file 
(although for some complex character sets you may want to pass other 
SCnvConversionData objects into this parameter).
@param aDefaultEndiannessOfForeignCharacters The default endian-ness to use 
when writing the characters in the foreign character set. If an endian-ness 
for foreign characters is specified in aConversionData (i.e. not 
SCnvConversionData::EUnspecified), then that value is used and the value of 
aDefaultEndiannessOfForeignCharacters is ignored.
@param aReplacementForUnconvertibleUnicodeCharacters The single character which 
is to be used to replace unconvertible characters. If aInputConversionFlags 
is set to EInputConversionFlagStopAtFirstUnconvertibleCharacter, this 
replacement character is used to replace the first unconvertible character, 
then the conversion will stop.
@param aForeign On return, contains the converted textÂ in a non-Unicode 
character set. This may already contain some text. If it does, and if 
aInputConversionFlags specifies EInputConversionFlagAppend, then the converted 
text is appended to this descriptor.
@param aUnicode The source Unicode text to be converted.
@param aIndicesOfUnconvertibleCharacters On return holds the indices of each 
Unicode character in the source descriptor aUnicode which could not be converted. 
The maximum size of this array is KMaximumNumberOfIndices whose value is 25 now.
so only the first 25 unconvertible characters will be recorded.
(because the target character set does not have an equivalent character).
@param aOutputConversionFlags If the input descriptor ended in a truncated 
sequence, e.g. the first half of a Unicode surrogate pair, 
aOutputConversionFlags returns with the EOutputConversionFlagInputIsTruncated 
flag set.
@param aInputConversionFlags Specify EInputConversionFlagAppend to append the 
text in aUnicode to aForeign. Specify 
EInputConversionFlagStopAtFirstUnconvertibleCharacter to stop converting when 
the first unconvertible character is reached. Specify 
EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable to prevent 
the function from returning the error-code EErrorIllFormedInput when the input 
descriptor consists of nothing but a truncated sequence. 
@return The number of unconverted characters left at the end of the input 
descriptor (e.g. because aForeign was not long enough to hold all the text), 
or a negative error value, as defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::DoConvertFromUnicode(
								const SCnvConversionData& aConversionData, 
								TEndianness aDefaultEndiannessOfForeignCharacters, 
								const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
								TDes8& aForeign, 
								const TDesC16& aUnicode, 
								TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, 
								TUint& aOutputConversionFlags, 
								TUint aInputConversionFlags)
	{
	aOutputConversionFlags=0;
	if (aUnicode.Length()==0)
		{
		if (~aInputConversionFlags&EInputConversionFlagAppend)
			{
			aForeign.SetLength(0);
			}
		return 0;
		}
	if (aForeign.MaxLength()==((aInputConversionFlags&EInputConversionFlagAppend)? aForeign.Length(): 0))
		{
		return aUnicode.Length();
		}
	TUint8* pointerToPreviousForeignByte=CONST_CAST(TUint8*, aForeign.Ptr()-1);
	const TUint8* const pointerToLastForeignByte=pointerToPreviousForeignByte+aForeign.MaxLength();
	if (aInputConversionFlags&EInputConversionFlagAppend)
		{
		pointerToPreviousForeignByte+=aForeign.Length();
		}
	const TUint16* pointerToCurrentUnicodeCharacter=aUnicode.Ptr();
	const TUint16* const pointerToLastUnicodeCharacter=pointerToCurrentUnicodeCharacter+(aUnicode.Length()-1);
	if (aConversionData.iUnicodeToForeignData.iNumberOfRanges<=0)
	    {
	    OstTrace0( TRACE_DUMP, CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadNumberOfRanges1" );
	    }
	__ASSERT_DEBUG(aConversionData.iUnicodeToForeignData.iNumberOfRanges>0, Panic(EPanicBadNumberOfRanges1));
	const SCnvConversionData::SOneDirectionData::SRange* const firstRange=aConversionData.iUnicodeToForeignData.iRangeArray;
	const SCnvConversionData::SOneDirectionData::SRange* const lastRange=firstRange+(aConversionData.iUnicodeToForeignData.iNumberOfRanges-1);
	const TEndianness endiannessToWriteForeignCharactersIn=EndiannessOfForeignCharacters(aConversionData, aDefaultEndiannessOfForeignCharacters);
	const TEndianness endiannessOfReplacementForUnconvertibleUnicodeCharacters=EndiannessOfForeignCharacters(aConversionData, ELittleEndian); // this has a hard-coded default
	const CCnvCharacterSetConverter* const currentCharacterSetConverter=TTlsData::CurrentCharacterSetConverter();
	const TBool downgradingPermitted=(currentCharacterSetConverter!=NULL); // downgrading is only permitted if we're not doing VFAT short-name generation
	const TDowngradeForExoticLineTerminatingCharacters downgradeForExoticLineTerminatingCharacters=(currentCharacterSetConverter!=NULL)? currentCharacterSetConverter->iDowngradeForExoticLineTerminatingCharacters: EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed;
	TUint nextInputCharacterCode=KNoConversionAvailable;
	FOREVER
		{
		if (pointerToPreviousForeignByte>pointerToLastForeignByte)
		    {
		    OstTrace0( TRACE_DUMP, DUP1_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit6" );
		    }
		__ASSERT_DEBUG(pointerToPreviousForeignByte<=pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit6));
		if (pointerToCurrentUnicodeCharacter>pointerToLastUnicodeCharacter)
		    {
		    OstTrace0( TRACE_DUMP, DUP2_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit7" );
		    }
		__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit7));
		TBool stop=EFalse;
		TUint inputCharacterCode;
		if (nextInputCharacterCode==KNoConversionAvailable)
			{
			inputCharacterCode=*pointerToCurrentUnicodeCharacter;
			}
		else
			{
			inputCharacterCode=nextInputCharacterCode;
			nextInputCharacterCode=KNoConversionAvailable;
			}
		if ((inputCharacterCode>=0xd800) && (inputCharacterCode<0xdc00))
			{
			if (pointerToCurrentUnicodeCharacter>pointerToLastUnicodeCharacter)
			    {
			    OstTrace0( TRACE_DUMP, DUP3_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit8" );
			    }
			__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit8));
			if (pointerToCurrentUnicodeCharacter>=pointerToLastUnicodeCharacter)
				{
				aOutputConversionFlags|=EOutputConversionFlagInputIsTruncated;
				goto end;
				}
			TUint secondHalfOfSurrogatePair=*(pointerToCurrentUnicodeCharacter+1);
			if ((secondHalfOfSurrogatePair<0xdc00) || (secondHalfOfSurrogatePair>=0xe000))
				{
				return EErrorIllFormedInput;
				}
			inputCharacterCode&=~0xd800;
			inputCharacterCode<<=10;
			secondHalfOfSurrogatePair&=~0xdc00;
			inputCharacterCode|=secondHalfOfSurrogatePair;
			inputCharacterCode+=0x00010000; // this must be added - it cannot be bitwise-"or"-ed
			if (!(inputCharacterCode&0xffff0000) || !(inputCharacterCode<0x00110000))
			    {
			    OstTrace0( TRACE_DUMP, DUP4_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadNon16BitCharacterCode1" );
			    }
			__ASSERT_DEBUG((inputCharacterCode&0xffff0000) && (inputCharacterCode<0x00110000), Panic(EPanicBadNon16BitCharacterCode1));
			}
convertInputCharacterCode:
		const SCnvConversionData::SOneDirectionData::SRange* currentRange=firstRange;
		FOREVER
			{
			if ((inputCharacterCode>=currentRange->iFirstInputCharacterCodeInRange) &&
				(inputCharacterCode<=currentRange->iLastInputCharacterCodeInRange))
				{
				TUint outputCharacterCode=OutputCharacterCode(inputCharacterCode, *currentRange);
				if (outputCharacterCode!=KNoConversionAvailable)
					{
					TInt temp=currentRange->iSizeOfOutputCharacterCodeInBytesIfForeign; // the meaning of temp changes during it's lifetime (hence the bland variable name)
					if ( (temp<=0) || ((temp>STATIC_CAST(TInt, sizeof(TUint)))) || !((temp==sizeof(TUint)) || (outputCharacterCode<STATIC_CAST(TUint, 1<<(temp*8)))) )
					    {
					    OstTrace0( TRACE_DUMP, DUP5_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadSizeOfForeignOutputCharacterCode" );
					    }
					__ASSERT_DEBUG((temp>0) && (temp<=STATIC_CAST(TInt, sizeof(TUint))) && ((temp==sizeof(TUint)) || (outputCharacterCode<STATIC_CAST(TUint, 1<<(temp*8)))), Panic(EPanicBadSizeOfForeignOutputCharacterCode)); // ?? this second half of this assert needs a corresponding "KErrCorrupt"-check when loading the file
					if (pointerToLastForeignByte-pointerToPreviousForeignByte<temp)
						{
						goto end;
						}
					--temp;
					temp*=8;
					switch (endiannessToWriteForeignCharactersIn)
						{
					case ELittleEndian:
						FOREVER
							{
							if (pointerToPreviousForeignByte>=pointerToLastForeignByte)
							    {
							    OstTrace0( TRACE_DUMP, DUP6_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit9" );
							    }
							__ASSERT_DEBUG(pointerToPreviousForeignByte<pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit9));
							++pointerToPreviousForeignByte;
							*pointerToPreviousForeignByte=STATIC_CAST(TUint8, outputCharacterCode);
							if (temp<0)
							    {
							    OstTrace0( TRACE_DUMP, DUP7_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadNumberOfRemainingForeignBytes1" );
							    }
							__ASSERT_DEBUG(temp>=0, Panic(EPanicBadNumberOfRemainingForeignBytes1));
							if (temp<=0)
								{
								break;
								}
							temp-=8;
							outputCharacterCode>>=8;
							}
						break;
					case EBigEndian:
						FOREVER
							{
							if (pointerToPreviousForeignByte>=pointerToLastForeignByte)
							    {
							    OstTrace0( TRACE_DUMP, DUP8_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit10" );
							    }
							__ASSERT_DEBUG(pointerToPreviousForeignByte<pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit10));
							++pointerToPreviousForeignByte;
							*pointerToPreviousForeignByte=STATIC_CAST(TUint8, outputCharacterCode>>temp);
							if (temp<0)
							    {
							    OstTrace0( TRACE_DUMP, DUP9_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadNumberOfRemainingForeignBytes2" );
							    }
							__ASSERT_DEBUG(temp>=0, Panic(EPanicBadNumberOfRemainingForeignBytes2));
							if (temp<=0)
								{
								break;
								}
							temp-=8;
							}
						break;
#if defined(_DEBUG)
					default:
					    OstTrace0( TRACE_DUMP, DUP10_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadEndianness1" );
						Panic(EPanicBadEndianness1);
						break;
#endif
						}
					break;
					}
				}
			if (currentRange>lastRange)
			    {
			    OstTrace0( TRACE_DUMP, DUP11_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit11" );
			    }
			__ASSERT_DEBUG(currentRange<=lastRange, Panic(EPanicPointerPastUpperLimit11));
			if (currentRange>=lastRange)
				{
				if (downgradingPermitted)
					{
					if ((inputCharacterCode==0x2029) || (inputCharacterCode==0x2028))
						{
						switch (downgradeForExoticLineTerminatingCharacters)
							{
						case EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed:
							{
							// check that there's enough room for the subsequent line-feed character, and check that both the carriage-return and the line-feed convert into the foreign character set
							TInt sizeOfForeignCarriageReturnInBytes;
							TInt sizeOfForeignLineFeedInBytes;
							if (ConvertsToForeignCharacterSet(sizeOfForeignCarriageReturnInBytes, 0x000d, firstRange, lastRange) &&
								ConvertsToForeignCharacterSet(sizeOfForeignLineFeedInBytes, 0x000a, firstRange, lastRange) &&
								(pointerToLastForeignByte-pointerToPreviousForeignByte>=sizeOfForeignCarriageReturnInBytes+sizeOfForeignLineFeedInBytes))
								{
								inputCharacterCode=0x000d;
								nextInputCharacterCode=0x000a;
								goto convertInputCharacterCode;
								}
							}
							break;
						case EDowngradeExoticLineTerminatingCharactersToJustLineFeed:
							inputCharacterCode=0x000a;
							goto convertInputCharacterCode;
#if defined(_DEBUG)
						default:
						    OstTrace0( TRACE_DUMP, DUP12_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicBadDowngradeForExoticLineTerminatingCharacters2" );
							Panic(EPanicBadDowngradeForExoticLineTerminatingCharacters2);
							break;
#endif
							}
						}
					}
				const TInt lengthOfReplacementForUnconvertibleUnicodeCharacters=aReplacementForUnconvertibleUnicodeCharacters.Length();
				if (lengthOfReplacementForUnconvertibleUnicodeCharacters>0)
					{
					if (pointerToLastForeignByte-pointerToPreviousForeignByte<lengthOfReplacementForUnconvertibleUnicodeCharacters)
						{
						goto end;
						}
					const TUint8* pointerToReadFrom=aReplacementForUnconvertibleUnicodeCharacters.Ptr();
					const TUint8* lastByteToReadFrom=pointerToReadFrom+(lengthOfReplacementForUnconvertibleUnicodeCharacters-1);
					TInt increment=1;
					if (endiannessOfReplacementForUnconvertibleUnicodeCharacters!=endiannessToWriteForeignCharactersIn)
						{
						const TUint8* temp=pointerToReadFrom;
						pointerToReadFrom=lastByteToReadFrom;
						lastByteToReadFrom=temp;
						increment=-1;
						}
					FOREVER
						{
						if (pointerToPreviousForeignByte>=pointerToLastForeignByte)
						    {
						    OstTrace0( TRACE_DUMP, DUP13_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit12" );
						    }
						__ASSERT_DEBUG(pointerToPreviousForeignByte<pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit12));
						++pointerToPreviousForeignByte;
						*pointerToPreviousForeignByte=*pointerToReadFrom;
						if (pointerToReadFrom==lastByteToReadFrom)
							{
							break;
							}
						pointerToReadFrom+=increment;
						}
					}
				if (aInputConversionFlags&EInputConversionFlagStopAtFirstUnconvertibleCharacter)
					{
					stop=ETrue;
					}
				aIndicesOfUnconvertibleCharacters.AppendIndex(pointerToCurrentUnicodeCharacter-aUnicode.Ptr());
				break;
				}
			++currentRange;
			}
		if (inputCharacterCode>=0x00010000)
			{
			if (pointerToCurrentUnicodeCharacter>=pointerToLastUnicodeCharacter)
			    {
			    OstTrace0( TRACE_DUMP, DUP14_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit13" );
			    }
			__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit13));
			if (nextInputCharacterCode==KNoConversionAvailable)
				{
				++pointerToCurrentUnicodeCharacter;
				}
			}
		if (pointerToCurrentUnicodeCharacter>pointerToLastUnicodeCharacter)
		    {
		    OstTrace0( TRACE_DUMP, DUP15_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastUpperLimit14" );
		    }
		__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit14));
		if (nextInputCharacterCode==KNoConversionAvailable)
			{
			if (pointerToCurrentUnicodeCharacter>=pointerToLastUnicodeCharacter)
				{
				++pointerToCurrentUnicodeCharacter; // this increment is done regardless of the test just above, but it's not done before the test as it may make pointerToCurrentUnicodeCharacter greater than pointerToLastUnicodeCharacter, and if pointerToLastUnicodeCharacter just happens to be pointing at 0xffffffff, pointerToCurrentUnicodeCharacter will be pointing at 0x00000000, thus the test (which would now be "if (pointerToCurrentUnicodeCharacter>pointerToLastUnicodeCharacter)") would fail
				goto end;
				}
			++pointerToCurrentUnicodeCharacter;
			}
		if (stop)
			{
			goto end;
			}
		}
end:
    if (pointerToCurrentUnicodeCharacter<aUnicode.Ptr())
        {
        OstTrace0( TRACE_DUMP, DUP16_CCNVCHARACTERSETCONVERTER_DOCONVERTFROMUNICODE, "EPanicPointerPastLowerLimit1" );
        }
	__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter>=aUnicode.Ptr(), Panic(EPanicPointerPastLowerLimit1));
	if ((pointerToCurrentUnicodeCharacter<=aUnicode.Ptr()) && (aOutputConversionFlags&EOutputConversionFlagInputIsTruncated) && (~aInputConversionFlags&EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable))
		{
		return EErrorIllFormedInput;
		}
	aForeign.SetLength((pointerToPreviousForeignByte+1)-aForeign.Ptr());
	return pointerToLastUnicodeCharacter-(pointerToCurrentUnicodeCharacter-1);
	}

/** Converts non-Unicode text into Unicode. The non-Unicode text specified in 
aForeign is converted using the conversion data object (aConversionData) 
provided by the plug-in for the foreign character set, and the converted text 
is returned in aUnicode.

Notes:

This is a utility function that should only be called from a plug-in conversion 
library's implementation of ConvertToUnicode(). Ordinary users of the Character 
Conversion API should use one of the overloads of ConvertToUnicode() instead.

The last two arguments return information about unconverted characters. Because 
Unicode is intended to cover all possible characters, these rarely report 
anything other than zero characters. However they report the existence of 
unconvertible characters if the input descriptor aForeign contains illegal 
characters, i.e. values not in the foreign character set.

@param aConversionData The conversion data object. Typically, you should specify 
conversionData, as declared in convgeneratedcpp.h. This is the 
SCnvConversionData object which is created in the cnvtool-generated .cpp file 
(although for some complex character sets you may want to pass other 
SCnvConversionData objects into this parameter).
@param aDefaultEndiannessOfForeignCharacters The default endian-ness of the 
foreign characters. If an endian-ness for foreign characters is specified 
in aConversionData, then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored.
@param aUnicode On return, contains the textÂ converted into Unicode.
@param aForeign The non-Unicode source text to be converted.
@param aNumberOfUnconvertibleCharacters On return, contains the number of 
characters in aForeign which were not converted. Characters which cannot be 
converted are output as Unicode replacement characters (0xFFFD).
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted.
@return The number of unconverted bytes left at the end of the input descriptor, 
or a negative error value, as defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::DoConvertToUnicode(
									const SCnvConversionData& aConversionData, 
									TEndianness aDefaultEndiannessOfForeignCharacters, 
									TDes16& aUnicode, 
									const TDesC8& aForeign, 
									TInt& aNumberOfUnconvertibleCharacters, 
									TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TUint notUsed;
	return DoConvertToUnicode(aConversionData, aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, notUsed, 0);
	}

/** Converts non-Unicode text into Unicode. The non-Unicode text specified in 
aForeign is converted using the conversion data object (aConversionData) 
provided by the plug-in for the foreign character set, and the converted text 
is returned in aUnicode.

This overload differs from the previous one in that it allows the caller to 
specify flags which give more control over the conversion.

Notes:

This is a utility function that should only be called from a plug-in conversion 
library's implementation of ConvertToUnicode(). Ordinary users of the Character 
Conversion API should use one of the overloads of ConvertToUnicode() instead.

The aNumberOfUnconvertibleCharacters and 
aIndexOfFirstByteOfFirstUnconvertibleCharacter arguments return information 
about unconverted characters. Because Unicode is intended to cover all 
possible characters, these rarely report anything other than zero characters. 
However they report the existence of unconvertible characters if the input 
descriptor aForeign contains illegal characters, i.e. values not in the 
foreign character set.

@param aConversionData The conversion data object. Typically, you should specify 
conversionData, as declared in convgeneratedcpp.h. This is the 
SCnvConversionData object which is created in the cnvtool-generated .cpp file 
(although for some complex character sets you may want to pass other 
SCnvConversionData objects into this parameter).
@param aDefaultEndiannessOfForeignCharacters The default endian-ness of the 
foreign characters. If an endian-ness for foreign characters is specified 
in aConversionData, then that is used instead and the value of 
aDefaultEndiannessOfForeignCharacters is ignored.
@param aUnicode On return, contains the textÂ converted into Unicode.
@param aForeign The non-Unicode source text to be converted.
@param aNumberOfUnconvertibleCharacters On return, contains the number of 
characters in aForeign which were not converted. Characters which cannot be 
converted are output as Unicode replacement characters (0xFFFD).
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted.
@param aOutputConversionFlags If the input descriptor ended in a truncated 
sequence, e.g. a multi-byte character that is not complete, aOutputConversionFlags 
returns with the EOutputConversionFlagInputIsTruncated flag set.
@param aInputConversionFlags Specify EInputConversionFlagAppend to append the 
converted text to aUnicode, otherwise the contents of aUnicode are overwritten. 
Specify EInputConversionFlagStopAtFirstUnconvertibleCharacter to stop converting 
when the first unconvertible character is reached. Specify 
EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable to prevent the 
function from returning the error-code EErrorIllFormedInput when the input 
descriptor consists of nothing but a truncated sequence.
@return The number of unconverted bytes left at the end of the input descriptor, 
or a negative error value defined in TError. */
EXPORT_C TInt CCnvCharacterSetConverter::DoConvertToUnicode(
								const SCnvConversionData& aConversionData, 
								TEndianness aDefaultEndiannessOfForeignCharacters, 
								TDes16& aUnicode, 
								const TDesC8& aForeign, 
								TInt& aNumberOfUnconvertibleCharacters, 
								TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
								TUint& aOutputConversionFlags, 
								TUint aInputConversionFlags)
	{
	aNumberOfUnconvertibleCharacters=0;
	aIndexOfFirstByteOfFirstUnconvertibleCharacter=-1;
	aOutputConversionFlags=0;
	if (aForeign.Length()==0)
		{
		if (~aInputConversionFlags&EInputConversionFlagAppend)
			{
			aUnicode.SetLength(0);
			}
		return 0;
		}
	if (aUnicode.MaxLength()==((aInputConversionFlags&EInputConversionFlagAppend)? aUnicode.Length(): 0))
		{
		return aForeign.Length();
		}
	TUint16* pointerToPreviousUnicodeCharacter=CONST_CAST(TUint16*, aUnicode.Ptr()-1);
	const TUint16* const pointerToLastUnicodeCharacter=pointerToPreviousUnicodeCharacter+aUnicode.MaxLength();
	if (aInputConversionFlags&EInputConversionFlagAppend)
		{
		pointerToPreviousUnicodeCharacter+=aUnicode.Length();
		}
	const TUint8* pointerToCurrentForeignByte=aForeign.Ptr();
	const TUint8* const pointerToLastForeignByte=pointerToCurrentForeignByte+(aForeign.Length()-1);
	if (aConversionData.iForeignVariableByteData.iNumberOfRanges<=0)
	    {
	    OstTrace0( TRACE_DUMP, CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicBadNumberOfRanges2" );
	    }
	__ASSERT_DEBUG(aConversionData.iForeignVariableByteData.iNumberOfRanges>0, Panic(EPanicBadNumberOfRanges2));
	const SCnvConversionData::SVariableByteData::SRange* const foreignVariableByteData_firstRange=aConversionData.iForeignVariableByteData.iRangeArray;
	const SCnvConversionData::SVariableByteData::SRange* const foreignVariableByteData_lastRange=foreignVariableByteData_firstRange+(aConversionData.iForeignVariableByteData.iNumberOfRanges-1);
	if (aConversionData.iForeignToUnicodeData.iNumberOfRanges<=0)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicBadNumberOfRanges3" );
	    }
	__ASSERT_DEBUG(aConversionData.iForeignToUnicodeData.iNumberOfRanges>0, Panic(EPanicBadNumberOfRanges3));
	const SCnvConversionData::SOneDirectionData::SRange* const oneDirectionData_firstRange=aConversionData.iForeignToUnicodeData.iRangeArray;
	const SCnvConversionData::SOneDirectionData::SRange* const oneDirectionData_lastRange=oneDirectionData_firstRange+(aConversionData.iForeignToUnicodeData.iNumberOfRanges-1);
	FOREVER
		{
		if (pointerToPreviousUnicodeCharacter>pointerToLastUnicodeCharacter)
		    {
		    OstTrace0( TRACE_DUMP, DUP2_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit15" );
		    }
		__ASSERT_DEBUG(pointerToPreviousUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit15));
		if (pointerToCurrentForeignByte>pointerToLastForeignByte)
		    {
		    OstTrace0( TRACE_DUMP, DUP3_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit16" );
		    }
		__ASSERT_DEBUG(pointerToCurrentForeignByte<=pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit16));
		TBool stop=EFalse;
		TUint inputCharacterCode=*pointerToCurrentForeignByte;
		const SCnvConversionData::SVariableByteData::SRange* foreignVariableByteData_currentRange=foreignVariableByteData_firstRange;
		FOREVER
			{
			if (foreignVariableByteData_currentRange->iNumberOfSubsequentBytes>=sizeof(TUint))
			    {
			    OstTrace0( TRACE_DUMP, DUP4_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicBadNumberOfSubsequentBytes" );
			    }
			__ASSERT_DEBUG(foreignVariableByteData_currentRange->iNumberOfSubsequentBytes<sizeof(TUint), Panic(EPanicBadNumberOfSubsequentBytes));
			if ((inputCharacterCode>=foreignVariableByteData_currentRange->iFirstInitialByteValueInRange) && (inputCharacterCode<=foreignVariableByteData_currentRange->iLastInitialByteValueInRange))
				{
				const TInt numberOfSubsequentBytes=foreignVariableByteData_currentRange->iNumberOfSubsequentBytes;
				if (pointerToCurrentForeignByte>pointerToLastForeignByte)
				    {
				    OstTrace0( TRACE_DUMP, DUP5_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit17" );
				    }
				__ASSERT_DEBUG(pointerToCurrentForeignByte<=pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit17));
				if (pointerToLastForeignByte-pointerToCurrentForeignByte<numberOfSubsequentBytes)
					{
					aOutputConversionFlags|=EOutputConversionFlagInputIsTruncated;
					goto end;
					}
				switch (EndiannessOfForeignCharacters(aConversionData, aDefaultEndiannessOfForeignCharacters))
					{
				case ELittleEndian:
					{
					for (TInt i=1; i<=numberOfSubsequentBytes; ++i)
						{
						if (pointerToCurrentForeignByte>=pointerToLastForeignByte)
						    {
						    OstTrace0( TRACE_DUMP, DUP6_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit18" );
						    }
						__ASSERT_DEBUG(pointerToCurrentForeignByte<pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit18));
						++pointerToCurrentForeignByte;
						TUint currentForeignByte=*pointerToCurrentForeignByte;
						currentForeignByte<<=(i*8);
						inputCharacterCode|=currentForeignByte;
						}
					}
					break;
				case EBigEndian:
					{
					for (TInt i=numberOfSubsequentBytes; i>0; --i)
						{
						if (pointerToCurrentForeignByte>=pointerToLastForeignByte)
						    {
						    OstTrace0( TRACE_DUMP, DUP7_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit19" );
						    }
						__ASSERT_DEBUG(pointerToCurrentForeignByte<pointerToLastForeignByte, Panic(EPanicPointerPastUpperLimit19));
						++pointerToCurrentForeignByte;
						inputCharacterCode<<=8;
						inputCharacterCode|=*pointerToCurrentForeignByte;
						}
					}
					break;
#if defined(_DEBUG)
				default:
				    OstTrace0( TRACE_DUMP, DUP8_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicBadEndianness2" );
					Panic(EPanicBadEndianness2);
					break;
#endif
					}
				pointerToCurrentForeignByte-=numberOfSubsequentBytes; // resets pointerToCurrentForeignByte to its value before the loop above
				break;
				}
			if (foreignVariableByteData_currentRange>foreignVariableByteData_lastRange)
			    {
			    OstTrace0( TRACE_DUMP, DUP9_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit20" );
			    }
			__ASSERT_DEBUG(foreignVariableByteData_currentRange<=foreignVariableByteData_lastRange, Panic(EPanicPointerPastUpperLimit20));
			if (foreignVariableByteData_currentRange>=foreignVariableByteData_lastRange)
				{
				return EErrorIllFormedInput;
				}
			++foreignVariableByteData_currentRange;
			}
		const SCnvConversionData::SOneDirectionData::SRange* oneDirectionData_currentRange=oneDirectionData_firstRange;
		TUint outputCharacterCode=KNoConversionAvailable;
		FOREVER
			{
			if ((inputCharacterCode>=oneDirectionData_currentRange->iFirstInputCharacterCodeInRange) &&
				(inputCharacterCode<=oneDirectionData_currentRange->iLastInputCharacterCodeInRange))
				{
				outputCharacterCode=OutputCharacterCode(inputCharacterCode, *oneDirectionData_currentRange);
				if (outputCharacterCode!=KNoConversionAvailable)
					{
					break;
					}
				}
			if (oneDirectionData_currentRange>oneDirectionData_lastRange)
			    {
			    OstTrace0( TRACE_DUMP, DUP10_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit21" );
			    }
			__ASSERT_DEBUG(oneDirectionData_currentRange<=oneDirectionData_lastRange, Panic(EPanicPointerPastUpperLimit21));
			if (oneDirectionData_currentRange>=oneDirectionData_lastRange)
				{
				break;
				}
			++oneDirectionData_currentRange;
			}
		if (pointerToPreviousUnicodeCharacter>pointerToLastUnicodeCharacter)
		    {
		    OstTrace0( TRACE_DUMP, DUP11_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit22" );
		    }
		__ASSERT_DEBUG(pointerToPreviousUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit22));
		if (pointerToPreviousUnicodeCharacter==pointerToLastUnicodeCharacter)
			{
			goto end;
			}
		if (outputCharacterCode==KNoConversionAvailable)
			{
			outputCharacterCode=0xfffd; // Unicode's "REPLACEMENT CHARACTER"
			if (aNumberOfUnconvertibleCharacters<0)
			    {
			    OstTrace0( TRACE_FATAL, DUP13_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicBadNumberOfUnconvertibleCharacters" );
			    }
			__ASSERT_ALWAYS(aNumberOfUnconvertibleCharacters>=0, Panic(EPanicBadNumberOfUnconvertibleCharacters));
			if (aNumberOfUnconvertibleCharacters<=0)
				{
				aIndexOfFirstByteOfFirstUnconvertibleCharacter=pointerToCurrentForeignByte-aForeign.Ptr();
				}
			++aNumberOfUnconvertibleCharacters;
			if (aInputConversionFlags&EInputConversionFlagStopAtFirstUnconvertibleCharacter)
				{
				stop=ETrue;
				}
			}
		if (outputCharacterCode<0x00010000)
			{
			++pointerToPreviousUnicodeCharacter;
			*pointerToPreviousUnicodeCharacter=STATIC_CAST(TUint16, outputCharacterCode);
			}
		else
			{
			if (outputCharacterCode>=0x00110000)
			    {
			    OstTrace0( TRACE_DUMP, DUP12_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicBadNon16BitCharacterCode2" );
				}
			__ASSERT_DEBUG(outputCharacterCode<0x00110000, Panic(EPanicBadNon16BitCharacterCode2));
			if (pointerToPreviousUnicodeCharacter>pointerToLastUnicodeCharacter)
			    {
			    OstTrace0( TRACE_DUMP, DUP14_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit23" );
			    }
			__ASSERT_DEBUG(pointerToPreviousUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicPointerPastUpperLimit23));
			if (pointerToLastUnicodeCharacter-pointerToPreviousUnicodeCharacter<2)
				{
				goto end;
				}
			outputCharacterCode-=0x00010000;
			++pointerToPreviousUnicodeCharacter;
			*pointerToPreviousUnicodeCharacter=STATIC_CAST(TUint16, (outputCharacterCode>>10)|0xd800);
			++pointerToPreviousUnicodeCharacter;
			*pointerToPreviousUnicodeCharacter=STATIC_CAST(TUint16, (outputCharacterCode&0x000003ff)|0xdc00);
			}
		if (pointerToLastForeignByte-pointerToCurrentForeignByte<foreignVariableByteData_currentRange->iNumberOfSubsequentBytes)
		    {
		    OstTrace0( TRACE_DUMP, DUP15_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastUpperLimit24" );
		    }
		__ASSERT_DEBUG(pointerToLastForeignByte-pointerToCurrentForeignByte>=foreignVariableByteData_currentRange->iNumberOfSubsequentBytes, Panic(EPanicPointerPastUpperLimit24));
		pointerToCurrentForeignByte+=foreignVariableByteData_currentRange->iNumberOfSubsequentBytes;
		if (pointerToCurrentForeignByte==pointerToLastForeignByte)
			{
			++pointerToCurrentForeignByte; // this increment is done regardless of the test just above, but it's not done before the test as it may make pointerToCurrentForeignByte greater than pointerToLastForeignByte, and if pointerToLastForeignByte just happens to be pointing at 0xffffffff, will pointerToCurrentForeignByte will be pointing at 0x00000000, thus the test (which would now be "if (pointerToCurrentForeignByte>pointerToLastForeignByte)") would fail
			goto end;
			}
		++pointerToCurrentForeignByte;
		if (stop)
			{
			goto end;
			}
		}
end:
    if (pointerToCurrentForeignByte<aForeign.Ptr())
        {
        OstTrace0( TRACE_DUMP, DUP16_CCNVCHARACTERSETCONVERTER_DOCONVERTTOUNICODE, "EPanicPointerPastLowerLimit2" );
        }
	__ASSERT_DEBUG(pointerToCurrentForeignByte>=aForeign.Ptr(), Panic(EPanicPointerPastLowerLimit2));
	if ((pointerToCurrentForeignByte<=aForeign.Ptr()) && (aOutputConversionFlags&EOutputConversionFlagInputIsTruncated) && (~aInputConversionFlags&EInputConversionFlagAllowTruncatedInputNotEvenPartlyConsumable))
		{
		return EErrorIllFormedInput;
		}
	aUnicode.SetLength((pointerToPreviousUnicodeCharacter+1)-aUnicode.Ptr());
	return pointerToLastForeignByte-(pointerToCurrentForeignByte-1);
	}


/** Returns a ready-made SCnvConversionData object for converting between 
Unicode and ASCII. This can be passed into the aConversionData parameter to 
DoConvertFromUnicode() or DoConvertToUnicode().

Note: This utility function should only be called by a plug-in conversion 
library.

@return ASCII conversion data object. */
EXPORT_C const SCnvConversionData& CCnvCharacterSetConverter::AsciiConversionData()
	{
	return asciiConversionData;
	}

CCnvCharacterSetConverter::CCnvCharacterSetConverter()
	:iStoredFlags(0),
	 iCharacterSetIdentifierOfLoadedConversionData(0),
	 iConversionData(NULL),
	 iDefaultEndiannessOfForeignCharacters(ELittleEndian),
	 iDowngradeForExoticLineTerminatingCharacters(EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed),
	 iReplacementForUnconvertibleUnicodeCharacters(KNullDesC8),
	 iStandardNamesAndMibEnums(NULL),
	 iIsSystemStandardNamesAndMibEnumsScanned(EFalse)
	{
	}

void CCnvCharacterSetConverter::ConstructL()
	{
	iCharsetCnvCache = CCharsetCnvCache::NewL();
	TTlsData::CharacterSetConverterIsBeingCreatedL();
	iTlsDataConstructed = ETrue;
	}

// set aIdentifierOfOnlyCharacterSetOfInterest to 0 if all character sets are of interest
//RFs& aFileServerSession function parameter is not used anymore.ECom plugin framework used.
CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* 
CCnvCharacterSetConverter::DoCreateArrayOfCharacterSetsAvailableLC(
								RFs& /*aFileServerSession*/, 
								TUint aIdentifierOfOnlyCharacterSetOfInterest) 
	{
	CArrayFix<SCharacterSet>* arrayOfCharacterSetsAvailable=CDeepDestructingArrayOfCharactersSets::NewLC(12);
	if ((AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierUtf7,               KLitCharacterSetNameUtf7				)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierUtf8,               KLitCharacterSetNameUtf8				)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierImapUtf7,           KLitCharacterSetNameImapUtf7			)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierJavaConformantUtf8, KLitCharacterSetNameJavaConformantUtf8	)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierCodePage1252,       KLitCharacterSetNameCodePage1252		)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierWin1252,       		KLitCharacterSetNameCodePage1252		)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierIso88591,           KLitCharacterSetNameIso88591			)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierAscii,              KLitCharacterSetNameAscii				)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierUnicodeLittle,		KLitCharacterSetNameUnicodeLittle		)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierUnicodeBig,			KLitCharacterSetNameUnicodeBig			)==EStopCharacterSetSearch) ||
		(AppendHardCodedCharacterSetIfRequiredL(*arrayOfCharacterSetsAvailable, aIdentifierOfOnlyCharacterSetOfInterest, KCharacterSetIdentifierSms7Bit,            KLitCharacterSetNameSms7Bit				)==EStopCharacterSetSearch))
		{
		return arrayOfCharacterSetsAvailable;
		}
	
	//Look for ECOM Character Set Convertors...
 	RImplInfoPtrArray convertorsImplArray;
 	REComSession::ListImplementationsL(KCharacterSetConvertorInterfaceUid, convertorsImplArray);
 	CleanupStack::PushL(TCleanupItem(ResetAndDestroyRImplInfoPtrArray, &convertorsImplArray));
 	TBool fileIsDll = ETrue;
	
	for(TInt i = 0; i < convertorsImplArray.Count(); i++)
		{
		CImplementationInformation* implInfo = convertorsImplArray[i];
		const TUint characterSetIdentifier = (implInfo->ImplementationUid()).iUid;

		if(aIdentifierOfOnlyCharacterSetOfInterest == 0) 
		// only if we're interested in all character sets do we need to make sure we've not already added this one to the array
			{
			for(TInt j = arrayOfCharacterSetsAvailable->Count() - 1; j >= 0; --j)
				{
				if((*arrayOfCharacterSetsAvailable)[j].Identifier() == characterSetIdentifier)
					{
					goto afterAddingNewCharacterSet;
					}
				}
			}

#if defined(_DEBUG)
			{
			for(TInt j = arrayOfCharacterSetsAvailable->Count() - 1; j >= 0; --j)
				{
				if ((*arrayOfCharacterSetsAvailable)[j].Identifier() == characterSetIdentifier)
				    {
				    OstTrace0( TRACE_DUMP, DUP1_CCNVCHARACTERSETCONVERTER_DOCREATEARRAYOFCHARACTERSETSAVAILABLELC, "EPanicCharacterSetAlreadyAdded" );
				    }
				__ASSERT_DEBUG((*arrayOfCharacterSetsAvailable)[j].Identifier() != characterSetIdentifier, Panic(EPanicCharacterSetAlreadyAdded));
				}
			}
#endif

		if((aIdentifierOfOnlyCharacterSetOfInterest == 0) || (aIdentifierOfOnlyCharacterSetOfInterest == characterSetIdentifier))
			{
			SCharacterSet characterSet;
			characterSet.iIdentifier = characterSetIdentifier;
			characterSet.iFlags = SCharacterSet::EFlagNameIsFileName;

			if(fileIsDll)
				{
				characterSet.iFlags |= SCharacterSet::EFlagFileIsConversionPlugInLibrary;
				}

			characterSet.iName = implInfo->DisplayName().AllocLC();

			arrayOfCharacterSetsAvailable->AppendL(characterSet);
			
			OstTraceDefExt1( OST_TRACE_CATEGORY_PRODUCTION | OST_TRACE_CATEGORY_RND, TRACE_INTERNALS, 
			            CCNVCHARACTERSETCONVERTER_DOCREATEARRAYOFCHARACTERSETSAVAILABLELC, 
			             "%S Appended to Character Set Array",  *(characterSet.iName) );

			CleanupStack::Pop(characterSet.iName); //characterSet.iName
			}

			if(aIdentifierOfOnlyCharacterSetOfInterest == characterSetIdentifier)
				{
					CleanupStack::PopAndDestroy(&convertorsImplArray); //convertorsImplArray
					return arrayOfCharacterSetsAvailable;
				}

afterAddingNewCharacterSet:
				; //dummy statement after label to prevent compiler warning

		}//end of for(TInt i = 0; i < convertorsImplArray.Count(); i++)

	CleanupStack::PopAndDestroy(&convertorsImplArray); //convertorsImplArray

	return arrayOfCharacterSetsAvailable;
	}

CCnvCharacterSetConverter::TCharacterSetSearch 
CCnvCharacterSetConverter::AppendHardCodedCharacterSetIfRequiredL(
										CArrayFix<SCharacterSet>& aArrayOfCharacterSets, 
										TUint aIdentifierOfOnlyCharacterSetOfInterest, 
										TUint aIdentifierOfHardCodedCharacterSet, 
										const TDesC& aNameOfHardCodedCharacterSet)
	{
	if ((aIdentifierOfOnlyCharacterSetOfInterest==0) || (aIdentifierOfOnlyCharacterSetOfInterest==aIdentifierOfHardCodedCharacterSet))
		{
		SCharacterSet characterSet;
		characterSet.iIdentifier=aIdentifierOfHardCodedCharacterSet;
		characterSet.iFlags=0;
		characterSet.iName=aNameOfHardCodedCharacterSet.AllocLC();
		aArrayOfCharacterSets.AppendL(characterSet);
		CleanupStack::Pop(); // characterSet.iName
		if (aIdentifierOfOnlyCharacterSetOfInterest==aIdentifierOfHardCodedCharacterSet)
			{
			return EStopCharacterSetSearch;
			}
		}
	return EContinueCharacterSetSearch;
	}

void 
CCnvCharacterSetConverter::ScanForStandardNamesAndMibEnumsROMOnlyL(RFs& aFileServerSession)
	{
	CStandardNamesAndMibEnums* standardNamesAndMibEnums=CStandardNamesAndMibEnums::NewLC();
	TFindFile* findFile=new(ELeave) TFindFile(aFileServerSession);
	CleanupStack::PushL(findFile);
	TParse* fileNameParser=new(ELeave) TParse;
	CleanupStack::PushL(fileNameParser);
	CDir* directory=NULL;
	TInt findResult=findFile->FindWildByDir(KLitWildCard, KLitROMSystemCharconvDirectory, directory);
	if (findResult==KErrNone)
		{
		CleanupStack::PushL(directory);
		for (TInt i=directory->Count()-1; i>=0; --i)
			{
			const TEntry& entry=(*directory)[i];
			if (entry.iType[0].iUid==0x1000589b)
				{
				fileNameParser->SetNoWild(entry.iName, &findFile->File(), NULL);
				standardNamesAndMibEnums->AddFromFileL(aFileServerSession, fileNameParser->FullName());
				}
			}
		CleanupStack::PopAndDestroy(); // directory
		directory=NULL;
		}
	delete directory;
	CleanupStack::PopAndDestroy(2); // fileNameParser and findFile
	delete iStandardNamesAndMibEnums;
	iStandardNamesAndMibEnums=standardNamesAndMibEnums;
	CleanupStack::Pop(); // standardNamesAndMibEnums
		}


void CCnvCharacterSetConverter::ScanForStandardNamesAndMibEnumsL(RFs& aFileServerSession)
	{
	CStandardNamesAndMibEnums* standardNamesAndMibEnums=CStandardNamesAndMibEnums::NewLC();
	TFindFile* findFile=new(ELeave) TFindFile(aFileServerSession);
	CleanupStack::PushL(findFile);
	TParse* fileNameParser=new(ELeave) TParse;
	CleanupStack::PushL(fileNameParser);
	CDir* directory=NULL;
	for (TInt findResult=findFile->FindWildByDir(KLitWildCard, KLitSystemCharconvDirectory, directory); findResult!=KErrNotFound; findResult=findFile->FindWild(directory))
		{
		CleanupStack::PushL(directory);
		User::LeaveIfError(findResult);
		for (TInt i=directory->Count()-1; i>=0; --i)
			{
			const TEntry& entry=(*directory)[i];
			if (entry.iType[0].iUid==0x1000589b)
				{
				fileNameParser->SetNoWild(entry.iName, &findFile->File(), NULL);
				standardNamesAndMibEnums->AddFromFileL(aFileServerSession, fileNameParser->FullName());
				}
			}
		CleanupStack::PopAndDestroy(); // directory
		directory=NULL;
		}
	delete directory;
	CleanupStack::PopAndDestroy(2); // fileNameParser and findFile
	delete iStandardNamesAndMibEnums;
	iStandardNamesAndMibEnums=standardNamesAndMibEnums;
	CleanupStack::Pop(); // standardNamesAndMibEnums
	}


CCnvCharacterSetConverter::TAvailability 
CCnvCharacterSetConverter::DoPrepareToConvertToOrFromL(
								TUint aCharacterSetIdentifier, 
								const CArrayFix<SCharacterSet>* aArrayOfCharacterSetsAvailable, 
								RFs& aFileServerSession)
	{
    OstTraceExt2( TRACE_DUMP, DUP3_CCNVCHARACTERSETCONVERTER_DOPREPARETOCONVERTTOORFROML, "Prepare to convert aCharacterSetIdentifier(0x%x) in aArrayOfCharacterSetsAvailable(0x%x)", aCharacterSetIdentifier, (unsigned int)aArrayOfCharacterSetsAvailable);
    
	//AutoDetectCharacterSetL relies on the fact that this function does not use 
	//aFileServerSession if aArrayOfCharacterSetsAvailable is *not* NULL and 
	//if aCharacterSetIdentifier is *not* a data file
	// aFileServerSession is no longer used load Plugin libraries. ECom framework used instead
	if (aCharacterSetIdentifier==0)
	    {
	    OstTrace0( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_DOPREPARETOCONVERTTOORFROML, "EPanicNullCharacterSetIdentifier3" );
	    }
	__ASSERT_ALWAYS(aCharacterSetIdentifier!=0, Panic(EPanicNullCharacterSetIdentifier3));
	if (iCharacterSetIdentifierOfLoadedConversionData!=aCharacterSetIdentifier)
		{

		TUint newStoredFlags=0;
		const SCnvConversionData* newConversionData=NULL;
		TBuf8<KMaximumLengthOfReplacementForUnconvertibleUnicodeCharacters> replacementForUnconvertibleUnicodeCharacters(KNullDesC8);
		switch (aCharacterSetIdentifier)
			{
		case KCharacterSetIdentifierUtf7:
		case KCharacterSetIdentifierUtf8:
		case KCharacterSetIdentifierImapUtf7:
		case KCharacterSetIdentifierJavaConformantUtf8:
			break;
		case KCharacterSetIdentifierCodePage1252:
		case KCharacterSetIdentifierWin1252:
			{
			newConversionData=&codePage1252ConversionData;
			replacementForUnconvertibleUnicodeCharacters=KLit8AsciiSubstituteCharacter;
			}
			break;
		case KCharacterSetIdentifierIso88591:
			{
			newConversionData=&iso88591ConversionData;
			replacementForUnconvertibleUnicodeCharacters=KLit8AsciiSubstituteCharacter;
			}
			break;
		case KCharacterSetIdentifierAscii:
			{
			newConversionData=&asciiConversionData;
			replacementForUnconvertibleUnicodeCharacters=KLit8AsciiSubstituteCharacter;
			}
			break;
		case KCharacterSetIdentifierSms7Bit:
			{
			newConversionData=&sms7BitConversionData;
			replacementForUnconvertibleUnicodeCharacters=KLit8Sms7BitSubstituteCharacter;
			}
			break;
		case KCharacterSetIdentifierUnicodeLittle:
			{
			newConversionData=&unicodeConversionDataLittle;
			}
			break;
		case KCharacterSetIdentifierUnicodeBig:
			{
			newConversionData=&unicodeConversionDataBig;
			}
			break;
		default:
			if (aArrayOfCharacterSetsAvailable==NULL)
				{
				aArrayOfCharacterSetsAvailable=DoCreateArrayOfCharacterSetsAvailableLC(aFileServerSession, aCharacterSetIdentifier);
				}
			else
				{
				CleanupStack::PushL(STATIC_CAST(TAny*, NULL)); // dummy item on the cleanup-stack so that we can simply CleanupStack::PopAndDestroy() on ending the loop below
				}

			for (TInt i=aArrayOfCharacterSetsAvailable->Count()-1; ; --i)
				{
				if (i<0)
					{
					CleanupStack::PopAndDestroy(); // aArrayOfCharacterSetsAvailable or dummy NULL pointer
					return ENotAvailable;
					}
				const SCharacterSet& characterSet=(*aArrayOfCharacterSetsAvailable)[i];
				if (characterSet.Identifier()==aCharacterSetIdentifier)
					{
					if (!characterSet.NameIsFileName())
					    {
					    OstTrace0( TRACE_DUMP, DUP1_CCNVCHARACTERSETCONVERTER_DOPREPARETOCONVERTTOORFROML, "EPanicNameIsNotFileName" );
					    }
					__ASSERT_DEBUG(characterSet.NameIsFileName(), Panic(EPanicNameIsNotFileName));
					if (characterSet.FileIsConversionPlugInLibrary())
						{
						newStoredFlags|=EStoredFlagConversionPlugInLibraryIsLoaded;
						
						TUid implUid;
 						implUid.iUid = aCharacterSetIdentifier;
 						replacementForUnconvertibleUnicodeCharacters = (iCharsetCnvCache->GetConverterL(implUid))->ReplacementForUnconvertibleUnicodeCharacters();
						}
					else
						{
						//You are here?! This should never happen! Source code here was related to
						//old type character set converter data!
						OstTrace0( TRACE_FATAL, DUP2_CCNVCHARACTERSETCONVERTER_DOPREPARETOCONVERTTOORFROML, "EPanicCharacterSetNotPresent" );
 						__ASSERT_ALWAYS(EFalse, Panic(EPanicCharacterSetNotPresent));
						}
					break;
					}
				}
			CleanupStack::PopAndDestroy(); // aArrayOfCharacterSetsAvailable or dummy NULL pointer
			break;
			}
		iStoredFlags&=~EStoredFlagConversionPlugInLibraryIsLoaded;
		iCharacterSetIdentifierOfLoadedConversionData=aCharacterSetIdentifier;
		iStoredFlags|=newStoredFlags;
		iConversionData=newConversionData;
		iDefaultEndiannessOfForeignCharacters=ELittleEndian;
		iDowngradeForExoticLineTerminatingCharacters=EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed;
		iReplacementForUnconvertibleUnicodeCharacters=replacementForUnconvertibleUnicodeCharacters;
		}
	return EAvailable;
	}

LOCAL_C void DeleteOneDirectionData(
								TInt aNumberOfRanges, 
								const SCnvConversionData::SOneDirectionData::SRange* aRange)
	{
	if ( !((aRange!=NULL) || (aNumberOfRanges==0)) )
	    {
	    OstTrace0( TRACE_DUMP, _DELETEONEDIRECTIONDATA, "EPanicBadNumberOfRanges4" );
	    }
	__ASSERT_DEBUG((aRange!=NULL) || (aNumberOfRanges==0), Panic(EPanicBadNumberOfRanges4));
	if (aRange!=NULL)
		{
		if (aNumberOfRanges<=0)
		    {
		    OstTrace0( TRACE_DUMP, DUP1__DELETEONEDIRECTIONDATA, "EPanicBadNumberOfRanges5" );
		    }
		__ASSERT_DEBUG(aNumberOfRanges>0, Panic(EPanicBadNumberOfRanges5));
		SCnvConversionData::SOneDirectionData::SRange* currentRange=CONST_CAST(SCnvConversionData::SOneDirectionData::SRange*, aRange);
		const SCnvConversionData::SOneDirectionData::SRange* const lastRange=currentRange+(aNumberOfRanges-1);
		FOREVER
			{
			switch (currentRange->iAlgorithm)
				{
			case SCnvConversionData::SOneDirectionData::SRange::EDirect:
			case SCnvConversionData::SOneDirectionData::SRange::EOffset:
				break;
			case SCnvConversionData::SOneDirectionData::SRange::EIndexedTable16:
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
				delete [] CONST_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16*, currentRange->iData.iIndexedTable16.iEntryArray);
#else
				delete [] REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16*, currentRange->iData.iWord1);
#endif
				break;
			case SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616:
#if defined(CONST_STATIC_UNIONS_ARE_POSSIBLE)
				delete [] CONST_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616*, currentRange->iData.iKeyedTable1616.iEntryArray);
#else
				delete [] REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616*, currentRange->iData.iWord2);
#endif
				break;
			case SCnvConversionData::SOneDirectionData::SRange::EKeyedTable16OfIndexedTables16:
				// fall through
#if defined(_DEBUG)
			default:
			    OstTrace0( TRACE_FATAL, DUP2__DELETEONEDIRECTIONDATA, "EPanicBadAlgorithm2" );
				Panic(EPanicBadAlgorithm2);
#endif
				break;
				}
			if (currentRange>lastRange)
			    {
			    OstTrace0( TRACE_DUMP, DUP3__DELETEONEDIRECTIONDATA, "EPanicPointerPastUpperLimit25" );
			    }
			__ASSERT_DEBUG(currentRange<=lastRange, Panic(EPanicPointerPastUpperLimit25));
			if (currentRange==lastRange)
				{
				break;
				}
			++currentRange;
			}
		delete [] CONST_CAST(SCnvConversionData::SOneDirectionData::SRange*, aRange);
		}
	}

void CCnvCharacterSetConverter::DeleteConversionData(const SCnvConversionData* aConversionData)
	{
	if (aConversionData!=NULL)
		{
		delete [] CONST_CAST(SCnvConversionData::SVariableByteData::SRange*, aConversionData->iForeignVariableByteData.iRangeArray);
		DeleteOneDirectionData(aConversionData->iForeignToUnicodeData.iNumberOfRanges, aConversionData->iForeignToUnicodeData.iRangeArray);
		DeleteOneDirectionData(aConversionData->iUnicodeToForeignData.iNumberOfRanges, aConversionData->iUnicodeToForeignData.iRangeArray);
		delete CONST_CAST(SCnvConversionData*, aConversionData);
		}
	}

void CCnvCharacterSetConverter::DeleteConversionData(TAny* aConversionData)
	{
	DeleteConversionData(STATIC_CAST(SCnvConversionData*, aConversionData));
	}

CCnvCharacterSetConverter::TEndianness 
CCnvCharacterSetConverter::EndiannessOfForeignCharacters(
											const SCnvConversionData& aConversionData, 
											TEndianness aDefaultEndiannessOfForeignCharacters)
	{
	switch (aConversionData.iEndiannessOfForeignCharacters)
		{
	case SCnvConversionData::EUnspecified:
		return aDefaultEndiannessOfForeignCharacters;
	case SCnvConversionData::EFixedLittleEndian:
		return ELittleEndian;
	case SCnvConversionData::EFixedBigEndian:
		return EBigEndian;
		}
#if defined(_DEBUG)
	OstTrace0( TRACE_DUMP, CCNVCHARACTERSETCONVERTER_ENDIANNESSOFFOREIGNCHARACTERS, "EPanicBadEndianness3" );
	Panic(EPanicBadEndianness3);
#endif
	return ELittleEndian; // dummy return to prevent compiler error
	}

// CCnvCharacterSetConverter::TArrayOfAscendingIndices

/** Appends an index to the array of indices.

The value of aIndex should be greater than that of the
last index in the array, to maintain an ascending array. The return
value should be tested to see whether the function succeeded or not.

@param aIndex The index to append to the array.
@return EAppendFailed if the append failed, or
EAppendSuccessful if it succeeded. */
EXPORT_C CCnvCharacterSetConverter::TArrayOfAscendingIndices::TAppendResult 
CCnvCharacterSetConverter::TArrayOfAscendingIndices::AppendIndex(TInt aIndex)
 	{
 	if ( aIndex<0 )
 	    {
 	    OstTrace1( TRACE_DUMP, DUP2_TARRAYOFASCENDINGINDICES_APPENDINDEX, "Bad index in TArrayOfAscendingIndices::AppendIndex;aIndex=%d", aIndex );    
 	    }
	__ASSERT_DEBUG(aIndex>=0, Panic(EPanicBadIndex));
	const TInt lengthOfArrayOfIndices=iArrayOfIndices.Length();
	if ((aIndex>STATIC_CAST(TInt, KMaxTUint16)) || (lengthOfArrayOfIndices==iArrayOfIndices.MaxLength()))
		{
		return EAppendFailed;
		}
	if ( (lengthOfArrayOfIndices!=0) && (iArrayOfIndices[lengthOfArrayOfIndices-1]>=aIndex))
	    {
	    OstTrace1( TRACE_DUMP, DUP1_TARRAYOFASCENDINGINDICES_APPENDINDEX, "Duplicate Index Or Not Ascending in TArrayOfAscendingIndices::AppendIndex;aIndex=%d", aIndex );
	    }
	__ASSERT_DEBUG((lengthOfArrayOfIndices==0) || (iArrayOfIndices[lengthOfArrayOfIndices-1]<aIndex), Panic(EPanicDuplicateIndexOrNotAscending));
	iArrayOfIndices.Append(aIndex);
	return EAppendSuccessful;
	}
	
/**
The method sets the max size of the internal character set converter cache.
The cache is used mainly to improve the performance of AutoDetectCharSetL() calls.
It caches loaded converter implementations. The next time when a specific implementation is needed,
a search will be done in the cache if this implementation is already loaded and if it is there,
the cached implementation will be used. SetMaxCacheSize() call
is used to limit the max cache size, because the loaded implementatiions may consume a lot of the 
system resources (memory for example).
By default (if SetMaxCacheSize() is never called) the max cache size is limited to 32 entries.
Note: Setting very small cache size will impact the overall performance of CHARCONV functions.
		If the choosen cache size is less than the number of existing character set converter 
		implementations, there will be no performance gain or it will be far beyond client's 
		expectations. For best performance the choosen cache size should be bigger or equal to 
		the number of the existing character set converter implementations.
@param aSize The new max cache size. It must be bigger or equal to 4.
@panic User::Invariant() if the new max cache size is less than 4.
@see CCnvCharacterSetConverter::AutoDetectCharSetL
*/
EXPORT_C void CCnvCharacterSetConverter::SetMaxCacheSize(TInt aSize)
    {
    if ( aSize < CCharsetCnvCache::KMinCacheSize )
        {
        OstTrace1( TRACE_FATAL, CCNVCHARACTERSETCONVERTER_SETMAXCACHESIZE, "Parameter aSize < KMinCacheSize in CCnvCharacterSetConverter::SetMaxCacheSize;aSize=%d", aSize );
        }
    __ASSERT_ALWAYS(aSize >= CCharsetCnvCache::KMinCacheSize, User::Invariant());
    iCharsetCnvCache->SetMaxSize(aSize);
    }

