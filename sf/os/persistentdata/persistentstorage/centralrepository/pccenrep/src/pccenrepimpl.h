// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "heaprepos.h"
#include "datatype.h"
#include "operations.h"

/**
PC side repository impl class
@internalTechnology
*/
NONSHARABLE_CLASS(CPcRepImpl): public CBase, public MOperationLogic
{
friend class CRepository;
public:
	static CPcRepImpl* NewL(TUid aRepositoryUid,const TDesC& aInFileName, const TDesC& aOutFileName,TBool aAutoLoading);
	~CPcRepImpl();
	void MoveL(TUint32 aSourcePartialKey, TUint32 aTargetPartialKey,TUint32 aMask, TUint32& aErrorKey);
	void DeleteRangeL(TUint32 aPartialKey, TUint32 aMask,TUint32& aErrorKey);
	void GetSingleMetaArrayL(RSingleMetaArray& aMetaArray);
	TInt Flush();
	static void FailTransactionCleanupOperation(TAny* /**aRepository*/);		

	//----------------virtuals from MOperationLogic------------------------------------
	void GetSingleMeta(TUint aKey,TUint32& aMeta)
		{
		TInt ret=iSingleMetaArray.Find(aKey,aMeta);
		if (ret==KErrNotFound)
			{
			//search for range/then default meta
			TSettingsDefaultMeta *defaultMeta = iRepository->RangeMetaArray().Find(aKey);
			if (defaultMeta)
				{
				aMeta=defaultMeta->GetDefaultMetadata();
				}
			else
				{
				aMeta=iRepository->DefaultMeta();
				}			
			}
		}

	TSettingsAccessPolicy* GetFallbackAccessPolicy(TUint32 aId)const
		{
		return iRepository->GetFallbackAccessPolicy(aId);
		}
	TServerSetting* GetSetting(TUint aKey)
		{
		return iRepository->SettingsArray().Find(aKey);
		}
	TInt FindSettings(TUint32 aSourcePartialKey,TUint32 aMask,RSettingPointerArray& aOutputArray) const
		{
		return iRepository->SettingsArray().Find(aSourcePartialKey,aMask,aOutputArray);
		}

	RSettingsArray& GetWritableSettingList()
		{
		return iRepository->SettingsArray();
		}
		
	//------------------------end of virtuals------------------------------------------
		
	template <class T>	
	void FindSettingsComparisonL(TUint32 aSourcePartialKey,TUint32 aMask,const T& aValue,TComparison aComparison,RArray<TUint32>& aFoundIds) const
		{
		RSettingPointerArray sourceArray;
		CleanupClosePushL(sourceArray);
		User::LeaveIfError(FindSettings(aSourcePartialKey,aMask,sourceArray));
		
		FindCompareL(sourceArray,aValue,aComparison,aFoundIds);
		CleanupStack::PopAndDestroy();
		}

private:
	void ConstructL(TUid aRepositoryUid,const TDesC& aInFileName,const TDesC& aOutFileName,TBool aAutoLoading);
	void IsOriginalL(TUid aUid, const TDesC& aOutFile, TBool aAutoLoading, TFileName& aOgnFileName, TBool& aIsOriginal);
	void RemoveAnyMarkDeleted()
		{
		for (TInt i=0;i<iRepository->SettingsArray().Count();i++)
			{
			TServerSetting ts=iRepository->SettingsArray()[i];
			if (ts.IsDeleted())
				{
				iRepository->SettingsArray().DeleteElement(i);
				i--;			
				}
			}		
		}

private:	
	CHeapRepository* iRepository;
	HBufC* iOutFileName;
	RFs iFs;
	RSingleMetaArray iSingleMetaArray;
	TBool iInitialised;	
};
