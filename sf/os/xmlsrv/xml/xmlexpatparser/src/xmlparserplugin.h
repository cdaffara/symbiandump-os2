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

#ifndef XMLPARSERPLUGIN_H
#define XMLPARSERPLUGIN_H

#include <xml/plugins/parserinitparams.h>
#include <xml/plugins/parserinterface.h>

#include "cexpat.h"

/*
CXmlParserPlugin class.

1.
This is the top level class of xmlparserplugin.dll - an Ecom plugin
for use in the Xml framework.

For full details of the plug-in configuration see the files pmlparser.mmp
and 101faa0a.rss.

Ecom constructs the plug-in by virtue of a call to the static member
function EcomNewL, a pointer to which is contained in the associated
TImplementationProxy table for the plug-in.

2.
This class implements the MParser interface defined in xml/parser.h

3.
The parsing functionality is subcontracted to the Expat parser toolkit,
captured in the class CExpat, see the file cexpat.h.
*/
class CXmlParserPlugin : public CBase, Xml::MParser
	{
public:

	/*
	EcomNewL function intended for construction by Ecom of implementation MParser interface.
	*/
	static Xml::MParser* EcomNewL(TAny* aInitParams);

	/*
	This class has a private destructor.  The Xml Framework destroys the parser using this Release method.
	*/
	void Release();

	/*
	Standard NewL function.  This function is used by EcomNewL but has been provided separately for
	testing purposes.
	*/
	static CXmlParserPlugin* NewL(TAny* aInitParams, TInt aDebugFailCount=0);
	

	/*
	Implementation of MParser interface
	*/
	TInt EnableFeature(TInt aParserFeature);
	TInt DisableFeature(TInt aParserFeature);
	TBool IsFeatureEnabled(TInt aParserFeature) const;
	void ParseChunkL(const TDesC8& aChunk);
	void ParseLastChunkL(const TDesC8& aFinalChunk);

	void SetContentSink(Xml::MContentHandler& aContentHandler);
	
	RStringPool& StringPool();

	/*
	Function used to access internal parser heap for testing
	*/
	RHeap* GetInternalHeap() const;

private:
	CXmlParserPlugin(Xml::MContentHandler* aContentHandler);
	void ConstructL(Xml::RStringDictionaryCollection* aStringDictionaryCollection, Xml::CCharSetConverter* aCharSetConverter,
		RElementStack* aElementStack, TInt aDebugFailCount);
	void ParseChunkInPartsL(const TDesC8& aChunk);
	virtual ~CXmlParserPlugin();

private:
	/*
	References to client-provided objects
	*/
	Xml::MContentHandler& iContentHandler;

	/*
	CExpat is the work horse of the parser
	*/
	CExpat* iExpat;
	TInt iExpatResetError;
	};

#endif // XMLPARSER_H
