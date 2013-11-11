/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/** @file
@publishedAll
@released
*/
#ifndef XMLENGELEMENT_H
#define XMLENGELEMENT_H

#include <xml/dom/xmlengattr.h>
#include <xml/dom/xmlengnamespace.h>

template<class T> class RXmlEngNodeList;


/**
This class represents an XML element in the DOM tree.

Namespace handling:
 
The namespace of a XML element is an URI that in pair with the local part of
the element's name consistute the @c expanded-name of the element. It is said
that "the element is of NNN namespace".

XML elements are shown as belonging to a specific namespace by using prefixes
that previously were bound to some namespace URIs. The scope of a prefix is the
element, where it was declared and all its child (sub-)elements.
     
Namespace declaration is created by using a special @c xmlns:{prefix-name} 
attribute (which is not really considered as an attribute in DOM):
@code 
   <a xmlns:pr="http://some.uri.com/"> ... </a>
OR
   <pr:a xmlns:pr="http://some.uri.com/"> ... </a>
   <a xmlns="http://some.uri.com/"> ... </a>
@endcode
 
The latter two examples are equivalent and show the use of @c default namespace.
   
Implementation notes:
- Elements having no namespace are either presented with a NULL 
  TXmlEngNamespace node or a TXmlEngNamespace node that has NULL (KNullDesC8)
  prefix and namespace URI set to "" (an empty descriptor).  The former is 
	used by default on all nodes, whereas the latter is for cases when some node 
	contains undeclaration of the default namespace:
  @code
	   <a xmlns=""> .. </a>
  @endcode
 
- The prefix of the default attribute is NULL (KNullDesC8), not an "" (empty
	descriptor).  An empty descriptor which corresponds to
  @code
      <a xmlns:="http://some.uri.com/"> ... </a>
  @endcode
  (it does not contradict XML spec, but you are strongly advised against using 
  this)

- Prefix "xml" is reserved by XML Namespace spec for special purposes; it is 
  implicitly bound to XML's namespace <i>"http://www.w3.org/XML/1998/namespace"</i> 
  and no one is allowed to use this prefix except as with spec-defined 
  elements and attributes or to rebind this prefix to other namespaces
 
- Namespace URI may be "" (an empty descriptor) only for default namespace. In
  other words, "" namespace URI may not be bound to non-NULL prefix.
         
  Declaration of "" (an empty descriptor) namespace with NULL (KNullDesC8) 
  prefix results in:
  @code
      <a xmlns=""> ... </a>
  @endcode
  which undeclares any existing (in some parent element) default namespace
  for the scope of element 'a': element, its attributes and all child nodes 
  of DOM tree. Note, that such "undeclaration" will be added only if neccessary.
       
- Unneccessary namespace declaration are ignored. Attemps to add namespace binding 
  using same namespace URI and prefix if such binding already exists in the scope
  will have no effect.
 
- IMPORTANT! Attributes DO NOT HAVE default namespaces. If an attribute has no
  prefix, its namespace is undeclared even if there is some default namespaces for
  the scope of the element, which contains the attribute.
 
So, it is wrong to write something like this:
@code
    <a xmlns="ns_uri"  attr="value"> ... </a>
@endcode
and assume that the attr belongs to namespace pointed to with @c ns_uri.
   
HINTS:
- Use namespace declaration nodes as much as possible (but watch out for prefix collisions).
- Add most referred to namespace declarations (AddNamespaceDeclarationL(uri,pref)) after
  any other namespace declarations in a element -- they will be found faster in
  namespace lookups.

For more informarion on NULL and NULL nodes:
@see TXmlEngNode
@see KNullDesC8
*/
class TXmlEngElement : public TXmlEngNode
{
  public:
    /** Default constructor */
    inline TXmlEngElement();

    /**
    Constructor
	@param aInternal element pointer
	*/
    inline TXmlEngElement(void* aInternal);
	
    /*
	Extensions to the DOM Level 3 Core methods
	*/

    /**
    Retrieves a list of the attribute nodes of an element

	Upon return, aList is initialized and is ready for use with HasNext() and
	Next() methods:
     
      @code
          ...
          TXmlEngElement root = doc.DocumentElement();
          RXmlEngNodeList<TXmlEngAttr>    attlist;
          root.GetAttributes(attlist);
          while (attlist.HasNext())
              processAttribute(attlist.Next());
          ...
	      attlist.Close();
      @endcode

		If there are no attributes the list will be empty.

    @param aList An attribute list to initialize
    */
    IMPORT_C void GetAttributes(RXmlEngNodeList<TXmlEngAttr>& aList) const;

    /**
    Retrieves a list of child elements of an element.

	Upon return, aList is initialized and is ready for use with HasNext() and
	Next() methods.

	Note: This returns a list of the child element nodes only.

	There is no affect if the element attributes are empty.

	@param aList A child list to initialize
	*/ 
    IMPORT_C void GetChildElements(RXmlEngNodeList<TXmlEngElement>& aList) const;

	/**
	Creates a new attribute node outside of any namespace (i.e. it has no
	prefix), sets the attribute's value and links it as the last attribute of
	the current element.

	Note:
			- No checks are made that an attribute with the same name exists.
              Use this method only on newly created elements!
              Otherwise, use TXmlEngElement::SetAttributeL()
            - Attributes do not inherit default namespace of its element
              (http://w3.org/TR/REC-xml-names/#defaulting)
            - The attribute's value is the second argument in all AddNewAttributeL() methods

    When adding the first attribute, use TXmlEngNode::SetAsFirstSibling() on the attribute:
    @code
        TXmlEngElement el = ... ; // get some element
        el.AddNewAttributeL("version","0.1").SetAsFirstSibling();
    @endcode
      
	 Copies are taken of descripters passed in.
	 
	@see SetAsLastSibling()
	@see MoveBeforeSibling(TXmlEngNode) 
	@see MoveAfterSibling(TXmlEngNode)
	 * 
	@param  aName   A local name of the attribute
	@param  aValue  Value to set for new attribute or KNullDesC8
	@return The created attribute
	@leave KXmlEngErrNullNode The node is NULL
	@leave KXmlEngErrWrongUseOfAPI aName is empty
	@leave - One of the system-wide error codes
	*/
    IMPORT_C TXmlEngAttr AddNewAttributeL(const TDesC8& aName, const TDesC8& aValue);
    
    /**
    Creates a new attribute node and adds it to the element.
    
	The provided namespace declaration is used to set the attribute's namespace.

	Note:  If aNsDef is not defined in some of the attribute's ascendants
	(including this element), then the ReconcileNamespacesL() method must be
	called on this element later.
    
	 Copies are taken of descripters passed in.
	 
	@param  aName   The local name of attribute
	@param  aValue  Value to set for new attribute or KNullDesC8
	@param  aNsDef  Namespace to add to the attribute
	@return The created attribute
	@leave KXmlEngErrNullNode The node is NULL
	@leave KXmlEngErrWrongUseOfAPI aName is empty
	@leave - One of the system-wide error codes
	*/
    IMPORT_C TXmlEngAttr AddNewAttributeL(const TDesC8& aName,
                                    const TDesC8& aValue, 
                                    const TXmlEngNamespace aNsDef);

	/**
	Creates a new attribute for the element. A namespace declaration for the
	attribute namespace is also created.

	Note:  Namespace declarations are reused if possible (no redundant ones are
	created)
    
	 Copies are taken of descripters passed in.
	 
	@param  aName   The local name of attribute
    @param  aValue  Value to set for the new attribute or an empty string
    @param  aNsUri  Namespace URI
    @param  aPrefix  Namespace prefix
    @return The created attribute
	@leave KXmlEngErrNullNode The node is NULL
	@leave KXmlEngErrWrongUseOfAPI aName is empty
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr AddNewAttributeL(const TDesC8& aName, 
                                    const TDesC8& aValue,
                                    const TDesC8& aNsUri, 
                                    const TDesC8& aPrefix);

	/**
	Creates a new attribute node using the namespace of its parent element
	(this element), sets the attribute's value and links it as the last
	attribute of the element

    For more hints how to use it @see AddNewAttributeL(const TDesC8&,const TDesC8&)
           
	Note:
        - No checks are made that an attribute with the same name exists
        - if the namespace of the parent element is default (i.e. bound prefix is KNullDesC8),
          then a temporary prefix will be used and bound to the same namespace URI as the element
          (It is due to the fact that default namespaces do not spread on unprefixed attributes,
          see http://w3.org/TR/REC-xml-names/#defaulting)
    
	 Copies are taken of descripters passed in.
	 @see KNullDesC8
	 
	@param aName Local name of attribute 
    @param aValue Value to set for the new attribute or empty.
    @return The created attribute
	@leave KXmlEngErrNullNode The node is NULL
	@leave KXmlEngErrWrongUseOfAPI aName is empty
	@leave - One of the system-wide error codes
    */
    inline   TXmlEngAttr AddNewAttributeSameNsL(const TDesC8& aName, const TDesC8& aValue);

    /**
    Creates a new attribute using the namespace which is bound to the specified prefix
    
    Use this mothod only for construction of new parts of DOM tree where
    you know for sure that the prefix is bound in the given scope.
    @code
        TXmlEngElement el = parent.AddNewAttributeUsePrefixL("property","ObjName","rdf");
        el.AddNewAttributeUsePrefixL("type", "xs:integer", "rdf");
    @endcode
    
    Otherwise, you should check that the prefix is bound like this example shows:
    @code
        TXmlEngNamespace boundNS = TXmlEngNamespace::LookupByPrefix(thisElement, prefix);
        if (boundNS.NotNull()){
            thisElement.AddNewAttributeUsePrefixL("name", value, prefix);
        }
    @endcode
    
	Note: Use AddNewAttributeNsL(name,value,nsDefNode) as much as you can,
	because it is the most efficient way to create namespaced DOM elements (no
	additional lookups for namespace declarations are required).
     
    @code
        // If namespace with given URI is not in the scope, then it will be declared
        // and bound to "data" prefix.
        TXmlEngNamespace nsDef = elem.FindOrCreateNsDefL("http://../Data", "data");
        elem.AddNewAttributeL("location", "...", nsDef);
        elem.AddNewElementL("child", nsDef).AddNewAttributeL("attr","...value...");
        // the result is
            ...
         <elem xmlns:data="http://../Data" data:location="...">
            <data:child attr="...value..."/>
         </elem>
            ...
        //
    @endcode    

	 Copies are taken of descripters passed in.
	 
	@param  aLocalName   The local name of the attribute
    @param  aValue  Value to set for the new attribute or empty string.
    @param  aPrefix  Namespace prefix for the new attribute
    @return The created attribute
	@leave KXmlEngErrNullNode The node is NULL
	@leave KXmlEngErrWrongUseOfAPI aLocalName is empty
	@leave KErrNoMemory The namespace is not found and the URI is not
	http://www.w3.org/XML/1998/namespace; or there is a memory allocation error
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr AddNewAttributeUsePrefixL(const TDesC8& aLocalName, 
                                             const TDesC8& aValue, 
                                             const TDesC8& aPrefix);

    /**
    Creates new attributes using the namespace in scope, which has the specified URI
      
    Almost the same as AddNewAttributeUsePrefixL() but does lookup by namespace URI

    @see AddNewAttributeUsePrefixL(const TDesC8&,const TDesC8&,const TDesC8&)
    
	 Copies are taken of descripters passed in.
	 @see KNullDesC8
	 
	@param  aLocalName   The local name of the attribute
    @param  aValue  Value to set for new attribute or empty string.
    @param  aNsUri  Namespace URI for new attribute
	@return NULL attribute if namespace declaration is not found OR newly added
	to the end of attribute list attribute of this element.
	@leave KXmlEngErrNullNode The node is NULL
	@leave KXmlEngErrWrongUseOfAPI aLocalName is empty
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr AddNewAttributeWithNsL(const TDesC8& aLocalName, 
                                          const TDesC8& aValue, 
                                          const TDesC8& aNsUri);
                                          
	/**
	Adds attribute to element that will be used as Xml:Id.  Does not check if
	an attribute with the same name exists.

	Note: Call RXmlEngDocument.RegisterXmlIdL(aName,aNsUri) first to register
	existed id's in the document.  If the ids are not registered the ID will
	not be added to the element.
    
	 Copies are taken of descripters passed in.
	 
	@param aLocalName Name of the attribute
	@param aValue Value of the attribute
	@param aNs Namespace of the attribute
	@return Attribute if created. Null attribute if Xml:Id exists
	@leave KXmlEngErrWrongUseOfAPI Element is NULL or attribute doesn't exist
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr AddXmlIdL(const TDesC8& aLocalName,
                                       const TDesC8& aValue,
                                       TXmlEngNamespace aNs = TXmlEngNamespace());
    
	/**
	Adds a child element with no namespace.  This results in adding an element
	with aName and no prefix.
     
	This method is the best for creation of non-namespace based documents
	or document fragments, where no default namespace declared.
    
	It may be used also as a method for adding elements from the default
	namespace, BUT namespace will be assigned ONLY after serialization of the
	current document and parsing it back into a DOM tree!! If you need the new
	element to inherit the default namespace use:

    @code
       ...
       TXmlEngNamespace defns = element.DefaultNamespace();
       TXmlEngElement newEl = element.AddNewElementL("Name",defns);
       ...
    @endcode
    
	It is not recommended that you use an undefined namespace with libxml.  
	If an undefined namespace for the element is truly required, then DO NOT
	USE this method if there is a default namespace in the scope!
	@see AddNamespaceDeclarationL

	Copies are taken of descripters passed in.
	 
	@param aName The name of the element
	@return The created element
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementL(const TDesC8& aName);

	/**
	Creates a new child element with the provided name and namespace
	declaration and adds it as the last child of its parent.
    
	 Copies are taken of descripters passed in.
	 
	@param aLocalName The name of the element
    @param aNsDecl The namespace declaration that must be retrieved from
               one of the ascendant nodes of the new elements (and its prefix
               should not be remapped to another namespace URI for the scope
               of the new element)
    @return Created element node
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementL(const TDesC8& aLocalName, TXmlEngNamespace aNsDecl);

	/**
	Creates a new child element with provided the name, prefix and namespace
	URI and adds it as the last child of its parent.
    
	The new namespace declaration will be attached to the parent (this) element
	and used as the namespace for newly created child element. If such a
	binding already exists (same prefix is bound to same URI), it will be
	reused. If the prefix is already bound to some other namespace URI, it will
	be rebound by the new namespace declaration node.
    
	 Copies are taken of descripters passed in.
	 
	@param aLocalName Name of the element
    @param aNsUri     URI of the element's namespace
    @param aPrefix    Prefix of the element
    @return The created element node 
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name or URI or prefix is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementL(const TDesC8& aLocalName, 
                                     const TDesC8& aNsUri, 
                                     const TDesC8& aPrefix);

	/**
	Creates a child element with the same namespace (and prefix if present) as
	the parent element and adds it as the last child of its parent.
     
	 Copies are taken of descripters passed in.
	 
	@param aLocalName The element's name
    @return The created element
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementSameNsL(const TDesC8& aLocalName);

	/**
	Performs a lookup for the namespace declaration for the specified prefix
	and adds a new child element with the found namespace as the last child of
	its parent.
    
	@pre The prefix is bound

	 Copies are taken of descripters passed in.
	 
	@param aLocalName The element's name
    @param aPrefix    The prefix to use for the search
    @return The created element
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name is not specified
	@leave KErrNoMemory The namespace is not found and the prefix is not "xml" 
	(i.e http://www.w3.org/XML/1998/namespace); or there is a memory allocation 
	error.
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementUsePrefixL(const TDesC8& aLocalName, const TDesC8& aPrefix);

	/**
	Performs a lookup for the namespace declaration for the specified namespace
	URI and adds a new child element with the found namespace.  The new element
	is added as the last child of this element.
    
	 Copies are taken of descripters passed in.
	 
	@pre A namespace with the given URI has been declared
    
	@param aLocalName    The element's name
    @param aNsUri        The namespace of the element
    @return The created element
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name is not specified
	@leave KErrNoMemory The namespace is not found and the URI is not http://www.w3.org/XML/1998/namespace 
	or there is a memory allocation error
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementWithNsL(const TDesC8& aLocalName, const TDesC8& aNsUri);

	/**
	Creates a new child element.  If there is no a prefix binding for the new
	element's namespace, a namespace decaration is created with a generated
	prefix at the specified element.

	The location of the namespace declaration may be controlled with
	aNsDeclTarget:

    @code
        el.AddNewElementAutoPrefixL(tagName,uri,KNullDesC8); // declare on the new element
        el.AddNewElementAutoPrefixL(tagName,uri,el);   // declare on the parent element
        el.AddNewElementAutoPrefixL(tagName,uri,doc.DocumentElement()); // declare on the root element
       ...
    @endcode

	Note:  The farther namespace declaration up in the document tree, the
	longer the namespace declaration lookups take.
    
	 Copies are taken of descripters passed in.
	 
	@param aLocalName    Name of the element to create
	@param aNsUri        Namespace URI of the new element
	@param aNsDeclTarget The element where the namespace declaraton should be placed.
	@return The created element
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI Name or URI is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement AddNewElementAutoPrefixL(const TDesC8& aLocalName, 
                                               const TDesC8& aNsUri, 
                                               TXmlEngElement aNsDeclTarget);

	/**
	Gets element content.  This method may be used in most cases when the element
	has only simple text content (without entity references embedded).  If the
	element's contents is mixed (other types of nodes present), only the contents
	of the first child node is returned (if it is a TXmlEngTextNode node). 
	
	For getting the contents of an element that has contents containing entity
	references, WholeTextValueCopyL() should be used.

	@see TXmlEngNode::WholeTextContentsCopyL()
    
	@return The simple text contents of the element or NULL if there is no text.
    */
    IMPORT_C TPtrC8 Text() const;

    /**
    Adds text as a child of the element to the end of the list of children.

	Note:  There may be several TXmlEngTextNode and TXmlEngEntityReference
	nodes added, depending on the aString value.  For example, if the curernt 
	node has text and attributes, a text node and attribute nodes will
	be added.
     
	 Copies are taken of descripters passed in.
	 
	@param aString Text to be added as the element's content.
	@leave KXmlEngErrNullNode Element is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  AddTextL(const TDesC8& aString);

    /**
    Sets text contents for the element.  Any child nodes are removed.
    This function is the same as TXmlEngNode::SetValueL(TDesC8&)

    @see TXmlEngNode::SetValueL(TDesC8&)
    
	@param aString Text to be set as element's content.
	@leave KXmlEngErrNullNode Element is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  SetTextL(const TDesC8& aString);

    /**
    Sets the text content of the element from an escaped string.
    @see TXmlEngAttr::SetEscapedValueL(TDesC8&)
    
	@param aEscapedString New value
	@leave KXmlEngErrNullNode Element is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  SetEscapedTextL(const TDesC8& aEscapedString);
    
	/**
	Sets the new element value exactly as presented in the string.  Predefined
	entities are not converted into the characters they represent.  Any child
	nodes are removed.     

    @see TXmlEngAttr::SetValueNoEncL(const TDesC8& aNewValue); 
    
	@param aNotEncString New string value
	@leave KXmlEngErrNullNode Element is NULL
	@leave - One of the system-wide error codes
    */    
    IMPORT_C void SetTextNoEncL(const TDesC8& aNotEncString);

	/**
	Appends a new text node with the value exactly as presented in the string.
	Predefined entities are not converted into the characters they represent.
	Existing child nodes are not removed.    

    @see TXmlEngAttr::SetValueNoEncL(const TDesC8& aNewValue); 
    
	 Copies are taken of descripters passed in.
	 
	@param aNotEncString Appended string value
	@leave KXmlEngErrNullNode Element is NULL
	@leave - One of the system-wide error codes
    */  
	IMPORT_C void AppendTextNoEncL(const TDesC8& aNotEncString);   

	/**
	Adds a namespace declaration to the current element: a binding of prefix to namespace URI.
    
	If the same namespace declaration exists (same prefix and URI), a redundant
	namespace declaration will not be created.
    
	Both KNullDesC8 or an empty descriptor may be used for "UNDEFINED URI" 
	and "NO PREFIX" values of arguments.  Please refer to the class
	documentation for more specific information.
	@see TXmlEngElement
	@see KNullDesC8	 

	Note: Undeclaring of default namespace (xmlns="") is supported by
	the SetNoDefaultNamespace() method.    
	@see SetNoDefaulNamespace()
    
	Note:   Adding a namespace declaration that rebinds the prefix mapping (or
	default namespace) used by nodes lower in the tree may damage the document
	tree because references to namespace declarations are not updated.
	However, after serialization, the document will have the desired structure.
	Use this method with care!
    
	Copies are taken of descripters passed in.
	 
	@param aNsUri Namespace URI, KNullDesC8 or an empty descriptor.
	@param aPrefix Namespace prefix, KNullDesC8 or an empty descriptor.
	@return The namespace that was created or found or a NULL namespace if the
	namespace is being undeclared.
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI URI or prefix is not specified
	@leave - One of the system-wide error codes
    */ 
    IMPORT_C TXmlEngNamespace AddNamespaceDeclarationL(const TDesC8& aNsUri, const TDesC8& aPrefix);

    /**
    Adds a default namespace declaration.

1
	This achieves the same result as with AddNamespaceDeclarationL(aNsUri,
	KNullDesC8), but additionally the element's namespace is modified (if it has no
	prefix and there were no default namespace declaration in the scope) to the
	new default one.
     
	 Copies are taken of descripters passed in.
	 @see KNullDesC8
	 
	@param aNsUri   Namespace URI;  KNullDesC8 and empty descriptor are allowed to represent UNDEFINED NAMSPACE
	@return The created namespace declaration (NULL for UNDEFINED NAMESPACE)
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI URI is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNamespace SetDefaultNamespaceL(const TDesC8& aNsUri);

    /**
    Undeclares any default namespace for this element and its descendants.
    
    If there is already a default namespace, a @c xmlns="" namespace
    declaration is added. Otherwise, nothing happens, since elements with no
    prefix in such scope are automaticaly considered outside of any namespace.
    
	The side effect of this method is that the namespace of the current element
	may change from the previous default namespace to a NULL TXmlEngNamespace,
	which is considered an absence of namespace.
    
	If the element has a prefix (i.e. not having default namespace), then the
	only effect for the element is undeclaration of existing default namespace. 
    
    If the element is in the scope of another @c xmlns="" undeclaration, no
    action is taken.
    
	Note: Use AddNamespaceDeclarationL(KNullDesC8,KNullDesC8) to force the creation of a @c
	xmlns="" declaration within the scope of another such declaration
	(otherwise unneccessary/duplicate declarations are not created).
    
	Note: This method should be called on elements before adding children,
	because default namespace undeclaration is not spread into its subtree and
	descendants' default namespaces are not reset to NULL. This should be taken
	into account if later some processing on the subtree occurs.  However,
	after serialization and deserialization, undeclared default namespace will
	affect whole element's subtree correctly.

	 @see KNullDesC8
		 
	@leave KXmlEngErrNullNode Element is NULL
	@leave - One of the system-wide error codes
	*/
	IMPORT_C void SetNoDefaultNamespaceL();

	/**
	Finds the namespace declaration that has a specific prefix in the list of parents for
	this element.
    
	If no prefix is specified (an empty descriptor or KNullDesC8) then it is 
	considered to be a "NO PREFIX" search.  Therefore, if the namespace 
	declaration for "no prefix" is searched, then the default namespace is 
	returned.
    
	Copies are taken of descripters passed in.
	@see KNullDesC8 
	 
	@param aPrefix Namespace prefix.
	@return The namespace, which may be NULL if prefix is not found; NULL result for "no prefix" means that default namespace is undefined.
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNamespace LookupNamespaceByPrefixL(const TDesC8& aPrefix) const;

	/**
	Finds a namespace declaration that has a specific namespace URI in the
	scope for the given node.
     
	KNullDesC8 value of aUri is equivalent to an empty descriptor and means
	"UNDEFINED NAMESPACE".
	For such URI's a NULL namespace handle is always returned even if there is
	a namespace undeclaration, which has an empty descriptor URI (and KNullDesC8 
	prefix).
    
	Hint: Use the returned instance of TXmlEngNamespace as the aNsDef argument
	to an element's methods that create new child elements and attributes. The
	same TXmlEngNamespace may be used on deeper descentants of the reference
	element (and doing this way will generally increase performance of DOM tree
	construction). However, if namespace bindings are not controlled for the
	element's children and the prefix (which is bound to the search namespace)
	is rebound to some other namespace URI, then reusing the namespace may lead
	to unwanted result.
    
	Note: At the moment it is possible to retrieve namespace declaration nodes
	whose prefixes were rebound. Be careful when using returned
	TXmlEngNamespace objects for creation of new elements.
	 
	Copies are taken of descripters passed in.
	 
	@param aUri  Namespace URI, for which the namespace declaration is searched
	@return The namespace declaration that binds the given namespace URI to a
	prefix or sets it as a default namespace
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNamespace LookupNamespaceByUriL(const TDesC8& aUri) const;

	/**
	Retrieves the implicitly declared XML infoset binding of the 'xml' prefix
	to XML's namespace URI: "http://www.w3.org/XML/1998/namespace"
     
    The result should be used for creating attributes beloging to the XML namespace
    (xml:lang, xml:space, xml:id , etc.)
    
	DO NOT USE the methods LookupNamespaceByUriL(const TDesC8&) and
	LookupNamespaceByPrefixL(const TDesC8&) (with the arguments
	"http://www.w3.org/XML/1998/namespace" and "xml" respectively) for
	retrieving the namespace node, since in the case of a memory allocation
	fault, a NULL result is returned (and breaks your program silently)
    
	Note:  Normally the 'xml' prefix is bound to the XML namespace URI in the
	document node, BUT if this element is not a part of the document tree yet,
	the requested namespace declaration WILL BE ADDED to the current node.
	This is the reason why the method may fail in OOM conditions.

	@return The namespace matching the prefix binding
	{xml,"http://www.w3.org/XML/1998/namespace"} in the current document
	@leave KErrNoMemory The element is NULL or there was a memory allocation error
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNamespace TheXMLNamespaceL() const;

	/**
	Get the default namespace for the element.
     
	A NULL TXmlEngNamespace means that an element with no prefix
	has no namespace associated with it because no default namespace was declared or
	the default namespace was undeclared with @c xmlns=""
    
	This function is equivalent to LookupNamespaceByPrefixL(KNullDesC8).
	@see KNullDesC8
	
	@return The default namespace in the scope of the element
	@leave - One of the system-wide error codes
    */
    inline   TXmlEngNamespace DefaultNamespaceL() const;

	/**
	Performs a search for the namespace in the scope of the element. This
	method will create new namespace declaration on the element if such
	namespace is not found.

	Note: Be sure not to use the result of this method for non-descendants of
	the element or in situations when prefix overlapping might occur.
	@see TXmlEngAttr
	@see TXmlEngNamespace
  Please read the documentation for this class:
	@see TXmlEngElement
     
	Copies are taken of descripters passed in.

	@param aNsUri   Namespace to search for
	@param aPrefix  Prefix to use for the new namespace declaration (if it is to be created)
	@return The namespace found or created
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI URI is not specified
	@leave - One of the system-wide error codes
	
	@see LookupNamespacebyUriL
    */
    IMPORT_C TXmlEngNamespace FindOrCreateNsDeclL(const TDesC8& aNsUri, const TDesC8& aPrefix);

	/**
	Performs a search on the element and its ascendants for any namespace
	declaration with a given URI.  Creates a new namespace declaration with a
	(unique) prefix if the search was not successful.		 
	@see AddNamespaceDeclarationL
     
	 Copies are taken of descripters passed in.
	 
	@param aNsUri Namespace to search for
	@return The namespace found or created
	@leave KXmlEngErrNullNode Element is NULL
	@leave KXmlEngErrWrongUseOfAPI URI is not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNamespace FindOrCreateNsDeclL(const TDesC8& aNsUri);

    /**
    Checks whether a prefix has been bound in this element (not in one of its ascendants)
    
    Use this method for preventing prefix-name collision in a element node
     
	 Copies are taken of descripters passed in.
	 
	@param aPrefix Namespace prefix
    @return ETrue if there is already a namespace declaration that uses aPrefix on this element
	@leave - One of the system-wide error codes
    */
    IMPORT_C TBool HasNsDeclarationForPrefixL(const TDesC8& aPrefix) const;
    
    /**
    Copies the element with its attributes, but not child nodes
    
	If the context is preserved (preserveNsContent), then all namespace
	declarations that are in the element are writen to the <element ...> tag.

	@param preserveNsContext Whether the namespace context should be preserved
    @return The copied element
	@leave KXmlEngErrNullNode The element is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngElement ElementCopyNoChildrenL(TBool preserveNsContext) const;
    
    /**
    Specialized version of TXmlEngNode::CopyL()
	@return Deep copy of the element.
	@leave KXmlEngErrNullNode The element is NULL
	@leave - One of the system-wide error codes
    */
    inline   TXmlEngElement CopyL() const;

    /** Resets element's content: all child nodes are removed */
    IMPORT_C void RemoveChildren();

    /** Resets element's attributes */
    IMPORT_C void RemoveAttributes();

    /**
    Resets all namespace declarations made in the element
    
	Note: There can be references to these namespace declarations elsewhere!
	Use ReconcileNamespacesL() to fix that.
	*/
	IMPORT_C void RemoveNamespaceDeclarations();

    /**
    Removes all element contents: child nodes, attributes and namespace declarations
    
  @see RemoveChildren() 
	@see RemoveAttributes()
	@see RemoveNamespaceDeclarations();
	*/
    inline   void ClearElement();

	/**
	Copies attributes from another element.  It may be a very convenient method
	for initializing element with a set of predefined attributes.

	Note: Namespaces of the this element may need to be reconciled if the
	copied attributes belong to any namespace that is not declared on some
	ascendant of this node.  @see ReconcileNamespacesL()

	@param aSrc Source element
	@leave KXmlEngErrNullNode The element is NULL
	@leave - One of the system-wide error codes
    */  
    IMPORT_C void CopyAttributesL(TXmlEngElement aSrc);

	/**
	Recursively copies the children (and all of it's associated information) 
	from another element.  Elements are appended to the current element's 
	children list.
    
	Note: Namespaces of this element may need to be reconciled after copy
	operation 
	@see ReconcileNamespacesL()

	@param aSrc Source element
	@leave KXmlEngErrNullNode The element is NULL
	@leave KXmlEngWrongUseOfAPI The source element is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void CopyChildrenL(TXmlEngElement aSrc);

	/**
	Removes the child element with the given name and namespace URI (if it exists).
	Memory allocated for the element is freed.
     
	@param aLocalName Child element name
    @param aNamespaceUri Child element namespace
	@leave KXmlEngErrNullNode The element is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void RemoveChildElementsL(const TDesC8& aLocalName,const TDesC8& aNamespaceUri);

	/**
	Renames the element with the given name, namespace URI, and namespace prefix.

	@param aLocalName The new element name
	@param aNamespaceUri The new namespace URI
	@param aPrefix The new namespace prefix
	@leave - One of the system-wide error codes
	*/
	IMPORT_C void RenameElementL(const TDesC8& aLocalName, const TDesC8& aNamespaceUri, const TDesC8& aPrefix);

    /* 
	DOM Level 3 Core methods

	Most methods of DOM spec operate with fully-qualified names (QNames)
	of elements and attributes. It is different in this API - all methods
	instead accept prefix and localName parts of QName.
    */
    
    /**
    Returns the value of the attribute with the given name and namespace URI.
     
	@param aLocalName Local name of the attribute
	@param aNamespaceUri Namespace URI of the attribute, or the default namespace if not specified.
	@return The attribute value (for as long as the attribute exists) or 
	NULL if not found.
	@leave - One of the system-wide error codes
    */
    IMPORT_C TPtrC8 AttributeValueL(const TDesC8& aLocalName, 
                                    const TDesC8& aNamespaceUri = KNullDesC8) const;

    /**
    Initializes list of child elements with matching names and namespace URIs.

    Note: This method does not list all descendants of the element, only child elements
	@see KNullDesC8	 
     
	@param aList Node list to be created
    @param aLocalName Element name
    @param aNamespaceUri if specified it sets the Namespace URI, default is KNullDesC8 (no namespace set).
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  GetElementsByTagNameL(RXmlEngNodeList<TXmlEngElement>& aList, 
                                         const TDesC8& aLocalName, 
                                         const TDesC8& aNamespaceUri = KNullDesC8) const;
    
	/**
	Sets the value of the given attribute.  The attribute is created if there
	is no such attribute yet.

	Note: If prefix is not KNullDesC8 (or an empty descriptor), 
	then the namespace URI may not be empty see 
 	http://www.w3.org/TR/REC-xml-names/#ns-decl (Definition #3)
     
	Copies are taken of descripters passed in.
	@see KNullDesC8	 
		 
	@param aLocalName Attribute name
    @param aValue Attribute value
    @param aNamespaceUri Namespace URI - default is KNullDesC8
    @param aPrefix Namespace prefix - default is KNullDesC8
	@leave KXmlEngErrNullNode The element is NULL
	@leave KXmlEngErrWrongUseOfAPI Attribute name not specified
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  SetAttributeL(const TDesC8& aLocalName, 
                                 const TDesC8& aValue, 
                                 const TDesC8& aNamespaceUri = KNullDesC8, 
                                 const TDesC8& aPrefix = KNullDesC8);

	/**
	Removes the attribute with the given name and namespace URI (if it exists).
	Memory allocated for the attribute is freed.

	@see KNullDesC8	 
	 
	@param aLocalName Name of the attribute
	@param aNamespaceUri Attribute namespace URI, default is KNullDesC8
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  RemoveAttributeL(const TDesC8& aLocalName, 
                                    const TDesC8& aNamespaceUri = KNullDesC8);

	/**
	Retrieves an attribute node with a specific namespace by its name.

	@see KNullDesC8	 
		 
	@param aLocalName Name of the attribute
	@param aNamespaceUri Attribute namespace URI, default is KNullDesC8
	@return Attribute node with matching namespace URI and name, NULL if not found.
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr AttributeNodeL(const TDesC8& aLocalName, 
                                  const TDesC8& aNamespaceUri = KNullDesC8) const;

    /**
    Check if the element has an attribute with given parameters.

    This function is the same as AttributeNodeL(uri,name).NotNull().

	@see KNullDesC8	 
     
	@param aLocalName Name of attribute
    @param aNamespaceUri Namespace uri, default is KNullDesC8.
    @return ETrue if the element holds an attribute with such namespace URI and name.
	@leave - One of the system-wide error codes
    */
    inline     TBool HasAttributeL(const TDesC8& aLocalName, 
                                   const TDesC8& aNamespaceUri  = KNullDesC8) const;

	/**
	Adds an attribute to this element.  If an attribute of the same name
	exists, it will be destroyed.
     
	@param aNewAttr The new attribute
	@leave KXmlEngErrNullNode The element or attribute is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void  SetAttributeNodeL(TXmlEngAttr aNewAttr);
};


#include <xml/dom/xmlengelement.inl>

#endif /* XMLENGELEMENT_H */

