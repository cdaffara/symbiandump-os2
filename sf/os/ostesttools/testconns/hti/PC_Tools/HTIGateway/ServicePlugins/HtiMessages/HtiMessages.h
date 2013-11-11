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
//gsoap ns1 service name: HtiMessages
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef int             xsd__int;
typedef char*           xsd__string;
typedef bool            xsd__boolean;
typedef unsigned char   xsd__unsignedByte;
typedef unsigned short  xsd__unsignedShort;

enum ns1__folder      { AllFolders, Indox, Drafts, Sent, Outbox };
enum ns1__messageType { AllMessages, SMS, MMS, SmartMsg, Email, IRmsg, BTmsg, AudioMsg };

enum ns1__bearerType      { CSD = 1, GPRS = 2, HSCSD = 4, CDMA = 16, WLAN = 32 };
enum ns1__mailboxType     { POP3, IMAP4 };
enum ns1__securitySetting { Off, TLS, SSL };
enum ns1__sendOption      { Immediately, NextConnection, OnRequest };
enum ns1__copyToOwn       { NoCopy, ToField, CcField, BccField };
enum ns1__retrievedParts  { OnlyHeaders, LessThanSize, BodyAndAttachments };
enum ns1__automaticUpdate { AlwaysOn, InHomeNetwork, NotEnabled };

enum ns1__mmsCreationMode { Restricted, Guided, Free };
enum ns1__mmsImageSize    { Small, Large, Original };
enum ns1__mmsReception    { AlwaysAutomatic, AutomaticInHomeNetwork,
                            AlwaysManual, Disabled };
enum ns1__msgValidity     { Maximum, OneHour, SixHours, TwentyFourHours,
							ThreeDays, OneWeek };

enum ns1__smsCharacterSupport    { Full, Reduced };
enum ns1__smsMessageConversion   { None, Fax, Paging, EMail };
enum ns1__smsPreferredConnection { GSM, PacketData };

struct ns1__HtiAPSettingField
{
    xsd__unsignedByte    fieldType;
    xsd__string          fieldData;
};

struct ArrayOfHtiAPSettingFields
{
    struct ns1__HtiAPSettingField* __ptrHtiAPSettingField;
    int __size;
};


//gsoap ns1 service method-action: addSMS "HtiMessages"
int ns1__addSMS(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      messageBody,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: addMMS "HtiMessages"
int ns1__addMMS(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      messageBody,
    xsd__string      attachmentPath,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: addEMail "HtiMessages"
int ns1__addEMail(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      messageBody,
    xsd__string      attachmentPath,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: addIRMessage "HtiMessages"
int ns1__addIRMessage(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      attachmentPath,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: addBTMessage "HtiMessages"
int ns1__addBTMessage(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      attachmentPath,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: addSmartMessage "HtiMessages"
int ns1__addSmartMessage(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      messageBody,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         BIOmessageUID,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: addAudioMessage "HtiMessages"
int ns1__addAudioMessage(
    xsd__string      fromField,
    xsd__string      description,
    xsd__string      attachmentPath,
    xsd__boolean     isNewFlag,
    xsd__boolean     isUnreadFlag,
    enum ns1__folder folder,
    xsd__int         &messageId
);

//gsoap ns1 service method-action: deleteMessage "HtiMessages"
int ns1__deleteMessage(
    xsd__int         messageId,
    struct ns1__deleteMessageResponse{} *out  // empty response
);

//gsoap ns1 service method-action: deleteAllFromFolder "HtiMessages"
int ns1__deleteAllFromFolder(
    enum ns1__messageType    type,
    enum ns1__folder         folder,
    struct ns1__deleteAllFromFolderResponse{} *out  // empty response
);

//gsoap ns1 service method-action: createAccessPoint "HtiMessages"
int ns1__createAccessPoint(
    enum ns1__bearerType    bearer,
    xsd__string             apName,
    struct ArrayOfHtiAPSettingFields    APSettings,
    xsd__int                            &apId
);

//gsoap ns1 service method-action: deleteAccessPoint "HtiMessages"
int ns1__deleteAccessPoint(
    xsd__string apName,
    struct ns1__deleteAccessPointResponse{} *out  //empty response
);

//gsoap ns1 service method-action: createMailbox "HtiMessages"
int ns1__createMailbox(
    enum ns1__mailboxType mailboxType,
    xsd__string mailboxName,
    xsd__string incomingServer,
    xsd__string incomingAccessPoint,
    xsd__string incomingUsername,
    xsd__string incomingPassword,
    enum ns1__securitySetting incomingSecurity,
    xsd__unsignedShort incomingPort,
    xsd__boolean aPopSecureLogin,
    xsd__string outgoingServer,
    xsd__string outgoingAccessPoint,
    xsd__string outgoingUsername,
    xsd__string outgoingPassword,
    enum ns1__securitySetting outgoingSecurity,
    xsd__unsignedShort outgoingPort,
    xsd__string ownMailAddress,
    xsd__string ownName,
    enum ns1__sendOption sendOption,
    enum ns1__copyToOwn copyToOwnAddress,
    xsd__string signatureText,
    xsd__boolean newMailIndicators,
    enum ns1__retrievedParts retrievedParts,
    xsd__unsignedShort retrieveSizeLimit,
    xsd__unsignedShort emailsToRetrieve,
    xsd__string imap4FolderPath,
    enum ns1__automaticUpdate automaticUpdate,
    xsd__boolean setAsDefault,
    struct ns1__createMailboxResponse{} *out  //empty response
);

//gsoap ns1 service method-action: deleteMailbox "HtiMessages"
int ns1__deleteMailbox(
    xsd__string mailboxName,
    struct ns1__deleteMailboxResponse{} *out  //empty response
);

//gsoap ns1 service method-action: setDefaultSmsCenter "HtiMessages"
int ns1__setDefaultSmsCenter(
    xsd__string smsCenterName,
    xsd__string smsCenterNumber,
    struct ns1__setDefaultSmsCenterResponse{} *out  //empty response
);

//gsoap ns1 service method-action: deleteSmsCenter "HtiMessages"
int ns1__deleteSmsCenter(
    xsd__string smsCenterName,
    struct ns1__deleteSmsCenterResponse{} *out  //empty response
);

//gsoap ns1 service method-action: setSmsSettings "HtiMessages"
int ns1__setSmsSettings(
    enum ns1__smsCharacterSupport characterSupport,
    xsd__boolean requestDeliveryReport,
    enum ns1__msgValidity validityPeriod,
    enum ns1__smsMessageConversion messageConversion,
    enum ns1__smsPreferredConnection preferredConnection,
    xsd__boolean replyViaSameCenter,
    struct ns1__setSmsSettingsResponse{} *out  //empty response
);

//gsoap ns1 service method-action: setMmsSettings "HtiMessages"
int ns1__setMmsSettings(
	xsd__string accessPointName,
    enum ns1__mmsCreationMode creationMode,
    enum ns1__mmsImageSize imageSize,
    enum ns1__mmsReception reception,
    xsd__boolean receiveAnonymous,
    xsd__boolean receiveAdvertisements,
    xsd__boolean requestDeliveryReports,
    xsd__boolean sendDeliveryReports,
    enum ns1__msgValidity validityPeriod,
    struct ns1__setMmsSettingsResponse{} *out  //empty response
);

//gsoap ns1 service method-action: createDestination "HtiMessages"
int ns1__createDestination(
    xsd__string destinationName,
    struct ns1__createDestinationResponse{} *out //empty response
);

//gsoap ns1 service method-action: deleteDestination "HtiMessages"
int ns1__deleteDestination(
    xsd__string destinationName,
    struct ns1__deleteDestinationResponse{} *out //empty response
);

//gsoap ns1 service method-action: addToDestination "HtiMessages"
int ns1__addToDestination(
    xsd__string accessPointName,
    xsd__string destinationName,
    struct ns1__addToDestinationResponse{} *out //empty response
);

//gsoap ns1 service method-action: removeFromDestination "HtiMessages"
int ns1__removeFromDestination(
    xsd__string accessPointName,
    xsd__string destinationName,
    struct ns1__removeFromDestinationResponse{} *out //empty response
);
