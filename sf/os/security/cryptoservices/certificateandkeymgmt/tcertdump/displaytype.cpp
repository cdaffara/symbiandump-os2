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


#include "displaytype.h"

const TDesC16& GetType(const TDesC16& aType)
	{

	if (aType.Compare(KX520CountryName)==0)
		{
		return KCountryName;
		}
	else if (aType.Compare(KX520OrganizationName)==0)
		{
		return KOrganizationName;
		}
	else if (aType.Compare(KX520OrganizationalUnitName)==0)
		{
		return KOrganizationalUnitName;
		}
	else if (aType.Compare(KX520LocalityName)==0)
		{
		return KLocalityName;
		}
	else if (aType.Compare(KX520StateOrProvinceName)==0)
		{
		return KStateOrProvinceName;
		}
	else if (aType.Compare(KX520Title)==0)
		{
		return KTitle;
		}
	else if (aType.Compare(KX520CommonName)==0)
		{
		return KCommonName;
		}
	else if (aType.Compare(KX520GivenName)==0)
		{
		return KGivenName;
		}
	else if (aType.Compare(KX520Surname)==0)
		{
		return KSurname;
		}
	else if (aType.Compare(KX520Initials)==0)
		{
		return KInitials;
		}
	else if (aType.Compare(KX520GenerationQualifier)==0)
		{
		return KGenerationQualifier;
		}
	else if (aType.Compare(KX520DNQualifier)==0)
		{
		return KDNQualifier;
		}
	else if (aType.Compare(KX520PostalCode)==0)
		{
		return KPostalCode;
		}
	else if (aType.Compare(KRFC2247DomainComponent)==0)
		{
		return KDomainComponent;
		}
	else if (aType.Compare(KRFC2256Street)==0)
		{
		return KStreet;
		}
	else
		{
		return aType;
		}
	
	}

