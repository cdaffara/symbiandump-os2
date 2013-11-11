// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "obsrvr_noc.h"
#include "panic.h"
#include "shrepos.h"
#include "srvrepos_noc.h"
#include "log.h"
#include "cachemgr.h"

#define TRAP_UNIFIED(_unifiedLeave, _function)	\
	{ \
	TInt _returnValue = 0; \
	TRAP(_unifiedLeave, _returnValue = _function);	\
	TInt& __rref = _unifiedLeave; \
	__rref = _unifiedLeave | _returnValue; \
	}

CObservable::~CObservable()
	{
	iObservers.Close();
	// cleanup owned objects if there's any left
	iOpenRepositories.ResetAndDestroy();
	iRepositoryInfo.ResetAndDestroy();
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	TInt multiRofsUidCount=iMultiRofsUidList.Count();
	for (TInt i=0;i<multiRofsUidCount;i++)
		{
		iMultiRofsUidList[i].iMountFlagList.Close();
		}
	iMultiRofsUidList.Close();
#endif	
	}

CObservable* CObservable::NewLC()
	{
	CObservable* self = new(ELeave) CObservable;
	CleanupStack::PushL(self);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	self->ConstructL();
#endif	
	return self;
	}
	
void CObservable::AddObserverL(TUid aUid, CServerRepository* aRepositoryPointer)
	{
	TRepositoryObserverInfo info;

	info.iRepositoryUid = aUid;
	info.iRepositoryPointer = aRepositoryPointer;	
	
	TLinearOrder<TRepositoryObserverInfo> observerSortOrder(CObservable::ObserverSortOrder);
	TInt err = iObservers.InsertInOrder(info, observerSortOrder);
	if ((err != KErrNone)&&(err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	}

void CObservable::RemoveObserver(TUid aUid, CServerRepository* aRepositoryPointer, TInt aInMemoryIndex)
	{
	TRepositoryObserverInfo info;

	info.iRepositoryUid = aUid;
	info.iRepositoryPointer = aRepositoryPointer;
	
	TLinearOrder<TRepositoryObserverInfo> observerSortOrder(CObservable::ObserverSortOrder);
	
	TInt i = iObservers.FindInOrder(info, observerSortOrder);
	if (i!=KErrNotFound)
		{
		// we remove the observer we find listening on the repository
		iObservers.Remove(i);
		if (iObservers.SpecificFindInSignedKeyOrder(info, EArrayFindMode_Any)!=KErrNotFound)
			{
			//if there are more 
			__CENTREP_TRACE1("CENTREP: Observer Removed when closing repository %x", aUid);
			}
		else
			{
			// removed the last observer with the given uid 
			// if the repository is in memory, try evicting it
			if (aInMemoryIndex>=0)
				{
				// Check cache size and carry out forced eviction if necessary			
				// Start Eviction if the repository fits in the cache	
				if (TServerResources::iCacheManager->Enabled()&&
					TServerResources::iCacheManager->StartEviction(GetOpenRepository(aInMemoryIndex)))
					{
					// Repository added to the idle list in eviction order
					__CENTREP_TRACE2("CENTREP: Repository Became Idle when closing repository %x size %d", aUid, GetOpenRepository(aInMemoryIndex)->Size());
					}
				else
					{
					__CENTREP_TRACE1("CENTREP: Repository Delete when closing repository %x", aUid);
					// Remove and Delete Open Repository
					delete iOpenRepositories[aInMemoryIndex];
					iOpenRepositories.Remove(aInMemoryIndex);
					}
				}
			// if no clients are connected on this repository, we no longer need the transaction information
			RemoveSharedRepositoryInfo(aUid);
			}
		}
	}

void CObservable::Notify(TUid aUid, TUint32 aVal) const
	{
	TInt i = FindConnectedRepository(aUid);
	if (i != KErrNotFound)
		{
		for(;(i<iObservers.Count())&&(iObservers[i].iRepositoryUid == aUid);i++)
			{
			iObservers[i].iRepositoryPointer->Notifier()->Notify(aVal);				
			}
		}
	}

TInt CObservable::FindConnectedRepository(TUid aUid) const
	{
	TRepositoryObserverInfo info;
	info.iRepositoryUid = aUid;
	
	return iObservers.SpecificFindInSignedKeyOrder(info, EArrayFindMode_First);
	}

#ifdef CACHE_OOM_TESTABILITY
void CObservable::Reset()
	{
	iObservers.Reset();
	iOpenRepositories.Reset();
	iRepositoryInfo.Reset();	
	}
	
void CObservable::CloseiOpenRepositories() 
	{
	for(TInt i=iOpenRepositories.Count()-1; i>=0; i--)
		{
		delete iOpenRepositories[i];			
		iOpenRepositories.Remove(i);
		}
	ASSERT(iOpenRepositories.Count()==0);
	iOpenRepositories.Close();
	}

void CObservable::CloseiRepositoryInfo() 
	{
	for(TInt i=iRepositoryInfo.Count()-1; i>=0; i--)
		{
		delete iRepositoryInfo[i];			
		iRepositoryInfo.Remove(i);
		}
	ASSERT(iRepositoryInfo.Count()==0);
	iRepositoryInfo.Close();
	}
	
#endif	

/**
This function compares two UID's and indicates their sorting order.  This
implementation avoids overflow problems inherent in 'return aUid1-aUid2' 
implementations.
*/
TInt CObservable::CompareTUidValues(TInt aUid1, TInt aUid2)
	{
	if (aUid1 > aUid2)
		return 1;
	if (aUid1 < aUid2)
		return -1;
	return 0;
	}


TInt CObservable::ObserverSortOrder(const TRepositoryObserverInfo &aRepository1, const TRepositoryObserverInfo &aRepository2)
	{
	TInt result;
	
	result = CompareTUidValues(aRepository1.iRepositoryUid.iUid, aRepository2.iRepositoryUid.iUid);
	if (result == 0)	
		{
		if (aRepository1.iRepositoryPointer > aRepository2.iRepositoryPointer)
			return 1;
		if (aRepository1.iRepositoryPointer < aRepository2.iRepositoryPointer)
			return -1;		
		}
		
	return result;
	}

void CObservable::RemoveOpenRepository(CSharedRepository* aRepository)
	{
	TInt index = iOpenRepositories.Find(aRepository);
	
	if (index>=0)		
		{
		// we don't fail any transactions here anymore, because transactions can now survive NOC repository
		// unloads. The shared information about transactions is kept in memory seperately as long as a 
		// client is connected. 
		iOpenRepositories.Remove(index);
		delete aRepository;
		}
	}

TInt CObservable::FindOpenRepository(TUid aUid) const
	{
	TInt i;
	TInt count=iOpenRepositories.Count();
	for(i=count-1; i>=0; i--)
		{
		if(iOpenRepositories[i]->Uid()==aUid)
			{
			break;
			}
		}
	return i;
	}

TInt CObservable::ReadIniFileL(CSharedRepository*& aRepository, TCentRepLocation aLocation)
	{
	TInt r=KErrNone;
	CIniFileIn* inifile = 0;

	HBufC* fileName(NULL);
    //allocates memory on the heap
    TServerResources::CreateRepositoryFileNameLC(fileName, aRepository->Uid(), aLocation, EIni);		
	r = CIniFileIn::NewLC(TServerResources::iFs,inifile,*fileName);
	if(r==KErrNone)
		{			
		r=ReadSettingsL(inifile, aRepository);		
		if(r==KErrCorrupt)
			{
			// File is corrupt, if it's not the ROM file, delete it
			if(fileName && aLocation != ERom)
				User::LeaveIfError(TServerResources::iFs.Delete(*fileName));
			// Delete any repository settings that may have been read in
			aRepository->GetSettings().Reset();
			}
		}

	CleanupStack::PopAndDestroy(inifile); // inifile	 
	CleanupStack::PopAndDestroy(fileName);	// filename
	return r;
	}

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS

// Function to compare two TMultiRofsList's.
TInt CObservable::CompareUid(const TMultiRofsList& aUid1, const TMultiRofsList& aUid2)
	{
    if (aUid1.iRepUid.iUid < aUid2.iRepUid.iUid)
        return -1 ;
    if (aUid1.iRepUid.iUid > aUid2.iRepUid.iUid)
		return 1 ;
   	return 0 ;
	}

void CObservable::ConstructL()
	{
	//now search for repositories uid that might be composed of multi rofs
#ifndef CENTREP_CONV_TOOL
	if (TServerResources::iRomDirectory)
		{
		TFileName searchFileFormat;
		searchFileFormat.Append(*(TServerResources::iRomDirectory));
		searchFileFormat.Append(_L("????????.???[*-00]"));
	    CDir* entryList=NULL;		
		User::LeaveIfError(TServerResources::iFs.GetDir(searchFileFormat,KEntryAttNormal,ESortByName,entryList));
		CleanupStack::PushL(entryList);
		ProcessMultiRofsListL(*entryList);
		CleanupStack::PopAndDestroy(entryList);
		}
#endif	
	}

void CObservable::ProcessMultiRofsListL(const CDir& aList)
	{
	TLex parser;
	TUint32 uidNum;
	TUint8 mountId;
	TLinearOrder<TMultiRofsList> reposSortOrder(CompareUid);
	TInt count=aList.Count();
	iMultiRofsUidList.ReserveL(count);
	for (TInt i=0;i<count;i++)
		{
		//file format as following XXXXYYYY.<cre/txt>[id-00]
		//Extract the Uid first
		parser.Assign(aList[i].iName.Left(8));		
		User::LeaveIfError(parser.Val(uidNum,EHex));		
		
		//Extract the Mount id now, see the file format above
		parser.Assign(aList[i].iName.Mid(13,2));
		User::LeaveIfError(parser.Val(mountId,EHex));
		
		//Now find whether this uid is already in the list
		TMultiRofsList find(TUid::Uid(uidNum));
		//passing it the extension of the file
		TRofsFlag newFlag(aList[i].iName.Mid(9,3),mountId);
		
		TInt err=iMultiRofsUidList.FindInOrder(find,reposSortOrder);
		if (err==KErrNotFound)
			{
			//new entry
			find.iMountFlagList.AppendL(newFlag);
			iMultiRofsUidList.InsertInOrderL(find,reposSortOrder);
			}
		else
			{
			//entry exist, just need to update the array inside TMultiRofsList
			err=iMultiRofsUidList[err].iMountFlagList.InsertInOrder(newFlag,TRofsFlag::CompareFlag);
			//ignore KErrAlreadyExists which might be the case if a txt and cre found on the same
			//rofs but the cre is preferred.
			if (err!=KErrNone && err!=KErrAlreadyExists)
				{
				User::Leave(err);
				}
			}
		}		
	}


void CObservable::OverrideSettingL(TServerSetting& aBaseSetting,const TServerSetting& aOvSetting,CSharedRepository* aCoreRepository,TBool aNewOv,TBool aNewOvIndivPolicy)
	{
	TUint32 ovId=aOvSetting.Key();
	
	//---------------------CHECK AND SET THE META----------------------------
	TBool indivOvMeta=aOvSetting.IsIndividualMeta()!=0;
	//if individually specified meta override this
	if (indivOvMeta)
		{
		aBaseSetting.SetMeta(aOvSetting.Meta());
		}
	//if not specified only when it is new we set it from the default meta
	else
		{
		if (aNewOv)	
			aCoreRepository->SetMetaDataOnRead(aBaseSetting,EFalse);		
		}

	//----------------------CHECK AND SET THE PLATSEC----------------------------	
	TLinearOrder<TSettingsAccessPolicy> order(&CHeapRepository::CompareKeyIds);	
	TSettingsAccessPolicy* ovTs=aOvSetting.AccessPolicy();
	TSettingsAccessPolicy* coreTs=aCoreRepository->GetFallbackAccessPolicy(ovId,ETrue);
	//new setting defined
	if (aNewOv)
		{
		//specify own settings
		if (aNewOvIndivPolicy)	
			{
			TSettingsAccessPolicy* newPol=new (ELeave)TSettingsAccessPolicy(*(const_cast<TSecurityPolicy*>
			(aOvSetting.GetReadAccessPolicy())),*(const_cast<TSecurityPolicy*>(aOvSetting.GetWriteAccessPolicy())),ovId,ovTs->HighKey(),ovTs->KeyMask());
			CleanupStack::PushL(newPol);
			//for single setting policy, the highkey is used to indicate whether the setting define its own read policy or it is just a fallback
			if (ovTs->HighKey()==0)
				{
				newPol->iReadAccessPolicy=*(coreTs->GetReadAccessPolicy());
				}
			//for single setting policy, the keymask is used to indicate whether the setting define its own write policy or it is just a fallback
			if (ovTs->KeyMask()==0)
				{
				newPol->iWriteAccessPolicy=*(coreTs->GetWriteAccessPolicy());
				}
			//insert the new one and set the setting policy to point to this
			aCoreRepository->iSimRep->SinglePolicyArray().InsertInOrderL(newPol,order);
			aBaseSetting.SetAccessPolicy(newPol);				
			CleanupStack::Pop(newPol);			
			}
		//no individual setting policy specified so revert to the default policy in the base	
		else
			{
			aBaseSetting.SetAccessPolicy(coreTs);	
			}
		}
	//old setting
	else
		{
		//if no individual policy specified do nothing as this keeps what we have in the base setting
		if (aNewOvIndivPolicy)
			{
			TInt baseIndividualPol=aCoreRepository->iSimRep->SinglePolicyArray().Find(aBaseSetting.AccessPolicy());	
			if (baseIndividualPol==KErrNotFound)
				{
				//no base individual but overriding specify one so create one and insert into list
				//point the old base setting to this newly defined invididual setting
				TSettingsAccessPolicy* newPol=new (ELeave)TSettingsAccessPolicy(*(const_cast<TSecurityPolicy*>
				(aOvSetting.GetReadAccessPolicy())),*(const_cast<TSecurityPolicy*>(aOvSetting.GetWriteAccessPolicy())),ovId,ovTs->HighKey(),ovTs->KeyMask());
				CleanupStack::PushL(newPol);
				//for single setting policy, the highkey is used to indicate whether the setting define its own read policy or it is just a fallback								
				if (ovTs->HighKey()==0)
					{
					newPol->iReadAccessPolicy=*(coreTs->GetReadAccessPolicy());
					}
				//for single setting policy, the keymask is used to indicate whether the setting define its own write policy or it is just a fallback				
				if (ovTs->KeyMask()==0)
					{
					newPol->iWriteAccessPolicy=*(coreTs->GetWriteAccessPolicy());
					}
				//insert the new one and set the setting policy to point to this
				aCoreRepository->iSimRep->SinglePolicyArray().InsertInOrderL(newPol,order);
				aBaseSetting.SetAccessPolicy(newPol);				
				CleanupStack::Pop(newPol);			
				}
			else
				{
				TSettingsAccessPolicy* oldPol=aBaseSetting.AccessPolicy();				
				//existing individual already exist, just update them
				//for single setting policy, the highkey is used to indicate whether the setting define its own read policy or it is just a fallback
				if (ovTs->HighKey()!=0)
					{
					oldPol->iReadAccessPolicy=*(aOvSetting.GetReadAccessPolicy());
					}
				//for single setting policy, the keymask is used to indicate whether the setting define its own write policy or it is just a fallback		
				if (ovTs->KeyMask()!=0)
					{
					oldPol->iWriteAccessPolicy=*(aOvSetting.GetWriteAccessPolicy());
					}
				}
			}
		}

	//---------------------SET THE VALUE---------------------------------------
	//override the value only if it is a new setting or an old setting with value modified
	if (aNewOv || (!aNewOv && aBaseSetting!=aOvSetting))
		User::LeaveIfError(aBaseSetting.CopyTypeValue(aOvSetting));
	
	//need to set it clean as this is still ROM settings
	aBaseSetting.SetClean();			
	}

//Function containing the rule of merging repositories to a core repository
//such as -ignoring of the global properties(non-overriden)
//        -type cannot be overriden
void CObservable::MergeRepositoryL(CSharedRepository* aCoreRepository,CHeapRepository* aOverrideRepository)
	{
	/**
	We may want to verify that the following global properties match
	-OwnerUid
	-Uid
	We may want to give warning if they try to have global propertie section in the overriding layers
	although this obviously now can be ignored now, just need to make sure that repository follows the format
	and not corrupted
	*/
	TSettingsAccessPolicy defaultTs=aOverrideRepository->GetDefaultAccessPolicy();
	//here we panic immediately if there is any defined in the range meta/policy(we can check individually if
	//they do override later on,we will assume any definiton of global policy is invalid here
	if (   aOverrideRepository->Owner() != aCoreRepository->iSimRep->Owner()
		|| (aOverrideRepository->DefaultMeta()!=0 && aOverrideRepository->DefaultMeta()!= aCoreRepository->iSimRep->DefaultMeta())
		|| (defaultTs.HighKey()!=0 && aOverrideRepository->GetDefaultReadAccessPolicy().Package() !=	aCoreRepository->iSimRep->GetDefaultReadAccessPolicy().Package())
		|| (defaultTs.KeyMask()!=0 && aOverrideRepository->GetDefaultWriteAccessPolicy().Package()!= aCoreRepository->iSimRep->GetDefaultWriteAccessPolicy().Package())
		|| (aOverrideRepository->RangeMetaArray().Count()!=0 && !(aCoreRepository->iSimRep->RangeMetaArray().IsEqual(aOverrideRepository->RangeMetaArray())))
		|| (aOverrideRepository->RangePolicyArray().Count()!=0 && !(aCoreRepository->iSimRep->RangePolicyArray().IsEqual(aOverrideRepository->RangePolicyArray())		
		)))
		{
		#ifdef _DEBUG		
			RDebug::Printf("Illegal Global Properties Overriding");
		#endif		
		User::Leave(KErrMultiRofsGlobalOverride);
		}

	TInt single_count=aOverrideRepository->SettingsArray().Count();
	for (TInt i=0;i<single_count;i++)
		{
		//get the overriding setting
		TServerSetting& ovTs=aOverrideRepository->SettingsArray()[i];
		TUint32 ovId=ovTs.Key();
				
		//find whether it exist in the core
		TServerSetting* coreTs=aCoreRepository->GetSettings().Find(ovId);
		//check whether in the overriding repository,the policy is individually specified policy
		TInt ovIndividualPol=aOverrideRepository->SinglePolicyArray().Find(ovTs.AccessPolicy());
		if (coreTs)
			{
			//found in core, need to check the type is the same else Panic
			if (coreTs->Type()!=ovTs.Type())
				{
				#ifdef _DEBUG			
					RDebug::Printf("Illegal Setting Type Overriding");
				#endif	
				User::Leave(KErrMultiRofsTypeOverride);				
				}
			OverrideSettingL(*coreTs,ovTs,aCoreRepository,EFalse,ovIndividualPol!=KErrNotFound);					
			}
		else
			{
			//this is a newly defined setting
			TServerSetting newTs(ovTs.Key());
			OverrideSettingL(newTs,ovTs,aCoreRepository,ETrue,ovIndividualPol!=KErrNotFound);
			//finally insert into the RSettingsArray for this new setting
			aCoreRepository->iSimRep->SettingsArray().OrderedInsertL(newTs);	
			}
		}
	
	}	

/**Function on initialising a repository of multi ROFS files
aCoreInitialized indicate whether there is already existing keyspace file in the core layer
otherwise the first one in the rofs layer will be the core repository
*/ 
void CObservable::MergeMultiRofsL(TBool aCoreInitialized,CSharedRepository* aCoreRepository,const RArray<TRofsFlag>& aOverridingFileList)
	{
	//load all the files and construct an array of CHeapRepository to merge content into it
	TInt sortedCount=aOverridingFileList.Count();
	TFileName repFileName;
	for (TInt i=0;i<sortedCount;i++)		
		{
		repFileName.Zero();
		TRofsFlag rofsFlag=aOverridingFileList[i];
		TPtrC extPointer(*(TServerResources::iCreExt));
		TBool isTxt=rofsFlag.iFlag & 0x80000000;
		if (isTxt)
			{
			extPointer.Set(*(TServerResources::iIniExt));
			}
		repFileName.Format(_L("z:\\private\\10202be9\\%08x%S[%02x-00]"),aCoreRepository->Uid().iUid,&extPointer,rofsFlag.iFlag & 0xFF);
		//only when the core is not initialized, the first item in the list now becomes the core
		if (!aCoreInitialized && i==0)
			{
			if (isTxt)
				{
				CIniFileIn* iniFile;
				TInt err=CIniFileIn::NewLC(TServerResources::iFs,iniFile,repFileName);
				User::LeaveIfError(err);
				err=aCoreRepository->ReloadContentL(*iniFile,ETrue);
				User::LeaveIfError(err);
				CleanupStack::PopAndDestroy(iniFile);//iniFile
				}
			else
				{
				aCoreRepository->iSimRep->CreateRepositoryFromCreFileL(TServerResources::iFs,repFileName);
				}
			}
		else
			{
			CHeapRepository* repos=CHeapRepository::NewL(aCoreRepository->Uid());
			CleanupStack::PushL(repos);
		
			if (isTxt)
				{
				CIniFileIn* iniFile;
				TInt err=CIniFileIn::NewLC(TServerResources::iFs,iniFile,repFileName);
				User::LeaveIfError(err);
				repos->ReloadContentL(*iniFile);
				CleanupStack::PopAndDestroy(iniFile);//iniFile
				}
			else
				{
				TUint8 creVersion;
				repos->CreateRepositoryFromCreFileL(TServerResources::iFs,repFileName,creVersion);
				//need to check the CRE version in the overloading layer
				if (creVersion<KPersistFormatSupportsIndMetaIndicator)
					{
					#ifdef _DEBUG		
						RDebug::Printf("Use of Cre version less than 2 in the overriding layer");
					#endif	
					//this macro is used for testing existing regression test with multi-rofs
					//so we allow older cre to be used in higher rofs
					#ifndef PDS_TEST_MULTIROFS											
						User::Leave(KErrMultiRofsOldCreUsed);
					#endif								
					}
				}
			//Now that the repository is initialized and at this stage the repository file has been
			//checked whether they are corrupt	
			repos->SettingsArray().SetIsDefault(ETrue);
			MergeRepositoryL(aCoreRepository,repos);

			CleanupStack::PopAndDestroy(repos);	//repos
			}
		}
	}
#endif	

TInt CObservable::CreateRepositoryL(CSharedRepository* aRepository, TCentRepLocation aLocation)
	{
	aRepository->GetSettings().SetIsDefault(aLocation!=EPersists);
    TInt err(KErrNotFound);
    
	err = aRepository->CreateRepositoryFromCreFileL(aLocation);
	if(err==KErrNotFound)
		{
		if (aLocation!=EPersists)
			err = ReadIniFileL(aRepository,aLocation);
		}
	//for ROM might want to consider the possibility of multi rofs file
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
	if (aLocation==ERom && iMultiRofsUidList.Count()!=0)
		{
		//if there is error and not because of non-existant we should return error immediately
		//and if it is fine but the uid does not match one of the multirofsuidlsit this means that
		//repository is not composed of multi file.
		if (err==KErrNone || err==KErrNotFound)
			{
			TMultiRofsList find(aRepository->Uid());
			TLinearOrder<TMultiRofsList> sort_order(CompareUid);			
			TInt index=iMultiRofsUidList.FindInOrder(find,sort_order);		
			if (index==KErrNotFound)	
				return err;
			//if the first layer has already got the exattrib=U we should leave with KErrMultiRofsIllegalRofs
			if (err==KErrNotFound && index!=KErrNotFound)	
				User::Leave(KErrMultiRofsIllegalRofs);
			//the list exist in one of the mappings stored earlier, get the name and construct them
			TRAP(err,MergeMultiRofsL(err==KErrNone,aRepository,iMultiRofsUidList[index].iMountFlagList));
			if (err==KErrNoMemory)	
				User::LeaveNoMemory();
			}
		}
#endif		
	return( err);
	}
	
/**
In order to create a repository this routine looks for a .cre or .txt file. 
Txt and cre files can co-exist in install and ROM but not in persists location.
If a persists txt file exists, the first write to the repository will cause a
cre file to be created and the txt file to be deleted.
If both files exist in the same location, the .cre is picked up first. 
If the .cre file is not found,a .txt file from the same location is tried.
Otherwise if the .cre file is corrupted,it tries a .cre file from a next location. 

Note:
If a cre file exists at a particular location, even if the cre file is corrupt a txt 
file will not be searched for in the same location. 
*/

TInt CObservable::CreateRepositoryL(CSharedRepository*& aRepository, CIniFileIn::TIniFileOpenMode aIniFileOpenMode)
    {
    TInt err(KErrNotFound);
    
    switch (aIniFileOpenMode)
	    {
	    case CIniFileIn::EAuto:
	        {
	        // Look in persists dir	 
	        err=CreateRepositoryL(aRepository, EPersists);		
	        				
			if(err==KErrNone)
				{
				return err;
				}
			 
	        // No persists file - look in ROM dir	 
	        // Do this before looking in the install dir, because if there is
	        // a ROM file, an install file and no persists file then this
	        // repository is being opened after a SW install but before the 
	        // merge. In this case the install file shouldn't be opened
	        // until after the merge.
	        err=CreateRepositoryL(aRepository, ERom);		
	        				
			if(err==KErrNone)
				{
				break;
				}
			else if(err==KErrNotFound)
				{				
			     // Look in install directory only if there was no ROM or persists file	            
				err=CreateRepositoryL(aRepository, EInstall);
				if(err==KErrNone)
					{
					TTime installFileTimeStamp=TServerResources::CentrepFileTimeStampL(aRepository->Uid(), EInstall);
					aRepository->SetInstallTime(installFileTimeStamp);
					}
				}
	        break;
	        }
	        
	    case CIniFileIn::EInstallOnly:
	        {
	    	err=CreateRepositoryL(aRepository, EInstall);		
	        break;
	        }

	    case CIniFileIn::ERomOnly:
	        {
	    	err=CreateRepositoryL(aRepository, ERom);		
	        break;
	        }
	    }

	return err;
	}
	
TInt CObservable::ReadSettingsL(CIniFileIn *aIniFile, CSharedRepository* aRep)
	{	
	return aRep->ReloadContentL(*aIniFile, ETrue);
	}

void CObservable::LoadRepositoryLC(TUid aUid, TBool aFailIfNotFound, CSharedRepository*& aRepository, CIniFileIn::TIniFileOpenMode aIniFileOpenMode)
	{
	// Calculate the amount of memory this repository will take in the heap
	// by checking the heap size before and after the internalization
	RHeap& myHeap = User::Heap();
	TInt firstSize = myHeap.Size();
	TInt biggestBlock;
	TInt firstAvail = myHeap.Available(biggestBlock);
	
	TRAPD(err, aRepository = CSharedRepository::NewL(aUid));
#ifdef CACHE_OOM_TESTABILITY
	if ((err!=KErrNone)&&!iTrapOOMOnOpen)	
		{
		User::Leave(err);
		}
#endif	
	if ((err!=KErrNone)&&TServerResources::iCacheManager->Enabled())
		{
		// If cache enabled, try recovery by releasing the cache
		TServerResources::iCacheManager->FlushCache(EFalse);
		// retry
		aRepository = CSharedRepository::NewL(aUid);
		err = KErrNone;
		}
	User::LeaveIfError(err);
		
	// Now that we have enough memory for the object and constructed it properly
	// we try to load it. We trap all errors, either from leaving functions or error code
	// returning functions and unify them (in all cases only one of these codes will
	// contain a valid value and the other will be 0, and for our purposes we treat
	// all errors the same no matter if they're thrown or returned)

	TInt unifiedErrorCode;
	TRAP_UNIFIED(unifiedErrorCode, CreateRepositoryL(aRepository, aIniFileOpenMode));
	
	if (unifiedErrorCode!=KErrNone)
	{
	switch(unifiedErrorCode)
		{
		case KErrNoMemory:
			{
			if (TServerResources::iCacheManager->Enabled()) // cache enabled
				{
#ifdef CACHE_OOM_TESTABILITY
				if (!iTrapOOMOnOpen)	
					{
					delete aRepository;
					aRepository = NULL;
					User::Leave(KErrNoMemory);
					}
#endif	
				// Flush cache
				TServerResources::iCacheManager->FlushCache(ETrue);

				firstSize = myHeap.Size();
				firstAvail = myHeap.Available(biggestBlock);
	
				//retry
				TRAP_UNIFIED(unifiedErrorCode, CreateRepositoryL(aRepository, aIniFileOpenMode));
				}
			}
			break;
		case KErrNotFound:
		case KErrPathNotFound:		
			{
			if (!aFailIfNotFound) // backup open
				{
				// override error condition and continue normally
				unifiedErrorCode = KErrNone;
				}
			}
			break;
		}
	}
	// If unhandled, leave
	if(unifiedErrorCode != KErrNone)
		{
		delete aRepository;
		aRepository = NULL;
		}
	User::LeaveIfError(unifiedErrorCode);
	CleanupStack::PushL(aRepository);

	// Otherwise, finalize calulations
	TInt lastSize = myHeap.Size();
	TInt lastAvail = myHeap.Available(biggestBlock);

	TInt calcSize = (lastSize - lastAvail) - (firstSize - firstAvail);
	// record repository size for cache algorithm purposes
	aRepository->SetSize(calcSize);
	}

CSharedRepository* CObservable::AccessL(TUid aUid, TBool aFailIfNotFound)
	{
	CSharedRepository* rep = NULL;
	
	TInt i = FindOpenRepository(aUid);
	if(i!=KErrNotFound)
		{
		rep = GetOpenRepository(i);
		// repository still open, can safely call RestoreConsistencyL
		rep->RestoreConsistencyL();
		}
	else
		{
		// For memory usage testing purposes
		RECORD_HEAP_SIZE(EMemLcnRepositoryOpen, aUid.iUid);
		// Various error conditions are handled in this function 
		LoadRepositoryLC(aUid, aFailIfNotFound, rep, CIniFileIn::EAuto);
		__CENTREP_TRACE1("CENTREP: Repository Load when opening repository %x", aUid.iUid);
		// For memory usage testing purposes
		RECORD_HEAP_SIZE(EMemLcnRepositoryOpen, aUid.iUid);

		AddOpenRepositoryL(rep);
		
		// We pop the rep here because if later call of TServerResources::AddOwnerIdLookupMapping fails of OOM
		// the call of RemoveOpenRepository() will delete the repository before leave.
		CleanupStack::Pop(rep);
		
		// Add owner mapping to list - Will fail if an entry already exists
		// with this Repository UID but this doesn't matter
		TUid owner = rep->Owner() ;
		TInt err = TServerResources::AddOwnerIdLookupMapping(aUid.iUid, owner.iUid);	
		if (err == KErrNoMemory)
			{
			RemoveOpenRepository(rep);
			User::Leave(err);
			}
		
		//Find the location of the current transaction for this repository
		const TInt offset (FindRepositoryInfo(aUid));
		if (offset >=0) //If there are no outstanding transactions
			{
			RefreshTransactorAccessPolicies(rep,offset);
			}
		}

	// re-start the timer
	if (TServerResources::iCacheManager->Enabled())
		{
		TServerResources::iCacheManager->StartEviction(rep);
		}	
		
	return rep;
	}

TInt CObservable::FindRepositoryInfo(TUid aUid)
	{
	TSharedRepositoryInfo info(aUid);
	TLinearOrder<TSharedRepositoryInfo> infoSortOrder(CObservable::InfoSortOrder);
	
	return iRepositoryInfo.FindInOrder(&info, infoSortOrder);
	}
	
CObservable::TSharedRepositoryInfo* CObservable::SharedRepositoryInfo(TUid aUid)
	{
	TInt pos = FindRepositoryInfo(aUid);
	return (pos!=KErrNotFound) ? iRepositoryInfo[pos] : NULL;
	}

void CObservable::CancelTransaction(CRepositoryTransactor& aTransactor,TUid aRepositoryUid)
    {
    if (aTransactor.IsInTransaction())
        {
        ReleaseTransactionLock(aTransactor,aRepositoryUid);
        CObservable::TSharedRepositoryInfo* shrepinfo =SharedRepositoryInfo(aRepositoryUid);
        ASSERT(shrepinfo);
        shrepinfo->iTransactors.Remove(aTransactor);
        //Remove the link to the next transaction
        aTransactor.iLink.iNext = NULL;
        aTransactor.Deque();
        }
    }

/** Private helper method which releases any read/write locks held in the shared repository
by this transactor. Caller must set transactor's state or remove from queue as appropriate.
@param aTransactor transactor whose read/write locks are to be released.
@post Any read/write locks held by transactor are released.
*/
void CObservable::ReleaseTransactionLock(CRepositoryTransactor& aTransactor,TUid aRepositoryUid)
    {
    CObservable::TSharedRepositoryInfo* shrepinfo = SharedRepositoryInfo(aRepositoryUid);
    ASSERT(shrepinfo);
    if (aTransactor.IsInActiveConcurrentReadWriteTransaction())
        {
        shrepinfo->iNumActiveConcurrentReadWriteTransactions--;
        ASSERT(shrepinfo->iNumActiveConcurrentReadWriteTransactions >= 0); // sanity check
        }
    else if (aTransactor.IsInActiveReadTransaction())
        {
        shrepinfo->iPessimisticTransactionLockCount--;
        ASSERT(shrepinfo->iPessimisticTransactionLockCount >= 0); // sanity check
        }
    else if (aTransactor.IsInActiveExclusiveReadWriteTransaction())
        {
        // can only be one exclusive read/write transaction active (lock value -1)
        ASSERT(shrepinfo->iPessimisticTransactionLockCount == -1);
        shrepinfo->iPessimisticTransactionLockCount = 0;
        }
    }
CObservable::TSharedRepositoryInfo::TSharedRepositoryInfo(TUid aUid) : 
	iRepositoryUid(aUid), iTransactors(_FOFF(CRepositoryTransactor, iLink)), 
	iPessimisticTransactionLockCount(0), iNumActiveConcurrentReadWriteTransactions(0) 
	{
	}
	
TInt CObservable::InfoSortOrder(const TSharedRepositoryInfo &aRepository1, const TSharedRepositoryInfo &aRepository2)
	{
	return CompareTUidValues(aRepository1.iRepositoryUid.iUid, aRepository2.iRepositoryUid.iUid);
	}
	
void CObservable::AddSharedRepositoryInfoL(TUid aUid)
	{
	TSharedRepositoryInfo* shinfo = new(ELeave) TSharedRepositoryInfo(aUid);

	TLinearOrder<TSharedRepositoryInfo> infoSortOrder(CObservable::InfoSortOrder);
	// Inserts if not already in the array, otherwise returns KErrAlreadyExists, which we handle gracefully
	TInt err = iRepositoryInfo.InsertInOrder(shinfo, infoSortOrder);	
	if (err==KErrAlreadyExists)
		{
		delete shinfo;
		}
	else
		{
		if (err!=KErrNone)
			delete shinfo;
		User::LeaveIfError(err);
		}
	}
	
void CObservable::RemoveSharedRepositoryInfo(TUid aUid)
	{
	TInt pos = FindRepositoryInfo(aUid);
	if (pos!=KErrNotFound)
		{
		delete iRepositoryInfo[pos];
		iRepositoryInfo.Remove(pos);
		}
	}

	
void CObservable::RefreshTransactorAccessPolicies(CSharedRepository* aRepository,const TInt offset)
	{
	TSglQueIter<CRepositoryTransactor> iter(iRepositoryInfo[offset]->iTransactors);
	CRepositoryTransactor* t (iter);
	
	while (iter++ != NULL)
		{
		const TInt count = t->iTransactionSettings.Count();
		for (TInt i=0; i<count;i++) //for the no. of changed settings in the transaction
			{
			TServerSetting temp = t->iTransactionSettings[i];
			t->iTransactionSettings[i].SetAccessPolicy(aRepository->GetFallbackAccessPolicy(temp.Key()));		
			//Correct the access policy pointer so that its pointing to the new valid location
			}
		t = iter;
		}
	}	
