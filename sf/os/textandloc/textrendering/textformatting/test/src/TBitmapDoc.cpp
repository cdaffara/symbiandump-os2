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
* @file
* @internalComponent 
*
*/


#include "TBitmapDoc.h"

EXPORT_C CTestBitmapFile* CTestBitmapFile::NewLC(const TRect& aDisplayRect, TCharFormat aCharFormat)
    {
    CTestBitmapFile* retVal = new(ELeave)CTestBitmapFile();
    CleanupStack::PushL(retVal);
    retVal->iDisplayRect = aDisplayRect;
    retVal->iCharFormat = aCharFormat;
    retVal->ConstructL();
    return retVal;
    }

CTestBitmapFile::CTestBitmapFile()
:CActive(CActive::EPriorityStandard), iSaveUtil(0), iCharFormat(_L("NewTimes"),300)
    {
    }

void CTestBitmapFile::ConstructL()
	{
	//create bitmap from rectangle
	const TInt twipFactor = 10;
	iBitmap = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(iBitmap);
	iBitmap->Create(iDisplayRect.Size(),EColor16);
	iBitmap->SetSizeInTwips(TSize(iDisplayRect.iBr.iX * twipFactor, iDisplayRect.iBr.iY * twipFactor));

	//create bitmap device from bitmap
	iDevice = CFbsBitmapDevice::NewL(iBitmap);
	CleanupStack::PushL(iDevice);

	//create rich text document model using a test font
	iParaFormat = CParaFormat::NewLC();
	iParaFormatLayer = CParaFormatLayer::NewL(iParaFormat,iParaFormatMask);
	CleanupStack::PushL(iParaFormatLayer);
	iCharFormatMask.SetAll(); //needed otherwise no font gets set
	iCharFormatLayer = CCharFormatLayer::NewL(iCharFormat,iCharFormatMask);
	CleanupStack::PushL(iCharFormatLayer);
	iDocModel = CRichText::NewL(iParaFormatLayer,iCharFormatLayer);
	CleanupStack::PushL(iDocModel);

	//create text layout from document model and rectangle
	iLayout = CTextLayout::NewL(iDocModel, iDisplayRect.Width());
	CleanupStack::PushL(iLayout);
	
	//create text view from layout, rectangle and device
	iView = CTextView::NewL(iLayout, iDisplayRect, iDevice, iDevice, 0, 0, 0);
	CleanupStack::PushL(iView);
	
 	//Initialise text content
	iView->SetDocPosL(0);         //seem to need this otherwise
 	iView->HandleGlobalChangeL(); //not all text is displayed

	//set up file server
	User::LeaveIfError(iFs.Connect());
	CleanupClosePushL(iFs);

	CActiveScheduler::Add(this);

	CleanupStack::Pop(9,iBitmap);
	}

void CTestBitmapFile::Delete(void* aObj)
    {
    if(aObj)
    	{
    	delete aObj;
	    aObj = NULL;
    	}
    }

EXPORT_C CTestBitmapFile::~CTestBitmapFile()
    {
    Delete(iSaveUtil);
    Delete(iView);
    Delete(iLayout);
    Delete(iDocModel);
    Delete(iCharFormatLayer);
    Delete(iParaFormatLayer);
    Delete(iParaFormat);
    Delete(iDevice);
    Delete(iBitmap);
    iFs.Close();
    }

EXPORT_C TBool CTestBitmapFile::CompareL(const TDes8& aData)
    {
    __ASSERT_ALWAYS(!IsActive(),User::Invariant());
	HBufC8* buf = NULL;
    // create a CImageEncoder to create a bitmap descriptor
    if(iSaveUtil)
    	{
    	delete iSaveUtil;
		iSaveUtil = NULL;
    	}
    iSaveUtil = CImageEncoder::DataNewL(buf, CImageEncoder::EOptionNone, KImageTypeBMPUid);
    // start reading the bitmap: RunL called when complete
    iSaveUtil->Convert(&iStatus, *iBitmap);
    SetActive();
    CActiveScheduler::Start();
    TBool ret = aData.CompareC(buf->Des()) == 0;
    if (buf)
    	{
    	delete buf;
    	buf = NULL;
    	}
    return ret;
    }

// This was used to create the bitmap files for comparison - not used in actual testing
EXPORT_C void CTestBitmapFile::SaveFileL(const TDesC& aFileName)
    {
    __ASSERT_ALWAYS(!IsActive(),User::Invariant());
    // create a CImageEncoder to save the bitmap to the specified file in the specified format
    if(iSaveUtil)
    	{
    	delete iSaveUtil;
		iSaveUtil = NULL;
    	}
    iSaveUtil = CImageEncoder::FileNewL(iFs, aFileName, CImageEncoder::EOptionNone, KImageTypeBMPUid);
    // start saving the bitmap: RunL called when complete
    iSaveUtil->Convert(&iStatus, *iBitmap);
    SetActive();
    CActiveScheduler::Start();
    }

EXPORT_C void CTestBitmapFile::AppendL(const TDesC& aString)
	{
	iDocModel->InsertL(iDocModel->DocumentLength(),aString);
	}

EXPORT_C void CTestBitmapFile::AppendL(CPicture* aPic)
	{
	TPictureHeader header;
   	header.iPicture=aPic;
 		
	//CRichText::InsertL (iDocModel) takes ownership of the picture via TPictureHeader
	TInt docLength = iDocModel->DocumentLength();
	iDocModel->InsertL(docLength,header);
 	iView->FormatTextL();
	CFbsBitGc* gc = CFbsBitGc::NewL();
	iDevice->CreateContext(gc);
	iView->DrawL(iDisplayRect, *gc);
	iView->SetDocPosL(docLength+1, EFalse);
 	}

void CTestBitmapFile::RunL()
    {
    CActiveScheduler::Stop();
    }
     
void CTestBitmapFile::DoCancel()
    {
    // Cancel everything possible
    if (iSaveUtil) iSaveUtil->Cancel();
    }

EXPORT_C CTestBitmapZipFileExtractor* CTestBitmapZipFileExtractor::NewLC(const TDesC& aZipFile)
	{
	CTestBitmapZipFileExtractor* retVal = new(ELeave)CTestBitmapZipFileExtractor;
	CleanupStack::PushL(retVal);
	retVal->ConstructL(aZipFile);
	return retVal;
	}
	
void CTestBitmapZipFileExtractor::ConstructL(const TDesC& aZipFile)
	{
	User::LeaveIfError(iFs.Connect());
	iZipFile = CZipFile::NewL(iFs,aZipFile);
	}

CTestBitmapZipFileExtractor::CTestBitmapZipFileExtractor():iBuf(0)
	{
	}

EXPORT_C CTestBitmapZipFileExtractor::~CTestBitmapZipFileExtractor()
    {
    if(iZipFile)
    	{
    	delete iZipFile;
		iZipFile = NULL;
    	}
	if(iBuf)
		{
		delete iBuf;
		iBuf = NULL;
		}
	iFs.Close();
    }
    
EXPORT_C TPtr8 CTestBitmapZipFileExtractor::BitmapFileL(const TDesC& aFilename)
	{
	//Get a pointer to the bitmap data (member) from the zip file
	CZipFileMember* member = iZipFile->MemberL(aFilename);
	CleanupStack::PushL(member);
	User::LeaveIfNull(member);
	//Get the bitmap data as a filestream
	RZipFileMemberReaderStream* fileStream;
	iZipFile->GetInputStreamL(member,fileStream);
	CleanupStack::PushL(fileStream);
	TUint32 usize = member->UncompressedSize();
	if(iBuf)
		{
		delete iBuf;
		iBuf = NULL;
		}
	iBuf = HBufC8::New(usize);
	CleanupStack::PushL(iBuf);
	//Convert the filestream to a descriptor
	TPtr8 ptr = iBuf->Des();
	User::LeaveIfError(fileStream->Read(ptr,usize));
	CleanupStack::Pop(iBuf);
	CleanupStack::PopAndDestroy(2,member);
	return ptr;
	}
