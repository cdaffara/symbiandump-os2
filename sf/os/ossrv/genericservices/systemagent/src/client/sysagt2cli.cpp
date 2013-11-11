// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "SaCls.h"
#include "SaPrivate.h"

/** 
The function starts SysAgt2 server.

SysAgt2 server does not offer any functionality other than registering system 
software P&S properties. This method is redundant and not used by system 
software, hence it is deprecated and will be removed in a future release. It 
should also be considered an internal API not for use outside of Symbian even 
though it is classified as publishedAll. Its interface access scope was 
incorrectly set when introduced. 

@return KErrNone The server was started successfully. <BR>
        KErrAlreadyExists An instance of the server already exists. <BR>
        KErrGeneral The server panicked at the startup. <BR>
        Some other system-wide error codes.
@publishedAll
@deprecated Redundant method no longer used by system software 
*/
EXPORT_C TInt StartSysAgt2()
	{
	// SysAgt2Svr uid
	const TUid KSystemAgentExeUid = {0x10204FC5}; 
	const TUidType serverUid(KNullUid, KNullUid, KSystemAgentExeUid);
	RProcess server;
	TInt err = server.Create(KSystemAgentServerImageName, KNullDesC, serverUid);
	if(err != KErrNone)
        {
		return err;
        }
	TRequestStatus stat;
	server.Rendezvous(stat);
	if(stat != KRequestPending)
        {
		server.Kill(0);		// abort startup
        }
	else
        {
		server.Resume();	// logon OK - start the server
        }
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = server.ExitType() == EExitPanic ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}
