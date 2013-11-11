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


#include "encdec.h"
#include <sstream>
#include <errno.h>
#include <s32ucmp.h>
#include "stringconv.h"
#include <stdio.h>
#include "x509utils.h"

RDecodeReadStream::RDecodeReadStream(CFileStore *aStore, RReadStream &aReadStream)
	: iStore(aStore), iCertBaseName(), iReadStream(aReadStream), iHumanReadable(false), iToken(), 
	  iPrefetchedTokenIsValid(false), iPrefetchedToken()
{
}

RDecodeReadStream::RDecodeReadStream(const std::string &aCertBaseName, RReadStream &aReadStream)
	: iStore(0), iCertBaseName(aCertBaseName), iReadStream(aReadStream), iHumanReadable(true), iToken(), 
	  iPrefetchedTokenIsValid(false), iPrefetchedToken()
{
}

void RDecodeReadStream::RawRead(void *aPtr, TUint32 aLength)
{
	iReadStream.ReadL((TUint8 *)aPtr, aLength);
}

void RDecodeReadStream::CheckName(const std::string &aExpected)
{
	ReadNextToken();

	if(iToken != aExpected)
		{
		dbg << Log::Indent() << "Expected token '" << aExpected <<"' but got token '" << iToken << "'" << Log::Endl();
		FatalError();
		}
	   
}

TUint32 ReadUnsignedNumber(std::string &aStr, size_t aSize)
{
	TUint32 val;
	std::istringstream ss(aStr);

	if((aStr.length() > 2) && 
	   (aStr[0] == '0') && 
	   ((aStr[1] == 'x') || (aStr[1] == 'X')))
		{
		// Hex number starting with 0x
		char ch;
		ss >> ch >> ch; // Discard the 0x
		ss >> std::hex >> val;
		}
	else
		{
		// Decimal number
		ss >> val;
		}

	// Now work out if we consumed the entire token without error
	if(ss.fail())
		{
		// Number decode failed
		dbg << Log::Indent() << "Failed to decode '" << aStr << "' as a number" << Log::Endl();
		FatalError();
		}

	// Make sure we consumed all data
	if(! ss.eof())
		{
		// Trailing chars on numeric token
		FatalError();
		}

	if(aSize != 4)
		{
		// Check range
		// nb the following check would fail if aSize==4 because x>>32 == x if sizeof(x)==4
		if((val >> (8*aSize)) != 0)
			{
			// Higher order bits are set above the size of the variable we
			// are returning into
			FatalError();
			}
		}
return val;
}


TUint32 RDecodeReadStream::ReadUnsignedNumber(size_t aSize)
{
BULLSEYE_OFF
	if((TUint32(aSize)>4))
		{
		FatalError();
		}
BULLSEYE_RESTORE

	ReadNextToken();

	return ::ReadUnsignedNumber(iToken, aSize);
}

const std::string &RDecodeReadStream::Token() const
{
	return iToken;
}

void RDecodeReadStream::ReadNextToken()
{
	if(iPrefetchedTokenIsValid)
		{
		// Copy prefetched token to current token
		iToken = iPrefetchedToken;
		iPrefetchedToken.clear();
		iPrefetchedTokenIsValid = false;
		return;
		}
		
	GetToken(iToken);
}

const std::string &RDecodeReadStream::PeakToken()
{
	if(!iPrefetchedTokenIsValid)
		{
		GetToken(iPrefetchedToken);
		iPrefetchedTokenIsValid = true;
		}

	return iPrefetchedToken;
}





bool RDecodeReadStream::HumanReadable() const
{
	return iHumanReadable;
}


void RDecodeReadStream::Close()
{
	iReadStream.Close();
}

void RDecodeReadStream::GetToken(std::string &aToken)
{
	aToken.clear();

	TUint8 ch;
	do 
		{
		iReadStream >> ch;
		if(ch == '#')
			{
			// Skip comment
			++ch;
			while((ch != '\r') && (ch != '\n'))
				{
				iReadStream >> ch;
				} 

			}
		} while((ch == ' ') || (ch == '\t') || (ch == '\r') || (ch == '\n'));
	
	if(ch == '"')
		{
		// Read a string
		iReadStream >> ch; // read first char
		while(ch != '"')
			{
			if(ch=='\\')
				{
				// \X causes X to always be saved (even if X is ")
				iReadStream >> ch;
				}
			
			aToken.push_back(ch);
			iReadStream >> ch;
			}
		// At this point ch contains WS so it can be discarded.
		return;
		}

	// Read a non-string token
	while((ch != ' ') && (ch != '\t') && (ch != '\r') && (ch != '\n'))
		{
		aToken.push_back(ch);
		iReadStream >> ch;
		}
	// At this point ch contains WS so it can be discarded.
	return;
}





REncodeWriteStream::REncodeWriteStream(CFileStore *aStore, RWriteStream &aWriteStream)
	: iStore(aStore),
	  iCertBaseName(), // not used for STORE based streams
	  iWriteStream(&aWriteStream),
	  iLogStream(0), 
	  iHumanReadable(false), iPemOut(false), iVerbose(false), iIndentLevel(0)
{
}

REncodeWriteStream::REncodeWriteStream(const std::string &aCertBaseName, RWriteStream &aWriteStream)
	: iStore(0), iCertBaseName(aCertBaseName), iWriteStream(&aWriteStream),
	  iLogStream(0), 
	  iHumanReadable(true), iPemOut(false), iVerbose(false), iIndentLevel(0)
{
}

REncodeWriteStream::REncodeWriteStream(Log &aLog)
	: iStore(0), iWriteStream(0), iLogStream(&aLog.Stream()), 
	  iHumanReadable(true), iPemOut(false), iVerbose(false), iIndentLevel(aLog.IndentLevel())
{
}

void REncodeWriteStream::WriteBin(const void *aPtr, TUint32 aLength)
{
	if(iWriteStream)
		{
		iWriteStream->WriteL((TUint8 *)aPtr, aLength);
		}
	if(iLogStream)
		{
		iLogStream->write((const char *)aPtr, aLength);
		iLogStream->flush();
		}
}

void REncodeWriteStream::WriteQuotedUtf8(const void *aStr, TUint32 aLength)
{
	std::string tmp((const char *)aStr, aLength);
	
	// Insert a backslash before any backslash chars
	size_t pos = 0;
	while((pos = tmp.find('\\', pos)) != std::string::npos)
		{
		tmp.insert(pos, "\\", 1);
		pos += 2;
		}

	// Insert a backslash before any double quote chars
	pos = 0;
	while((pos = tmp.find('"', pos)) != std::string::npos)
		{
		tmp.insert(pos, "\\", 1);
		pos += 2;
		}

	if(iWriteStream)
		{
		iWriteStream->WriteL((TUint8 *)tmp.data(), tmp.size());
		}
	if(iLogStream)
		{
		iLogStream->write(tmp.data(), tmp.size());
		iLogStream->flush();
		}
}

void REncodeWriteStream::WriteByte(TUint8 aByte)
{
	WriteBin(&aByte, 1);
}

void REncodeWriteStream::WriteCStr(const void *aCstr)
{
	WriteBin(aCstr, strlen((const char *)aCstr));
}

void REncodeWriteStream::WriteHexNumber(TUint32 aNumber)
{
	char buf[20];
	int len = sprintf(buf, "0x%x", aNumber);
	WriteBin(buf, len);
}


void REncodeWriteStream::WriteSpace()
{
BULLSEYE_OFF
	if(!iHumanReadable) return;
BULLSEYE_RESTORE
	WriteByte(' ');
}

void REncodeWriteStream::WriteLineEnd()
{
BULLSEYE_OFF
	if(!iHumanReadable) return;
BULLSEYE_RESTORE
#ifdef linux
	WriteByte('\n');
#else
	WriteBin("\r\n", 2);
#endif
}


void REncodeWriteStream::WriteIndent()
{
BULLSEYE_OFF
	if(!iHumanReadable) return;
BULLSEYE_RESTORE
	for(int i=0; i<iIndentLevel; ++i)
		{
		WriteByte('\t');
		}
}

void REncodeWriteStream::IncIndent()
{
	prog.IncIndent();
	++iIndentLevel;
BULLSEYE_OFF
	if(iIndentLevel < 0) FatalError();
BULLSEYE_RESTORE
}

void REncodeWriteStream::DecIndent()
{
	prog.DecIndent();
	--iIndentLevel;
BULLSEYE_OFF
	if(iIndentLevel < 0) FatalError();
BULLSEYE_RESTORE
}

bool REncodeWriteStream::HumanReadable() const
{
	return iHumanReadable;
}

bool &REncodeWriteStream::PemOut()
{
	return iPemOut;
}

bool &REncodeWriteStream::Verbose()
{
	return iVerbose;
}



std::string REncodeWriteStream::CertFileName(TUint32 aFormat, TUint32 aCertNumber)
{
	std::stringstream ss;
	ss << iCertBaseName;
	ss << "cert";
	ss << aCertNumber;
	if(aFormat == EX509Certificate)
		{
		if(iPemOut)
			{
			ss << ".pem";
			}
		else
			{
			ss << ".der";
			}
		}
	else
		{
		ss << ".dat";
		}
	
	return ss.str();
}



void REncodeWriteStream::Close()
{
BULLSEYE_OFF
	if(iWriteStream)
BULLSEYE_RESTORE
		{
		iWriteStream->Close();
		}
}

CFileStore *REncodeWriteStream::StoreObject()
{
BULLSEYE_OFF
	if(iStore == 0) FatalError();
BULLSEYE_RESTORE
	return iStore;
}

RWriteStream &REncodeWriteStream::StoreWriteStream()
{
BULLSEYE_OFF
	if(iWriteStream == 0) FatalError();
BULLSEYE_RESTORE
	return *iWriteStream;
}

bool REncodeWriteStream::Quiet() const
{
	return iLogStream != 0;
}

TUint8 fromHex(TUint8 ch)
	/**
	   Convert a single hex char from ascii
	 */
{
	// convert to lowercase
	if((ch >= 'A') && (ch <= 'F'))
		{
		ch -= ('A' -'a');
		}
	// Handle a-f
	if((ch >= 'a') && (ch <= 'f'))
		{
		return ch - 'a' + 10;
		}

	// Handle 0-9
	if((ch >= '0') && (ch <= '9'))
		{
		return ch - '0';
		}

	// Illegal
	FatalError();
	return 0xff;
}

EncDecContainerItem::~EncDecContainerItem()
{
}


#ifdef _BullseyeCoverage
#pragma BullseyeCoverage off
#endif
std::string EncDecContainerItem::ItemName() const
{
	// Should never be called
	exit(-1);
	std::string tmp;
	return tmp;
}

void EncDecContainerItem::SetItemName(const std::string &)
{
	// Should never be called
	exit(-1);
}
#ifdef _BullseyeCoverage
#pragma BullseyeCoverage restore
#endif



EncDecContainer::EncDecContainer(const char *aContainerName, EncDecContainerItemFactoryFunc *aFactory)
	: iName(aContainerName), iFactory(aFactory), iArray()
{
}

EncDecContainer::~EncDecContainer()
{
	reset();
}

void EncDecContainer::push_back(EncDecContainerItem *aItem)
{
	iArray.push_back(aItem);
}

const EncDecContainerItem &EncDecContainer::operator[](TUint32 aIndex) const
{
	return *iArray[aIndex];
}

EncDecContainerItem &EncDecContainer::operator[](TUint32 aIndex)
{
	return *iArray[aIndex];
}

TUint32 EncDecContainer::size() const
{
	return iArray.size();
}

void EncDecContainer::reset()
{
	while(!iArray.empty())
		{
		EncDecContainerItem *p = iArray.back();
		iArray.pop_back();
		delete p;
		}
}



void EncDecContainer::Encode(REncodeWriteStream &aWriteStream) const
{
	// Calculate Start/End container names
	std::string startName("Start");
	startName.append(iName);
	std::string endName("End");
	endName.append(iName);

	prog << Log::Indent() << "Writing " << startName << Log::Endl();

	TUint32 arrayLength = iArray.size();

	if(aWriteStream.HumanReadable())
		{
		// Human
		aWriteStream.WriteIndent();
		aWriteStream.WriteBin(startName.data(), startName.size());
		aWriteStream.WriteLineEnd();
		}
	else
		{
		// Binary
		// Write length of array
		aWriteStream.WriteBin(&arrayLength, sizeof(arrayLength));
		}
	
	aWriteStream.IncIndent();

	// Write array entries
	for(TUint32 i=0; i < arrayLength; ++i)
		{
		std::ostringstream entryComment;
		entryComment << "# Entry " << i+1;
		prog << Log::Indent() << entryComment.str() << Log::Endl();
		
		bool bracketItem = (iArray[i]->ItemType() != 0);
		if(aWriteStream.HumanReadable())
			{
			if(bracketItem)
				{
				std::ostringstream itemStart;
				itemStart << "Start" << iArray[i]->ItemType() << " " << iArray[i]->ItemName();
				prog << Log::Indent() << itemStart.str() << Log::Endl();
				
				aWriteStream.WriteIndent();
				aWriteStream.WriteCStr("Start");
				aWriteStream.WriteCStr(iArray[i]->ItemType());
				aWriteStream.WriteCStr(" \"");
				aWriteStream.WriteQuotedUtf8(iArray[i]->ItemName().data(), iArray[i]->ItemName().size());
				aWriteStream.WriteCStr("\"");
				aWriteStream.WriteLineEnd();
				}
			else
				{
				aWriteStream.WriteIndent();
				aWriteStream.WriteBin(entryComment.str().data(), entryComment.str().size());
				aWriteStream.WriteLineEnd();
				}
			}
		aWriteStream.IncIndent();
		iArray[i]->Encode(aWriteStream);
		aWriteStream.DecIndent();
		if(bracketItem && aWriteStream.HumanReadable())
			{
			std::ostringstream tmp;
			tmp << "End" << iArray[i]->ItemType();

			prog << Log::Indent() << tmp.str() << Log::Endl();

			aWriteStream.WriteIndent();
			aWriteStream.WriteBin(tmp.str().data(), tmp.str().size());
			aWriteStream.WriteLineEnd();
			}
		}

	aWriteStream.DecIndent();

	if(aWriteStream.HumanReadable())
		{
		// Human
		aWriteStream.WriteIndent();
		aWriteStream.WriteCStr("End");
		aWriteStream.WriteBin(iName.data(), iName.size());
		aWriteStream.WriteLineEnd();
		}

	prog << Log::Indent() << endName << Log::Endl();

	return;
}

void EncDecContainer::Decode(RDecodeReadStream &aReadStream)
{
	// Calculate Start/End container names
	std::string startName("Start");
	startName.append(iName);
	std::string endName("End");
	endName.append(iName);

	prog << Log::Indent() << "Reading " << startName << Log::Endl();

	if(aReadStream.HumanReadable())
		{
		// Human

		// Check/consume container StartX
		aReadStream.CheckName(startName);

		prog.IncIndent();
		
		// Create items until we find the container EndX
		TUint32 entryNum=1;
		while(aReadStream.PeakToken() != endName)
			{
			// Progress message
			prog << Log::Indent() << "# Entry " << std::dec << entryNum++ << std::hex << Log::Endl();

			EncDecContainerItem *item = iFactory();
			bool bracketedItem = (item->ItemType() != 0);
			std::string itemName;
			if(bracketedItem && aReadStream.HumanReadable())
				{
				// Bracketed item, so read ItemType tag and ItemName
				std::string itemStart("Start");
				itemStart.append(item->ItemType());
				aReadStream.CheckName(itemStart);
				prog << Log::Indent() << itemStart;

				// Read item name
				aReadStream.ReadNextToken();
				itemName = aReadStream.Token();
				}
		
			prog.IncIndent();
			item->Decode(aReadStream);
			iArray.push_back(item);
			prog.DecIndent();
			if(bracketedItem && aReadStream.HumanReadable())
				{
				// Bracketed item, so read End ItemType tag
				std::string itemEnd("End");
				itemEnd.append(item->ItemType());
				aReadStream.CheckName(itemEnd);
				// and set item name
				item->SetItemName(itemName);
				}
			}

		// Check/consume container EndX
		aReadStream.CheckName(endName);

		prog.DecIndent();
		}
	else
		{
		// Binary
		// Read number of entries
		TUint32 arrayLength;
		aReadStream.RawRead(&arrayLength, sizeof(arrayLength));

		prog.IncIndent();
		for(TUint32 i=0; i < arrayLength; ++i)
			{
			EncDecContainerItem *item = iFactory();
			prog << Log::Indent() << "# Entry " << std::dec << i+1 << std::hex << Log::Endl();
			prog.IncIndent();
			item->Decode(aReadStream);
			prog.DecIndent();
			iArray.push_back(item);
			}
		prog.DecIndent();
		}

	prog << Log::Indent() << endName << Log::Endl();

}




REncodeWriteStream& operator<<(REncodeWriteStream& aStream, const EncDecContainer &aContainer)
{
	aContainer.Encode(aStream);
	return aStream;
}

RDecodeReadStream& operator>>(RDecodeReadStream& aStream,EncDecContainer &aContainer)
{
	aContainer.Decode(aStream);
	return aStream;
}

//
// TUid
//
void EncodeHuman(REncodeWriteStream& aStream,const TUid &aUid)
{
	aStream.WriteHexNumber(aUid.iUid);
}
void DecodeHuman(RDecodeReadStream& aStream,TUid &aUid)
{
	aUid.iUid = aStream.ReadUnsignedNumber(sizeof(aUid.iUid));
}

//
// TName
//
void EncodeHuman(REncodeWriteStream& aStream,const TName &aName)
{
	// Compress the internal UTF-16 to human readable UTF-8
	;
	TInt outputBytes = 0;
	TUint8 *outBuf = cstrFromUtf16(aName.Ptr(), aName.Length(), outputBytes);
	
	aStream.WriteByte('"');
	aStream.WriteQuotedUtf8(outBuf, outputBytes);
	aStream.WriteByte('"');

	delete [] outBuf;
}
void DecodeHuman(RDecodeReadStream& aStream,TName &aName)
{
	aStream.ReadNextToken();

	// Expand UTF-8 into internal UTF-16LE representation
	TInt outputWords = 0;
	TText *outputBuf = utf16FromUtf8((const TUint8 *)aStream.Token().data(), aStream.Token().size(), outputWords);
	memcpy((void *)aName.Ptr(), outputBuf, outputWords*2);
	aName.SetLength(outputWords);
	delete [] outputBuf;
}

void readContainer(const std::string &aFileName, bool aHuman, EncDecContainer &container)
{
	if(aHuman)
		{
		FileReadStream fileReadStream(aFileName, true);
		std::string certBaseName(aFileName);
		size_t dotLoc = certBaseName.rfind('.');
		if(dotLoc != std::string::npos)
			{
			certBaseName.erase(dotLoc);
			}
		certBaseName += '_';
		RDecodeReadStream inStream(certBaseName, fileReadStream); // human readable
		inStream >> container;
		inStream.Close();
		}
	else
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		
		CPermanentFileStore* store = CPermanentFileStore::OpenLC(fs, _L16(aFileName.c_str()),EFileShareAny | EFileRead);
		store->SetTypeL(KPermanentFileStoreLayoutUid);

		RStoreReadStream rootStream;
		rootStream.OpenLC(*store, store->Root());
		TUint32 dataStreamId;
		rootStream >> dataStreamId;
		CleanupStack::PopAndDestroy(&rootStream); 

		RStoreReadStream dataStream;
		dataStream.OpenLC(*store, dataStreamId);

		RDecodeReadStream readStream(store, dataStream); // binary store

		readStream >> container;

		CleanupStack::PopAndDestroy(&dataStream); 
		CleanupStack::PopAndDestroy(store); 
		CleanupStack::PopAndDestroy(&fs); 
		}
}


void writeContainer(const char *aFileName, bool aHuman, bool aPemOut, bool aVerbose, const EncDecContainer &container)
{
	prog << Log::Indent() << "Writing output file '" << aFileName << "'" << Log::Endl();
	prog.IncIndent();
	if(aHuman)
		{
		FileWriteStream fileWriteStream(aFileName);
		// Calculate based name by striping last .xx extension and appending an underscore.
		std::string certBaseName(aFileName);
		size_t dotLoc = certBaseName.rfind('.');
		if(dotLoc != std::string::npos)
			{
			certBaseName.erase(dotLoc);
			}
		certBaseName += '_';
		
		REncodeWriteStream outStream(certBaseName, fileWriteStream); // human readable
		outStream.PemOut() = aPemOut;
		outStream.Verbose() = aVerbose;
		outStream << container;
		outStream.Close();
		}
	else
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);

		CFileStore* store = CPermanentFileStore::ReplaceLC(fs,_L16(aFileName),EFileShareAny | EFileRead | EFileWrite);
		store->SetTypeL(KPermanentFileStoreLayoutUid);
		
		RStoreWriteStream dataStream;
		TStreamId dataStreamId = dataStream.CreateLC(*store);

		REncodeWriteStream outStream(store, dataStream); // binary
		outStream << container;
		outStream.Close();

		dataStream.CommitL();
		dataStream.Close();

		RStoreWriteStream rootStream;
		TStreamId rootId = rootStream.CreateLC(*store);
		rootStream << dataStreamId;
		rootStream.CommitL();
		rootStream.Close();
		

		store->SetRootL(rootId);
		store->CommitL();
		CleanupStack::PopAndDestroy(&dataStream); 
		CleanupStack::PopAndDestroy(store); 
		CleanupStack::PopAndDestroy(&fs); 
		}
	prog.DecIndent();
}


// End of file
