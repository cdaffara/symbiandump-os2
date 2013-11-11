// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common Driver components
// 
//

#ifndef __D32DRVR_H__
#define __D32DRVR_H__

#ifndef __D32DBAS_H__
#include "D32DBAS.H"
#endif
#ifndef __D32CACHE_H__
#include "D32CACHE.H"
#endif

// classes referenced
class CDbDatabase;

/**
@internalComponent
*/
typedef TBufC<KDbMaxFormatName> TDbFormatName;

template <class T,class Factory> inline T* AttachContext(Factory* aF,T* aT);

void ExtractUidAndName(const TDesC& aDbFormatStr, TUid& aDbsUid, TPtrC& aUidName);
CDbDatabase* CreateDatabaseL(TDbFormat::TCreate aMode, RFs& aFs, 
							 const TDesC& aSource, const TDesC& aFormat);

/**
@internalComponent
*/
class CDbContext : public CBase
	{
	friend class CDbObject;
public:
	void Attach(CDbObject* aObject);
protected:
	inline CDbContext() {}
	inline TInt Ref() const;
	inline void Open();
	void Close();
	virtual void Idle();
private:
	TInt iRef;
	};

#include "D32DRVR.INL"
#endif
