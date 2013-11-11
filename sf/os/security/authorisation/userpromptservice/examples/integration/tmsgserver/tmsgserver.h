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
* Example code
*
*/


/**
 @file
 @internalComponent
 @test
*/

#ifndef TMSGSERVER_H_
#define TMSGSERVER_H_

#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>
#include <ups/upsclient.h>


// Standard shutdown timer for transient servers
class CShutdown : public CTimer
	{
	enum {KMyShutdownDelay=0x200000};	// approx 2s
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	}; 

enum TMsgServerPanic
	{
	EPanicIllegalFunction,
	};	
		
/**
 Example system server that uses the User Prompt Service.
 */
class CMsgServer : public CPolicyServer
	{
public:
	static CMsgServer* NewLC();
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	inline UserPromptService::RUpsSession& Ups();	
	void AddSession();
	void DropSession();	   
	TCustomResult CustomFailureActionL(const RMessage2& aMessage, TInt aAction, const TSecurityInfo& aMissing);
	~CMsgServer();
private:
	CMsgServer();
	void ConstructL();
private:
	// Server Policies
	static const TInt KPolicyRanges = 3;
	static const TInt KPolicyElements = 1;
	static const TInt iRanges[KPolicyRanges];
	static const TUint8 iElementsIndex[KPolicyRanges];
	static const CPolicyServer::TPolicyElement iPolicyElements[KPolicyElements];
	static const CPolicyServer::TPolicy iPolicy;
	CShutdown iShutdown;
	TInt iSessionCount;
	
	UserPromptService::RUpsSession iUps;	/// UPS session class held by Server
	};	
	
/**
 Active object that asynchronously processes the sending of a message. 
 An extra state is added to support UPS authorisation.
 In a more complicated server there may be several different message processor
 classes.
 */	   
class CMsgProcessor : public CActive
	{
	enum TState {EIdle, EAuthorising, ESending};
public:
	static CMsgProcessor* NewL(UserPromptService::RUpsSubsession& aAuth);		
	void ProcessL(const RMessage2& aMessage, TBool aPlatsecResult);
	~CMsgProcessor();
private:
	CMsgProcessor(UserPromptService::RUpsSubsession& aAuth);
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	void Reset();
	void GetParamsL(const RMessage2& aMessage);	
	void AuthoriseL(const RMessage2& aMessage, TBool aPlatsecResult);
	void SendL();
	
private:
	RMessage2 iMessage;
	TState iState;
	RBuf iMsgTo;
	RBuf iMsgBody;
	RTimer iTimer;		
	
	UserPromptService::RUpsSubsession& iAuth;	/// Handle to UPS sub-session for client
	TUpsDecision iDecision;						/// Whether the request was authorised
	TBool iPlatsecResult;						/// Whether the platsec check passed	
	// In techview this corresponds to 
	// ups\policies\test\tupspolicies\resource\ups_01041000_01041001.rss
	// and is recognised by the example UPS dialog creator and notifier.
	static const TUint KServiceId = 0x01041001;
	};
	
/**
 Session class for example message server that uses the User Prompt Service
 */
class CMsgServerSession : public CSession2
	{
public:
	CMsgServerSession();
	void CreateL();	
	void SetPlatsecResult(TBool aResult);
	~CMsgServerSession();
private:
	inline CMsgServer& Server(); 
	void ServiceL(const RMessage2& aMessage);		 
	CMsgProcessor* iProcessor;
		  
	/// Normally, a sub-session is created for each client or client connection
	UserPromptService::RUpsSubsession iAuth;	
	/// Only initialise iAuth once
	TBool iAuthInitialised;
	/// RUpsSubsession::Authorise needs to know whether the platsec check passed
	TBool iPlatsecResult;
	};
#endif /* TMSGSERVER_H_*/
