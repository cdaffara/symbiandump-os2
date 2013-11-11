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
* testagentconsumer.cpp
* implements some of OMA DRM for testing purposes
*
*/



#include <caf/caf.h>
#include "testagentdata.h"
#include "testagentattributes.h"
#include "TestAgentFile.h"
#include "TestAgentDrmContent.h"

using namespace ContentAccess;

CTestAgentData* CTestAgentData::NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CTestAgentData* self = NewLC(aVirtualPath, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentData* CTestAgentData::NewLC(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CTestAgentData* self=new(ELeave) CTestAgentData();
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aShareMode);
	return self;
	}

CTestAgentData* CTestAgentData::NewL(RFile& aFile, const TDesC& aUniqueId)
	{	
	CTestAgentData* self = NewLC(aFile, aUniqueId);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentData* CTestAgentData::NewLC(RFile& aFile, const TDesC& aUniqueId)
	{
	CTestAgentData* self=new(ELeave) CTestAgentData();
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId);
	return self;
	}

CTestAgentData::CTestAgentData()
	{
	}

CTestAgentData::~CTestAgentData()
	{
	delete iVirtualPath;
	delete iTestFileObject;

	iFile.Close();
	iFs.Close();
	}
  
void CTestAgentData::ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	// It is at this point that the agent can open the target content
	// and cache the size. Evaluation of intent will be left until the
	// CData object is created.

	// default share mode of EFileShareReadersOnly
	TUint mode = EFileStream | EFileRead;

	if(aShareMode == EContentShareReadWrite)
		{
		mode |= EFileShareReadersOrWriters;
		}
	else if (aShareMode == EContentShareExclusive)
		{
		mode |= EFileShareExclusive;
		}
	else
		{
		mode |= EFileShareReadersOnly;
		}

	TFileName dummyFileName;
	TTestAgentAttributes::GetDummyFileName(aVirtualPath.URI(), dummyFileName);
	
	// open the file with the correct mode
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareAuto());
	User::LeaveIfError(iFile.Open(iFs, dummyFileName, mode));

	// remember the location of the file
	iVirtualPath = CVirtualPath::NewL(dummyFileName, aVirtualPath.UniqueId());

	// Parse the structure of the DCF file if asked to open default content
	if(aVirtualPath.UniqueId().Length() == 0)
		{
		iTestFileObject = CTestAgentFile::NewL(iFile);
		}
	else
		{
		iTestFileObject = CTestAgentDrmContent::NewL(iFile);
		}

	// seek to start of data in the file, ready to "read"
	TInt Pos = 0;
	Seek(ESeekStart, Pos);
	}


void CTestAgentData::ConstructL(RFile& aFile, const TDesC& aUniqueId)
	{
	User::LeaveIfError(iFile.Duplicate(aFile));

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareAuto());	
	
	// only parse the drm content if requested to open it
	if(aUniqueId.Length() == 0)
		{
		iTestFileObject = CTestAgentFile::NewL(iFile);
		}
	else
		{
		iTestFileObject = CTestAgentDrmContent::NewL(iFile);
		}
		

	// seek to start of data in the file, ready to "read"
	TInt Pos = 0;
	Seek(ESeekStart, Pos);
	}

void CTestAgentData::DataSizeL(TInt &aSize)
	{
	iTestFileObject->DataSizeL(aSize);
	}

TInt CTestAgentData::EvaluateIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TInt CTestAgentData::ExecuteIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TInt CTestAgentData::Read(TDes8& aDes) 
	{
	return iTestFileObject->Read(aDes,aDes.MaxLength());
	}

TInt CTestAgentData::Read(TDes8& aDes,TInt aLength) 
	{
	return iTestFileObject->Read(aDes,aLength);
	}

void CTestAgentData::Read(TDes8& aDes,TRequestStatus& aStatus) 
	{
	iTestFileObject->Read(aDes, aDes.MaxLength(), aStatus);
	}

void CTestAgentData::Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus) 
	{
	iTestFileObject->Read(aDes, aLength, aStatus);
	}

TInt CTestAgentData::Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus) 
	{
	iTestFileObject->Read(aPos, aDes, aLength, aStatus);
	return KErrNone;
	}
	
void CTestAgentData::ReadCancel(TRequestStatus& aStatus)
{
	iTestFileObject->ReadCancel(aStatus);
}		
	
TInt CTestAgentData::Seek(TSeek aMode, TInt& aPos) 
	{
	return iTestFileObject->Seek(aMode, aPos);
	}

TInt CTestAgentData::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrCANotSupported;
	}

TInt CTestAgentData::GetAttribute(TInt aAttribute, TInt& aValue)
	{
	return iTestFileObject->GetAttribute(aAttribute, aValue);
	}

TInt CTestAgentData::GetAttributeSet(RAttributeSet& aAttributeSet)
	{
	return TTestAgentAttributes::GetAttributeSet(*iTestFileObject, aAttributeSet);
	}

TInt CTestAgentData::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
	return iTestFileObject->GetStringAttribute(aAttribute, aValue);
	}

TInt CTestAgentData::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet)
	{
	return TTestAgentAttributes::GetStringAttributeSet(*iTestFileObject, aStringAttributeSet);
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CTestAgentData::Read64(TInt64 /* aPos */, TDes8& /* aDes */, TInt /* aLength */, TRequestStatus& /* aStatus */)
	{
	return KErrCANotSupported;
	}

void CTestAgentData::DataSize64L(TInt64& /*aSize*/)
	{
	User::Leave(KErrCANotSupported);
	}

TInt CTestAgentData::Seek64(TSeek /*aMode*/, TInt64& /*aPos*/)
	{
	return KErrCANotSupported;
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

