/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Mostly obsolete code to manage hash tests
*
*/


/**
 @file
*/

#include "hashtestutils.h"


CTestData* CTestData::NewL(const TDesC& aFilename)

	{
	CTestData* self;
	self=new (ELeave) CTestData;
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	CleanupStack::Pop();		// self
	return self;
	}

CTestData::CTestData(void):CBase()

	{
	}

CTestData::~CTestData(void)

	{
	delete iFile;
	delete iLine;
	}

void CTestData::ConstructL(const TDesC& aFilename)

	{
	RFs fs;
	RFile file;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	TDriveUnit sysDrive(fs.GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<24> filePath (driveName);
	filePath.Append(_L("\\thash\\"));
	User::LeaveIfError(fs.SetSessionPath(filePath));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs,aFilename,EFileShareAny|EFileRead));
	// read into iFile
	TInt size=0;
	file.Size(size);
	iFile=HBufC8::NewMaxL(size);
	TPtr8 ptr=iFile->Des();
	User::LeaveIfError(file.Read(ptr));
	CleanupStack::PopAndDestroy(2, &fs);
	iCurrentPlace=0;
	iLine=HBufC8::NewMaxL(2000);
	}
	
CTestData::TType CTestData::Type(void)

	{
	TPtr8 ptr=iLine->Des();
	if (iCurrentPlace>=iFile->Length())
		{
		return EFinished;
		}
	TInt8 ch=(*iFile)[iCurrentPlace++];
	ptr.SetLength(0);
	while ((ch!='\r')&&(ch!='\n')&&(iCurrentPlace<=iFile->Length()))
		{
		ptr.Append(ch);
		ch=(*iFile)[iCurrentPlace++];
		}
	if ((iCurrentPlace==iFile->Length())||(iLine->Length()==0))
		{
		return EFinished;
		}
	if (((*iFile)[iCurrentPlace]=='\r')||((*iFile)[iCurrentPlace]=='\n'))
		{
		iCurrentPlace++;
		}
	if ((*iLine)[0]=='+')
		{
		switch ((*iLine)[1])
			{
		case 'M':				// Message
			return EMessage;
		case 'F':				// File Name
			return EFileName;
		case 'C':				// Comment
			return Type();
		default:
			return EError;
			}
		}
	return EData;
	}

HBufC8* CTestData::operator [] (TInt aIndex)

	{
	TInt i;
	TInt count=0;
	HBufC8* ret;
	ret=HBufC8::New(4000);
	for (i=0;(i<iLine->Length())&&(count<aIndex);i++)
		{
		if ((*iLine)[i]==' ')
			{
			count++;
			}
		}
	if(ret != NULL)
        	{
	   	TPtr8 ptr=ret->Des();
	   	for (;(i<iLine->Length()&&(*iLine)[i]!=' ');i++)
			{
			ptr.Append((*iLine)[i]);
			}
	   	}
	return ret;
	}

HBufC* CTestData::Message(void)

	{
	HBufC* ret=HBufC::NewMax(iLine->Length()-3);
	TPtrC8 ptr=iLine->Right(iLine->Length()-3);
	if(ret != NULL)
		{
		TPtr dest=ret->Des();
		dest.Copy(ptr);
		}
	return ret;
	}


CTestConsole* CTestConsole::NewL(CConsoleBase* aCon)
	{
	CTestConsole* self;
	self=new (ELeave) CTestConsole;
	self->iCon=aCon;
	self->iFile=NULL;
	return self;
	}

CTestConsole::CTestConsole(void):CConsoleBase()

	{
	}

CTestConsole::~CTestConsole(void)

	{
	delete iCon;
	if (iFile)
		{
		iFile->Close();
		}
	}

void CTestConsole::Write(const TDesC16& aString)

	{
	iCon->Write(aString);
	if (iFile)
		{
		TUint8 space[200];
		TPtr8 ptr(space,200);
		ptr.Copy(aString);
		iFile->Write(ptr);
		}
	}

void CTestConsole::SetLogFile(RFile* aFile)

	{
	iFile=aFile;
	}
