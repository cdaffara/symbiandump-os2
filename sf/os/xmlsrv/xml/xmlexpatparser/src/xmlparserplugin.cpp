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

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include <xml/contenthandler.h>
#include <xml/stringdictionarycollection.h>

#include "xmlparserplugin.h"

using namespace Xml;

const TInt KExpatBufferSize = 2048;

MParser* CXmlParserPlugin::EcomNewL(TAny* aInitParams)
	{
	// The cast to the MParser interface is a particular to the Xml Framework.  Ecom plug-ins usually
	// return the pointer to the C class.
	return static_cast<MParser*>(CXmlParserPlugin::NewL(aInitParams));
	}

/*
CXmlParserPlugin::NewL is for testing and can only be accessed by linking directly to XmlParser.o

aDebugFailCount is used to configure parser heap failure during construction testing - the
usual heap failure macros do not work for our purposes here as it is a private heap.  See
CExpat for details.
*/
CXmlParserPlugin* CXmlParserPlugin::NewL(TAny* aInitParams, TInt aDebugFailCount)
	{
	TParserInitParams* p = reinterpret_cast<TParserInitParams*>(aInitParams);

	CXmlParserPlugin* self = new(ELeave) CXmlParserPlugin(p->iContentHandler);
	
	CleanupStack::PushL(self);
	self->ConstructL(p->iStringDictionaryCollection, p->iCharSetConverter, p->iElementStack, aDebugFailCount);
	CleanupStack::Pop(self);

	return self;
	}

CXmlParserPlugin::CXmlParserPlugin(MContentHandler* aContentHandler) :
	iContentHandler(*aContentHandler)
	{
	}

void CXmlParserPlugin::ConstructL(RStringDictionaryCollection* aStringDictionaryCollection, CCharSetConverter* aCharSetConverter,
							RElementStack* aElementStack, TInt aDebugFailCount)
	{
	iExpat = CExpat::NewL(iContentHandler, aStringDictionaryCollection->StringPool(), *aCharSetConverter, *aElementStack, aDebugFailCount);
	}

/*
Used by framework to delete this object
*/
void CXmlParserPlugin::Release()
	{
	delete this;
	}

/*
Private destructor - only accessible from Release method
*/
CXmlParserPlugin::~CXmlParserPlugin()
	{
	delete iExpat;
	}


TInt CXmlParserPlugin::EnableFeature(TInt aParseMode)
	{
	return iExpat->EnableFeature(aParseMode);
	}

TInt CXmlParserPlugin::DisableFeature(TInt aParseMode)
	{
	return iExpat->DisableFeature(aParseMode);
	}

TBool CXmlParserPlugin::IsFeatureEnabled(TInt aParseMode) const
	{
	return iExpat->IsFeatureEnabled(aParseMode);
	}


void CXmlParserPlugin::ParseChunkL(const TDesC8& aChunk)
	{
	// If a Reset failed, iExpatResetError will have been set.  Here we report it by leaving with the error,
	// which will in turn be passed to MContentHandler::OnError.
	User::LeaveIfError(iExpatResetError);
	ParseChunkInPartsL(aChunk);
	}

void CXmlParserPlugin::ParseLastChunkL(const TDesC8& aFinalChunk)
	{
	// If a Reset failed, iExpatResetError will have been set.  Here we report it by leaving with the error,
	// which will in turn be passed to MContentHandler::OnError.
	User::LeaveIfError(iExpatResetError);

	// Could be that there is some enourmous piece of data here.
	ParseChunkInPartsL(aFinalChunk);
	
	// We need a buffer even though we have nothing to parse,
	// as any previous buffers are destroyed.
	// We are just going through the sequence of completing the parsing gracefully.
	TDes8& buf = iExpat->GetBufferL(1);
	buf = _L8("");
	iExpat->ParseLastL();

	// TRAP doesn't set its result variable unless a leave occurs - so reset it first.
	iExpatResetError = KErrNone;

	// The Reset interface of the Xml Framework provides no mechanism for failure reporting.  If iExpat.ResetL
	// fails we set iExpatResetError.  The error will be reported to MContentHandler::OnError when an attempt
	// is made by the client to begin parsing (see RunL).
	TRAP(iExpatResetError, iExpat->ResetL());
	}

void CXmlParserPlugin::ParseChunkInPartsL(const TDesC8& aChunk)
	{
	// The Expat parser has a buffer of 64k that it stores its state variables in 
	// and allocates its heap memory from.
	// We do this because we may need to upgrade the version of Expat later and this
	// makes it easier.
	// Beacuse of this we copy the buffer that could have a performance issue, 
	// we will address this if it later arises.
	// We parse the descriptor in parts as we do not want to blow the buffer.
	// Even though the descriptor may already be passed in chunks they may be too big.
	
	if (aChunk.Length() == 0)
		{
		return;
		}

	TPtrC8 part;
	TInt offset = 0;
	TInt size = aChunk.Size();
	TInt partSize = size;

	do
		{
		TDes8& buf = iExpat->GetBufferL(KExpatBufferSize);

		if (partSize > KExpatBufferSize) 
			{	
			partSize = KExpatBufferSize;
			}
			
		part.Set (aChunk.Mid(offset, partSize));

		buf = part;
		iExpat->ParseL();

		// The remaining part stats.
		offset += part.Length();
		partSize = size - offset;

		} while (partSize);
	}

void CXmlParserPlugin::SetContentSink(class MContentHandler &aContentSink)
	{
	iExpat->SetContentSink(aContentSink);
	}

RHeap* CXmlParserPlugin::GetInternalHeap() const
	{
	return iExpat->GetInternalHeap();
	}

//
// Ecom implementation infrastructure
//

// KUidXmlParserPlugin is the unique UID for our implementation of the MParser interface.  This interface is
// 'published' as KParserInterfaceUid defined in Xml/FrameworkConstants.h.  KUidXmlParserPlugin must be
// the same as the value of implementation_uid in the file 101faa0a.rss
const TInt KUidXmlParserPlugin = 0x101FAA0C;

const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(KUidXmlParserPlugin, CXmlParserPlugin::EcomNewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

