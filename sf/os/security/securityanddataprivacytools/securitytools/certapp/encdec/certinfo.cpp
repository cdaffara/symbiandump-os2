/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "certinfo.h"
#include "stringconv.h"
#define KReadOnlyFlagMask 128

static const EnumEntry enumDetailsForTBool[] =
{
    { "false", false},
    { "true", true},
    { "EFalse", false},
    { "ETrue", true},
	{ 0,0 }
};


// Enum values for TCertificateFormat
static const EnumEntry enumDetailsForTCertificateFormat[] =
{
    { "EX509Certificate", EX509Certificate},
    { "EWTLSCertificate", EWTLSCertificate},
	{ "EX968Certificate", EX968Certificate},
	{ "EUnknownCertificate", EUnknownCertificate},
	{ "EX509CertificateUrl", EX509CertificateUrl},
	{ "EWTLSCertificateUrl", EWTLSCertificateUrl},
	{ "EX968CertificateUrl", EX968CertificateUrl},
	{ 0,0 }
};


static const EnumEntry enumDetailsForTCertificateOwnerType[] =
{
	{ "ECACertificate", ECACertificate},
	{ "EUserCertificate", EUserCertificate},
	{ "EPeerCertificate", EPeerCertificate},
	{ 0,0 }
};


CertInfo::CertInfo(bool aSwiMode)
	: iTmpCombinedDeletableAndFormat("Deletable/Format"), 
	  iDeletable("Deletable", enumDetailsForTBool, aSwiMode), 
	  iFormat("Format", enumDetailsForTCertificateFormat), 
	  iSize("Size", true), // Only supported as a comment in human mode
	  iLabel("Label"),
	  iReadCertificateId("CertId(read)", true),
	  iWriteCertificateId("CertId(write)", false),
	  iCertificateOwnerType("CertOwnerType", enumDetailsForTCertificateOwnerType),
	  iSubjectKeyId("SubjectKeyId"), iIssuerKeyId("IssuerKeyId"),
	  iSwiMode(aSwiMode)
	
{
	// We only need to initialise EncDecObject members which wrap non-class types
	iTmpCombinedDeletableAndFormat.Value() = 0;
	iSize.Value() = 0;
	iReadCertificateId.Value() = 0;
	iWriteCertificateId.Value() = 0;
	iCertificateOwnerType.Value() = 0;
}


void CertInfo::Encode(REncodeWriteStream &aWriteStream)
{
	if(aWriteStream.HumanReadable())
		{
		aWriteStream << iDeletable;
		aWriteStream << iFormat;
		}
	else
		{
		// Write the binary field containing both format and deletable
		// flag.
		//
		// iDeletable flag is the significant digit in order to store
		// the flag without changing the externalized record
		// format. The value is OPPOSITE for backward compatibility
		iTmpCombinedDeletableAndFormat.Value() = static_cast <TUint8>(iFormat.Value() | (iDeletable.Value() ? 0 : KReadOnlyFlagMask));
		aWriteStream << iTmpCombinedDeletableAndFormat;
		}
	

	aWriteStream << iSize;
	if(aWriteStream.HumanReadable())
		{
		// In human readable form the label has already been written as part of the item header
		// Write out certificate ID we read in
		aWriteStream << iReadCertificateId;
		}
	else
		{
		aWriteStream << iLabel;
		aWriteStream << iWriteCertificateId;
		}
	
	
	aWriteStream << iCertificateOwnerType;
	aWriteStream <<  iSubjectKeyId;
	aWriteStream <<  iIssuerKeyId;
}


void CertInfo::Decode(RDecodeReadStream &aReadStream)
{
	if(aReadStream.HumanReadable())
		{
		// Read the Deletable and Format fields
		aReadStream >> iDeletable;
		aReadStream >> iFormat;
		}
	else
		{
		// Read the binary field containing both format and deletable
		// flag.  
		//
		// iDeletable flag is the significant digit in order to store
		// the flag without changing the externalized record
		// format. The value is OPPOSITE for backward compatibility
		aReadStream >> iTmpCombinedDeletableAndFormat;

		iDeletable.SetValue((iTmpCombinedDeletableAndFormat.Value() & KReadOnlyFlagMask) == 0);
		iFormat.SetValue((iTmpCombinedDeletableAndFormat.Value() & ~KReadOnlyFlagMask));
		}
	
	aReadStream >> iSize;
	if(!aReadStream.HumanReadable())
		{
		aReadStream >> iLabel;
		}
	aReadStream >> iReadCertificateId;
	aReadStream >> iCertificateOwnerType;


	if(!aReadStream.HumanReadable() || (aReadStream.PeakToken() == iSubjectKeyId.Name()))
		{
		// Either in binary mode, or the next token is SubjectKeyId, so read the field
		aReadStream >> iSubjectKeyId;
		}
	else
		{
		// In human mode and field not present, so set it to auto
		iSubjectKeyId.Value().iAutoKey = true;
		iSubjectKeyId.Value().iHash.SetLength(0);
		}

	if(!aReadStream.HumanReadable() || (aReadStream.PeakToken() == iIssuerKeyId.Name()))
		{
		// Either in binary mode, or the next token is IssuerKeyId, so read the field
		aReadStream >> iIssuerKeyId;
		}
	else
		{
		// In human mode and field not present, so set it to auto
		iIssuerKeyId.Value().iAutoKey = true;
		iIssuerKeyId.Value().iHash.SetLength(0);
		}
}


TUint32 CertInfo::CertSize() const
{
	return iSize.Value();
}

void CertInfo::SetCertSize(TUint32 aSize)
{
	iSize.Value() = aSize;
}

const TCertLabel &CertInfo::Label() const
{
	return iLabel.Value();
}

TCertLabel &CertInfo::Label()
{
	return iLabel.Value();
}

TCertificateFormat CertInfo::CertificateFormat() const
{
	return (TCertificateFormat)iFormat.Value();
}

TCertificateOwnerType CertInfo::CertificateOwnerType() const
{
	return (TCertificateOwnerType)iCertificateOwnerType.Value();
}


KeyIdentifierObject &CertInfo::SubjectKeyId()
{
	return iSubjectKeyId.Value();
}

const KeyIdentifierObject &CertInfo::SubjectKeyId() const
{
	return iSubjectKeyId.Value();
}

KeyIdentifierObject &CertInfo::IssuerKeyId()
{
	return iIssuerKeyId.Value();
}

#ifdef _BullseyeCoverage
#pragma BullseyeCoverage off
#endif
const KeyIdentifierObject &CertInfo::IssuerKeyId() const
{
	return iIssuerKeyId.Value();
}
#ifdef _BullseyeCoverage
#pragma BullseyeCoverage restore
#endif

TUint32 CertInfo::OutputCertificateId() const
{
	return iWriteCertificateId.Value();
}


void CertInfo::SetOutputCertificateId(TUint32 aId)
{
	iWriteCertificateId.Value() = aId;
}




//
// TCertLabel
//
void EncodeHuman(REncodeWriteStream& aStream,const TCertLabel &aLabel)
{
	// Compress the internal UTF-16 to human readable UTF-8
	TInt outputBytes = 0;
	TUint8 *outBuf = cstrFromUtf16(aLabel.Ptr(), aLabel.Length(), outputBytes);
	
	aStream.WriteByte('"');
	aStream.WriteQuotedUtf8(outBuf, outputBytes);
	aStream.WriteByte('"');

	delete [] outBuf;
}
void DecodeHuman(RDecodeReadStream& aStream,TCertLabel &aLabel)
{
	aStream.ReadNextToken();

	// Expand UTF-8 into internal UTF-16LE representation
	TInt outputWords = 0;
	TText *outputBuf = utf16FromUtf8((const TUint8 *)aStream.Token().data(), aStream.Token().size(), outputWords);
	if(outputWords > aLabel.MaxLength())
		{
		dbg << Log::Indent() << "String too long" << Log::Endl();
		FatalError();
		}
	
	memcpy((void *)aLabel.Ptr(), outputBuf, outputWords*2);
	aLabel.SetLength(outputWords);
	delete [] outputBuf;
}




// End of file
