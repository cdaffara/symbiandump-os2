// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGSERVSECURITY_H__
#define __LOGSERVSECURITY_H__

#include <e32base.h>

class RMessage2;
class CLogServResourceInterpreter;

/**
The enum declares two basic types of operations performed on LogEngServer events
and their data - "read" and "write". Every time when a client tries to execute 
"read" and "write" operation for a specified event type, its capabilities have
to be asserted against a predefined set of read/write capabilities for that event type.
@internalComponent
*/
enum TEventOp 
	{
	EReadOp, 
	EWriteOp
	};

/**
This class manages security info against which client requests can be checked 
for the necessary capabilities.  An instance of this class is created in 
CLogServServer::ConstructL().  The security settings are defined in logwrap.rss
- see the sections entitled 'r_log_security' and 'r_log_initial_events'.
@internalComponent
*/
class CLogServSecurity : public CBase
	{
public:		
	static CLogServSecurity* NewL(CLogServResourceInterpreter& aResourceInterface);
	virtual ~CLogServSecurity();		
	virtual TBool IsAllowed(const RMessage2& aMsg, 
							TUid aEventType,
							TEventOp aEventOp, const char* aDiagnostic) = 0;

#ifdef LOGSERV_CAPABILITY_TEST
	virtual TSecurityPolicy SecurityPolicy(TUid aEventType, TEventOp aEventOp) = 0;
#endif //LOGSERV_CAPABILITY_TEST
	};
		
#endif //__LOGSERVSECURITY_H__
