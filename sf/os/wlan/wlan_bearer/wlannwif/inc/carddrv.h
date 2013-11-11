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
* Description:  Defines CardPktDrv class
*
*/

/*
* %version: 10 %
*/

#ifndef __CARDDRV_H__
#define __CARDDRV_H__

#include <e32base.h> 
#include <e32cmn.h> 

#include "802dot11.h" 
#include "RWlanLogicalChannel.h"
#include "EtherCardApi.h" 
#include <es_mbuf.h>

const TUint KWlanpktDrvMajorVersionNumber = 0;
const TUint KWlanpktDrvMinorVersionNumber = 0;
const TUint KWlanpktDrvBuildVersionNumber = 0;

const TInt KConfigBufferSize = 12;

class CSender;   
class CReceiver;  
class CLANLinkCommon; 


/**
*  CPcCardPktDrv
*
*/
class CPcCardPktDrv : public CBase 
	{                                     
public:                                    
    
    friend class CSender; 

    friend class CReceiver; 
    
    /**
     * NewL
     * @param aParent
     * @return 
     */
    static CPcCardPktDrv* NewL( CLANLinkCommon* aParent );

    /** 
     * constructor
     */
	CPcCardPktDrv();

    /** 
     * destructor
     */
	virtual ~CPcCardPktDrv();

    /** 
     * ConstructL
     * @param aParent
     */
	void ConstructL( CLANLinkCommon* aParent );

    /** 
     * StartInterface
     * @return 
     */
	TInt StartInterface();

    /** 
     * StopInterface
     * @return 
     */
	TInt StopInterface();

    /** 
     * ResetInterface
     * @return 
     */
	TInt ResetInterface();
	
    /** 
     * GetInterfaceAddress
     * @return 
     */
	TUint8* GetInterfaceAddress();

    /** 
     * Send
     * @param aPkt
     * @return 
     */
	TInt Send( RMBufChain& aPkt );
	
    /**
     * StartL
     */
    void StartL();

    /**
     * Stop
     */
    void Stop();
    
    /**
     * CardOpen
     * @return 
     */
    TBool CardOpen() {return iCardOpen;};
	
private: 
        
    /** */
    CLANLinkCommon* iParent; 
    
    /** */
    TBool iCardOpen;

    /** */
    TBuf8<KConfigBufferSize> iConfig;
    /** */
    CSender* iSender;
    /** */
    CReceiver* iReceiver;
    /** */
    RPcmNetCardIf iCard;
    
	};

#endif // __CARDDRV_H__
