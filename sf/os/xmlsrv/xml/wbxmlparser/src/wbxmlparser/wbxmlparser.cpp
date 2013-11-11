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
#include <utf.h>
#include <charconv.h>

#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <xml/stringdictionarycollection.h>
#include <xml/wbxmlextensionhandler.h>
#include <xml/xmlframeworkerrors.h>
#include <xml/plugins/charsetconverter.h>
#include <xml/plugins/stringdictionary.h>

#include "xml/wbxmlparsererrors.h"

#include "wbxmlconstants.h"
#include "wbxmlparser.h"
#include "descriptordatasource.h"

using namespace Xml;

/**
The local buffer granularity.
@internalTechnology
*/
const TUint  KBufferGran			= 0x100;

/**
Bit mask indicating that there are attributes belonging to this element.
@internalTechnology
*/
const TUint8 KAttributesFollow		= 0x80;

/**
Bit mask indicating that there is content following this element and/or its attributes.
@internalTechnology
*/
const TUint8 KContentsFollow		= 0x40;

/**
Tokens with a value <128 (0x80) indicate an AttrStart, >=128 indicate AttrValue.
@internalTechnology
*/
const TUint8 KAttributeBoundary		= 0x80;

/**
Wbxml encode the major version minus 1 in the upper 4-bits, and the minor version 
in the lower 4-bits.
This parser conforms to the version 1.3.
@internalTechnology
*/
const TUint8 KSupportedVersion		= 0x03; // version 1.3

/**
Value used to represent an unknown PublicId as stated by OMNA.
@see http://www.openmobilealliance.org/tech/omna/omna-wbxml-public-docid.htm
@internalTechnology
*/
const TUint8 KUnknownPublicId		= 0x01;

/**
Mib enum value representing the utf-8 charcter set as defined by IANA.
@see http://www.iana.org/assignments/character-sets
@internalTechnology
*/
const TInt	 KUtf8MibEnum			= 0x6A; // 106 decimal

/**
Extra local byte storage required for the heap.
@internalTechnology
*/
const TUint  KPubIdExtraStorage		= 20;

/**
Codepage seperator, used to enable codepage identification.
@internalTechnology
*/
_LIT8(KCodepageSeperator, "~");


LOCAL_C void DestroyCBufFlat(TAny* aCBufFlat)
/**
This method is used when pointer reallocation is needed and the
original pointer needs to be cleaned via the cleanup stack.
Examples of this could be when data is streamed and not complete
causing reiteration of parsing for the same data.

@param				aCBufFlat the wide buffer.
@internalTechnology

*/
	{
	delete *static_cast<CBufFlat**>(aCBufFlat);
	*static_cast<CBufFlat**>(aCBufFlat) = NULL;
	}



LOCAL_C void DestroyHBufC8(TAny* aHBufC)
/**
This method is used when pointer reallocation is needed and the
original pointer needs to be cleaned via the cleanup stack.
Examples of this could be when data is streamed and not complete
causing reiteration of parsing for the same data.

@param				aHBufC the narrow buffer.
@internalTechnology

*/
	{
	delete *static_cast<HBufC8**>(aHBufC);
	*static_cast<HBufC8**>(aHBufC) = NULL;
 	}



MParser* CWbxmlParser::NewL(TAny* aInitParams)
/**
This method creates an instance of this class.

If there is insufficient memory to create the object, the function leaves.

@return				The instantiated object.

@param				aInitParams Initialisation parameters for the parser.

@see				TParserInitParams

*/
	{
	CWbxmlParser* self = new(ELeave) CWbxmlParser(reinterpret_cast<TParserInitParams*>(aInitParams));
	
	CleanupStack::PushL(self);
	self->ConstructL();

	CleanupStack::Pop(self);
	
	return (self);
 	}



CWbxmlParser::CWbxmlParser(TParserInitParams* aInitParams)
/**
Constructor.

@param				aInitParams The initialisation parameters.
*/
:	iParserRunState(EParserRunStateVersion),
	iParserModeState(EParserModeStateTag),
	iIsPiInstruction(EFalse),
	iContentHandler(reinterpret_cast<MContentHandler*>(aInitParams->iContentHandler)),
	iStringDictionaryCollection(reinterpret_cast<RStringDictionaryCollection*>(aInitParams->iStringDictionaryCollection)),
	iCharSetConverter (reinterpret_cast<CCharSetConverter*>(aInitParams->iCharSetConverter)),
	iParseMode(EErrorOnUnrecognisedTags),
	iElementStack(reinterpret_cast<RElementStack*>(aInitParams->iElementStack)),
	iLastChunk(EFalse),
	iNumEnds(0)
	{
	// do nothing;
	}



void CWbxmlParser::ConstructL()
/**
Two phase construction.

@param				aDictionaryMimeType The String Distionary to load.

*/
	{
	// We use a Flat as we need a contiguous memory area when obtaining a 
	// descriptor of the whole buffer.
  	iUtf8String = CBufFlat::NewL(KBufferGran);
	}



void CWbxmlParser::Release()
/**
Destruction process method.
ECom is responsible for actually maintaining this object, we need a way to clean up memory.
This is due to the object being placed, and remaining, on the garbage heap once ECom is 
finished with it. As there is no other use other than the framework we can safely clean up.
*/
	{
	delete (this);
	}



CWbxmlParser::~CWbxmlParser()
/**
Destructor.

*/
	{
	// Complete the tidy-up of memory for shutdown.
	TidyMemoryForReuse();

	if (iUtf8String) 
		{
		delete iUtf8String;
		}
	
	// We don't own these, do not delete:
	//
	// iContentHandler;
	// iStringDictionaryCollection;
	// iCharSetConverter;
	// iElementStack;
	}



void CWbxmlParser::TidyMemoryForReuse()
/**
Cleans up memory created by a parse of a document, ready to be reused 
or recreated again for another.
*/
	{
	// All memory here created during parse phase.
	for (TInt i=0, c=iAttributes.Count(); i<c; ++i)
		{
		iAttributes[i].Close();
		}
	iAttributes.Reset();
	
	delete iStringTable;
	iStringTable = NULL;

	delete iFormalPublicId;
	iFormalPublicId = NULL;

	delete iEncodingNull;
	iEncodingNull = NULL;

	if (iUtf8String) 
		{
		iUtf8String->Reset();
		}

	delete iDataSource;
	iDataSource = NULL;
	}



void CWbxmlParser::SetContentSink (MContentHandler& aContentHandler)
/**
This method allows for the correct streaming of data to another plugin in the chain.

@post				the next plugin in the chain is set to receive our callbacks.

*/
	{
	iContentHandler = &aContentHandler;
	}
	


RStringPool& CWbxmlParser::StringPool()
/**
This accessor method obtains a reference to the string dictionary string pool. 

@return				A reference to the string dictionary string pool.

*/
	{
	return iStringDictionaryCollection->StringPool();
	}


	
	
void CWbxmlParser::ParseChunkL(const TDesC8& aChunk)
/**
This method starts the parser parsing a descriptor.
The descriptor does not contain the last part of the document. 

Eof is NOT checked for, so if the descriptor contains the last
part of the document OnEndDocument will NOT be called.

If this is not the first document you are parsing then Reset MUST be called,
otherwise undefined results will occur.

@pre				OpenL has been called

@param				aChunk the chunk to parse - not the last chunk

*/
	{
	iLastChunk = EFalse;

	// Create the Data Source if this is for the first time	
	if (iDataSource == NULL)
		{
		iDataSource = CDescriptorDataSource::NewL();
		}

	DoReadAndParseDescriptorL(aChunk);
	}


void CWbxmlParser::ParseLastChunkL(const TDesC8& aFinalChunk)
/**
This method starts the parser parsing a descriptor.
The descriptor contains the last part of the document or the
whole document.

Eof is checked for, so OnEndDocument will be called.

@pre				OpenL has been called

@param				aFinalChunk the final chunk to parse

*/
	{
	iLastChunk = ETrue;

	// Create the Data Source if this is for the first time	
	if (iDataSource == NULL)
		{
		iDataSource = CDescriptorDataSource::NewL();
		}

	DoReadAndParseDescriptorL(aFinalChunk);		
	
	// Reset the parser incase another XML doc is to be parsed straight afterwards.
	Reset();
	}



void CWbxmlParser::DoReadAndParseDescriptorL(const TDesC8& aDescriptor)
/**
Obtains and parses to the end of data. 

Should NEVER return KErrNone.
Should return KErrEof when no more data from this descriptor can be parsed.
*/
	{
	// if there is any data left over from the last parse
	// it will be concatenated for this parse.
	iDataSource->ReadBytesL(aDescriptor);

	TInt err = KErrNone;
	
	TInt forever=1;
	TRAP(err, do { DoParseL(); } while (forever) );
		
	// Send the right callback
	CheckErrorL(err);

	// We should not leave if we read the end of a chunk or the file.
	if (err != KErrNone && err != KErrEof)
		{
		Reset();
		User::Leave(err);
		}
	}
	
	
	
void CWbxmlParser::Reset()
/**
This method allows the parser to reset the document to be parsed.

*/
	{
	// start parsing in the right state
	iParserRunState = EParserRunStateVersion;
			
	// Clean up memory usage as we are parsing afresh.
	TidyMemoryForReuse();
	
	iNumEnds=0;
	}


const TInt KOptionalFeatures  = ERawContent;
const TInt KMandatoryFeatures = EErrorOnUnrecognisedTags;

TInt CWbxmlParser::EnableFeature(TInt aParseMode)
	{
	if(aParseMode & ~(KOptionalFeatures | KMandatoryFeatures))
		return KErrNotSupported;

	iParseMode |= aParseMode | KMandatoryFeatures;

	return KErrNone;
	}

TInt CWbxmlParser::DisableFeature(TInt aParseMode)
	{
	if(aParseMode & KMandatoryFeatures)
		return KErrNotSupported;

	iParseMode &= ~aParseMode;

	return KErrNone;
	}

TBool CWbxmlParser::IsFeatureEnabled(TInt aParseMode) const
	{
	return iParseMode & aParseMode;
	}





void CWbxmlParser::DoParseL()
/**
This method is based on states as it is long-running.

The order of the type of bytes to parse is specified by the WBXML 1.3 BNF rules
described at:
	http://www.openmobilealliance.org/wapdocs/wap-192-wbxml-20010725-a.pdf

For a full list of documents goto:
	http://www.openmobilealliance.org/wapdownload.html


If a section is not complete, i.e. Eof, it will Leave and be restarted when
the section is rescheduled, i.e. when there is more data to parse.

Multistate sections that are short running, could continue into the next section
instead of break'ing.

Breaking will not allow a section to be restarted due to EndTransaction 
being called.

@panic				EWbxmlParserErrorUnexpectedState If any other state other than
					those defined are set.

*/
	{
	iDataSource->StartTransaction();


	switch (iParserRunState)
		{
		case EParserRunStateVersion:
			{
			VersionL();
			iParserRunState = EParserRunStatePublicId;
			break;
			}
		case EParserRunStatePublicId:
			{
			PublicIdL();
			iParserRunState = EParserRunStateCharset;
			break;
			}
		case EParserRunStateCharset:
			{
			CharsetL();
			iParserRunState = EParserRunStateStringTbl;
			break;
			}
		case EParserRunStateStringTbl:
			{
			StrtblL();
			LoadFormalDictionaryL();
			iParserRunState = EParserRunStateBody;
			OnStartDocumentL();
			break;
			}
		case EParserRunStateBody:
			{
			// Parse until Eof
			BodyL();
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, ::Panic(EWbxmlParserErrorUnexpectedState));
			break;
			}
		}

	iDataSource->EndTransaction();

	}



void CWbxmlParser::CheckErrorL(TInt aError)
	{
	if (aError == KErrEof)
		{
		// Check if the end of the document.
		if (iLastChunk)
			{
			// Mismatched number of END tokens means something wrong with doc (truncated?)
			if (iNumEnds)
				{
				iContentHandler->OnError(KErrXmlDocumentCorrupt);
				}
			else
				{
				// EOF reached
				iContentHandler->OnEndDocumentL(KErrNone);
				}
			}
		else
			{
			// No just out of data.
			iDataSource->Rollback();
			}
		}
	else
		{
		iContentHandler->OnError(aError);
		}
	}



void CWbxmlParser::VersionL()
/**
BNF: version = u_int8

@leave				KErrXmlUnsupportedDocumentVersion

*/
	{
	iVersion = iDataSource->GetByteL();

	// Assume backwards compatibility.
	if (iVersion > KSupportedVersion)
		{
		User::Leave(KErrXmlUnsupportedDocumentVersion);
		}
	}



void CWbxmlParser::PublicIdL()
/**
BNF: publicid = mb_u_int32 | (zero index)
zero = u_int8
index = mb_u_int32

*/
	{
	TInt32 word = 0;

	if ((iPublicId = GetMultiByteInt32L ()) == 0)
		{
		// this was zero so an offset into the string table follows
		word = GetMultiByteInt32L ();
		}

	iPublicIdIndex = word;
	}	



void CWbxmlParser::CharsetL()
/**
BNF: charset = mb_u_int32

*/
	{
	// Can't assert position as it depends on the previous content size

	TInt32 mibEnum = GetMultiByteInt32L();

	if (mibEnum == 0)
		{
		mibEnum = KUtf8MibEnum;
		}

	iCharSetConverter->PrepareCharConvL(iCharSetUid, mibEnum);

	TBuf8<1> nullChar;
	nullChar.Append(0);

	ConvertToEncodingL(KCharacterSetIdentifierAscii, nullChar, iCharSetUid, iEncodingNull);

	// Leave in an expected state
	iCharSetConverter->PrepareToConvertToOrFromL(iCharSetUid);
	}



void CWbxmlParser::StrtblL()
/**
BNF: strtbl = length *byte
length = mb_u_int32

*/
	{
	// If we leave and we are streaming, the state will not change.
	// We will re-enter this code and obtain a memory leak when we new the memory again.
	HBufC8* strtbl = NULL;
	CleanupStack::PushL(TCleanupItem(DestroyHBufC8, &strtbl));//push buffer's address

	if ((iStringTblLength = GetMultiByteInt32L ()) != 0)
		{
		// We may not have enough memory to read from the file buffer.
		// If this happened we would leave and iStringTable would be
		// reinitialised each time round until we got all bytes from the buffer.
		// This could happen during streaming.
		strtbl = HBufC8::NewMaxL(iStringTblLength);
		TPtr8 append = strtbl->Des();
		append = iDataSource->GetBytesL(iStringTblLength);

		// iPublicIdIndex could be zero and we still have a string pool,
		// but if its not then the stringpool should be larger than the index.
		if (strtbl->Size() < iPublicIdIndex)
			{
			User::Leave(KErrXmlDocumentCorrupt);
			}
		}
	CleanupStack::Pop(&strtbl);
	iStringTable = strtbl;

	}



void CWbxmlParser::LoadFormalDictionaryL()
/**
This method handles the loading of the string dictionary required for parsing.
Based on the already parsed data, it supplies the correct information to the actual
loading function.

*/
	{
	CBufFlat* formal = NULL;
	CleanupStack::PushL(TCleanupItem(DestroyCBufFlat, &formal));//push buffer's address

	// Load the Dictionary from the string table
	if (iPublicId == 0)
		{
		// Get the URI from the string table
		formal = CBufFlat::NewL(KBufferGran);
		ExtractStringUtf8L(iPublicIdIndex, *formal);
		formal->Compress();
		}

	// Member variable about to be used, so set.
	CleanupStack::Pop(&formal);
	iFormalPublicId = formal;

	// Always load codepage 0 to begin with
	LoadDictionaryL(0);

	// Remember the uri as we have to report it in our callbacks.
	// This has been optimised in wbxml documents as the 
	// initial namespace will not appear in the form of a SwitchCodePage
	// as codepage 0 is the default.
	iStringDictionaryCollection->CurrentDictionaryL().NamespaceUri(iCurrentUri);
	iTagCurrentUri = iAttributeCurrentUri = iCurrentUri;
	iCurrentCodePage = iTagCurrentCodePage = iAttributeCurrentCodePage = 0;
	}


void CWbxmlParser::BodyL()
/**
This method cordinates the parsing of the 'body' part of a wbxml document.

It is based on states as it is a long running.

Briefly, the parser loops around each token parsed and acts accordingly.
It does not look forward to see what the next byte is, this is more to the
BNF style.

This method of parsing is more efficient in terms of speed and memory, but
harder to understand.

@panic				EWbxmlParserErrorUnexpectedState If any other state other than
					those defined are set.

*/
	{

	// IMPORTANT: The way this works.
	//
	// Each time a byte is read it is matched against it's global token and handled.
	// If a match is not found then it is matched against default.
	// Within default it is matched against the sub-token.
	//
	// For example:
	//		For an element that has the BNF rule:
	//		element = ([switchPage] stag) [ 1*attribute END ] [ *content END ]
	//
	//      We match a SwitchPage which is handled by EGlobalTokenSWITCH_PAGE
	//			switchPage = SWITCH_PAGE pageindex
	//		then on the next iteration
	//      We then match an stag that can be handled by one of the following:
	//			stag = TAG | (literalTag index)
	//		
	//			We match default for TAG, as this is not a global token. 
	//				so this is handled by EParserModeStateTag
	//			or
	//			literalTag matches to 
	//				EGlobalTokenLITERAL, EGlobalTokenLITERAL_C, EGlobalTokenLITERAL_A, EGlobalTokenLITERAL_AC.
	//		
	//			From either of these we can tell if we have attribute or content following.
	//			So we set our substate to 
	//				EParserModeStateAttribute
	//
	//			If this is EParserModeStateAttribute then
	//					attribute = attrStart *attrValue
	//
	//			attrStart = ([switchPage] ATTRSTART) | ( LITERAL index )
	//			attrValue = ([switchPage] ATTRVALUE) | string | extension | entity | opaque
	//
	//					Here we only need to handle ATTRSTART and ATTRVALUE
	//					under state EParserModeStateAttribute. The others are handled 
	//					by the global token states.
	//
	//					The only thing is that we need to make sure that if in
	//					EParserModeStateAttribute state, we only get a EGlobalTokenLITERAL
	//
	//
	//			Both of these end with EGlobalTokenEND that allows us to reset the state
	//			accordingly.
	//
	//			content is also made up from different BNF rules and these are handled 
	//			in the same manner but in state EParserModeStateTag.


	TUint8 byte = iDataSource->GetByteL();

	switch (byte)
		{
		case EGlobalTokenSWITCH_PAGE:
			{
			SwitchPageL();
			break;
			}
		case EGlobalTokenEND:
			{
			DetermineNextToParseL();
			break;
			}
		case EGlobalTokenENTITY:
			{
			EntityL();
			break;
			}
		case EGlobalTokenSTR_I:
			{
			InlineL();
			break;
			}
		case EGlobalTokenSTR_T:
			{
			TableRefL();
			break;
			}
		case EGlobalTokenPI:
			{
			// We will have an end tag to match this.
			// That way we know when to emit the event.
			iIsPiInstruction = ETrue;
			iNumEnds++;
			// We are expecting an attribute next
			iParserModeState = EParserModeStateAttribute;
			break;
			}
		case EGlobalTokenLITERAL:
		case EGlobalTokenLITERAL_C:
		case EGlobalTokenLITERAL_A:
		case EGlobalTokenLITERAL_AC:
			{
			LiteralL(byte);
			break;
			}
		case EGlobalTokenEXT_0:
		case EGlobalTokenEXT_1:
		case EGlobalTokenEXT_2:
			{
			ExtensionL(byte);
			break;
			}
		case EGlobalTokenEXT_I_0:
		case EGlobalTokenEXT_I_1:
		case EGlobalTokenEXT_I_2:
			{
			ExtensionIL(byte);
			break;
			}
		case EGlobalTokenEXT_T_0:
		case EGlobalTokenEXT_T_1:
		case EGlobalTokenEXT_T_2:
			{
			ExtensionTL(byte);
			break;
			}
		case EGlobalTokenOPAQUE:
			{
			OpaqueL();
			break;
			}
		default:
			{
			ParseTagOrAttributeL(byte);
			break;
			}
		}; // switch
	}



void CWbxmlParser::DetermineNextToParseL()
/**
Based on the bits set of the previous Element token
work out what there is to parse next and set the parser states accordingly.

*/
	{
	// END follows an attribute, content, or pi
	// A tag by itself doesn't have an END as the Excess identifies
	//	if anything should follow or not.
	// A tag with just attributes has no contents so this END ends the element
	// A tag with just content, the content END ends the element
	// A tag with both, the content END ends the element
	//
	iNumEnds--;

	switch (iParserModeState)
		{
		case EParserModeStateTag:
			{

			// Remember a TAG by itself (no attr or content) has no END.
				
			// This is the end of content
			// Must be character data 
			iContentHandler->OnContentL(iUtf8String->Ptr(0), KErrNone);
			OnEndElementL();

			// We don't amend the size as if we have one long
			// string there may be others. So dont use Reset.
			iUtf8String->Delete(0, iUtf8String->Size());
			break;
			}
		case EParserModeStateAttribute:
			{
			// Keep track of Tag stack as we may be leaving a PI definition

			if (iIsPiInstruction)
				{
				OnProcessingInstructionL();
				iIsPiInstruction = EFalse;
				
				// no excess for PI attributes as they are not really attributes
				iParserModeState = EParserModeStateTag;
				}
			else
				{
				// Attributes are always before content
				if (iTagExcess & KAttributesFollow)
					{ // end of attribute list
					iTagExcess ^= KAttributesFollow;

					// Store previous attributes value
					HandleAttributeEndL();
					OnStartElementL(); 
					}
			
				// if no content follow send the event
				if (!(iTagExcess & KContentsFollow))
					{
					// This is an END that matches no attribute or content
					// must be an end element
					OnEndElementL();

					// contents may follow and are handled in tag state.
					iParserModeState = EParserModeStateTag;
					}
				else
				// If there is content handle it in tag state.
					{	  
					iTagExcess ^= KContentsFollow;
					iParserModeState = EParserModeStateTag;
					}
				}
			// We don't amend the size as if we have one long
			// string there may be others. So dont use Reset.
			iUtf8String->Delete(0, iUtf8String->Size());
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, ::Panic(EWbxmlParserErrorUnexpectedState));
			}
		}
	}



void CWbxmlParser::ParseTagOrAttributeL(TUint8 aByte)
/**
BNF: element = ([SWITCHPAGE] stag) [1*attribute END] [*content END] 
BNF: attribute = attrStart *attrValue
BNF: attrStart = ([switchPage] ATTRSTART) | ( LITERAL index )
BNF: attrValue = ([switchPage] ATTRVALUE) | string | extension | entity | opaque
BNF: content = element | string | extension | entity | pi | opaque

*/
	{
	// If we are here then we depend on the internal state to determine what
	// we are to receive next based on the BNF ruleset snippet described above.
	//
	// So the stag, ATTRSTART, or ATTRVALUE bytes will be handled here. 
	switch (iParserModeState)
		{
		case EParserModeStateTag: // stag
			{
			HandleTagL(aByte);
			break;
			}
		case EParserModeStateAttribute: // attrStart, attrValue
			{
			HandleAttributeL(aByte);
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, ::Panic(EWbxmlParserErrorUnexpectedState));
			}
		}; // switch
	}




void CWbxmlParser::HandleTagL(TUint8 aByte)
/**
This method handles the tag processing.

It extracts from the passed data other information as stipulated by the 
wbxml standard BNF ruleset, and allows the parser to parse the 
current and as yet expected but un-parsed data with a degree of success.

@param				aByte The current byte of data parsed.

@leave				KErrXmlDocumentCorrupt If we are not in the tag codepage.

@panic				EWbxmlParserErrorUnexpectedState If the parser is not in the
					tag state.

*/
	{
	__ASSERT_ALWAYS(iParserModeState == EParserModeStateTag, ::Panic(EWbxmlParserErrorUnexpectedState));
	
	// There should have been a switch codepage for this state unless 
	// it is a literal.
	if ((~(KAttributesFollow | KContentsFollow) & aByte) != EGlobalTokenLITERAL	&& 
		iCurrentCodePage != iTagCurrentCodePage)
		{
		User::Leave(KErrXmlDocumentCorrupt);
		}


	iTagExcess = (KAttributesFollow | KContentsFollow) & aByte;
	iTagValue = ~(KAttributesFollow | KContentsFollow) & aByte;

	if (iTagExcess & KAttributesFollow)
		{
		iNumEnds++;
		}
	if (iTagExcess & KContentsFollow)
		{
		iNumEnds++;
		}
	

	// Check if this is character content from a previous tag,
	// if so we need to send it before handling this tag. 
	//
	// Tags are not associated with any END unless they have attributes or content.
	// So the only way to determine this is to say that we know this is the start 
	// of a tag, any character content in the buffer must belong to the previous tag.
	if (iUtf8String->Size())
		{
		iContentHandler->OnContentL(iUtf8String->Ptr(0), KErrNone);
		// We don't amend the size as if we have one long
		// string there may be others. So dont use Reset.
		iUtf8String->Delete(0, iUtf8String->Size());
		}

	//
	// Note we will never need a Content state as content is made up from
	// the other BNF rules. These are handled by the different parts
	// of this code.
	// The parser only has two states, tag and attribute.
	//

	RString tagName;
	CleanupClosePushL(tagName);


	// Need to determine whether this is an stag value or a literal value
	// literal values we can access the stringpool for.
	//
	// Test for a LITERAL, remember all literals LITERAL, LITERAL_A, LITERAL_C
	// and LITERAL_AC are LITERAL with the bits 6 or 7 set)
	//
	// If this is a LITERAL then the LiteralL() function will handle this
	// initially and then pass it to us.
	if ((~(KAttributesFollow | KContentsFollow) & aByte) != EGlobalTokenLITERAL)
		{
		// Not the global tokens EGlobalTokenLITERAL, EGlobalTokenLITERAL_C, 
		// EGlobalTokenLITERAL_A, or EGlobalTokenLITERAL_AC
		//
		iStringDictionaryCollection->CurrentDictionaryL().ElementL(iTagValue, tagName);
		}
	else // literal values are in the string table
		{
		TInt32 index = GetMultiByteInt32L();
		ExtractStringUtf8L(index, *iUtf8String);
		tagName = StringPool().OpenStringL(iUtf8String->Ptr(0));
		// we won't use this again, eg to append to,
		// and value parts won't be part of literal
		//
		// We don't amend the size as if we have one long
		// string there may be others. So dont use Reset.
		iUtf8String->Delete(0, iUtf8String->Size());
		}


	// Always push onto the stack for callbacks to access
	User::LeaveIfError(iElementStack->Append(tagName));


	// If we have attributes there will be an END tag
	// So wait before sending an element event as we need to pass them up as well
	if (iTagExcess & KAttributesFollow)
		{
		// preserve the iTagExcess for the END
		iParserModeState = EParserModeStateAttribute;
		}
	else
		{
		// No excess, so send event
		OnStartElementL();
		}

	// if no attributes but content process them instead
	if (!(iTagExcess & KAttributesFollow) && iTagExcess & KContentsFollow)
		{	  
		// preserve the iTagExcess for the END
		iParserModeState = EParserModeStateTag;
		}

	// sometimes there are no attributes and content.
	// in this case the END token will NOT appear to trigger the appropriate events
	// so we check the excess
	if (!(iTagExcess & (KAttributesFollow|KContentsFollow)))
   		{
		OnEndElementL();
 		iParserModeState = EParserModeStateTag;
		}

	CleanupStack::Pop(&tagName);
	}



void CWbxmlParser::HandleAttributeL(TUint8 aByte)
/**
This method handles the attribute processing.

Attributes are split into two codespaces attrStart and attrValue and
must be handled in the parsers attribute state, EParserModeStateAttribute.

@param				aByte The current byte of data parsed.

@leave				KErrXmlDocumentCorrupt If we are not in the attribute codepage.

@panic				EWbxmlParserErrorUnexpectedState If the parser is not in the 
					attribute state and attributes should NOT follow.

*/
	{
	if (iIsPiInstruction)
		{
		iPiTarget = aByte;
		
		// There is no excess for PI instructions.
		}
	else
		{
		// Contents are handled in Tag state
		__ASSERT_ALWAYS(iParserModeState == EParserModeStateAttribute && (iTagExcess & KAttributesFollow), 
			::Panic(EWbxmlParserErrorUnexpectedState));
		}

	// There should have been a switch codepage for this state unless 
	// it is a literal.
	if ((~(KAttributesFollow | KContentsFollow) & aByte) != EGlobalTokenLITERAL	&& 
		iCurrentCodePage != iAttributeCurrentCodePage)
		{
		User::Leave(KErrXmlDocumentCorrupt);
		}


	// Attribute start are <128
	// Attribute value are =>128
	// Anything else should be handled by the other switch cases
	//
	// EGlobalTokenLITERAL has valuse 04 and can be specified for attrStart
	if (aByte < KAttributeBoundary)
		{
		// start on a new attribute
		AttrStartL(aByte);
		}
	else
		{
		AttrValueL(aByte);
		}
	}



void CWbxmlParser::HandleAttributeEndL()
/**
This method handles an attribute end. This is a helper function
invoked from different areas in the code to assemble the current attributes
data into its element on the attribute list. Other storage areas are also cleaned.

*/
	{
	if (iUtf8String->Size())
		{
		// We will always have attrValue (if present) matched to a attrStart
		// Obtain it.
		
		RString val;
		CleanupClosePushL(val);

		val = StringPool().OpenStringL(iUtf8String->Ptr(0));

		TInt count = iAttributes.Count();
		if(!count)
 		User::Leave(KErrXmlDocumentCorrupt);
		iAttributes[count-1].SetValue(val);


		// We don't amend the size as if we have one long
		// string there may be others. So dont use Reset.
		iUtf8String->Delete(0, iUtf8String->Size());
		CleanupStack::Pop(&val);
		}
	}



void CWbxmlParser::SwitchPageL()
/**
This method handles the codepage switch as stipulated by the document being parsed.
Codepage switches happen when the scope of an item to be parsed changes. 
Codepages are associated with tags, attributes, and attribute values and represent
data required within that scope. The data has no meaning outside that scope.

BNF: switchPage = SWITCH_PAGE pageindex
pageindex = u_int8

@panic				EWbxmlParserErrorUnexpectedState If any other state other than
					those defined are set.

*/
	{
	TUint8 pageindex = iDataSource->GetByteL();

	iCurrentCodePage = iStringDictionaryCollection->CurrentDictionaryL().SwitchCodePage(pageindex);


	// We have two designs for associating codepages with Dictionaries.
	// The first is when changes in codepage lie within the scope of the 
	// same Dictionary.
	// The second is when changes in codepage refer to a different Dictionary.
	// Here we try the second as the first failed.

	if (iCurrentCodePage == KErrXmlMissingStringDictionary)
		{
		LoadDictionaryL(pageindex);
		iCurrentCodePage = pageindex;
		}

	// Get the uri for this codepage
	iStringDictionaryCollection->CurrentDictionaryL().NamespaceUri(iCurrentUri);

	switch (iParserModeState)
		{
		case EParserModeStateTag:
			{
			iTagCurrentCodePage = pageindex;
			iTagCurrentUri = iCurrentUri;
			break;
			}
		case EParserModeStateAttribute:
			{
			iAttributeCurrentCodePage = pageindex;
			iAttributeCurrentUri = iCurrentUri;
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, ::Panic(EWbxmlParserErrorUnexpectedState));
			}
		}
	}



void CWbxmlParser::LoadDictionaryL(TInt aCodePage)
/**
This method assertains the correct information needed to load a string dictionary
to allow for a succesful code page switch. 

Used in two cases:

1) When the codepage is not loaded and we parsed a switch.
2) To switch between tag and attribute codepages.

@param				aCodePage The String Dictionary codepage to switch to.
*/
	{
	HBufC8* codespace = NULL;
	TPtr8 csPtr(0, 0);

	// uri form of public id.
	if (iFormalPublicId != NULL)
		{
		codespace = HBufC8::NewLC(iFormalPublicId->Size() + KPubIdExtraStorage);
		
		csPtr.Set (codespace->Des());
		csPtr.Copy(iFormalPublicId->Ptr(0));
		}
	else
	if (iPublicId == KUnknownPublicId)
		{
		// may be a RString representation of an int or a string
		RString pId;
		CleanupClosePushL(pId);
		
		// Client should have loaded a dictionary prior to parsing this doc.
		// We don't care which version of the pubId we get, e.g. formal or hex.
		iStringDictionaryCollection->CurrentDictionaryL().PublicIdentifier(pId);
		
		codespace = HBufC8::NewL(pId.DesC().Size() + KPubIdExtraStorage);
				
		// The publicId extracted from the .st file already has the codepage appended.
		// Extract it as this may be a codePage switch.
		csPtr.Set (codespace->Des());
		TInt sepPos = pId.DesC().Find(KCodepageSeperator);
		User::LeaveIfError(sepPos);
		csPtr.Copy(pId.DesC().Left(sepPos));
		
		pId.Close();
		CleanupStack::Pop(&pId);
		CleanupStack::PushL(codespace);
		}
	else
		{
		// PublicId is a multibyte int but we currently set this to be a TUint32 or 4-bytes.
		// Max hex numbers this can hold are 8. So we new a area big enough
		// for safety.
		codespace = HBufC8::NewLC(KPubIdExtraStorage);
		
		// Must always be in Hex to match.
		// pubId in .st file and in .rss file must also be in hex (excluding 0x).
		csPtr.Set (codespace->Des());
		csPtr.NumUC(iPublicId, EHex);
		}


	// The match string are unique between String Dictionaries.
	csPtr.Append(KCodepageSeperator);
	csPtr.AppendNumUC(aCodePage, EHex);
		
	iStringDictionaryCollection->OpenDictionaryL(*codespace);

	CleanupStack::PopAndDestroy(codespace);
	}



void CWbxmlParser::AttrStartL(TUint8 aByte)
/**
This method handles the processing of an attribute start.

Tokens with a value less than 128 indicate the start of an attribute. 
The attribute start token fully identifies the attribute name, e.g., URL=, 
and may optionally specify the beginning of the attribute value, e.g.,
PUBLIC="TRUE". Unknown attribute names are encoded with the globally unique
code LITERAL. LITERAL must not be used to encode any portion of an attribute value.

BNF: attrStart = ([switchPage] ATTRSTART) | ( LITERAL index )

@param				aByte The current byte of data parsed.

*/
	{
	// Attributes can have an optional value part associated.
	// Our .st files are written to seperate these values into the 
	// appropriate attribute, value tables. So there is no need to
	// parse for the value part. However, we need to concatenate it to
	// the actual value part if supplied.

	// Store value from previous attribute
	HandleAttributeEndL();


	RString attr;
	RString val;
	CleanupClosePushL(attr);
	CleanupClosePushL(val);

	// Test for a LITERAL, remember all literals LITERAL, LITERAL_A, LITERAL_C
	// and LITERAL_AC are LITERAL with the bits 6 or 7 set)
	// However, we should have a value <128 so we only really need a mask
	// containing ~KContentsFollow.
	if ((~(KAttributesFollow | KContentsFollow) & aByte) != EGlobalTokenLITERAL)
		{
		// not the global EGlobalTokenLITERAL token
		iStringDictionaryCollection->CurrentDictionaryL().AttributeL(aByte, attr);
		}
	else
		{
		TInt end = iUtf8String->Size();
   		TInt32 index = GetMultiByteInt32L();
  		ExtractStringUtf8L(index, *iUtf8String);

		// Add the attribute name to the string pool
  		attr = StringPool().OpenStringL(iUtf8String->Ptr(end));
		// clear this for the new value part
		//
		// We don't amend the size as if we have one long
		// string there may be others. So dont use Reset.
		iUtf8String->Delete(0, iUtf8String->Size());
		}

	// the args have no meaning in wbxml
	// we will add the attribute value later
	//
	// We don't copy iAttributeCurrentUri as closing is harmless,
	// we don't copy iBlank as we never opened it, using its default properties.
	iAttribute.Open(iAttributeCurrentUri, iBlank, attr, EAttributeType_NONE);

	User::LeaveIfError(iAttributes.Append(iAttribute));

	// get any optional value part if not a literal
	if ((~(KAttributesFollow | KContentsFollow) & aByte) != EGlobalTokenLITERAL)
		{
		iStringDictionaryCollection->CurrentDictionaryL().AttributeValueL(aByte, val);
		}
		
	// String should be empty, so store it
	iUtf8String->InsertL(iUtf8String->Size(), val.DesC());

	CleanupStack::Pop(&val);
	CleanupStack::Pop(&attr);
	}



void CWbxmlParser::AttrValueL(TUint8 aByte)
/**
This method handles the processing of an attribute value.

Tokens with a value of 128 or greater represent a well-known string present in 
an attribute value. These tokens may only be used to represent attribute values.
Unknown attribute values are encoded with string, entity or extension codes.

LITERAL must not be used to encode any portion of an attribute value. 

BNF: attrValue = ([switchPage] ATTRVALUE) | string | extension | entity | opaque

@param				aByte The current byte of data parsed.

*/
	{
	if ((~(KAttributesFollow | KContentsFollow) & aByte) == EGlobalTokenLITERAL)
		{
		User::Leave(KErrXmlDocumentCorrupt);		
		}
		
	// Value parts can appear as two. The optional value part from the attribute,
	// and the actual value part (>=128). These must be concatenated.
	// The optional part was inserted during the attribute retrieval stage.

	RString val;
	CleanupClosePushL(val);

	iStringDictionaryCollection->CurrentDictionaryL().AttributeValueL(aByte, val);

	// append it to string
	iUtf8String->InsertL(iUtf8String->Size(),val.DesC());

	CleanupStack::Pop(&val);
	}



void CWbxmlParser::LiteralL(TUint8 aLiteral)
/**
This method handles tha LITERAL token that encodes a tag or attribute name that
does not have a well-known token code. The actual meaning of the token 
(i.e., tag versus attribute name) is determined by the token parsing state. 
The tokens LITERAL_A, LITERAL_C, and LITERAL_AC are used when the tag posesses
respectively attributes, content, or both.
All literal tokens indicate a reference into the string table, which contains the 
actual name.

Note that the tags LITERAL_A, LITERAL_C, and LITERAL_AC are the LITERAL tag with the
appropriate combinations of bits 6 and 7 set.

BNF: literalTag = LITERAL | LITERAL_A | LITERAL_C | LITERAL_AC
BNF: attrStart = ([switchPage] ATTRSTART) | ( LITERAL index )

@param				aLiteral The current byte of data parsed.

@panic				EWbxmlParserErrorUnexpectedLogic If we are unable to determine
					how to handle this literal.

*/
	{
	if (iParserModeState == EParserModeStateTag)
		{
		HandleTagL(aLiteral);
		}
	else // must be an attribute
		{
		if (iTagExcess & KAttributesFollow)
			{
			// Could easily specify AttrStart as AttrValue don't have LITERALs
			HandleAttributeL(aLiteral);
			}
		else
			{
			// Not a tag and no attributes to follow!
			// Do not know what else it can be.
			__ASSERT_ALWAYS(EFalse, ::Panic(EWbxmlParserErrorUnexpectedLogic));
			}
		}
	}



void CWbxmlParser::ConvertToEncodingL(TUint32 aSrcCharSetUid, TDesC8& aSrc, 
									  TUint32	aDestCharSetUid, HBufC8*& aDest)
/**
This method converts the given character text from the specified source 
character encoding to the specified destination character set.

@param				aSrcCharSetUid
@param				aDestCharSetUid
@param				aDest

*/
	{
	// We should never fail as the entire data should be passed to us.
	// If the data cannnot be extracted then the extracting process will Leave before
	// passing us the data.

	TPtr16 unicodeConv(0,0);
	//TPtr version of ConvertToUnicodeL is used as it is more efficient 
	User::LeaveIfError(iCharSetConverter->ConvertToUnicodeL(
							aSrcCharSetUid, 
							aSrc, 
							unicodeConv));

	// Note: No need to assert bytes unconverted == 0 as this is handled by the
	// character set converter in the form of an error being returned.

	// convert unicode null to encoding

	//HBufC version of ConvertFromUnicodeL is used as the HBufC needs to be passed back to the calling function
	User::LeaveIfError(iCharSetConverter->ConvertFromUnicodeL(
							unicodeConv, 
							aDestCharSetUid, 
							aDest));
	}



TBool CWbxmlParser::DetermineCharacterNull(const TPtr8& aStringPtr, 
										   TUint aCharLength, 
										   TInt32 aOffset)
/**
Determine if the next character is a NULL, corresponding to the character set, or not. 

@param				aStringPtr The data to search
@param				aCharLength The character length in bytes
@param				aOffset The character offset within the string to check

@return				ETrue if this NULL has been located, EFalse otherwise.
*/
	{
	TBool found = EFalse;
	
	// Loop around the bytes for this character and determine if it is a NULL
	for (TUint32 j = 0; j < aCharLength; ++j)
		{
		if (aStringPtr[aOffset+j] == (*iEncodingNull)[j])
			{
			found = ETrue;
			}
		else
			{
			found = EFalse;
			break;
			}
		}
		
	return found;		
	}




void CWbxmlParser::ExtractStringUtf8L (TInt32 aStringTblIndex, 
									   CBufFlat& aUtf8String, 
									   TBool aAppendString)
/**
This method extracts a null terminated string from the Wbxml string table,
where the string will be in the character encoding previously determined.
The string is then converted into utf-8 before being placed into the passed 
buffer.

Strings in the Wbxml document (inline strings) are generally null terminated.

Even though the standard states:

"String termination is dependent on the character document encoding and 
 should not be presumed to include NULL termination."

In practise this is hard to police so it is assumed here that all strings
are null terminated. This is because most transmitted character encodings
are UNICODE that all have NULL termination characters.

Though it isn't stated in the wbxml standard, sometimes if there 
exists only one string in the string table it isn't null terminated
due to space constraints - every transmitted byte counts.
The same goes for the last string in the string table.
The length of the string table represents the end of that string.

@param				aStringTblIndex Index into the string table representing 
					the start of string.
@param				aUtf8String Buffer to place the obtained string into.
@param				aAppendString Indicates whether to flush the buffer before 
					appending.

*/
	{
	// If not string table or the index exceeds its length, bad doc!
	if (!iStringTable || iStringTable->Size() < aStringTblIndex)
		{
		User::Leave(KErrXmlDocumentCorrupt);
		}
	

	// If charlen for NULL >1 then this is generally a fixed width character representation
	TUint charLength = iEncodingNull->Length();
	TInt32 pos = aStringTblIndex;
	TInt zeroOffset = 0 - charLength;
	TBool found = EFalse;

	// The stringtable should either be of multibyte or fixed byte width.
	// If multibyte, charLength should equal 1 (ASCII 0 width), else the width of the fixed 
	// width character. There should be no modulus.
	if((iStringTable->Length() % charLength) != 0)
		{
		User::Leave(KErrTotalLossOfPrecision);
		}

	while (!found && pos < iStringTable->Length())
		{
		zeroOffset += charLength;

		found = DetermineCharacterNull(iStringTable->Des(), charLength, pos);

		pos += charLength;
		}

	TPtrC8 string;
	if (!found && pos == iStringTable->Length())
		{
		string.Set(iStringTable->Mid(aStringTblIndex));
		}
	else
		{
		// get the substring from the index to the start of the null
		string.Set(iStringTable->Mid(aStringTblIndex, zeroOffset));
		}

	HBufC8* buf = NULL;

	ConvertToEncodingL(iCharSetUid, string, KCharacterSetIdentifierUtf8, buf);
	CleanupStack::PushL(buf);

	// Leave in an expected state
	iCharSetConverter->PrepareToConvertToOrFromL(iCharSetUid);



	if (aAppendString)
		{
		aUtf8String.InsertL(aUtf8String.Size(), buf->Des());
		}
	else
		{
		// We don't amend the size as if we have one long
		// string there may be others. So dont use Reset.
		aUtf8String.Delete(0, aUtf8String.Size());
		aUtf8String.InsertL(0, buf->Des());
		}

	CleanupStack::PopAndDestroy(buf);
	}



void CWbxmlParser::ExtractStringUtf8L(CBufFlat& aUtf8String, 
									  TBool aAppendString)
/**
This method extracts a null terminated string from the next position within
the Wbxml document to be parsed, where the string will be in the character
encoding previously determined.

The string is then converted into utf-8 before being placed into the passed 
buffer.

Strings in the Wbxml document (inline strings) are generally null terminated.

Even though the standard states:

"String termination is dependent on the character document encoding and 
 should not be presumed to include NULL termination."

In practise this is hard to police so it is assumed here that all strings
are null terminated. This is because most transmitted character encodings
are UNICODE that all have NULL termination characters.


@param				aUtf8String Buffer to place the obtained string into.
@param				aAppendString Indicates whether to flush the buffer before 
					appending.

@leave				KErrXmlDocumentCorrupt if the string isn't found

*/
	{
	// If charlen for NULL >1 then this is generally a fixed width character representation
	TUint charLength = iEncodingNull->Length();
	TBool found = EFalse;
	TInt zeroOffset = 0 - charLength;

	CBufFlat* string = CBufFlat::NewL(charLength*256); // guess we have 256 bytes for our string
	CleanupStack::PushL(string);

	TPtr8 stringPtr (string->Ptr(0));

	while (!found)
		{
		zeroOffset += charLength;

		string->InsertL(zeroOffset, iDataSource->GetBytesL(charLength));
		stringPtr.Set(string->Ptr(0)); // to capture new size

		found = DetermineCharacterNull(stringPtr, charLength, zeroOffset);
		}

/*
	if (!found)
		{
		User::Leave(KErrXmlDocumentCorrupt);
		}
*/

	// Remove the zero bytes and minimise
	string->ResizeL(zeroOffset);
	stringPtr.Set(string->Ptr(0)); // to capture new size

	HBufC8* buf = NULL;

	ConvertToEncodingL(iCharSetUid, stringPtr, KCharacterSetIdentifierUtf8, buf);
	CleanupStack::PushL(buf);

	// Leave in an expected state
	iCharSetConverter->PrepareToConvertToOrFromL(iCharSetUid);

	if (aAppendString)
		{
		aUtf8String.InsertL(aUtf8String.Size(), buf->Des());
		}
	else
		{
		// We don't amend the size as if we have one long
		// string there may be others. So dont use Reset.
		aUtf8String.Delete(0, aUtf8String.Size());
		aUtf8String.InsertL(0, buf->Des());
		}

	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(string);
	}



void CWbxmlParser::InlineL()
/**
This method Obtains an inline NULL terminated string from the Wbxml document.

An inline string is a string that lies at the very next position to be
parsed in the Wbxml document.

BNF: inline = STR_I termstr

*/
	{
	// APPEND ONTO END, then when read END token send as value or content
	ExtractStringUtf8L(*iUtf8String, ETrue);
	}



void CWbxmlParser::TableRefL()
/**
This method Obtains an inline NULL terminated string within the string table 
of the wbxml document.

A table ref string is a string that lies at an offset position within the 
Wbxml document string table. The offset lies at the next position within
the Wbxml document to be parsed.

It is assumed to be NULL terminated in most cases, except when there is
only one string in the string table or it is the last string.

BNF: tableref = STR_T index

*/
	{
	TInt32 index = GetMultiByteInt32L();

	// Get the string from the string table

	// APPEND ONTO END, then when read END token send as value or content
	ExtractStringUtf8L(index, *iUtf8String, ETrue);
	}



void CWbxmlParser::ExtensionL(TUint8 aByte)
/**
This method provides the single-byte global extension.

Extension tokens are available for document-specific use. The semantics
of the tokens are defined only within the context of a particular document type,
but the format is well defined across all documents. There are three
classes of global extension tokens: single-byte extension tokens, inline string
extension tokens and inline integer extension tokens.

BNF: extension = [switchPage] (( EXT_I termStr) | ( EXT_T index) | EXT)

@param				aByte The extension token.

*/
	{
	OnExtensionL(iBlank, aByte, KErrNone);
	
	// Clear the string buffer.
	// There should be nothing in here, but just in case.
	//
	// We don't amend the size as if we have one long
	// string there may be others. So dont use Reset.
	iUtf8String->Delete(0, iUtf8String->Size());
	}



void CWbxmlParser::ExtensionIL(TUint8 aByte)
/**
This method provides the inline string global extension.

Extension tokens are available for document-specific use. The semantics
of the tokens are defined only within the context of a particular document type,
but the format is well defined across all documents. There are three
classes of global extension tokens: single-byte extension tokens, inline string
extension tokens and inline integer extension tokens.

BNF: extension = [switchPage] (( EXT_I termStr) | ( EXT_T index) | EXT)

@param				aByte The extension token.

*/
	{
	// For now, just append the string to the string buffer.
	// Later, need to find out how to intepret the ext as we have no
	// current examples of how this is used.

	InlineL();
	RString ext;
	CleanupClosePushL(ext);
	ext = StringPool().OpenStringL(iUtf8String->Ptr(0));
	OnExtensionL(ext, aByte, KErrNone);
	ext.Close();
	CleanupStack::Pop(&ext);
	// We don't amend the size as if we have one long
	// string there may be others. So dont use Reset.
	iUtf8String->Delete(0, iUtf8String->Size());
	}



void CWbxmlParser::ExtensionTL(TUint8 aByte)
/**
This method provides the inline integer global extension.
Inline integer has been understood to be a table ref string.

Extension tokens are available for document-specific use. The semantics
of the tokens are defined only within the context of a particular document type,
but the format is well defined across all documents. There are three
classes of global extension tokens: single-byte extension tokens, inline string
extension tokens and inline integer extension tokens.

BNF: extension = [switchPage] (( EXT_I termStr) | ( EXT_T index) | EXT)

@param				aByte The extension token.

*/
	{
	// For now, just append the string to the string buffer.
	// Later, need to find out how to intepret the ext as we have no
	// current examples of how this is used.

	TableRefL();
	RString ext;
	CleanupClosePushL(ext);
	ext = StringPool().OpenStringL(iUtf8String->Ptr(0));
	OnExtensionL(ext, aByte, KErrNone);
	ext.Close();
	CleanupStack::Pop(&ext);
	// We don't amend the size as if we have one long
	// string there may be others. So dont use Reset.
	iUtf8String->Delete(0, iUtf8String->Size());
	}



void CWbxmlParser::EntityL()
/**
This method provides for a character entity.

The ucs-4 character representing the entity is retrieved and converted into
utf-8 if the parse mode ERawContent is NOT set.

The character entity token (ENTITY) encodes a numeric character entity. 
This has the same semantics as an XML numeric character entity (e.g., &#32;). 
The mb_u_int32 refers to a character in the UCS-4 character encoding. All
entities in the source XML document must be represented using either a 
string token (e.g., STR_I) or the ENTITY token.

BNF: entity = ENTITY entcode
entcode = mb_u_int32

@see TParseMode

*/
	{
	// UCS-4 character code
	TUint32 code = GetMultiByteInt32L();
	HBufC8* ucs4 = NULL;
	const TInt numCharacters = 1;

	if(iParseMode & ERawContent)
		{
		User::LeaveIfError(
			iCharSetConverter->ConvertUcs4CharactersToEncodingL(&code, 
																numCharacters, 
																iCharSetUid, 
																ucs4));
		}
	else
		{
		User::LeaveIfError(
			iCharSetConverter->ConvertUcs4CharactersToEncodingL(&code, 
																numCharacters, 
																KCharacterSetIdentifierUtf8, 
																ucs4));
		}
	CleanupStack::PushL(ucs4);


	iUtf8String->InsertL(iUtf8String->Size(), *ucs4);

	// Make sure charconv is in correct state.
	iCharSetConverter->PrepareToConvertToOrFromL(iCharSetUid);

	CleanupStack::PopAndDestroy(ucs4);
	}



void CWbxmlParser::OpaqueL()
/**
This method provides for opaque data.

The opaque data is retrieved and converted into utf-8 if the parse
mode ERawContent is NOT set.

The opaque token (OPAQUE) encodes application-specific data. A length field 
and zero or more bytes of data follow the token. The length field encodes the 
number of bytes of data, excluding the OPAQUE token and the length field.

BNF: opaque = OPAQUE length *byte
length = mb_u_int32

@see				TParseMode

*/
	{
	if ((iOpaqueLength = GetMultiByteInt32L()) != 0)
		{
		TPtrC8 bytes (iDataSource->GetBytesL(iOpaqueLength));

		if (!(iParseMode & ERawContent))
			{
			HBufC8* buf = NULL;

			ConvertToEncodingL(iCharSetUid, bytes, KCharacterSetIdentifierUtf8, buf);
			CleanupStack::PushL(buf);

			iUtf8String->InsertL(iUtf8String->Size(), *buf);

			CleanupStack::PopAndDestroy(buf);
			}
		else
			{
			iUtf8String->InsertL(iUtf8String->Size(), bytes);
			}
		}

	// Leave in an expected state
	iCharSetConverter->PrepareToConvertToOrFromL(iCharSetUid);
	}



TInt32 CWbxmlParser::GetMultiByteInt32L()
/**
This method extracts from the next position within the Wbxml dccument 
a multibyte integer and reconstitutes it into a TInt32.

@return				The reconstituted multibyte integer.

@leave				KErrXmlDocumentCorrupt If the bytes obtained exceed their limit.

*/
	{
	TUint32 result=0;
	TUint8 byte=0;
	TUint8 byteCount=0;
	do
		{
		result <<=7;			// position for next insertion
		byte = iDataSource->GetByteL();
		result|=byte & ~0x80;	// loose continuation flag
		++byteCount;
		}
	while ( (byte & 0x80) && (byteCount <= 4) );	// until we read the last byte
	
	//Check for negative result.
	if ( (byte & 0x80) || ((TInt32)result < 0) ) 		
		{
		User::Leave(KErrXmlDocumentCorrupt);
		}

	return result;
	}



void CWbxmlParser::NegIndexL(TInt aIndex)
/**
This method tests the passed index for negativity.
This is done so as to avoid panics in the code due to indexing into arrays
with negative values.

@param				aIndex An array index

@leave				KErrXmlBadIndex If the index is negative

*/
	{
	if (aIndex < 0)
		{
		User::Leave(KErrXmlBadIndex);
		}
	}



void CWbxmlParser::OnStartDocumentL()
/**
This method prepares the data before passing it onto the next object in line
via the function of the same name.

This is purely a helper function.

@see				MContentHandler::OnStartDocumentL 

*/
	{
	RDocumentParameters docParam;
	RString charsetname;
	CleanupClosePushL(docParam);
	CleanupClosePushL(charsetname);
	
	HBufC8* name = NULL;
	
	iCharSetConverter->ConvertCharacterSetIdentifierToStandardNameL(iCharSetUid, name);
	CleanupStack::PushL(name);
	
	charsetname = StringPool().OpenStringL(*name);

	docParam.Open (charsetname);
	iContentHandler->OnStartDocumentL(docParam, KErrNone);
	
	docParam.Close();
	CleanupStack::PopAndDestroy(name);
	CleanupStack::Pop(&charsetname);
	CleanupStack::Pop(&docParam);
	}



void CWbxmlParser::OnStartElementL()
/**
This method prepares the data before passing it onto the next object in line
via the function of the same name.

This is purely a helper function.

*/
	{
	RTagInfo tagInfo;
	RString& localName = 
		(*iElementStack)[iElementStack->Count()-1];
	
	// We don't copy iTagCurrentUri as closing is harmless,
	// we don't copy iBlank as we never opened it, using its default properties.
	tagInfo.Open(iTagCurrentUri, iBlank, localName);
	
	iContentHandler->OnStartElementL(
		tagInfo, iAttributes, KErrNone);

	for (TInt i=0, c=iAttributes.Count(); i<c; ++i)
		{
		iAttributes[i].Close();
		}
	iAttributes.Reset();
	}
	

void CWbxmlParser::OnEndElementL()
/**
This method prepares the data before passing it onto the next object in line
via the function of the same name.

This is purely a helper function.

*/
	{
	//This case happen when document contain extra tag "END" i.e. (01 end of an element)
	if(iElementStack->Count() < 1)
		User::Leave(KErrXmlDocumentCorrupt);

	RTagInfo tagInfo;
	RString& localName = 
		(*iElementStack)[iElementStack->Count()-1];
	
	// We don't copy iTagCurrentUri as closing is harmless,
	// we don't copy iBlank as we never opened it, using its default properties.
	tagInfo.Open(iTagCurrentUri, iBlank, localName);
	
	iContentHandler->OnEndElementL(tagInfo, KErrNone);

	NegIndexL(iElementStack->Count()-1);
	(*iElementStack)[iElementStack->Count()-1].Close();
	iElementStack->Remove(iElementStack->Count()-1); 
	}



void CWbxmlParser::OnProcessingInstructionL()
/**
This method prepares the data before passing it onto the next object in line
via the function of the same name.

This is purely a helper function.

*/
	{
	RAttribute& attr = iAttributes[iAttributes.Count()-1];
	
	iContentHandler->OnProcessingInstructionL(
		attr.Attribute().LocalName().DesC(),
		attr.Value().DesC(),
		KErrNone);
		
	NegIndexL(iAttributes.Count()-1);
	attr.Close();
	iAttributes.Remove(iAttributes.Count()-1); 
	}


void CWbxmlParser::OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode)
/**
This method prepares the data before passing it onto the next object in line
via the function of the same name.

This is purely a helper function.

@param				aData The parsed data.
@param				aToken The extension token value parsed.
@param				aErrorCode The error code relating to the success of
					the parsed data.

@leave				KErrXmlUnsupportedExtInterface If the interface is not 
					supported by the next object.

*/
	{
	MWbxmlExtensionHandler* ptr = 
		static_cast<MWbxmlExtensionHandler*>
			(iContentHandler->GetExtendedInterface(MWbxmlExtensionHandler::EExtInterfaceUid));

	if (!ptr)
		{
		User::Leave(KErrXmlUnsupportedExtInterface);
		}
		
	ptr->OnExtensionL(aData, aToken, aErrorCode);	
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x101FB6F3,CWbxmlParser::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

