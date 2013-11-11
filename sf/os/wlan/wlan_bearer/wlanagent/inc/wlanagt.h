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
* Description:  WLAN Agent class
*
*/

/*
* %version: 6 %
*/

#ifndef _WLANAGT_H_
#define _WLANAGT_H_

#include <comms-infras/nifagt.h>
#include <comms-infras/cstatemachineagentbase.h>

const TInt KMajorVersionNumber=0;
const TInt KMinorVersionNumber=0;
const TInt KBuildVersionNumber=0;

_LIT(KWlanAgentName,"wlanagt");

/**
 * WLAN agent factory
 * 
 *  @lib wlanagt.agt
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanAgentFactory ) : public CNifAgentFactory
    {
protected:

    /**
     * InstallL
     *
     * @since S60 v3.0
     */
    void InstallL();

    /**
     * NewAgentL
     *
     * @since S60 v3.0
     * @param aName of the agent
     * @return instance of the requested agent
     */
    CNifAgentBase* NewAgentL(const TDesC& aName);

    /**
     * Info
     *
     * @since S60 v3.0
     * @param aInfo
     * @param aIndex
     * @return 
     */
    TInt Info(TNifAgentInfo& aInfo, TInt aIndex) const;
    };

/**
 * WLAN agent
 * 
 *  @lib wlanagt.agt
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CWlanAgent ): public CStateMachineAgentBase
    {
public:

    /**
     * NewL
     *
     * @since S60 v3.0
     * @return instance of the requested agent
     */
    static CWlanAgent* NewL();

    /**
     * destructor
     */
    virtual ~CWlanAgent();

protected:

    /**
     * ConstructL
     *
     * @since S60 v3.0
     */
    void ConstructL();
    
    /**
     * constructor
     *
     * @since S60 v3.0
     */
    CWlanAgent();

public: // From CStateMachineAgentBase

    /**
     * Info
     *
     * @since S60 v3.0
     * @param aInfo
     */
    void Info(TNifAgentInfo& aInfo) const;

    /**
     * CreateAgentSML
     *
     * @since S60 v3.0
     * @param aObserver
     * @param aDlgPrc
     * @param aDb
     * @param aDir
     * @return instance of the requested agent statemachine
     */
    CAgentSMBase* CreateAgentSML(
        MAgentNotify& aObserver, 
        CDialogProcessor* aDlgPrc, 
        CCommsDbAccess& aDb, 
        TCommDbConnectionDirection aDir );

    /**
     * Retrieve the bearer information to the Agent CPR.
     *
     * @return Bearer information to the Agent CPR.
     */
    TUint32 GetBearerInfo() const;
    };

#endif // _WLANAGT_H_
