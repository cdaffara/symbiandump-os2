// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : filestream.cpp
// Part of     : stdio server
// To handle file read and write
// 
//

#include "StdioServer.h"
#include "filestream.h"


//
// -------------- FileStream implementation ------------------------
//

//-------------------------------------------------------------------------------
// Function Name :  CFileStream::NewL()
// Description   :  Implements two phase construction. 
//-------------------------------------------------------------------------------	


CFileStream* CFileStream::NewL(TDesC& aFileName, TBool aIsWrite) 
	{
	CFileStream* r = new(ELeave) CFileStream(aIsWrite);
	CleanupStack::PushL(r);
	r->ConstructL(aFileName);
	CleanupStack::Pop();
	return r;
	}


//-------------------------------------------------------------------------------
// Function Name :  CFileStream::CFileStream()
// Description   :  Default Constructor.
//-------------------------------------------------------------------------------	

CFileStream::CFileStream(TBool aIsWrite) : iIsWrite(aIsWrite) {}

//-------------------------------------------------------------------------------
// Function Name :  CFileStream::~CFileStream()
// Description   :  Destructor. Closes the RFileReadStream and RFileWriteStream
//					Commits data to the stream before freeing resources used by 
//					the stream. This ensures that any buffered data is written 
//					to the stream.
//-------------------------------------------------------------------------------	

CFileStream::~CFileStream() {
	iFile.Close();
}


//-------------------------------------------------------------------------------
// Function Name :  CFileStream::ConstructL()
// Description   :  second phase construction.
//-------------------------------------------------------------------------------	

void CFileStream::ConstructL(TDesC& aFileName) 
	{
	
	if ( iIsWrite )
		{
		CStdioServer::FsSession().MkDirAll(aFileName);
		iFileErr = iFile.Replace(CStdioServer::FsSession(), aFileName, EFileShareAny | EFileWrite);
		User::LeaveIfError(iFileErr);	
		}
	else
		{
		iFileErr = iFile.Open(CStdioServer::FsSession(), aFileName, EFileShareAny | EFileRead );
		//if file can not opened with ShareOnly mode
		if ( KErrPermissionDenied == iFileErr )
			{
			iFileErr = iFile.Open(CStdioServer::FsSession(), aFileName, EFileShareReadersOnly);
			}
		User::LeaveIfError(iFileErr);	
		iFileName.Copy(aFileName);
		}
	}


//-------------------------------------------------------------------------------
// Function Name :  CFileStream::WriteL()
// Description   :  Print the descriptor onto the File. Convert to unicode first
//					then write to the file and flush the stream buffer
//-------------------------------------------------------------------------------	

void CFileStream::WriteL(TDes8& aDes) 
	{
	TInt err = iFile.Write(aDes);
	if (err != KErrNone) 
		{
		User::InfoPrint(_L("flush error"));
		User::Leave(err);
		}
	}

//-------------------------------------------------------------------------------
// Function Name :  CFileStream::Read()
// Description   :  Handles a read request from a client session.
//-------------------------------------------------------------------------------	
	
void CFileStream::Read(TRequestStatus& aStatus, const TInt aLength) 
	{
	iBuf.Zero();
	//if the requested buffer length is greater then 256 use 256
	//otherwise use requested buffer length
	TInt len = iBuf.MaxLength() < aLength ? iBuf.MaxLength() : aLength;
	TInt err =  iFile.Read(iBuf, len);

	if ( !iBuf.Length() )	
		{
		err = -1;
		TInt size;
		iFile.Size(size);
		iFile.Close();
		err = iFile.Open(CStdioServer::FsSession(), iFileName, EFileShareAny | EFileRead );
		//if file can not opened with ShareOnly mode
		if ( KErrPermissionDenied == err )
			{
			err = iFile.Open(CStdioServer::FsSession(), iFileName, EFileShareReadersOnly);
			}
	
		if ( KErrNone == err)
			{
			err = iFile.Seek(ESeekStart, size);	
			if ( KErrNone == err)
				{
				err =  iFile.Read(iBuf, len);
				}
			}
		}
		
	//with err code returned EErrEof there can be data in
	//iBuf that is last few bytes, this time we need to say 
	//msg Complete	and recording this len of buf in iPreLen
	//so that on next call to read we can compare this and 
	// see if the same data is read again with EErrEof of
	//this is new data.
	TRequestStatus* status = &aStatus;
	if( !iBuf.Length() && KErrNone == err )
		{
		// file has reached end of file
		err = KErrEof;
		}
	// complete the read request by sending the buffer
    // (or part of it) to the client
   	User::RequestComplete(status, err);
	return;
	}
		
 



