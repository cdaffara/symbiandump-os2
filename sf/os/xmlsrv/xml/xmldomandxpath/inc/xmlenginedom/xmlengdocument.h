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
// Document node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGDOCUMENT_H
#define XMLENGDOCUMENT_H

#include <f32file.h> 

#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengserializationoptions.h>

class RXmlEngDOMImplementation;

/**
This class represents an XML document in the DOM tree.  It stores all nodes 
and associated information about the XML document.  

This class implements the interface.  Another class, RXmlEngDOMImplementation,
provides the implementation.  An instance of RXmlEngDOMImplementation must be
constructed and opened first and passed to RXmlEngDocument::OpenL().
*/
class RXmlEngDocument : public TXmlEngNode
{
public:    
    /** 
	Default constructor.  An instance of RXmlEngDocument must be "opened" with
	one of OpenL() overloads before methods are invoked on the object.
	*/
    IMPORT_C RXmlEngDocument();
    
    /** 
    Opens the document.
	@param aDOMImpl An opened DOM implementation object
	@leave - One of the system-wide error codes
    */
    IMPORT_C void OpenL(RXmlEngDOMImplementation& aDOMImpl);
	
	/** 
	Opens the document, initializing it with the internal state pointer from
	another RXmlEngDocument.  This document becomes an alias for the document
	whose state is represented by aInternal and a change in either document
	will be reflected in the other.  Close() need only be called once, however,
	it is not an error to call Close() on each RXmlEngDocument.

	@param aDOMImpl An opened DOM implementation object
 	@param aInternal The internal document state to initialize this object with
	@leave - One of the system-wide error codes
    */
    IMPORT_C void OpenL(RXmlEngDOMImplementation& aDOMImpl, void* aInternal);
    
	/** 
	Opens the document and adds aRoot as the root of the DOM tree.  If aRoot is
	currently part of another document, it will be unlinked.  Ownership is
	transferred to this document.

	@param aDOMImpl An opened DOM implementation object
 	@param aRoot The element that will be the root of the DOM tree
	@leave - One of the system-wide error codes
    */
    IMPORT_C void OpenL(RXmlEngDOMImplementation& aDOMImpl, TXmlEngElement aRoot);
    
    /** 
	Closes document:  All owned nodes, child nodes, and namespaces are freed.  All
	data containers on the data container list are freed.
	*/
    IMPORT_C  void Close();

	/**
	Serializes document tree into a file.  For nodes containing binary data in
	the form of BinaryDataContainer, FileContainer or ChunkContainer, the
	client can implement custom serialization by implementing the
	MXmlEngDataSerializer interface and saving a pointer to the customer
	serializer in the iDataSerializer member of the aSaveOptions parameter. If
	no custom serialization is specified, the binary data container nodes are
	serialized like text nodes.

	If no aRoot is provided, the entire DOM tree is serialized.  aRoot does not
	need to be owned by this document.
    
	@param aFileName A file name (with path)
 	@param aRoot Root node to be serialized	
	@param aSaveOptions Options that control how serialization is performed 
    @return Number of bytes written
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseOfAPI Document is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
	*/
    IMPORT_C TInt SaveL( const TDesC& aFileName,
    					 TXmlEngNode aRoot = TXmlEngNode(),
    					 const TXmlEngSerializationOptions& aSaveOptions = TXmlEngSerializationOptions() ) const;
   
    /**
	Serializes document tree into a file.  For nodes containing binary data in
	the form of BinaryDataContainer, FileContainer or ChunkContainer, the
	client can implement custom serialization by implementing the
	MXmlEngDataSerializer interface and saving a pointer to the customer
	serializer in the iDataSerializer member of the aSaveOptions parameter. If
	no custom serialization is specified, the binary data container nodes are
	serialized like text nodes.
    
	If no aRoot is provided, the entire DOM tree is serialized.  aRoot does not
	need to be owned by this document.
    
	@param aRFs An open file Server session
	@param aFileName A file name (with path)
 	@param aRoot Root node to be serialized	
	@param aSaveOptions Options that control how serialization is performed 
    @return Number of bytes written
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseOfAPI Document is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
    */                        
    IMPORT_C TInt SaveL( RFs& aRFs,
                         const TDesC& aFileName,
                         TXmlEngNode aRoot = TXmlEngNode(),
                         const TXmlEngSerializationOptions& aSaveOptions = TXmlEngSerializationOptions() ) const;
    
	/**
	Serializes document tree into provided output stream, which supports
	progressive writing of data.  For nodes containing binary data in the form
	of BinaryDataContainer, FileContainer or ChunkContainer, the client can
	implement custom serialization by implementing the MXmlEngDataSerializer
	interface and saving a pointer to the customer serializer in the
	iDataSerializer member of the aSaveOptions parameter. If no custom
	serialization is specified, the binary data container nodes are serialized
	like text nodes.

	If no aRoot is provided, the entire DOM tree is serialized.  aRoot does not
	need to be owned by this document.
    
	@param aStream  An output stream to write the serialized DOM tree
 	@param aRoot Root node to be serialized	
	@param aSaveOptions Options that control how serialization is performed 
    @return Number of bytes written
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseOfAPI Document is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
    */
    IMPORT_C TInt SaveL( MXmlEngOutputStream& aStream,
						 TXmlEngNode aRoot = TXmlEngNode(),
						 const TXmlEngSerializationOptions& aSaveOptions = TXmlEngSerializationOptions() ) const;

    /**
    Saves document tree into memory buffer.

	Any existing contents in aBuffer will be deleted.  The memory required for
	aBuffer will be allocated by this method.  The method caller must Close()
	aBuffer.

	If no aRoot is provided, the entire DOM tree is serialized.  aRoot does not
	need to be owned by this document.
    
	@param aBuffer Resulting buffer
    @param aRoot The root of the subtree to serialize
    @param aSaveOptions Options that control how serialization is performed
    @return Size of buffer 
	@leave KXmlEngErrWrongEncoding Encoding not understood
	@leave KXmlEngErrWrongUseOfAPI Document is NULL
    @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
	@leave - One of the system-wide error codes
    */
    IMPORT_C TInt SaveL(RBuf8& aBuffer, 
    					TXmlEngNode aRoot = TXmlEngNode(), 
                        const TXmlEngSerializationOptions& aSaveOptions = TXmlEngSerializationOptions()) const;
   
	/**
	Creates a complete copy of the document and transfers ownership to the
	caller.  The caller is required to call Close() on the new document.  The
	new document is independant from this document and this document may be
	changed or closed without affecting the new document.

	@return Complete copy of the document 
	@leave - One of the system-wide error codes
    */
	IMPORT_C RXmlEngDocument CloneDocumentL() const;

	/**
	Creates a new element from a specific namespace to be the root of the
	document tree.  Any existing document element of the document is destroyed.
    
	@param aName Element name
    @param aNamespaceUri Element namespace URI
    @param aPrefix Element namemespace prefix
    @return The new root element
	@leave KXmlEngErrWrongUseOfAPI No name has been specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement CreateDocumentElementL(const TDesC8& aName, 
                                             const TDesC8& aNamespaceUri = KNullDesC8, 
                                             const TDesC8& aPrefix = KNullDesC8);

	/**
	Replaces (and destroys) the document element.  

	Note:  Use TXmlEngElement::ReconcileNamespacesL() on the new document
	element if it or its descendants can contain references to namespace
	declarations outside of the element.
    
	@param aNewDocElement New document element
    @see TXmlEngElement::ReconcileNamespacesL()
    */
    IMPORT_C void SetDocumentElement(TXmlEngElement aNewDocElement);

    /**
    Get document encoding.
	@return Encoding of the source XML data or TPtrC8("") if none.
    */
    IMPORT_C TPtrC8 XmlEncoding() const;

    /**
    Get xml version
	@return Version number reported by the XML declaration or TPtrC8("") if none.
    */
    IMPORT_C TPtrC8 XmlVersion() const;
  
    /**
    Retrieves base URI (if defined) of the document
	@return Document URI or TPtrC8("") if none.
    */
    IMPORT_C TPtrC8 DocumentUri() const;

    /**
    Check if document is standalone
	@return ETrue if standalone="true" was specified in the XML declaration in
	the source XML file.
    */
	IMPORT_C TBool IsStandalone() const;

    /**
    Sets XML version number to be shown in XML declaration when document is serialized.
	@param aVersion Version string
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  SetXmlVersionL(const TDesC8& aVersion);

	/**
	Sets the location of the document.  The document's URI is used as the
	top-level base URI definition.
	@param aUri Document URI
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  SetDocumentUriL(const TDesC8& aUri);

    /** 
	Sets "standalone" attribute of XML declaration for the document
	@param aStandalone Is the document standalone
    */
    IMPORT_C void  SetStandalone(TBool aStandalone);

    /** 
	Get the DOM implementation.  Ownership is not transferred.  Any operation
	on the returned object will affect this document directly, in particular, 
	a call to RXmlEngDOMImplementation::Close() will cause further operations
	on this document to fail.

	@return Object that represents current DOM implementation
    */
    IMPORT_C RXmlEngDOMImplementation Implementation() const;
    
    /**
    Get the document element
	@return The document element -- the top-most element in the document tree
    */
    IMPORT_C TXmlEngElement DocumentElement() const;

	/**
	Sets the "document" property on the node and all its descendants to be this
	RXmlEngDocument node 
	@param aSource Node that should be added.
    @return Adopted node
	@leave KXmlEngErrWrongUseOfAPI The node has a parent node, the node is
	already owned by this document, or the node is a document.
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNode AdoptNodeL(TXmlEngNode aSource);

    /**
    Creates a new attribute.

	aValue should represent the correct value of an attribute if it is put
	as-is into an XML file (with all characters correctly escaped with entity
	references when XML spec requires)
    
	The TXmlEngElement class provides a rich set of attribute creation methods,
	which not only create attributes but also link them into elements.
    
	@see TXmlEngElement
    
    There is no way to create attributes with namespaces (despite the DOM spec); 
    you have to use one of the TXmlEngElement::AddNewAttributeL(..) methods instead

	The returned attribute is the only reference to the allocated memory until
	you have attached the attribute to some element node.

	@param aName Name of the atribute; no prefix allowed
    @param aValue Value of the attribute (optional)
    @return The newly created attribute 
	@leave KXmlEngErrWrongUseOfAPI No name specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr CreateAttributeL(const TDesC8& aName,
                                    const TDesC8& aValue = KNullDesC8);

    /**
    Creates a new text node and copies the content string into it.
	@param aCharacters Text node content
    @return The created node
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngTextNode CreateTextNodeL(const TDesC8& aCharacters = KNullDesC8);

	/**
	Creates a new binary container and copies the specified cid and data into
	it.  A pointer to the container is stored in the document's data container
	list that can be fetched using GetDataContainerList().

    @see GetDataContainerList( RArray<TXmlEngDataContainer>& aList )
	@param aCid Content identifier     
	@param aData Binary octets
    @return The new binary container
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngBinaryContainer CreateBinaryContainerL( const TDesC8& aCid,
    												  const TDesC8& aData );

	/**
	Creates a new chunk container and copies the specified cid into it.  A
	reference to a memory chunk is stored in the container.  The memory chunk
	must stay in scope for the lifetime of the container.  A pointer to the
	container is stored in the document's data container list that can be
	fetched using GetDataContainerList().

    @see GetDataContainerList( RArray<TXmlEngDataContainer>& aList )     
	@param aCid Content identifier 
	@param aChunk RChunk reference
	@param aChunkOffset Offset to the binary data in aChunk
	@param aDataSize Size of binary data in aChunk
    @return The new chunk container
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngChunkContainer CreateChunkContainerL( const TDesC8& aCid, 
    												const RChunk& aChunk,
					                                const TInt aChunkOffset,
					                                const TInt aDataSize );

	/**
	Creates a new file container and copies the specified cid into it.  A
	reference to a file is stored in the container.  aFile must stay in scope
	of the lifetime of the container.  A pointer to the container is stored in
	the document's data container list that can be fetched using
	GetDataContainerList().

    @see GetDataContainerList( RArray<TXmlEngDataContainer>& aList )     
	@param aCid Content identifier
	@param aFile The file to reference
    @return The new file container
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngFileContainer CreateFileContainerL( const TDesC8& aCid, 
    											  const RFile& aFile );

	/**
	Creates a new element node that belongs to the specific namespace.  A
	namespace declaration node is created on the element.

	If the provided namespace uri is NULL, the element will be created without
	namespace.

	@param aNamespaceUri Namespace of new element
    @param aPrefix Prefix to use for the namespace binding and the QName of the element
    @param aLocalName Local name of the element 
    @return The created element
	@leave KXmlEngErrWrongUseOfAPI No name specified
	@leave - One of the system-wide error codes
    */  
    IMPORT_C TXmlEngElement CreateElementL(const TDesC8& aLocalName, 
                                     const TDesC8& aNamespaceUri = KNullDesC8, 
                                     const TDesC8& aPrefix = KNullDesC8);

    /**
	Creates a new comment node and copies the specified string into it.
	@param aText New comment
    @return The created node
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngComment CreateCommentL(const TDesC8& aText = KNullDesC8);

    /**
	Creates a new CDATA section node and copies the specified string into it.
	@param aContents CDATASection content
    @return The created node
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngCDATASection CreateCDATASectionL(const TDesC8& aContents = KNullDesC8);

    /**
	Creates a new entity reference node and copies the specified string into
	it.

	Note:  &lt; , &gt; , &apos; , &quot; and other predefined entity references
	should not be created with this method. These entity references are rather
	"character references" and are encoded/decoded automatically.

	@param aEntityRef is a string in one of these forms:
        -  name
        -  &name
        -  &name;
    where name is the name of the entity
    @return The new entity reference
	@leave KXmlEngErrWrongUseOfAPI No entity specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngEntityReference CreateEntityReferenceL(const TDesC8& aEntityRef);

    /**
    Creates a new empty Document Fragment node.  The document fragment is owned by
	this document.
	@return The created document fragment
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngDocumentFragment CreateDocumentFragmentL();

	/**
	Creates a new processing instruction node and copies "target" and "data"
	into it.
	
	@param aTarget Target
    @param aData Data
    @return The created processing instruction
	@leave KXmlEngErrWrongUseOfAPI No target specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngProcessingInstruction CreateProcessingInstructionL(const TDesC8& aTarget, 
                                                                 const TDesC8& aData = KNullDesC8);
                                                                 
	/**
	Sets the specified attribute as a xml:id attribute, starting at
	aStartElement and recursing through the subtree.  To set the specified
	attribute as a xml:id attribute for the entire DOM tree, see
	RegisterXmlId(const TDesC8&,const TDesC8&).

    @param aStartElement Root of the subtree to recurse
    @param aLocalName Name of the attribute
	@param aNamespaceUri Namespace of the new element (default empty)
	@leave KXmlEngErrWrongUseOfAPI The starting element is NULL, the attribute
	name is not specified, the starting element is the document, or the
	starting element does not belong to a document.
	@leave KErrAlreadyExists The attribute is already set to be xml:id
	@leave - One of the system-wide error codes
    */
    IMPORT_C void RegisterXmlIdL(TXmlEngElement aStartElement,
                                 const TDesC8& aLocalName, 
                                 const TDesC8& aNamespaceUri = KNullDesC8);

	/**
	Sets the specified attribute as a xml:id attribute, recursing through the
	entire DOM tree.  In order to specify a subtree only, see
	RegisterXmlId(TXmlEngElement,const TDesC8,const TDesC8).

    @param aLocalName Name of attribute
	@param aNamespaceUri Namespace of new element (default empty)
	@leave KXmlEngErrWrongUseOfAPI The document is NULL
	@leave KErrAlreadyExists The attribute is already set to be xml:id
	@leave - One of the system-wide error codes
    */
    IMPORT_C void RegisterXmlIdL(const TDesC8& aLocalName, 
                                 const TDesC8& aNamespaceUri = KNullDesC8);
       
    /**
    Looks for an element with the specified xml:id attribute
	@param aValue Name of attribute
    @return The found element or a NULL element if not found
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement FindElementByXmlIdL(const TDesC8& aValue ) const;  

    /**
    Retrieves an array of data containers owned by this document.

	Note: The document ceases to be the owner of a data container when the data
	container (or one of its predecessors) is removed from the document or
	becomes part of another document.  Unlinking a data container (or one of its
	predecessors) doesn't remove ownership of the data container from this
	document so the list might contain containers that are not linked to this
	document anymore.    

	@param aList Array of data containers
	@return KErrNone if successful or one of the system wide error codes otherwise
    */
    IMPORT_C TInt GetDataContainerList( RArray<TXmlEngDataContainer>& aList );
                                                                 
protected:
    friend class RXmlEngDOMParser;
    friend class TXmlEngNode;
    friend class TXmlEngAttr;
    friend class TXmlEngElement;
    friend class RXmlEngDOMImplementation;

protected:
    /**
    Constructor
	@param aInternal Document pointer
	*/
    inline RXmlEngDocument(void* aInternal);

    /**
    DISABLED for document; CloneDocumentL() must be used
	*/
    inline TXmlEngNode CopyL() const; 

    /**
    DISABLED for document; Close() must be used
	*/    
    inline void Remove();

    /**
    DISABLED for document; Close() must be used
	*/ 
    inline void ReplaceWith(TXmlEngNode aNode);
    inline void ReplaceWithL(TXmlEngNode aNode);
    
    /**
    DISABLED for document; Close() must be used
	 */ 
    inline TXmlEngNode SubstituteForL(TXmlEngNode aNode);

private:
	TInt SaveNodeL( TXmlEngNode aNode,
					RBuf8& aBuffer,
               	    MXmlEngOutputStream* aOutputStream = NULL,
               	    TXmlEngSerializationOptions aOpt = TXmlEngSerializationOptions()) const;	

    void InitOwnedNodeListL();
    void TakeOwnership(TXmlEngNode aNode);
    void RemoveOwnership(TXmlEngNode aNode);

protected:
    /**  Pointer to DOM implementation object */
    RXmlEngDOMImplementation* iImpl;

};// class RXmlEngDocument



#include <xml/dom/xmlengdocument.inl>

#endif /* XMLENGDOCUMENT_H */
