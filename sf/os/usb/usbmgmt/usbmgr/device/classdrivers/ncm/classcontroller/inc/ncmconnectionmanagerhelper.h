/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
@internalComponent
*/

#ifndef NCMCONNECTIONMANAGERHELPER_H
#define NCMCONNECTIONMANAGERHELPER_H

#include <e32base.h>

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_essock.h>
#include <usb/testncmcc/dummy_insock.h>
#else
#include <es_sock.h>
#include <in_sock.h> 
#include <commdbconnpref.h>
#endif // OVERDUMMY_NCMCC

class MNcmConnectionObserver;

/**
 * CNcmConnectionManHelper is the handler for NCM connection.
 * 1 Start a NCM connection
 * 2 Cancel starting attempt
 * 3 Stop a NCM connection 
 */
NONSHARABLE_CLASS(CNcmConnectionManHelper) : public CActive
	{
public:
    /** 
     * Constructor
     * @param[in]  aObserver, IAP connection observer
     * @param[in]  aConnection, the NCM connection
     */ 
    CNcmConnectionManHelper(MNcmConnectionObserver& aObserver, RConnection& aConnection);

	~CNcmConnectionManHelper();
	void Start(TConnPref& aPref);
	void Stop();
	
private:
	
	// From CActive
	void RunL();
	void DoCancel();

private:
	MNcmConnectionObserver&	iObserver; // IAP connection observer
	RConnection&			iConnection; // The NCM connection	
	};

#endif // NCMCONNECTIONMANAGERHELPER_H

