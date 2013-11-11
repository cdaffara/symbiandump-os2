/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* test code for the J5 character converter plug-in that can convert
* from any of the following character sets:
* Windows-31J (Microsoft code page 932),
* JIS (ISO-2022-JP-1),
* EUC,
* UTF8
* and UCS2.
* This differs from TJ5_Kddiau (test cases 0209 to 0216) in that it uses an alternative version of
* the Shift-JIS plug-in which can handle DoCoMo operator specific extensions.
* Note the following values of test case ID number have been allocated for this code 1349-1356
*
*/ 


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include "shiftjis.h"
#include <utf.h>
#include "t_j5_docomo.h"

#define test(cond)                                  \
    {                                               \
    TBool __bb = (cond);                            \
    TEST(__bb);                                     \
    if (!__bb)                                      \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }                                           \
    }


// test data files
#ifdef __WINS__
	// on C drive for emulator
	_LIT16(KAnsiTxt,"c:\\test\\data\\Ansi.test");
	_LIT16(KImodeTxt,"c:\\test\\data\\imode.test");
	_LIT16(KJpEUCjTxt,"c:\\test\\data\\JpEUC-J.test");
	_LIT16(KJpJISTxt,"c:\\test\\data\\JpJIS.test");
	_LIT16(KJpSJISTxt,"c:\\test\\data\\JpSJIS.test");
	_LIT16(KJpUnicodeTxt,"c:\\test\\data\\JpUnicode.test");
	_LIT16(KJpUTF8Txt,"c:\\test\\data\\JpUTF8.test");
	_LIT16(KmoperaTxt,"c:\\test\\data\\mopera.test");
	_LIT16(KMScodeFontTxt,"c:\\test\\data\\MScodefont.test");
	_LIT16(KSJISTxt,"c:\\test\\data\\S-JIS.test");

	_LIT16(KAnsiUni,"c:\\test\\data\\Ansi.uni");
	_LIT16(KImodeUni,"c:\\test\\data\\imode.uni");
	_LIT16(KJpEUCjUni,"c:\\test\\data\\JpEUC-J.uni");
	_LIT16(KJpJISUni,"c:\\test\\data\\JpJIS.uni");
	_LIT16(KJpSJISUni,"c:\\test\\data\\JpSJIS.uni");
	_LIT16(KJpUnicodeUni,"c:\\test\\data\\JpUnicode.uni");
	_LIT16(KJpUTF8Uni,"c:\\test\\data\\JpUTF8.uni");
	_LIT16(KmoperaUni,"c:\\test\\data\\mopera.uni");
	_LIT16(KMScodeFontUni,"c:\\test\\data\\MScodefont.uni");
	_LIT16(KSJISUni,"c:\\test\\data\\S-JIS_docomo.uni");
#else
	// on Z drive for target ROM
	_LIT16(KAnsiTxt,"z:\\test\\data\\Ansi.test");
	_LIT16(KImodeTxt,"z:\\test\\data\\imode.test");
	_LIT16(KJpEUCjTxt,"z:\\test\\data\\JpEUC-J.test");
	_LIT16(KJpJISTxt,"z:\\test\\data\\JpJIS.test");
	_LIT16(KJpSJISTxt,"z:\\test\\data\\JpSJIS.test");
	_LIT16(KJpUnicodeTxt,"z:\\test\\data\\JpUnicode.test");
	_LIT16(KJpUTF8Txt,"z:\\test\\data\\JpUTF8.test");
	_LIT16(KmoperaTxt,"z:\\test\\data\\mopera.test");
	_LIT16(KMScodeFontTxt,"z:\\test\\data\\MScodefont.test");
	_LIT16(KSJISTxt,"z:\\test\\data\\S-JIS.test");

	_LIT16(KAnsiUni,"z:\\test\\data\\Ansi.uni");
	_LIT16(KImodeUni,"z:\\test\\data\\imode.uni");
	_LIT16(KJpEUCjUni,"z:\\test\\data\\JpEUC-J.uni");
	_LIT16(KJpJISUni,"z:\\test\\data\\JpJIS.uni");
	_LIT16(KJpSJISUni,"z:\\test\\data\\JpSJIS.uni");
	_LIT16(KJpUnicodeUni,"z:\\test\\data\\JpUnicode.uni");
	_LIT16(KJpUTF8Uni,"z:\\test\\data\\JpUTF8.uni");
	_LIT16(KmoperaUni,"z:\\test\\data\\mopera.uni");
	_LIT16(KMScodeFontUni,"z:\\test\\data\\MScodefont.uni");
	_LIT16(KSJISUni,"z:\\test\\data\\S-JIS_docomo.uni");
#endif

	_LIT16(KAnsiOut,"c:\\test\\data\\Ansi.out");
	_LIT16(KImodeOut,"c:\\test\\data\\imode.out");
	_LIT16(KJpEUCjOut,"c:\\test\\data\\JpEUC-J.out");
	_LIT16(KJpJISOut,"c:\\test\\data\\JpJIS.out");
	_LIT16(KJpSJISOut,"c:\\test\\data\\JpSJIS.out");
	_LIT16(KJpUnicodeOut,"c:\\test\\data\\JpUnicode.out");
	_LIT16(KJpUTF8Out,"c:\\test\\data\\JpUTF8.out");
	_LIT16(KmoperaOut,"c:\\test\\data\\mopera.out");
	_LIT16(KMScodeFontOut,"c:\\test\\data\\MScodefont.out");
	_LIT16(KSJISOut,"c:\\test\\data\\S-JIS_docomo.out");

// this has been copied from J5, as this not exported
// and only needed for testing
enum TJ5Encoding
	{
	EShiftjis = 1,
	EIso2022jp1,
	EEucjp,
	EUtf8,
	EUcs2,
	EUnknown
	};


//this is the identifier for the shiftjis kddi/au test plugin rather than the
//shiftjis docomo plugin identifier
const TUint KCharacterSetTestShiftJisKddiau=0x01000001;


/**
  For development only! - used for generating unicode files
  Save the converted file as unicode
  */
#ifdef __WINS__
void CT_J5_DOCOMO::SaveConvertedUnicodeAsFile( RFs &fileServerSession,
		const TDesC &aFileName,
		const TDesC &aConvertedUnicode)
	{
 	RFile file;
	User::LeaveIfError( file.Replace( fileServerSession, aFileName, EFileWrite ));

	// get a pointer to 8bit data
	const TUint16* p16 = aConvertedUnicode.Ptr();
 	TPtrC8 p8( (TUint8 *)p16, aConvertedUnicode.Size() );


	file.Write( p8, aConvertedUnicode.Size());
	file.Close();
	}
#endif


/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1349-CP
@SYMTestCaseDesc Check that the J5 converter is available for testing
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The character conversion framework is called to check J5 is loaded.
@SYMTestExpectedResults J5 is in the list of avalable converters.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::checkJ5Available(RTest1 test, RFs &aFileServerSession)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1349-CP "));
	TBool J5found = EFalse;

	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(aFileServerSession);

	// check the list of available converters looking for J5
	INFO_PRINTF1(_L("charconv plugins available:\n"));
	for (TInt i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& charactersSet=(*arrayOfCharacterSetsAvailable)[i];
		TPtrC charactersSetName(charactersSet.Name());
		if (charactersSet.NameIsFileName())
			{
			charactersSetName.Set(TParsePtrC(charactersSetName).Name());

			if ( charactersSetName.Compare(_L("J5")) == 0)
				J5found = ETrue;
			}
		INFO_PRINTF2(_L("    %S\n"), &charactersSetName);
		}

	if (J5found)
		INFO_PRINTF1(_L("J5 available\n") );
	else
		{
		INFO_PRINTF1(_L("Test failed J5 not available\n") );
		}

	test(J5found);

	// PopAndDestroy characterSetConverter
	CleanupStack::PopAndDestroy(1);
	}

/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1350-CP
@SYMTestCaseDesc Convert the file supplied (aForeign) to unicode and then
				 check the converted data against aExpectedUnicode
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The J5 character converter is used convert ansi.test.
@SYMTestExpectedResults The result is checked against the expected result imode.uni.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::convertFileUsingJ5L(RTest1& test,
	RFs fileServerSession,
	CCnvCharacterSetConverter*aJ5converter,
	const TDesC &aForeign,
	const TDesC &aExpectedUnicode,
	const TDesC &aOutputUnicode )
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1350-CP "));
	INFO_PRINTF3(_L("Read %S check that it converts to %S \n"),&aForeign, &aExpectedUnicode);

  	// open the file & read into sourceBuffer
 	RFile source, expected;
 	User::LeaveIfError( source.Open( fileServerSession, aForeign, EFileRead ));
 	User::LeaveIfError( expected.Open( fileServerSession, aExpectedUnicode, EFileRead ));
 	TInt sourceSize = 0, expectedSize = 0;
 	source.Size(sourceSize);
 	expected.Size(expectedSize);

 	// create buffers for source file and expected unicode file
 	HBufC8 * sourceBuffer = HBufC8::NewLC(sourceSize);
 	TPtr8 sourcePtr(sourceBuffer->Des());
 	HBufC8 * expectedBuffer = HBufC8::NewLC(expectedSize);
 	TPtr8 expectedPtr(expectedBuffer->Des());

 	// buffer to convert into
 	HBufC * outputBuffer = HBufC::NewLC(expectedSize);
 	TPtr outputPtr(outputBuffer->Des());

	// read the whole source & output file into the buffer
	User::LeaveIfError( source.Read( sourcePtr, sourceSize ));
	User::LeaveIfError( expected.Read( expectedPtr, expectedSize ));

     // measure the time taken to convert.
    TTime startTime, stopTime;
    startTime.UniversalTime();

 	// now convert using J5 into unicode in the output buffer
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TInt error = KErrNone;
	error = aJ5converter->ConvertToUnicode(outputPtr, sourcePtr, state);
	if (error != KErrNone )
		{
		INFO_PRINTF2(_L("ConvertToUnicode returned %d\n"), error);
		}

    stopTime.UniversalTime();
    TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
 	TInt result = timeTaken.Int64();
    INFO_PRINTF3(_L("Time for autodetect and convert to unicode %S using J5 %d microseconds\n"),
    	&aForeign, result );

	TInt outputLength = outputBuffer->Length() ; // 16 bit words
	TInt expectedLength = expectedPtr.Length() / 2 ; // Bytes (hence divide by 2!)
	TInt checkLength = Min(outputLength, expectedLength) ;

	if (outputLength != expectedLength)
		{
		INFO_PRINTF3(_L("Fail: Output length incorrect (expected %d bytes after conversion, got %d\n"),
		              expectedLength, outputLength) ;
		}
	// check that the output buffer contains the same as the expected buffer
	TInt j=0;
	TUint8 firstByte =0;
	TUint8 secondbyte =0;

	for ( TInt i=0; i < checkLength; i++, j+=2)
		{
		// two bytes in the file - one 16bit unicode value
		firstByte = TUint8(outputPtr[i] & 0x00ff);
		secondbyte = (outputPtr[i] & 0xff00) >> 8 ;

		if ( ( firstByte == expectedPtr[j] ) && ( secondbyte == expectedPtr[j+1] ) )
			{
			// useful for development
 			/*INFO_PRINTF1(_L(" %02x%02x = %02x%02x ok\n"),
				secondbyte, firstByte, expectedPtr[j+1], expectedPtr[j]);*/
			}
		else
			{
			INFO_PRINTF6(_L("line:%d %02x%02x (output) != %02x%02x (expected) fail\n"),
				i, secondbyte, firstByte, expectedPtr[j+1], expectedPtr[j]);
			}
		}

#ifdef __WINS__
	// for debugging save as file
	SaveConvertedUnicodeAsFile( fileServerSession, aOutputUnicode, outputPtr );
#endif

	source.Close();
	expected.Close();

	// sourceBuffer, expectedBufffer, outputBuffer
	CleanupStack::PopAndDestroy(3);
	}


/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1351-CP
@SYMTestCaseDesc Check that J5 converts to UTF8 when ConvertFromUnicode() is called
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The J5 character converter is used to convert unicode to UTF8
@SYMTestExpectedResults The result is checked against the expected UTF8.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::convertFromUnicodeUsingJ5(RTest1& test,
	RFs fileServerSession,
	CCnvCharacterSetConverter*aJ5converter )
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1351-CP "));
	/* sample UTF8 for testing */
	_LIT16(KsampleUnicode,"\x0069\x002d\x006d\x006f\x0064\x0065\xFFFD");
	_LIT8(KsampleAsUTF8,"\x69\x2D\x6D\x6F\x64\x65\xEF\xBF\xBD");
	TBuf8<100> utf8buffer;

	INFO_PRINTF1(_L("Check J5 convertd from Unicode to UTF8 \n"));

    CCnvCharacterSetConverter::TAvailability available = aJ5converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierJ5, fileServerSession);
    if (  available != CCnvCharacterSetConverter::EAvailable)
        User::Leave(KErrNotSupported);

 	// J5 convert from unicode
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TInt numberNotConverted =0;
	numberNotConverted = aJ5converter->ConvertFromUnicode(utf8buffer, KsampleUnicode, state);
	if (numberNotConverted != 0)
		{
		INFO_PRINTF2(_L("numberNotConverted = %d"),	numberNotConverted );
		}

	// compare the UTF8 created to that supplied
	test(utf8buffer==KsampleAsUTF8);

	}

/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1352-CP
@SYMTestCaseDesc Check that the J5 converter autodetection
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The J5 character converter is used convert sample files.
@SYMTestExpectedResults The result is passed back for testing in the state paramater.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::checkDetection(RTest1& test,
	RFs fileServerSession,
	const TDesC &aForeign,
	TInt aExpectedCode )
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1352-CP "));
	__UHEAP_MARK;
	INFO_PRINTF3(_L("Read %S check that it converts to %d \n"),&aForeign, aExpectedCode);

  	// open the file & read into sourceBuffer
 	RFile source;
 	User::LeaveIfError( source.Open( fileServerSession, aForeign, EFileRead ));
 	TInt sourceSize = 0;
 	source.Size(sourceSize);

 	// create buffers for source file
 	HBufC8 * sourceBuffer = HBufC8::NewLC(sourceSize);
 	TPtr8 sourcePtr(sourceBuffer->Des());

 	// buffer to convert into
 	HBufC * outputBuffer = HBufC::NewLC(sourceSize * 2);
 	TPtr outputPtr(outputBuffer->Des());

	// read the whole source & output file into the buffer
	User::LeaveIfError( source.Read( sourcePtr, sourceSize ));
 	source.Close();

    // load the converter ready for testing
 	CCnvCharacterSetConverter* J5Converter=CCnvCharacterSetConverter::NewLC();
    CCnvCharacterSetConverter::TAvailability available = CCnvCharacterSetConverter::EAvailable;
    available = J5Converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierJ5, fileServerSession);
    if (  available != CCnvCharacterSetConverter::EAvailable)
        User::Leave(KErrNotSupported);

 	// now convert using J5 into unicode in the output buffer
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TInt numberCharsNotConverted = 0;
 	numberCharsNotConverted = J5Converter->ConvertToUnicode(outputPtr, sourcePtr, state);
	if ( numberCharsNotConverted != 0)
		INFO_PRINTF3(_L("number of characters not converted:%d of %d\n"),
			numberCharsNotConverted, sourceSize );

	// check that J5 has been able to convert all the data
 	test(J5Converter->ConvertToUnicode(outputPtr, sourcePtr, state)==0);

	// look at the code returned in the state
	if ( aExpectedCode == state )
		{
		INFO_PRINTF3(_L(" expected:%d found:%d ok\n"), aExpectedCode , state  );
		}
	else
		{
		INFO_PRINTF3(_L(" expected:%d found:%d failed \n"), aExpectedCode , state );
	 	test( aExpectedCode == state);
		}

	// sourceBuffer, expectedBufffer, J5Converter
	CleanupStack::PopAndDestroy(3);
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1353-CP
@SYMTestCaseDesc Check the J5 on some small data samples
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The sample data is converted to unicode by J5.
@SYMTestExpectedResults The converted unicode data is checked against expected data.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::convertDataUsingJ5(RTest1& test,
	RFs &fileServerSession,
	const TDesC8 &aForeign,
	const TDesC16 &aUnicode )
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1353-CP "));
    // for the unicode created
    TBuf16<512> unicode;

    // load the converter ready for testing
 	CCnvCharacterSetConverter* J5Converter=CCnvCharacterSetConverter::NewLC();
    CCnvCharacterSetConverter::TAvailability available = CCnvCharacterSetConverter::EAvailable;
    available = J5Converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierJ5, fileServerSession);
    if (  available != CCnvCharacterSetConverter::EAvailable)
        User::Leave(KErrNotSupported);

 	// J5 decode to unicode
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TInt numberNotConverted =0;
	numberNotConverted = J5Converter->ConvertToUnicode(unicode, aForeign, state);
	if (numberNotConverted != 0)
		{
		INFO_PRINTF2(_L("numberNotConverted = %d"),	numberNotConverted );
		}

	// compare the unicode created to that supplied
	test(unicode==aUnicode);

	// tidy up J5Converter
	CleanupStack::PopAndDestroy(1);
	}


/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1354-CP
@SYMTestCaseDesc Check that J5 does not interfere with the normal operation of Autodetect
				 And make a rough timing measurement.
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions Autodetect using other plugins is tested when J5 is available
@SYMTestExpectedResults Autodetect still works and does NOT select J5.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::checkNormalAutodetectL( RTest1& test, RFs &aFileServerSession, const TDesC &aForeign )
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1354-CP "));
    INFO_PRINTF1(_L("Test normal autodetect \n") );
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(aFileServerSession);

  	// open the file & read into sourceBuffer
 	RFile source;
 	User::LeaveIfError( source.Open( aFileServerSession, aForeign, EFileRead ));
 	TInt sourceSize = 0;
 	source.Size(sourceSize);

 	// create buffers for source file
 	HBufC8 * sourceBuffer = HBufC8::NewLC(sourceSize);
 	TPtr8 sourcePtr(sourceBuffer->Des());

	// read the whole source file into the buffer
	User::LeaveIfError( source.Read( sourcePtr, sourceSize ));
	source.Close();

    // measure the time taken to autodetect.
    TTime startTime, stopTime;
    startTime.UniversalTime();

	// do the autodetect
	TInt confidenceLevel =0;
	TUint characterSetIdentifier = 0;
	characterSetConverter->AutoDetectCharSetL( confidenceLevel,
		characterSetIdentifier, *arrayOfCharacterSetsAvailable, sourcePtr);

	//characterSetIdentifier could either be the ShiftJis Kddi/au identifier or the Shiftjis Docomo Test identifier
	//as both plugins are visible to ECOM
	test((characterSetIdentifier == KCharacterSetIdentifierShiftJis)
		||(characterSetIdentifier == KCharacterSetTestShiftJisKddiau));

    stopTime.UniversalTime();
    TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
 	TInt result = timeTaken.Int64();
    INFO_PRINTF3(_L("Time taken for old autodetect %S = %d microseconds\n"),
    	&aForeign, result );

	// characterSetConverter,arrayOfCharacterSetsAvailable, sourceBuffer
	CleanupStack::PopAndDestroy(3);
	}


/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1355-CP
@SYMTestCaseDesc Check the conversion speed of shiftjis
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The sample data is converted to unicode by J5.
@SYMTestExpectedResults The converted unicode data is checked against expected data.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::checkConversionSpeedL(RTest1& test, RFs &aFileServerSession, const TDesC &aForeign)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1355-CP "));
    INFO_PRINTF1(_L("Test conversion speed \n") );

  	// open the file & read into sourceBuffer
 	RFile source;
 	User::LeaveIfError( source.Open( aFileServerSession, aForeign, EFileRead ));
 	TInt sourceSize = 0, expectedSize = 0;
 	source.Size(sourceSize);

	// assume that every byte coverts to 2 bytes of unicode and the buffer will be big enough
	expectedSize = sourceSize * 2;

 	// create buffer for source file
 	HBufC8 * sourceBuffer = HBufC8::NewLC(sourceSize);
 	TPtr8 sourcePtr(sourceBuffer->Des());

    // for the unicode created
 	HBufC * outputBuffer = HBufC::NewLC(expectedSize);
 	TPtr outputPtr(outputBuffer->Des());

    // load the converter ready for testing
 	CCnvCharacterSetConverter* charConverter=CCnvCharacterSetConverter::NewLC();
    CCnvCharacterSetConverter::TAvailability available = CCnvCharacterSetConverter::EAvailable;
    available = charConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierShiftJis, aFileServerSession);
    if (  available != CCnvCharacterSetConverter::EAvailable)
        User::Leave(KErrNotSupported);

	// read the whole source file into the buffer
	User::LeaveIfError( source.Read( sourcePtr, sourceSize ));
	source.Close();

    // measure the time taken to convert.
    TTime startTime, stopTime;
    startTime.UniversalTime();

 	// now convert using J5 into unicode in the output buffer
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TInt numberCharsNotConverted = 0;

    // repeat this to get a more accurate measurement
    const int scale =10;
    TInt i=0;
	for ( i=0; i<scale; i++)
		{
		numberCharsNotConverted = charConverter->ConvertToUnicode(outputPtr, sourcePtr, state);
		if ( numberCharsNotConverted != 0)
			INFO_PRINTF2(_L("number of characters not converted:%d \n"),
				numberCharsNotConverted );
		}

    stopTime.UniversalTime();
    TTimeIntervalMicroSeconds timeTaken = stopTime.MicroSecondsFrom(startTime);
  	TInt result = timeTaken.Int64()/scale;
   	INFO_PRINTF3(_L("Time taken for converting %S to unicode using shiftjis:%d microseconds\n"),
    	&aForeign, result );


	// characterSetConverter, sourceBuffer, outputBuffer
	CleanupStack::PopAndDestroy(3);
	}

/**
@SYMTestCaseID SYSLIB-CharConv-J5-U-1356-CP
@SYMTestCaseDesc Check some UCS2 conversions
@SYMTestStatus Implemented
@SYMTestPriority Critical
@SYMTestActions The UCS2 sample data is converted to unicode by J5.
@SYMTestExpectedResults The converted unicode data is checked against expected data.
@SYMTestType Unit
@SYMREQ REQ4828
*/
void CT_J5_DOCOMO::checkUCS2converstion(RTest1& test, CCnvCharacterSetConverter *aJ5converter )
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-J5-U-1356-CP "));
	// buffer to convert into
    TBuf16<512> unicode;
	TInt numberOfUnconvertibleCharacters;
	TInt indexOfFirstByteOfFirstUnconvertibleCharacter;

	// small sample of EBigEndian UCS2 with no Byte order markers
	INFO_PRINTF1(_L("Check J5 conversion of  EBigEndian UCS2 with no Byte order markers to unicode \n"));
	_LIT8(Kucs2sample1,"\x0\x69\x0\x2d\x0\x6d\x0\x6f\x0\x64\x0\x65");
	_LIT16(KucsAsUnicode1,"\x0069\x002d\x006d\x006f\x0064\x0065");

 	// J5 convert from unicode
	TInt state = CCnvCharacterSetConverter::KStateDefault;
	TInt numberNotConverted =0;
	numberNotConverted = aJ5converter->ConvertToUnicode(unicode, Kucs2sample1, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter );
	test(numberNotConverted == 0);
	test(numberOfUnconvertibleCharacters == 0);
	test(indexOfFirstByteOfFirstUnconvertibleCharacter == -1);

	// compare the unicode created to that supplied
	test(unicode==KucsAsUnicode1);

	// small sample of ELittleEndian UCS2 with no Byte order markers
	INFO_PRINTF1(_L("Check J5 conversion of  ELittleEndian UCS2 with no Byte order markers to unicode \n"));
	_LIT8(Kucs2sample2,"\x69\x0\x2d\x0\x6d\x0\x6f\x0\x64\x0\x65\x0");
	_LIT16(KucsAsUnicode2,"\x0069\x002d\x006d\x006f\x0064\x0065");
	state = CCnvCharacterSetConverter::KStateDefault;
	numberNotConverted = aJ5converter->ConvertToUnicode(unicode, Kucs2sample2, state);
	test(numberNotConverted == 0);

	// compare the unicode created to that supplied
	test(unicode==KucsAsUnicode2);

	// check UCS2 converter handles output buffer too small case
	INFO_PRINTF1(_L("Check J5 conversion of  UCS2 with small output buffer \n"));
    TBuf16<4> smallOutputBuffer;
	numberNotConverted = aJ5converter->ConvertToUnicode(smallOutputBuffer, Kucs2sample2, state);
	// check numberNotConverted = size of KucsAsUnicode2(12bytes) - smallOutputBuffer(8bytes) = 4
	INFO_PRINTF2(_L("numberNotConverted %d should be 4 \n"), numberNotConverted );
	test( numberNotConverted == 4);

	// check UCS2 converter handles a odd number of input bytes
	INFO_PRINTF1(_L("Check J5 conversion of UCS2 handles a odd number of input bytes \n"));
	_LIT8(Kucs2sample3,"\xff\xfe\x69\x0\x2d\x0\x6d\x0\x6f\x0\x64\x0\x65\x0\x23");
	_LIT16(KucsAsUnicode3,"\x0069\x002d\x006d\x006f\x0064\x0065");
	numberNotConverted = aJ5converter->ConvertToUnicode(unicode, Kucs2sample3, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
	INFO_PRINTF2(_L("numberOfUnconvertibleCharacters %d should be the 1 odd byte at the end\n"), numberOfUnconvertibleCharacters );
	test(unicode==KucsAsUnicode3);
	test(numberNotConverted == 1);
	test(numberOfUnconvertibleCharacters == 0);
	test(indexOfFirstByteOfFirstUnconvertibleCharacter == -1);
	}


/**
 test code starts here...
 */
void CT_J5_DOCOMO::DoE32MainL()
	{
	RTest1 test;
	INFO_PRINTF1(_L("Docomo TJ5 start"));
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());

	// check the J5 converter is available
    checkJ5Available( test, fileServerSession );

	// Convert some sample data to unicode using J5 and check result
	_LIT8(KShiftJis,"\x82\xB1\x82\xF1\x82\xC9\x82\xBF\x82\xCD\x82\xC9\x82\xBF\x82\xCD");
	_LIT16(KUnicode,"\x3053\x3093\x306B\x3061\x306F\x306B\x3061\x306F");
	// test J5 on simple example shift jis test data,
	convertDataUsingJ5(test, fileServerSession, KShiftJis, KUnicode );

	// small sample of code set 1&3 EUC_JP
	_LIT8(KCodeSet2EucJp,"\x8f\xa2\xc2\xa1\xf1\xa1\xf2\x8f\xa2\xc2\x5c\x8f\xa2\xc3\xa1\xf8");
	// should convert to ¡¢£¤¥¦?
	_LIT16(KCodeSet2EucJpAsUnicode,"\x00A1\x00A2\x00A3\x00A1\x00A5\x00A6\x00A7");
	convertDataUsingJ5( test, fileServerSession, KCodeSet2EucJp, KCodeSet2EucJpAsUnicode);

    /* example of code set 3 EUC_JP 8faba98fa9c18fabae8fabb28fabb18fabb48fabb38fabc0 converts to åæçèéêëì*/
    /* Note this could also be decoded as shiftjis */
	_LIT8(KCodeSet3EucJp,"\x8f\xab\xa9\x8f\xa9\xc1\x8f\xab\xae\x8f\xab\xb2\x8f\xab\xb1\x8f\xab\xb4\x8f\xab\xb3\x8f\xab\xc0");
	_LIT16(KCodeSet3EucJpAsUnicode,"\x00E5\x00E6\x00E7\x00E8\x00E9\x00EA\x00EB\x00EC");
	convertDataUsingJ5( test, fileServerSession, KCodeSet3EucJp, KCodeSet3EucJpAsUnicode);

	// small sample of UTF8 ®¯°±²³
	_LIT8(Kutf8,"\xc2\x8e\xc2\xaf\xc2\xb0\xc2\xb1\xc2\xb2\xc2\xb3");
	_LIT16(Kutf8AsUnicode,"\x008E\x00AF\x00B0\x00B1\x00B2\x00B3");
	convertDataUsingJ5( test, fileServerSession, Kutf8, Kutf8AsUnicode);

	// small sample of ISO2022jp
	_LIT8(Kiso2022jp,"\x1B\x24\x42\x24\x33\x24\x73\x24");
	_LIT16(Kiso2022jpAsUnicode,"\x3053\x3093");
	convertDataUsingJ5( test, fileServerSession, Kiso2022jp, Kiso2022jpAsUnicode);

	// some shiftjis which starts with two bytes that could be EUC 0xe1,0x81
	_LIT8(KShiftJisWithEucStart,"\xE1\x81\x82\xB1\x82\xF1\x82\xC9\x82\xBF\x82\xCD\x82\xC9\x82\xBF\x82\xCD");
	_LIT16(KUnicodeShiftJisWithEucStart,"\x7601\x3053\x3093\x306B\x3061\x306F\x306B\x3061\x306F");
	convertDataUsingJ5(test, fileServerSession, KShiftJisWithEucStart, KUnicodeShiftJisWithEucStart );

	// some shiftjis which starts with two bytes that could be UTF8 0xc2,0x8e
	_LIT8(KShiftJisWithUtf8Start,"\xC2\x8E\x82\xB1\x82\xF1\x82\xC9\x82\xBF\x82\xCD\x82\xC9\x82\xBF\x82\xCD");
	_LIT16(KUnicodeShiftJisWithUtf8Start,"\xFF82\x7345\xFF71\x3093\x306B\x3061\x306F\x306B\x3061\x306F");
	convertDataUsingJ5(test, fileServerSession, KShiftJisWithUtf8Start, KUnicodeShiftJisWithUtf8Start );

	//	Small shiftjis sample with some half width Katakana
	_LIT8(KShiftJisHalfWidthKatakana,"\xB0\xE0\x70\xB0\x70");
	_LIT16(KUnicodeShiftJisHalfWidthKatakana,"\xFF70\x703E\xFF70\x0070");
	convertDataUsingJ5(test, fileServerSession, KShiftJisHalfWidthKatakana, KUnicodeShiftJisHalfWidthKatakana );

	/* Read in a text file and convert to unicode using J5
       then check the correct converstion was used by checking
       returned state variable */
	//	Note ansi.txt is valid for several conversions EUtf8, Eucjp
	checkDetection( test, fileServerSession, KAnsiTxt, EEucjp);
	checkDetection( test, fileServerSession, KImodeTxt, EShiftjis);
	checkDetection( test, fileServerSession, KJpEUCjTxt, EEucjp);
	checkDetection( test, fileServerSession, KJpJISTxt, EIso2022jp1);
	checkDetection( test, fileServerSession, KJpSJISTxt, EShiftjis);
	checkDetection( test, fileServerSession, KJpUnicodeTxt, EUcs2);
	checkDetection( test, fileServerSession, KJpUTF8Txt, EUtf8);
	checkDetection( test, fileServerSession, KmoperaTxt, EShiftjis);
	checkDetection( test, fileServerSession, KMScodeFontTxt,EShiftjis );
	checkDetection( test, fileServerSession, KSJISTxt, EShiftjis );

    // load the converter ready for testing
 	CCnvCharacterSetConverter* J5Converter=CCnvCharacterSetConverter::NewLC();
    CCnvCharacterSetConverter::TAvailability available = CCnvCharacterSetConverter::EAvailable;
    available = J5Converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierJ5, fileServerSession);
    if (  available != CCnvCharacterSetConverter::EAvailable)
        User::Leave(KErrNotSupported);

	// check some UCS2 examples
	checkUCS2converstion( test, J5Converter );

	// Convert the file Ansi.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KAnsiTxt, KAnsiUni, KAnsiOut );

	// Convert the file imode.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KImodeTxt, KImodeUni, KImodeOut );

	// Convert the file JpEUC-J.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KJpEUCjTxt, KJpEUCjUni, KJpEUCjOut );

	// Convert the file JpJIS.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KJpJISTxt, KJpJISUni, KJpJISOut);

	// Convert the file JpSJIS.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KJpSJISTxt, KJpSJISUni, KJpSJISOut );

	// Convert the file JpUnicode.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KJpUnicodeTxt, KJpUnicodeUni, KJpUnicodeOut );

	// Convert the file JpUTF8.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KJpUTF8Txt, KJpUTF8Uni, KJpUTF8Out);

	// Convert the file mopera.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KmoperaTxt, KmoperaUni, KmoperaOut);

	// Convert the file MScodefont.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KMScodeFontTxt, KMScodeFontUni, KMScodeFontOut);

	// Convert the file S-JIS.test supplied by Symbian Japan to unicode
	convertFileUsingJ5L( test, fileServerSession, J5Converter, KSJISTxt, KSJISUni, KSJISOut);


	// Check that J5 converts to UTF8 when ConvertFromUnicode() is called
	convertFromUnicodeUsingJ5( test, fileServerSession, J5Converter );

	// autodetect speed benchmark test
	checkNormalAutodetectL( test, fileServerSession, KMScodeFontTxt );
	checkNormalAutodetectL( test, fileServerSession, KSJISTxt );
	checkNormalAutodetectL( test, fileServerSession, KImodeTxt );

	// conversion speed benchmark test
	checkConversionSpeedL(test, fileServerSession, KMScodeFontTxt);
	checkConversionSpeedL(test, fileServerSession, KSJISTxt);
	checkConversionSpeedL(test, fileServerSession, KImodeTxt);

	// 	J5Converter
	CleanupStack::PopAndDestroy(1 /*, &J5Converter*/);

	User::After(3000000);
	// test, fileServerSession
	CleanupStack::PopAndDestroy(1);
	}

CT_J5_DOCOMO::CT_J5_DOCOMO()
    {
    SetTestStepName(KTestStep_T_J5_DOCOMO);
    }

TVerdict CT_J5_DOCOMO::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;
    TRAPD(error1, DoE32MainL());    
    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }

