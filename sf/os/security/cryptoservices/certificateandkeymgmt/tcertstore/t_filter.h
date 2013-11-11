/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* T_FILTER_H_
*
*/


/**
 @file 
 @internalTechnology
*/


#ifndef T_FILTER_H_
#define T_FILTER_H_

#include "securitydefs.h"

#include <e32std.h>
#include <e32base.h>

class Output;
class CCertAttributeFilter;

class CFilter : public CBase
	{
	public:
		static CCertAttributeFilter* NewL(	Output& aOut, 
											const TDesC8& aFilter); 
	
	private:
		CFilter(Output& aOut);
		~CFilter();
		void ConstructL(const TDesC8& aFilter);
		
	private:
		// Set the UID filter.
		void SetLabelFilter(const TDesC8& aLabel);
		// Set uid filter.
		void SetUIDFilter(const TDesC8& aUid);
		// Set format filter.
		void SetFormatFilterL(const TDesC8& aFormatString);
		// Set ownertype filter.
		void SetOwnerTypeFilterL(const TDesC8& aOwnerType);
		// Set keyusage filter.
		void SetKeyUsageFilterL(const TDesC8& aKeyUsageDescription);
		// Returns the usage of the key based on the X509 scheme.
		TKeyUsageX509 ParseKeyUsageL(const TDesC8& keyUsage);
		// Returns the usage of the key based on the PKCS15 scheme.
		TKeyUsageX509 ParseKeyUsageL(const TDesC8& keyUsage, const TDesC8& keyType);
		// Sets the subjetkeyid filter.
		void SetSubjectKeyIdFilter(const TDesC8& aSubjectKeyId);
		// Sets the issuerkeyid which is synonymous to the authority key identifier.
		void SetIssuerKeyIdFilter(const TDesC8& aIssuerKeyId);
		// Sets the subjetkeyid filter using the certificate.
		void SetSubjectKeyIdFromCertFilterL(const TDesC8& aCertPath);
				
	
	private:
		static CCertAttributeFilter* iFilter;
		Output& iOut;
		
	};

#endif /*T_FILTER_H_*/
