/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef __RANDCLISERV_H__
#define __RANDCLISERV_H__

_LIT(KRandomServerLib,"randsvr");
_LIT(KRandomServerName,"!RandomServer");

class TServerStart
/**
 * @publishedPartner
 * @released
 */
	{
public:
	TServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;
	/**
	 * @internalAll
	 */
	TServerStart();
	/**
	 * @internalAll
	 */
	TInt GetCommand();
	/**
	 * @internalAll
	 */
	void SignalL();
private:
	TThreadId iId;
	TRequestStatus* iStatus;
	};

inline TServerStart::TServerStart(TRequestStatus& aStatus)
	:iId(RThread().Id()),iStatus(&aStatus)
	{aStatus=KRequestPending;}
inline TPtrC TServerStart::AsCommand() const
	{return TPtrC(reinterpret_cast<const TText*>(this),sizeof(TServerStart)/sizeof(TText));}

#endif // __RANDCLISERV_H__
