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

#ifndef NCMIAPREADER_H
#define NCMIAPREADER_H

#include <e32base.h>

class MNcmIAPReaderObserver;
class TCommDbConnPref;

/**
 * IAP reader reads NCM IAP from CommDB and set it into NCM connection preference.
 */
NONSHARABLE_CLASS(CNcmIapReader) : public CActive
	{
public:

    /**
     * Constructor
     * @param[in]  aObserver, the observer of IAP reading progress. 
     * @param[in]  aConnection, the NCM connection preference.
     */    
    CNcmIapReader(MNcmIAPReaderObserver& aObserver, TCommDbConnPref& aConnPref);
    
    /**
     * Destructor
     */
	~CNcmIapReader();
	
	/**
	 * Start NCM IAP reader
	 */
	void Start();
	
private:
	
	// From CActive
	void RunL();
	void DoCancel();

private:
	void SearchNcmIapL();
	
private:
	MNcmIAPReaderObserver&    iObserver;
	TCommDbConnPref&       iConnPref;
	};

#endif // NCMIAPREADER_H

