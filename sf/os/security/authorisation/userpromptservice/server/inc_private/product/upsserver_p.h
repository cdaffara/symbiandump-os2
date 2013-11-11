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
* Server-side classes which are used to implement the UPS server.
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef UPSSERVER_P_H
#define UPSSERVER_P_H

namespace UserPromptService
{
NONSHARABLE_CLASS(CSwiWatcher) : public CActive
/**
	This class is used to watch for SWI activity and to, if possible, unload our unused ECOM plugins
	so that we do not block the SWI operation which is being attempted.

	In particular ECOM may have open file handles on the RSC files and this will block uninstall.
*/
	{
public:
	static CSwiWatcher *NewL(CUpsServer &aUpsServer);
	~CSwiWatcher();
private:
	CSwiWatcher(CUpsServer &aUpsServer);
	void ConstructL();

	// Implement CActive virtuals
	virtual void RunL();
	virtual void DoCancel();
	
	CUpsServer &iUpsServer;
	RProperty iSwiProperty;
	};

} // End of namespace UserPromptService


#endif	// #ifndef UPSSERVER_H

