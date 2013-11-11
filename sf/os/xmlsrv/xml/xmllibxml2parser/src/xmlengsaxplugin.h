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
// SAX libxml2 parser plugin
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGSAXPLUGIN_H
#define XMLENGSAXPLUGIN_H

#include <e32std.h>
#include <stringpool.h>
#include <stdlib.h>
#include <badesca.h>

#include <ecom/implementationproxy.h>
#include <xml/xmlframeworkerrors.h>
#include <xml/wbxmlextensionhandler.h>
#include <xml/stringdictionarycollection.h>
#include <xml/taginfo.h>
#include <xml/parserfeature.h>
#include <xml/attribute.h>
#include <xml/documentparameters.h>
#include <xml/contenthandler.h>
#include <xml/plugins/parserinterface.h>
#include <xml/plugins/parserinitparams.h>
#include <xml/xmlparsererrors.h>

#include <libxml2_parser.h>
#include <libxml2_entities.h>
#include <libxml2_sax2.h>

#include <xml/utils/xmlengxestd.h>

using namespace Xml;

const TInt KFeatures  = EConvertTagsToLowerCase | EReportNamespaceMapping 
						| EReportNamespaces | EReportNamespacePrefixes 
						| EReplaceIntEntityFromExtSubsetByRef;

const TInt KGranularityOfEntitiesArray = 8;						

/**
 * GetLengthString:
 * @param   aString     the string 
 *
 * Return length of string.
 */	
TInt GetLengthString(const xmlChar* aString);

/**
 * EntityDecl:
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
				xmlChar *aContent);

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
				const xmlChar *aSystemID);

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
				const xmlChar *aSystemId);			

/**
 * StructuredError:
 * @param aUserData user provided data for the error callback
 * @param aError the error being raised.
 *
 * Signature of the function to use when there is an error and
 * the module handles the new error reporting mechanism.
 */
void StructuredError(void *aUserData, 
				xmlErrorPtr aError);

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
				const xmlChar *aName);

/**
 * GetEntityCal:
 * @param aUserData the user data (XML parser context)
 * @param aName The entity name
 *
 * Get an entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */	
xmlEntityPtr GetEntityCal(void *aUserData,
				const xmlChar *aName);

/**
 * GetParameterEntityL:
 * @param aUserData the user data (XML parser context)
 * @param aName The entity name
 *
 * Get a parameter entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */					
xmlEntityPtr GetParameterEntity(void *aUserData,
				const xmlChar *aName);

/**
 * StartDocument:
 * @param aUserData the user data (XML parser context)
 *
 * Called when the document start being processed.
 */
void StartDocument(void* aUserData);

/**
 * EndDocument:
 * @param aUserData the user data (XML parser context)
 *
 * Called when the document end has been detected.
 */
void EndDocument(void* aUserData);

/**
 * Characters:
 * @param aUserData the user data (XML parser context)
 * @param aCharacter a xmlChar string
 * @param aLen the number of xmlChar
 *
 * Receiving some chars from the parser.
 */
void Characters(void* aUserData, const xmlChar* aCharacter, int aLen);

/**
 * IgnorableWhitespace:
 * @param aUserData the user data (XML parser context)
 * @param aCharacter a xmlChar string
 * @param aLen the number of xmlChar
 *
 * Receiving some ignorable whitespaces from the parser.
 * UNUSED: by default the DOM building will use characters.
 */
void IgnorableWhitespace(void *aUserData, const xmlChar* aCharacter, int aLen);

/**
 * ProcessingInstruction:
 * @param aUserData the user data (XML parser context)
 * @param aTarget the target name
 * @param aData the PI data's
 *
 * A processing instruction has been parsed.
 */
void ProcessingInstruction(void *aUserData, const xmlChar* aTarget, const xmlChar* aData);

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
					const xmlChar *aLocalname,
					const xmlChar *aPrefix,
					const xmlChar *aURI,
					int aNbNamespaces,
					const xmlChar **aNamespaces,
					int aNbAttributes,
					int aNbDefaulted,
					const xmlChar **aAttributes);

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
					const xmlChar *aLocalname,
					const xmlChar *aPrefix,
					const xmlChar *aURI,
					int aNbNamespaces,
					const xmlChar **aNamespaces,
					int aNbAttributes,
					int aNbDefaulted,
					const xmlChar **aAttributes);

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
					const xmlChar *aLocalname,
					const xmlChar *aPrefix,
					const xmlChar *aURI);

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
void EndElementNs(
					void *aUserData,
					const xmlChar *aLocalname,
					const xmlChar *aPrefix,
					const xmlChar *aURI);

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
		const xmlChar* aURI);

/**
 * EndPrefixMappingL:
 * @param   aUserData     the user data (XML parser context)
 * @param   aPrefix  the element namespace prefix if available, NULL otherwise
 *
 * SAX2 callback when namespace prefix mapping is getting out of scope.
 */
void EndPrefixMappingL(
		void* aUserData,
		const xmlChar* aPrefix);

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
		const xmlChar* aURI);

/**
 * EndPrefixMapping:
 * @param   aUserData     the user data (XML parser context)
 * @param   aPrefix  the element namespace prefix if available, NULL otherwise
 *
 * SAX2 callback when namespace prefix mapping is getting out of scope.
 */
void EndPrefixMapping(
		void* aUserData,
		const xmlChar* aPrefix);

/*

CXMLEngineSAXPlugin class.

This class encapsulates the LibXML2 parser. It is the engine behind CXMLEngineSAXPlugin.

Handles construction and deletion of LibXML2 parser. Defines handlers required for LibXML2,
transforms handler data to required types and passes this to the client by MContentHandler 
callbacks.

*/

class CXMLEngineSAXPlugin : public CBase, public MParser
	{
	
	
	public:

		static MParser* NewL(TAny* aInitParams);
		
		virtual ~CXMLEngineSAXPlugin();

		//From MParser
		
		/** 
		 * Enable a feature. 
		 */
		TInt EnableFeature(TInt aParserFeature);
		
		/** 
		 * Disable a feature. 
		 */
		TInt DisableFeature(TInt);

		/** 
		 * See if a feature is enabled. 
		 */	
		TBool IsFeatureEnabled(TInt) const;
	
		/** 
		 * Parses a descriptor that contains part of a document. 
		 */
		void ParseChunkL (const TDesC8& aDescriptor);
		
		/** 
		 * Parses a descriptor that contains the last  part of a document. 
		 */
		void ParseLastChunkL(const TDesC8& aDescriptor);

		/** 
		 * Interfaces don't have a destructor, so we have an explicit method instead. 
		 */
		void Release();

		// From MContentSouce
	
		/** 
		 * This method tells us what's the next content handler in the chain. 
		 */	
		void SetContentSink (MContentHandler& aContentHandler);

		//Public getter and setter method
		
		/** 
		 * This method returns pool object. 
		 */
		inline RStringPool& StringPool();

		/** 
		 * This method returns a user handler object. 
		 */		
		inline MContentHandler* getContentHandler() const;

		/** 
		 * This method returns a parser context object. 
		 */	
		xmlParserCtxtPtr getParserContext() const;
		
		/** 
		 * This method return error status. 
		 */			
		inline TBool IsErrorStatus();
		
		/** 
		 * This method set error status. 
		 */			
		inline void SetErrorStatus(TBool aStatus);
		
		/** 
		 * This method stop parsing. 
		 */			
		void StopParsing(TInt aError);
		
		xmlEntity& GetEntity();
		
		/** 
		 * This method releases context and leaves with aError code
		 */			
		 void CleanupAndLeaveL(TInt aError);
		
	private:

		//constructor of CXMLEngineSAXPlugin
		CXMLEngineSAXPlugin(TParserInitParams* aInitParams);		
		void ConstructL();
		
	private:
		
		//user handler object 
		MContentHandler* iContentHandler;
		
		RStringDictionaryCollection* iStringDictionaryCollection;
		CCharSetConverter* iCharSetConverter;
		RElementStack* iElementStack;
		RStringPool iStringPool;
		
		//context to the libxml2 sax parser
		xmlParserCtxtPtr iParserContext;
		
		//context to the libxml2 sax handler
		xmlSAXHandler iParserEvents;
		
		//Features mode
		TInt iParserMode;
		
		//flag which tells that in parsing process error occurs (TRUE if error was found)
		TBool Flag_Error_Status; 
		
		xmlEntity iEntity;
	};

#include "xmlengsaxplugin.inl"

#endif /* XMLENGSAXPLUGIN_H */

