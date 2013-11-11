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
//gsoap ns1 service name: HtiScreenshot
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char * xsd__string;
typedef wchar_t * xsd__string_;
typedef int xsd__int;
typedef unsigned char xsd__unsignedByte;
typedef unsigned short xsd__unsignedShort;
typedef bool xsd__boolean;

enum ns1__screenOrientation { Rotated0, Rotated90, Rotated180, Rotated270 };

struct ns1__HtiSoapAttachment
{
	//@xsd__string href;
	xsd__string href;
	xsd__string mimeType;
};

struct ns1__HtiFont
{
	xsd__string_ fontName;
	xsd__unsignedShort fontHeight;
	xsd__boolean isBold;
	xsd__boolean isItalic;
	xsd__boolean isNotAntialiased;
	xsd__unsignedByte position;
};

struct ArrayOfHtiFonts
{
	struct ns1__HtiFont* __ptrHtiFont;
	int __size;
};

struct ns1__HtiRecognizeTextResult
{
	xsd__boolean found;
	xsd__unsignedShort x;
	xsd__unsignedShort y;
	xsd__unsignedShort fontIndex;
};

struct ns1__HtiRect
{
	xsd__unsignedShort topLeftX;
	xsd__unsignedShort topLeftY;
	xsd__unsignedShort bottomRightX;
	xsd__unsignedShort bottomRightY;
};

struct ns1__HtiRGB
{
	xsd__unsignedByte	red;
	xsd__unsignedByte	green;
	xsd__unsignedByte	blue;
};

struct ns1__HtiDeltaCaptureAttachment
{
	xsd__string href;
	xsd__string mimeType;
	struct ns1__HtiRect rect;
};

struct ns1__HtiScreenModeResult
{
    xsd__unsignedByte screenNumber;
    xsd__unsignedShort screenWidth;
    xsd__unsignedShort screenHeight;
    enum ns1__screenOrientation screenRotation;
    xsd__unsignedByte displayMode;
    xsd__unsignedByte focusScreenNumber;
};

struct ns1__captureScreenResponse{ struct ns1__HtiSoapAttachment _returnAttachment;};

struct ns1__captureDeltaScreenResponse{
	//struct ns1__HtiSoapAttachment _returnAttachment;
	//struct ns1__HtiRect rect;
	struct ns1__HtiDeltaCaptureAttachment _returnDeltaAttachment;
};

struct ns1__recognizeTextResponse{ struct ns1__HtiRecognizeTextResult _returnText;};

struct ArrayOfFiles
{
    xsd__string	*__ptr;
    xsd__int	__size;
};

struct ns1__screenModeResponse{ struct ns1__HtiScreenModeResult _returnScreenMode;};

//gsoap ns1 service method-action: captureFullScreen "HtiScreenshot"
int ns1__captureFullScreen(
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
	struct ns1__captureScreenResponse& r
);

//gsoap ns1 service method-action: captureFullScreenZip "HtiScreenshot"
int ns1__captureFullScreenZip(
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
	struct ns1__captureScreenResponse& r
);

//gsoap ns1 service method-action: captureRegion "HtiScreenshot"
int ns1__captureRegion(
	struct ns1__HtiRect    rect,
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
   struct ns1__captureScreenResponse& r
);

//gsoap ns1 service method-action: captureRegionZip "HtiScreenshot"
int ns1__captureRegionZip(
	struct ns1__HtiRect    rect,
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
	struct ns1__captureScreenResponse& r
);

//gsoap ns1 service method-action: recognizeText "HtiScreenshot"
int ns1__recognizeText(
	xsd__string_		text,
	struct ArrayOfHtiFonts* array,
	struct ns1__recognizeTextResponse& r
);

//gsoap ns1 service method-action: createTextImage "HtiScreenshot"
int ns1__createTextImage(
	xsd__unsignedByte		colorDepth,
	xsd__string				imageMimeType,
	xsd__string_			text,
	struct ns1__HtiFont*	font,
	struct ns1__HtiRGB    fgColor,
	struct ns1__HtiRGB    bgColor,
	struct ns1__captureScreenResponse& r
);

//gsoap ns1 service method-action: captureFullScreenSeries "HtiScreenshot"
int ns1__captureFullScreenSeries(
	xsd__int			duration,
	xsd__int			interval,
	xsd__unsignedByte	colorDepth,
    xsd__string			imageMimeType,
	struct ArrayOfFiles *files
);

//gsoap ns1 service method-action: captureFullScreenZipSeries "HtiScreenshot"
int ns1__captureFullScreenZipSeries(
	xsd__int			duration,
	xsd__int			interval,
	xsd__unsignedByte	colorDepth,
    xsd__string			imageMimeType,
	struct ArrayOfFiles *files
);

//gsoap ns1 service method-action: captureRegionSeries "HtiScreenshot"
int ns1__captureRegionSeries(
	xsd__int			duration,
	xsd__int			interval,
	xsd__unsignedByte	colorDepth,
	struct ns1__HtiRect    rect,
	xsd__string			imageMimeType,
	struct ArrayOfFiles *files
);

//gsoap ns1 service method-action: captureRegionZipSeries "HtiScreenshot"
int ns1__captureRegionZipSeries(
	xsd__int			duration,
	xsd__int			interval,
	xsd__unsignedByte	colorDepth,
	struct ns1__HtiRect    rect,
	xsd__string			imageMimeType,
	struct ArrayOfFiles *files
);

//gsoap ns1 service method-action: captureDeltaFullScreen "HtiScreenshot"
int ns1__captureDeltaFullScreen(
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
	struct ns1__captureDeltaScreenResponse& r
);

//gsoap ns1 service method-action: captureDeltaFullScreenZip "HtiScreenshot"
int ns1__captureDeltaFullScreenZip(
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
	struct ns1__captureDeltaScreenResponse& r
);

//gsoap ns1 service method-action: captureDeltaRegion "HtiScreenshot"
int ns1__captureDeltaRegion(
	struct ns1__HtiRect    rect,
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
    struct ns1__captureDeltaScreenResponse& r
);

//gsoap ns1 service method-action: captureDeltaRegionZip "HtiScreenshot"
int ns1__captureDeltaRegionZip(
	struct ns1__HtiRect    rect,
	xsd__unsignedByte	colorDepth,
	xsd__string			imageMimeType,
	struct ns1__captureDeltaScreenResponse& r
);

//gsoap ns1 service method-action: selectScreen "HtiScreenshot"
int ns1__selectScreen(
	xsd__unsignedByte	screenNr,
	struct ns1__selectScreenResponse{} *out
);

//gsoap ns1 service method-action: resetDeltaScreen "HtiScreenshot"
int ns1__resetDeltaScreen(
    void *_, // no in parameter,
	struct ns1__resetDeltaScreenResponse{} *out // empty response
);

//gsoap ns1 service method-action: screenMode "HtiScreenshot"
int ns1__screenMode(
    void *_, // no in parameter,
	struct ns1__screenModeResponse& r
);