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
// Implementation of XOP serializer Output Stream
//

#include <bafl/sysutil.h>
#include "xmlengxopfileoutputstream.h"

TXmlEngSXOPFileOutputStream::TXmlEngSXOPFileOutputStream(RFile& aFile,RFs& aRFs)
	{
	TDriveInfo drInfo;
	iError = aFile.Drive(iDrive, drInfo);			
	iFile = aFile;
	iRFs = aRFs;
	}

TInt TXmlEngSXOPFileOutputStream::Write(const TDesC8 &aBuffer)
	{
	TRAP(iError,WriteL(aBuffer));
	if(iError)
	    {
    	return -1;
	    }
	return aBuffer.Size();
	}
	
TInt TXmlEngSXOPFileOutputStream::Close()
	{
	return KErrNone;
	}
   
TInt TXmlEngSXOPFileOutputStream::CloseAll()
	{
	iFile.Close();
	return KErrNone;
	}
	
TInt TXmlEngSXOPFileOutputStream::CheckError()
    {
    return iError;
    }

void TXmlEngSXOPFileOutputStream::WriteL(const TDesC8 &aBuffer)
	{
	if(SysUtil::DiskSpaceBelowCriticalLevelL(&iRFs, aBuffer.Size(), iDrive))
		{
		User::Leave(KErrDiskFull);
		}
	User::LeaveIfError(iFile.Write(aBuffer));
	}
