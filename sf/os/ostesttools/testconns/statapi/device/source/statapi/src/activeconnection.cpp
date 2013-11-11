/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "activeconnection.h"

CActiveConnection* CActiveConnection::NewL(const TDesC& ipAddress, TInt port)
{
	CActiveConnection* self=new(ELeave)CActiveConnection();
	CleanupStack::PushL(self);
	self->ConstructL(ipAddress,port);
	CleanupStack::Pop();
	return self;	
}

CActiveConnection::CActiveConnection(void) : CActive(0)
{
	;
}

void CActiveConnection::ConstructL(const TDesC& ipAddress, TInt port)
{
	iDstAddr.SetPort(port);		
    iDstAddr.Input(ipAddress);
    
	CActiveScheduler::Add(this);
}

CActiveConnection::~CActiveConnection()
{
	Cancel();
}

//Checks for user input to console.
void CActiveConnection::RunL()
{
	
	if(iStatus==KErrInUse)
	{
		Connect();
		SetActive();
	}

}

void CActiveConnection::DoCancel()
{
}

void CActiveConnection::Connect(TActiveConnectionMode aConnMode, TInt aConnIndex)
{
	//Wait 2 seconds before attempting to connect to NTRAS
	//It solves 8, randomly failing to connect to NTRAS

	User::After(KDelay);
	
#if defined(__WINS__)

    // WinTap IAP
	TUint32 lIAP = 0;
	
	_LIT16(KWIN_TAP, "WinTAP");
	
	/****************** CommsDat Soloution ************************
		CMDBSession* iDb = CMDBSession::NewL(CMDBSession::LatestVersion());
		CCDIAPRecord* ptrIAPRecord = static_cast<CCDIAPRecord *> (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
		
		TPtrC recordname(KWIN_TAP);
		ptrIAPRecord->iRecordName.SetMaxLengthL(recordname.Length());
		ptrIAPRecord->iRecordName = recordname;

		if(ptrIAPRecord->FindL(*iDb))
	    {
		    // Found a matching record
	//	    ptrIAPRecord->iService.LoadL(*iDb);
		    
		    delete ptrIAPRecord;  
	    }
	***************************************************************/


	if (aConnMode == EModeIAP && aConnIndex == 0)
	{
		// Open CommDB IAP table
		CCommsDatabase* lCommsDb = CCommsDatabase::NewL();
    	CleanupStack::PushL(lCommsDb);
		CCommsDbTableView* lIapTable = lCommsDb->OpenTableLC(TPtrC(IAP));
		
		TBuf<52> lIapName;
	
		// Point to the first entry
		User::LeaveIfError(lIapTable->GotoFirstRecord());
		lIapTable->ReadTextL(TPtrC(COMMDB_NAME), lIapName);

		if (lIapName.Find(KWIN_TAP) != KErrNotFound) 
		{
			lIapTable->ReadUintL(TPtrC(COMMDB_ID), lIAP);
		}
		else
		{
			// Point to the rest of the entries
			TInt err = KErrNone;
			while (err = lIapTable->GotoNextRecord(), err == KErrNone)
			{
				lIapTable->ReadTextL(TPtrC(COMMDB_NAME), lIapName);
				if (lIapName.Find(KWIN_TAP) != KErrNotFound) 
				{
					lIapTable->ReadUintL(TPtrC(COMMDB_ID), lIAP);
				}
	
			}
		}
		
		CleanupStack::PopAndDestroy(lIapTable);
    	CleanupStack::PopAndDestroy(lCommsDb);

		aConnIndex = lIAP;
	}

#endif

	// Create Connection with IAP
	TInt lErr = iSocketServer.Connect();
	if (lErr != KErrNone) 
		return;
	
	RConnection lConn;
    lErr = lConn.Open(iSocketServer);
    
    if (lErr == KErrNone && aConnIndex > 0)
    {
    	if (aConnMode == EModeIAP)
    	{
	    	// Set IAP Preferences
			TCommDbConnPref lPrefs;
			lPrefs.SetIapId(aConnIndex);
			lPrefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
			lErr = lConn.Start(lPrefs);
    	}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	else if (aConnMode == EModeSnap)
    	{
    		TConnSnapPref lPrefs(aConnIndex);
    		lErr = lConn.Start(lPrefs);
    	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	else
    	{
    		// use default mode
    		lErr = KErrNotSupported;
    	}

		if (lErr == KErrNone) 
		{
			RHostResolver lHostResolver;
			lErr = lHostResolver.Open(iSocketServer, KAfInet, KProtocolInetTcp, lConn);
			
			if (lErr == KErrNone) 
			{
				lErr = iSocket.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp, lConn);
			}
		}
    }
    
    if (aConnIndex == 0 || lErr != KErrNone)
    {
    	if(iSocket.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp) != KErrNone) {
    		return;
    	}
    }
   
    // Connect
	iSocket.Connect(iDstAddr, iStatus);
}

//Initial request
void CActiveConnection::Start(TActiveConnectionMode aConnMode, TInt aConnIndex)
{    
	Connect(aConnMode, aConnIndex);
	SetActive();
}

void CActiveConnection::CloseSocket()
{    
	iSocket.CancelConnect();
	iSocket.Close();
}
