// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Callback methods for SAX libxml2 parser
//

#include <stdapis/libxml2/libxml2_modules.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include "xmlengsaxplugin.h"
#include "xmlengsaxpluginerror.h"

_LIT8(KEmptyString, "");

LOCAL_C const TInt KAttribPrefixOffset = 1;
LOCAL_C const TInt KAttribURIOffset = 2;
LOCAL_C const TInt KAttribValueWithEndOffset = 3;
LOCAL_C const TInt KAttribEndOffset = 4;
LOCAL_C const TInt KAttributeNextOffset = 5;

/**
 * GetLengthString:
 * @param   aString     the string 
 *
 * Return length of string.
 */	
TInt GetLengthString(const xmlChar* aString) 
	{
		const xmlChar* ptr = aString;		
	if(!ptr)
	    {
	    return 0;
	    }
        while (*ptr)
            {
            ++ptr;
            }

        return ptr - aString;
	}

/**
 * AttributeArrayDelete:
 * @param   aPtr     the argument required by TCleanupItem object
 *
 * Operation which pop and closed RAttributeArray object.
 */	
LOCAL_C void AttributeArrayDelete(TAny *aPtr)
	{
	RAttributeArray& attributes = *(RAttributeArray*)aPtr;

	TInt nAttributes = attributes.Count();

	for(TInt i=0; i<nAttributes; i++)
		{
		attributes[i].Close();
		}

	attributes.Close();
	}

/**
 * EntityDeclL:
 * @param aUserData the user data (XML parser context)
 * @param aName the entity name 
 * @param aType the entity type 
 * @param aPublicId The public ID of the entity
 * @param aSystemId The system ID of the entity
 * @param aContent the entity value (without processing).
 *
 * An entity definition has been parsed.
 */
void EntityDeclaration(void *aUserData,
				const xmlChar *aName,
				int aType,
				const xmlChar *aPublicId,
				const xmlChar *aSystemId,
				xmlChar *aContent)
	{
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	

	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	//because condition in parser.c file
	if (aType == XML_EXTERNAL_GENERAL_PARSED_ENTITY)
		{
		wrapper->getParserContext()->replaceEntities = 1;
		}
	else
		{
		wrapper->getParserContext()->replaceEntities = 0;
		}
	xmlSAX2EntityDecl(wrapper->getParserContext(), aName, aType, aPublicId, aSystemId, aContent);
	}

/**
 * ExternalSubset:
 * @param aUserData the user data (XML parser context)
 * @param aName the root element name
 * @param aExternalID the external ID
 * @param aSystemID the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on external subset declaration.
 */	
void ExternalSubset(void *aUserData,
				const xmlChar *aName,
				const xmlChar *aExternalID,
				const xmlChar *aSystemID)
				
	{
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	xmlSAX2ExternalSubset(wrapper->getParserContext(), aName, aExternalID, aSystemID);
	}			

/**
 * ResolveEntity:
 * @param aUserData the user data (XML parser context)
 * @param aPublicId The public ID of the entity
 * @param aSystemId The system ID of the entity
 *
 * Callback on resolve entity.
 */
xmlParserInputPtr ResolveEntity(void *aUserData,
				const xmlChar *aPublicId,
				const xmlChar *aSystemId)			
    {
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);		
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
    return xmlSAX2ResolveEntity(wrapper->getParserContext(), aPublicId, aSystemId);
    }

/**
 * GetEntityL:
 * @param aUserData the user data (XML parser context)
 * @param aName The entity name
 *
 * Get an entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */	
xmlEntityPtr GetEntityL(void *aUserData,
				const xmlChar *aName)
	{
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
	wrapper->getParserContext()->replaceEntities = 0;
	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	xmlEntityPtr entity;
	entity = xmlSAX2GetEntity(wrapper->getParserContext(), aName);
	
	//predefined, internal and external entity
	if (entity) 
		{
		//entity in attribute
		if (wrapper->getParserContext()->instate == XML_PARSER_ATTRIBUTE_VALUE) 
			{
			wrapper->getParserContext()->replaceEntities = 1;
			}
		//entity in content
		else 
			{
			
			//internal and predefined - nothing
			
			//external entity
			if (entity->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY) 
				{
				TPtrC8 skipString(aName);
				RString ref = wrapper->StringPool().OpenStringL(skipString);
		
				CleanupClosePushL(ref);		
				wrapper->getContentHandler()->OnSkippedEntityL(ref, KErrNone);
				CleanupStack::PopAndDestroy(&ref);			
				}
			}
			
		return entity;
		}

	//external subset
	if (wrapper->getParserContext()->hasExternalSubset)
		{
		
		wrapper->GetEntity().name = aName;
		wrapper->GetEntity().type = XML_ENTITY_DECL;
		wrapper->GetEntity().etype = XML_INTERNAL_GENERAL_ENTITY;
		wrapper->GetEntity().orig = BAD_CAST "";
		wrapper->GetEntity().content = BAD_CAST "";

		if (wrapper->getParserContext()->instate == XML_PARSER_ATTRIBUTE_VALUE 
				&& !wrapper->IsFeatureEnabled(EReplaceIntEntityFromExtSubsetByRef)) 
			{
			wrapper->getParserContext()->replaceEntities = 1;
			return &(wrapper->GetEntity());
			}
	
		TPtrC8 skipString(aName);
		RString ref = wrapper->StringPool().OpenStringL(skipString);
		
		CleanupClosePushL(ref);		
		wrapper->getContentHandler()->OnSkippedEntityL(ref, KErrNone);
		CleanupStack::PopAndDestroy(&ref);
		
		return &(wrapper->GetEntity());
		
		}
		
	return NULL;
	}

/**
 * GetEntityCal:
 * @param aUserData the user data (XML parser context)
 * @param aName The entity name
 *
 * Get an entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */	
xmlEntityPtr GetEntityCal(void *aUserData, const xmlChar *aName)
    {
    xmlEntityPtr tmp = NULL;
    TRAPD(error,tmp = GetEntityL(aUserData,aName));
    if(error != KErrNone)
        {
        CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
        wrapper->StopParsing(error);
        }
    return tmp;
    }

/**
 * GetParameterEntity:
 * @param aUserData the user data (XML parser context)
 * @param aName The entity name
 *
 * Get a parameter entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */	
xmlEntityPtr GetParameterEntity(void *aUserData,
				const xmlChar *aName)
{
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
	
	xmlEntityPtr entity;
	entity = xmlSAX2GetParameterEntity(wrapper->getParserContext(), aName);
	
	return entity;
}	

/**
 * StructuredError:
 * @param aUserData user provided data for the error callback
 * @param aError the error being raised.
 *
 * Signature of the function to use when there is an error and
 * the module handles the new error reporting mechanism.
 */
void StructuredError(void *aUserData, 
				xmlErrorPtr aError)
{
	
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
	TInt error = GetErrorNum(aError->code);
	
	// warning handling
	if (error == KErrNone)
		{
		return;
		}
	
	//because cascade of errors
	//first error is returned in case when any other event occurs between cascade of calling libxml2_StructuredError event
	if (!wrapper->IsErrorStatus())
	{
		wrapper->getContentHandler()->OnError(error);
		wrapper->SetErrorStatus(ETrue);
	}
}

/**
 * StartDocumentL:
 * @param aUserData the user data (XML parser context)
 *
 * Called when the document start being processed.
 */
void StartDocumentL(void* aUserData) 
	{	

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	RDocumentParameters params;	
	TPtrC8 enc(KEmptyString);
	
	if (wrapper->getParserContext()->encoding) 
		{
		TPtrC8 tmpStr(wrapper->getParserContext()->encoding);
		enc.Set(tmpStr);
		}
	
	params.Open(wrapper->StringPool().OpenStringL(enc));
	CleanupClosePushL(params);
	
	wrapper->getContentHandler()->OnStartDocumentL(params, KErrNone);
    CleanupStack::PopAndDestroy(&params);
	}

/**
 * StartDocument:
 * @param aUserData the user data (XML parser context)
 *
 * Called when the document start being processed.
 */
void StartDocument(void* aUserData) 
    {
    TRAPD(error,StartDocumentL(aUserData));
    if(error != KErrNone)
        {
        CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
        wrapper->StopParsing(error);
        }
    }

/**
 * EndDocumentL:
 * @param aUserData the user data (XML parser context)
 *
 * Called when the document end has been detected.
 */
void EndDocument(void* aUserData)
	{

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	TRAPD(error,wrapper->getContentHandler()->OnEndDocumentL(KErrNone));
    if(error)
        {
        wrapper->StopParsing(error);
        }
	}

/**
 * Characters:
 * @param aUserData the user data (XML parser context)
 * @param aCharacter a xmlChar string
 * @param aLen the number of xmlChar
 *
 * Receiving some chars from the parser.
 */
void Characters(void* aUserData, const xmlChar* aCharacter, int aLen)
{	
	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);	
	
	TPtrC8 ptr(aCharacter, aLen);
	TRAPD(error,wrapper->getContentHandler()->OnContentL(ptr, KErrNone));
    if(error)
        {
        wrapper->StopParsing(error);
        }	
}

/**
 * IgnorableWhitespace:
 * @param aUserData the user data (XML parser context)
 * @param aCharacter a xmlChar string
 * @param aLen the number of xmlChar
 *
 * Receiving some ignorable whitespaces from the parser.
 * UNUSED: by default the DOM building will use characters.
 */
void IgnorableWhitespace(void *aUserData, const xmlChar* aCharacter, int aLen)
	{	

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	TPtrC8 ptr(aCharacter, aLen);	
	TRAPD(error,wrapper->getContentHandler()->OnIgnorableWhiteSpaceL(ptr, KErrNone));
    if(error)
        {
        wrapper->StopParsing(error);
        }
	}

/**
 * ProcessingInstruction:
 * @param aUserData the user data (XML parser context)
 * @param aTarget the target name
 * @param aData the PI data's
 *
 * A processing instruction has been parsed.
 */
void ProcessingInstruction(void *aUserData, const xmlChar* aTarget, const xmlChar* aData)
	{	

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	TPtrC8 target(KEmptyString);
	TPtrC8 data(KEmptyString);
	
	if (aTarget) 
		{
		TPtrC8 ptrTemp(aTarget);
		target.Set(ptrTemp);		
		}
		
	if (aData) 
		{
		TPtrC8 ptrTemp(aData);
		data.Set(ptrTemp);		
		}	
	
	TRAPD(error,wrapper->getContentHandler()->OnProcessingInstructionL(target, data, KErrNone));
    if(error)
        {
        wrapper->StopParsing(error);
        }
	}

/**
 * StartElementNsL:
 * @param aUserData the user data (XML parser context)
 * @param aLocalname the local name of the element
 * @param aPrefix the element namespace prefix if available
 * @param aURI the element namespace name if available
 * @param aNbNamespaces number of namespace definitions on that node
 * @param aNamespaces pointer to the array of prefix/URI pairs namespace definitions
 * @param aNbAttributes the number of attributes on that node
 * @param aNbDefaulted the number of defaulted attributes. The defaulted
 *                  ones are at the end of the array
 * @param aAttributes pointer to the array of (localname/prefix/URI/value/end)
 *               attribute values.
 *
 * SAX2 callback when an element start has been detected by the parser.
 * It provides the namespace informations for the element, as well as
 * the new namespace declarations on the element.
 */
void StartElementNsL(
					void *aUserData,
					const xmlChar *aLocalName,
					const xmlChar *aPrefix,
					const xmlChar *aURI,
					int /*aNbNamespaces*/,
					const xmlChar **/*aNamespaces*/,
					int aNbAttributes,
					int /*aNbDefaulted*/,
					const xmlChar **aAttributes)
	{	

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);

	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);

	TPtrC8 ptrName(aLocalName);
	TPtrC8 ptrPrefix;	
	TPtrC8 ptrUri;
	HBufC8* buf = NULL;
	
	if(wrapper->IsFeatureEnabled(EConvertTagsToLowerCase))
		{
		//remove buffer when any function in this method leave		
		buf = HBufC8::NewLC(ptrName.Length());
		buf->Des().Copy(ptrName);
		buf->Des().LowerCase();
		ptrName.Set(buf->Des());		
		}

	if (aPrefix) 
		ptrPrefix.Set(aPrefix);
	
	if (aURI) 
		ptrUri.Set(aURI);

	RTagInfo nameTagInfo;
	RString strURI, strPrefix, strName;

    strURI = wrapper->StringPool().OpenStringL(ptrUri);
    CleanupClosePushL(strURI);
	
	strPrefix = wrapper->StringPool().OpenStringL(ptrPrefix);
	CleanupClosePushL(strPrefix);
	
	strName = wrapper->StringPool().OpenStringL(ptrName);	
		
	CleanupStack::Pop(&strPrefix);
	CleanupStack::Pop(&strURI);
		
	nameTagInfo.Open(strURI, strPrefix, strName);
	CleanupClosePushL(nameTagInfo);

	const TUint8** ptr;
	ptr = aAttributes;

	RAttributeArray attributeArray;
	CleanupStack::PushL(TCleanupItem(AttributeArrayDelete, &attributeArray));
		
	if (ptr) 
		{
		for (TInt i=0; i<aNbAttributes; i++) 
			{
			TPtrC8 attribName(*ptr);
			TPtrC8 attribPrefix;			
			if (*(ptr+1)) 
				{
				attribPrefix.Set(*(ptr + KAttribPrefixOffset));
				}
			TPtrC8 attribURI;
			if (*(ptr + KAttribURIOffset)) 
				{
				attribURI.Set(*(ptr + KAttribURIOffset));
				}

			TPtrC8 attribValue(*(ptr + KAttribValueWithEndOffset), *(ptr+KAttribEndOffset) - *(ptr+KAttribValueWithEndOffset));
				     			

			RString strAttribURI, strAttribPrefix, strAttribName, strAttribValue;
			
			strAttribURI = wrapper->StringPool().OpenStringL(attribURI);
			CleanupClosePushL(strAttribURI);
				
			strAttribPrefix = wrapper->StringPool().OpenStringL(attribPrefix);
			CleanupClosePushL(strAttribPrefix);
				
			strAttribName = wrapper->StringPool().OpenStringL(attribName);
			CleanupClosePushL(strAttribName);
				
			strAttribValue = wrapper->StringPool().OpenStringL(attribValue);
				
			CleanupStack::Pop(&strAttribName);
			CleanupStack::Pop(&strAttribPrefix);
			CleanupStack::Pop(&strAttribURI);

			RAttribute attrib;
			attrib.Open(strAttribURI, strAttribPrefix, strAttribName, strAttribValue);
			CleanupClosePushL(attrib);
			attributeArray.AppendL(attrib);
			CleanupStack::Pop(&attrib);

			ptr = ptr + KAttributeNextOffset;
			}
		}
	wrapper->getContentHandler()->OnStartElementL(nameTagInfo,attributeArray, KErrNone);
	CleanupStack::PopAndDestroy(&attributeArray);
	CleanupStack::PopAndDestroy(&nameTagInfo);	
		
	if (buf) 
		{
		CleanupStack::PopAndDestroy(buf);     
		}
	}

/**
 * StartElementNs:
 * @param aUserData the user data (XML parser context)
 * @param aLocalname the local name of the element
 * @param aPrefix the element namespace prefix if available
 * @param aURI the element namespace name if available
 * @param aNbNamespaces number of namespace definitions on that node
 * @param aNamespaces pointer to the array of prefix/URI pairs namespace definitions
 * @param aNbAttributes the number of attributes on that node
 * @param aNbDefaulted the number of defaulted attributes. The defaulted
 *                  ones are at the end of the array
 * @param aAttributes pointer to the array of (localname/prefix/URI/value/end)
 *               attribute values.
 *
 * SAX2 callback when an element start has been detected by the parser.
 * It provides the namespace informations for the element, as well as
 * the new namespace declarations on the element.
 */
void StartElementNs(
					void *aUserData,
					const xmlChar *aLocalName,
					const xmlChar *aPrefix,
					const xmlChar *aURI,
					int aNbNamespaces,
					const xmlChar **aNamespaces,
					int aNbAttributes,
					int aNbDefaulted,
					const xmlChar **aAttributes)
    {
	TRAPD(error, StartElementNsL(aUserData,aLocalName,aPrefix,aURI,
	                    aNbNamespaces,aNamespaces,aNbAttributes,aNbDefaulted,aAttributes));
    if(error != KErrNone)
        {
        CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
        wrapper->StopParsing(error);
        }
    }

/**
 * EndElementNsL:
 * @param aUserData the user data (XML parser context)
 * @param aLocalname the local name of the element
 * @param aPrefix the element namespace prefix if available
 * @param aURI the element namespace name if available
 *
 * SAX2 callback when an element end has been detected by the parser.
 * It provides the namespace informations for the element.
 */
void EndElementNsL(
					void *aUserData,
					const xmlChar *aLocalName,
					const xmlChar *aPrefix,
					const xmlChar *aURI)
	{	

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);

	TPtrC8 ptrUri;	
	TPtrC8 ptrPrefix;
	TPtrC8 ptrName(aLocalName);
	
	HBufC8* buf = NULL;
	
	if(wrapper->IsFeatureEnabled(EConvertTagsToLowerCase))
		{
		//remove buffer when any function in this method leave		
		buf = HBufC8::NewLC(ptrName.Length());
		buf->Des().Copy(ptrName);
		buf->Des().LowerCase();
		ptrName.Set(buf->Des());
		}

	
	if (aPrefix) 
		ptrPrefix.Set(aPrefix);
	
	if (aURI) 
		ptrUri.Set(aURI);
	
	RTagInfo nameTagInfo;
	RString strURI, strPrefix, strName;

	strURI = wrapper->StringPool().OpenStringL(ptrUri);
	CleanupClosePushL(strURI);
       
    strPrefix = wrapper->StringPool().OpenStringL(ptrPrefix);
	CleanupClosePushL(strPrefix);
		
    strName = wrapper->StringPool().OpenStringL(ptrName);
	CleanupClosePushL(strName);

	nameTagInfo.Open(strURI, strPrefix, strName);
	wrapper->getContentHandler()->OnEndElementL(nameTagInfo, KErrNone);
				   
	CleanupStack::PopAndDestroy(&strName);
	CleanupStack::PopAndDestroy(&strPrefix);
	CleanupStack::PopAndDestroy(&strURI);
	
	if (buf) 
		{
		CleanupStack::PopAndDestroy(buf);
		}
	}

/**
 * EndElementNs:
 * @param aUserData the user data (XML parser context)
 * @param aLocalname the local name of the element
 * @param aPrefix the element namespace prefix if available
 * @param aURI the element namespace name if available
 *
 * SAX2 callback when an element end has been detected by the parser.
 * It provides the namespace informations for the element.
 */
void EndElementNs(	void *aUserData,
					const xmlChar *aLocalName,
					const xmlChar *aPrefix,
					const xmlChar *aURI)
    {
	TRAPD(error, EndElementNsL(aUserData,aLocalName,aPrefix,aURI));
    if(error != KErrNone)
        {
        CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
        wrapper->StopParsing(error);
        }
    }

/**
 * StartPrefixMappingL:
 * @param aUserData     the user data (XML parser context)
 * @param aPrefix  the element namespace prefix if available, NULL if default namespace
 * @param aURI     the element namespace name if available
 *
 * SAX2 callback when namespace prefix mapping is done.
 */
void StartPrefixMappingL(
		void* aUserData,
		const xmlChar* aPrefix,
		const xmlChar* aURI)
{

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	if (wrapper->IsFeatureEnabled(EReportNamespaceMapping))
	{
		TPtrC8 ptrPrefix;
		TPtrC8 ptrURI;
	
		if (aPrefix) 
		{
			ptrPrefix.Set(aPrefix);
		}
		
		if (aURI) 
		{
			ptrURI.Set(aURI);
		}
	
	
		RString strPrefix = wrapper->StringPool().OpenStringL(ptrPrefix);
		CleanupClosePushL(strPrefix);
		RString strURI = wrapper->StringPool().OpenStringL(ptrURI);
	    CleanupClosePushL(strURI);
		wrapper->getContentHandler()->OnStartPrefixMappingL(strPrefix, strURI, KErrNone);
		CleanupStack::PopAndDestroy(&strURI);
		CleanupStack::PopAndDestroy(&strPrefix);
	}	
}

/**
 * StartPrefixMapping:
 * @param aUserData     the user data (XML parser context)
 * @param aPrefix  the element namespace prefix if available, NULL if default namespace
 * @param aURI     the element namespace name if available
 *
 * SAX2 callback when namespace prefix mapping is done.
 */
void StartPrefixMapping(
		void* aUserData,
		const xmlChar* aPrefix,
		const xmlChar* aURI)
    {
	TRAPD(error, StartPrefixMappingL(aUserData,aPrefix,aURI));
    if(error != KErrNone)
        {
        CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
        wrapper->StopParsing(error);
        }
    }

/**
 * EndPrefixMappingL:
 * @param   aUserData     the user data (XML parser context)
 * @param   aPrefix  the element namespace prefix if available, NULL otherwise
 *
 * SAX2 callback when namespace prefix mapping is getting out of scope.
 */
void EndPrefixMappingL(
		void* aUserData,
		const xmlChar* aPrefix)
{	

	CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);	
	//reset of recoverable error
	wrapper->SetErrorStatus(EFalse);
	
	if (wrapper->IsFeatureEnabled(EReportNamespaceMapping))
	{
		TPtrC8 ptrPrefix;
	
		if (aPrefix) 
		{
			ptrPrefix.Set(aPrefix);
		}
	
		RString strPrefix = wrapper->StringPool().OpenStringL(ptrPrefix);
		CleanupClosePushL(strPrefix);
		wrapper->getContentHandler()->OnEndPrefixMappingL(strPrefix, KErrNone);
        CleanupStack::PopAndDestroy(&strPrefix);
	}
}	

/**
 * EndPrefixMapping:
 * @param   aUserData     the user data (XML parser context)
 * @param   aPrefix  the element namespace prefix if available, NULL otherwise
 *
 * SAX2 callback when namespace prefix mapping is getting out of scope.
 */
void EndPrefixMapping(
		void* aUserData,
		const xmlChar* aPrefix)
    {
	TRAPD(error, EndPrefixMappingL(aUserData,aPrefix));
    if(error != KErrNone)
        {
        CXMLEngineSAXPlugin* wrapper = reinterpret_cast<CXMLEngineSAXPlugin*>(aUserData);
        wrapper->StopParsing(error);
        }
    }
