/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines receiver class for target
*
*/

/*
* %version: 8 %
*/

#ifndef _RECEIVER_HW_H_
#define _RECEIVER_HW_H_

#include <e32base.h>

class CLANLinkCommon;
class CPcCardPktDrv;
class TDataBuffer;

/**
*  Receiver active object
*
*/
class CReceiver : public CActive
    {
public:  // Methods

    // NewL
	static CReceiver* NewL( CPcCardPktDrv* aParent );

    // destructor
	virtual ~CReceiver();

	/**
	* Requests a new data frame from LDD. Calls SetActive().
	*/
	void QueueRead();

private: // Methods
	/**
	* C++ constructor
	* @param aParent
	*/
	CReceiver( CPcCardPktDrv* aParent );

	/**
	* RunL
	*/
	void RunL();

	/**
	* DoCancel
	*/
	void DoCancel();

	/**
	* Symbian second phase constructor
	*/
	void ConstructL();

	// Prohibit copy constructor
	CReceiver(const CReceiver&);
	// Prohibit assigment operator
	CReceiver& operator= (const CReceiver&);

private:    // Data

    /** */
    CPcCardPktDrv* iParent;

    /** */
    TDataBuffer* iDataBuffer;
    
	/** Rx frame to free */
    TDataBuffer* iFrameToFree;

    };

#endif //_RECEIVER_HW_H_
