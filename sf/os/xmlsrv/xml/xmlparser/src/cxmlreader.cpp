/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "cxmlreader.h"


/* Dictionar Public ID's */
#define WML_1_1_PUBLIC_ID         4      
#define WML_1_2_PUBLIC_ID         9     
#define WML_1_3_PUBLIC_ID        10    
#define XHTML_1_0_PUBLIC_ID      55    
#define SYNCML_1_0_PUBLIC_ID  0xFD1
#define WAP_PROV_1_0_PUBLIC_ID   11
#define SI_1_0_PUBLIC_ID          5
#define SL_1_0_PUBLIC_ID          6
#define PUSH_PROV_1_0_PUBLIC_ID   1    
extern "C" 
{ 
	extern NW_WBXML_Dictionary_t NW_Wml_1_0_WBXMLDictionary;
	extern NW_WBXML_Dictionary_t NW_Wml_1_1_WBXMLDictionary;
	extern NW_WBXML_Dictionary_t NW_Wml_1_2_WBXMLDictionary;
	extern NW_WBXML_Dictionary_t NW_Wml_1_3_WBXMLDictionary;
	extern NW_WBXML_Dictionary_t NW_XHTML_WBXMLDictionary;
	extern NW_WBXML_Dictionary_t NW_SyncML_1_0_WBXMLDictionary;
	extern NW_WBXML_Dictionary_t NW_SL_WBXMLDictionary;
//	extern NW_WBXML_Dictionary_t NW_wap_prov_doc_WBXMLDictionary;
//	extern NW_WBXML_Dictionary_t SI_dictionary;
//	extern NW_WBXML_Dictionary_t SL_dictionary;
//	extern NW_WBXML_Dictionary_t push_nokprov_dictionary;
}
#define DICTIONARY_COUNT 7


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// BeginElementCB
// BeginElement callback, stores elemntname in elemntlist
// -----------------------------------------------------------------------------
//
NW_Status_t BeginElementCB(NW_XML_Reader_t* aReader, 
						 const NW_XML_Reader_Interval_t* aName, 
						 void* aParser)
{
	CXMLObserver *wObserver = (CXMLObserver*)aParser;
	TPtrC wElementName( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
	TRAPD(error, wObserver->SetElementL(wElementName));
	if(error)
	{
		(wObserver->GetAttributes())->Reset();
		return NW_STAT_FAILURE;
	}
	// New element, reset attributes list
	(wObserver->GetAttributes())->Reset();
	return NW_STAT_SUCCESS;
}

// -----------------------------------------------------------------------------
// EndElementCB. calls MXMLContentHandler::EndElement with elemntname, 
// removes closed element from elemntlist. 
// -----------------------------------------------------------------------------
//
NW_Status_t EndElementCB(NW_XML_Reader_t* aReader, 
					     const NW_XML_Reader_Interval_t* aName, 
					     NW_Uint32 /*aEmpFlag*/,
					     void* aParser)
{
	_LIT(KEmpty,"");
	TPtrC wEmpty(KEmpty);
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	wObserver->RemoveElement();
	MXMLContentHandler* aContentHandler = wObserver->GetContentHandler();
	TPtrC aElementName( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
	TInt result = aContentHandler->EndElement(wEmpty, wEmpty, aElementName);
	aContentHandler->ReaderIndex( aReader->charIndex );
	return  result ? NW_STAT_SUCCESS:NW_STAT_FAILURE;		
}

// -----------------------------------------------------------------------------
// AttributesStartCB 
// -----------------------------------------------------------------------------
//
NW_Status_t AttributeStartCB (NW_XML_Reader_t*,
                               const NW_XML_Reader_Interval_t* /*pI_name*/,
                                void*)
{
	return NW_STAT_SUCCESS;	
}

// -----------------------------------------------------------------------------
// AttreibuteValueCB Adds new attribute to attributes list.
// Adds URI to URI list if namespaces is used
// -----------------------------------------------------------------------------
//
NW_Status_t AttributeValueCB(NW_XML_Reader_t* aReader, 
						   const NW_XML_Reader_Interval_t* aName,
                           const NW_XML_Reader_Interval_t* aValue,
                           void* aParser)
{
	_LIT(KNamespace,"xmlns:");
	TPtrC wNamespace(KNamespace);
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	CXMLAttributes* wAttr = wObserver->GetAttributes();
	
	// Append new attribute to attributes list
	TPtrC wAttributeName( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
	TPtrC wAttributeValue( (TUint16*)aReader->pBuf + aValue->charStart, aValue->charStop - aValue->charStart );
	TRAPD(error,wAttr->AppendL(wAttributeName, wAttributeValue));
	if(error)
	{
		return NW_STAT_FAILURE;
	}
	// namespace declaration, store URI and extension
	if(wAttributeName.Left(6).Match(wNamespace)!=KErrNotFound)
	{
		TPtrC wExtension = wAttributeName.Right(wAttributeName.Length()-6);
		TRAP(error, wObserver->SetURIL(wAttributeValue, wExtension));
		if(error)
		{
			return NW_STAT_FAILURE;
		}
	}
	return NW_STAT_SUCCESS;		
}

// -----------------------------------------------------------------------------
// AttributesEndCB Calls MXMLContentHandler::StartElement with element name and
// attributes list.
// -----------------------------------------------------------------------------
//
NW_Status_t AttributesEndCB(NW_XML_Reader_t* /*aReader*/, 
						    NW_Uint32 /*aLength*/, 
						    void* aParser)
{
	_LIT(KLocalName,"");
	TPtrC wLocalName(KLocalName);
	_LIT(KURI,"");
	TPtrC wURI(KURI);
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	MXMLContentHandler* wContentHandler = wObserver->GetContentHandler();
				
	TPtrC wElementName = wObserver->GetElement();
	CXMLAttributes* wAttributes = wObserver->GetAttributes();
		
	// if using namespace, get URI and localname
	TInt wIndex = wElementName.Locate(':');
	if(wIndex > 0)
	{
		wLocalName.Set(wElementName.Right(wElementName.Length()-wIndex-1));
		TPtrC temp = wElementName.Left(wIndex);
		wURI.Set(wObserver->GetURI(temp));
	}
	// Call StartElement, now that we have all the attributes
	return wContentHandler->StartElement(wURI, wLocalName, wElementName, wAttributes) ? NW_STAT_SUCCESS:NW_STAT_FAILURE;	
}

// -----------------------------------------------------------------------------
// CDataCB Reports actual CData as MXMLContentHandler::Characters (between 
// MXMLContentHandler::StartCData and MXMLContentHandler::EndCData
// -----------------------------------------------------------------------------
//
NW_Status_t CdataCB (NW_XML_Reader_t* aReader,
                     const NW_XML_Reader_Interval_t* aName,
                     void* aParser)
{	
    CXMLObserver* wObserver = (CXMLObserver*)aParser;
	MXMLContentHandler* aContentHandler = wObserver->GetContentHandler();
	TPtrC wChar( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
	
	aContentHandler->StartCDATA();
	aContentHandler->Charecters( wChar, aName->charStart, (aName->charStop - aName->charStart) ); 
	return aContentHandler->EndCDATA()? NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// ContentCB Content reported as MXMLContentHandler::Characters
// -----------------------------------------------------------------------------
//
NW_Status_t ContentCB (NW_XML_Reader_t* aReader,
                             const NW_XML_Reader_Interval_t* aValue,
                             void* aParser)
{

	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	TPtrC wChar( (TUint16*)aReader->pBuf + aValue->charStart, aValue->charStop - aValue->charStart );
	
	MXMLContentHandler* aContentHandler = wObserver->GetContentHandler();
	
	return (aContentHandler->Charecters(wChar, aValue->charStart, (aValue->charStop - aValue->charStart)))?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}


// -----------------------------------------------------------------------------
// AttrEntityVarValCB Adds new attribute to attributes list.
// Adds URI to URI list if namespaces is used
// -----------------------------------------------------------------------------
//
NW_Status_t AttrEntityVarValCB (NW_XML_Reader_t* aReader,
									const NW_XML_Reader_Interval_t* aName,
                                    NW_Uint8* aValue,
                                    NW_Uint32 aValueByteLength,
                                    void* aParser)
  {
        _LIT(KNamespace, "xmlns:");
        TPtrC wNamespace(KNamespace);
        CXMLObserver* wObserver = (CXMLObserver*)aParser;
        CXMLAttributes* wAttr = wObserver->GetAttributes();

        // Append new attribute to attributes list
        TPtrC wAttributeName( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
        TPtrC wAttributeValue( (TUint16*)aValue, aValueByteLength/2 );

        TRAPD(error,wAttr->AppendL(wAttributeName, wAttributeValue));
        if(error)
        {
                return NW_STAT_FAILURE;
        }

        // namespace declaration, store URI and extension
        if(wAttributeName.Left(6).Match(wNamespace)!=KErrNotFound)
        {
                TPtrC wExtension = wAttributeName.Right(wAttributeName.Length()-6);
                TRAP(error, wObserver->SetURIL(wAttributeValue, wExtension));
                if(error)
                {
                        return NW_STAT_FAILURE;
                }
        }
	return NW_STAT_SUCCESS;
  }


// -----------------------------------------------------------------------------
// EntityCB Entity reported as MXMLContentHandler::StartEntity
// -----------------------------------------------------------------------------
//
NW_Status_t EntityCB (NW_XML_Reader_t* aReader,
                             NW_Uint32 aNumEntity,
                             void* aParser, 
                             CXML_Uint8* aIntEntityValStr) 
{

	CXMLObserver *wObserver = (CXMLObserver*)aParser;

	NW_String_t *tempString = NW_String_new();
	// handle OOM
	if(tempString == NULL)
	{
		return NW_STAT_FAILURE;
	}

    if(aIntEntityValStr == NULL) 
    { 
     //It is character/predefined entities 
	NW_String_entityToString(aNumEntity, tempString, aReader->encoding);
    } 
	
    if(aIntEntityValStr != NULL) 
	{
     //It is the internal entity 
      tempString->storage = aIntEntityValStr; 
	}

    TPtrC wTmpEntityPtr((TUint16*)tempString->storage); 
        TInt result(wObserver->GetContentHandler()->StartEntity(wTmpEntityPtr)); 
	
	if (result == 0) 
	{
		NW_String_delete(tempString);
		return NW_STAT_FAILURE;
	}
        result = wObserver->GetContentHandler()->EndEntity(wTmpEntityPtr); 
		NW_String_delete(tempString);

	return (result?NW_STAT_SUCCESS:NW_STAT_FAILURE);
	
}

// -----------------------------------------------------------------------------
// CommentCB 
// -----------------------------------------------------------------------------
//
NW_Status_t CommentCB (NW_XML_Reader_t* aReader,
                       const NW_XML_Reader_Interval_t* aName,
                       void* aParser )
{
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	TPtrC wComment( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
	return (wObserver->GetContentHandler())->Comment(wComment)?NW_STAT_SUCCESS:NW_STAT_FAILURE;		
}

// -----------------------------------------------------------------------------
// StartDocumentCB 
// -----------------------------------------------------------------------------
//
NW_Status_t StartDocumentCB (NW_XML_Reader_t* /*aReader*/,
                             void* aParser )

{
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	return (wObserver->GetContentHandler())->StartDocument()?NW_STAT_SUCCESS:NW_STAT_FAILURE;		

}

// -----------------------------------------------------------------------------
// EndDocumentCB 
// -----------------------------------------------------------------------------
//
NW_Status_t EndDocumentCB (NW_XML_Reader_t* /*aReader*/,
                           void* aParser) 
{
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	return (wObserver->GetContentHandler())->EndDocument()?NW_STAT_SUCCESS:NW_STAT_FAILURE;		
}

// -----------------------------------------------------------------------------
// PiCB Report Processing Instruction with MXMLProcessingInstruction(Target, Data)
// -----------------------------------------------------------------------------
//
NW_Status_t PiCB (NW_XML_Reader_t* aReader,
                  NW_PiFormTypeTag_t aTag,
                  const NW_XML_Reader_Interval_t* aName,
                  const NW_XML_Reader_Interval_t* aVersion,
                  const NW_XML_Reader_Interval_t* /*aEncoding*/,
                  const NW_XML_Reader_Interval_t* /*aStandalone*/,
                  const NW_XML_Reader_Interval_t* aContent,
                  void* aParser)
{
	CXMLObserver* wObserver = (CXMLObserver*)aParser;
	TPtrC aTarget( (TUint16*)aReader->pBuf + aName->charStart, aName->charStop - aName->charStart );
	if(aTag==XMLDECL)
	{
		TPtrC aData( (TUint16*)aReader->pBuf + aVersion->charStart, aVersion->charStop - aVersion->charStart );
		return (wObserver->GetContentHandler())->ProcessingInstructions(aTarget, aData)?NW_STAT_SUCCESS:NW_STAT_FAILURE;	
	}
	else
	{
		TPtrC aData( (TUint16*)aReader->pBuf + aContent->charStart, aContent->charStop - aContent->charStart );
		return (wObserver->GetContentHandler())->ProcessingInstructions(aTarget, aData)?NW_STAT_SUCCESS:NW_STAT_FAILURE;
	}
}


// Binary Call backs

// -----------------------------------------------------------------------------
// Binary_StartDocument_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_StartDocument_CB (NW_WBXML_Parser_t* /*aParser*/, 
                                   NW_WBXML_Document_t* /*aDocument*/, 
                                   void * aContext)
{
	CXMLObserver* wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->StartDocument()?NW_STAT_SUCCESS:NW_STAT_FAILURE;		
}

// -----------------------------------------------------------------------------
// Binary_EndDocument_CB 
// -----------------------------------------------------------------------------
// 
NW_Status_t Binary_EndDocument_CB(NW_WBXML_Parser_t* /*aParser*/, 
                                 void * aContext)
{
	CXMLObserver* wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->EndDocument()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Pi_CB ?description.
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Pi_CB (NW_WBXML_Parser_t* /*aParser*/, 
                        void * aContext)
{
	CXMLObserver* wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->ProcessingInstruction()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Pi_End_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Pi_End_CB (NW_WBXML_Parser_t* /*aParser*/, 
                            void* aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->ProcessingInstructionEnd()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Tag_Start_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Tag_Start_CB(NW_WBXML_Parser_t* /*aParser*/, 
                               void *aContext)
{
	
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->TagStart()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Tag_End_CB ?description.
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Tag_End_CB(NW_WBXML_Parser_t* /*aParser*/, 
                             void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;	
	return (wObserver->GetBinaryContentHandler())->TagEnd()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Attr_Start_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Attr_Start_CB(NW_WBXML_Parser_t* /*aParser*/, 
                                void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->AttributeStart()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Attr_Val_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Attr_Val_CB(NW_WBXML_Parser_t* /*aParser*/, 
                              void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->AttributeValue()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Content_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Content_CB(NW_WBXML_Parser_t* /*aParser*/, 
                             void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->Content()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_CodePage_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_CodePage_CB(NW_WBXML_Parser_t* /*aParser*/, 
                              void* aContext)
{
	// Code page switch in WBXML doc
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->CodePageSwitch()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Extension_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Extension_CB(NW_WBXML_Parser_t* /*aParser*/, 
                               void* aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->Extension()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Token_CB 
// -----------------------------------------------------------------------------
//  
NW_Status_t Binary_Token_CB(NW_WBXML_Parser_t* /*aParser*/, 
                           NW_Uint8 aToken, 
                           void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	NW_String_UCS2Buff_t *wTemp = NW_WBXML_Dictionary_getTagByFqToken (aToken);
	TPtrC wString((TUint16*)wTemp);
	return (wObserver->GetBinaryContentHandler())->Token(aToken, wString)?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_FQToken_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_FQToken_CB(NW_WBXML_Parser_t* /*aParser*/, 
                            NW_Uint32 aFQToken, 
                            void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	NW_Byte aToken = (NW_Byte) (aFQToken & NW_WBXML_MASK_TOKEN);
	if( (aToken & NW_WBXML_MASK_TAG_ID) != NW_WBXML_LITERAL)
	{
		NW_String_UCS2Buff_t *wTemp = NW_WBXML_Dictionary_getTagByFqToken(aFQToken); 
		if(wTemp)
		{
			TPtrC wString((TUint16*)wTemp);
			return (wObserver->GetBinaryContentHandler())->Token(aToken, wString)?NW_STAT_SUCCESS:NW_STAT_FAILURE;
		}
		return NW_STAT_FAILURE;
	}
	return NW_STAT_SUCCESS;
}

// -----------------------------------------------------------------------------
// Binary_InlineString_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_InlineString_CB(NW_WBXML_Parser_t *aParser, 
                                  NW_Uint32 /*aIndex*/, 
                                  void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	NW_String_t *tempString = NW_String_new();
	// handle OOM
	if(tempString == NULL)
	{
		return NW_STAT_FAILURE;
	}
	NW_WBXML_Document_t *doc = aParser->doc;

	NW_WBXML_Parser_getStringInline(aParser, doc, tempString);

	/**!
	* This was changed because of MNUI-68TAZP - German umlauts were displayed wrong.
	* This was caused by NW_String_byteToUCS2Char() function - this is not recognize utf-8 encoding and treats 
	* all characters as a 1byte ASCII. In case of character encoded on more than 1 byte
	* we get two or more separate symbols instead of one compound of these bytes.
	* The CnvUtfConverter::ConvertToUnicodeFromUtf8() recognizes utf-8 properly.

	NW_Ucs2 *ucs2;
    NW_String_byteToUCS2Char(tempString->storage, ((*tempString).length) & ~0x80000000, &ucs2); //VC, AM
	// handle OOM
	if(ucs2 == NULL)
	{
		NW_String_delete(tempString);
		return NW_STAT_FAILURE;
	}
	TPtrC wString((TUint16*)ucs2);
	NW_String_delete(tempString);
	*/

	TPtrC8 string8(tempString->storage);
	HBufC *string16=NULL;
	TRAPD(err, string16 = HBufC::NewL(string8.Length()));
	if(err)
	{
		NW_String_delete(tempString);
		return NW_STAT_FAILURE;
	}
	TPtr wString(string16->Des());

#if defined(_UNICODE)
	err = CnvUtfConverter::ConvertToUnicodeFromUtf8(wString,string8);
	if(err != KErrNone)
	{
		NW_String_delete(tempString);
		delete string16;
		return NW_STAT_FAILURE;
	}
#else
	wString.Copy(string8);
#endif
	
	TInt result((wObserver->GetBinaryContentHandler())->InlineString(wString));

	NW_String_delete(tempString);
	delete string16;

	return (result?NW_STAT_SUCCESS:NW_STAT_FAILURE);
}

// -----------------------------------------------------------------------------
// Binary_TableString_CB
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_TableString_CB(NW_WBXML_Parser_t *aParser, 
                                 NW_Uint32 aIndex, 
                                 void *aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	NW_String_t *tempString = NW_String_new();
	// handle OOM
	if(tempString == NULL)
	{
		return NW_STAT_FAILURE;
	}
	NW_WBXML_Document_t *doc = aParser->doc;
	NW_WBXML_Document_getTableString(doc, aIndex, tempString);
	/**!
	* This was changed because of MNUI-68TAZP - German umlauts were displayed wrong.
	* This was caused by NW_String_byteToUCS2Char() function - this is not recognize utf-8 encoding and treats 
	* all characters as a 1byte ASCII. In case of character encoded on more than 1 byte
	* we get two or more separate symbols instead of one compound of these bytes.
	* The CnvUtfConverter::ConvertToUnicodeFromUtf8() recognizes utf-8 properly.

	NW_Ucs2 *ucs2;
    NW_String_byteToUCS2Char(tempString->storage, ((*tempString).length) & ~0x80000000, &ucs2); //VC, AM
	// handle OOM
	if(ucs2 == NULL)
	{
		NW_String_delete(tempString);
		return NW_STAT_FAILURE;
	}
	TPtrC wString((TUint16*)ucs2);
	NW_String_delete(tempString);
	*/

	TPtrC8 string8(tempString->storage);
	HBufC *string16=NULL;
	TRAPD(err, string16 = HBufC::NewL(string8.Length()));
	if(err)
	{
		NW_String_delete(tempString);
		return NW_STAT_FAILURE;
	}

	TPtr wString(string16->Des());

#if defined(_UNICODE)
	err = CnvUtfConverter::ConvertToUnicodeFromUtf8(wString,string8);
	if(err != KErrNone)
	{
		NW_String_delete(tempString);
		delete string16;
		return NW_STAT_FAILURE;
	}
#else
	wString.Copy(string8);
#endif
	
	TInt result((wObserver->GetBinaryContentHandler())->TableString(wString));

	NW_String_delete(tempString);
	delete string16;

	return (result?NW_STAT_SUCCESS:NW_STAT_FAILURE);
}

// -----------------------------------------------------------------------------
// Binary_Binary_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Binary_CB(NW_WBXML_Parser_t* /*aParser*/, 
                            NW_Uint32 aIndex, 
                            void* aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->Binary(aIndex)?NW_STAT_SUCCESS:NW_STAT_FAILURE; 
}

// -----------------------------------------------------------------------------
// Binary_Opaque_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Opaque_CB(NW_WBXML_Parser_t *aParser, 
                            NW_Uint32 aLength, 
                            void * aContext)
{
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	TPtrC wString((TUint16*)aParser->p, aLength);
	return (wObserver->GetBinaryContentHandler())->Opaque(aLength, wString)?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}

// -----------------------------------------------------------------------------
// Binary_Entity_CB 
// -----------------------------------------------------------------------------
//
NW_Status_t Binary_Entity_CB(NW_WBXML_Parser_t *aParser, 
                            NW_Uint32 aIndex, 
                            void* aContext)
{
	NW_String_t *tempString = NW_String_new();
	// handle OOM
	if(tempString == NULL)
	{
		return NW_STAT_FAILURE;
	}
	NW_String_entityToString(aIndex, tempString, aParser->doc->charset);
	NW_Ucs2 *ucs2;
    NW_String_byteToUCS2Char(tempString->storage, tempString->length, &ucs2);
	// handle OOM
	if(ucs2 == NULL)
	{
		NW_String_delete(tempString);
		return NW_STAT_FAILURE;
	}
	TPtrC wString((TUint16*)ucs2);
	NW_String_delete(tempString);
	
	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	TInt result((wObserver->GetBinaryContentHandler())->Entity(aIndex, wString));
	delete (void*)wString.Ptr();
	
	return (result?NW_STAT_SUCCESS:NW_STAT_FAILURE);
}
  
// -----------------------------------------------------------------------------
// Binary_Exception_CB For parse errors
// -----------------------------------------------------------------------------
//  
NW_Status_t Binary_Exception_CB(NW_WBXML_Parser_t* /*aParser*/, 
                               NW_WBXML_Exception_t /*aException*/, 
                               void* aContext)
{

	CXMLObserver *wObserver = (CXMLObserver*)aContext;
	return (wObserver->GetBinaryContentHandler())->Exception()?NW_STAT_SUCCESS:NW_STAT_FAILURE;
}




// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CXMLReader::CXMLReader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXMLReader::CXMLReader() 
{
	
}

// -----------------------------------------------------------------------------
// CXMLReader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXMLReader::ConstructL()
{
	iParserObserver = CXMLObserver::NewL();
	// Default parser = XML , not WBXML
	iIsBinary = FALSE; 
}

// -----------------------------------------------------------------------------
// ?classname::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXMLReader* CXMLReader::NewL()
{
	CXMLReader* self = new( ELeave ) CXMLReader();
   
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();

	return self;
}
    
// Destructor
CXMLReader::~CXMLReader()
{
	if(iParserObserver)
	{
		delete iParserObserver;
	}
	
}

// -----------------------------------------------------------------------------
// CXMLReader::SetFeature
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLReader::SetFeature(TXMLFeature aFeature, TBool aStatus)
{
	TInt wReturn;
	wReturn=0;

	switch(aFeature)
	{
		case EXMLBinary:
			iIsBinary = aStatus;
			wReturn = KErrNone;
		break;
		case EXMLValidation:
			wReturn = KErrNotSupported;
		break;
		default:
			wReturn = KErrNotSupported;
		break;
	}
	return wReturn;
}

// -----------------------------------------------------------------------------
// CXMLReader::GetFeature
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLReader::GetFeature(TXMLFeature aFeature, TBool &aStatus)
{
	TInt wReturn;
	wReturn=0;

	switch(aFeature)
	{
		case EXMLBinary:
			aStatus = iIsBinary;
			wReturn = KErrNone;
		break;
		case EXMLValidation:
			aStatus = FALSE;
			wReturn = KErrNotSupported;
		break;
		default:
			wReturn = KErrNotSupported;
		break;
	}
	return wReturn;
}

// -----------------------------------------------------------------------------
// CXMLReader::SetContentHandler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLReader::SetContentHandler(MXMLContentHandler* aParserObserver)
{
	iParserObserver->SetContentHandler(aParserObserver);
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CXMLReader::SetContentHandler
// Set binary XML content handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLReader::SetContentHandler(MWBXMLContentHandler* aParserObserver)
{
	iParserObserver->SetContentHandler(aParserObserver);
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CXMLReader::Parse
// Reades the input file, and calls ParseL(RFile&).
// @ exception can Leave due to OOM
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXMLReader::ParseL(RFs& aRFs, const TDesC& aFileToParse)
{
	RFile wXMLFile;
		
	//Open file
	User::LeaveIfError(wXMLFile.Open(aRFs, aFileToParse, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(wXMLFile);
	ParseL(wXMLFile);
	CleanupStack::PopAndDestroy(); // wXMLFile
}

// -----------------------------------------------------------------------------
// CXMLReader::Parse
// Reades the input from the opened file into buffer, and calls RunL.
// @ exception can Leave due to OOM
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXMLReader::ParseL(RFile& aOpenedFile)
{
	TInt wSize;
	User::LeaveIfError(aOpenedFile.Size(wSize));
		
	HBufC8 * binaryBuffer = HBufC8::NewLC(wSize);
	TPtr8 binaryBufferPtr = binaryBuffer->Des();
	User::LeaveIfError(aOpenedFile.Read(binaryBufferPtr, wSize));

	if(!iIsBinary)
	{
		TInt hichar = (CEditableText::EReversedByteOrderMark & 0xFF00)>>8;
		TInt lochar = CEditableText::EReversedByteOrderMark & 0xFF;
		TInt bytesPerChar = 1;
		if(binaryBufferPtr.Ptr()[0] == hichar && binaryBufferPtr.Ptr()[1] == lochar)
		{
			bytesPerChar = 2;
		}

		HBufC * dataBuffer = HBufC::NewLC(wSize/bytesPerChar);
		TPtr dataBufferPtr = dataBuffer->Des();
		
		if(bytesPerChar == 2)
		{
			dataBufferPtr.Set((TUint16*)binaryBufferPtr.Ptr()+1,
							wSize/bytesPerChar-1, 
							wSize/bytesPerChar-1);
		}
		else
		{
			CnvUtfConverter::ConvertToUnicodeFromUtf8(dataBufferPtr, binaryBuffer->Des());
		}
		User::LeaveIfError(ParseXML(dataBufferPtr));
		CleanupStack::Pop(); // dataBuffer
		CleanupStack::PopAndDestroy(); // binaryBuffer
		delete dataBuffer;
	}
	else
	{
		User::LeaveIfError(ParseWBXML(binaryBufferPtr));
		CleanupStack::PopAndDestroy(); // binaryBuffer
	}
}

// -----------------------------------------------------------------------------
// CXMLReader::ParseL
// Copies aBuff into input buffer and calls RunL
// @ exception can Leave due to OOM
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CXMLReader::ParseL(const TDesC8& aBuff) // change the allocations
{
	

	if(!iIsBinary)
	{
		HBufC * dataBuffer = HBufC::NewLC(aBuff.Size());
		TPtr dataBufferPtr = dataBuffer->Des();
		CnvUtfConverter::ConvertToUnicodeFromUtf8(dataBufferPtr, aBuff);
		User::LeaveIfError(ParseXML(dataBufferPtr));
		CleanupStack::PopAndDestroy(); // dataBuffer
	}
	else
	{
		User::LeaveIfError(ParseWBXML(aBuff));
	}
}



// -----------------------------------------------------------------------------
// CXMLReader::ParseXML
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLReader::ParseXML(const TDesC& aBuff)
{
	NW_Status_t status;
	struct NW_XML_Parser_EventCallbacks_s cb;
	struct NW_XML_Reader_s reader;

	Mem::FillZ(&cb, sizeof(NW_XML_Parser_EventCallbacks_s));

	// cXML callbacks
	cb.StartDocument_CB = StartDocumentCB;
	cb.EndDocument_CB = EndDocumentCB;
	cb.Tag_Start_CB = BeginElementCB;
	cb.Attr_Start_CB = AttributeStartCB;
	cb.Attr_VarVal_CB = AttributeValueCB;
	cb.Attributes_End_CB = AttributesEndCB;
	cb.Tag_End_CB = EndElementCB;
	cb.Content_CB = ContentCB;
	cb.Cdata_CB = CdataCB; 
	cb.Comment_CB = CommentCB;
	cb.PiForm_CB = PiCB;
	cb.pClientPointer = (void*)iParserObserver;
	cb.Entity_CB = EntityCB;
	cb.Attr_Entity_VarVal_CB = AttrEntityVarValCB;

	// Remove whitespace from beginning	
	TInt start(aBuff.Locate('<'));
	
	if (start == -1) 
	{
		return KErrParseFailed;
	}

	status = NW_XML_Reader_InitFromBuffer(&reader, aBuff.Right(aBuff.Length()-start).Size(), 
										(unsigned char*)aBuff.Right(aBuff.Length()-start).Ptr());
	NW_XML_Reader_SetEncoding(&reader, HTTP_iso_10646_ucs_2);
		
	// parse it!
	status = NW_XML_Parse(&reader, &cb);		

    ASSERT(!(reader.length % 2));
    TPtrC16 wChar((const TUint16*)reader.pBuf, reader.length / 2);
    MXMLContentHandler* aContentHandler = iParserObserver->GetContentHandler();
    aContentHandler->DataBuffer(wChar);


	if (!NW_STAT_IS_SUCCESS(status))
	{
		return KErrParseFailed;
	}
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CXMLReader::ParseWBXML
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CXMLReader::ParseWBXML(const TDesC8& aBuff)
{
	// WBXML CallBacks
	struct NW_WBXML_EventHandler_s binaryCB;
	
	Mem::FillZ(&binaryCB, sizeof(NW_WBXML_EventHandler_s));

	// State change events
	binaryCB.StartDocument_CB = Binary_StartDocument_CB;
	binaryCB.EndDocument_CB = Binary_EndDocument_CB;
	binaryCB.Pi_CB = Binary_Pi_CB;
	binaryCB.Pi_End_CB = Binary_Pi_End_CB;
	binaryCB.Tag_Start_CB = Binary_Tag_Start_CB;
	binaryCB.Tag_End_CB = Binary_Tag_End_CB;
	binaryCB.Attr_Start_CB = Binary_Attr_Start_CB;
	binaryCB.Attr_Val_CB = Binary_Attr_Val_CB;
	binaryCB.Content_CB = Binary_Content_CB;
	binaryCB.CodePage_CB = Binary_CodePage_CB;
	binaryCB.Extension_CB = Binary_Extension_CB;
	// Handle data types 
	binaryCB.Token_CB = Binary_Token_CB;
	binaryCB.FQToken_CB = Binary_FQToken_CB;
	binaryCB.InlineString_CB = Binary_InlineString_CB;
	binaryCB.TableString_CB = Binary_TableString_CB;
	binaryCB.Binary_CB = Binary_Binary_CB;
	binaryCB.Opaque_CB = Binary_Opaque_CB;
	binaryCB.Entity_CB = Binary_Entity_CB;
  	// For parse errors 
	binaryCB.Exception_CB = Binary_Exception_CB;

	// init dictionarys
	NW_WBXML_Dictionary_t *dictionaries[DICTIONARY_COUNT];
	dictionaries[0] = (NW_WBXML_Dictionary_t *)&NW_SyncML_1_0_WBXMLDictionary;
	dictionaries[1] = (NW_WBXML_Dictionary_t *)&NW_Wml_1_0_WBXMLDictionary;
	dictionaries[2] = (NW_WBXML_Dictionary_t *)&NW_Wml_1_1_WBXMLDictionary;
	dictionaries[3] = (NW_WBXML_Dictionary_t *)&NW_Wml_1_2_WBXMLDictionary;
	dictionaries[4] = (NW_WBXML_Dictionary_t *)&NW_Wml_1_3_WBXMLDictionary;
	dictionaries[5] = (NW_WBXML_Dictionary_t *)&NW_XHTML_WBXMLDictionary;
	dictionaries[6] = (NW_WBXML_Dictionary_t *)&NW_SL_WBXMLDictionary;
//	dictionaries[7] = (NW_WBXML_Dictionary_t *)&SL_dictionary;
//	dictionaries[8] = (NW_WBXML_Dictionary_t *)&NW_wap_prov_doc_WBXMLDictionary;
//	dictionaries[9] = (NW_WBXML_Dictionary_t *)&push_nokprov_dictionary;

	if ((NW_WBXML_Dictionary_initialize (DICTIONARY_COUNT, dictionaries)) != NW_STAT_SUCCESS)	
	{
		return KErrDictionaryInitFailed;
	} 

	NW_Status_t status;
    
    // create parser
	NW_WBXML_Parser_t *parser = (NW_WBXML_Parser_t*)User::Alloc(sizeof(NW_WBXML_Parser_t));		
	if(parser == NULL)
	{
		NW_WBXML_Dictionary_destroy();
		return KErrParseFailed;
	}
	NW_WBXML_Parser_newInPlace(parser);
	
	// create document
	NW_WBXML_Document_t *doc = (NW_WBXML_Document_t*)User::Alloc(sizeof(NW_WBXML_Document_t));
	if(doc == NULL)
	{
		NW_WBXML_Dictionary_destroy();
		User::Free(parser);
		parser = NULL;
		return KErrParseFailed;
	}
	status = NW_WBXML_Document_construct(doc, SL_1_0_PUBLIC_ID);
	if(status != NW_STAT_SUCCESS)
	{
		NW_WBXML_Dictionary_destroy();
		User::Free(parser);
		parser = NULL;
		User::Free(doc);
		doc = NULL;
		return KErrParseFailed;
	}
	
	
	status =NW_WBXML_Parser_registerHandler(parser, &binaryCB, (void*)iParserObserver);
	
  
  	if (status == NW_STAT_SUCCESS)
  	{
		status = NW_WBXML_Parser_parseBuffer(parser, doc, (NW_Byte*)aBuff.Ptr(), (NW_Uint32)aBuff.Length());
	}

	NW_WBXML_Dictionary_destroy();

	User::Free(parser);
	parser = NULL;
	NW_WBXML_Document_destruct(doc);
	User::Free(doc);
	doc = NULL;
	if(status != NW_STAT_SUCCESS)
	{
		return KErrParseFailed;
	}
	return KErrNone;
}

//  End of File  
