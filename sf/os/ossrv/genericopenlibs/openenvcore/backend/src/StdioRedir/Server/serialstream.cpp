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
// Name        : serialstream.cpp
// Part of     : stdio server
// To handle the read and write on serial(comm)
// SerialStream implementation. Provides the implementation for opening, reading
// writting on the serial port.
// 
//

#ifdef __WINSCW__  
#define PDD_NAME _L("ECDRV")
#else
#define PDD_NAME _L("EUART1") //USE EUART1 FOR HARDWARE
#endif

#define LDD_NAME _L("ECOMM")

#include "serialstream.h"
#include "streambase.h"


_LIT8(KCRLF,"\r\n");
//-------------------------------------------------------------------------------
// Function Name : 	CSerialStreamFactory::NewL()
// Description   : 	Returns the constructed CSerialStream object.
//					
//-------------------------------------------------------------------------------	

CSerialStream* CSerialStream::NewL(TInt aBaudRate, TDesC& aPortName) 
	{
    // implements two phase construction
    CSerialStream* r=new(ELeave) CSerialStream(aBaudRate,aPortName);
    CleanupStack::PushL(r);
    r->ConstructL();
    CleanupStack::Pop();
    return r;
    }

//-------------------------------------------------------------------------------
// Function Name : 	CSerialStream::CSerialStream()
// Description   : 	Simple constructor
//					
//-------------------------------------------------------------------------------	

CSerialStream::CSerialStream(TInt aBaudRate, TDesC& aPortName) :
		iPortName(aPortName), iBaudRate(aBaudRate)
	{
    // nada
	}


//-------------------------------------------------------------------------------
// Function Name : 	CSerialStream::~CSerialStream()
// Description   : 	destructor
//					
//-------------------------------------------------------------------------------	

CSerialStream::~CSerialStream()
	{
    iPort.Close();
	}

//-------------------------------------------------------------------------------
// Function Name : 	CSerialStream::ConstructL()
// Description   : 	Loads the required Logical and physical devices fro Serial
//					port communication.Creates the SerialStream object held 
//					internally.
//					
//-------------------------------------------------------------------------------	

void CSerialStream::ConstructL()
    {
   	TInt err=0;
    err = User::LoadPhysicalDevice (PDD_NAME); 
    if ( err != KErrAlreadyExists && err)
    	User::Leave(err); 
  	err = User::LoadLogicalDevice (LDD_NAME);
  	if ( err != KErrAlreadyExists && err)
    	User::Leave(err);
  	StartC32();
    err = CStdioServer::CsSession().LoadCommModule(_L("ECUART"));
    if ( err != KErrAlreadyExists && err)
    	User::Leave(err);
    
   
   
    User::LeaveIfError( iOpenErr = iPort.Open(CStdioServer::CsSession(),iPortName,ECommShared) );
        
    TCommConfig config;
    iPort.Config(config);
            
    switch(iBaudRate)
    	{
    	case 4800:
    		config().iRate = EBps4800;
    		break;
    	case 7200:
    		config().iRate = EBps7200;
    		break;
    	case 9600:
    		config().iRate = EBps9600;
    		break;
    	case 19200:
    		config().iRate = EBps19200;
    		break;
    	case 38400:
    		config().iRate = EBps38400;
    		break;
    	case 57600:
    		config().iRate = EBps57600;
    		break;
    	case 115200:
    		config().iRate = EBps115200;
    		break;
    	case 230400:
    		config().iRate = EBps230400;
    		break;
    	default:
    		config().iRate = EBps115200;
    		break;
    	}
            	
    config().iParity = EParityNone;
    config().iHandshake = 0;
    iConfigErr = iPort.SetConfig(config);
 
    if ( iConfigErr != KErrNone )
        {
        iPort.Close();
        User::LeaveIfError(iConfigErr);
        }
    }




//-------------------------------------------------------------------------------
// Function Name : 	WriteL()
// Description   : 	static function to do actual writting on port.
//-------------------------------------------------------------------------------	

static void WriteL(RComm& aPort,const TDesC8& aDes)
    {
    TRequestStatus stat;
    aPort.Write(stat,aDes);
    User::WaitForRequest(stat);
    User::LeaveIfError(stat.Int());
    }
    
//-------------------------------------------------------------------------------
// Function Name : 	CSerialStream::WriteL()
// Description   : 	print the descriptor onto the Serial port
//					
//-------------------------------------------------------------------------------	

void CSerialStream::WriteL(TDes8& aDes)
    {
    // call the Serial Port WriteL() function to print the descriptor
    // onto the Serial port
    TPtrC8 p(aDes);
    // then we have a real port (the open port command did not fail)
    for (;;)
        {
        TInt x = p.Locate('\n');
        if ( x < 0 )
         	{
           	break;
           	}
        if ( x > 0 )
           	{
           	::WriteL(iPort,p.Left(x));
           	}
        ::WriteL(iPort,KCRLF);
        p.Set(p.Mid(x+1));
        }
            
    if ( p.Length() > 0 )
       	{
       	::WriteL(iPort,p);
       	}
   }
 

//-------------------------------------------------------------------------------
// Function Name : 	CSerialStream::Read()
// Description   : 	Handles the Read Request from a Client process
//					
//-------------------------------------------------------------------------------	

void CSerialStream::Read(TRequestStatus& aStatus, const TInt /*aLength*/)
 	{
    // internal buffer is empty so we read from the Serial port into a local buffer
    // as long as the port id is valid (> 0)
    iPort.ReadOneOrMore(aStatus, iBuf);
  	}
 	
//-------------------------------------------------------------------------------
// Function Name : 	CSerialStream::ReadCancel()
// Description   : 	Cancels any pending read requests
//					
//-------------------------------------------------------------------------------	

void CSerialStream::ReadCancel()
 	{
    // internal buffer is empty so we read from the Serial port into a local buffer
    // as long as the port id is valid (> 0)
    iPort.ReadCancel();
  	}
 	
 	

