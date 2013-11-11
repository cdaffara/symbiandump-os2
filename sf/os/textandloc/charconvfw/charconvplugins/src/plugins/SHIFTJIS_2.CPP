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
#include <charconv.h>
#include "SHIFTJIS_2.H"
#include <ecom/implementationproxy.h>
#include "charactersetconverter.h"
#include "featmgr/featmgr.h"

/**
Shift-JIS character converter wrapper

@internalTechnology 
@released 9.1
*/
class CShiftJisConverterImpl : public CCharacterSetConverterPluginInterface
	{

public:
	virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

	virtual TInt ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);

	virtual TInt ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);

	virtual TBool IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample);

	static CShiftJisConverterImpl* NewL();
	virtual ~CShiftJisConverterImpl();

private:
	CShiftJisConverterImpl();
	void ConstructL();

	};

/**
Get the the Shift-JIS byte sequence which will replace any Unicode characters which can't be converted.

@return The Shift-JIS byte sequence which will replace any Unicode characters which can't be converted.
@internalTechnology 
*/
const TDesC8& CShiftJisConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters();
	}

TInt CShiftJisConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	return CnvShiftJis::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters);
	}


/**
 Converts Shift-JIS encoded input text to Unicode
 
 NOTE: For debugging the selected character set is returned in the state.
 
  @released  9.1
  @param     aDefaultEndiannessOfForeignCharacters The default endian-ness to use when reading characters
             in the foreign character set.
  @param     aUnicode On return, contains the text converted into Unicode.
  @param     aForeign The non-Unicode source text to be converted.
  @param     aState Used to save state information across multiple calls
             to <code>ConvertToUnicode()</code>.
  @param     aNumberOfUnconvertibleCharacters On return, contains the number of bytes which were not
             converted.
  @param     aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, contains the index of the first bytein the
             input text that could not be converted. A negative
             value indicates that all the characters were
             converted.
  @return 	 The number of unconverted bytes left at the end of the input descriptor 
 		     (e.g. because the output descriptor is not long enough to hold all the text), 
 		     or one of the error values defined in TError. 
  @internalTechnology 
*/
TInt CShiftJisConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& /*aState*/, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	return CnvShiftJis::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	}


/**
 This API is used by CCnvCharacterSetConverter::AutoDetectCharacterSetL(). 
 This method returns a value between 0 and 100, indicating how likely it 
 is that this is the correct converter, for the text supplied.  
 @internalTechnology 
 */
TBool CShiftJisConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue=ETrue;
	TInt sampleLength = aSample.Length();
	aConfidenceLevel = 0;
	TInt numberOfShiftJis=0;
	TInt occurrence=0;
	for (TInt i = 0; i < sampleLength; ++i)
		{
		// Check for JISX 0208:1997 Charset
		// First Byte in range 0x81-0x9f, 0xe0-0xef
		if (((aSample[i] >= 0x81) && (aSample[i] <= 0x9f)) ||
			((aSample[i] >= 0xe0) && (aSample[i] <= 0xef)))
			{
			// check that the second byte is in range as well 
			TInt increment1 = i+1;
			if(increment1 >= sampleLength)
				break;
			if (((aSample[increment1] >= 0x40) && (aSample[increment1] <= 0x7e)) ||
				((aSample[increment1] >= 0x80) && (aSample[increment1] <= 0xfc)))
				{
				// increase the confidence of this sample as ShiftJis
				aConfidenceLevel=(aConfidenceLevel >0)?aConfidenceLevel+5:60;
	
				TUint charShiftJis=(aSample[i]<<8)|(aSample[increment1]);
				if ((charShiftJis>=0x829f)&&(charShiftJis<=0x82f1)||
					(charShiftJis>=0x8340)&&(charShiftJis<=0x8396))//those are kanas range
					occurrence++;
				numberOfShiftJis++;
				i++;
				}
			}
		// Check That no other Japanese escape sequence occur... if they do, cancel this and return 0
		// eg EUC-JP's SS(Single shift) characters followed by the
		if(aSample[i]==0x8e)
			{
			TInt increment1 = i+1;
			if(increment1 >= sampleLength)
				break;
			if ((aSample[increment1] >= 0xa1) && (aSample[increment1] <= 0xdf))
				{
				// This could be EUC-JP format..
				aConfidenceLevel=0;
				i++;
				}
			}
		if(aSample[i]==0x8f)
			{
			TInt increment1 = i+1;
			TInt increment2 = i+2;
			if((increment1 >= sampleLength) || (increment2 >= sampleLength))
				break;
			if (((aSample[increment1] >= 0xa1) && (aSample[increment1] <= 0xfe)) && 
				((aSample[increment2] >= 0xa1) && (aSample[increment2] <= 0xfe)))
				{
				// 	This is definitely EUC-JP format. 
				aConfidenceLevel=0;
				break;
				}
			}
        // Check the half width Katakana
        if (aSample[i]>=0xa1 && aSample[i]<=0xdf)
            {
            // increase the confidence of this sample as ShiftJis
            aConfidenceLevel=(aConfidenceLevel > 0) ? aConfidenceLevel+5 : 75;
            occurrence++;
            numberOfShiftJis++;
            }
        else if (aSample[i]>=0xf0)
            {
            aConfidenceLevel=0;
            }
		} // for 

	if(numberOfShiftJis)
		{
		aConfidenceLevel=(aConfidenceLevel >100)?100:((aConfidenceLevel <0)?0:aConfidenceLevel);
		aConfidenceLevel=aConfidenceLevel-Max(0,(30-occurrence*100/numberOfShiftJis));
		}
	aConfidenceLevel=(aConfidenceLevel < 0)?0:aConfidenceLevel;
	return ETrue;
	}


CShiftJisConverterImpl* CShiftJisConverterImpl::NewL()
	{
	CShiftJisConverterImpl* self = new(ELeave) CShiftJisConverterImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CShiftJisConverterImpl::~CShiftJisConverterImpl()
	{
    FeatureManager::UnInitializeLib();	
	}

CShiftJisConverterImpl::CShiftJisConverterImpl()
	{
	}


void CShiftJisConverterImpl::ConstructL()
	{
    FeatureManager::InitializeLibL();	
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10000FBD,	CShiftJisConverterImpl::NewL)
	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
