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

#ifndef __PARSERIMPL_H__
#define __PARSERIMPL_H__

#include <xml/contentprocessoruids.h> // needed for the typedef
#include <xml/plugins/elementstack.h> // needed for the typedef
#include <xml/matchdata.h>

class RFs;

namespace Xml
{
class MContentHandler;
class MContentProcessor;
class MParser;
class CCharSetConverter;
class RStringDictionaryCollection;


class TParserImpl
/** The TParserImpl class hides the implementation of the CParser.
It is responsible for building and maintaining the list of plugins
in the parsing chain, and has ownership of several key objects within
the framework. */
	{
public:

	TParserImpl();

	void OpenL(const CMatchData& aMatchData, MContentHandler& aCallback);
	void Close();


	void ParseFileL(RFs& aFs, const TDesC& aFilename);
	
	void ParseChunkL(const TDesC8& aChunk);
	void ParseLastChunkL(const TDesC8& aFinalChunk);

	void SetContentHandler(MContentHandler& aCallback);
	void SetProcessorChainL(const RContentProcessorUids& aPlugins);
	void SetMimeTypeL(const CMatchData& aCriteria);
	void ResetMimeTypeL();

	TInt EnableFeature(TInt aParserFeature);
	TInt DisableFeature(TInt aParserFeature);
	TBool IsFeatureEnabled(TInt aParserFeature) const;

	void AddPreloadedDictionaryL(const TDesC8& aPublicId);

	RStringPool& StringPool();
	RStringDictionaryCollection& StringDictionaryCollection();
	
private:

	TParserImpl(const TParserImpl& aOriginal);
	TParserImpl& operator=(const TParserImpl& aRhs);

	MContentHandler* BuildChainL(MContentHandler& aCallback,
								 const RContentProcessorUids& aPlugins);

	void DestroyChain();
	
	MParser* ConstructParserL(const CMatchData& aMatchData, MContentHandler& aCallback);

	MContentProcessor* ConstructPluginInReverseL(const TUid& aImplementationUid, 
												 MContentHandler& aCallback);

private:

	/** The common element stack for this collection of plugins. */
	RElementStack iElementStack; 

	struct TDtorContentProcessorPtrMapping
		{
		TUid				iDtorKey;
		MContentProcessor*	iContentProcessor;
		};

	static void DestroyContentProcessorPtrMapping(TAny* aPtr);

	/** The collection of MContentProcessor pointers and Dtor keys that make up the chain of command.
    The chain order is determined by the the index, 0 being the first plugin in the chain. */
	RPointerArray<TDtorContentProcessorPtrMapping>	iDtorKeyAndPluginList;
	
	TUid						 iParserDtorKey;
	MParser*					 iParser;
	CCharSetConverter*  		 iCharSetConverter;
	RStringDictionaryCollection* iStringDictionaryCollection;
	MContentHandler*			 iClient;
	CMatchData                       iDefParserMatchData;
	CMatchData                       iCurrentParserMatchData;
	};

}

#endif //__PARSERIMPL_H__
