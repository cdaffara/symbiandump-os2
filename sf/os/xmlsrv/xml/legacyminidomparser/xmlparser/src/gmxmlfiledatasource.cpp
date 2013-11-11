// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// This file contains the definition of the CMDXMLParserFileDataSource class
// which is responsible for feeding the data source interface from a given XML file.
// The headers for this class exist in the XMLParser.h header file so that the old
// file-based interface can be implemented using the data source API without removing
// source compatibility for older code.
// 
//


#include <f32file.h>
#include <gmxmlparser.h>
#include "GMXMLFileDataSource.h"

CMDXMLParserFileDataSource *CMDXMLParserFileDataSource::NewLC(RFs aRFs, const TDesC& aFileToParse)
	{
	CMDXMLParserFileDataSource* self = new (ELeave) CMDXMLParserFileDataSource();
	CleanupStack::PushL(self);
	self->ConstructL(aRFs, aFileToParse);
	return self;
	}

CMDXMLParserFileDataSource *CMDXMLParserFileDataSource::NewL(RFs aRFs, const TDesC& aFileToParse)
	{
	CMDXMLParserFileDataSource *self = NewLC(aRFs, aFileToParse);
	CleanupStack::Pop();
	return self;
	}
	
CMDXMLParserFileDataSource* CMDXMLParserFileDataSource::NewL(RFile& aFileHandleToParse)
	{
	CMDXMLParserFileDataSource* me = new(ELeave) CMDXMLParserFileDataSource();
	CleanupStack::PushL(me);
	me->ConstructL(aFileHandleToParse);
	CleanupStack::Pop(me);
	return me;
	}

void CMDXMLParserFileDataSource::ConstructL(RFile& aFileHandleToParse)
	{ 
	// take ownership of file handle
	iXMLFile = aFileHandleToParse;
	
	// Calculate size of file
	TInt fileSize;
	User::LeaveIfError(iXMLFile.Size(fileSize));

	// make buffer
	iXMLFileBuffer = HBufC8::NewL(fileSize);
	TPtr8 tmpbuffer(iXMLFileBuffer->Des());

	// read file into buffer
	User::LeaveIfError(iXMLFile.Read(tmpbuffer, fileSize));
	iState = KInit;

	iXMLFile.Close();
	}	


CMDXMLParserFileDataSource::~CMDXMLParserFileDataSource()
	{
	iXMLFile.Close();
	delete iXMLFileBuffer;
	}

CMDXMLParserFileDataSource::CMDXMLParserFileDataSource() : iState(KError), iXMLFileBuffer(0)
	{
	}

void CMDXMLParserFileDataSource::ConstructL(RFs aRFs, const TDesC& aFileToParse)
	{ 
	User::LeaveIfError(iXMLFile.Open(aRFs, aFileToParse, EFileRead | EFileShareReadersOnly));
	
	// Calculate size of file
	TInt fileSize;
	User::LeaveIfError(iXMLFile.Size(fileSize));

	// make buffer
	iXMLFileBuffer = HBufC8::NewL(fileSize);
	TPtr8 tmpbuffer(iXMLFileBuffer->Des());

	// read file into buffer
	User::LeaveIfError(iXMLFile.Read(tmpbuffer, fileSize));
	iState = KInit;

	iXMLFile.Close();

	}

void CMDXMLParserFileDataSource::GetData(TPtrC8 &aPtr, TRequestStatus &aStatus)
	{
	TRequestStatus *s = &aStatus;
	switch(iState)
		{
	case KInit:
		aPtr.Set(iXMLFileBuffer->Des());
		iState = KDataSent;
		User::RequestComplete(s, KMoreData);
		break;

	case KDataSent:
		delete iXMLFileBuffer;
		iXMLFileBuffer = 0;
		iState = KDone;
		User::RequestComplete(s, KDataStreamEnd);
		break;

	case KDone:
		User::RequestComplete(s, KDataStreamEnd);
		break;

	default:
		User::RequestComplete(s, KDataStreamError);
		break;
		};
	}

void CMDXMLParserFileDataSource::Disconnect()
	{
	delete iXMLFileBuffer;
	iXMLFileBuffer = 0;
	iState = KDone;
	}

// End of File
