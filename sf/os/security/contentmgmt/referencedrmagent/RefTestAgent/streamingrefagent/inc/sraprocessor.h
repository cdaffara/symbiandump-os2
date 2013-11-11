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
* Defines the SRA processor class which listens to a pre-determined socket
* and processes recieving keys.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRAPROCESSOR_H
#define SRAPROCESSOR_H

#include <es_sock.h>
#include <in_sock.h>
#include <lib_pfkey.h>

class CSraSession;

class CSraProcessor : public CActive
/**
	An instance of this class processes all received keys. 
 */
	{
	/** Defines the states of the SRA processor. */
	enum TProcessorState
		{
		EIdle,			///< The initial state.
		EListening,		///< The processor is waiting to receive a key.
		EProcessing,	///< The processor is processing a received key.
		EFinish			///< The received key stream has finished. 
		};
		
public:
	static CSraProcessor* NewL(CSraSession& aSession, const TDesC& aListenAddr, TUint aListenPort);
	void StartProcessing();
	TProcessorState ProgressStatus();
				
	~CSraProcessor();
				
protected:
	CSraProcessor(CSraSession& aSession);
	//Inherited pure virtual methods of CActive
	void RunL();
	void DoCancel();
			
private:
	void ConstructL(const TDesC& aListenAddr, TUint aListenPort);
	void Listen();
	void ProcessL();
	void Finish();
	void SelfComplete();
	
private:
	CSraSession& iSession;
	TProcessorState iState;
	RSocketServ iSocketServ;
	RConnection iConnection;
	RSocket iClientSocket;
	RBuf8 iBuffer;
	};

#endif