// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Text node functions
//

#include "TestFileOutputStream.h"

TInt CTestFileOutputStream::Write(const TDesC8 &aBuffer)
	{
	TRAP(iError,WriteL(aBuffer));
	if(iError)
	    {
    	return -1;
	    }
	return aBuffer.Size();
	};
	
TInt CTestFileOutputStream::Close()
	{
	return KErrNone;
	}
	
TInt CTestFileOutputStream::CheckError()
    {
    return iError;
    }
	
CTestFileOutputStream::CTestFileOutputStream(RFile& aFile,RFs& aRFs)
	{
	iError = KErrNone;
	iFile = aFile;
	iRFs = aRFs;
	}
   
void CTestFileOutputStream::WriteL(const TDesC8 &aBuffer)
	{
	TInt drive;
	TDriveInfo drInfo;
	User::LeaveIfError(iFile.Drive(drive,drInfo));
	User::LeaveIfError(iFile.Write(aBuffer));
	}
 
