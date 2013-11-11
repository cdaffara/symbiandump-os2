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
#include "HtiScreenshotH.h"

#include "hticommon.h"
#include "HtiPlugin.h"
#include <string.h>

#include "HtiSoapHandlerInterface.h"

// Command codes
const unsigned char CMD_SCR_FULL		= 0x01;
const unsigned char CMD_SCR_REGION		= 0x02;
const unsigned char CMD_SCR_FULL_ZIP	= 0x03;
const unsigned char CMD_SCR_REGION_ZIP	= 0x04;

const unsigned char CMD_TEXT_RCG_U		= 0x11;
const unsigned char CMD_TEXT_IMG_U		= 0x13;

const unsigned char CMD_SCR_FULL_SERIES	      = 0x21;
const unsigned char CMD_SCR_REGION_SERIES	  = 0x22;
const unsigned char CMD_SCR_FULL_ZIP_SERIES	  = 0x23;
const unsigned char CMD_SCR_REGION_ZIP_SERIES = 0x24;

const unsigned char CMD_CHANGE_SCREEN = 0x30;

const unsigned char CMD_SCREEN_MODE = 0x3A;

const unsigned char CMD_DELTA_SCR_FULL              = 0x81;
const unsigned char CMD_DELTA_SCR_REGION            = 0x82;
const unsigned char CMD_DELTA_SCR_FULL_ZIP          = 0x83;
const unsigned char CMD_DELTA_SCR_REGION_ZIP        = 0x84;
const unsigned char CMD_DELTA_SCR_RESET             = 0x85;
const unsigned char CMD_DELTA_SCR_FULL_SERIES       = 0xA1;
const unsigned char CMD_DELTA_SCR_REGION_SERIES     = 0xA2;
const unsigned char CMD_DELTA_SCR_FULL_ZIP_SERIES   = 0xA3;
const unsigned char CMD_DELTA_SCR_REGION_ZIP_SERIES = 0xA4;


const unsigned char HTI_FONT_BOLD	= 0x01;
const unsigned char HTI_FONT_ITALIC	= 0x02;
const unsigned char HTI_FONT_NOTAA	= 0x04;
const unsigned char HTI_FONT_SUPER	= 0x08;
const unsigned char HTI_FONT_SUB	= 0x18;

const unsigned char TXT_RCG_OK	= 0xF0;
const unsigned char TXT_RCG_NOK	= 0xF1;

const int MAX_HREF_LEN = 128;

char zipMimeType[] = "application/octet-stream"; //??

const static int g_ScreenHtiTimeoutImage = 120000;
const static int g_ScreenHtiTimeoutRecongition = 60000;

//**********************************************************************************
// HELPER FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// AddHtiFont
//**********************************************************************************
void AddHtiFont(HtiMsgHelper &msg, struct ns1__HtiFont &font)
{
	msg.AddWCStringWithLengthByte( font.fontName );
	msg.AddWord( font.fontHeight );

	BYTE fontStyle = 0x0;

	// These can be combined
	if( font.isBold )
		fontStyle |= HTI_FONT_BOLD;
	if( font.isItalic )
		fontStyle |= HTI_FONT_ITALIC;
	if( font.isNotAntialiased )
		fontStyle |= HTI_FONT_NOTAA;

	// And these cannot
	if( font.position == 1 )
		fontStyle |= HTI_FONT_SUPER;
	else if( font.position == 2 )
		fontStyle |= HTI_FONT_SUB;

	msg.AddByte( fontStyle );
}

//**********************************************************************************
// GetHref
//**********************************************************************************
void GetHref(char* str, const unsigned char cmd)
{
	sprintf(str, "htiSs%x%d", cmd, clock() );
}

//**********************************************************************************
// SetScreenCaptureAttachment
//**********************************************************************************
int SetScreenCaptureAttachment(struct soap* soap,
							   BYTE* data,
							   int dataSize,
                               BYTE command,
							   char* mimeType,
                               struct ns1__captureScreenResponse &r)
{
	// alloc soap memory for attachment
	char* soapAttachment = (char*)soap_malloc(soap, dataSize );
	memcpy( soapAttachment, data, dataSize );

	// get & set href for attachment
	char href[MAX_HREF_LEN];
	GetHref(href, command);
	r._returnAttachment.href = (char*)soap_malloc(soap, strlen(href)+1 );
	strcpy( r._returnAttachment.href, href );

	// default mimetype is bmp
	if ( !( mimeType ? strlen( mimeType ) : 0 ) )
        mimeType = "image/bmp";

	// set mimetype
	r._returnAttachment.mimeType = (char*)soap_malloc(soap, strlen(mimeType)+1 );
	strcpy( r._returnAttachment.mimeType, mimeType );

	// set the attahcment
	soap_set_dime(soap);
	return soap_set_dime_attachment(soap, soapAttachment, dataSize,
	                                mimeType, href, 0, NULL);
}

//**********************************************************************************
// SetDeltaScreenCaptureAttachment
//**********************************************************************************
int SetDeltaScreenCaptureAttachment(struct soap* soap,
							        BYTE* data,
							        int dataSize,
                                    BYTE command,
							        char* mimeType,
                                    struct ns1__captureDeltaScreenResponse &r)
{
	// Set rectangle
	r._returnDeltaAttachment.rect.topLeftX     = *(WORD*)data; data+=2;
	r._returnDeltaAttachment.rect.topLeftY     = *(WORD*)data; data+=2;
	r._returnDeltaAttachment.rect.bottomRightX = *(WORD*)data; data+=2;
	r._returnDeltaAttachment.rect.bottomRightY = *(WORD*)data; data+=2;
	dataSize -= 2*4;

	// No attachment?
	if ( dataSize == 0 )
		return SOAP_OK;

	// alloc soap memory for attachment
	char* soapAttachment = (char*)soap_malloc(soap, dataSize );
	memcpy( soapAttachment, data, dataSize );

	// get & set href for attachment
	char href[MAX_HREF_LEN];
	GetHref(href, command);
	r._returnDeltaAttachment.href = (char*)soap_malloc(soap, strlen(href)+1 );
	strcpy( r._returnDeltaAttachment.href, href );

	// default mimetype is bmp
	if ( !( mimeType ? strlen( mimeType ) : 0 ) )
        mimeType = "image/bmp";

	// set mimetype
	r._returnDeltaAttachment.mimeType = (char*)soap_malloc(soap, strlen(mimeType)+1 );
	strcpy( r._returnDeltaAttachment.mimeType, mimeType );

	// set the attahcment
	soap_set_dime(soap);
	return soap_set_dime_attachment(soap, soapAttachment, dataSize,
	                                mimeType, href, 0, NULL);
}

//**********************************************************************************
// GetSerieShotFiles
//**********************************************************************************
int GetSerieShotFiles(struct soap* soap,
						HtiMsgHelper &msg,
                        struct ArrayOfFiles *files )
{
	// list of screenshot files is returned
	char** tmp_array_ptr = NULL;
	int i = 0;
	files->__size = 0;
	while( i < msg.GetMsgLen() )
	{
		int fileNameLen = msg.GetMsgBody()[i++];
		tmp_array_ptr = (char**) realloc(tmp_array_ptr,
		                                 (files->__size+1)*sizeof(char**));
		tmp_array_ptr[files->__size++] = msg.GetSoapString( i, fileNameLen );
		i += fileNameLen;
	}

	if ( files->__size == 0 )
	{
		soap->error = soap_receiver_fault( soap, "HtiError",
			"No screenshot files returned");
		return SOAP_FAULT;
	}

	// alloc (soap) space for pointers & copy data from tmp_array_ptr
	files->__ptr = (char**) soap_malloc(soap, sizeof(char**)*files->__size);
	memcpy(files->__ptr, tmp_array_ptr, sizeof(char**)*files->__size);
	free(tmp_array_ptr);
	return SOAP_OK;
}

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// ns1__captureFullScreen
//**********************************************************************************
int ns1__captureFullScreen(struct soap* soap,
						   unsigned char colorDepth,
						   char *imageMimeType,
						   struct ns1__captureScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_FULL );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_SCR_FULL, imageMimeType ,r );
}

//**********************************************************************************
// ns1__captureFullScreenZip
//**********************************************************************************
int ns1__captureFullScreenZip(struct soap* soap,
							  unsigned char colorDepth,
							  char *imageMimeType,
							  struct ns1__captureScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_FULL_ZIP );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_SCR_FULL_ZIP, zipMimeType ,r );
}

//**********************************************************************************
// ns1__captureRegion
//**********************************************************************************
int ns1__captureRegion(struct soap* soap,
					   struct ns1__HtiRect rect,
					   unsigned char colorDepth,
					   char *imageMimeType,
					   struct ns1__captureScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_REGION );
	msg.AddWord( rect.topLeftX );
	msg.AddWord( rect.topLeftY );
	msg.AddWord( rect.bottomRightX );
	msg.AddWord( rect.bottomRightY );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_SCR_REGION, imageMimeType ,r );
}

//**********************************************************************************
// ns1__captureRegionZip
//**********************************************************************************
int ns1__captureRegionZip(struct soap* soap,
						  struct ns1__HtiRect rect,
						  unsigned char colorDepth,
						  char *imageMimeType,
						  struct ns1__captureScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_REGION_ZIP );
	msg.AddWord( rect.topLeftX );
	msg.AddWord( rect.topLeftY );
	msg.AddWord( rect.bottomRightX );
	msg.AddWord( rect.bottomRightY );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_SCR_REGION_ZIP, zipMimeType ,r );
}

//**********************************************************************************
// ns1__recognizeText
//**********************************************************************************
int ns1__recognizeText(struct soap* soap,
					   wchar_t *text,
					   struct ArrayOfHtiFonts* array,
					   struct ns1__recognizeTextResponse &r)
{
	// check text parameter
	if ( check_mandatory_wcstring_parameter( soap, text, "text" ) )
		return SOAP_FAULT;
	else if ( wcslen( text ) > 0xff )
	{
		soap->error = soap_sender_fault(soap, "HtiGateway", "Text too long");
		return SOAP_FAULT;
	}

	// check array parameter
	if ( array == NULL )
	{
		soap->error = soap_sender_fault( soap, "HtiGateway",
		                                 "No font array parameter" );
		return SOAP_FAULT;
	}
	else if ( array->__size < 0 )
	{
		soap->error = soap_sender_fault( soap, "HtiGateway", "Font array empty" );
		return SOAP_FAULT;
	}
	else if ( array->__size > 0xff )
	{
		soap->error = soap_sender_fault( soap, "HtiGateway", "Too many fonts" );
		return SOAP_FAULT;
	}
	for ( int i = 0; i < array->__size; i++ )
	{
		char tmp[20];
		sprintf(tmp, "HtiFont[%d]", i);
		if ( check_mandatory_wcstring_parameter( soap,
			            array->__ptrHtiFont[i].fontName, tmp ) )
			return SOAP_FAULT;
	}

	// construct and send the message
	HtiMsgHelper msg( soap, HTI_UID, CMD_TEXT_RCG_U );
	msg.AddWCStringWithLengthByte( text );
	msg.AddByte( array->__size );
	for( int i = 0; i < array->__size; i++ )
		AddHtiFont( msg, array->__ptrHtiFont[i] );

	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutRecongition ) )
		return SOAP_FAULT;

	// Check response
	if ( msg.CheckMsgExactLen( 6 ) )
		return SOAP_FAULT;

	switch( msg.GetByte(0) )
	{
	case TXT_RCG_OK:
		r._returnText.found = true;
		r._returnText.x = msg.GetWord( 1 );
		r._returnText.y = msg.GetWord( 3 );
		r._returnText.fontIndex = msg.GetByte( 5 );
		break;

	case TXT_RCG_NOK:
		r._returnText.found = false;
		break;

	default:
		soap->error = soap_receiver_fault_format(soap, "HtiError",
			"Invalid response command code 0x%x", msg.GetByte(0) );
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

//**********************************************************************************
// ns1__createTextImage
//**********************************************************************************
int ns1__createTextImage(struct soap* soap,
						 unsigned char colorDepth,
						 char *imageMimeType,
						 wchar_t *text,
						 struct ns1__HtiFont* font,
						 struct ns1__HtiRGB    fgColor,
						 struct ns1__HtiRGB    bgColor,
						 struct ns1__captureScreenResponse &r)
{
	// check text & imageMimeType parameters
	if ( check_mandatory_wcstring_parameter( soap, text, "text" ) ||
		 check_mandatory_string_parameter( soap, imageMimeType, "imageMimeType" ) )
		return SOAP_FAULT;
	else if ( wcslen( text ) > 0xff )
	{
		soap->error = soap_sender_fault(soap, "HtiGateway", "Text too long");
		return SOAP_FAULT;
	}

	// check font parameter
	if ( font == NULL )
	{
		soap->error = soap_sender_fault( soap, "HtiGateway", "No font parameter" );
		return SOAP_FAULT;
	}
	if ( check_mandatory_wcstring_parameter( soap, font->fontName, "fontName" ) )
		return SOAP_FAULT;

	// construct and send the message
	HtiMsgHelper msg( soap, HTI_UID, CMD_TEXT_IMG_U );
	msg.AddByte( colorDepth );
	msg.AddStringWithLengthByte( imageMimeType );
	msg.AddWCStringWithLengthByte( text );
	AddHtiFont( msg, *font );
	msg.AddByte( fgColor.red ); // note little endian encoding
	msg.AddByte( fgColor.green );
	msg.AddByte( fgColor.blue );
	msg.AddByte( 0x00 );
	msg.AddByte( bgColor.red );
	msg.AddByte( bgColor.green );
	msg.AddByte( bgColor.blue );
	msg.AddByte( 0x00 );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

	// get response
	return SetScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_TEXT_IMG_U, imageMimeType ,r );
}



//**********************************************************************************
// ns1__captureFullScreenSeries
//**********************************************************************************
int ns1__captureFullScreenSeries(struct soap* soap,
								 int duration,
								 int interval,
								 unsigned char colorDepth,
								 char *imageMimeType,
                                 struct ArrayOfFiles *files)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_FULL_SERIES );
	msg.AddInt( duration );
	msg.AddInt( interval );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	int timeout = HTIMSG_TIMEOUT_10_SECONDS + duration/1000; // duration is in us
	if ( msg.SendReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return GetSerieShotFiles( soap, msg, files );
}

//**********************************************************************************
// ns1__captureFullScreenZipSeries
//**********************************************************************************
int ns1__captureFullScreenZipSeries(struct soap* soap,
									int duration,
									int interval,
									unsigned char colorDepth,
									char *imageMimeType,
                                    struct ArrayOfFiles *files)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_FULL_ZIP_SERIES );
	msg.AddInt( duration );
	msg.AddInt( interval );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	int timeout = HTIMSG_TIMEOUT_10_SECONDS + duration/1000; // duration is in us
	if ( msg.SendReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return GetSerieShotFiles( soap, msg, files );
}

//**********************************************************************************
// ns1__captureRegionSeries
//**********************************************************************************
int ns1__captureRegionSeries(struct soap* soap,
							 int duration,
							 int interval,
							 unsigned char colorDepth,
							 struct ns1__HtiRect rect,
							 char *imageMimeType,
                             struct ArrayOfFiles *files)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_REGION_SERIES );
	msg.AddInt( duration );
	msg.AddInt( interval );
	msg.AddByte( colorDepth );
	msg.AddWord( rect.topLeftX );
	msg.AddWord( rect.topLeftY );
	msg.AddWord( rect.bottomRightX );
	msg.AddWord( rect.bottomRightY );
	msg.AddString( imageMimeType );
	int timeout = HTIMSG_TIMEOUT_10_SECONDS + duration/1000; // duration is in us
	if ( msg.SendReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return GetSerieShotFiles( soap, msg, files );
}

//**********************************************************************************
// ns1__captureRegionZipSeries
//**********************************************************************************
int ns1__captureRegionZipSeries(struct soap* soap,
								int duration,
								int interval,
								unsigned char colorDepth,
								struct ns1__HtiRect rect,
								char *imageMimeType,
                                struct ArrayOfFiles *files)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCR_REGION_ZIP_SERIES );
	msg.AddInt( duration );
	msg.AddInt( interval );
	msg.AddByte( colorDepth );
	msg.AddWord( rect.topLeftX );
	msg.AddWord( rect.topLeftY );
	msg.AddWord( rect.bottomRightX );
	msg.AddWord( rect.bottomRightY );
	msg.AddString( imageMimeType );
	int timeout = HTIMSG_TIMEOUT_10_SECONDS + duration/1000; // duration is in us
	if ( msg.SendReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return GetSerieShotFiles( soap, msg, files );
}

//**********************************************************************************
// ns1__captureDeltaFullScreen
//**********************************************************************************
int ns1__captureDeltaFullScreen(struct soap* soap,
								unsigned char colorDepth,
								char *imageMimeType,
                                struct ns1__captureDeltaScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELTA_SCR_FULL );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetDeltaScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_DELTA_SCR_FULL, imageMimeType ,r );
}
//**********************************************************************************
// ns1__captureDeltaFullScreenZip
//**********************************************************************************
int ns1__captureDeltaFullScreenZip(struct soap* soap,
								   unsigned char colorDepth,
								   char *imageMimeType,
                                   struct ns1__captureDeltaScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELTA_SCR_FULL_ZIP );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetDeltaScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_DELTA_SCR_FULL_ZIP, zipMimeType ,r );
}
//**********************************************************************************
// ns1__captureDeltaRegion
//**********************************************************************************
int ns1__captureDeltaRegion(struct soap* soap,
                            struct ns1__HtiRect rect,
							unsigned char colorDepth,
							char *imageMimeType,
							struct ns1__captureDeltaScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELTA_SCR_REGION );
	msg.AddWord( rect.topLeftX );
	msg.AddWord( rect.topLeftY );
	msg.AddWord( rect.bottomRightX );
	msg.AddWord( rect.bottomRightY );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetDeltaScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_DELTA_SCR_REGION, imageMimeType ,r );
}
//**********************************************************************************
// ns1__captureDeltaRegionZip
//**********************************************************************************
int ns1__captureDeltaRegionZip(struct soap* soap,
                               struct ns1__HtiRect rect,
							   unsigned char colorDepth,
							   char *imageMimeType,
							   struct ns1__captureDeltaScreenResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELTA_SCR_REGION_ZIP );
	msg.AddWord( rect.topLeftX );
	msg.AddWord( rect.topLeftY );
	msg.AddWord( rect.bottomRightX );
	msg.AddWord( rect.bottomRightY );
	msg.AddByte( colorDepth );
	msg.AddString( imageMimeType );
	if ( msg.SendReceiveMsg( g_ScreenHtiTimeoutImage ) )
		return SOAP_FAULT;

    return SetDeltaScreenCaptureAttachment( soap, msg.GetMsgBody(), msg.GetMsgLen(),
		CMD_DELTA_SCR_REGION_ZIP, zipMimeType ,r );
}

//**********************************************************************************
// ns1__selectScreen
//**********************************************************************************
int ns1__selectScreen(struct soap* soap,
					  unsigned char screenNr,
                      struct ns1__selectScreenResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_CHANGE_SCREEN );
	msg.AddByte( screenNr );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__resetDeltaScreen
//**********************************************************************************
int ns1__resetDeltaScreen(struct soap* soap,
                          void *_,
                          struct ns1__resetDeltaScreenResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELTA_SCR_RESET );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__screenMode
//**********************************************************************************
int ns1__screenMode(struct soap* soap,
                    void *_,
                    struct ns1__screenModeResponse& r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SCREEN_MODE );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;
    
	// Check response
	if ( msg.CheckMsgExactLen( 8 ) )
		return SOAP_FAULT;

    r._returnScreenMode.screenNumber = msg.GetByte( 0 );
    r._returnScreenMode.screenWidth = msg.GetWord( 1 );
    r._returnScreenMode.screenHeight = msg.GetWord( 3 );
    r._returnScreenMode.screenRotation = (ns1__screenOrientation)msg.GetByte( 5 );
    r._returnScreenMode.displayMode = msg.GetByte( 6 );
    r._returnScreenMode.focusScreenNumber = msg.GetByte( 7 );

    return SOAP_OK;
}
