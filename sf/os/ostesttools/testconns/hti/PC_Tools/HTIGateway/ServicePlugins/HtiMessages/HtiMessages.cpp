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
#include "HtiMessagesH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

//**********************************************************************************
// CONSTANTS
//
//**********************************************************************************

// commands
const unsigned char CMD_ADD_SMS_TO_INDOX        = 0x01;
const unsigned char CMD_ADD_MMS_TO_INDOX        = 0x02;
const unsigned char CMD_ADD_EMAIL               = 0x03;
const unsigned char CMD_ADD_IR_MESSAGE          = 0x04;
const unsigned char CMD_ADD_BT_MESSAGE          = 0x05;
const unsigned char CMD_ADD_SMART_MESSAGE       = 0x06;
const unsigned char CMD_ADD_AUDIO_MESSAGE       = 0x07;
const unsigned char CMD_DELETE_MESSAGE          = 0x10;
const unsigned char CMD_DELETE_ALL_FROM_FOLDER  = 0x11;
const unsigned char CMD_CREATE_MAILBOX          = 0x20;
const unsigned char CMD_DELETE_MAILBOX          = 0x21;
const unsigned char CMD_CREATE_ACCESS_POINT     = 0x30;
const unsigned char CMD_DELETE_ACCESS_POINT     = 0x31;
const unsigned char CMD_CREATE_DESTINATION      = 0x32;
const unsigned char CMD_DELETE_DESTINATION      = 0x33;
const unsigned char CMD_ADD_TO_DESTINATION      = 0x34;
const unsigned char CMD_REMOVE_FROM_DESTINATION = 0x35;
const unsigned char CMD_SET_DEFAULT_SMS_CENTER  = 0x40;
const unsigned char CMD_DELETE_SMS_CENTER       = 0x41;
const unsigned char CMD_SET_SMS_SETTINGS        = 0x42;
const unsigned char CMD_SET_MMS_SETTINGS        = 0x45;


//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// ns1__addSMS
//**********************************************************************************
int ns1__addSMS(struct soap* soap,
                char *fromField,
                char *description,
                char *messageBody,
                bool isNewFlag,
                bool isUnreadFlag,
                enum ns1__folder folder,
                int &messageId)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_SMS_TO_INDOX );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthWordZero( messageBody );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__addMMS
//**********************************************************************************
int ns1__addMMS(struct soap* soap,
                char *fromField,
                char *description,
                char *messageBody,
                char *attachmentPath,
                bool isNewFlag,
                bool isUnreadFlag,
                enum ns1__folder folder,
                int &messageId)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_MMS_TO_INDOX );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthWordZero( messageBody );
    msg.AddStringWithLengthByteZero( attachmentPath );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__addEMail
//**********************************************************************************
int ns1__addEMail(struct soap* soap,
                  char *fromField,
                  char *description,
                  char *messageBody,
                  char *attachmentPath,
                  bool isNewFlag,
                  bool isUnreadFlag,
                  enum ns1__folder folder,
                  int &messageId)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_EMAIL );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthWordZero( messageBody );
    msg.AddStringWithLengthByteZero( attachmentPath );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__addIRMessage
//**********************************************************************************
int ns1__addIRMessage(struct soap* soap,
                      char *fromField,
                      char *description,
                      char *attachmentPath,
                      bool isNewFlag,
                      bool isUnreadFlag,
                      enum ns1__folder folder,
                      int &messageId)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_IR_MESSAGE );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthByteZero( attachmentPath );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__addBTMessage
//**********************************************************************************
int ns1__addBTMessage(struct soap* soap,
                      char *fromField,
                      char *description,
                      char *attachmentPath,
                      bool isNewFlag,
                      bool isUnreadFlag,
                      enum ns1__folder folder,
                      int &messageId)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_BT_MESSAGE );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthByteZero( attachmentPath );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__addSmartMessage
//**********************************************************************************
int ns1__addSmartMessage(struct soap* soap,
                         char *fromField,
                         char *description,
                         char *messageBody,
                         bool isNewFlag,
                         bool isUnreadFlag,
                         enum ns1__folder folder,
                         int BIOmessageType,
                         int &messageId)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_SMART_MESSAGE );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthWordZero( messageBody );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    msg.AddInt( BIOmessageType );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__addAudioMessage
//**********************************************************************************
int ns1__addAudioMessage(struct soap* soap,
                         char *fromField,
                         char *description,
                         char *attachmentPath,
                         bool isNewFlag,
                         bool isUnreadFlag,
                         enum ns1__folder folder,
                         int &messageId)
{
    if ( check_mandatory_string_parameter( soap, attachmentPath, "attachmentPath" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_AUDIO_MESSAGE );
    msg.AddStringWithLengthByteZero( fromField );
    msg.AddStringWithLengthByteZero( description );
    msg.AddStringWithLengthByteZero( attachmentPath );
    msg.AddByte( isNewFlag );
    msg.AddByte( isUnreadFlag );
    msg.AddByte( folder );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    messageId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__deleteMessage
//**********************************************************************************
int ns1__deleteMessage(struct soap* soap,
                       int messageId,
                       struct ns1__deleteMessageResponse *out)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_MESSAGE );
    msg.AddInt( messageId );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteMessage
//**********************************************************************************
int ns1__deleteAllFromFolder(struct soap* soap,
                             enum ns1__messageType type,
                             enum ns1__folder folder,
                             struct ns1__deleteAllFromFolderResponse *out)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_ALL_FROM_FOLDER );
    msg.AddByte( folder );
    msg.AddByte( type );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__createAccessPoint
//**********************************************************************************
int ns1__createAccessPoint(struct soap* soap,
                           enum ns1__bearerType bearer,
                           char *apName,
                           struct ArrayOfHtiAPSettingFields APSettings,
                           int &entryId)
{
    if ( check_mandatory_string_parameter( soap, apName, "apName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_CREATE_ACCESS_POINT );
    msg.AddByte( bearer );
    msg.AddStringWithLengthByte( apName );
    msg.AddByte( (BYTE) APSettings.__size );

    // Loop through setting fields
    for ( int i = 0; i < APSettings.__size; i++ )
    {
        msg.AddByte( APSettings.__ptrHtiAPSettingField[i].fieldType );

        // fieldData cannot be empty
        if ( check_mandatory_string_parameter( soap,
               APSettings.__ptrHtiAPSettingField[i].fieldData,
               "HtiAPSettingField.fieldData" ) )
               return SOAP_FAULT;

        msg.AddStringWithLengthByte( APSettings.__ptrHtiAPSettingField[i].fieldData );
    }

    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    if ( msg.CheckCommandCode( 0xFF ) || msg.CheckMsgExactLen( 5 ) )
        return SOAP_FAULT;

    entryId = msg.GetInt( 1 );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__deleteAccessPoint
//**********************************************************************************
int ns1__deleteAccessPoint(struct soap* soap,
                           char *apName,
                           struct ns1__deleteAccessPointResponse *out)
{
    if ( check_mandatory_string_parameter( soap, apName, "apName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_ACCESS_POINT );
    msg.AddStringWithLengthByte( apName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__createMailbox
//**********************************************************************************
int ns1__createMailbox(struct soap* soap,
                       enum ns1__mailboxType mailboxType,
                       char *mailboxName,
                       char *incomingServer,
                       char *incomingAccessPoint,
                       char *incomingUsername,
                       char *incomingPassword,
                       enum ns1__securitySetting incomingSecurity,
                       unsigned short incomingPort,
                       bool aPopSecureLogin,
                       char *outgoingServer,
                       char *outgoingAccessPoint,
                       char *outgoingUsername,
                       char *outgoingPassword,
                       enum ns1__securitySetting outgoingSecurity,
                       unsigned short outgoingPort,
                       char *ownMailAddress,
                       char *ownName,
                       enum ns1__sendOption sendOption,
                       enum ns1__copyToOwn copyToOwnAddress,
                       char *signatureText,
                       bool newMailIndicators,
                       enum ns1__retrievedParts retrievedParts,
                       unsigned short retrieveSizeLimit,
                       unsigned short emailsToRetrieve,
                       char *imap4FolderPath,
                       enum ns1__automaticUpdate automaticUpdate,
                       bool setAsDefault,
                       struct ns1__createMailboxResponse *out)
{
    // Check for mandatory stuff
    if ( check_mandatory_string_parameter( soap, mailboxName, "mailboxName" ) )
        return SOAP_FAULT;

    if ( check_mandatory_string_parameter( soap, incomingServer, "incomingServer" ) )
        return SOAP_FAULT;

    if ( check_mandatory_string_parameter( soap, outgoingServer, "outgoingServer" ) )
        return SOAP_FAULT;

    if ( check_mandatory_string_parameter( soap, ownMailAddress, "ownMailAddress" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_CREATE_MAILBOX );
    msg.AddByte( mailboxType );
    msg.AddStringWithLengthByte( mailboxName );
    msg.AddStringWithLengthByte( incomingServer );
    msg.AddStringWithLengthByteZero( incomingAccessPoint );
    msg.AddStringWithLengthByteZero( incomingUsername );
    msg.AddStringWithLengthByteZero( incomingPassword );
    msg.AddByte( incomingSecurity );
    msg.AddWord( incomingPort );
    msg.AddByte( aPopSecureLogin );
    msg.AddStringWithLengthByte( outgoingServer );
    msg.AddStringWithLengthByteZero( outgoingAccessPoint );
    msg.AddStringWithLengthByteZero( outgoingUsername );
    msg.AddStringWithLengthByteZero( outgoingPassword );
    msg.AddByte( outgoingSecurity );
    msg.AddWord( outgoingPort );
    msg.AddStringWithLengthByte( ownMailAddress );
    msg.AddStringWithLengthByteZero( ownName );
    msg.AddByte( sendOption );
    msg.AddByte( copyToOwnAddress );
    msg.AddStringWithLengthWordZero( signatureText );
    msg.AddByte( newMailIndicators );
    msg.AddByte( retrievedParts );
    msg.AddWord( retrieveSizeLimit );
    msg.AddWord( emailsToRetrieve );
    msg.AddStringWithLengthByteZero( imap4FolderPath );
    msg.AddByte( automaticUpdate );
    msg.AddByte( setAsDefault );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteMailbox
//**********************************************************************************
int ns1__deleteMailbox(struct soap* soap,
                       char *mailboxName,
                       struct ns1__deleteMailboxResponse *out)
{
    if ( check_mandatory_string_parameter( soap, mailboxName, "mailboxName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_MAILBOX );
    msg.AddStringWithLengthByte( mailboxName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__setDefaultSmsCenter
//**********************************************************************************
int ns1__setDefaultSmsCenter(struct soap* soap,
                             char *smsCenterName,
                             char *smsCenterNumber,
                             struct ns1__setDefaultSmsCenterResponse *out)
{
    if ( check_mandatory_string_parameter( soap, smsCenterName, "smsCenterName" ) )
        return SOAP_FAULT;
    if ( check_mandatory_string_parameter( soap, smsCenterNumber, "smsCenterNumber" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_SET_DEFAULT_SMS_CENTER );
    msg.AddStringWithLengthByte( smsCenterName );
    msg.AddStringWithLengthByte( smsCenterNumber );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteSmsCenter
//**********************************************************************************
int ns1__deleteSmsCenter(struct soap* soap,
                         char *smsCenterName,
                         struct ns1__deleteSmsCenterResponse *out)
{
    if ( check_mandatory_string_parameter( soap, smsCenterName, "smsCenterName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_SMS_CENTER );
    msg.AddStringWithLengthByte( smsCenterName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__setSmsSettings
//**********************************************************************************
int ns1__setSmsSettings(struct soap* soap,
                        enum ns1__smsCharacterSupport characterSupport,
                        bool requestDeliveryReport,
                        enum ns1__msgValidity validityPeriod,
                        enum ns1__smsMessageConversion messageConversion,
                        enum ns1__smsPreferredConnection preferredConnection,
                        bool replyViaSameCenter,
                        struct ns1__setSmsSettingsResponse *out)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_SET_SMS_SETTINGS );
    msg.AddByte( characterSupport );
    msg.AddByte( requestDeliveryReport );
    msg.AddByte( validityPeriod );
    msg.AddByte( messageConversion );
    msg.AddByte( preferredConnection );
    msg.AddByte( replyViaSameCenter );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__setMmsSettings
//**********************************************************************************
int ns1__setMmsSettings(struct soap* soap,
                        char *accessPointName,
					    enum ns1__mmsCreationMode creationMode,
					    enum ns1__mmsImageSize imageSize,
					    enum ns1__mmsReception reception,
					    bool receiveAnonymous,
					    bool receiveAdvertisements,
					    bool requestDeliveryReports,
					    bool sendDeliveryReports,
 					    enum ns1__msgValidity validityPeriod,
                        struct ns1__setMmsSettingsResponse *out)
{
    if ( check_mandatory_string_parameter( soap, accessPointName, "accessPointName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_SET_MMS_SETTINGS );
    msg.AddStringWithLengthByte( accessPointName );
    msg.AddByte( creationMode );
    msg.AddByte( imageSize );
    msg.AddByte( reception );
    msg.AddByte( receiveAnonymous );
    msg.AddByte( receiveAdvertisements );
    msg.AddByte( requestDeliveryReports );
    msg.AddByte( sendDeliveryReports );
    msg.AddByte( validityPeriod );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__createDestination
//**********************************************************************************
int ns1__createDestination(struct soap* soap,
                           char *destinationName,
                           struct ns1__createDestinationResponse *out)
{
    if ( check_mandatory_string_parameter( soap, destinationName, "destinationName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_CREATE_DESTINATION );
    msg.AddStringWithLengthByte( destinationName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteDestination
//**********************************************************************************
int ns1__deleteDestination(struct soap* soap,
                           char *destinationName,
                           struct ns1__deleteDestinationResponse *out)
{
    if ( check_mandatory_string_parameter( soap, destinationName, "destinationName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_DESTINATION );
    msg.AddStringWithLengthByte( destinationName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__addToDestination
//**********************************************************************************
int ns1__addToDestination(struct soap* soap,
                          char *accessPointName,
                          char *destinationName,
                          struct ns1__addToDestinationResponse *out)
{
    if ( check_mandatory_string_parameter( soap, accessPointName, "accessPointName" ) )
        return SOAP_FAULT;

    if ( check_mandatory_string_parameter( soap, destinationName, "destinationName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_ADD_TO_DESTINATION );
    msg.AddStringWithLengthByte( accessPointName );
    msg.AddStringWithLengthByte( destinationName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__removeFromDestination
//**********************************************************************************
int ns1__removeFromDestination(struct soap* soap,
                               char *accessPointName,
                               char *destinationName,
                               struct ns1__removeFromDestinationResponse *out)
{
    if ( check_mandatory_string_parameter( soap, accessPointName, "accessPointName" ) )
        return SOAP_FAULT;

    if ( check_mandatory_string_parameter( soap, destinationName, "destinationName" ) )
        return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_REMOVE_FROM_DESTINATION );
    msg.AddStringWithLengthByte( accessPointName );
    msg.AddStringWithLengthByte( destinationName );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}
