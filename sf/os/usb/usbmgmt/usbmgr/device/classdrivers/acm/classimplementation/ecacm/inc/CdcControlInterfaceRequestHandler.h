/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CDCCOMMSCLASSREQUESTHANDLER_H__
#define __CDCCOMMSCLASSREQUESTHANDLER_H__

#include <e32std.h>

class MCdcCommsClassRequestHandler
/**
 * Interface for control interface requests from the host. Called by the 
 * reader on the control endpoint (ep0) when it decodes a relevant command.
 */
	{
public:
/**
 * Callback for Send Encap Command requests
 * @param aData Message data.
 */
	virtual TInt HandleSendEncapCommand(const TDesC8& aData) = 0;
/**
 * Callback for Get Encapsulated Response requests
 * @param aReturnData Message data.
 */
	virtual TInt HandleGetEncapResponse(TDes8& aReturnData) = 0;
/**
 * Callback for Set Comm Feature requests
 * @param aSelector The source of wValue control over feature selection
 * @param aData   The multiplexed data/idle state or country code from ISO 3166
 */
	virtual TInt HandleSetCommFeature(const TUint16 aSelector, 
		const TDes8& aData) = 0;
/**
 * Callback for Get Comm Feature requests
 * @param aSelector The source of wValue control over feature selection
 * @param aReturnData The multiplexed data/idle state or country code from ISO 
 * 3166
 */
	virtual TInt HandleGetCommFeature(const TUint16 aSelector, 
		TDes8& aReturnData) = 0;
/**
 * Callback for Clear Comm Feature requests
 * @param aSelector
 */
	virtual TInt HandleClearCommFeature(const TUint16 aSelector) = 0;
/**
 * Callback for Set Line Coding requests
 * @param aData Message data.
 */
	virtual TInt HandleSetLineCoding(const TDesC8& aData) = 0;
/**
 * Callback for Get Line Coding requests
 * @param aReturnData Message data.
 */
	virtual TInt HandleGetLineCoding(TDes8& aReturnData) = 0;
/**
 * Callback for Set Control Line State requests
 * @param aRtsState RTS state.
 * @param aDtrState DTR state.
 */
	virtual TInt HandleSetControlLineState(TBool aRtsState,
										   TBool aDtrState) = 0;
/**
 * Callback for Send Break requests
 * @param aDuration Duration of the break in microseconds
 */
	virtual TInt HandleSendBreak(TUint16 aDuration) = 0;
	};

#endif // __CDCCOMMSCLASSREQUESTHANDLER_H__
