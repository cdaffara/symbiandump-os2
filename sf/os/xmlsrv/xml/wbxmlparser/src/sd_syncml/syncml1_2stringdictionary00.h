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
// Plugin interface
// 
//


#ifndef __SYNCMLSTRINGDICTIONARY00_V1_2_H__
#define __SYNCMLSTRINGDICTIONARY00_V1_2_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>

class CSyncml1_2StringDictionary00 : public CBase, public Xml::MStringDictionary
/**
The Syncml1_2StringDictionary00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
*/
	{
public:

	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CSyncml1_2StringDictionary00();
	void ConstructL();

	void ElementL(TInt aToken, RString& aElement) const;
	void AttributeL(TInt aToken, RString& aAttribute) const;
	void AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const;
	void AttributeValueL(TInt aToken, RString& aValue) const;

	TBool CompareThisDictionary(const RString& aDictionaryDescription) const;

	TInt SwitchCodePage(TInt aCodePage);
	void PublicIdentifier(RString& aPubId);
	void NamespaceUri(RString& aUri);

	void Release();

public:

	// CodePage tokens.
	//
	// Note: There will be correlation between these token values and their
	// strings via the CDictionaryCodePage using the RStringPool.

	enum TTokenValue {
		// CodePage 00
		// -//SYNCML//DTD SyncML 1.2//EN
		ETokenValueAdd					= 0x05,
		ETokenValueAlert				= 0x06,
		ETokenValueArchive				= 0x07,
		ETokenValueAtomic				= 0x08,
		ETokenValueChal					= 0x09,
		ETokenValueCmd					= 0x0A,
		ETokenValueCmdID				= 0x0B,
		ETokenValueCmdRef				= 0x0C,
		ETokenValueCopy					= 0x0D,
		ETokenValueCred					= 0x0E,
		ETokenValueData					= 0x0F,
		ETokenValueDelete				= 0x10,
		ETokenValueExec					= 0x11,
		ETokenValueFinal				= 0x12,
		ETokenValueGet					= 0x13,
		ETokenValueItem					= 0x14,
		ETokenValueLang					= 0x15,
		ETokenValueLocName				= 0x16,
		ETokenValueLocURI				= 0x17,
		ETokenValueMap					= 0x18,
		ETokenValueMapItem				= 0x19,
		ETokenValueMeta					= 0x1A,
		ETokenValueMsgID				= 0x1B,
		ETokenValueMsgRef				= 0x1C,
		ETokenValueNoResp				= 0x1D,
		ETokenValueNoResults			= 0x1E,
		ETokenValuePut					= 0x1F,
		ETokenValueReplace				= 0x20,
		ETokenValueRespURI				= 0x21,
		ETokenValueResults				= 0x22,
		ETokenValueSearch				= 0x23,
		ETokenValueSequence				= 0x24,
		ETokenValueSessionID			= 0x25,
		ETokenValueSftDel				= 0x26,
		ETokenValueSource				= 0x27,
		ETokenValueSourceRef			= 0x28,
		ETokenValueStatus				= 0x29,
		ETokenValueSync					= 0x2A,
		ETokenValueSyncBody				= 0x2B,
		ETokenValueSyncHdr				= 0x2C,
		ETokenValueSyncML				= 0x2D,
		ETokenValueTarget				= 0x2E,
		ETokenValueTargetRef			= 0x2F,
		ETokenValueReservedForFutureUse	= 0x30,
		ETokenValueVerDTD				= 0x31,
		ETokenValueVerProto				= 0x32,
		ETokenValueNumberOfChanges		= 0x33,
		ETokenValueMoreData				= 0x34,
		ETokenValueField				= 0x35,
		ETokenValueFilter				= 0x36,
		ETokenValueRecord				= 0x37,
		ETokenValueFilterType			= 0x38,
		ETokenValueSourceParent			= 0x39,
		ETokenValueTargetParent			= 0x3A,
		ETokenValueMove					= 0x3B,
		ECorrelator						= 0x3C,
	};


private:

	CSyncml1_2StringDictionary00(RStringPool* aStringPool);

private:

/**
The StringPool for this string dictionary.
We don't own this.
*/
	RStringPool				iStringPool;

	Xml::CDictionaryCodePage*	iCodepage00Table;

	TInt					iCodePage;

	};


const TInt iCodePage00[] = 
	{CSyncml1_2StringDictionary00::ETokenValueAdd,
	 CSyncml1_2StringDictionary00::ETokenValueAlert,
	 CSyncml1_2StringDictionary00::ETokenValueArchive,
	 CSyncml1_2StringDictionary00::ETokenValueAtomic,
	 CSyncml1_2StringDictionary00::ETokenValueChal,
	 CSyncml1_2StringDictionary00::ETokenValueCmd,
	 CSyncml1_2StringDictionary00::ETokenValueCmdID,
	 CSyncml1_2StringDictionary00::ETokenValueCmdRef,
	 CSyncml1_2StringDictionary00::ETokenValueCopy,
	 CSyncml1_2StringDictionary00::ETokenValueCred,
	 CSyncml1_2StringDictionary00::ETokenValueData,
	 CSyncml1_2StringDictionary00::ETokenValueDelete,
	 CSyncml1_2StringDictionary00::ETokenValueExec,
	 CSyncml1_2StringDictionary00::ETokenValueFinal,
	 CSyncml1_2StringDictionary00::ETokenValueGet,
	 CSyncml1_2StringDictionary00::ETokenValueItem,
	 CSyncml1_2StringDictionary00::ETokenValueLang,
	 CSyncml1_2StringDictionary00::ETokenValueLocName,
	 CSyncml1_2StringDictionary00::ETokenValueLocURI,
	 CSyncml1_2StringDictionary00::ETokenValueMap,
	 CSyncml1_2StringDictionary00::ETokenValueMapItem,
	 CSyncml1_2StringDictionary00::ETokenValueMeta,
	 CSyncml1_2StringDictionary00::ETokenValueMsgID,
	 CSyncml1_2StringDictionary00::ETokenValueMsgRef,
	 CSyncml1_2StringDictionary00::ETokenValueNoResp,
	 CSyncml1_2StringDictionary00::ETokenValueNoResults,
	 CSyncml1_2StringDictionary00::ETokenValuePut,
	 CSyncml1_2StringDictionary00::ETokenValueReplace,
	 CSyncml1_2StringDictionary00::ETokenValueRespURI,
	 CSyncml1_2StringDictionary00::ETokenValueResults,
	 CSyncml1_2StringDictionary00::ETokenValueSearch,
	 CSyncml1_2StringDictionary00::ETokenValueSequence,
	 CSyncml1_2StringDictionary00::ETokenValueSessionID,
	 CSyncml1_2StringDictionary00::ETokenValueSftDel,
	 CSyncml1_2StringDictionary00::ETokenValueSource,
	 CSyncml1_2StringDictionary00::ETokenValueSourceRef,
	 CSyncml1_2StringDictionary00::ETokenValueStatus,
	 CSyncml1_2StringDictionary00::ETokenValueSync,
	 CSyncml1_2StringDictionary00::ETokenValueSyncBody,
	 CSyncml1_2StringDictionary00::ETokenValueSyncHdr,
	 CSyncml1_2StringDictionary00::ETokenValueSyncML,
	 CSyncml1_2StringDictionary00::ETokenValueTarget,
	 CSyncml1_2StringDictionary00::ETokenValueTargetRef,
	 CSyncml1_2StringDictionary00::ETokenValueReservedForFutureUse,
	 CSyncml1_2StringDictionary00::ETokenValueVerDTD,
	 CSyncml1_2StringDictionary00::ETokenValueVerProto,
	 CSyncml1_2StringDictionary00::ETokenValueNumberOfChanges,
	 CSyncml1_2StringDictionary00::ETokenValueMoreData,
	 CSyncml1_2StringDictionary00::ETokenValueField,
	 CSyncml1_2StringDictionary00::ETokenValueFilter,
	 CSyncml1_2StringDictionary00::ETokenValueRecord,
	 CSyncml1_2StringDictionary00::ETokenValueFilterType,
	 CSyncml1_2StringDictionary00::ETokenValueSourceParent,
	 CSyncml1_2StringDictionary00::ETokenValueTargetParent,
	 CSyncml1_2StringDictionary00::ETokenValueMove,
	 CSyncml1_2StringDictionary00::ECorrelator,
	 0}; // ok to end with zero as this is used by a global token

#endif //__SYNCMLSTRINGDICTIONARY00_V1_2_H__
