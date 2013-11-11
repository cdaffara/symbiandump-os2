/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : filestream.h
* Part of     : stdio server
* To handle file read and write
* File stream for Stdio Redirection app
* 
*
*/




#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_

#include "streambase.h"


//-----------------------------------------------------------------------
//Class name:  CFileStream 
//Description: This class represents the File stream abstraction.
// 			   It is created through the factory class CFileStreamFactory
//-----------------------------------------------------------------------
class CFileStream : public CStreamBase
	{
public:
	static CFileStream* NewL(TDesC& aFileName, TBool aIsWrite);
	~CFileStream();
 	virtual void WriteL(TDes8& aDes);
    virtual void Read(TRequestStatus& aStatus, const TInt aLength);
	virtual void ReadCancel() {}
   	TStreamType Type() { return EFile;}
   	
private:
	void ConstructL(TDesC& aFileName);
	CFileStream(TBool aIsWrite);
  	RFile iFile;
	TInt iFileErr;
	//if file is used for write.
	TBool iIsWrite;
	TFileName iFileName;
	};


#endif //_FILESTREAM_H_
