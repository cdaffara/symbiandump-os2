/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines sender class for target
*
*/

/*
* %version: 11 %
*/

#ifndef _SENDER_HW_H_
#define _SENDER_HW_H_

#include <e32base.h>
#include <es_mbuf.h>

class CLANLinkCommon;
class CPcCardPktDrv;

/**
*  Sender active object
*
*/
class CSender : public CActive
    {

public:
    
    // NewL
	static CSender* NewL(CPcCardPktDrv* aParent);
	
	// destructor
	~CSender();

	/**
	* Send is called by upper layers to send data.
	* @param aPacket data packet to be sent
	* @return 0 if CSender isn't currently ready to accept more data,
	*         1 if CSender is ready to accept more data immediately
	*/
	TInt Send( RMBufChain& aPacket );

private:

	/**
	* C++ default constructor.
	*/
	CSender();

	/**
	* RunL
	*/
	virtual void RunL();

	/**
	* DoCancel
	*/
	virtual void DoCancel();

	/**
	* InitL
	* Initialises CSender
	* @param aParent CPcCardControlEngine parent class
	*/
	void InitL( CPcCardPktDrv* aParent );

	/**
	* Requests notification once Tx is once again allowed
	* sets the CSender active object as active.
	*/
	void RequestResumeTx();

	// Prohibit copy constructor
	CSender( const CSender& );
	// Prohibit assigment operator
	CSender& operator= ( const CSender& );

private:   // Data

    /** */
	CPcCardPktDrv* iParent;

    /** */
	TBool iStopSending;
	
	/** */
	TBool iReqPending;

   	};

#endif //_SENDER_HW_H_
