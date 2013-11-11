/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains text and glyph data for testing
*
*/


#include <stdlib.h>
#include <stdarg.h>
#include <e32svr.h>
#include <s32file.h>


//class used for holding input and output data for the shaper, for testing purposes.
class CTestData : public CBase
	{
public:
	//ctor
	CTestData(): iTypeFaceName(NULL), iFilename(NULL), iTextInput(NULL), iStart(0), iEnd(0), 
	iGlyphCount(0), iGlyphs(NULL), iIndices(NULL), iPositions(NULL)
			{}

	~CTestData();
	
	//reads in all the data from the test file
	void Internalize(TPtrC16 aTestDataFilename);
	
	//input parameters
	TBuf16<50> iTypeFaceName;
	TBuf16<100> iFilename;
	TDes16* iTextInput;
	TInt iStart;
	TInt iEnd;
		
	//equivalent expected output
	TInt iGlyphCount;
	CArrayFixFlat<TUint32>* iGlyphs;		// note RArray not used for these members as
	CArrayFixFlat<TUint16>* iIndices;		// is aligned to 4byte boundary
	CArrayFixFlat<TUint16>* iPositions;
	TPoint iAdvance;
	TInt iCharacterCount;
	};
	
CTestData::~CTestData(void)
	{
	delete iTextInput;
	delete iGlyphs;
	delete iIndices;
	delete iPositions;
	}
	
void CTestData::Internalize(TPtrC16 aTestDataFilename)
	{
	// open rfs handle
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);	
	
	// open file
	RFile file;
	file.Open(fs, aTestDataFilename, EFileRead);
	
	// set up file buf and read stream
	RFileBuf buf;
	buf.Attach(file);
	CleanupClosePushL(buf);
	RReadStream stream(&buf);	
	CleanupClosePushL(stream);
	
	//read in from file
	TBuf16<1500> tempBuf;
	TInt count;
	TInt i=0;
	TChar delimiter('|');
	
	//FIRST THE INPUT DATA
	
	//number of input chars
	count = stream.ReadInt16L();
	
	//typeface name
	stream.ReadL(iTypeFaceName,delimiter);
	iTypeFaceName.Delete(iTypeFaceName.Length() - 1, 1);

	//filename
	stream.ReadL(iFilename,delimiter);
	iFilename.Delete(iFilename.Length() - 1, 1);

	//start
	iStart = stream.ReadInt16L();
	stream.ReadL(tempBuf,delimiter);

	//end
	iEnd = stream.ReadInt16L();
	stream.ReadL(tempBuf,delimiter);

	//text input
	iTextInput = new(ELeave) TBuf16<1500>;
	stream.ReadL(*iTextInput,count);
	stream.ReadL(tempBuf,delimiter);
			
	//NOW THE OUTPUT DATA	
	
	//firt the glyph count
	iGlyphCount = stream.ReadInt16L();
	
	//then the Glyphs
	iGlyphs = new(ELeave) CArrayFixFlat<TUint32>(1);
	for (i=0; i<iGlyphCount; i++)
		{
			iGlyphs->AppendL(stream.ReadInt32L());
		}
	
	//then the x and y positions
	iPositions = new(ELeave) CArrayFixFlat<TUint16>(1);
	for (i=0; i<iGlyphCount*2; i++)
		{
			iPositions->AppendL(stream.ReadInt16L());
		}

	//then the advance		
	iAdvance.iX = stream.ReadInt16L();
	iAdvance.iY = stream.ReadInt16L();
	
	//and finally the indices
	iIndices = new(ELeave) CArrayFixFlat<TUint16>(1);
	for (i=0; i<iGlyphCount; i++)
		{
			
			iIndices->AppendL(stream.ReadInt16L());
		}

	//and the character count
	iCharacterCount = stream.ReadInt16L();

	CleanupStack::PopAndDestroy(3);	//buf, fs
	}



