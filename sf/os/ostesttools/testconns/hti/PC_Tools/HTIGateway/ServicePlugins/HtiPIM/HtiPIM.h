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
//gsoap ns1 service name: HtiPIM
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char*			xsd__string;
typedef wchar_t*		_xsd__string;
typedef int				xsd__int;
typedef unsigned char	xsd__unsignedByte;
typedef unsigned short	xsd__unsignedShort;

struct ns1__HtiSoapAttachment
{
	xsd__string href;
	xsd__string mimeType;
};

enum SIMContactFieldType
{
	FieldTypeName = 1,
	FieldTypeSecondName,
	FieldTypeNumber,
	FieldTypeEmail,
	FieldTypeAdditionalNumber
};

struct ns1__HtiSIMContactField
{
	enum SIMContactFieldType	fieldType;
	xsd__string					fieldData;
};

struct ArrayOfHtiSIMContactFields
{
	struct ns1__HtiSIMContactField* __ptrHtiSIMContactField;
	int __size;
};

struct ns1__HtiSIMCardCaps
{
	xsd__unsignedByte	maxNumOfSecondNames;
	xsd__unsignedByte	maxNumOfAdditionalNums;
	xsd__unsignedByte	maxNumOfEmails;
	xsd__unsignedByte	maxLengthOfName;
	xsd__unsignedByte	maxLengthOfNumber;
	xsd__unsignedByte	maxLengthOfSecondName;
	xsd__unsignedByte	maxLengthOfAdditionalNum;
	xsd__unsignedByte	maxLenghtOfEmail;
	xsd__unsignedShort	totalSlots;
	xsd__unsignedShort	usedSlots;
};


// String versions
//gsoap ns1 service method-action: import_vCard "HtiPIM"
int ns1__import_vCard(
    xsd__string		vCard,
    xsd__int		&entryId
);
//gsoap ns1 service method-action: import_vCalendar "HtiPIM"
int ns1__import_vCalendar(
    xsd__string		vCal,
    xsd__int		&entryId
);
// DIME versions
//gsoap ns1 service method-action: import_vCardDime "HtiPIM"
int ns1__import_vCardDime(
    struct ns1__HtiSoapAttachment		*vCardDimeAttachment,
    xsd__int							&entryId
);
//gsoap ns1 service method-action: import_vCalendarDime "HtiPIM"
int ns1__import_vCalendarDime(
    struct ns1__HtiSoapAttachment		*vCalendarDimeAttachment,
    xsd__int							&entryId
);

//gsoap ns1 service method-action: deleteContactEntry "HtiPIM"
int ns1__deleteContactEntry(
    xsd__int		entryId,
    struct ns1__deleteContactEntryResponse{} *out // empty response
);
//gsoap ns1 service method-action: deleteAllContactEntries "HtiPIM"
int ns1__deleteAllContactEntries(
    void *_, // no 'in' parameter
    struct ns1__deleteAllContactEntriesResponse{} *out // empty response
);
//gsoap ns1 service method-action: deleteCalendarEntry "HtiPIM"
int ns1__deleteCalendarEntry(
    xsd__int		entryId,
    struct ns1__deleteCalendarEntryResponse{} *out // empty response
);
//gsoap ns1 service method-action: deleteAllCalendarEntries "HtiPIM"
int ns1__deleteAllCalendarEntries(
    void *_, // no 'in' parameter
    struct ns1__deleteAllCalendarEntriesResponse{} *out // empty response
);
struct ns1__getSIMCardCapsResponse { struct ns1__HtiSIMCardCaps _returnHtiSIMCardCaps; };
//gsoap ns1 service method-action: getSIMCardCaps "HtiPIM"
int ns1__getSIMCardCaps(
    void *_, // no 'in' parameter
    struct ns1__getSIMCardCapsResponse &r
);
//gsoap ns1 service method-action: importSIMContact "HtiPIM"
int ns1__importSIMContact(
    struct ArrayOfHtiSIMContactFields 	SIMContact,
    xsd__int							&entryId
);
//gsoap ns1 service method-action: deleteSIMContact "HtiPIM"
int ns1__deleteSIMContact(
    xsd__int	entryId,
	struct ns1__deleteSIMContactResponse{} *out // empty response
);
//gsoap ns1 service method-action: deleteAllSIMContacts "HtiPIM"
int ns1__deleteAllSIMContacts(
    void *_, // no 'in' parameter
	struct ns1__deleteAllSIMContactsResponse{} *out // empty response
);
//gsoap ns1 service method-action: notepadAddMemo "HtiPIM"
int ns1__notepadAddMemo(
    xsd__string		text,
    struct ns1__notepadAddMemoResponse{} *out // empty response
);
//gsoap ns1 service method-action: notepadAddMemoFromFile "HtiPIM"
int ns1__notepadAddMemoFromFile(
    xsd__string		filePath,
    struct ns1__notepadAddMemoFromFileResponse{} *out // empty response
);
//gsoap ns1 service method-action: notepadDeleteAll "HtiPIM"
int ns1__notepadDeleteAll(
    void *_, // no 'in' parameter
    struct ns1__notepadDeleteAllResponse{} *out // empty response
);
//gsoap ns1 service method-action: createBookmark "HtiPIM"
int ns1__createBookmark(
    xsd__string folderName,
    xsd__string bookmarkName,
    xsd__string url,
    xsd__string accessPointName,
    xsd__string userName,
    xsd__string password,
    xsd__int    &createdItemCount
);
//gsoap ns1 service method-action: deleteBookmark "HtiPIM"
int ns1__deleteBookmark(
    xsd__string folderName,
    xsd__string bookmarkName,
    xsd__int    &deletedItemCount
);
