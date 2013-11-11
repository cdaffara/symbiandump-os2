// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : nullstream.cpp
// Part of     : stdio server
//



#include "StdioServer.h"
#include "nullstream.h"


//
// -------------- NullStream implementation ------------------------
//

//-------------------------------------------------------------------------------
// Function Name :  CNullStream::NewL()
//-------------------------------------------------------------------------------	


CNullStream* CNullStream::NewL( ) 
	{
	CNullStream* r = new(ELeave) CNullStream();
	return r;
	}


//-------------------------------------------------------------------------------
// Function Name :  CNullStream::CNullStream()
// Description   :  Default Constructor.
//-------------------------------------------------------------------------------	

CNullStream::CNullStream() {

}

//-------------------------------------------------------------------------------
// Function Name :  CNullStream::~CNullStream()

//-------------------------------------------------------------------------------	

CNullStream::~CNullStream() {
	
}


//-------------------------------------------------------------------------------
// Function Name :  CNullStream::WriteL()
//-------------------------------------------------------------------------------	

void CNullStream::WriteL(TDes8& /*aDes*/ ) 
	{
	
	}

//-------------------------------------------------------------------------------
// Function Name :  CNullStream::Read()
// Description   :  always returns EOF.
//-------------------------------------------------------------------------------	
	
void CNullStream::Read(TRequestStatus& aStatus, const TInt /*aLength */ ) 
	{
	if(!iBuf.Length())
		iBuf.Zero();
	//with err code returned KErrEof 
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrEof);
	return;
	}
		
 



