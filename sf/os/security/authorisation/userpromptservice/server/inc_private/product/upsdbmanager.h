/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Class to manage/recover the database handle
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef UPSDBMANAGER_H
#define UPSDBMANAGER_H
#include <e32base.h>

namespace UserPromptService
{

class RUpsDbHandleSlave;
class MDbHandleClient;
class CDecisionDbW;

NONSHARABLE_CLASS(RUpsDbHandleMaster)
	/**
	This class manages the CDecisionDbW handle used by the UPS.

	Code can either access the database via the master (ie. this class) -> operator, or can use an
	instance of the RUpsDbHandleSlave which provides for notification that the handle is about to be destroyed.

	Typically the user will not need to explicitly OpenL the handle, but should ensure that Close is
	called before the class instance is deleted.

	In practise this means that anything using an asynchronous database operation (ie. CreateViewL)
	should use the slave class.
	*/
	{
public:
	RUpsDbHandleMaster(RFs &aFs);
	virtual ~RUpsDbHandleMaster();

	void OpenL();
	void Close();

	TBool IsOpen() const;

	CDecisionDbW *operator->();
private:
	friend class RUpsDbHandleSlave;
	void Register(RUpsDbHandleSlave *aClient);
	void UnRegister(RUpsDbHandleSlave *aClient);

	RFs &iFs;
	CDecisionDbW *iDatabase;
	RUpsDbHandleSlave *iClientListHead;
	};

NONSHARABLE_CLASS(RUpsDbHandleSlave)
/**
	This class functions as a slave to the RUpsDbHandleMaster class, providing access to its
	database handle via the -> operator. This class supports a callback to notify the user that
	the database handle is about to be deleted, which allows the user to cleanup/delete
	open database views, or compaction sessions, before the handle is deleted.

	Typically the user will not need to explicitly OpenL the handle, but should ensure that Close is
	called before the class instance is deleted.

	If you are using asynchronous database operations (ie. CreateViewL , ) you should use
	this class.
*/
	{
public:
	RUpsDbHandleSlave(RUpsDbHandleMaster &aMaster, MDbHandleClient *aClient);
	~RUpsDbHandleSlave();

	void CloseMaster();

	CDecisionDbW *operator->();

	void SetCallback(MDbHandleClient *aClient);

	void Close();
private:
	friend class RUpsDbHandleMaster;

	TBool iInUse;
	
	RUpsDbHandleMaster &iMaster;
	MDbHandleClient *iClient;

	RUpsDbHandleSlave *iClientListNext;
	};

class MDbHandleClient
	{
public:
	virtual void DbHandleAboutToBeDeleted() = 0;
	};

} // End of UserPromptService namespace

#endif
