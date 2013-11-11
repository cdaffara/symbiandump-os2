/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Needs the following libraries in the MMP:
* euser.lib etext.lib fbscli.lib bitgdi.lib form.lib EFSRV.LIB imageconversion.lib ezip.lib
*
*/



#include <e32base.h>
#include <txtfrmat.h>
#include <txtrich.h>
#include <frmtview.h>
#include <imageconversion.h>
#include <zipfile.h>

// Complete richtext document class that uses a file-based
// bitmap instead of a screen device allowing the creation,
// saving and comparing of what would normally be screen-shots
class CTestBitmapFile : public CActive
	{
public:
	// Construction
	// Optionally specify screen size and font/ fontsize
	IMPORT_C static CTestBitmapFile* NewLC(const TRect& aDisplayRect = TRect(0, 0, 252, 94),
		TCharFormat aCharFormat = TCharFormat(_L("NewTimes"),300));
	IMPORT_C ~CTestBitmapFile();

	// Load/save operations
	IMPORT_C TBool CompareL(const TDes8& aData);
	IMPORT_C void SaveFileL(const TDesC& aFileName);
    
	// Gets
	inline CRichText* DocModel() {return iDocModel;}
	inline CTextView* View() {return iView;}
	inline CTextLayout* Layout() {return iLayout;}
	
	IMPORT_C void AppendL(const TDesC&);
	IMPORT_C void AppendL(CPicture* aPic);

private:
	// Construction
	CTestBitmapFile();
	void ConstructL();
    
	// Destruction
	void Delete(void*);

	// Active object interface
	void RunL(); 
	void DoCancel();

private:
	CFbsBitmap* iBitmap; 
	CImageEncoder* iSaveUtil;
	RFs iFs;
	TRect iDisplayRect;
	CFbsBitmapDevice* iDevice;
	CParaFormat* iParaFormat;
 	TParaFormatMask iParaFormatMask;
	CParaFormatLayer* iParaFormatLayer;
	TCharFormat iCharFormat;
	TCharFormatMask iCharFormatMask;
	CCharFormatLayer* iCharFormatLayer;
	CRichText* iDocModel;
	CTextLayout* iLayout;
	CTextView* iView;
	};


//Extracts zipped bitmap files into descriptors for convenient comparison
class CTestBitmapZipFileExtractor : public CBase
	{
public:
    // Construction
    IMPORT_C static CTestBitmapZipFileExtractor* NewLC(const TDesC& aZipFile);
    IMPORT_C ~CTestBitmapZipFileExtractor();
	// Bitmap file extractor and descriptor converter
    IMPORT_C TPtr8 BitmapFileL(const TDesC& aFilename);

private:
    // Construction
    CTestBitmapZipFileExtractor();
    void ConstructL(const TDesC& aZipFile);

private:
	RFs iFs;
	// Zip file object
	CZipFile* iZipFile;
	//temporary buffer for descriptor conversion
	HBufC8* iBuf;
	};
