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


#include "filecertstore.h"
#include "appuidmap.h"
#include "logger.h"
#include "stringconv.h"
#include "utils.h"
#include <iomanip>

EncDecContainerItem *AppUidListEntry::Factory()
{
	return new AppUidListEntry(AppUidMap::EnumEntries());
}

AppUidListEntry::AppUidListEntry(const EnumEntry *aEnumEntries)
	: EncDecContainerItem(), iUid("Application", aEnumEntries)
{
}

AppUidListEntry::~AppUidListEntry()
{
}

const char *AppUidListEntry::ItemType() const
{
	return 0; // n/a
}


void AppUidListEntry::Encode(REncodeWriteStream &aWriteStream)
{
	aWriteStream << iUid;
}

void AppUidListEntry::Decode(RDecodeReadStream &aReadStream)
{
	aReadStream >> iUid;
}


EncDecContainerItem *CertStoreEntry::Factory()
{
	return new CertStoreEntry;
}


static const EnumEntry enumDetailsForTBool[] =
{
    { "false", 0x00},
    { "true", 0x01},
    { "EFalse", false},
    { "ETrue", true},
	{ 0,0 }
};


CertStoreEntry::CertStoreEntry()
	: EncDecContainerItem(), 
	  iCertInfo(false),
	  iCertApps("ApplicationList", AppUidListEntry::Factory),
	  iTrusted("Trusted", enumDetailsForTBool), 
	  iReadDataStreamId("DataStreamId(read)", true),
	  iWriteDataStreamId("DataStreamId(write)", false),
	  iDataFileName("DataFileName"),
	  iCertData(),
	  iSwiMode(false)
{
	// We only need to initialise EncDecObject members which wrap non-class types
	iReadDataStreamId.Value() = 0;
	iWriteDataStreamId.Value() = 0;
}

CertStoreEntry::CertStoreEntry(bool aSwiMode)
	: EncDecContainerItem(), 
	  iCertInfo(aSwiMode),
	  iCertApps("ApplicationList", AppUidListEntry::Factory),
	  iTrusted("Trusted", enumDetailsForTBool), 
	  iReadDataStreamId("DataStreamId(read)", true),
	  iWriteDataStreamId("DataStreamId(write)", false),
	  iDataFileName("DataFileName"),
	  iCertData(),
	  iSwiMode(aSwiMode)
{
	// We only need to initialise EncDecObject members which wrap non-class types
	iReadDataStreamId.Value() = 0;
	iWriteDataStreamId.Value() = 0;
}

CertStoreEntry::~CertStoreEntry()
{
}

const TCertLabel &CertStoreEntry::Label() const
{
	return iCertInfo.Label();
}

CertInfo &CertStoreEntry::Info()
{
	return iCertInfo;
}

const CertInfo &CertStoreEntry::Info() const
{
	return iCertInfo;
}



const char *CertStoreEntry::ItemType() const
{
	return "Entry";
}

std::string CertStoreEntry::ItemName() const
{
	return stringFromUtf16(Label());
}


void CertStoreEntry::SetItemName(const std::string &aName)
{
	TInt outputWords;
	TText *outputBuf = utf16FromUtf8((const TUint8 *)aName.data(), aName.size(), outputWords);
	iCertInfo.Label() = TPtrC16(outputBuf, outputWords);
	delete [] outputBuf;
}


void CertStoreEntry::Encode(REncodeWriteStream &aWriteStream)
{
	iCertInfo.Encode(aWriteStream);
	aWriteStream << iCertApps;
	aWriteStream << iTrusted;
	if(aWriteStream.HumanReadable())
		{
		// Write data to a file

		// Generate a file name
		std::string certFileName = aWriteStream.CertFileName(iCertInfo.CertificateFormat(), iCertInfo.OutputCertificateId());
		iDataFileName.Value().Copy(TPtrC8((const TUint8*)certFileName.data(), certFileName.size()));
		
		// Write file name
		aWriteStream << iDataFileName;
		
		std::fstream certDataFile;
		OpenUtf8FStreamForWrite(certDataFile, certFileName.c_str());
		if(certDataFile.fail())
			{
			dbg << Log::Indent() << "Failed to open '" << certDataFile << "' for output!" << Log::Endl();
			FatalError();
			}
		if((iCertInfo.CertificateFormat() == EX509Certificate) && aWriteStream.PemOut())
			{
			std::string pemCert;
			Der2Pem(iCertData, pemCert);
			certDataFile.write(pemCert.data(), pemCert.size());
			}
		else
			{
			certDataFile.write(iCertData.data(), iCertData.size());
			}
		
		certDataFile.close();
		if(certDataFile.fail())
			{
			dbg << Log::Indent() << "Failed to write cert data to '" << certDataFile <<  Log::Endl();
			FatalError();
			}
		aWriteStream << iReadDataStreamId;
		}
	else
		{
		// Write to the store
		if(iCertData.size() != iCertInfo.CertSize())
			{
			dbg << Log::Indent() << "Internal error - cert data size does not match meta data" << Log::Endl();
			FatalError();
			}

		RStoreWriteStream dataStream;
		TStreamId dataStreamId = dataStream.CreateLC(*aWriteStream.StoreObject());
		prog << Log::Indent() << "Created store stream " << dataStreamId << " for certificate data" << Log::Endl();
		iWriteDataStreamId.Value() = dataStreamId;

		prog << Log::Indent() << "Writing " << iCertData.size() << " bytes of binary data" << Log::Endl();
		dataStream.WriteL((const TUint8 *)iCertData.data(), iCertData.size());

		CleanupStack::PopAndDestroy(&dataStream);
		aWriteStream << iWriteDataStreamId;
		}
}

void CertStoreEntry::Decode(RDecodeReadStream &aReadStream)
{
	iCertInfo.Decode(aReadStream);
	aReadStream >> iCertApps;
	if((!aReadStream.HumanReadable()) ||
	   (aReadStream.PeakToken() == iTrusted.Name()))
		{
		aReadStream >> iTrusted;
		}
	else
		{
		iTrusted.SetValue(true);
		}
	aReadStream >> iReadDataStreamId;
	if(aReadStream.HumanReadable())
		{
		aReadStream >> iDataFileName;
		// Read data from the specified file
		std::string nFileName = stringFromUtf16(iDataFileName.Value());
		
		std::fstream certDataFile;
		OpenUtf8FStreamForRead(certDataFile, nFileName.c_str());
		if(certDataFile.fail())
			{
			dbg << Log::Indent() << "Failed to open '" << nFileName << "' for input!" << Log::Endl();
			FatalError();
			}
		
		certDataFile.seekg(0, std::ios_base::end);
		TUint32 certSize = certDataFile.tellg();
		
		char *rawCertData = new char[certSize];
		
		certDataFile.seekg(0, std::ios_base::beg);
		certDataFile.read(rawCertData, certSize);
		
		certDataFile.close();
		if(certDataFile.fail())
			{
			dbg << Log::Indent() << "Failed to read cert data from '" << certDataFile << Log::Endl();
			FatalError();
			}
		iCertData.assign(rawCertData, certSize);
		delete [] rawCertData;
		
		if(iCertInfo.CertificateFormat() == EX509Certificate)
			{
			// It might be a PEM cert
			std::string derFromPem;
			if(Pem2Der(iCertData, derFromPem))
				{
				prog << Log::Indent() << "Converted PEM cert to DER" << Log::Endl();
				iCertData = derFromPem;
				certSize = iCertData.size();
				}
			}
		iCertInfo.SetCertSize(certSize);
		}
	else 
		{
		// Read data from the store
		RStoreReadStream dataStream;
		dataStream.OpenLC(*aReadStream.iStore, iReadDataStreamId.Value());

		TUint32 certSize = iCertInfo.CertSize();
		TUint8 * certData = new TUint8[certSize];

		prog << Log::Indent() << "Reading " << certSize << " byte certificate from store stream " << iReadDataStreamId.Value() << Log::Endl();
		
		dataStream.ReadL(certData, certSize);

		iCertData.assign((const char *)certData, certSize);
		
		CleanupStack::PopAndDestroy(&dataStream);
		}

	if(iCertInfo.CertificateFormat() == EX509Certificate)
		{
		TKeyIdentifier subjectKeyId;
		bool isCA = ( iCertInfo.CertificateOwnerType() != EUserCertificate );

		// nb. If processing a swicertstore we ignore any SubjectKeyId in the extension.
		if(X509SubjectKeyId((iSwiMode)?(KIgnoreCertificateExtension) : (KUseCertificateExtension), 
							false, isCA,
							iCertData,
							iCertSubject, subjectKeyId))
			{
			prog << Log::Indent() << "Subject = '" << iCertSubject << "'" << Log::Endl();

			prog << Log::Indent() << "Calculated SubjectKeyId is ";
			const TUint8 *p = subjectKeyId.Ptr();
			for(int i=0; i<subjectKeyId.Length(); ++i)
				{
				if(i) prog << ":";
				prog.Stream() << std::setfill('0') << std::setw(2) << int(p[i]);
				}
			prog.Stream() << std::setw(0);
			prog << Log::Endl();
			
			if(aReadStream.HumanReadable() && iCertInfo.SubjectKeyId().iAutoKey)
				{
				// Reading config file and auto set so copy generated
				// SubjectKeyId to value.
				prog << Log::Indent() << "Field set to auto so using calculated SubjectKeyId" << Log::Endl();;
				iCertInfo.SubjectKeyId().iHash = subjectKeyId;
				}
			else
				{
				// If the read value matches the calculated value then
				// set iAutoKey so we dump it as auto (with the value
				// as a comment).
				if(iCertInfo.SubjectKeyId().iHash == subjectKeyId)
					{
					prog << Log::Indent() << "Calculated SubjectKeyId matches value read from input so setting to auto" << Log::Endl();;
					iCertInfo.SubjectKeyId().iAutoKey = true;
					}
				else
					{
					prog << Log::Indent() << "Calculated SubjectKeyId does NOT match value read from input so setting to value read" << Log::Endl();;
					}
				
				}
			}
		}
}
	
CertStoreEntry& CertStoreEntry::operator= (const CertStoreEntry& aRhs)
{
	if(this == &aRhs) return *this; // handle self assignment

	EncDecContainerItem::operator=(*static_cast<const EncDecContainerItem *>(&aRhs));

	iCertInfo = aRhs.iCertInfo;

	iCertApps.reset();
	for(TUint32 i=0; i<aRhs.iCertApps.size(); ++i)
		{
		AppUidListEntry *newApp = new AppUidListEntry(AppUidMap::EnumEntries());
		const AppUidListEntry *oldApp = static_cast<const AppUidListEntry *>(&aRhs.iCertApps[i]);
		*newApp = *oldApp;
		iCertApps.push_back(newApp);
		}

	iTrusted = aRhs.iTrusted;
	iReadDataStreamId = aRhs.iReadDataStreamId;
	iWriteDataStreamId = aRhs.iWriteDataStreamId;
	iDataFileName = aRhs.iDataFileName;
	iCertData = aRhs.iCertData;

	iCertSubject = aRhs.iCertSubject;

	iSwiMode = aRhs.iSwiMode;
		
	return *this;
}

const TUint8 * CertStoreEntry::CertData() const
{
	return (const TUint8 *)iCertData.data();
}


const std::string &CertStoreEntry::CertSubject() const
{
	return iCertSubject;
}



// End of file
