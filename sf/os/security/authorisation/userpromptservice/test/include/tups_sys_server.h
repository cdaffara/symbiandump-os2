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
*
*/


/**
 @file  
 @test 
*/
 
#if (!defined __UPS_TEST_SERVER_H__)
#define __UPS_TEST_SERVER_H__

#include <e32base.h>
#include <test/tefexportconst.h>
#include <ups/upsclient.h>
#include "tupstesteractive.h"
#include "tups_clientapi_sysserver.h"


class CUPSTestServer : public CServer2
	{
public:
	static CUPSTestServer* NewL(const TDesC& serverName);

	IMPORT_C virtual ~CUPSTestServer();
	IMPORT_C CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	IMPORT_C virtual void ConstructL(const TDesC& aName);
	void SessionClosed();
	inline const TDesC& Name() const {return iServerName;};
protected:
	IMPORT_C CUPSTestServer();
	inline void IncSessionCount() {++iSessionCount;};
protected:
public:
	TInt	iSessionCount;
	TBuf<KMaxTestExecuteNameLength> iServerName;
	
public:
	UserPromptService::RUpsSession iUpsSession;
	
	
	};

class CUPSTestSession : public CSession2

	{
public:

	static CUPSTestSession* NewL(const CUPSTestServer *aServer, const RMessage2& aMessage);
	virtual void ServiceL(const RMessage2& aMessage);
	
	void CTUPSTesterActiveComplete(CTUPSTesterActive *aTester);
private:
	void ConstructL(CUPSTestServer *aServer, const RMessage2& aMessage);

	IMPORT_C CUPSTestSession();
	IMPORT_C virtual ~CUPSTestSession();

    void makeRequestToUPsL(const RMessage2& aMessage);
    
    CTUPSTesterActive *iUpsTestActiveObj;
    
public:
 	UserPromptService::RUpsSubsession 	iUpsSubsession;
 	TUpsTestServiceRequestData 			iRequestData;

	};

#endif
