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
#include "HtiPIMH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

//**********************************************************************************
// CONSTANTS
//
//**********************************************************************************

// commands
const unsigned char CMD_IMPORT_VCARD               = 0x01;
const unsigned char CMD_IMPORT_VCALENDAR           = 0x02;
const unsigned char CMD_DELETE_CONTACT_ENTRIES     = 0x03;
const unsigned char CMD_DELETE_CALENDAR_ENTRIES    = 0x04;
const unsigned char CMD_NOTEPAD_ADD_MEMO           = 0x05;
const unsigned char CMD_NOTEPAD_ADD_MEMO_FROM_FILE = 0x06;
const unsigned char CMD_NOTEPAD_DELETE_ALL         = 0x07;
const unsigned char CMD_SIM_CARD_INFORMATION	   = 0x10;
const unsigned char CMD_IMPORT_SIM_CONTACT	       = 0x11;
const unsigned char CMD_DELETE_SIM_CONTACT	       = 0x12;
const unsigned char CMD_CREATE_BOOKMARK	           = 0x1A;
const unsigned char CMD_DELETE_BOOKMARK	           = 0x1B;


//**********************************************************************************
// UTIL FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// GetSoapDIMEAttachment
//**********************************************************************************
int GetSoapDIMEAttachment(struct soap* soap,
					      char* href,
					      void*& data,
					      int &dataLen )
{
	struct soap_multipart *attachment;
	for ( attachment = soap->dime.list; attachment; attachment = attachment->next )
	{
		if( !strcmp( (*attachment).id, href ) )
		{
			data = attachment->ptr,
			dataLen = (int) (*attachment).size;
			return SOAP_OK;
		}
	}

	soap->error = soap_receiver_fault_format(soap, "HtiGateway", 
		"Did not find DIME attachment \"%s\"", href);
	return SOAP_FAULT;
}

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// ns1__import_vCard
//**********************************************************************************
int ns1__import_vCard(struct soap* soap,
					  char *vCard,
					  int &entryId)
{
	if(check_mandatory_string_parameter(soap, vCard, "vCard"))
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_UID, CMD_IMPORT_VCARD );
	msg.AddString( vCard );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 5 ) || msg.CheckCommandCode( 0xFF ) )
		return SOAP_FAULT;

	entryId = msg.GetInt( 1 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__import_vCardDime
//**********************************************************************************
int ns1__import_vCardDime(struct soap* soap,
                          struct ns1__HtiSoapAttachment *vCardDimeAttachment,
						  int &entryId)
{
	if( check_mandatory_string_parameter( soap, vCardDimeAttachment->href, 
		                                  "vCardDimeAttachment href" ) )
		return SOAP_FAULT;

	void* data = NULL;
	int dataLen = 0;
	if ( GetSoapDIMEAttachment( soap, vCardDimeAttachment->href, data, dataLen ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_UID, CMD_IMPORT_VCARD );
	msg.AddData( data, dataLen );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 5 ) || msg.CheckCommandCode( 0xFF ) )
		return SOAP_FAULT;

	entryId = msg.GetInt( 1 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__import_vCalendar
//**********************************************************************************
int ns1__import_vCalendar(struct soap* soap,
						  char *vCal, 
						  int &entryId)
{
	if(check_mandatory_string_parameter(soap, vCal, "vCalendar"))
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_UID, CMD_IMPORT_VCALENDAR );
	msg.AddString( vCal );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 5 ) || msg.CheckCommandCode( 0xFF ) )
		return SOAP_FAULT;

	entryId = msg.GetInt( 1 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__import_vCalendarDime
//**********************************************************************************
int ns1__import_vCalendarDime(struct soap* soap,
                              struct ns1__HtiSoapAttachment *vCalendarDimeAttachment,
							  int &entryId)
{
	if(check_mandatory_string_parameter(soap, vCalendarDimeAttachment->href, 
		                                "vCalendarDimeAttachment href"))
		return SOAP_FAULT;

	void* data = NULL;
	int dataLen = 0;
	if ( GetSoapDIMEAttachment( soap, vCalendarDimeAttachment->href, data, dataLen ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_UID, CMD_IMPORT_VCALENDAR );
	msg.AddData( data, dataLen );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckMsgExactLen( 5 ) || msg.CheckCommandCode( 0xFF ) )
		return SOAP_FAULT;

	entryId = msg.GetInt( 1 );

	return SOAP_OK;
}

//**********************************************************************************
// ns1__deleteContactEntry
//**********************************************************************************
int ns1__deleteContactEntry(struct soap* soap,
							int entryId,
                            struct ns1__deleteContactEntryResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_CONTACT_ENTRIES );
	msg.AddInt( entryId );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteAllContactEntries
//**********************************************************************************
int ns1__deleteAllContactEntries(struct soap* soap,
								 void *_,
                                 struct ns1__deleteAllContactEntriesResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_CONTACT_ENTRIES );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteCalendarEntry
//**********************************************************************************
int ns1__deleteCalendarEntry(struct soap* soap,
							 int entryId,
                             struct ns1__deleteCalendarEntryResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_CALENDAR_ENTRIES );
	msg.AddInt( entryId );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteAllCalendarEntries
//**********************************************************************************
int ns1__deleteAllCalendarEntries(struct soap* soap,
								  void *_,
                                  struct ns1__deleteAllCalendarEntriesResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_CALENDAR_ENTRIES );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__getSIMCardCaps
//**********************************************************************************
int ns1__getSIMCardCaps(struct soap* soap,
						void *_,
                        struct ns1__getSIMCardCapsResponse &r)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_SIM_CARD_INFORMATION );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;
	
	if ( msg.CheckCommandCode( 0xFF ) || msg.CheckMsgExactLen( 13 ) )
		return SOAP_FAULT;

	r._returnHtiSIMCardCaps.maxNumOfSecondNames			= msg.GetByte( 1 );
	r._returnHtiSIMCardCaps.maxNumOfAdditionalNums		= msg.GetByte( 2 );
	r._returnHtiSIMCardCaps.maxNumOfEmails				= msg.GetByte( 3 );
	r._returnHtiSIMCardCaps.maxLengthOfName				= msg.GetByte( 4 );
	r._returnHtiSIMCardCaps.maxLengthOfNumber			= msg.GetByte( 5 );
	r._returnHtiSIMCardCaps.maxLengthOfSecondName		= msg.GetByte( 6 );
	r._returnHtiSIMCardCaps.maxLengthOfAdditionalNum	= msg.GetByte( 7 );
	r._returnHtiSIMCardCaps.maxLenghtOfEmail			= msg.GetByte( 8 );
	r._returnHtiSIMCardCaps.totalSlots					= msg.GetWord( 9 );
	r._returnHtiSIMCardCaps.usedSlots					= msg.GetWord( 11 );
	
	return SOAP_OK;
}

//**********************************************************************************
// ns1__importSIMContact
//**********************************************************************************
int ns1__importSIMContact(struct soap* soap,
                          struct ArrayOfHtiSIMContactFields SIMContacts,
						  int &entryId)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_IMPORT_SIM_CONTACT );

	// Only one byte for number of contacts
	if ( SIMContacts.__size > 0xFF )
	{
		soap->error = soap_receiver_fault( soap, "HtiGateway", 
			"Too many contacts" );
		return SOAP_FAULT;
	}
	msg.AddByte( (BYTE) SIMContacts.__size );

	// Loop through contacts
	for ( int i = 0; i < SIMContacts.__size; i++ )
	{
		msg.AddByte( SIMContacts.__ptrHtiSIMContactField[i].fieldType );
		
		// fieldData cannot be empty
		if ( check_mandatory_string_parameter( soap, 
			   SIMContacts.__ptrHtiSIMContactField[i].fieldData,
			   "HtiSIMContactField.fieldData" ) )
			   return SOAP_FAULT;
		
		msg.AddStringWithLengthByte( SIMContacts.__ptrHtiSIMContactField[i].fieldData );
	}

	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckCommandCode( 0xFF ) || msg.CheckMsgExactLen( 5 ) )
		return SOAP_FAULT;

	entryId = msg.GetInt( 1 );

    return SOAP_OK;	
}

//**********************************************************************************
// ns1__deleteSIMContact
//**********************************************************************************
int ns1__deleteSIMContact(struct soap* soap,
						  int entryId,
                          struct ns1__deleteSIMContactResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_SIM_CONTACT );
	msg.AddInt( entryId );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__deleteAllSIMContacts
//**********************************************************************************
int ns1__deleteAllSIMContacts(struct soap* soap,
							  void *_,
                              struct ns1__deleteAllSIMContactsResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_SIM_CONTACT );
	// 60 second timeout maybe not enough(?)
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_60_SECONDS ); 
}

//**********************************************************************************
// ns1__notepadAddMemo
//**********************************************************************************
int ns1__notepadAddMemo(struct soap* soap,
						char *text,
                        struct ns1__notepadAddMemoResponse *out)
{
	if( check_mandatory_string_parameter( soap, text, "text" ) )
		return SOAP_FAULT;

	HtiMsgHelper msg( soap, HTI_UID, CMD_NOTEPAD_ADD_MEMO );
	msg.AddString( text );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}
//**********************************************************************************
// ns1__notepadAddMemoFromFile
//**********************************************************************************
int ns1__notepadAddMemoFromFile(struct soap* soap,
						        char *filePath,
                                struct ns1__notepadAddMemoFromFileResponse *out)
{
	if( check_mandatory_string_parameter( soap, filePath, "filePath" ) )
		return SOAP_FAULT;
	
	HtiMsgHelper msg( soap, HTI_UID, CMD_NOTEPAD_ADD_MEMO_FROM_FILE );
	msg.AddString( filePath );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__notepadDeleteAll
//**********************************************************************************
int ns1__notepadDeleteAll(struct soap* soap,
						  void *_,
                          struct ns1__notepadDeleteAllResponse *out)
{
	HtiMsgHelper msg( soap, HTI_UID, CMD_NOTEPAD_DELETE_ALL );
	return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}

//**********************************************************************************
// ns1__createBookmark
//**********************************************************************************
int ns1__createBookmark(struct soap* soap,
					    char *folderName,
                        char *bookmarkName,
                        char *url,
                        char *accessPointName,
                        char *userName,
                        char *password,
                        int  &createdItemCount)
{
    if( check_mandatory_string_parameter( soap, bookmarkName, "bookmarkName" ) )
		return SOAP_FAULT;
    if( check_mandatory_string_parameter( soap, url, "url" ) )
		return SOAP_FAULT;

    HtiMsgHelper msg( soap, HTI_UID, CMD_CREATE_BOOKMARK );
    msg.AddStringWithLengthByteZero( folderName );
    msg.AddStringWithLengthByte( bookmarkName );
    msg.AddStringWithLengthWordZero( url );
    msg.AddStringWithLengthByteZero( accessPointName );
    msg.AddStringWithLengthByteZero( userName );
    msg.AddStringWithLengthByteZero( password );

 	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

    if ( msg.CheckCommandCode( 0xFF ) || msg.CheckMsgExactLen( 2 ) )
        return SOAP_FAULT;

    createdItemCount = msg.GetByte( 1 );

	return SOAP_OK;
}
//**********************************************************************************
// ns1__deleteBookmark
//**********************************************************************************
int ns1__deleteBookmark(struct soap* soap,
					    char *folderName,
                        char *bookmarkName,
                        int  &deletedItemCount)
{
    HtiMsgHelper msg( soap, HTI_UID, CMD_DELETE_BOOKMARK );
    msg.AddStringWithLengthByteZero( folderName );
    msg.AddStringWithLengthByteZero( bookmarkName );

 	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

    if ( msg.CheckCommandCode( 0xFF ) || msg.CheckMsgExactLen( 2 ) )
        return SOAP_FAULT;

    deletedItemCount = msg.GetByte( 1 );

	return SOAP_OK;
}
