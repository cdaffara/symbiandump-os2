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
* Description: 
*    Defines class for interaction with WLMServer for Admission control purpose
*
*/

/*
* %version: 9 %
*/

#ifndef __NIFWLMSERVERIF_H__
#define __NIFWLMSERVERIF_H__

#include "WlanProto.h"
#include "rwlmserver.h"

/**
 * This class contains the information of a single access class
 * and includes methods for manipulating traffic streams of that
 * particular class. 
 */
NONSHARABLE_CLASS( CLANNifWLMServerPerAC ) : public CActive
    {

public:

    /**
     * Definitions for possible active object states.  
     */
    enum TActiveObjectContext
        {
        /** Traffic stream create request is ongoing. */
        ETSCreateRequest,
        /** Traffic stream inactivity timer is running. */
    	ETSDelTimer
        };

    /**
     * Factory method for creating an instance of CLANNifWLMServerPerAC.
     *
     * @param aAccessClass Access class definition for this instance.
     * @param aInactivityTime Inactivity time in microseconds after which
     *                        the current traffic stream will be deleted.
     * @param aIsAutomaticMgmt Whether automatic stream management is allowed.
     * @return Pointer to the created instance.
     */
    static CLANNifWLMServerPerAC* NewL(
        TWlmAccessClass aAccessClass,
        TUint aInactivityTime,
        TBool aIsAutomaticMgmt );

    /**
     * Destructor.
     */
    virtual ~CLANNifWLMServerPerAC();

    /**
     * Whether traffic is admitted on this access class.
     * 
     * @return ETrue if traffic is admitted, EFalse otherwise.
     */
    TBool IsAdmitted();

    /**
     * Set the traffic mode for this  access class.
     * 
     * @param aMode Traffic mode to set.
     */
    void SetTrafficMode(
        TWlmAcTrafficMode aMode );

    /**
     * Set the traffic status for this access class.
     *
     * @param aStatus Traffic status to set.
     */
    void SetTrafficStatus(
        TWlmAcTrafficStatus aStatus );

    /**
     * Suspend the inactivity timer.
     */
    void SuspendInactivityTimer();

    /**
     * Resume a suspended inactivity timer.
     */
    void ResumeInactivityTimer();    

    /**
     * Called by CLANLinkCommon when a packet is sent on this
     * particular access class.
     */
    void OnFrameSend();

    /**
     * Called by CLANLinkCommon when a packet has been received on this
     * particular access class.
     */
    void OnFrameReceive();

protected: // From CActive
	
    /**
     * From CActive.
     * Called by the active object framework when a request has been completed.
     */
	void RunL();

    /**
     * From CActive.
     * Called by the framework if RunL leaves.
     *
     * @param aError The error code RunL leaved with.
     * @return KErrNone if leave was handled, one of the system-wide error codes otherwise.
     */
    TInt RunError(
        TInt aError );	

    /**
     * From CActive.
     * Called by the framework when Cancel() has been called.
     */
	void DoCancel();

private:

    /**
     * Constructor.
     */
    CLANNifWLMServerPerAC(
        TWlmAccessClass aAccessClass,
        TUint aInactivityTime,
        TBool aIsAutomaticMgmt );

    /**
     * Second phase constructor.
     */
    void ConstructL();

private: // Data

	/**
	 * Handle to client API instance of WLAN Engine.
	 */
	RWLMServer iWlmServer;

	/**
	 * Contains the access class definition for this instance.
	 */
	const TWlmAccessClass iAccessClass;

	/** 
	 * Current traffic mode.
	 */
	TWlmAcTrafficMode iTrafficMode;

	/**
	 * Current traffic status.
	 */
	TWlmAcTrafficStatus iTrafficStatus;
	 
	/**
	 * Current Active Object context.
	 */
	TActiveObjectContext iContext;

	/**
	 * Whether a traffic stream has been created.
	 */
	TBool iIsTsCreated;
	   
    /**
     * ID of the current traffic stream.
     */
    TUint iTsId;

    /**
     * Traffic stream parameters.
     */
    TWlanTrafficStreamParameters iTsParams;
    
    /**
     * Status of the traffic stream.
     */
    TWlanTrafficStreamStatus iTsStatus;
    
    /** 
     * Inactivity time in microseconds after which the current traffic stream
     * will be deleted.
     */
    const TUint iTsInactivityTime;

	/**
	 * TS Deletion timer related variables.
	 */
	TTimeIntervalMicroSeconds32 iTsDelOrigTime;
	TTimeIntervalMicroSeconds32 iTsDelRemainTime;
	RTimer iTsDelTimer;
	TTime iTsDelStartTime;

	/**
	 * Whether automatic stream management is allowed.
	 */
	TBool iIsAutomaticMgmt;

    };

/**
 * This class implements the callback interface for
 * asynchronous notifications from WLAN engine.  
 */
NONSHARABLE_CLASS ( CLANNifWLMServerCommon ) :
    public CBase,
    public MWLMNotify
    {

public:

    /**
     * Constructor.
     */
	CLANNifWLMServerCommon(
	    CLANLinkCommon *aLinkCommon );

    /**
     * Destructor.
     */
	~CLANNifWLMServerCommon();

    /**
     * Second phase constructor.
     */
	void ConstructL();

    /**
     * Get the current traffic status for access classes.
     * 
     * @param aArray Traffic status for access classes.
     * @return KErrNone if information is available, an error otherwise. 
     */
    TInt GetAcTrafficStatus(
        TWlmAcTrafficStatusArray& aArray );

public:	// From MWLMNotify

    /**
     * From MWLMNotify.
     * The traffic mode of an access class has changed.
     *
     * @param aAccessClass Access class.
     * @param aMode Traffic mode of the access class.
     */
    void AccessClassTrafficModeChanged(
        TWlmAccessClass aAccessClass,
        TWlmAcTrafficMode aMode );

    /**
     * From MWLMNotify.
     * The traffic status of an access class has changed.
     *
     * @param aAccessClass Access class.
     * @param aStatus Traffic status of the access class.
     */
    void AccessClassTrafficStatusChanged(
        TWlmAccessClass aAccessClass,
        TWlmAcTrafficStatus aStatus );

private: // Data

    /** Handle to CLANLinkCommon object. Not owned by this pointer. */
	CLANLinkCommon* iLinkCommon;

	/** Handle to client API instance of WLAN Engine. */
	RWLMServer iWlmServer;

    };

#endif //__NIFWLMSERVERIF_H__
