/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file 
 @internalTechnology
*/
 
#ifndef DISPLAYTYPE_H
#define DISPLAYTYPE_H

#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <f32file.h>
#include <x509cert.h>

_LIT16(KCountryName,"CountryName");
_LIT16(KOrganizationName,"OrganizationName");
_LIT16(KOrganizationalUnitName,"OrganizationalUnitName");
_LIT16(KLocalityName,"LocalityName");
_LIT16(KStateOrProvinceName,"StateOrProvinceName");
_LIT16(KTitle,"Title");
_LIT16(KCommonName,"CommonName");
_LIT16(KGivenName,"GivenName");
_LIT16(KSurname,"Surname");
_LIT16(KInitials,"Initials");
_LIT16(KGenerationQualifier,"GenerationQualifier");
_LIT16(KDNQualifier,"DNQualifier");
_LIT16(KSerialNumber,"SerialNumber");
_LIT16(KPostalCode,"PostalCode");
_LIT16(KDomainComponent, "DomainComponent");
_LIT16(KStreet,"Street");


const TDesC16& GetType(const TDesC16& aType);

#endif

