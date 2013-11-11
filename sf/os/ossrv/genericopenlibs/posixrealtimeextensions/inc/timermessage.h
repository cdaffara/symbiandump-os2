// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// timer message header file
// 
//


#ifndef  __TIMERMESSAGE__H
#define  __TIMERMESSAGE__H

#include <e32des8.h>
const TInt KTimerMsgMaxLen = 12;

//encapsulates the timer request message.
class TRtTimerMsg
{
public:
	TRtTimerMsg(const TInt& aTimerId, const TInt& aAction = 0): iTimerId(aTimerId), iRqstAct(aAction){}
	TRtTimerMsg(){}
	inline TInt Serialize(TDes8& aTo) const; 
	inline TInt DeSerialize(const TDes8& aFrom);

	enum 
		{
		EDELETETIMER = 2,	
		}TRqstAction;
		
	TInt 				iTimerId;	// timer id
	TInt				iRqstAct;
};

//serialize the timer request. 
TInt TRtTimerMsg::Serialize(TDes8& aTo) const
	{
	TInt lRet = KErrNone;
	if(aTo.MaxSize() != KTimerMsgMaxLen)
		lRet = KErrArgument;	
	else
		{
		TUint8	lBuf[KTimerMsgMaxLen];		
		Mem::FillZ(&lBuf[0], KTimerMsgMaxLen);
		
		Mem::Copy(&lBuf[0], &iTimerId, sizeof(TInt));
		Mem::Copy(&lBuf[sizeof (TInt)], &iRqstAct, sizeof(TInt));
			
		aTo.Zero();
		aTo.Append(lBuf,KTimerMsgMaxLen);
		}	
		
	return lRet;		
	}

//deserialize the timer request.
TInt TRtTimerMsg::DeSerialize(const TDes8& aFrom)
	{
	TInt lRet = KErrNone;
	if(aFrom.MaxSize() != KTimerMsgMaxLen)
		lRet = KErrArgument;
	else
		{
		const TUint8* lBuf = aFrom.Ptr();	
		Mem::Copy(&iTimerId, &lBuf[0], sizeof(TInt));
		Mem::Copy(&iRqstAct, &lBuf[sizeof (TInt)], sizeof(TInt));
		}

	return lRet;		
	}
	
#endif // __TIMERMESSAGE__H

