// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "pccenrepimpl.h"

_LIT(KCreExt,".cre");
_LIT(KTxtExt,".txt");
_LIT(KOgnExt,".ogn");
const TInt KExtLength=4;
const TInt KMinReposFileLength=12;

//dummy fail transaction cleanup operation
void CPcRepImpl::FailTransactionCleanupOperation(TAny* /**aRepository*/)
    {
    //do nothing
    }

CPcRepImpl* CPcRepImpl::NewL(TUid aRepositoryUid,const TDesC& aInFileName,const TDesC& aOutFileName,TBool aAutoLoading)
	{
	CPcRepImpl* self=new (ELeave)CPcRepImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aRepositoryUid,aInFileName,aOutFileName,aAutoLoading);
	CleanupStack::Pop();
	return self;
	}

void CPcRepImpl::ConstructL(TUid aRepositoryUid,const TDesC& aInFileName,const TDesC& aOutFileName,TBool aAutoLoading)
	{
	User::LeaveIfError(iFs.Connect());
	iRepository=CHeapRepository::NewL(aRepositoryUid);
	
	TFileName ognFile; // XXXXXXXX.ogn
	TBool isOriginal;
	
	IsOriginalL(aRepositoryUid, aOutFileName, aAutoLoading, ognFile, isOriginal);
	iRepository->SettingsArray().SetIsDefault(isOriginal);
	if (!aAutoLoading)
		{
		//verify file name must be in format of XXXXXXXX.<cre/txt> that is minimum length is 8+1+3=12
		if (aInFileName.Length()<KMinReposFileLength || aOutFileName.Length()<KMinReposFileLength)
			User::Leave(KErrArgument);
		//verify it is in the format of <path>XXXXXXXX.<cre/txt>
		TPtrC inFileName(aInFileName.Right(KMinReposFileLength));
		TPtrC outFileName(aOutFileName.Right(KMinReposFileLength));
		TUint inRepUid;
		TLex parser(inFileName.Left(8));
		TInt ret=parser.Val(inRepUid,EHex);
		if (ret!=KErrNone)
			(ret==KErrNoMemory)?User::Leave(ret):User::Leave(KErrArgument);
		parser.Assign(outFileName.Left(8));
		TUint outRepUid;
		ret=parser.Val(outRepUid,EHex);
		if (ret!=KErrNone)
			(ret==KErrNoMemory)?User::Leave(ret):User::Leave(KErrArgument);
		
		//now finally verify the extension of the output file
		if (aOutFileName.Right(KExtLength).CompareF(KCreExt())!=0)						
			User::Leave(KErrArgument);
		if (aInFileName.Right(KExtLength).CompareF(KTxtExt())==0)
			{
			iRepository->SetUid(TUid::Uid(inRepUid));
			CIniFileIn* iniFile=NULL;
			ret=CIniFileIn::NewLC(iFs,iniFile,aInFileName);
			User::LeaveIfError(ret);
			iRepository->ReloadContentL(*iniFile);
			CleanupStack::PopAndDestroy();		
			}
		else if (aInFileName.Right(KExtLength).CompareF(KCreExt())==0)
			{
			iRepository->SetUid(TUid::Uid(inRepUid));
			iRepository->CreateRepositoryFromCreFileL(iFs,aInFileName);
			}
		else
			User::Leave(KErrArgument);
		iOutFileName=aOutFileName.AllocL();
		}
	else
		{
		//auto mode look for CRE first then TXT
		TFileName crefile;
		crefile.AppendNumFixedWidth(aRepositoryUid.iUid,EHex,8);
		crefile.Append(KCreExt());
		TInt ret=KErrNone;
		TRAP(ret,iRepository->CreateRepositoryFromCreFileL(iFs,crefile));
		if (ret!=KErrNone)		
			{
			if (ret!=KErrNotFound)
				User::Leave(ret);
			//look for TXT now
			TFileName file;
			file.AppendNumFixedWidth(aRepositoryUid.iUid,EHex,8);
			file.Append(KTxtExt());
			CIniFileIn* iniFile=NULL;
			ret=CIniFileIn::NewLC(iFs,iniFile,file);
			User::LeaveIfError(ret);
			iRepository->ReloadContentL(*iniFile);
			CleanupStack::PopAndDestroy();
			}
		//output is always cre	
		iOutFileName=crefile.AllocL();
		}
	GetSingleMetaArrayL(iSingleMetaArray);
	if (isOriginal)
		{
		// An empty file is generated in the name of <Repository Uid>.ogn when
		// the repository is loaded for the first time. see IsOriginalL().
		RFile file;
		User::LeaveIfError(file.Create(iFs, ognFile, EFileWrite));
		file.Close();
		}
	iInitialised=ETrue;
	}

CPcRepImpl::~CPcRepImpl()
	{
	/** persist to cre on destructor */	
	if (iRepository && iInitialised)
		Flush();
	
	iFs.Close();
	iSingleMetaArray.Close();
	delete iRepository;
	delete iOutFileName;
	}
	
TInt CPcRepImpl::Flush()
	{
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	return iRepository->CommitChanges(iFs,KPersistFormatSupportsIndMetaIndicator,*iOutFileName);
#else	
	return iRepository->CommitChanges(iFs,KPersistFormatVersion,*iOutFileName);
#endif		
	}

void CPcRepImpl::MoveL(TUint32 aSourcePartialKey, TUint32 aTargetPartialKey,TUint32 aMask, TUint32& aErrorKey)
	{
	RSettingPointerArray sourceSettings;
	CleanupClosePushL(sourceSettings);
	TInt error=iRepository->SettingsArray().Find(aSourcePartialKey & aMask, aMask, sourceSettings);
	
	//dont fail transaction if source settings is empty
	if (error!=KErrNone)
		{
		aErrorKey = aSourcePartialKey;
		User::Leave(error);				
		}
	else if (sourceSettings.Count() == 0)
		{
		aErrorKey = aSourcePartialKey;
		User::Leave(KErrNotFound);				
		}
	
	for (TInt i=0;i<sourceSettings.Count();i++)
		{
		TServerSetting* ts=sourceSettings[i];
		TUint32 sourceKey = ts->Key();
		TUint32 targetKey = sourceKey ^ ((aSourcePartialKey & aMask) ^ (aTargetPartialKey & aMask));
		TServerSetting* targetSetting = GetSetting(targetKey);
		if (targetSetting && !targetSetting->IsDeleted())
			{
			aErrorKey=targetKey;
			User::Leave(KErrAlreadyExists);
			}
		}
			
	TRAPD(err,error = MOperationLogic::MoveL(aSourcePartialKey,aTargetPartialKey,aMask,aErrorKey, sourceSettings));

	//the Move operation logic only mark it as deleted, now remove it from the settings list	
	RemoveAnyMarkDeleted();

	User::LeaveIfError(err);
	User::LeaveIfError(error);

	CleanupStack::PopAndDestroy(&sourceSettings);
	}
	
void CPcRepImpl::DeleteRangeL(TUint32 aPartialKey, TUint32 aMask,TUint32& aErrorKey)
	{
	RSettingPointerArray sourceSettings;
	CleanupClosePushL(sourceSettings);
	TInt error=FindSettings(aPartialKey & aMask, aMask, sourceSettings);
	if (error==KErrNotFound)
		{
		aErrorKey=aPartialKey;
		}
	User::LeaveIfError(error);
	DeleteSettingsRangeL(sourceSettings,aPartialKey,aErrorKey);
	RemoveAnyMarkDeleted();
	CleanupStack::PopAndDestroy(&sourceSettings);		
	}

void CPcRepImpl::GetSingleMetaArrayL(RSingleMetaArray& aMetaArray)
	{	
	TInt numSettings = iRepository->SettingsArray().Count();
	aMetaArray.Reset();
	aMetaArray.ReserveL(numSettings);
	for (TInt i = 0; i < numSettings; i++)
		{
		TUint32 key = iRepository->SettingsArray()[i].Key();
		TUint32 meta = iRepository->SettingsArray()[i].Meta();
		TSettingSingleMeta metaItem(key, meta);
		aMetaArray.AppendL(metaItem);
		}
	}

// This function is used to identify wether the repository is loaded for the first
// time or not. The purpose of this function is to determine whether entries of a 
// repository should be set clean during initializing process. At symbian side, this 
// flag is only set when load from ROM, but at PC side, there is no ROM, so the clean 
// flag is set when load for the first time to keep consistency with Symbian side Library. 
void CPcRepImpl::IsOriginalL(TUid aUid, const TDesC& aOutFile, TBool aAutoLoading, TFileName& aOgnFileName, TBool& aIsOriginal)
	{
	if (!aAutoLoading)
		{
		TInt len = aOutFile.Length();
		aOgnFileName = aOutFile.Left(len - KExtLength);
		}
	else 
		{
		aOgnFileName.AppendNumFixedWidth(aUid.iUid,EHex,8);
		}
	aOgnFileName.Append(KOgnExt());
	
	RFile file;
	TInt err = file.Open(iFs,aOgnFileName,EFileRead|EFileShareReadersOnly);
	file.Close();
	if (err != KErrNone)
		{
		if (err == KErrNotFound || err == KErrPathNotFound)
			{
			aIsOriginal = ETrue;
			return;
			}
		else
			User::Leave(err);
		}
	aIsOriginal = EFalse;
	}
