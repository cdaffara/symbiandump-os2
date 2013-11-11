// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __PARSER_H__
#define __PARSER_H__

/** 
@file

XML Framework CParser API header file.

@publishedAll
@released
*/

#include <stringpool.h>
#include <xml/contentprocessoruids.h> // needed for the typedef


class RFs;
class RFile;

namespace Xml
{

class MContentHandler;
class RStringDictionaryCollection;
class TParserImpl;
class CMatchData;

/** 
If you need to parse xml this is the class you need to use.

Default plain-text XML parser plug-in  supports following character set encodings: 

ISO_8859_1
US_ASCII
UTF_8
UTF_16
UTF_16BE
UTF_16LE

@publishedAll
@released
*/
class CParser : public CBase
	{
public:
	static IMPORT_C CParser* NewL(const TDesC8& aParserMimeType, MContentHandler& aCallback);
	static IMPORT_C CParser* NewLC(const TDesC8& aParserMimeType, MContentHandler& aCallback);
	
	static IMPORT_C CParser* NewL(const CMatchData& aCriteria, MContentHandler& aCallback);
	static IMPORT_C CParser* NewLC(const CMatchData& aCriteria, MContentHandler& aCallback);

	virtual ~CParser();

	IMPORT_C void ParseBeginL();
	IMPORT_C void ParseBeginL(const TDesC8& aDocumentMimeType);
	IMPORT_C void ParseBeginL(const CMatchData& aCriteria);	
	IMPORT_C void ParseL(const TDesC8& aFragment);
	IMPORT_C void ParseEndL();
	
	IMPORT_C void SetProcessorChainL(const RContentProcessorUids& aPlugins);

	IMPORT_C TInt EnableFeature(TInt aParserFeature);
	IMPORT_C TInt DisableFeature(TInt aParserFeature);
	IMPORT_C TBool IsFeatureEnabled(TInt aParserFeature) const;

	IMPORT_C void AddPreloadedDictionaryL(const TDesC8& aPublicId);

	IMPORT_C RStringPool& StringPool();
	IMPORT_C RStringDictionaryCollection& StringDictionaryCollection();
	
private:
	CParser();
	void ConstructL(const TDesC8& aParserMimeType, MContentHandler& aCallback);
	void ConstructL(const CMatchData& aMatchData, MContentHandler& aCallback);
	TParserImpl* iImpl;
	};

IMPORT_C void ParseL(CParser& aParser, const TDesC8& aDocument);
IMPORT_C void ParseL(CParser& aParser, RFs& aFs, const TDesC& aFilename);
IMPORT_C void ParseL(CParser& aParser, RFile& aFile);
}

#endif //__PARSER_H__
