/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <x509gn.h>
#include "asn1dec.h"

/*
 * __SECURITY_PLATSEC_ARCH__: Changes for platform security
 *
 * If this macro is defined, the represenation of TPtrC arrays is changed from
 * using CPtrCArray to RArray<TPtrC>.  This is because CPtrCArray is supplied by
 * bafl.dll, which is not trusted for TCB.  x509 needs to be trusted for TCB so
 * it can be used by software install, hence this dependency was removed.
 *
 * This changes is implmented as a series of macros for the basic operations,
 * to minimise the amount of conditional compilation.
 */


#define REP_INIT_L		/* do nothing */
#define REP_FINAL 		iRep.Close()
#define REP_APPEND_L(X)	User::LeaveIfError(iRep.Append(X))
#define REP_COUNT		iRep.Count()
#define REP_AT(X)		iRep[X]
#define REP_VAL			iRep
#define REP_VAL_TYPE	RArray<TPtrC>


_LIT(KX509RFC822HostDomainSeparator,"@");
_LIT(KX509SubdomainSeparator,".");
_LIT(KX509URISchemeSpecificStart, "//");
_LIT(KX509URIUserPasswordEnd, "@");
_LIT(KX509URIPortStart, ":");
_LIT(KX509URIurlPathStart, "/");

const TInt KX509URISchemeSpecificStartLength = 2;
const TInt KX509URIUserPasswordEndLength = 1;
const TInt KX509MaxDNSNameLabelLength = 63;

//superclass: common functionality for DNS names & RFC 822 email addresses
EXPORT_C CX509DomainName::~CX509DomainName()
	{
	REP_FINAL;
	delete iName;
	}

CX509DomainName::CX509DomainName()
	{
	}

EXPORT_C TPtrC CX509DomainName::Name() const 
	{
	return *iName;
	}

EXPORT_C TBool CX509DomainName::IsWithinSubtree(const CX509DomainName& aName) const
   	{
   	TInt myCount = REP_COUNT;
   	TInt otherCount = aName.REP_COUNT;
   	if (otherCount > myCount)
   		{
   		return EFalse;
   		}
   	for (TInt i = otherCount-1; i >= 0; i--)
   		{
   		myCount--;
   		if ( KErrNotFound == REP_AT(myCount).MatchF(aName.REP_AT(i)) )
   			{
   			return EFalse;
   			}
   		}
   	return ETrue;
   	}

TBool CX509DomainName::AddDomainL(TInt& aPos)
	{
	TInt end = iName->Length();
	FOREVER
		{
		if (!(AddSubdomainL(aPos)))
			{
			return EFalse;
			}
		if (aPos == end)
			{
			break;
			}
		AddSubdomainSeparatorL(aPos);
		}
	return ETrue;
	}

TBool CX509DomainName::AddSubdomainL(TInt& aPos)
	{
	TBool res = EFalse;
	TInt end = iName->Length();
	if (aPos >= end)
		{
		return res;
		}
	TPtrC whatsLeft(&(iName->operator[] (aPos)), end - aPos);
	TInt subdomainEnd = whatsLeft.FindF(KX509SubdomainSeparator);
	if (subdomainEnd == 0)
		{
		return res;
		}
	if (subdomainEnd == KErrNotFound)
		{
		subdomainEnd = end - aPos;
		}
	TPtrC subdomain(&whatsLeft[0], subdomainEnd);
	if (IsValidString(subdomain))
		{
		REP_APPEND_L(subdomain);
		res = ETrue;
		}
	aPos = aPos + subdomainEnd;
	return res;
	}

TBool CX509DomainName::AddSubdomainSeparatorL(TInt& aPos)
	{
	TBool res = EFalse;
	TInt end = iName->Length();
	if (end <= aPos)
		{
		return res;
		}
	TPtrC whatsLeft(&(iName->operator[] (aPos)), end - aPos);
	TInt separatorEnd = whatsLeft.FindF(KX509SubdomainSeparator);
	if (separatorEnd == 0)
		{
		TPtrC separator(&whatsLeft[0], 1);
		REP_APPEND_L(separator);
		aPos++;
		res = ETrue;
		}
	return res;
	}

TBool CX509DomainName::IsValidString(const TDesC& aStr) const
	{
	TInt pos=0;
	TInt end=aStr.Length()-1;
	if (end < 0)
		{
		return ETrue;	
		}
	while (pos<end && IsValidChar(aStr[pos]))
		{
		pos++;
		}
	return (pos==end && IsValidChar(aStr[end]));
	}

TBool CX509DomainName::IsAlpha(const TChar& aChar) const 
	{
	return (	((aChar >= 97) && (aChar <= 122))	||
				((aChar >= 65) && (aChar <= 90))	);
	}

TBool CX509DomainName::IsAlphaOrNum(const TChar& aChar) const
	{
	return ((IsAlpha(aChar)) ||
			((aChar >= 48) && (aChar <= 57)) || (aChar == 42));
	}

TBool CX509DomainName::IsValidChar(const TChar& aChar) const
	{
	//default implementation: must be letter, number or hyphen
	return ((IsAlphaOrNum(aChar)) ||
			(aChar == 45) || (aChar == 42));
	}

//RFC 822 email address
//subtree
EXPORT_C CX509RFC822NameSubtree* CX509RFC822NameSubtree::NewL(const TDesC8& aBinaryData)
	{
	CX509RFC822NameSubtree* self = CX509RFC822NameSubtree::NewLC(aBinaryData);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509RFC822NameSubtree* CX509RFC822NameSubtree::NewLC(const TDesC8& aBinaryData)
	{
	CX509RFC822NameSubtree* self = new(ELeave) CX509RFC822NameSubtree;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

void CX509RFC822NameSubtree::ConstructL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	TASN1DecIA5String encStr;
	iName = encStr.DecodeDERL(aBinaryData, pos);
	REP_INIT_L;
	//now, parse your data
	pos = 0;	
	AddLocalHostL(pos);
	AddSubdomainSeparatorL(pos);
	if (!(AddDomainL(pos)))
		{
		User::Leave(KErrArgument);
		}
	}

TBool CX509RFC822NameSubtree::AddLocalHostL(TInt& aPos)
	{
	TInt localHostLength = iName->FindF(KX509RFC822HostDomainSeparator);
	if ((localHostLength != KErrNotFound) && (localHostLength > 0))
		{
		TPtrC localHost(&(iName->operator[] (aPos)), localHostLength);		
		//the local host name is not checked here as it caused defect PDEF108960 
		//and for compatability with IE and Firefox.
		REP_APPEND_L(localHost);
		aPos = aPos + localHostLength;
		aPos++;		//skip the @ symbol
		return ETrue;
		}
	return EFalse;	//local host not found
	}


EXPORT_C const REP_VAL_TYPE& CX509RFC822NameSubtree::Rep() const
	{
	return REP_VAL;
	}

TBool CX509RFC822NameSubtree::IsValidChar(const TChar& aChar) const
	{
	//we permit "." here, 'cos it's allowed in local host names
	//and must have been stripped out by domain parsing code,
	//since it's the separator char
	return (	(aChar == 33)						||
				((aChar >= 35) && (aChar <= 40))	||
				(aChar == 42)						||
				(aChar == 43)						||
				((aChar >= 45) && (aChar <= 57))	||
				(aChar == 61)						||
				(aChar == 63)						||
				((aChar >= 65) && (aChar <= 90))	||
				((aChar >= 94) && (aChar <= 126))	);
	}

//full rfc 822 name: exactly as subtree, but requires local host and full domain name
EXPORT_C CX509RFC822Name* CX509RFC822Name::NewL(const TDesC8& aBinaryData)
	{
	CX509RFC822Name* self = CX509RFC822Name::NewLC(aBinaryData);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509RFC822Name* CX509RFC822Name::NewLC(const TDesC8& aBinaryData)
	{
	CX509RFC822Name* self = new(ELeave) CX509RFC822Name;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

void CX509RFC822Name::ConstructL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	TASN1DecIA5String encStr;
	iName = encStr.DecodeDERL(aBinaryData, pos);
	REP_INIT_L;
	//now, parse your data
	pos = 0;	
	if (! ((AddLocalHostL(pos)) && (AddDomainL(pos))) )
		{
		User::Leave(KErrArgument);
		}
	}

//DNS Name subtree
EXPORT_C CX509DNSNameSubtree* CX509DNSNameSubtree::NewL(const TDesC8& aBinaryData)
	{
	CX509DNSNameSubtree* self = CX509DNSNameSubtree::NewLC(aBinaryData);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509DNSNameSubtree* CX509DNSNameSubtree::NewLC(const TDesC8& aBinaryData)
	{
	CX509DNSNameSubtree* self = new(ELeave) CX509DNSNameSubtree;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

void CX509DNSNameSubtree::ConstructL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	TASN1DecIA5String encStr;
	iName = encStr.DecodeDERL(aBinaryData, pos);
	REP_INIT_L;
	pos = 0;
	AddSubdomainSeparatorL(pos);//a subtree may start with a period
	if (!(AddDomainL(pos)))
		{
		User::Leave(KErrArgument);
		}
	}

EXPORT_C const REP_VAL_TYPE& CX509DNSNameSubtree::Rep() const
	{
	return REP_VAL;
	}

TBool CX509DNSNameSubtree::IsValidString(const TDesC& aStr) const
	{
	//must be <= 63 chars long
	//must start with letter, end with letter or number
	TInt len = aStr.Length();
	return (	(len <= KX509MaxDNSNameLabelLength) &&
				(IsAlphaOrNum(aStr[0]))				&&
				(IsAlphaOrNum(aStr[len-1]))			&&
				(CX509DomainName::IsValidString(aStr))				);
	}

//dns name: exactly as subtree but requires full domain name
EXPORT_C CX509DNSName* CX509DNSName::NewL(const TDesC8& aBinaryData)
	{
	CX509DNSName* self = CX509DNSName::NewLC(aBinaryData);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509DNSName* CX509DNSName::NewLC(const TDesC8& aBinaryData)
	{
	CX509DNSName* self = new(ELeave) CX509DNSName;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

EXPORT_C CX509DNSName* CX509DNSName::NewL(const CX509DNSName& aName)
	{
	CX509DNSName* self = CX509DNSName::NewLC(aName);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509DNSName* CX509DNSName::NewLC(const CX509DNSName& aName)
	{	
	CX509DNSName* self = new(ELeave) CX509DNSName;
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;	
	}


EXPORT_C CX509DNSName* CX509DNSName::NewL(const TDesC& aNameString)
	{
	CX509DNSName* self = CX509DNSName::NewLC(aNameString);
	CleanupStack::Pop();//self
	return self;	
	}

EXPORT_C CX509DNSName* CX509DNSName::NewLC(const TDesC& aNameString)
	{
	CX509DNSName* self = new(ELeave) CX509DNSName;
	CleanupStack::PushL(self);
	self->ConstructL(aNameString);
	return self;
	}

void CX509DNSName::ConstructL(const TDesC& aNameString)
	{
	TInt pos = 0;
	REP_INIT_L;
	iName = aNameString.AllocL();
	AddSubdomainSeparatorL(pos);//a subtree may start with a period
	if (!(AddDomainL(pos)))
		{
		User::Leave(KErrArgument);
		}
	}

void CX509DNSName::ConstructL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	TASN1DecIA5String encStr;
	iName = encStr.DecodeDERL(aBinaryData, pos);
	ParseNameL();
	}

void CX509DNSName::ConstructL(const CX509DNSName& aName)
	{
	iName = aName.iName->AllocL();
	ParseNameL();
	}

void CX509DNSName::ParseNameL()
	{
	REP_INIT_L;
	TInt pos = 0;
	if  (!AddDomainL(pos))
		{
		User::Leave(KErrArgument);
		}
	}

//URI: must be of 'ip-based' form (rfc 1738 section 3.1)
//_and_ contain a domain name (not an IP address)
EXPORT_C CX509IPBasedURI* CX509IPBasedURI::NewL(const TDesC8& aBinaryData)
	{
	CX509IPBasedURI* self = CX509IPBasedURI::NewLC(aBinaryData);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509IPBasedURI* CX509IPBasedURI::NewLC(const TDesC8& aBinaryData)
	{
	CX509IPBasedURI* self = new(ELeave) CX509IPBasedURI;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

void CX509IPBasedURI::ConstructL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	TASN1DecIA5String encStr;
	iName = encStr.DecodeDERL(aBinaryData, pos);
	iHost = CX509DNSName::NewL(ExtractHostNameL());
	}

EXPORT_C CX509IPBasedURI::~CX509IPBasedURI()
	{
	delete iName;
	delete iHost;
	}

CX509IPBasedURI::CX509IPBasedURI()
	:iHost(NULL), iName(NULL)
	{
	}

EXPORT_C const CX509DNSName& CX509IPBasedURI::Host() const
	{
	return *iHost;
	}

EXPORT_C TPtrC CX509IPBasedURI::Name() const
	{
	return iName->Des();
	}

TPtrC CX509IPBasedURI::ExtractHostNameL() const
	{
	TInt hostStart;
	TInt hostEnd;
	TInt len = iName->Length();
	TInt schemeSpecificStart = (iName->FindF(KX509URISchemeSpecificStart) + KX509URISchemeSpecificStartLength);
	TInt userPasswordEnd = (iName->FindF(KX509URIUserPasswordEnd)) + KX509URIUserPasswordEndLength;
	hostStart = ((userPasswordEnd == 0)? schemeSpecificStart : userPasswordEnd);
	if (hostStart == KErrNotFound)
		{
		User::Leave(KErrArgument);
		}
	TPtrC whatsLeft(&(iName->operator[](hostStart)), len - hostStart);
	TInt newlen = whatsLeft.Length();
	TInt portStart = whatsLeft.FindF(KX509URIPortStart);
	TInt urlPathStart = whatsLeft.FindF(KX509URIurlPathStart);
	if (portStart == KErrNotFound)
		{
		if (urlPathStart == KErrNotFound)
			{
			hostEnd = newlen;
			}
		else
			{
			hostEnd = urlPathStart;
			}
		}
	else
		{
		if (urlPathStart == KErrNotFound)
			{
			hostEnd = portStart;
			}
		else //both are there, choose the first one
			{
			hostEnd = ((urlPathStart > portStart)? portStart: urlPathStart);
			}
		}
	TPtrC host(&(iName->operator[](hostStart)), hostEnd);
	return host;
	}

//IP Address
//subnet mask
EXPORT_C CX509IPSubnetMask* CX509IPSubnetMask::NewL(const TDesC8& aBinaryData)
	{
	CX509IPSubnetMask* self = CX509IPSubnetMask::NewLC(aBinaryData);
	CleanupStack::Pop();//self;
	return self;
	}

EXPORT_C CX509IPSubnetMask* CX509IPSubnetMask::NewLC(const TDesC8& aBinaryData)
	{
	CX509IPSubnetMask* self = new(ELeave) CX509IPSubnetMask;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

void CX509IPSubnetMask::ConstructL(const TDesC8& aBinaryData)
	{
	//!!!need to correct this when we have octet strings going!!!
	TASN1DecGeneric encAddr(aBinaryData);
	encAddr.InitL();
	iName = encAddr.GetContentDER().AllocL();// = CASN1OctetString::DecodeDERL(aBinaryData, pos);
	TInt len = iName->Length();
	if (!((len == 8) || (len == 32)))
		{
		User::Leave(KErrArgument);
		}
	}

EXPORT_C CX509IPSubnetMask::~CX509IPSubnetMask()
	{
	delete iName;
	}

EXPORT_C TPtrC8 CX509IPSubnetMask::BaseAddress() const
	{
	TInt half = iName->Length()/2;
	TPtrC8 ptr(&(iName->operator [] (0)), half);
	return ptr;
	}

EXPORT_C TPtrC8 CX509IPSubnetMask::Mask() const
	{
	TInt half = iName->Length()/2;
	TPtrC8 ptr(&(iName->operator [] (half)), half);
	return ptr;
	}

CX509IPSubnetMask::CX509IPSubnetMask()
	:iName(NULL)
	{
	}

//ip address
EXPORT_C CX509IPAddress* CX509IPAddress::NewL(const TDesC8& aBinaryData)
	{
	CX509IPAddress* self = CX509IPAddress::NewLC(aBinaryData);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509IPAddress* CX509IPAddress::NewLC(const TDesC8& aBinaryData)
	{
	CX509IPAddress* self = new(ELeave) CX509IPAddress;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData);
	return self;
	}

void CX509IPAddress::ConstructL(const TDesC8& aBinaryData)
	{
	TASN1DecGeneric encAddr(aBinaryData);
	encAddr.InitL();
	iName = encAddr.GetContentDER().AllocL();
	TInt len = iName->Length();
	if (!(len == 4 || len == 16))
		{
		User::Leave(KErrArgument);
		}
	}

EXPORT_C CX509IPAddress::~CX509IPAddress()
	{
	delete iName;
	}

EXPORT_C TBool CX509IPAddress::IsWithinSubtree(const CX509IPSubnetMask& aName) const
	{
	TInt addrLen = iName->Length();
	if (((aName.iName->Length())/2) !=  addrLen)
		{
		return EFalse;
		}
	for (TInt i = 0; i < addrLen; i++)
		{
		//stop stupid compiler warning
		TUint8 masked = (TUint8) ((iName->operator [] (i)) & (aName.iName->operator [] (i + addrLen)));
		if (masked != (aName.iName->operator [] (i)))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

EXPORT_C TPtrC8 CX509IPAddress::Address() const
	{
	return iName->Des();
	}

CX509IPAddress::CX509IPAddress()
	:iName(NULL)
	{
	}

//*******************X.509 General Name**********************//
EXPORT_C CX509GeneralName* CX509GeneralName::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509GeneralName::NewL(aBinaryData, pos);
	}

EXPORT_C CX509GeneralName* CX509GeneralName::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509GeneralName::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509GeneralName* CX509GeneralName::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509GeneralName* self = CX509GeneralName::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509GeneralName* CX509GeneralName::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509GeneralName* self = new(ELeave) CX509GeneralName;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509GeneralName* CX509GeneralName::NewL(const CX509GeneralName& aName)
	{
	CX509GeneralName* self = CX509GeneralName::NewLC(aName);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509GeneralName* CX509GeneralName::NewLC(const CX509GeneralName& aName)
	{
	CX509GeneralName* self = new(ELeave) CX509GeneralName(aName.iTag);
	CleanupStack::PushL(self);
	self->ConstructL(aName.iData->Des());
	return self;
	}

void CX509GeneralName::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric gen(aBinaryData.Right(aBinaryData.Length() - aPos));
	gen.InitL();
	aPos += gen.LengthDER();//add on header info
	if (gen.Class() != EContextSpecific)
		{
		User::Leave(KErrArgument);
		}
	iData = gen.Tag() == 4 ? gen.GetContentDER().AllocL(): gen.Encoding().AllocL();
	iTag = gen.Tag();
	}

void CX509GeneralName::ConstructL(const TDesC8& aData)
	{
	iData = aData.AllocL();
	}

CX509GeneralName::CX509GeneralName(TGNType aType)
	:iTag(aType)
	{
	}

CX509GeneralName::CX509GeneralName()
	{
	}

EXPORT_C TGNType CX509GeneralName::Tag() const
	{
	return iTag;
	}

EXPORT_C TPtrC8 CX509GeneralName::Data() const
	{
	return iData->Des();
	}

EXPORT_C TBool CX509GeneralName::ExactMatch(const CX509GeneralName& /*aName*/) const
	{
	return EFalse;
	}

EXPORT_C CX509GeneralName::~CX509GeneralName()
	{
	delete iData;
	}
