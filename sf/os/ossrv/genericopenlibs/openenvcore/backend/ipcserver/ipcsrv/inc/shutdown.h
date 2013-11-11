/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : shutdown.h
* Part of     : stdio server
* To implement the timer for transient server
* 
*
*/




#ifndef _SHUTDOWN_H_
#define _SHUTDOWN_H_

#include <e32base.h>//CTimer

//-----------------------------------------------------------------------
//Class name: 	CShutDown
//Description: 	Class to Implement the Timer so that stdio server can be 
//			    implemented as Transient Server. This class provides the API for
//			    the server to implement itself as Transient.
//-----------------------------------------------------------------------

class CShutDown : public CTimer
	{
public:
	static CShutDown* NewL();
	void ConstructL();
	void Start();
	
private:
	CShutDown();
	void RunL();
	};
	
#endif //_SHUTDOWN_H_
    
