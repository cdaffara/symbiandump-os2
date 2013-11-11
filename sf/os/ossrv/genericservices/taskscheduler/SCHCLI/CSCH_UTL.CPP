// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "CSCHCODE.H"
#include <e32math.h>
#include "SCHEXE.H"

// Constants
_LIT(KScheduleServerExe, "Schexe");
const TUid KTaskSchedulerExeUid = { 0x10005399 }; 

  
EXPORT_C TInt StartSch32()
	{
	TRequestStatus stat;
	const TUidType serverUid(KNullUid, KNullUid, KTaskSchedulerExeUid);
	RProcess server;
	TInt r=server.Create(KScheduleServerExe,KNullDesC,serverUid);
	if (r!=KErrNone)
		return r;

	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}
	


	
