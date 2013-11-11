// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __INSTALL_H__
#define __INSTALL_H__

#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include "srvrepos_noc.h"
#include <sacls.h>
#include <saclscommon.h>

class CInstallEntry : public CBase
	{
public:
	static CInstallEntry* NewL();
	CInstallEntry();
	~CInstallEntry();
	void ConstructL();
	void SetL(TEntry& aEntry);
	void HandleFileDeleteL(TInt aOperation);
	void HandleFileCreateL(TInt aOperation);
	void HandleFileUpdateL(TInt aOperation);

	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

	TUid Uid() const;
	TTime Modified() const;
	TCentRepFileType FileExt() const;
private:
	TUid   iUid;
	TTime  iModified;
	TCentRepFileType iFileExt;
	CServerRepository* iRepository;
	CSessionNotifier* iNotifier;
	};
	
class CCentRepSWIWatcher: public CActive
	{
public:
	static CCentRepSWIWatcher* NewL(RFs& aFs);
	void Start();
	~CCentRepSWIWatcher();
	void HandleSWIEventL(TInt swiProperty);
    void FindChangedEntriesL(TBool aStartup=EFalse);
	//From CActive
protected:	
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	CCentRepSWIWatcher(RFs& aFs);
	void NotifyChange();
	void ConstructL();
	
	void ReadInstallDirL(RPointerArray<CInstallEntry>& aEntryArray);
	
	void SaveInstallDirL();
	void GetInstallDirL();
	void ReadAndInternalizeInstallDirL(const TDesC& aInstallDirFilePath);
	void HandleFileChangesL(TBool aStartup);
	
	static TBool MatchEntries(const CInstallEntry &aSource, const CInstallEntry &aTarget);

	TDesC& iInstallDir;
	RFs& iFs;
	RPointerArray<CInstallEntry> iInstallEntryArray;
	RPointerArray<CInstallEntry> iCurrentInstallDirEntries;
	RProperty iSWIKey;
	TInt iSWIOperation;
	TInt iSWIStatus;
	};
#endif
