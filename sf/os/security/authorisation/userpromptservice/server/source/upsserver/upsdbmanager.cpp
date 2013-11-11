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
* Implements the UPS database handle manager.  See class and function
* definitions for more information.
*
*/


/**
 @file
*/
#include <f32file.h>
#include "upscommon.h"
#include <ups/upsdbw.h>
#include "upsdbmanager.h"

namespace UserPromptService
{

RUpsDbHandleMaster::RUpsDbHandleMaster(RFs &aFs)
/**
	Construct the handle master

	This class should be used for synchronous database operations.

	If you need to do asynchronous operations then create/use an instance of the RUpsDbHandleSlave
	linked to the master.

	If your database operation fails, particularly due to OOM, then you should call Close your handle. You probably
	do NOT need to explicitly re-open it - it will automatically re-open when the -> operator is used.

	Note that the -> operator may leave.
*/
	: iFs(aFs), iDatabase(0), iClientListHead(0)
	{
	}

RUpsDbHandleMaster::~RUpsDbHandleMaster()
/**
	Destroy the handle master
*/
	{
	ASSERT(iClientListHead == 0); // Should be no clients!!!!
	Close(); // Deletes iDatabase...
	iClientListHead = 0;
	// Do NOT close the iFs (it is a reference to the main server's handle).
	}

_LIT(KDatabaseFile,"?:\\private\\10283558\\database\\ups.db");
void RUpsDbHandleMaster::OpenL()
/**
	Open the database handle

       It is probably clearer to explictly call this function to open the handle before first use, but the -> operator will
       auto-open it if called on a closed handle. Note that the -> operator can leave.

	If any database operation fails, particularly due to OOM, then you should Close your handle. Instead of 
	immediately re-opening it, it is generally safer to let it auto-reopen when the -> operator is next called.

*/
	{
	BULLSEYE_OFF
	if(iDatabase != 0)
		{
		// Close/delete the existing database handle.
		Close();
		}
	BULLSEYE_RESTORE

	// Open up a new handle.
	TFileName databaseFileBuf = KDatabaseFile();

	databaseFileBuf[0] = iFs.GetSystemDriveChar();
	TParsePtrC databaseFile(databaseFileBuf);

	// Make sure the dir exists
	(void)iFs.MkDirAll(databaseFile.DriveAndPath());
	// Create/Open the database
	iDatabase = CDecisionDbW::NewL(databaseFile.FullName(), iFs);
	}

void RUpsDbHandleMaster::Close()
/**
	Notify all client handles that the master (real) database handle is about to be closed, and then close it.
*/
	{
	// Notify all registered clients that we are about to delete the database handle
	RUpsDbHandleSlave *p = iClientListHead;
	while(p)
		{
		ASSERT(p->iClient != 0);
		p->iClient->DbHandleAboutToBeDeleted();
		p = p->iClientListNext;
		}

	// Delete database handle
	delete iDatabase;
	iDatabase = 0;
	}

TBool RUpsDbHandleMaster::IsOpen() const
/**
	Returns true if the handle is already open.
*/
	{
	return iDatabase != 0;
	}

CDecisionDbW *RUpsDbHandleMaster::operator->()
/**
	Returns the database handle so -> can be used on an RUpsDbHandleMaster instance to call database functions

	If the database is not already open then it calls OpenL.

	This operator CAN leave.
*/
	{
	if(iDatabase == 0)
		{
		OpenL();
		}
	return iDatabase;
	}

void RUpsDbHandleMaster::Register(RUpsDbHandleSlave *aClient)
/**
	Register new client with us.
*/
	{
	BULLSEYE_OFF
	if(aClient == 0)
		{
		return;
		}
	BULLSEYE_RESTORE
		
	aClient->iClientListNext = iClientListHead;
	iClientListHead = aClient;
	}

void RUpsDbHandleMaster::UnRegister(RUpsDbHandleSlave *aClient)
/**
	UnRegister client with us.
*/
	{
	RUpsDbHandleSlave **pp = &iClientListHead;
BULLSEYE_OFF
	while(*pp)
		{
BULLSEYE_RESTORE
		if(*pp == aClient)
			{
			// Found ptr to client, so remove it.
			*pp = (*pp)->iClientListNext;
			aClient->iClientListNext = 0;
			break;
			}
		pp = &(*pp)->iClientListNext;
		}
	}



RUpsDbHandleSlave::RUpsDbHandleSlave(RUpsDbHandleMaster & aMaster, MDbHandleClient * aClient)
/**
	Initialise new client and register it with the RUpsDbHandleMaster

	This class is intended for use when asynchronous database operations are being performed. These may then be failed
	due to another database operation failing, BUT the asycnhronous operation classes (eg. a db view) needs deleting
	before the actual database handle is deleted/re-created.
*/
	: iInUse(EFalse), iMaster(aMaster), iClient(aClient), iClientListNext(0)
	{
	ASSERT(aClient != 0);
	//RDebug::Printf("RUpsDbHandleSlave(%x, %x) - %x\n", &aMaster, aClient, this);
	}

RUpsDbHandleSlave::~RUpsDbHandleSlave()
	{
	//RDebug::Printf("~RUpsDbHandleSlave() master %x client %x - %x\n", &iMaster, iClient, this);
	Close();
	}

CDecisionDbW *RUpsDbHandleSlave::operator->()
/**
	Returns the database handle so -> can be used on an RUpsDbHandleMaster instance to call database functions

	If the database is not already open, then the master will open it.

	This operator CAN leave.
*/
	{
	if(!iInUse)
		{
		iMaster.Register(this);
		iInUse = ETrue;
		}
	return iMaster.operator->();
	}

void RUpsDbHandleSlave::SetCallback(MDbHandleClient *aClient)
/**
	Change what callback should be run if the master handle is closed.
	Do NOT set it to 0.
*/
	{
	ASSERT(aClient != 0);
	//RDebug::Printf("RUpsDbHandleSlave::SetCallback client %x - %x\n", aClient, this);
	iClient = aClient;
	}

void RUpsDbHandleSlave::Close()
/**
	Close this slave handle. This does NOT close the master, it just means we de-register with the
	master to we are no longer notified if the master is closed.
	It will automatically re-register if the -> operator is subsequently used.
*/
	{
	if(iInUse)
		{
		iMaster.UnRegister(this);
		iInUse = 0;
		}
	}

void RUpsDbHandleSlave::CloseMaster()
/**
	Notify all client handles that the master (real) database handle is about to be closed, and then close it.

	Typically used after a database error (particularly OOM) in an attempt to recover. Usually the database is not 
	explicitly re-opened, instead the re-open is defered until the next use of the -> operator on either tha master
	or a client handle.
*/
	{
		Close();
		iMaster.Close();
	}


} // End of UserPromptService namespace

// End of file
