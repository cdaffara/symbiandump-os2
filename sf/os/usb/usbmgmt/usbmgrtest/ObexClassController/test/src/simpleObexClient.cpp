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

#include <bautils.h>
#include <obexconstants.h>
#include "simpleObexClient.h"
#include "simpleObexApp.h"
#include "obexAppConstants.h"



/**
 * Constructor, ownership of Console is passed.
 */
CObexClientHandler::CObexClientHandler(CActiveConsole* aParent)
    : CActive(CActive::EPriorityStandard),
      iParent(aParent)
    {
    iClient    = NULL;
    iObject    = NULL;
    iState     = EIdle;
    }


/**
 * NewL function.
 */
CObexClientHandler* CObexClientHandler::NewL(CActiveConsole* aParent, TTransport aTransport)
    {
    CObexClientHandler* self = new (ELeave) CObexClientHandler(aParent);

    CleanupStack::PushL (self);
    self->ConstructL(aTransport);
    CActiveScheduler::Add (self);
    CleanupStack::Pop ();
    return (self);
    }


/**
 * 2nd phase constructor. Responsible for determining which
 * transport has been selected and to create a new OBEX Client.
 */
void CObexClientHandler::ConstructL(TTransport aTransport)
    {
	
	// Construct client if bluetooth selected
	if (aTransport == EBluetooth)
		{
		iDevAddr = devAddr;  // take the Bluetooth address

		//start the SDP Query
		delete iSdpServiceFinder;
		iSdpServiceFinder=0;
		iSdpServiceFinder = CRFCOMMServiceFinder::NewL(ftpUuid, iDevAddr, *this);
		// Start search process
		iSdpServiceFinder->FindPortL();
		iParent->Console()->Printf(_L("\nSearching for SDP service....\n"));
		}
	else if (aTransport == EIrda)
		{
		// Create transport info
		TObexIrProtocolInfo transportInfo;
		//assigning the unique name for the irda transport
		transportInfo.iTransport = KObexIrTTPProtocol; 
		//assigning irda specific attributes
		transportInfo.iClassName      = KIrdaClassName;					
		transportInfo.iAttributeName = KIrdaTransportAttrName;
		
  		iClient = CObexClient::NewL (transportInfo);	//create the obex client
		iParent->iMode = E_Client;
		}
	else if (aTransport == EUsb)
		{
		// Create transport info
		TObexUsbProtocolInfo aInfo;
		aInfo.iTransport = KObexUsbProtocol;
		aInfo.iInterfaceStringDescriptor = KClientInterfaceDescriptor;

	    iClient = CObexClient::NewL (aInfo);// Create the CObexClient
		}
	else if (aTransport == EWin32Usb)
		{
		// Ensure win32usb Obex transport plugin exists
		// Create transport info
		TObexUsbProtocolInfo aInfo;
		aInfo.iTransport = KObexWin32UsbProtocol;
		aInfo.iInterfaceStringDescriptor = KClientInterfaceDescriptor;

	    iClient = CObexClient::NewL (aInfo);
		}
	else
		{
		User::Invariant();
		}

	// Setup Obex objects
	iFileObject = CObexFileObject::NewL();
	iObjectBuffer = CBufFlat::NewL(KBufExpandSize);
    iObject = CObexBufObject::NewL(iObjectBuffer);
    iObexName = KNullDesC;
	iGetType = KObjectType;
	
	// Create file session and create a new private path on drive C
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	
	User::LeaveIfError(fileSession.CreatePrivatePath(EDriveC));
	User::LeaveIfError(fileSession.SetSessionToPrivate(EDriveC));	
	User::LeaveIfError(fileSession.SessionPath(iSessionPath));
	
	// Copy the files from oby file ro the newly created path
	User::LeaveIfError(BaflUtils::CopyFile(fileSession, KFilePath1, KFilename1,0));
	User::LeaveIfError(BaflUtils::CopyFile(fileSession, KFilePath2, KFilename2,0));
	User::LeaveIfError(BaflUtils::CopyFile(fileSession, KFilePath3, KFilename3,0));
 
	fileSession.Close();
	
	// Assign file names
	iFilename1 = KFilename1; 
	iFilename2 = KFilename2;
	iFilename3 = KFilename3;

	
    }


/**
 * Destructor.
 */
CObexClientHandler::~CObexClientHandler()
    {
    // do cleanup
    Cancel();
    delete iObject;
	delete iFileObject;
    delete iClient;
	delete iObjectBuffer;
	delete iTargetHeaderObject;
	delete iSdpServiceFinder;
    }
    
/**
 * This function is called when the SDP search finishes.
 * If search was successful then the OBEX port number is
 * taken an used in starting the OBEX client.
 * If unsuccessful, go back to the initial menu.
 */
void CObexClientHandler::SearchResult(TInt aError, TUint8 aPort)
	{
	// Service not supported?
	if (aError != KErrNone)
		{
		iParent->Console()->Printf(_L("\r\n Could not find SDP service in remote device : error %d \r\n"),aError);	
		iParent->iMode = E_Inactive;
		iParent->Cancel();
		}
	
	
	// Set protocol info for bluetooth
		
	TObexBluetoothProtocolInfo aInfo;
	aInfo.iAddr.SetBTAddr(iDevAddr);
	aInfo.iAddr.SetPort(aPort); 
	aInfo.iTransport = KObexRfcommProtocol;
	//now create the obex client...
	TRAPD(error,iClient = CObexClient::NewL (aInfo));
	if (error)
		{
		iParent->Console()->Printf(_L("\r\n Could not create client! : error %d \r\n"),error);
		iParent->iMode = E_Inactive;
		iParent->Cancel();
		}
		
	iParent->Console()->Printf(_L("\nSDP search complete OK!\n"));
	iParent->iMode = E_Client;
	
	iParent->RequestCharacter();

	}



/**
 * This function performs the actual connection of client to server.
 */

void CObexClientHandler::Connect()
    {
    if(IsActive())
		{
		iParent->Console()->Printf(KAlreadyActive);
		return;
		}
   
    TObexConnectInfo iLocalInfo = iClient->LocalInfo();
    iLocalInfo.iWho = KNullDesC8;
    iLocalInfo.iWho = EPOCIDENT;  
    iLocalInfo.iWho.Append(KLocalInfoAppend);
	// Connect to Obex client
    iClient->Connect(iStatus);
    SetActive();
    iState = EConnecting;
    }

    
/**
 * This fucntion performs the disconnection from OBEX client and server
 */    
void CObexClientHandler::Disconnect()
    {
    if(IsActive())
		{
		iParent->Console()->Printf(KAlreadyActive);
		return;
		} 
    // Disconnect from Obex client
	iClient->Disconnect(iStatus);
    SetActive();
    iState = EDisconnecting;
    }


/**
 * This function is called when an OBEX get is requested by name.
 * It takes the current name of the OBEX object, replaces it with
 * a user defined name. it then performs a Get on this object name. 
 */
void CObexClientHandler::GetByNameL()
    {
    if(IsActive())
		{
		iParent->Console()->Printf(KAlreadyActive);
		return;
		}
	
    iObject->Reset ();
    // Call SetName passing current name
    // This will set a newly selected name from the user
	SetName(iObexName);
	// Set the name
    iObject->SetNameL (iObexName);
    // Call get with required object
    iClient->Get(*iObject, iStatus);
    SetActive();
    iState = EGetting;
    }



/**
 * This function is a wrapper for the CObexClient::Put function.
 * It is responsible for setting up a new Obex object from a specified file.
 * It then performs the Put operation.
 */    
void CObexClientHandler::Put(TDes& aFilename)
    {
    if(IsActive())
		{
		iParent->Console()->Printf(KAlreadyActive);
		return;
		}
		
	// Setup the object from file
	TInt err = SetUpObjectFromFile (aFilename);
	// Advise user if unsuccessful
	if( err != KErrNone)
		{
		iParent->Console()->Printf(_L("\r\n Couldnt set up object : error %d \r\n"),err);
		Disconnect();
		return;
		}
	// Call the put	
	iClient->Put(*iFileObject,iStatus);
	SetActive();
	iState = EPutting;
	}


/**
 * This function is responsible for initiating a 
 * client/server connection with authentication.
 * The password is set in source.
 */
void CObexClientHandler::ConnectWithAuthenticationL()
	{
    if(IsActive())
		{
		iParent->Console()->Printf(KAlreadyActive);
		return;
		}
	// Set generic password
    iChallengePassword = KAuthPassword;
    // Connect client to server sending password
    iClient->ConnectL(iChallengePassword, iStatus);
    // Active the active object
    SetActive();
    iState = EConnecting;
	}
	
/**
 * This function is called from within the GetByName function.
 * It takes in the current object name and changes it to a user
 * defined alternative.
 */
void CObexClientHandler::SetName(TDes& aName)
    {
	TBuf<64> oldName;
	oldName = aName;

	TKeyCode aCode;
	TBuf<1> aChar;
	iParent->Console()->Printf(_L("\nEnter a name: %S"),&aName);
	FOREVER
		{
		aCode = iParent->Console()->Getch();
		aChar.SetLength(0);
		aChar.Append(aCode);

		iParent->Console()->Printf(_L("%S"),&aChar);
	
		// If <CR> finish editing string
		if (aCode ==  EStdKeyDelete)
			break;
		
		// if <BS> remove last character
		if ((aCode == EStdKeyHome)&&(aName.Length() != 0))
			aName.SetLength((aName.Length()-1));
		else
			aName.Append(aCode);
		}
	iParent->Console()->Printf(_L("\n"));


    }



/**
 * This is the active object RunL function.
 * When the function is called the currently selected mode of operation
 * is examined and then dealt with accordingly.
 * It displays the active object iStatus current state to the user.
 */
void CObexClientHandler::RunL ()
    {
    if (iStatus != KErrNone)
		{// Handle error
		}

    TBuf<80> filename;
    switch (iState)
		{
		// If we were connecting, and iStatus has completed then we want to change iState to EConnected
		case EConnecting:
			iParent->Console()->Printf(_L("\r\nConnect completed with error code: %d\r\n\r\n"),iStatus.Int());
			iState = EConnected;
			break;
		// If we were performing a put and the iStatus has completed change iState to EConnected
		case EPutting:
			iState = EConnected;
			iParent->Console()->Printf(_L("\r\nPut completed with error code: %d\r\n\r\n"),iStatus.Int());
			break;
		// If we were performing a get and the iStatus has completed change iState to EConnected
		// Also we want to write the received object to file
		case EGetting:
			iState = EConnected;
			iParent->Console()->Printf(_L("\r\nGet completed with error code: %d\r\n\r\n"),iStatus.Int());
			DisplayObjectInformation();
			filename = iSessionPath;
			filename.Append(iObject->Name());		
			// Write objects data to file, test if the file already exists as well
			if (iObject->WriteToFile(filename) == KErrAlreadyExists)
				{
				iParent->Console()->Printf(_L("\r\nWrite failed, File Already Exists\n"));
				}
			iObject->Reset ();
			break;
		// If we wanted to disconnect and iStatus has completed, then go back to original state 
		case EDisconnecting:
			iParent->Console()->Printf(_L("\r\nDisconnect completed with error code: %d\r\n\r\n"),iStatus.Int());
			iState = EIdle;
		
		default:
			iParent->Console()->Printf(_L("\r\nTest Code is in an incorrect state: %d\r\n\r\n"),iState);
			break;
		}
   
    }

/**
 * This is the active object DoCancel function.
 */
void CObexClientHandler::DoCancel()
    {
    delete iClient;
    iClient = NULL;
    }
    
    
/**
 * DisplayObjectInformation prints information of the received object.
 */    
void CObexClientHandler::DisplayObjectInformation()
    {
    // Display Contents of CBufFlat data on current console
	iParent->Console()->Printf(_L("Size of received object = %d\n"),iObjectBuffer->Size());
	TDateTime dt = iObject->Time().DateTime();
	iParent->Console()->Printf(_L("\r\nTimestamp: %d/%d/%d, %d:%d:%d\r\n\r\n"),
				   dt.Day(), dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());

    TBuf8<1024> tempBuffer;
	iObjectBuffer->Read(0, tempBuffer, tempBuffer.MaxSize() < iObjectBuffer->Size() ? tempBuffer.MaxSize() : iObjectBuffer->Size());
	// Printf fails with Descriptor bigger than X hundred bytes so write byte at a time
	for(TInt count = 0; count < tempBuffer.Size(); count++) 
		{
		iParent->Console()->Printf(_L("%C"),tempBuffer[count]);
		}
    }



/**
 * This function is called when a Put operation has started.
 * It takes a filename and then attempts to setup an OBEX object
 * from that file. Returns the appropriate error code, if the  
 * file didn't exist then this function creates a new file of
 * the same name. 
 */
TInt CObexClientHandler::SetUpObjectFromFile(TDes& aFilename)
    {
    // Try to create CObexFileObject with filename 
	TRAPD (err, iFileObject->InitFromFileL (aFilename));
	if (err != KErrNone)
		{
		RFs fs;
		RFile f;
		err = fs.Connect();
		if (err== KErrNone)
			{
			err = fs.CreatePrivatePath(EDriveC);
		 	}
		 
		if (err== KErrNone || err == KErrAlreadyExists )
			{
			err = fs.SetSessionToPrivate(EDriveC);
			}
			
		if (err == KErrNone)
			{
			// File didn't exist so create a file of same name in it place	
			err = f.Create (fs, aFilename, EFileShareExclusive | EFileWrite);
			}
			
		if (err != KErrNone)
			{
			iParent->Console()->Printf(_L("\r\nError reading '%s'.\r\nI tried to create this file for you, but failed to do that too. Sorry.\r\n\r\n"), aFilename.PtrZ ());			
			}
		else
			{
			f.Write (_L8("Test file for sending from EPOC\r\n\r\nLooks like obex is sending OK!!\r\n"));
			f.Close ();
			iParent->Console()->Printf(_L("\r\nFile '%s' did not exist, so I've created one.\r\nPlease try again.\r\n\r\n"), aFilename.PtrZ ());	
			}
		fs.Close ();
		
		}
	
	return (err);

    }


    
/**
 * NewL function for CRFCOMMServiceFinder 
 */

CRFCOMMServiceFinder* CRFCOMMServiceFinder::NewL(	const TUUID& aServiceClass,
							const TBTDevAddr& aDevAddr,
							MRFCOMMServiceSeeker& aSeeker)
	{
	CRFCOMMServiceFinder* self= new (ELeave) CRFCOMMServiceFinder(aSeeker);
	CleanupStack::PushL(self);
	self->ConstructL(aDevAddr, aServiceClass);
	CleanupStack::Pop();
	return (self);
	}

/**
 * Destructor.
 */
CRFCOMMServiceFinder::~CRFCOMMServiceFinder()
	{
	delete iPattern;
	delete iAgent;
	}

/**
 * Constructor.
 */	
CRFCOMMServiceFinder::CRFCOMMServiceFinder(MRFCOMMServiceSeeker& aSeeker)
: iSeeker(aSeeker)
	{
	}

/**
 * 2nd phase constructor. Sets the seach pattern for SDP with the UUID supplied.
 * Then Creates a new SDP agent with the device address supplied.
 */
void CRFCOMMServiceFinder::ConstructL(const TBTDevAddr& aDevAddr, const TUUID& aServiceClass)
	{
	iPattern=CSdpSearchPattern::NewL();
	iPattern->AddL(aServiceClass);
	iAgent=CSdpAgent::NewL(*this, aDevAddr);
	// Filter out classes that arent FTP
	iAgent->SetRecordFilterL(*iPattern);
	}

/**
 * This function is responsible for calling the SDP agent NextRecordRequestL
 * function, this gets the next service record that the device has registered
 * in the SDP database.
 */
void CRFCOMMServiceFinder::FindPortL()
	{
	iPort=KNotAPort;	//0xFF will never be returned from a query, 
			//because RFCOMM server channels only
			//go up to 30.
	
	iAgent->NextRecordRequestL();
	}


/**
 * This function is from the MSdpAgentNotify interface and is called when there
 * are no more SDP records registered in the database. It calls SearchResult implemented 
 * by this class.
 */
void CRFCOMMServiceFinder::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt /*aTotalRecordsCount*/) 
	{
	if(aError)
		iSeeker.SearchResult(aError,KNoPort);	
	else
		{
		//We have the record, kick off the attribute request
		TRAPD(err,iAgent->AttributeRequestL(aHandle,KSdpAttrIdProtocolDescriptorList )); 
		if(err)
			iSeeker.SearchResult(err,KNoPort);
		}
	}


	
/**
 * This function comes from the MSdpAgentNotifer interface, It is called when
 * the attribute request process has finished.
 */
void CRFCOMMServiceFinder::AttributeRequestComplete(TSdpServRecordHandle /*aHandle*/, TInt aError)
	{
	if(aError!=KErrNone || iPort==KNotAPort)
		iSeeker.SearchResult(aError?aError:KErrNotFound,KNoPort);
	else
		iSeeker.SearchResult(KErrNone, iPort);
	}

 


