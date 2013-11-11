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
#include "HtiSysInfoH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

const unsigned char CMD_HAL					 = 0x01;
const unsigned char CMD_IMEI				 = 0x02;
const unsigned char CMD_SW_VERSION			 = 0x03;
const unsigned char CMD_LANG_VERSION		 = 0x04;
const unsigned char CMD_SW_LANG_VERSION		 = 0x05;
const unsigned char CMD_USERAGENT_STRING	 = 0x06;
const unsigned char CMD_GET_FREE_RAM		 = 0x07;
const unsigned char CMD_GET_USED_RAM		 = 0x08;
const unsigned char CMD_GET_TOTAL_RAM		 = 0x09;
const unsigned char CMD_EAT_RAM				 = 0x0A;
const unsigned char CMD_RELEASE_RAM			 = 0x0B;
const unsigned char CMD_GET_FREE_DISKSPACE	 = 0x0C;
const unsigned char CMD_GET_USED_DISKSPACE	 = 0x0D;
const unsigned char CMD_GET_TOTAL_DISKSPACE	 = 0x0E;
const unsigned char CMD_EAT_DISKSPACE		 = 0x0F;
const unsigned char CMD_RELEASE_DISKSPACE	 = 0x10;
const unsigned char CMD_SET_HOMETIME		 = 0x20;
const unsigned char CMD_GET_HOMETIME		 = 0x21;
const unsigned char CMD_SET_DATE_TIME_FORMAT = 0x22;
const unsigned char CMD_LIGHT_STATUS	 	 = 0x30;
const unsigned char CMD_LIGHT_ON			 = 0x31;
const unsigned char CMD_LIGHT_OFF			 = 0x32;
const unsigned char CMD_LIGHT_BLINK			 = 0x33;
const unsigned char CMD_LIGHT_RELEASE		 = 0x3A;
const unsigned char CMD_SCREENSAVER_DISABLE	 = 0x40;
const unsigned char CMD_SCREENSAVER_ENABLE	 = 0x41;
const unsigned char CMD_SCREENSAVER_TIMEOUT	 = 0x42;
const unsigned char CMD_GET_NETWORK_MODE	 = 0x50;
const unsigned char CMD_SET_NETWORK_MODE	 = 0x51;
const unsigned char CMD_SET_NETWORK_MODE_NO_REBOOT = 0x52;
const unsigned char CMD_HSDPA_ENABLE_DISABLE = 0x53;
const unsigned char CMD_IR_ACTIVATE          = 0x5A;
const unsigned char CMD_BT_POWER_STATE       = 0x5B;
const unsigned char CMD_BT_SETTINGS          = 0x5C;
const unsigned char CMD_BT_DELETE_PAIRINGS   = 0x5D;
const unsigned char CMD_KEY_LOCK_TOGGLE      = 0x60;
const unsigned char CMD_AUTO_KEY_LOCK_TIME   = 0x61;
const unsigned char CMD_EMPTY_DRM_RIGHTS_DB  = 0x65;
const unsigned char CMD_BATTERY_STATUS       = 0x70;
const unsigned char CMD_SIGNAL_STRENGTH      = 0x71;
const unsigned char CMD_UPDATE_MEDIA_GALLERY = 0x7A;
const unsigned char CMD_ACTIVATE_SKIN        = 0x80;

//const unsigned char CMD_LIGHT_SUCCESS		= 0x00;

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// ns1__getHalAttribute()
//**********************************************************************************
int ns1__getHalAttribute(struct soap* soap,
						 int attribute,
						 int &halAttribute)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_HAL );
	msg.AddInt( attribute );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 4 ) )
		return SOAP_FAULT;

	halAttribute = msg.GetInt( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getImei()
//**********************************************************************************
int ns1__getImei(struct soap* soap,
				 void *_,
				 char *&imei)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_IMEI );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	imei = msg.GetSoapString( 0, msg.GetMsgLen() );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getSwVersion()
//**********************************************************************************
int ns1__getSwVersion(struct soap* soap,
					  void *_,
					  wchar_t *&SwVersion)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SW_VERSION );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	// UCS-2 encoded string size must always be even
	if( msg.GetMsgLen() % 2 )
	{
		soap->error = soap_receiver_fault(soap, "HtiError",
			"response message size not even");
		return SOAP_FAULT;
	}

	// SwVersion returned as UCS-2 encoded text
	SwVersion = msg.GetSoapWCString( 0, msg.GetMsgLen()/2 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getLangVersion()
//**********************************************************************************
int ns1__getLangVersion(struct soap* soap,
						void *_,
						wchar_t *&langVersion)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_LANG_VERSION );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	// UCS-2 encoded string size must always be even
	if( msg.GetMsgLen() % 2 )
	{
		soap->error = soap_receiver_fault(soap, "HtiError",
			"response message size not even");
		return SOAP_FAULT;
	}

	// LangVersion returned as UCS-2 encoded text
	langVersion = msg.GetSoapWCString( 0, msg.GetMsgLen()/2 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getSwLangVersion()
//**********************************************************************************
int ns1__getSwLangVersion(struct soap* soap,
						  void *_,
						  wchar_t *&swLangVersion)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SW_LANG_VERSION );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	// UCS-2 encoded string size must always be even
	if( msg.GetMsgLen() % 2 )
	{
		soap->error = soap_receiver_fault(soap, "HtiError",
			"response message size not even");
		return SOAP_FAULT;
	}

	// swLangVersion returned as UCS-2 encoded text
	swLangVersion = msg.GetSoapWCString( 0, msg.GetMsgLen()/2 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getUserAgentString()
//**********************************************************************************
int ns1__getUserAgentString(struct soap* soap,
							void *_,
							char *&userAgentString)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_USERAGENT_STRING );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	userAgentString = msg.GetSoapString( 0, msg.GetMsgLen() );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__setHomeTime()
//**********************************************************************************
int ns1__setHomeTime(struct soap* soap,
                     struct ns1__HtiTime * time,
					 struct ns1__setHomeTimeResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SET_HOMETIME );
	msg.AddWord( time->year );
	msg.AddByte( time->month );
	msg.AddByte( time->day );
	msg.AddByte( time->hour );
	msg.AddByte( time->minute );
	msg.AddByte( time->second );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__getHomeTime()
//**********************************************************************************
int ns1__getHomeTime(struct soap* soap,
					 void *_,
                     struct getHomeTimeResponse& r)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_HOMETIME );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 7 ) )
		return SOAP_FAULT;

	r._returnHtiTime.year   = msg.GetWord( 0 );
	r._returnHtiTime.month  = msg.GetByte( 2 );
	r._returnHtiTime.day    = msg.GetByte( 3 );
	r._returnHtiTime.hour   = msg.GetByte( 4 );
	r._returnHtiTime.minute = msg.GetByte( 5 );
	r._returnHtiTime.second = msg.GetByte( 6 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__setDateTimeFormat()
//**********************************************************************************
int ns1__setDateTimeFormat(struct soap* soap,
                           enum DateFormat dateFormat,
                           unsigned char dateSeparator,
                           enum TimeFormat timeFormat,
                           unsigned char timeSeparator,
                           enum ClockFormat  clockFormat,
					       struct ns1__setDateTimeFormatResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SET_DATE_TIME_FORMAT );
	msg.AddByte( dateFormat );
	msg.AddByte( dateSeparator );
	msg.AddByte( timeFormat );
	msg.AddByte( timeSeparator );
	msg.AddByte( clockFormat );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__getFreeRAM()
//**********************************************************************************
int ns1__getFreeRAM(struct soap* soap,
					void *_,
					unsigned int &freeRAM)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_FREE_RAM );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	freeRAM = (unsigned int) msg.GetInt( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getUsedRAM()
//**********************************************************************************
int ns1__getUsedRAM(struct soap* soap,
					void *_,
					unsigned int &usedRAM)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_USED_RAM );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	usedRAM = (unsigned int) msg.GetInt( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getTotalRAM()
//**********************************************************************************
int ns1__getTotalRAM(struct soap* soap,
					 void *_,
					 unsigned int &totalRAM)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_TOTAL_RAM );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	totalRAM = (unsigned int) msg.GetInt( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__eatRAM()
//**********************************************************************************
int ns1__eatRAM(struct soap* soap,
				unsigned int RAMToBeLeftOver,
				unsigned int &freeRAM)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_EAT_RAM );
	msg.AddInt( RAMToBeLeftOver );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	freeRAM = (unsigned int) msg.GetInt( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__releaseRAM()
//**********************************************************************************
int ns1__releaseRAM(struct soap* soap,
					void *_,
					unsigned int &freeRAM)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_RELEASE_RAM );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	freeRAM = (unsigned int) msg.GetInt( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getFreeDiskSpace()
//**********************************************************************************
int ns1__getFreeDiskSpace(struct soap* soap,
						  char *driveLetter,
						  ULONG64 &freeDiskSpace)
{
	if ( check_mandatory_string_parameter( soap, driveLetter, "driveLetter" ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_FREE_DISKSPACE );
	msg.AddByte( driveLetter[0] );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	freeDiskSpace = *(ULONG64*) msg.GetMsgBody();

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getUsedDiskSpace()
//**********************************************************************************
int ns1__getUsedDiskSpace(struct soap* soap,
						  char *driveLetter,
						  ULONG64 &usedDiskSpace)
{
	if ( check_mandatory_string_parameter( soap, driveLetter, "driveLetter" ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_USED_DISKSPACE );
	msg.AddByte( driveLetter[0] );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	usedDiskSpace = *(ULONG64*) msg.GetMsgBody();

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getTotalDiskSpace()
//**********************************************************************************
int ns1__getTotalDiskSpace(struct soap* soap,
						   char *driveLetter,
						   ULONG64 &totalDiskSpace)
{
	if ( check_mandatory_string_parameter( soap, driveLetter, "driveLetter" ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_TOTAL_DISKSPACE );
	msg.AddByte( driveLetter[0] );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	totalDiskSpace = *(ULONG64*) msg.GetMsgBody();

	return SOAP_OK;
}

//**********************************************************************************
// ns1__eatDiskSpace()
//**********************************************************************************
int ns1__eatDiskSpace(struct soap* soap,
					  char *driveLetter,
					  ULONG64 diskSpaceToBeLeftOver,
					  ULONG64 &freeDiskSpace)
{
	if ( check_mandatory_string_parameter( soap, driveLetter, "driveLetter" ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_EAT_DISKSPACE );
	msg.AddByte( driveLetter[0] );
	msg.AddUInt64( diskSpaceToBeLeftOver );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	freeDiskSpace = *(ULONG64*) msg.GetMsgBody();

	return SOAP_OK;
}

//**********************************************************************************
// ns1__releaseDiskSpace()
//**********************************************************************************
int ns1__releaseDiskSpace(struct soap* soap,
						  char *driveLetter,
						  ULONG64 &freeDiskSpace)
{
	if ( check_mandatory_string_parameter( soap, driveLetter, "driveLetter" ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_RELEASE_DISKSPACE );
	msg.AddByte( driveLetter[0] );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	freeDiskSpace = *(ULONG64*) msg.GetMsgBody();

	return SOAP_OK;
}

//**********************************************************************************
// ns1__lightStatus()
//**********************************************************************************
int ns1__lightStatus(struct soap* soap,
					 unsigned char lightTarget,
					 unsigned char &lightStatus)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_LIGHT_STATUS );
	msg.AddByte( lightTarget );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	lightStatus = msg.GetByte( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__lightOn()
//**********************************************************************************
int ns1__lightOn(struct soap* soap,
				 unsigned char lightTarget,
				 unsigned short duration,
				 unsigned char intensity,
				 unsigned char fadeIn,
                 struct ns1__lightOnResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_LIGHT_ON );
	msg.AddByte( lightTarget );
	msg.AddWord( duration );
	msg.AddByte( intensity );
	msg.AddByte( fadeIn );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__lightOff()
//**********************************************************************************
int ns1__lightOff(struct soap* soap,
				  unsigned char lightTarget,
				  unsigned short duration,
				  unsigned char fadeOut,
                  struct ns1__lightOffResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_LIGHT_OFF );
	msg.AddByte( lightTarget );
	msg.AddWord( duration );
	msg.AddByte( fadeOut );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__lightBlink()
//**********************************************************************************
int ns1__lightBlink(struct soap* soap,
					unsigned char lightTarget,
					unsigned short duration,
					unsigned short onDuration,
					unsigned short offDuration,
					unsigned char intensity,
                    struct ns1__lightBlinkResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_LIGHT_BLINK );
	msg.AddByte( lightTarget );
	msg.AddWord( duration );
	msg.AddWord( onDuration );
	msg.AddWord( offDuration );
	msg.AddByte( intensity );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__lightRelease()
//**********************************************************************************
int ns1__lightRelease(struct soap* soap,
					  void *_,
                      struct ns1__lightReleaseResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_LIGHT_RELEASE );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__screenSaverDisable()
//**********************************************************************************
int ns1__screenSaverDisable(struct soap* soap,
							void *_,
                            struct ns1__screenSaverDisableResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SCREENSAVER_DISABLE );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__screenSaverEnable()
//**********************************************************************************
int ns1__screenSaverEnable(struct soap* soap,
						   void *_,
                           struct ns1__screenSaverEnableResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SCREENSAVER_ENABLE );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__screenSaverTimeout()
//**********************************************************************************
int ns1__screenSaverTimeout(struct soap* soap,
						    unsigned char timeout,
                            struct ns1__screenSaverTimeoutResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SCREENSAVER_TIMEOUT );
    msg.AddByte( timeout );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__getNetworkMode()
//**********************************************************************************
int ns1__getNetworkMode(struct soap* soap,
					void *_,
                    enum NetworkMode &mode)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_GET_NETWORK_MODE );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckCommandCode( CMD_GET_NETWORK_MODE ) )
		return SOAP_FAULT;

	mode = (NetworkMode) msg.GetInt( 1 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__setNetworkMode()
//**********************************************************************************
int ns1__setNetworkMode(struct soap* soap,
					enum NetworkMode mode,
                    struct ns1__setNetworkModeResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SET_NETWORK_MODE );
	msg.AddInt( mode );
	msg.SendMsg();

	HtiSoapHandlerInterface* handler = 
		static_cast<HtiSoapHandlerInterface*>( soap->user );

	// Wait a while if there is an error message. If there is no 
	// response it is assumed that the command has succeeded.
	handler->WaitForHtiMessage( HTIMSG_TIMEOUT_10_SECONDS );
	if ( handler->IsReceivedHtiError() )
	{
		handler->SendSoapFaultFromReceivedHtiError();
		return SOAP_FAULT;
	}
	return SOAP_OK;
}

//**********************************************************************************
// ns1__setNetworkModeNoReboot()
//**********************************************************************************
int ns1__setNetworkModeNoReboot(struct soap* soap,
					            enum NetworkMode mode,
                                struct ns1__setNetworkModeNoRebootResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SET_NETWORK_MODE_NO_REBOOT );
	msg.AddInt( mode );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__setHsdpaStatus()
//**********************************************************************************
int ns1__setHsdpaStatus(struct soap* soap,
					    enum HsdpaStatus status,
                        struct ns1__setHsdpaStatusResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_HSDPA_ENABLE_DISABLE );
	msg.AddInt( status );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__irActivate()
//**********************************************************************************
int ns1__irActivate(struct soap* soap,
		            void *_,
                    struct ns1__irActivateResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_IR_ACTIVATE );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__btPowerState()
//**********************************************************************************
int ns1__btPowerState(struct soap* soap,
                      bool btOn,
                      bool force,
                      struct ns1__btPowerStateResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_BT_POWER_STATE );
    msg.AddByte( btOn );
    msg.AddByte( force );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__btSettings()
//**********************************************************************************
int ns1__btSettings(struct soap* soap,
                    bool discoverable,
                    bool enableSAP,
                    char *btName,
                    struct ns1__btSettingsResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_BT_SETTINGS );
    msg.AddByte( discoverable );
    msg.AddByte( enableSAP );
    msg.AddStringWithLengthByteZero( btName );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__btDeletePairings()
//**********************************************************************************
int ns1__btDeletePairings(struct soap* soap,
                          bool closeConnections,
                          char *btName,
                          int &deleteCount)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_BT_DELETE_PAIRINGS );
    msg.AddByte( closeConnections );
    msg.AddStringWithLengthByteZero( btName );

    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
    	return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 1 ) )
		return SOAP_FAULT;

    deleteCount = msg.GetByte( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__keyLockToggle()
//**********************************************************************************
int ns1__keyLockToggle(struct soap* soap,
                       bool keyLockOn,
                       bool showNote,
                       struct ns1__keyLockToggleResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_KEY_LOCK_TOGGLE );
    msg.AddByte( keyLockOn );
    msg.AddByte( showNote );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__autoKeyLockTime()
//**********************************************************************************
int ns1__autoKeyLockTime(struct soap* soap,
                         int seconds,
                         struct ns1__autoKeyLockTimeResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_AUTO_KEY_LOCK_TIME );
    msg.AddWord( seconds );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__emptyDrmRightsDb()
//**********************************************************************************
int ns1__emptyDrmRightsDb(struct soap* soap,
		                  void *_,
                          struct ns1__emptyDrmRightsDbResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_EMPTY_DRM_RIGHTS_DB );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__batteryStatus()
//**********************************************************************************
int ns1__batteryStatus(struct soap* soap,
					   void *_,
                       struct batteryStatusResponse& r)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_BATTERY_STATUS );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 2 ) )
		return SOAP_FAULT;

	r._returnHtiBatteryStatus.batteryBars = msg.GetByte( 0 );
	r._returnHtiBatteryStatus.chargingStatus = msg.GetByte( 1 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__signalStrength()
//**********************************************************************************
int ns1__signalStrength(struct soap* soap,
					    void *_,
                        int &signalBars)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_SIGNAL_STRENGTH );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 1 ) )
		return SOAP_FAULT;
    
    signalBars = msg.GetByte( 0 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__updateMediaGallery()
//**********************************************************************************
int ns1__updateMediaGallery(struct soap* soap,
					        char* filePath,
                            struct ns1__updateMediaGalleryResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_UPDATE_MEDIA_GALLERY );
    msg.AddStringWithLengthByteZero( filePath );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS );
}

//**********************************************************************************
// ns1__activateSkin()
//**********************************************************************************
int ns1__activateSkin(struct soap* soap,
					  char* skinName,
                      struct ns1__activateSkinResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSINFO_UID, CMD_ACTIVATE_SKIN );
    msg.AddStringWithLengthByteZero( skinName );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}
