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


#ifndef __WML1_1STRINGDICTIONARY00_H__
#define __WML1_1STRINGDICTIONARY00_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>

class CWml1_1StringDictionary00 : public CBase, public Xml::MStringDictionary
/**
The Wml1_1StringDictionary00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
*/
	{
public:
	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CWml1_1StringDictionary00();
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

	enum TTagToken {
		// CodePage 00
		// -//WAPFORUM//DTD WML 1.1//EN
		ETagTokenA							= 0x1C,
		ETagTokenAnchor						= 0x22,
		ETagTokenAccess						= 0x23,
		ETagTokenB							= 0x24,
		ETagTokenBig						= 0x25,
		ETagTokenBr							= 0x26,
		ETagTokenCard						= 0x27,
		ETagTokenDo							= 0x28,
		ETagTokenEm							= 0x29,
		ETagTokenFieldset					= 0x2A,
		ETagTokenGo							= 0x2B,
		ETagTokenHead						= 0x2C,
		ETagTokenI							= 0x2D,
		ETagTokenImg						= 0x2E,
		ETagTokenInput						= 0x2F,
		ETagTokenMeta						= 0x30,
		ETagTokenNoop						= 0x31,
		ETagTokenP							= 0x20,
		ETagTokenPostfield					= 0x21,
		ETagTokenPrev						= 0x32,
		ETagTokenOnevent					= 0x33,
		ETagTokenOptgroup					= 0x34,
		ETagTokenOption						= 0x35,
		ETagTokenRefresh					= 0x36,
		ETagTokenSelect						= 0x37,
		ETagTokenSetvar						= 0x3E,
		ETagTokenSmall						= 0x38,
		ETagTokenStrong						= 0x39,
		ETagTokenTable						= 0x1F,
		ETagTokenTd							= 0x1D,
		ETagTokenTemplate					= 0x3B,
		ETagTokenTimer						= 0x3C,
		ETagTokenTr							= 0x1E,
		ETagTokenU							= 0x3D,
		ETagTokenWml						= 0x3F,
	};


	enum TAttributeToken {
		// CodePage 00
		// -//WAPFORUM//DTD WML 1.1//EN
		EAttributeTokenAcceptcharset		= 0x05,
		EAttributeTokenAlign1				= 0x52,
		EAttributeTokenAlign2				= 0x06,
		EAttributeTokenAlign3				= 0x07,
		EAttributeTokenAlign4				= 0x08,
		EAttributeTokenAlign5				= 0x09,
		EAttributeTokenAlign6				= 0x0A,
		EAttributeTokenAlign7				= 0x0B,
		EAttributeTokenAlt					= 0x0C,
		EAttributeTokenClass				= 0x54,
		EAttributeTokenColumns				= 0x53,
		EAttributeTokenContent1				= 0x0D,
		EAttributeTokenContent2				= 0x5C,
		EAttributeTokenDomain				= 0x0F,
		EAttributeTokenEmptyok1				= 0x10,
		EAttributeTokenEmptyok2				= 0x11,
		EAttributeTokenFormat				= 0x12,
		EAttributeTokenForua1				= 0x56,
		EAttributeTokenForua2				= 0x57,
		EAttributeTokenHeight				= 0x13,
		EAttributeTokenHref1				= 0x4A,
		EAttributeTokenHref2				= 0x4B,
		EAttributeTokenHref3				= 0x4C,
		EAttributeTokenHspace				= 0x14,
		EAttributeTokenHttpequiv1			= 0x5A,
		EAttributeTokenHttpequiv2			= 0x5B,
		EAttributeTokenHttpequiv3			= 0x5D,
		EAttributeTokenId					= 0x55,
		EAttributeTokenIvalue				= 0x15,
		EAttributeTokenIname				= 0x16,
		EAttributeTokenLabel				= 0x18,
		EAttributeTokenLocalsrc				= 0x19,
		EAttributeTokenMaxlength			= 0x1A,
		EAttributeTokenMethod1 				= 0x1B,
		EAttributeTokenMethod2 				= 0x1C,
		EAttributeTokenMode1 				= 0x1D,
		EAttributeTokenMode2				= 0x1E,
		EAttributeTokenMultiple1			= 0x1F,
		EAttributeTokenMultiple2			= 0x20,
		EAttributeTokenName					= 0x21,
		EAttributeTokenNewcontext1			= 0x22,
		EAttributeTokenNewcontext2			= 0x23,
		EAttributeTokenOnenterbackward		= 0x25,
		EAttributeTokenOnenterforward		= 0x26,
		EAttributeTokenOnpick				= 0x24,
		EAttributeTokenOntimer				= 0x27,
		EAttributeTokenOptional1			= 0x28,
		EAttributeTokenOptional2			= 0x29,
		EAttributeTokenPath					= 0x2A,
		EAttributeTokenScheme				= 0x2E,
		EAttributeTokenSendreferer1			= 0x2F,
		EAttributeTokenSendreferer2			= 0x30,
		EAttributeTokenSize					= 0x31,
		EAttributeTokenSrc1					= 0x32,
		EAttributeTokenSrc2					= 0x58,
		EAttributeTokenSrc3					= 0x59,
		EAttributeTokenOrdered1				= 0x33,
		EAttributeTokenOrdered2				= 0x34,
		EAttributeTokenTabindex				= 0x35,
		EAttributeTokenTitle				= 0x36,
		EAttributeTokenType1				= 0x37,
		EAttributeTokenType2				= 0x38,
		EAttributeTokenType3				= 0x39,
		EAttributeTokenType4				= 0x3A,
		EAttributeTokenType5				= 0x3B,
		EAttributeTokenType6				= 0x3C,
		EAttributeTokenType7				= 0x3D,
		EAttributeTokenType8				= 0x3E,
		EAttributeTokenType9				= 0x3F,
		EAttributeTokenType10				= 0x45,
		EAttributeTokenType11				= 0x46,
		EAttributeTokenType12				= 0x47,
		EAttributeTokenType13				= 0x48,
		EAttributeTokenType14				= 0x49,
		EAttributeTokenValue				= 0x4D,
		EAttributeTokenVspace				= 0x4E,
		EAttributeTokenWidth				= 0x4F,
		EAttributeTokenXmllang				= 0x50,
	};



	enum TAttributeValueToken {
		// CodePage 00
		// -//WAPFORUM//DTD WML 1.1//EN
		EAttributeValueTokenAcceptcharset		= 0x05,
		EAttributeValueTokenAlign1				= 0x52,
		EAttributeValueTokenAlign2				= 0x06,
		EAttributeValueTokenAlign3				= 0x07,
		EAttributeValueTokenAlign4				= 0x08,
		EAttributeValueTokenAlign5				= 0x09,
		EAttributeValueTokenAlign6				= 0x0A,
		EAttributeValueTokenAlign7				= 0x0B,
		EAttributeValueTokenAlt					= 0x0C,
		EAttributeValueTokenClass				= 0x54,
		EAttributeValueTokenColumns				= 0x53,
		EAttributeValueTokenContent1			= 0x0D,
		EAttributeValueTokenContent2			= 0x5C,
		EAttributeValueTokenDomain				= 0x0F,
		EAttributeValueTokenEmptyok1			= 0x10,
		EAttributeValueTokenEmptyok2			= 0x11,
		EAttributeValueTokenFormat				= 0x12,
		EAttributeValueTokenForua1				= 0x56,
		EAttributeValueTokenForua2				= 0x57,
		EAttributeValueTokenHeight				= 0x13,
		EAttributeValueTokenHref1				= 0x4A,
		EAttributeValueTokenHref2				= 0x4B,
		EAttributeValueTokenHref3				= 0x4C,
		EAttributeValueTokenHspace				= 0x14,
		EAttributeValueTokenHttpequiv1			= 0x5A,
		EAttributeValueTokenHttpequiv2			= 0x5B,
		EAttributeValueTokenHttpequiv3			= 0x5D,
		EAttributeValueTokenId					= 0x55,
		EAttributeValueTokenIvalue				= 0x15,
		EAttributeValueTokenIname				= 0x16,
		EAttributeValueTokenLabel				= 0x18,
		EAttributeValueTokenLocalsrc			= 0x19,
		EAttributeValueTokenMaxlength			= 0x1A,
		EAttributeValueTokenMethod1 			= 0x1B,
		EAttributeValueTokenMethod2 			= 0x1C,
		EAttributeValueTokenMode1 				= 0x1D,
		EAttributeValueTokenMode2				= 0x1E,
		EAttributeValueTokenMultiple1			= 0x1F,
		EAttributeValueTokenMultiple2			= 0x20,
		EAttributeValueTokenName				= 0x21,
		EAttributeValueTokenNewcontext1			= 0x22,
		EAttributeValueTokenNewcontext2			= 0x23,
		EAttributeValueTokenOnenterbackward1	= 0x25,
		EAttributeValueTokenOnenterforward1		= 0x26,
		EAttributeValueTokenOnpick1				= 0x24,
		EAttributeValueTokenOntimer1			= 0x27,
		EAttributeValueTokenOptional1			= 0x28,
		EAttributeValueTokenOptional2			= 0x29,
		EAttributeValueTokenPath				= 0x2A,
		EAttributeValueTokenScheme				= 0x2E,
		EAttributeValueTokenSendreferer1		= 0x2F,
		EAttributeValueTokenSendreferer2		= 0x30,
		EAttributeValueTokenSize				= 0x31,
		EAttributeValueTokenSrc1				= 0x32,
		EAttributeValueTokenSrc2				= 0x58,
		EAttributeValueTokenSrc3				= 0x59,
		EAttributeValueTokenOrdered1			= 0x33,
		EAttributeValueTokenOrdered2			= 0x34,
		EAttributeValueTokenTabindex			= 0x35,
		EAttributeValueTokenTitle				= 0x36,
		EAttributeValueTokenType1				= 0x37,
		EAttributeValueTokenType2				= 0x38,
		EAttributeValueTokenType3				= 0x39,
		EAttributeValueTokenType4				= 0x3A,
		EAttributeValueTokenType5				= 0x3B,
		EAttributeValueTokenType6				= 0x3C,
		EAttributeValueTokenType7				= 0x3D,
		EAttributeValueTokenType8				= 0x3E,
		EAttributeValueTokenType9				= 0x3F,
		EAttributeValueTokenType10				= 0x45,
		EAttributeValueTokenType11				= 0x46,
		EAttributeValueTokenType12				= 0x47,
		EAttributeValueTokenType13				= 0x48,
		EAttributeValueTokenType14				= 0x49,
		EAttributeValueTokenValue				= 0x4D,
		EAttributeValueTokenVspace				= 0x4E,
		EAttributeValueTokenWidth				= 0x4F,
		EAttributeValueTokenXmllang				= 0x50,
		EAttributeValueTokenCom					= 0x85,
		EAttributeValueTokenEdu 				= 0x86,
		EAttributeValueTokenNet 				= 0x87,
		EAttributeValueTokenOrg 				= 0x88,
		EAttributeValueTokenAccept				= 0x89,
		EAttributeValueTokenBottom 				= 0x8A,
		EAttributeValueTokenClear 				= 0x8B,
		EAttributeValueTokenDelete 				= 0x8C,
		EAttributeValueTokenHelp 				= 0x8D,
		EAttributeValueTokenHttp1 				= 0x8E,
		EAttributeValueTokenHttp2 				= 0x8F,
		EAttributeValueTokenHttps1 				= 0x90,
		EAttributeValueTokenHttps2 				= 0x91,
		EAttributeValueTokenMiddle 				= 0x93,
		EAttributeValueTokenNowrap 				= 0x94,
		EAttributeValueTokenOnenterbackward2	= 0x96,
		EAttributeValueTokenOnenterforward2		= 0x97,
		EAttributeValueTokenOnpick2				= 0x95,
		EAttributeValueTokenOntimer2 			= 0x98,
		EAttributeValueTokenOptions 			= 0x99,
		EAttributeValueTokenPassword 			= 0x9A,
		EAttributeValueTokenReset 				= 0x9B,
		EAttributeValueTokenText 				= 0x9D,
		EAttributeValueTokenTop 				= 0x9E,
		EAttributeValueTokenUnknown 			= 0x9F,
		EAttributeValueTokenWrap 				= 0xA0,
		EAttributeValueTokenWww 				= 0xA1,
	};



private:

	CWml1_1StringDictionary00(RStringPool* aStringPool);

private:

/**
The StringPool for this string dictionary.
We don't own this.
*/
	RStringPool				iStringPool;

	Xml::CDictionaryCodePage*	iCodepage00Table;

	TInt					iCodePage;

	};


const TInt iTagCodePage00[] = 
	{CWml1_1StringDictionary00::ETagTokenA,
	 CWml1_1StringDictionary00::ETagTokenAnchor,
	 CWml1_1StringDictionary00::ETagTokenAccess,
	 CWml1_1StringDictionary00::ETagTokenB,
	 CWml1_1StringDictionary00::ETagTokenBig,
	 CWml1_1StringDictionary00::ETagTokenBr,
	 CWml1_1StringDictionary00::ETagTokenCard,
	 CWml1_1StringDictionary00::ETagTokenDo,
	 CWml1_1StringDictionary00::ETagTokenEm,
	 CWml1_1StringDictionary00::ETagTokenFieldset,
	 CWml1_1StringDictionary00::ETagTokenGo,
	 CWml1_1StringDictionary00::ETagTokenHead,
	 CWml1_1StringDictionary00::ETagTokenI,
	 CWml1_1StringDictionary00::ETagTokenImg,
	 CWml1_1StringDictionary00::ETagTokenInput,
	 CWml1_1StringDictionary00::ETagTokenMeta,
	 CWml1_1StringDictionary00::ETagTokenNoop,
	 CWml1_1StringDictionary00::ETagTokenP,
	 CWml1_1StringDictionary00::ETagTokenPostfield,
	 CWml1_1StringDictionary00::ETagTokenPrev,
	 CWml1_1StringDictionary00::ETagTokenOnevent,
	 CWml1_1StringDictionary00::ETagTokenOptgroup,
	 CWml1_1StringDictionary00::ETagTokenOption,
	 CWml1_1StringDictionary00::ETagTokenRefresh,
	 CWml1_1StringDictionary00::ETagTokenSelect,
	 CWml1_1StringDictionary00::ETagTokenSetvar,
	 CWml1_1StringDictionary00::ETagTokenSmall,
	 CWml1_1StringDictionary00::ETagTokenStrong,
	 CWml1_1StringDictionary00::ETagTokenTable,
	 CWml1_1StringDictionary00::ETagTokenTd,
	 CWml1_1StringDictionary00::ETagTokenTemplate,
	 CWml1_1StringDictionary00::ETagTokenTimer,
	 CWml1_1StringDictionary00::ETagTokenTr,
	 CWml1_1StringDictionary00::ETagTokenU,
	 CWml1_1StringDictionary00::ETagTokenWml,
	 0}; // ok to end with zero as this is used by a global token


const TInt iAttributeCodePage00[] = 
	{CWml1_1StringDictionary00::EAttributeTokenAcceptcharset,
	 CWml1_1StringDictionary00::EAttributeTokenAlign1,
	 CWml1_1StringDictionary00::EAttributeTokenAlign2,
	 CWml1_1StringDictionary00::EAttributeTokenAlign3,
	 CWml1_1StringDictionary00::EAttributeTokenAlign4,
	 CWml1_1StringDictionary00::EAttributeTokenAlign5,
	 CWml1_1StringDictionary00::EAttributeTokenAlign6,
	 CWml1_1StringDictionary00::EAttributeTokenAlign7,
	 CWml1_1StringDictionary00::EAttributeTokenAlt,
	 CWml1_1StringDictionary00::EAttributeTokenClass,
	 CWml1_1StringDictionary00::EAttributeTokenColumns,
	 CWml1_1StringDictionary00::EAttributeTokenContent1,
	 CWml1_1StringDictionary00::EAttributeTokenContent2,
	 CWml1_1StringDictionary00::EAttributeTokenDomain,
	 CWml1_1StringDictionary00::EAttributeTokenEmptyok1,
	 CWml1_1StringDictionary00::EAttributeTokenEmptyok2,
	 CWml1_1StringDictionary00::EAttributeTokenFormat,
	 CWml1_1StringDictionary00::EAttributeTokenForua1,
	 CWml1_1StringDictionary00::EAttributeTokenForua2,
	 CWml1_1StringDictionary00::EAttributeTokenHeight,
	 CWml1_1StringDictionary00::EAttributeTokenHref1,
	 CWml1_1StringDictionary00::EAttributeTokenHref2,
	 CWml1_1StringDictionary00::EAttributeTokenHref3,
	 CWml1_1StringDictionary00::EAttributeTokenHspace,
	 CWml1_1StringDictionary00::EAttributeTokenHttpequiv1,
	 CWml1_1StringDictionary00::EAttributeTokenHttpequiv2,
	 CWml1_1StringDictionary00::EAttributeTokenHttpequiv3,
	 CWml1_1StringDictionary00::EAttributeTokenId,
	 CWml1_1StringDictionary00::EAttributeTokenIvalue,
	 CWml1_1StringDictionary00::EAttributeTokenIname,
	 CWml1_1StringDictionary00::EAttributeTokenLabel,
	 CWml1_1StringDictionary00::EAttributeTokenLocalsrc,
	 CWml1_1StringDictionary00::EAttributeTokenMaxlength,
	 CWml1_1StringDictionary00::EAttributeTokenMethod1,
	 CWml1_1StringDictionary00::EAttributeTokenMethod2,
	 CWml1_1StringDictionary00::EAttributeTokenMode1,
	 CWml1_1StringDictionary00::EAttributeTokenMode2,
	 CWml1_1StringDictionary00::EAttributeTokenMultiple1,
	 CWml1_1StringDictionary00::EAttributeTokenMultiple2,
	 CWml1_1StringDictionary00::EAttributeTokenName,
	 CWml1_1StringDictionary00::EAttributeTokenNewcontext1,
	 CWml1_1StringDictionary00::EAttributeTokenNewcontext2,
	 CWml1_1StringDictionary00::EAttributeTokenOnenterbackward,
	 CWml1_1StringDictionary00::EAttributeTokenOnenterforward,
	 CWml1_1StringDictionary00::EAttributeTokenOnpick,
	 CWml1_1StringDictionary00::EAttributeTokenOntimer,
	 CWml1_1StringDictionary00::EAttributeTokenOptional1,
	 CWml1_1StringDictionary00::EAttributeTokenOptional2,
	 CWml1_1StringDictionary00::EAttributeTokenPath,
	 CWml1_1StringDictionary00::EAttributeTokenScheme,
	 CWml1_1StringDictionary00::EAttributeTokenSendreferer1,
	 CWml1_1StringDictionary00::EAttributeTokenSendreferer2,
	 CWml1_1StringDictionary00::EAttributeTokenSize,
	 CWml1_1StringDictionary00::EAttributeTokenSrc1,
	 CWml1_1StringDictionary00::EAttributeTokenSrc2,
	 CWml1_1StringDictionary00::EAttributeTokenSrc3,
	 CWml1_1StringDictionary00::EAttributeTokenOrdered1,
	 CWml1_1StringDictionary00::EAttributeTokenOrdered2,
	 CWml1_1StringDictionary00::EAttributeTokenTabindex,
	 CWml1_1StringDictionary00::EAttributeTokenTitle,
	 CWml1_1StringDictionary00::EAttributeTokenType1,
	 CWml1_1StringDictionary00::EAttributeTokenType2,
	 CWml1_1StringDictionary00::EAttributeTokenType3,
	 CWml1_1StringDictionary00::EAttributeTokenType4,
	 CWml1_1StringDictionary00::EAttributeTokenType5,
	 CWml1_1StringDictionary00::EAttributeTokenType6,
	 CWml1_1StringDictionary00::EAttributeTokenType7,
	 CWml1_1StringDictionary00::EAttributeTokenType8,
	 CWml1_1StringDictionary00::EAttributeTokenType9,
	 CWml1_1StringDictionary00::EAttributeTokenType10,
	 CWml1_1StringDictionary00::EAttributeTokenType11,
	 CWml1_1StringDictionary00::EAttributeTokenType12,
	 CWml1_1StringDictionary00::EAttributeTokenType13,
	 CWml1_1StringDictionary00::EAttributeTokenType14,
	 CWml1_1StringDictionary00::EAttributeTokenValue,
	 CWml1_1StringDictionary00::EAttributeTokenVspace,
	 CWml1_1StringDictionary00::EAttributeTokenWidth,
	 CWml1_1StringDictionary00::EAttributeTokenXmllang,
	 0}; // ok to end with zero as this is used by a global token



const TInt iAttributeValueCodePage00[] = 
	{CWml1_1StringDictionary00::EAttributeValueTokenAcceptcharset,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign1,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign2,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign3,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign4,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign5,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign6,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlign7,
	 CWml1_1StringDictionary00::EAttributeValueTokenAlt,
	 CWml1_1StringDictionary00::EAttributeValueTokenClass,
	 CWml1_1StringDictionary00::EAttributeValueTokenColumns,
	 CWml1_1StringDictionary00::EAttributeValueTokenContent1,
	 CWml1_1StringDictionary00::EAttributeValueTokenContent2,
	 CWml1_1StringDictionary00::EAttributeValueTokenDomain,
	 CWml1_1StringDictionary00::EAttributeValueTokenEmptyok1,
	 CWml1_1StringDictionary00::EAttributeValueTokenEmptyok2,
	 CWml1_1StringDictionary00::EAttributeValueTokenFormat,
	 CWml1_1StringDictionary00::EAttributeValueTokenForua1,
	 CWml1_1StringDictionary00::EAttributeValueTokenForua2,
	 CWml1_1StringDictionary00::EAttributeValueTokenHeight,
	 CWml1_1StringDictionary00::EAttributeValueTokenHref1,
	 CWml1_1StringDictionary00::EAttributeValueTokenHref2,
	 CWml1_1StringDictionary00::EAttributeValueTokenHref3,
	 CWml1_1StringDictionary00::EAttributeValueTokenHspace,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttpequiv1,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttpequiv2,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttpequiv3,
	 CWml1_1StringDictionary00::EAttributeValueTokenId,
	 CWml1_1StringDictionary00::EAttributeValueTokenIvalue,
	 CWml1_1StringDictionary00::EAttributeValueTokenIname,
	 CWml1_1StringDictionary00::EAttributeValueTokenLabel,
	 CWml1_1StringDictionary00::EAttributeValueTokenLocalsrc,
	 CWml1_1StringDictionary00::EAttributeValueTokenMaxlength,
	 CWml1_1StringDictionary00::EAttributeValueTokenMethod1,
	 CWml1_1StringDictionary00::EAttributeValueTokenMethod2,
	 CWml1_1StringDictionary00::EAttributeValueTokenMode1,
	 CWml1_1StringDictionary00::EAttributeValueTokenMode2,
	 CWml1_1StringDictionary00::EAttributeValueTokenMultiple1,
	 CWml1_1StringDictionary00::EAttributeValueTokenMultiple2,
	 CWml1_1StringDictionary00::EAttributeValueTokenName,
	 CWml1_1StringDictionary00::EAttributeValueTokenNewcontext1,
	 CWml1_1StringDictionary00::EAttributeValueTokenNewcontext2,
	 CWml1_1StringDictionary00::EAttributeValueTokenOnenterbackward1,
	 CWml1_1StringDictionary00::EAttributeValueTokenOnenterforward1,
	 CWml1_1StringDictionary00::EAttributeValueTokenOnpick1,
	 CWml1_1StringDictionary00::EAttributeValueTokenOntimer1,
	 CWml1_1StringDictionary00::EAttributeValueTokenOptional1,
	 CWml1_1StringDictionary00::EAttributeValueTokenOptional2,
	 CWml1_1StringDictionary00::EAttributeValueTokenPath,
	 CWml1_1StringDictionary00::EAttributeValueTokenScheme,
	 CWml1_1StringDictionary00::EAttributeValueTokenSendreferer1,
	 CWml1_1StringDictionary00::EAttributeValueTokenSendreferer2,
	 CWml1_1StringDictionary00::EAttributeValueTokenSize,
	 CWml1_1StringDictionary00::EAttributeValueTokenSrc1,
	 CWml1_1StringDictionary00::EAttributeValueTokenSrc2,
	 CWml1_1StringDictionary00::EAttributeValueTokenSrc3,
	 CWml1_1StringDictionary00::EAttributeValueTokenOrdered1,
	 CWml1_1StringDictionary00::EAttributeValueTokenOrdered2,
	 CWml1_1StringDictionary00::EAttributeValueTokenTabindex,
	 CWml1_1StringDictionary00::EAttributeValueTokenTitle,
	 CWml1_1StringDictionary00::EAttributeValueTokenType1,
	 CWml1_1StringDictionary00::EAttributeValueTokenType2,
	 CWml1_1StringDictionary00::EAttributeValueTokenType3,
	 CWml1_1StringDictionary00::EAttributeValueTokenType4,
	 CWml1_1StringDictionary00::EAttributeValueTokenType5,
	 CWml1_1StringDictionary00::EAttributeValueTokenType6,
	 CWml1_1StringDictionary00::EAttributeValueTokenType7,
	 CWml1_1StringDictionary00::EAttributeValueTokenType8,
	 CWml1_1StringDictionary00::EAttributeValueTokenType9,
	 CWml1_1StringDictionary00::EAttributeValueTokenType10,
	 CWml1_1StringDictionary00::EAttributeValueTokenType11,
	 CWml1_1StringDictionary00::EAttributeValueTokenType12,
	 CWml1_1StringDictionary00::EAttributeValueTokenType13,
	 CWml1_1StringDictionary00::EAttributeValueTokenType14,
	 CWml1_1StringDictionary00::EAttributeValueTokenValue,
	 CWml1_1StringDictionary00::EAttributeValueTokenVspace,
	 CWml1_1StringDictionary00::EAttributeValueTokenWidth,
	 CWml1_1StringDictionary00::EAttributeValueTokenXmllang,
	 CWml1_1StringDictionary00::EAttributeValueTokenCom,
	 CWml1_1StringDictionary00::EAttributeValueTokenEdu,
	 CWml1_1StringDictionary00::EAttributeValueTokenNet,
	 CWml1_1StringDictionary00::EAttributeValueTokenOrg,
	 CWml1_1StringDictionary00::EAttributeValueTokenAccept,
	 CWml1_1StringDictionary00::EAttributeValueTokenBottom,
	 CWml1_1StringDictionary00::EAttributeValueTokenClear,
	 CWml1_1StringDictionary00::EAttributeValueTokenDelete,
	 CWml1_1StringDictionary00::EAttributeValueTokenHelp,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttp1,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttp2,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttps1,
	 CWml1_1StringDictionary00::EAttributeValueTokenHttps2,
	 CWml1_1StringDictionary00::EAttributeValueTokenMiddle,
	 CWml1_1StringDictionary00::EAttributeValueTokenNowrap,
	 CWml1_1StringDictionary00::EAttributeValueTokenOnenterbackward2,
	 CWml1_1StringDictionary00::EAttributeValueTokenOnenterforward2,
	 CWml1_1StringDictionary00::EAttributeValueTokenOnpick2,
	 CWml1_1StringDictionary00::EAttributeValueTokenOntimer2,
	 CWml1_1StringDictionary00::EAttributeValueTokenOptions,
	 CWml1_1StringDictionary00::EAttributeValueTokenPassword,
	 CWml1_1StringDictionary00::EAttributeValueTokenReset,
	 CWml1_1StringDictionary00::EAttributeValueTokenText,
	 CWml1_1StringDictionary00::EAttributeValueTokenTop,
	 CWml1_1StringDictionary00::EAttributeValueTokenUnknown,
	 CWml1_1StringDictionary00::EAttributeValueTokenWrap,
	 CWml1_1StringDictionary00::EAttributeValueTokenWww,
	 0}; // ok to end with zero as this is used by a global token


#endif //__WML1_1STRINGDICTIONARY00_H__
