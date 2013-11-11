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

#ifndef __WBXMLPARSER_H__
#define __WBXMLPARSER_H__

#include <e32def.h>

#include <xml/attribute.h>
#include <xml/parserfeature.h>
#include <xml/plugins/parserinterface.h>
#include <xml/plugins/parserinitparams.h>


class CDescriptorDataSource;

namespace Xml
{

/**
Helper type to describe a list of RAttributes.
@internalTechnology
*/
typedef RArray<RAttribute> RAttributeArray; 


class CWbxmlParser : public CBase, public MParser
/**
The CWbxmlParser class implements a  wbxml parser.
It is used to parse wbxml documents that conform to the 
'Binary XML Content Format Specification Version 1.3, 25 July 2001'.
This specification is backwards compatible with its predecessors.

@internalTechnology
*/
	{

public:

	static MParser* NewL(TAny* aInitParams);
	virtual ~CWbxmlParser();

	// From MParser
	TInt EnableFeature(TInt aParserFeature);
	TInt DisableFeature(TInt aParserFeature);
	TBool IsFeatureEnabled(TInt aParserFeature) const;
	void Release();
	void Reset();
	void ParseChunkL(const TDesC8& aChunk);
	void ParseLastChunkL(const TDesC8& aFinalChunk);

	void SetContentSink (MContentHandler& aContentHandler);
	RStringPool& StringPool();

private:

	CWbxmlParser();
	CWbxmlParser(TParserInitParams* aInitParams);
	void ConstructL();
	void TidyMemoryForReuse();
	void DoParseL();
	void CheckErrorL(TInt aError);
	void DoReadAndParseDescriptorL(const TDesC8& aDescriptor);

	void VersionL();
	void PublicIdL();
	void CharsetL();
	void StrtblL();
	void LoadFormalDictionaryL();
	void BodyL();
	void DetermineNextToParseL();
	void ParseTagOrAttributeL(TUint8 aByte);
	void HandleTagL(TUint8 aByte);
	void HandleAttributeL(TUint8 aByte);
	void HandleAttributeEndL();
	void SwitchPageL();
	void LoadDictionaryL(TInt aCodePage);
	void ContentL();
	void AttrStartL(TUint8 aByte);
	void AttrValueL(TUint8 aByte);
	void LiteralL(TUint8 aLiteral);

	void ConvertToEncodingL(TUint32 aSrcCharSetUid, TDesC8& aSrc, 
							TUint32 aDestCharSetUid, HBufC8*& aDest);

	TBool DetermineCharacterNull(const TPtr8& aStringPtr, TUint aCharLength, 
							 	 TInt32 aOffset);

	void ExtractStringUtf8L (TInt32 aStringTblIndex, 
							 CBufFlat& aUtf8String, 
							 TBool aAppendString = EFalse);

	void ExtractStringUtf8L (CBufFlat& aUtf8String, 
							 TBool aAppendString = EFalse);
	void InlineL();
	void TableRefL();
	void ExtensionL(TUint8 aByte);
	void ExtensionIL(TUint8 aByte);
	void ExtensionTL(TUint8 aByte);
	void EntityL();
	void OpaqueL();

	TInt32 GetMultiByteInt32L();
	inline void NegIndexL(TInt aIndex);

	inline void OnStartDocumentL();
	inline void OnStartElementL();
	inline void OnEndElementL();
	inline void OnProcessingInstructionL();
	inline void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);

private:

/**
Parser run states.
*/
	enum TParserRunState {
		EParserRunStateVersion,
		EParserRunStatePublicId,
		EParserRunStateCharset,
		EParserRunStateStringTbl,
		EParserRunStateBody,
	};
	
/**
Parser mode states.
*/
	enum TParserModeState {
		EParserModeStateTag,
		EParserModeStateAttribute
	};


/**
The current Tag (Element) code page.
*/
	TInt							iTagCurrentCodePage;

/**
The current Attribute/Value code page.
*/
	TInt							iAttributeCurrentCodePage;

/**
The current code page being processed.
Allows us to determine if we are in the the correct codepage for this state.
*/
	TInt							iCurrentCodePage;

/**
Namespace associated with the tag codepage.
*/
	RString							iTagCurrentUri;

/**
Namespace associated with the attribute codepage.
*/
	RString							iAttributeCurrentUri;


/**
The current uri of the codepage being processed.
*/
	RString							iCurrentUri;
 
/**
The current run state.
*/
    TParserRunState					iParserRunState;

/**
The current parser mode state.
*/
	TParserModeState				iParserModeState;


	// Data variables

/**
The character set uid as returned from CharConv
*/
	TUint							iCharSetUid;

/**
The wbxml document string table.
*/
	HBufC8*							iStringTable;

/**
The wbxml document string table's length.
*/
	TInt32							iStringTblLength;

/**
The wbxml document version.
*/
	TUint8							iVersion;

/**
The wbxml document public id, if available.
*/
	TInt32							iPublicId;

/**
The wbxml document public id index into the string table, if available.
*/
	TInt32							iPublicIdIndex;

/**
The wbxml document formal public id, if available.
*/
	CBufFlat*						iFormalPublicId;

/**
Bit settings indicating whether attributes or contents follow a tag.
*/
	TInt							iTagExcess;

/**
The tag/element value
*/
	TInt							iTagValue;

/**
The length of the currently parsed opaque section of the wbxml document.
*/
	TInt32							iOpaqueLength;

/**
The null character represented in the current document encoding.
This allows us to search for null terminated strings.
*/
	HBufC8*							iEncodingNull;

/**
Helper buffer used for recording strings throughout the parser.
For example, used heaverly in the concatenation of attribute values parts.
*/
	CBufFlat*						iUtf8String;

/**
The current list of attributes for a tag/element.
*/
	RAttributeArray					iAttributes;

/**
Indicates whether this is a Processing Instruction (PI) or not.
*/
	TBool							iIsPiInstruction;

/**
Used to record the current PI instruction tag being processed.
*/
	TUint8							iPiTarget;

/**
Helper class of an attribute to ease construction, and improve performance.
*/
	RAttribute						iAttribute;

/**
Helper class to improve performance.
This is never opened/closed, we use RStrings default properties that
returns a null string.
*/
	RString							iBlank;

/**
The child to callback.
We do not own this.
*/
	MContentHandler*				iContentHandler;

/**
Useful stringpool table collection of RStrings to improve performance
when parsing. Also, improves comparison performance in the client.
We do not own this.
*/
	RStringDictionaryCollection*	iStringDictionaryCollection;

/**
The document to read.
We do not own this.
*/
	CDescriptorDataSource* 			iDataSource;

/**
Helper class to convert between character sets.
We do not own this.
*/
	CCharSetConverter* 				iCharSetConverter;

/**
The current parse mode of the parser.
*/
	TInt 							iParseMode;

/**
A stack that helps correlate the start and end of elements.
We do not own this.
*/
	RElementStack* 					iElementStack;

/**
Indicates the last chunk of the data for a descriptor.
*/
	TBool							iLastChunk;

/**
Count to help correlate End's with their beginnings
to avoid mismatch.
*/
	TInt 							iNumEnds;
	};
}

#endif //__WBXMLPARSER_H__


