/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*/
#include "HtiKeyEventH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

const unsigned char CMD_SINGLE_KEY_PRESS   = 0x01;
const unsigned char CMD_KEY_DOWN           = 0x02;
const unsigned char CMD_KEY_UP             = 0x03;
const unsigned char CMD_TYPE_TEXT          = 0x04;
const unsigned char CMD_LONG_KEY_PRESS     = 0x05;
const unsigned char CMD_TYPE_TEXT_PASSWORD = 0x06;
const unsigned char CMD_KEY_PRESS_SEQUENCE = 0x07;
const unsigned char CMD_TAP_SCREEN         = 0x10;
const unsigned char CMD_TAP_AND_DRAG       = 0x11;
const unsigned char CMD_TAP_AND_DRAG_MULTI = 0x12;
const unsigned char CMD_POINTER_DOWN       = 0x13;
const unsigned char CMD_POINTER_UP         = 0x14;

const unsigned char KEY_EVENT_OK = 0xFF;

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// ns1__pressKey()
//**********************************************************************************
int ns1__pressKey(struct soap* soap,
				  short key,
                  struct ns1__pressKeyResponse *out)
{
	HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_SINGLE_KEY_PRESS );
	msg.AddWord( key );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__holdKey()
//**********************************************************************************
int ns1__holdKey(struct soap* soap,
				 short key,
                 struct ns1__holdKeyResponse *out )
{
	HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_KEY_DOWN );
	msg.AddWord( key );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__releaseKey()
//**********************************************************************************
int ns1__releaseKey(struct soap* soap, 
					short key,
                    struct ns1__releaseKeyResponse *out)
{
	HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_KEY_UP );
	msg.AddWord( key );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__longKeyPress()
//**********************************************************************************
int ns1__longKeyPress(struct soap* soap,
					  short key,
					  short time,
                      struct ns1__longKeyPressResponse *out)
{
	HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_LONG_KEY_PRESS );
	msg.AddWord( key );
	msg.AddWord( time );
	return msg.SendReceiveMsg( time + HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__typeText()
//**********************************************************************************
int ns1__typeText(struct soap* soap, 
				  wchar_t *text, 
                  struct ns1__typeTextResponse *out)
{
	if(check_mandatory_wcstring_parameter(soap, text, "text"))
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_TYPE_TEXT );
	msg.AddWCString( text );

	// Set timeout for ok message
	// Assume that writing 100 characters takes 10 seconds (should be more than enough)
	// = 10s/100char = 0.1s/char = 100ms/char
	int timeout = (int) wcslen(text)*100;
	timeout+= HTIMSG_TIMEOUT_10_SECONDS; // add extra 10 seconds for lag
	return msg.SendReceiveMsg( timeout );
}

//**********************************************************************************
// ns1__typeTextPassword()
//**********************************************************************************
int ns1__typeTextPassword(struct soap* soap, 
						  wchar_t *text, 
                          struct ns1__typeTextPasswordResponse *out)
{
	if(check_mandatory_wcstring_parameter(soap, text, "text"))
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_TYPE_TEXT_PASSWORD );
	msg.AddWCString( text );

	// Set timeout for ok message
	// Assume that writing 100 characters takes 10 seconds (should be more than enough)
	// = 10s/100char = 0.1s/char = 100ms/char
	int timeout = (int) wcslen(text)*100;
	timeout+= HTIMSG_TIMEOUT_10_SECONDS; // add extra 10 seconds for lag
	return msg.SendReceiveMsg( timeout );
}

//**********************************************************************************
// ns1__keyPressSequence()
//**********************************************************************************
int ns1__keyPressSequence(struct soap* soap,
                          short time,
                          short interval,
                          struct ns1__arrayOfKeyCodes keyCodes,
                       	  struct ns1__keyPressSequenceResponse *out)
{
    HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_KEY_PRESS_SEQUENCE );
    msg.AddWord( time );
    msg.AddWord( interval );
 	for ( int i = 0; i < keyCodes.__size; i++ )
	{
		msg.AddWord( keyCodes.__ptrKeyCode[i] );
    }
    int timeout = keyCodes.__size * ( time + interval );
    timeout += HTIMSG_TIMEOUT_10_SECONDS;
    return msg.SendReceiveMsg( timeout );
}

//**********************************************************************************
// ns1__tapScreen()
//**********************************************************************************
int ns1__tapScreen(struct soap* soap,
				   struct ns1__HtiPoint tapPoint,
				   short timeToHold,
				   short tapCount,
                   short pauseBetweenTaps,
                   struct ns1__tapScreenResponse *out)
{
    HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_TAP_SCREEN );
    msg.AddWord( tapPoint.xCoordinate );
	msg.AddWord( tapPoint.yCoordinate );
    msg.AddWord( timeToHold );
    msg.AddWord( tapCount );
    msg.AddWord( pauseBetweenTaps );
    int timeout = tapCount * ( timeToHold + pauseBetweenTaps );
    timeout += HTIMSG_TIMEOUT_10_SECONDS;
	return msg.SendReceiveMsg( timeout );
}

//**********************************************************************************
// ns1__tapAndDrag()
//**********************************************************************************
int ns1__tapAndDrag(struct soap* soap,
                    struct ns1__HtiPoint pointDown,
                    struct ns1__HtiPoint pointUp,
                    short dragTime,
                    struct ns1__tapAndDragResponse *out)
{
    HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_TAP_AND_DRAG );
    msg.AddWord( pointDown.xCoordinate );
    msg.AddWord( pointDown.yCoordinate );
    msg.AddWord( pointUp.xCoordinate );
    msg.AddWord( pointUp.yCoordinate );
    msg.AddWord( dragTime );
    return msg.SendReceiveMsg( dragTime + HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__tapAndDragMultipoint()
//**********************************************************************************
int ns1__tapAndDragMultipoint(struct soap* soap,
                              short timeBetweenPoints,
                              short timeBetweenLines,
                              struct ns1__arrayOfLines lines,
                              struct ns1__tapAndDragMultipointResponse *out)
{
    HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_TAP_AND_DRAG_MULTI );
    msg.AddWord( timeBetweenPoints );
    msg.AddWord( timeBetweenLines );
    int pointCount = 0;
    for ( int i = 0; i < lines.__size; i++ )
    {
        int pointsInLine = lines.__ptrLine[i].__size;
        pointCount += pointsInLine;
        msg.AddWord( pointsInLine );
        for ( int j = 0; j < pointsInLine; j++ )
        {
            msg.AddWord( lines.__ptrLine[i].__ptrPoint[j].xCoordinate );
            msg.AddWord( lines.__ptrLine[i].__ptrPoint[j].yCoordinate );
        }
    }

    int timeout = ( pointCount * timeBetweenPoints ) +
                  ( lines.__size * timeBetweenLines );
    timeout += HTIMSG_TIMEOUT_10_SECONDS;
    return msg.SendReceiveMsg( timeout );
}

//**********************************************************************************
// ns1__pointerDown()
//**********************************************************************************
int ns1__pointerDown(struct soap* soap,
				     struct ns1__HtiPoint pointDown,
                     struct ns1__pointerDownResponse *out )
{
    HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_POINTER_DOWN );
    msg.AddWord( pointDown.xCoordinate );
    msg.AddWord( pointDown.yCoordinate );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__pointerUp()
//**********************************************************************************
int ns1__pointerUp(struct soap* soap,
				   struct ns1__HtiPoint pointUp,
                   struct ns1__pointerUpResponse *out )
{
    HtiMsgHelper msg( soap, HTI_KEYEVENT_UID, CMD_POINTER_UP );
    msg.AddWord( pointUp.xCoordinate );
    msg.AddWord( pointUp.yCoordinate );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}
