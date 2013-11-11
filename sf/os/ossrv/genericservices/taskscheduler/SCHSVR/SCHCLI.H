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
// class CClientProxy
// -scheduler's representation of client
// -contains info scheduler needs to start real client in its own process	
// -reference-counted: deleted when no tasks & no sessions reference it
// 
//

#ifndef __SCHCLI_H__
#define __SCHCLI_H__

#include <e32base.h>

// Classes referenced
class RFs;
class CScheduledTask;
class RWriteStream;
class CFileStore;
class CSchLogManager;


/**
@internalComponent
*/
NONSHARABLE_CLASS(CClientProxy) : public CBase
	{
public:
	static CClientProxy* NewL(RFs& aFsSession, 
							RReadStream& aStream,
							CSchLogManager& aLogManager);
	static CClientProxy* NewL(RFs& aFsSession, 
							const TDesC& aFileName, 
							TInt aPriority,
							CSchLogManager& aLogManager);
	~CClientProxy();

private:
	CClientProxy(RFs& aFsSession,CSchLogManager& aLogManager);
	CClientProxy(RFs& aFsSession, TInt aPriority,CSchLogManager& aLogManager);
	void ConstructL(const TDesC& aFileName); 

public:
	TBool IsEqual(const TDesC& aFilename, TInt aPriority) const;
	void ExecuteTasks();
	void AddTask(CScheduledTask& aTask);
	void RemoveTask(CScheduledTask* aTask);
	void RemoveDueTasks();
	void TransferTasks(CClientProxy& aTargetClient);

	// sharing stuff
	inline void DecUsers();			
	inline void IncUsers();				
	inline TInt Users() const;	

	// execution
	inline void ReadyToExecute();		
	inline TBool IsReadyToExecute() const;

	// list capability
	inline static TInt Offset();
	inline void Remove();// remove youself from que

	inline const TDesC& ExecutorFileName() const { return *iFileName; }
	TDblQueIter<CScheduledTask> TaskIterator();
	TInt Priority() const;

public:
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

private: // internal functions
	void DoExecuteTasksL();
	CArrayPtr<CScheduledTask>* DueTasksL();// copy the due tasks into an array

	// called before running the task
	TInt DoExecuteTasks(const CArrayPtr<CScheduledTask>& aTasks, 
						const TDesC& aFileName);

	void DoExecuteTasksL(const CArrayPtr<CScheduledTask>& aTasks, 
						const TDesC& aFileName,
						const TDesC& aErrorMessage);

	void SaveTasksToFileL(const CArrayPtr<CScheduledTask>& aTasks, 
						const TDesC& aFileName);// save 'em to a new file
	void SaveTasksL(CFileStore& aStore, const CArrayPtr<CScheduledTask>& aTasks);
	CFileStore* CreateStoreL(const TDesC& aName, TUint aFileMode);// needed so we can call store->CreateLC() in a trap
	
private: // member data
	RFs& iFsSession;
	TBool iReadyToExecute;
	TInt iUsers;// for sharing
	HBufC* iFileName;// name of supplied ExeDll thingie
	//
	TPriQueLink iPriLink;// owned by scheduler & kept in priority que
	TPriQue<CScheduledTask> iTasks;
	CSchLogManager& iSchLogManager;
	};

#include "SCHCLI.INL"
#endif
