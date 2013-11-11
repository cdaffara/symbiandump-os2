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
// HTI service functions
//gsoap ns1 service name: HtiKeyEvent
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char*		xsd__string;
typedef wchar_t*	xsd__string_;
typedef int         xsd__int;
typedef short		xsd__short;

struct ns1__HtiPoint
{
    xsd__short xCoordinate;
    xsd__short yCoordinate;
};

struct ns1__arrayOfKeyCodes
{
    xsd__short* __ptrKeyCode;
    xsd__int    __size;
};

struct ns1__arrayOfPoints // = line
{
    struct ns1__HtiPoint* __ptrPoint;
    int __size;    
};

struct ns1__arrayOfLines
{
    struct ns1__arrayOfPoints* __ptrLine;
    int __size;
};

//gsoap ns1 service method-action: pressKey "HtiKeyEvent" 
int ns1__pressKey(
    xsd__short    key,
	struct ns1__pressKeyResponse{} *out // empty response
);
//gsoap ns1 service method-action: holdKey "HtiKeyEvent" 
int ns1__holdKey(
	xsd__short    key,
	struct ns1__holdKeyResponse{} *out // empty response
);
//gsoap ns1 service method-action: releaseKey "HtiKeyEvent" 
int ns1__releaseKey(
	xsd__short    key,
	struct ns1__releaseKeyResponse{} *out // empty response
);
//gsoap ns1 service method-action: longKeyPress "HtiKeyEvent" 
int ns1__longKeyPress(
	xsd__short    key,
	xsd__short    time,
	struct ns1__longKeyPressResponse{} *out // empty response
);
//gsoap ns1 service method-action: typeText "HtiKeyEvent" 
int ns1__typeText(
	xsd__string_    text,
	struct ns1__typeTextResponse{} *out // empty response
);
//gsoap ns1 service method-action: typeTextPassword "HtiKeyEvent" 
int ns1__typeTextPassword(
	xsd__string_    text,
	struct ns1__typeTextPasswordResponse{} *out // empty response
);
//gsoap ns1 service method-action: keyPressSequence "HtiKeyEvent" 
int ns1__keyPressSequence(
	xsd__short    time,
    xsd__short    interval,
    struct ns1__arrayOfKeyCodes keyCodes,
	struct ns1__keyPressSequenceResponse{} *out // empty response
);
//gsoap ns1 service method-action: tapScreen "HtiKeyEvent" 
int ns1__tapScreen(
    struct ns1__HtiPoint tapPoint,
    xsd__short    timeToHold,
    xsd__short    tapCount,
    xsd__short    pauseBetweenTaps,
    struct ns1__tapScreenResponse{} *out // empty response
);
//gsoap ns1 service method-action: tapAndDrag "HtiKeyEvent" 
int ns1__tapAndDrag(
    struct ns1__HtiPoint pointDown,
    struct ns1__HtiPoint pointUp,
    xsd__short    dragTime,
    struct ns1__tapAndDragResponse{} *out // empty response
);
//gsoap ns1 service method-action: tapAndDragMultipoint "HtiKeyEvent" 
int ns1__tapAndDragMultipoint(
    xsd__short    timeBetweenPoints,
    xsd__short    timeBetweenLines,
    struct ns1__arrayOfLines lines,
    struct ns1__tapAndDragMultipointResponse{} *out // empty response
);
//gsoap ns1 service method-action: pointerDown "HtiKeyEvent" 
int ns1__pointerDown(
    struct ns1__HtiPoint pointDown,
    struct ns1__pointerDownResponse{} *out // empty response
);
//gsoap ns1 service method-action: pointerUp "HtiKeyEvent" 
int ns1__pointerUp(
    struct ns1__HtiPoint pointUp,
    struct ns1__pointerUpResponse{} *out // empty response
);
