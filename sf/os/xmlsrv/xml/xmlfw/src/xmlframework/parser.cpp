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

#include <f32file.h>

#include <xml/parser.h>
#include <xml/matchdata.h>

#include "parserimpl.h"


using namespace Xml;


CParser::CParser() : CBase() , iImpl(0)
	{
	}

/**  This method creates a parser that is ready to parse documents 
of the specified mime type.

If there are multiple parser plugins in the system which can parse the 
mime type, the XML framework will choose a parser. 

The criteria used to choose a parser, from many matching parsers, is as follows:

- A Symbian-supplied parser (with variant field set to "Symbian") will be selected by default. 
If there are multiple Symbian-supplied parsers, the one with the lowest Uid will be selected. 
Otherwise, the non-Symbian parser with the lowest Uid will be selected.

@return	A constructed parser

@param	aParserMimeType The mime type of the documents to parse (e.g. "txt/xml").
@param	aCallback 		The handler for parser generated events.

@leave KErrNoMemory If there is not enough memory to create parser or
					one of system wide error codes. 
					
@leave KErrXmlParserPluginNotFound
					If Xml framework is unable to find a parser 	
					based on data provided in aParserMimeType.
					
@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@see CParser::NewL(const CMatchData& aCriteria, MContentHandler& aCallback)
*/
EXPORT_C CParser* CParser::NewL(const TDesC8& aParserMimeType, MContentHandler& aCallback)
	{
	CParser* self=NewLC(aParserMimeType, aCallback);
	CleanupStack::Pop(self);
	return self;
	}

/**  This method is similar to NewL, but leaves the created parser on the
cleanup stack.

@return	A constructed parser

@param	aParserMimeType The mime type of the documents to parse (e.g. "txt/xml").
@param	aCallback 		The handler for parser generated events.

@leave KErrNoMemory If there is not enough memory to create parser or 
					one of system wide error codes. 
					
@leave KErrXmlParserPluginNotFound
					If Xml framework is unable to find a parser 	
					based on data provided in aParserMimeType.
		
@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@see CParser::NewL(const TDesC8& aParserMimeType, MContentHandler& aCallback)		
@see CParser::NewLC(const CMatchData& aCriteria, MContentHandler& aCallback)					
*/
EXPORT_C CParser* CParser::NewLC(const TDesC8& aParserMimeType, MContentHandler& aCallback)
	{
	CParser* self = new (ELeave) CParser;
	CleanupStack::PushL(self);
	self->ConstructL(aParserMimeType, aCallback);
	return self;
	}
		
/**  
This method constructs the object with default settings

@param aParserMimeType Parser Mime type
@param aCallback Event handler

@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@internalComponent
*/
		
void CParser::ConstructL(const TDesC8& aParserMimeType, MContentHandler& aCallback)
	{
	// create parser implementaion class if or created yet. 
	if (!iImpl)
		{
		iImpl = new (ELeave) TParserImpl;
		}
		
	// create CMatchData object with default parameters and adequate mime type
	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(aParserMimeType);
	iImpl->OpenL(*matchData, aCallback);
	CleanupStack::PopAndDestroy(matchData);
	} 

/**  
This method constructs the object according to the details specified in CMatchData object

@param aMatchData Detailed parser information 
@param aCallback Event handler

@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@internalComponent
*/
void CParser::ConstructL(const CMatchData& aMatchData, MContentHandler& aCallback)
	{
	if (!iImpl)
		{
		iImpl = new (ELeave) TParserImpl;
		}
	iImpl->OpenL(aMatchData, aCallback);
	}
	
/**  This method is the destructor for the object.*/
CParser::~CParser()
	{
	if (iImpl)
		{
		iImpl->Close();
		delete iImpl;
		iImpl = NULL;
		}
	}


const TUint  KDefChunkSize				= 0x100;


/** 
This convenience Xml function may be used to parse the file named in aFilename 
using the supplied parser. It reads the file in chunks and passes these to 
CParser::ParseL(). When the end of file is reach CParser::ParseEndL() is called.
It assumes the caller has successfully created a valid parser with a valid
MContentHandler reference and has called ParserBeginL().

@param aParser   A valid CParser object to perform the parsing which has 
                 already had ParseBeginL() called.
@param aFs       The file server session to use
@param aFilename The filename of the file to open, read and parse
@leave ...       One of the system wide error codes or one of the Xml 
                 specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void Xml::ParseL(CParser& aParser, RFs& aFs, const TDesC& aFilename)
	{
	RFile file;
	
	User::LeaveIfError(file.Open(aFs, aFilename, EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(file);
	
	TBuf8<KDefChunkSize> data;
	User::LeaveIfError(file.Read(data,:: KDefChunkSize));
	TUint length = data.Length();

	while (length)
		{
		aParser.ParseL(data);
		User::LeaveIfError(file.Read(data, KDefChunkSize));
		length = data.Length();
		}
	aParser.ParseEndL();
	CleanupStack::PopAndDestroy(&file);
	}

/** 
This convenience Xml function may be used to parse the file held in aFile 
using the supplied parser. It reads the file in chunks and passes these to 
CParser::ParseL(). When the end of file is reach CParser::ParseEndL() is called.
It assumes the caller has successfully created a valid parser with a valid
MContentHandler reference and has called ParserBeginL().

@param aParser   A valid CParser object to perform the parsing which has 
                 already had ParseBeginL() called.
@param aFilename Open file handle to the file to read and parse
@leave ...       One of the system wide error codes or one of the Xml 
                 specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void Xml::ParseL(CParser& aParser, RFile& aFile)
	{
	TBuf8<KDefChunkSize> data;
	User::LeaveIfError(aFile.Read(data,:: KDefChunkSize));
	TUint length = data.Length();

	while (length)
		{
		aParser.ParseL(data);
		User::LeaveIfError(aFile.Read(data, KDefChunkSize));
		length = data.Length();
		}
	aParser.ParseEndL();
	} 

/** 
This convenience Xml function may be used to parse the XML document held in a
descriptor in memory using the supplied parser. This variant function does call 
ParseBeginL() to reset the parser to the default parser as selected for the 
mime type given at creation time. It passes the entire descriptor into  
CParser::ParseL(). When this returns it calls CParser::ParseEndL().
It assumes the caller has successfully created a valid parser with a valid
MContentHandler reference.

This API can be used when parsing different kinds of documents with
the majority being parsed associated with the default parser/mime type.

@param aParser   A valid CParser object to perform the parsing
@param aContent  The entire XML document content to parse
@leave ...       One of the system wide error codes or one of the Xml 
                 specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void Xml::ParseL(CParser& aParser, const TDesC8& aContent)
	{
	aParser.ParseBeginL(); // resets to default mime type
	aParser.ParseL(aContent);
	aParser.ParseEndL();
	}


/** This method tells the parser that we're going to start parsing a document
using the parser associated with this mime type.

@param aDocumentMimeType the mime type of the document
*/
EXPORT_C void CParser::ParseBeginL(const TDesC8& aDocumentMimeType)
	{
	// create CMatchData object with default parameters and adequate mime type
	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(aDocumentMimeType);
	iImpl->SetMimeTypeL(*matchData);
	CleanupStack::PopAndDestroy();
	}

/** This method tells the parser that we're going to start parsing a document 
using the default mime type specified on construction.

The processor chain and features will be cleared if the parser currently 
set is not the default, all old features are removed as these generally 
have no meaning between parsers.

@leave ...       One of the system wide error codes or one of the Xml 
                 specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void CParser::ParseBeginL()
	{
	iImpl->ResetMimeTypeL();
	}


/** This method tells the parser to parse a fragment of a document. 
Could be the whole document. ParseEndL should be called once the whole document has
been parsed. 

The parser currently set will be used.

@param aFragment the fragment to parse
@leave ...       One of the system wide error codes or one of the Xml 
                 specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void CParser::ParseL(const TDesC8& aFragment)
	{
	iImpl->ParseChunkL(aFragment);
	}


/** This method tells the parser that we've finished parsing the current document
and should be called after calling CParser::ParseL for the final time,
as this will initiate error callbacks via MContentHandler, and clean up memory
where appropriate, should an error have occured during the parsing process.
Such an error could occur when trying to parse a truncated document.
@leave ...       One of the system wide error codes or one of the Xml 
                 specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void CParser::ParseEndL()
	{
	iImpl->ParseLastChunkL(_L8(""));
	}
	
	
	
/** This method changes the client and plugin chain.

@param			aCallback the client at the end of the callback
				chain that is to receive the parsed document information.
@param			aPlugins a list of plugin implementation uids that
				make up the callback chain.
@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void CParser::SetProcessorChainL(const RContentProcessorUids& aPlugins)
	{
	iImpl->SetProcessorChainL(aPlugins);
	}	
	
	
/** This method enables a specific feature of the parser.

@return KErrNone if successful, KErrNotSupported if the parser doesn't support the feature.
@param aParserFeature The parser feature that must be enabled.
@see TParserFeature
*/
EXPORT_C TInt CParser::EnableFeature(TInt aParserFeature)
	{
	return iImpl->EnableFeature(aParserFeature);
	}

/** This method disables a specific feature of the parser.

@return	KErrNone if successful, KErrNotSupported if the feature can't be disabled.
@param aParserFeature The parser feature that must be disabled.
@see TParserFeature 
*/
EXPORT_C TInt CParser::DisableFeature(TInt aParserFeature)
	{
	return iImpl->DisableFeature(aParserFeature);
	}

/** This method tell whether a specific feature of the parser is enabled.
@return	True if the feature is enabled.
@see				TParserFeature 
*/
EXPORT_C TBool CParser::IsFeatureEnabled(TInt aParserFeature) const
	{
	return iImpl->IsFeatureEnabled(aParserFeature);
	}



/** This method preloads a string dictionary prior to parsing.

@param			aPublicId  the public identifier representing the 
				document dtd. 
@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
*/
EXPORT_C void CParser::AddPreloadedDictionaryL(const TDesC8& aPublicId)
	{
	iImpl->AddPreloadedDictionaryL(aPublicId);
	}
	


/** This method obtains a handle to the current string pool.

@return				handle to the current string pool.
*/
EXPORT_C RStringPool& CParser::StringPool()
	{
	return iImpl->StringPool();
	}


/** This method obtains a handle to the current StringDictionaryCollection.

@return				handle to the current StringDictionaryCollection.
*/
EXPORT_C RStringDictionaryCollection& CParser::StringDictionaryCollection()
	{
	return iImpl->StringDictionaryCollection();
	}


/**  This method creates the particular parser specified in CMatchData parameter.

The parser plugin resolution process is based on mime type and variant field. 
Both are provided in CMatchData parameter. Mime Type is a mandatory string 
for the resolution process and it is matched against the data field of plugin resource files. 
Variant string is optional. If it exists, it is matched against the first entry 
of the opaque data field of plugin resource files. 	
	
If the query is narrowed down to many parsers, the XML framework might either leave with 
an error (KErrXmlMoreThanOneParserMatched), or choose a parser. The behaviour is specified 
by LeaveOnMany flag. The default value of the flag is FALSE ('choose a parser' behaviour). 

The criteria used to choose a parser, from many matching parsers, is as follows:
	
- If the optional Variant field is specified, the XML framework will choose the parser with
the lowest Uid from the list. 

- If the optional Variant field is not specified, a Symbian-supplied parser (with variant 
field set to "Symbian") will be selected by default. If there are multiple Symbian-supplied 
parsers, the one with the lowest Uid will be selected. Otherwise, the non-Symbian parser 
with the lowest Uid will be selected.

Case sensitivity of the string matching process is applied according to the relevant flag 
in CMatchData. 
The default value is TRUE (Case Sensitivity enabled). 

Only ROM-based parsers are returned if the relevant flag is set in CMatchData.
The default value is FALSE (all parsers are considered).
	
@return	A constructed parser

@param	aCriteria 	The specific information about required parser 
					(mime type, variant data).
@param	aCallback 	The xml/wbxml event handler.

@leave KErrNoMemory If there is not enough memory to create parser or 
					one of system wide error codes. 

@leave KErrXmlParserPluginNotFound 	
					If Xml framework is unable to find a parser 	
					based on data provided in CMatchData.
								
@leave KErrXmlMoreThanOneParserMatched 	
					If Xml framework narrowed down the query 
					to many parsers and a user requested to leave 
					in such case (LeaveOnMany flag set). 

@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@see CMatchData
*/
EXPORT_C CParser* CParser::NewL(const CMatchData& aCriteria, MContentHandler& aCallback)
	{
	CParser* me = CParser::NewLC(aCriteria, aCallback);
	CleanupStack::Pop(me);
	return me;
	}
	
/** This method creates the particular parser specified in CMatchData parameter.
It performs the same way as NewL with the exception that it leaves the object 
on the cleanup stack. 

@return	A constructed parser

@param	aCriteria 	The specific information about required parser 
					(mime type, version, variant data).
@param	aCallback 	The xml/wbxml event handler.

@leave KErrNoMemory If there is not enough memory to create parser or 
					one of system wide error codes. 

@leave KErrXmlParserPluginNotFound 	
					If Xml framework is unable to find a parser 	
					based on data provided in CMatchData.
								
@leave KErrXmlMoreThanOneParserMatched 	
					If Xml framework narrowed down the query 
					to many parsers and a user requested to leave 
					in such case (LeaveOnMany flag set). 

@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@see CParser::NewL(const CMatchData& aCriteria, MContentHandler& aCallback)
@see CMatchData
*/
EXPORT_C CParser* CParser::NewLC(const CMatchData& aCriteria, MContentHandler& aCallback)
	{
	CParser* me = new (ELeave) CParser();
	CleanupStack::PushL(me);
	me->ConstructL(aCriteria, aCallback);
	return me;
	}

/** This method tells the parser that we're going to start parsing a document
using the parser associated with given CMatchData criteria.

@param aCriteria 		The specific information about required parser 
					(mime type, version, variant data).


@leave KErrNoMemory 	If there is not enough memory to create parser.

@leave KErrArgument 	If the data specified in CMatchData are not sufficient.

@leave KErrXmlParserPluginNotFound 	
					If Xml framework is unable to find a parser 	
					based on data provided in CMatchData.
								
@leave KErrXmlMoreThanOneParserMatched 	
					If Xml framework narrowed down the query 
					to many parsers and a user requested to leave 
					in such case (LeaveOnMany flag set). 
					
@leave ...      One of the system wide error codes or one of the Xml 
                specific ones defined in XmlFrameworkErrors.h
@see CMatchData					
*/
EXPORT_C void CParser::ParseBeginL(const CMatchData& aCriteria)
	{
	iImpl->SetMimeTypeL(aCriteria);
	}


