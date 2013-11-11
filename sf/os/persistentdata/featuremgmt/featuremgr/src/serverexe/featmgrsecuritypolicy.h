/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FEATMGR_SECURITY_POLICY_H
#define FEATMGR_SECURITY_POLICY_H


/**
 * Number of different IPC ranges
 */
const TUint KFeatMgrPlatSecRangeCount = 3;


#ifndef EXTENDED_FEATURE_MANAGER_TEST
//
// NORMAL
// The security ranges for messages in the framework.
// See also featmgrclientserver.h for message enum TFeatMgrServRequest
//

/**
 * IPC ranges segmented into groups based on their capability requirements
 */
const TInt KFeatMgrPlatSecRanges[KFeatMgrPlatSecRangeCount] =
    {
    0,  // IPC 0-7 Always passed
    8,  // IPC 8-19 WriteDeviceData
    20  // IPC Not supported: 20 = 16 messages + 4 "resource check" messages
    };


/**
 * IPC segments and their capability requirements
 */
const TUint8 KFeatMgrPlatSecElementsIndex[KFeatMgrPlatSecRangeCount] =
    {
    CPolicyServer::EAlwaysPass,             // IPC 0-3
    0,                                      // IPC 8-19
    CPolicyServer::ENotSupported            // IPC 20 -> 
    };

#else
//
// CONFIGURED test server has debug only API(s) built into the framework. 
// e.g. EFeatMgrNumberOfNotifyFeatures which will return debug information about 
// the server for testing purposes. Consequently, the platform security setup 
// for the test server differs slightly to allow these specific debug only message(s).
//

/**
 * IPC ranges segmented into groups based on their capability requirements
 */
const TInt KFeatMgrPlatSecRanges[KFeatMgrPlatSecRangeCount] =
    {
    0,   // IPC 0-9 Always passed
    10,  // IPC 10-21 WriteDeviceData
    22   // IPC Not supported 22 ->
    };
/**
 * IPC segments and their capability requirements
 */
const TUint8 KFeatMgrPlatSecElementsIndex[KFeatMgrPlatSecRangeCount] =
    {
    CPolicyServer::EAlwaysPass,             // IPC 0-3
    0,                                      // IPC 10-21
    CPolicyServer::ENotSupported            // IPC 22 ->
    };

#endif


/**
 * Capability sets and the action performed in case of failure
 */
const CPolicyServer::TPolicyElement KFeatMgrPlatSecElements[] =
    {
       
        {
        _INIT_SECURITY_POLICY_C1( ECapabilityWriteDeviceData ),
        CPolicyServer::EFailClient
        }
       
    };


/**
 * Platform security policy that Feature Manager server uses.
 */
const CPolicyServer::TPolicy KFeatMgrPlatSecPolicy =
    {
    CPolicyServer::EAlwaysPass,     // Specifies all connect attempts should pass
    KFeatMgrPlatSecRangeCount,      // Number of IPC ranges
    KFeatMgrPlatSecRanges,          // IPC Ranges
    KFeatMgrPlatSecElementsIndex,   // Policy to IPC mapping array
    KFeatMgrPlatSecElements         // Policies used in Feature Manager Server
    };


#endif
