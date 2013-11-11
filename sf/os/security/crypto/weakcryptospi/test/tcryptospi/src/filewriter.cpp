/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @internalTechnology
*/
#include "filewriter.h"

#include "cryptospi/cryptospidef.h"
#include <e32std.h>
#include <e32def.h>
#include <f32file.h>

// a simple utility class to write to a text file given the path in its 
// NewLC / NewL 

CFileWriter* CFileWriter::NewL(TPtrC aFilePath)
	{
	CFileWriter* self=CFileWriter::NewLC(aFilePath);
	CleanupStack::Pop(self);
	return self;				
	}

CFileWriter* CFileWriter::NewLC(TPtrC aFilePath)
	{
	CFileWriter* self=new(ELeave) CFileWriter();
	CleanupStack::PushL(self);
	self->ConstructL(aFilePath);
	return self;				
	}

	
CFileWriter::~CFileWriter()
	{
	// nulled in constructor so safe to call
	delete iFilePath;
	
	iFile.Close();
	iRfs.Close();
	}
	
CFileWriter::CFileWriter() : iFilePath(0)
	{
	}

void CFileWriter::WriteBlockL(TPtrC8 aData)
	{
	iFile.Write(aData);
	}
	
	
void CFileWriter::CreateFileL()
{
	CleanupClosePushL(iRfs);
	
	iRfs.Delete(iFilePath->Des());

	User::LeaveIfError(iFile.Create(iRfs, iFilePath->Des(), EFileWrite));

	CleanupStack::Pop(&iRfs);
}	
	
void CFileWriter::ConstructL(TPtrC aFilePath)	
	{
	iFilePath = HBufC::NewL(aFilePath.Length());
	
	iFilePath->Des().Copy(aFilePath);

	User::LeaveIfError(iRfs.Connect());
	
	CreateFileL();
	}
