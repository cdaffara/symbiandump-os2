/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/*************************************************************************
 *
 * Switches
 *
 ************************************************************************/
#ifndef __STATCONSOLE_H__
#define __STATCONSOLE_H__

/*************************************************************************
 *
 * System Includes
 *
 ************************************************************************/
#include <e32base.h>
#include <e32cons.h>

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/
#include "stat_interfaces.h"

/*************************************************************************
 *
 * Definitions
 *
 ************************************************************************/

/*************************************************************************
 *
 * CStatConsole
 *
 ************************************************************************/
class CStatConsole : public CBase, public MNotifyUI
{
public:
	// Construction
	static CStatConsole *NewL( CConsoleBase *aConsole, MNotifyLogMessage *const aMsg );
	void ConstructL( CConsoleBase *aConsole, MNotifyLogMessage *const aMsg ); 
	~CStatConsole();

	// MNotifyUI
	void HandleStatusChange( TInt aSessionId, TCommStatus aNewStatus );
	void HandleError( TInt aError, void *aErrorData );
	void HandleInfo( const TDesC *aInfo );
	void UserExitRequest();

private:
	CConsoleBase *iConsole;
	TInt iUserExitRequest;

	MNotifyLogMessage *iMsg;
};

#endif //__STATCONSOLE_H__
