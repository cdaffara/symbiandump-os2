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
//

/**
 @file
 @internalTechnology
*/

#ifndef __BASSNDSTORE_H__
#define __BASSNDSTORE_H__

#include <bassnd.h>
#include <centralrepository.h>


/**
This class encapsulates storing and retrieving sound data
to and from persistent storage.
*/
NONSHARABLE_CLASS(CBaSsndStore) : public CBase
	{
public:
	static CBaSsndStore* NewL();
	~CBaSsndStore();

	TInt GetSound(const TBaSystemSoundType& aSoundType,
				  TBaSystemSoundInfo& aInfo) const;
	TInt GetSoundCategory(const TBaSystemSoundUid& aSSUid,
						  CArrayFixFlat<TBaSystemSoundInfo>& aArray) const;
	void SetSoundL(const TBaSystemSoundInfo& aInfo) const;

protected:
	// These are protected to facilitate testing concurrent SetSound
	CBaSsndStore();
	void ConstructL();

	virtual void KeyOfSoundTypeL(TBaSystemSoundType& aSoundType, TUint32& aKey) const;
	virtual void SetSoundInTransactionL(TBaSystemSoundType& aSoundType,
										const TDesC8& aDes8) const;

private:
	void GetSoundL(const TBaSystemSoundType& aSoundType,
				   TBaSystemSoundInfo& aInfo) const;
	void GetSoundCategoryL(const TBaSystemSoundUid& aSSUid,
						  CArrayFixFlat<TBaSystemSoundInfo>& aArray) const;
	void FindKeyL(const TBaSystemSoundType& aSoundType,
				  TUint32& aKey,
				  TBool aNoLeaveIfNotFound) const;
	void FindKeyL(const TBaSystemSoundUid& aSSUid, RArray<TUint32>& aKeys) const;
	void FindNextEmptySlotL(TUint32& aKey) const;

protected:
	CRepository*	iRepository;
	};
#endif
