/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* dcffile.cpp
*
*/


#include <caf/caf.h>
#include "TestAgentFile.h"

using namespace ContentAccess;

_LIT8(KDrmMimeType, "application/testagent.drm");

CTestAgentFile* CTestAgentFile::NewL(RFile& aFile)
	{
	CTestAgentFile *self = new (ELeave) CTestAgentFile(aFile);
	return self;
	}

CTestAgentFile::CTestAgentFile(RFile& aFile) : iFile(aFile)
	{
	}

CTestAgentFile::~CTestAgentFile()
	{	
	}	

TInt CTestAgentFile::Read(TDes8& aDes,TInt aLength)
	{
	return iFile.Read(aDes, aLength);
	}

void CTestAgentFile::Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	iFile.Read(aDes, aLength, aStatus);
	}
	
void CTestAgentFile::ReadCancel(TRequestStatus& aStatus)
{
	iFile.ReadCancel(aStatus);
}

TInt CTestAgentFile::Seek(TSeek aMode, TInt& aPos)
	{
	return iFile.Seek(aMode, aPos);
	}

TPtrC8 CTestAgentFile::ContentMimeType()
	{
	return KDrmMimeType();
	}

TInt CTestAgentFile::GetAttribute(TInt aAttribute, TInt& aValue)
	{
	TInt err = KErrNone;
	
	switch(aAttribute)
		{
	case EIsProtected:
		aValue = EFalse;
		break;
	case EIsForwardable:
		aValue = ETrue;
		break;
	case EIsModifyable:
		aValue = EFalse;
		break;
	case EIsCopyable:
		aValue = ETrue;
		break;
	case EContentCDataInUse:
		aValue = KErrCANotSupported;
		break;
	case ECopyPaste:
		aValue = ETrue;	
		break;
	case ECanMove:
		aValue = ETrue;
		break;
	case ECanRename:
		aValue = ETrue;
		break;
	case EContentVersion:
	default:
		err = KErrCANotSupported;
		break;
		};
	return err;
	}
	
TInt CTestAgentFile::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
	TInt err = KErrNone;
	
	// the file itself
	switch(aAttribute)
		{
		case EMimeType:
			aValue.Copy(KDrmMimeType());
			break;
		default:
			err = KErrCANotSupported;
			break;
		};
	return err;	
	}
	
void CTestAgentFile::DataSizeL(TInt &aSize)
	{
	iFile.Size(aSize);
	}
	
void CTestAgentFile::Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	iFile.Read(aPos, aDes, aLength, aStatus);
	}	

