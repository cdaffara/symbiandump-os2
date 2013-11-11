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

#include <e32base.h>
#include <obexconstants.h>

#include "simpleObexServer.h"
#include "simpleObexApp.h"
#include "obexAppConstants.h"


/**
 * Constructor.
 */
CObexServerHandler::CObexServerHandler(CActiveConsole* aParent)
:iAcceptPuts(ETrue)
    {
	iParent = aParent;
    iServer    = NULL;
    iObject    = NULL;
    }



/**
 * NewL function
 */
CObexServerHandler* CObexServerHandler::NewL(CActiveConsole* aParent, TTransport aTransport)
    {
    CObexServerHandler* self = new (ELeave) CObexServerHandler(aParent);

    CleanupStack::PushL(self);
    self->ConstructL(aTransport);
    CleanupStack::Pop();
    return (self);
    }


/**
 * 2nd phase constructor. This function performs the initial steps needed in 
 * creating a new CObexServer object. It then creates the CObexServer object for the specified
 * transport.
 *
 */
void CObexServerHandler::ConstructL(TTransport aTransport)
    {
	iTransportLayer = aTransport;
	if (aTransport == EBluetooth)
		{
		// Start bluetooth socket and set security
		RSocketServ socketServ;
		User::LeaveIfError(socketServ.Connect());
		RSocket listen;
		TInt test = listen.Open(socketServ, KRFCOMMDesC);

		if (test == KErrNone)
			{
			iParent->Console()->Printf(_L("Socket Open ... Success\n"));	
			}

		TBTSockAddr addr;
		// Auto bind to RFComm port
		addr.SetPort(KRfcommPassiveAutoBind);
		
		TBTServiceSecurity serviceSecurity;
			
		serviceSecurity.SetUid(KUidServiceSDP);
		serviceSecurity.SetAuthentication(EFalse);
		serviceSecurity.SetEncryption(EFalse);
		serviceSecurity.SetAuthorisation(EFalse);
		serviceSecurity.SetDenied(EFalse);
		addr.SetSecurity(serviceSecurity);
			
		TInt error;	
		// Set bluetooth protocol info
		TObexBluetoothProtocolInfo info;
		info.iAddr.SetPort(KObexTestPort);
		info.iTransport = KRFCOMMDesC;
		TRAP(error,iServer  = CObexServer::NewL(info));
		if (error)
			{
			iParent->Console()->Printf(_L("\r\n Could not create obex server! : error %d \r\n"),error);
			iParent->iMode = E_Inactive;
			iParent->iMode = E_Server;
			iParent->Cancel(); // cancel request for key
			}
		// Add services to the Service discovery database
			
		User::LeaveIfError(iSdp.Connect());
		User::LeaveIfError(iSdpdb.Open(iSdp));
		
		TSdpServRecordHandle ftphandle;

		iSdpdb.CreateServiceRecordL(ftpUuid, ftphandle);
		iSdpdb.UpdateAttributeL(ftphandle, KSdpAttrIdServiceName, KServerDesC);
				
		iProtDescList = CSdpAttrValueDES::NewDESL(0);

			iProtDescList
			->StartListL()
				->BuildDESL()
				->StartListL()
					->BuildUUIDL(KL2CAPUUID) 
				->EndListL()
				->BuildDESL()
				->StartListL()
					->BuildUUIDL(KRfcommUuid) 
						->BuildUintL(TSdpIntBuf<TInt8>(KObexTestPort))
				->EndListL()
				->BuildDESL()
				->StartListL()
					->BuildUUIDL(KObexProtocolUuid) 
				->EndListL()
			->EndListL();
		// Update attribute
		iSdpdb.UpdateAttributeL(ftphandle, KProtocolDescriptorListUUID, *iProtDescList); // put into both records
		delete iProtDescList;
		iProtDescList = NULL;
			
		User::LeaveIfError(listen.Bind(addr)); 
			
		TInt queueSize=2;
		User::LeaveIfError(listen.Listen(queueSize));
	
		iParent->Console()->Printf(_L("\n.....Now Listening.....\n"));
		
		}

	else if (aTransport == EIrda)
		{
		
		//creating the transport info
		TObexIrProtocolInfo transportInfo;
		transportInfo.iAddr.SetPort(KAutoBindLSAP);//default obex server for now
		transportInfo.iTransport = KObexIrTTPProtocol; 
		transportInfo.iClassName      = KIrdaClassName;					
		transportInfo.iAttributeName = KIrdaTransportAttrName;

  		iServer = CObexServer::NewL (transportInfo);
		
		}
	
    else if (aTransport == EWin32Usb)
		{
		// Create transport info
		TObexUsbProtocolInfo aInfo;
		aInfo.iTransport = KObexWin32UsbProtocol;
        aInfo.iInterfaceStringDescriptor = KServerInterfaceDescriptor;
		iServer = CObexServer::NewL (aInfo);
		
		}
   
   // Now that the transport has been selected and Obex Server started
   // We need to do some initialisation with Obex Objects so that we are 
   // prepared to deal with Put/Get requests from the client
    
    iObject  = CObexBufObject::NewL (NULL);

    iBuf = CBufFlat::NewL(KServerBufExpandSize);
  	
	iBuf->ResizeL(KServerBufExpandSize);// Resize the buffer

	iObject = CObexBufObject::NewL(NULL);
	iObject->SetDataBufL(iBuf);
	//Create the RFile to be used
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
	User::LeaveIfError(iFs.SetSessionToPrivate(EDriveC));
	User::LeaveIfError(iFs.SessionPath( iSessionPath ));
	// Start the server
	User::LeaveIfError(iServer->Start (this)); 
    
    }




/**
 * This function stops the OBEX server
 */
void CObexServerHandler::Stop()
    {
	if (iServer)
		{
		iServer->Stop();
		}
    }


/**
 * Destructor.
 */
CObexServerHandler::~CObexServerHandler ()
    {
    Stop();
	
	iSdpdb.Close();
	iSdp.Close();

	delete iObject;
    delete iBuf;
    delete iServer;
    }

/**
 * This function enables authentication on the OBEX server.
 * The password is defined in source.
 */
void CObexServerHandler::EnableAuthenticationL()
	{
	iChallengePassword = KAuthPassword;
	iServer->SetChallengeL(iChallengePassword);
	iIsAuthenticationEnabled = ETrue;
	}

/**
 * This function disables authentication on the server.
 */
void CObexServerHandler::DisableAuthentication()
	{
	iServer->ResetChallenge();
	iIsAuthenticationEnabled = EFalse;
	}





// MObexServerNotify interface functions

/**
 * Called if an OBEX protocol error occurs.
 */
void CObexServerHandler::ErrorIndication (TInt aError)
    {
    iParent->Console()->Printf(_L("Obex Server error %d"), aError);
    }

/**
 * Called when the underlying transport connection is made from a remote client to the server.
 */
void CObexServerHandler::TransportUpIndication ()
    {
    iParent->Console()->Printf(_L("\nTransport started\n"));
    }

/**
 * Called when the transport connection is dropped (by either party).
 */
void CObexServerHandler::TransportDownIndication ()
    {
    iParent->Console()->Printf(_L("\nTransport down\n"));
    }

/**
 * Called when an OBEX connection is made from a remote client.
 */
TInt CObexServerHandler::ObexConnectIndication(const TObexConnectInfo& aRemoteInfo, const TDesC8& /*aInfo*/)
    {
    
    iParent->Console()->Printf(_L("\r\nCObexServerHandler::ObexConnectIndication"));
	iParent->Console()->Printf(_L("\r\nConnected to machine with OBEX version %d.%d\r\n"), 
		     aRemoteInfo.VersionMajor (), aRemoteInfo.VersionMinor ());

     return (KErrNone);
    
    }

/**
 * Called on a (graceful) OBEX disconnection by the client.
 */
void CObexServerHandler::ObexDisconnectIndication (const TDesC8& /*aInfo*/)
    {
    iParent->Console()->Printf(_L("\r\nObex Disconnected\r\n\r\n"));
    }


/**
 * Called on receipt of the first packet of a (valid) put request.
 */
CObexBufObject* CObexServerHandler::PutRequestIndication ()
    {
    iParent->Console()->Printf(_L("Receiving object...\r\n"));
    iObject->Reset ();

	if ( iAcceptPuts)
		return (iObject);
	else
		return (NULL);
    }

/**
 * Called on receipt of every packet of an OBEX PUT operation.
 */
TInt CObexServerHandler::PutPacketIndication ()
    {
    
    TUint length = iObject->Length();

	TUint received = iObject->BytesReceived();
	TUint8 percent = 0;
    if (length > 0)
		{
		percent = TUint8((100 * received) / length);
		iParent->Console()->Printf(_L("\r%d %%      "), percent);
		}
    else
		{
		iParent->Console()->Printf(_L("\r%d      "), iObject->BytesReceived ());
		}
    return (KErrNone);
    
    }

/**
 * Called after the final put packet has been successfully received and parsed.
 */
TInt CObexServerHandler::PutCompleteIndication ()
    {
    TPtrC name=iObject->Name();
	TBuf<100> type;
	type.Copy(iObject->Type());
	iParent->Console()->Printf(_L("\r\nSuccessfully received '%S'\r\nType[%d]: '%S'\r\n"), &name, type.Length(), &type);

	TInt err = KErrNone;
	TBuf<80> filename(iSessionPath);
	filename.Append(iObject->Name());
	err = iObject->WriteToFile(filename);
    if (err == KErrAlreadyExists)
    	{
       	iParent->Console()->Printf(_L("\r\nWrite failed, File Already Exists\n"));
        }
    
	iObject->Reset ();
    return (err);
    }


/**
 * Called when a full get request has been received from the client.
 */
CObexBufObject* CObexServerHandler::GetRequestIndication (CObexBaseObject* aRequiredObject)
    {

	TRAPD(err,SetUpGetObjectL(aRequiredObject));
	if (err != KErrNone)
		{
		iParent->Console()->Printf(_L("\nSetUpGetObjectL() returned %d.\n"), err);
		return NULL;
		}
		
	return (iObject);
	
	}
	
	
/**
 * Called for every packet of get reply sent by the server back to the client.
 */
TInt CObexServerHandler::GetPacketIndication ()
    {
    if (iObject->Length () > 0)
		iParent->Console()->Printf(_L("\r%d %%      "), 
			 100 * iObject->BytesSent () / iObject->Length ());
    else
		iParent->Console()->Printf(_L("\r%d Bytes      "), iObject->BytesSent ());
    return (KErrNone);
    }

/**
 * Called when the final packet of the object has been returned to the client.
 */
TInt CObexServerHandler::GetCompleteIndication ()
    {
    iParent->Console()->Printf(_L("Obex Get Complete\r\n"));
    iObject->Reset ();
	return (KErrNone);
    }
    
    
    
/**
 * Called when an OBEX SETPATH command is received by the server.
 *
 */
TInt CObexServerHandler::SetPathIndication (const CObex::TSetPathInfo& aPathInfo, const TDesC8& /*aInfo*/)
    {
    iParent->Console()->Printf(_L("Obex SetPath request:\r\n"));
    iParent->Console()->Printf(_L("   --- Flags = '%d' - Constants = '%d' - "), aPathInfo.iFlags, aPathInfo.iConstants);
    
    if (aPathInfo.iNamePresent)
		iParent->Console()->Printf(_L("Name = %S\r\n"), &aPathInfo.iName);
	else
		iParent->Console()->Printf(_L("> No Name Present <\r\n"));
	
	iParent->Console()->Printf(_L("\nReturning success...!\n"));
	return (KErrNone);
    
    }

/**
 * Called when an abort packet is received from the client.
 */
void CObexServerHandler::AbortIndication ()
    {
    iParent->Console()->Printf(_L("Obex Operation aborted\r\n"));
    if(iObject)
		{
		// Determine if we have recieved any amount of the object
		// Return without notifing user if we haven't.
		if(!iObject->BytesReceived())
		    return;
		iParent->Console()->Printf(_L("\r\nWe have received part of an Obex object\r\n\r\n"));

		iObject->Reset();
		}
	}


/**
 * This is the function that sets up an object ready for a Get.
 * It takes as a parameter the requested object and then examines it's
 * name and if a filename matches then the object it setup from that file.
 */
void CObexServerHandler::SetUpGetObjectL(CObexBaseObject *aRequestedObject)
	{
	// Get the name of the requested object
	TBuf<200> name (aRequestedObject->Name ());
	if(name.Length())
		{
		iParent->Console()->Printf(_L("Obex Get Request for name '%s'\r\n"), name.PtrZ ());
		}

	else
		{
		iParent->Console()->Printf(_L("Obex Get Request unknown details\r\n"));
		User::Leave(KErrNotSupported);
		}
	iObject->Reset();

	RFs fs;
	RFile f;
	// Attempt to open the specified file
	if ((fs.Connect () != KErrNone) || 
		(f.Open (fs, name, EFileShareReadersOnly | EFileRead) != KErrNone))
		{
		iParent->Console()->Printf(_L("\r\nError reading '%S'."), &name);
		User::Leave(KErrNotFound);
		}
	// We can now proceed in setting up the object using the file retrieved
	
	TInt size = 0;
	// Set size of file
	User::LeaveIfError(f.Size (size));
	// Resize buffer object
	iBuf->ResizeL(size);
	// Get a pointer to represent the data
	TInt bufStart=0;
	TPtr8 data (iBuf->Ptr(bufStart));
	// Read data from file,
	f.Read (data);
	if (iBuf->Size() < size)
		User::Leave(KErrGeneral);
	// Set name of the object
	iObject->SetNameL(name);
	// Set length
	iObject->SetLengthL(size);
	TTime time;
	if (f.Modified(time) == KErrNone)
		iObject->SetTimeL(time);
	}
