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
//gsoap ns1 service name: HtiSysInfo
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char*			xsd__string;
typedef wchar_t *		xsd__string_;
typedef unsigned char	xsd__unsignedByte;
typedef unsigned short	xsd__unsignedShort;
typedef int				xsd__int;
typedef unsigned int	xsd__unsignedInt;
typedef ULONG64			xsd__unsignedLong;
typedef bool            xsd__boolean;

struct ns1__HtiTime
{
	xsd__unsignedShort	year;
	xsd__unsignedByte	month;
	xsd__unsignedByte	day;
	xsd__unsignedByte	hour;
	xsd__unsignedByte	minute;
	xsd__unsignedByte	second;
};

struct ns1__HtiBatteryStatus
{
    xsd__unsignedByte batteryBars;
    xsd__unsignedByte chargingStatus;
};

enum NetworkMode {
	NetworkModeGsm     = 0x01,
	NetworkModeUmts    = 0x02,
	NetworkModeDual    = 0x04
};

enum DateFormat {
    DateFormatAmerican = 0x00,
    DateFormatEuropean = 0x01,
    DateFormatJapanese = 0x02
};

enum TimeFormat {
    TimeFormat12h = 0x00,
    TimeFormat24h = 0x01
};

enum ClockFormat {
    ClockFormatAnalogue = 0x00,
    ClockFormatDigital  = 0x01
};

enum HsdpaStatus {
    HsdpaDisabled = 0x00,
    HsdpaEnabled  = 0x01
};

//gsoap ns1 service method-action: getHalAttribute "HtiSysInfo"
int ns1__getHalAttribute(
    xsd__int		attribute,
    xsd__int		&halAttribute
);
//gsoap ns1 service method-action: getImei "HtiSysInfo"
int ns1__getImei(
    void *_, // no in parameter
    xsd__string		&imei
);
//gsoap ns1 service method-action: getSwVersion "HtiSysInfo"
int ns1__getSwVersion(
    void *_, // no in parameter
    xsd__string_	&swVersion
);
//gsoap ns1 service method-action: getLangVersion "HtiSysInfo"
int ns1__getLangVersion(
    void *_, // no in parameter
    xsd__string_	&langVersion
);
//gsoap ns1 service method-action: getSwLangVersion "HtiSysInfo"
int ns1__getSwLangVersion(
    void *_, // no in parameter
    xsd__string_	&swLangVersion
);
//gsoap ns1 service method-action: getUserAgentString "HtiSysInfo"
int ns1__getUserAgentString(
    void *_, // no in parameter
    xsd__string		&userAgentString
);

//gsoap ns1 service method-action: setHomeTime "HtiSysInfo"
int ns1__setHomeTime(
	struct ns1__HtiTime* time,
	struct ns1__setHomeTimeResponse{} *out  // empty response
);

struct getHomeTimeResponse{struct ns1__HtiTime _returnHtiTime;};
//gsoap ns1 service method-action: getHomeTime "HtiSysInfo"
int ns1__getHomeTime(
    void *_, // no in parameter
	struct getHomeTimeResponse& r
);

//gsoap ns1 service method-action: setDateTimeFormat "HtiSysInfo"
int ns1__setDateTimeFormat(
	enum DateFormat   dateFormat,
	xsd__unsignedByte dateSeparator,
	enum TimeFormat   timeFormat,
	xsd__unsignedByte timeSeparator,
	enum ClockFormat  clockFormat,
	struct ns1__setDateTimeFormatResponse{} *out // empty response
);

//gsoap ns1 service method-action: getFreeRAM "HtiSysInfo"
int ns1__getFreeRAM(
	void *_, // no in parameter
	xsd__unsignedInt		&freeRAM
);

//gsoap ns1 service method-action: getUsedRAM "HtiSysInfo"
int ns1__getUsedRAM(
	void *_, // no in parameter
	xsd__unsignedInt		&usedRAM
);

//gsoap ns1 service method-action: getTotalRAM "HtiSysInfo"
int ns1__getTotalRAM(
	void *_, // no in parameter
	xsd__unsignedInt		&totalRAM
);

//gsoap ns1 service method-action: eatRAM "HtiSysInfo"
int ns1__eatRAM(
	xsd__unsignedInt		RAMToBeLeftOver,
	xsd__unsignedInt		&freeRAM
);

//gsoap ns1 service method-action: releaseRAM "HtiSysInfo"
int ns1__releaseRAM(
	void *_, // no in parameter
	xsd__unsignedInt		&freeRAM
);

//gsoap ns1 service method-action: getFreeDiskSpace "HtiSysInfo"
int ns1__getFreeDiskSpace(
	xsd__string				driveLetter,
	xsd__unsignedLong		&freeDiskSpace
);

//gsoap ns1 service method-action: getUsedDiskSpace "HtiSysInfo"
int ns1__getUsedDiskSpace(
	xsd__string				driveLetter,
	xsd__unsignedLong		&usedDiskSpace
);

//gsoap ns1 service method-action: getTotalDiskSpace "HtiSysInfo"
int ns1__getTotalDiskSpace(
	xsd__string				driveLetter,
	xsd__unsignedLong		&totalDiskSpace
);

//gsoap ns1 service method-action: eatDiskSpace "HtiSysInfo"
int ns1__eatDiskSpace(
	xsd__string				driveLetter,
	xsd__unsignedLong		diskSpaceToBeLeftOver,
	xsd__unsignedLong		&freeDiskSpace
);

//gsoap ns1 service method-action: releaseDiskSpace "HtiSysInfo"
int ns1__releaseDiskSpace(
	xsd__string				driveLetter,
	xsd__unsignedLong		&freeDiskSpace
);

//gsoap ns1 service method-action: lightStatus "HtiSysInfo"
int ns1__lightStatus(
	xsd__unsignedByte		lightTarget,
	xsd__unsignedByte		&lightStatus
);

//gsoap ns1 service method-action: lightOn "HtiSysInfo"
int ns1__lightOn(
	xsd__unsignedByte		lightTarget,
	xsd__unsignedShort		duration,
	xsd__unsignedByte		intensity,
	xsd__unsignedByte		fadeIn,
	struct ns1__lightOnResponse{} *out // empty response
);

//gsoap ns1 service method-action: lightOff "HtiSysInfo"
int ns1__lightOff(
	xsd__unsignedByte		lightTarget,
	xsd__unsignedShort		duration,
	xsd__unsignedByte		fadeOut,
	struct ns1__lightOffResponse{} *out // empty response
);

//gsoap ns1 service method-action: lightBlink "HtiSysInfo"
int ns1__lightBlink(
	xsd__unsignedByte		lightTarget,
	xsd__unsignedShort		duration,
	xsd__unsignedShort		onDuration,
	xsd__unsignedShort		offDuration,
	xsd__unsignedByte		intensity,
	struct ns1__lightBlinkResponse{} *out // empty response
);

//gsoap ns1 service method-action: lightRelease "HtiSysInfo"
int ns1__lightRelease(
	void *_, // no in parameter
	struct ns1__lightReleaseResponse{} *out // empty response
);

//gsoap ns1 service method-action: screenSaverDisable "HtiSysInfo"
int ns1__screenSaverDisable(
	void *_, // no in parameter
	struct ns1__screenSaverDisableResponse{} *out // empty response
);

//gsoap ns1 service method-action: screenSaverEnable "HtiSysInfo"
int ns1__screenSaverEnable(
	void *_, // no in parameter
	struct ns1__screenSaverEnableResponse{} *out // empty response
);

//gsoap ns1 service method-action: screenSaverTimeout "HtiSysInfo"
int ns1__screenSaverTimeout(
	xsd__unsignedByte timeout,
	struct ns1__screenSaverTimeoutResponse{} *out // empty response
);

//gsoap ns1 service method-action: getNetworkMode "HtiSysInfo"
int ns1__getNetworkMode(
	void *_, // no in parameter
	enum NetworkMode &mode
);

//gsoap ns1 service method-action: setNetworkMode "HtiSysInfo"
int ns1__setNetworkMode(
	enum NetworkMode mode,
	struct ns1__setNetworkModeResponse{} *out // empty response
);

//gsoap ns1 service method-action: setNetworkModeNoReboot "HtiSysInfo"
int ns1__setNetworkModeNoReboot(
	enum NetworkMode mode,
	struct ns1__setNetworkModeNoRebootResponse{} *out // empty response
);

//gsoap ns1 service method-action: setHsdpaStatus "HtiSysInfo"
int ns1__setHsdpaStatus(
	enum HsdpaStatus status,
	struct ns1__setHsdpaStatusResponse{} *out // empty response
);

//gsoap ns1 service method-action: irActivate "HtiSysInfo"
int ns1__irActivate(
	void *_, // no in parameter,
	struct ns1__irActivateResponse{} *out // empty response
);

//gsoap ns1 service method-action: btPowerState "HtiSysInfo"
int ns1__btPowerState(
	xsd__boolean btOn,
    xsd__boolean force,
	struct ns1__btPowerStateResponse{} *out // empty response
);

//gsoap ns1 service method-action: btSettings "HtiSysInfo"
int ns1__btSettings(
	xsd__boolean discoverable,
    xsd__boolean enableSAP,
    xsd__string  btName,
	struct ns1__btSettingsResponse{} *out // empty response
);

//gsoap ns1 service method-action: btDeletePairings "HtiSysInfo"
int ns1__btDeletePairings(
	xsd__boolean closeConnections,
    xsd__string  btName,
	xsd__int     &deleteCount
);

//gsoap ns1 service method-action: keyLockToggle "HtiSysInfo"
int ns1__keyLockToggle(
	xsd__boolean keyLockOn,
    xsd__boolean showNote,
	struct ns1__keyLockToggleResponse{} *out // empty response
);

//gsoap ns1 service method-action: autoKeyLockTime "HtiSysInfo"
int ns1__autoKeyLockTime(
    xsd__int seconds,
	struct ns1__autoKeyLockTimeResponse{} *out // empty response
);

//gsoap ns1 service method-action: emptyDrmRightsDb "HtiSysInfo"
int ns1__emptyDrmRightsDb(
	void *_, // no in parameter,
	struct ns1__emptyDrmRightsDbResponse{} *out // empty response
);

struct batteryStatusResponse
{
    struct ns1__HtiBatteryStatus _returnHtiBatteryStatus;
};
//gsoap ns1 service method-action: batteryStatus "HtiSysInfo"
int ns1__batteryStatus(
    void *_, // no in parameter
	struct batteryStatusResponse& r
);

//gsoap ns1 service method-action: signalStrength "HtiSysInfo"
int ns1__signalStrength(
	void *_, // no in parameter
	xsd__int &signalBars
);

//gsoap ns1 service method-action: updateMediaGallery "HtiSysInfo"
int ns1__updateMediaGallery(
	xsd__string filePath,
    struct ns1__updateMediaGalleryResponse{} *out // empty response
);

//gsoap ns1 service method-action: activateSkin "HtiSysInfo"
int ns1__activateSkin(
	xsd__string skinName,
    struct ns1__activateSkinResponse{} *out // empty response
);
