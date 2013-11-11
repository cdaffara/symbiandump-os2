/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef SIMPLEOBEXAPPCONSTANTS_H
#define SIMPLEOBEXAPPCONSTANTS_H

/**
 * This file defines the constants that are used in the simpleObexApp
 * program.
 */

/* 
 Casira address for OBEX over Bluetooth
 This value needs to be changed
 to the address that your OBEX server will listen on
 */
TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5b019a36));	
/* The UUID for FTP in SDP. */
TUUID ftpUuid(0x1106);

/* This defines the size to expand the buffer by when additional space is required */
const TUint KBufExpandSize=8;
const TUint KServerBufExpandSize=500;

/* Service name attribute is offset with the language base value */
const TUint KSdpAttrIdServiceName=KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName;

/* This value is used as when a port is found it will be between 0 - 30 */
const TUint8 KNotAPort = 0xFF;
const TUint KNoPort = 0;
/* Obex test port value */
static const TUint8 KObexTestPort = 17;
/* RFCOMM protocol UUID, not in btsdp.h at time of writing */ 
static const TInt KRfcommUuid = 0x0003;
/* Obex Protocol UUID, not in btsdp.h at time of writing */
static const TInt KObexProtocolUuid = 0x0003;


/* String Literals used in application */
_LIT(KObjectType,"text/x-vcard");
_LIT(KClientInterfaceDescriptor,"Client Interface");
_LIT(KServerInterfaceDescriptor,"Server Interface");
_LIT8(KIrdaTransportAttrName, "IrDA:TinyTP:LsapSel");
_LIT8(KIrdaClassName, "OBEX");
_LIT8(KLocalInfoAppend," EikIrOBEXFile ");
_LIT(KFilePath1,"Z:\\private\\00000000\\Contactsbak.vcf");
_LIT(KFilePath2,"Z:\\private\\00000000\\Contactsbak2.vcf");
_LIT(KFilePath3,"Z:\\private\\00000000\\Contactsbak3.vcf");
_LIT(KFilename1,"Contacts.vcf");
_LIT(KFilename2,"Contacts2.vcf");
_LIT(KFilename3,"Contacts3.vcf");
_LIT(KAlreadyActive,"\r\nError: Client handler already active\r\n");
_LIT(KAuthPassword,"password");
_LIT8(KServerDesC,"File transfer server");

#define EPOCIDENT _L8("EPOC32 ER5")

#endif // SIMPLEOBEXAPPCONSTANTS_H