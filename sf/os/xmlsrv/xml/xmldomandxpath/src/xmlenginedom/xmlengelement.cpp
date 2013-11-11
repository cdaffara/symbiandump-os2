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
// Methods for element node
//

#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengnamespace.h>
#include <xml/dom/xmlengattr.h>
#include <xml/dom/xmlengnodelist.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include "xmlengdomdefs.h"
#include <xml/utils/xmlengutils.h>
#include <stdapis/libxml2/libxml2_parserinternals.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>
#include "libxml2_tree_private.h"

#define LIBXML_ELEMENT (static_cast<xmlNodePtr>(iInternal))

void ResetNs(xmlNodePtr node,xmlNsPtr ns)
    {
    if(node->ns == ns)
        {
        node->ns = NULL;
        }
    xmlNodePtr child = node->children;
    while (child)
        {
        if(child->type == XML_ELEMENT_NODE)
            {
            ResetNs(child,ns);
            }
        child = child->next;
        }
    xmlAttrPtr attr = node->properties;
    while (attr)
        {
        if(attr->type == XML_ATTRIBUTE_NODE && attr->ns == ns)
            {
            attr->ns = NULL;
            }
        attr = attr->next;
        }
    };

// ---------------------------------------------------------------------------------------------
// Creates new attribute node out of any namespace (i.e. it has no prefix),
// sets attribute's value and links it as the last attribute of the current element
//   
// @param  aName   A local name of attribute
// @param  aValue  Value to set for new attribute or NULL (sets value to "")
// @return         A handler to the newly created attribute node;
//
// For adding attribute as the first one, use TXmlEngNode::SetAsFirstSibling() on the attribute:
// @code
//     TXmlEngElement el = ... ; // get some element
//     el.AddNewAttributeL("version","0.1").SetAsFirstSibling();
// @endcode
//   
//   @see   SetAsLastSibling(), MoveBeforeSibling(TXmlEngNode) and MoveAfterSibling(TXmlEngNode)
//
//   @note   - No checks are made that attribute with such name exists
//             Use this method only on newly created elements!
//             Otherwise, use TXmlEngElement::SetAttributeL(..)
//           - Attributes do not inherit default namespace of its element
//             (http://w3.org/TR/REC-xml-names/#defaulting)
//           - attribute's value is the second argument in all AddNewAttributeL(..) methods
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngElement::AddNewAttributeL(
    const TDesC8& aName,
    const TDesC8& aValue )
    {
    if ( !iInternal )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if ( aName.Length() <= 0 ) 
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    //
    xmlChar* name = xmlCharFromDesC8L(aName);
    CleanupStack::PushL(name);
    xmlChar* value = xmlCharFromDesC8L(aValue);
    xmlAttrPtr attr = xmlNewProp( // NOTE: no checks that such attribute already exists
                            LIBXML_ELEMENT,
                            name,
                            value);
    delete value;
    CleanupStack::PopAndDestroy(name);
    OOM_IF_NULL(attr);
    return TXmlEngNode(attr).AsAttr();
    }

// ---------------------------------------------------------------------------------------------
// Creates new attribute node and add it to the element
//
// Provided handle to namespace declaration is used to set up
// attribute's namespace.
//
// @note If aNsDef is not defined in some of attributes ascendants
//     (including this element), then
//      ReconcileNamespacesL() method must be called on
//      this element later.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr  TXmlEngElement::AddNewAttributeL(
    const TDesC8& aName,
    const TDesC8& aValue,
    TXmlEngNamespace aNsDef)
    {
    TXmlEngAttr attr = AddNewAttributeL(aName, aValue);
    
    _LIT(KAutoPrefix,"_pr%d");
    const TInt KMaxPrefLength = 20;
    TBuf<KMaxPrefLength> prefix;
    
    TUint ind = 0;
    char* prefCh = NULL;
    xmlNsPtr ns;
    
    if (aNsDef.NotNull())
        {
        if (aNsDef.IsDefault())
            {
            // create new temporary namespace binding (with non-NULL prefix)
            ns = NULL;
            while(!ns)
                {
                // generate prefix
                ++ind;
                prefix.Format(KAutoPrefix,ind);
                prefCh = XmlEngXmlCharFromDesL(prefix);
                TXmlEngConstString pref(prefCh);
                delete prefCh;
        		
        		if(!xmlSearchNs(LIBXML_ELEMENT->doc,
	        					LIBXML_ELEMENT,
	        					CAST_DOMSTRING_TO_XMLCHAR(pref)))
        			{
	                ns = xmlNewNs(
	                        LIBXML_ELEMENT,
	                        INTERNAL_NSPTR(aNsDef)->href,
	                        CAST_DOMSTRING_TO_XMLCHAR(pref));
	                TEST_OOM_FLAG;        			
        			}
                }
            }
        else
            {
            ns = INTERNAL_NSPTR(aNsDef);
            }
        INTERNAL_ATTRPTR(attr)->ns = ns;
        }
    return attr;
    }

// ---------------------------------------------------------------------------------------------
// Creates new attribute on the element. Namespace declaration for the attribute namespace is
// created too.
//
// @note
//     - Namespace declarations are reused if possible (no redundant ones are created)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngElement::AddNewAttributeL(
    const TDesC8& aName,
    const TDesC8& aValue,
    const TDesC8& aNsUri,
    const TDesC8& aPrefix )
    {
    TXmlEngAttr attr = AddNewAttributeL(aName, aValue);
    
    if (!aPrefix.Length()) // NULL or "" 	 
        {	  	 
        if (aNsUri.Length()) // !(NULL or "") --> namespace URI is present
            {
            // An attribute cannot have default namespace (Pref = "" AND nsUri!="")
	        WRONG_USE_OF_API;
            }	  	 
        return attr; 	  	  	 
        }
    
    TXmlEngNamespace nsDef = AddNamespaceDeclarationL(aNsUri, aPrefix);
    INTERNAL_ATTRPTR(attr)->ns = INTERNAL_NSPTR(nsDef);
    return attr;
    }

// ---------------------------------------------------------------------------------------------
// Creates new attributes using namespace, which is bound to the specified prefix
//
// Please, use this mothod only for construction of new parts of DOM tree, where
// you know for sure that prefix is bound in the given scope.
// @code
//     TXmlEngElement el = parent.AddNewAttributeUsePrefixL("property","ObjName","rdf");
//     el.AddNewAttributeUsePrefixL("type", "xs:integer", "rdf");
// @endcode
//
// Otherwise, you should check that prefix is bound like this example shows:
// @code
//     TXmlEngNamespace boundNS = TXmlEngNamespace::LookupByPrefix(thisElement, prefix);
//     if (boundNS.NotNull()){
//         thisElement.AddNewAttributeUsePrefixL("name", value, prefix);
//     }
// @endcode
//
// @note
//     Use AddNewAttributeNsL(name,value,nsDefNode) as much as you can, because
//     it is most efficient way to create namespaced DOM elements (no additional
//     lookups for namespace declarations are required).
//   
// @code
//     // If namespace with given URI is not in the scope, then it will be declared
//     // and bound to "data" prefix.
//     TXmlEngNamespace nsDef = elem.FindOrCreateNsDeclL("http://../Data", "data");
//     elem.AddNewAttributeL("location", "...", nsDef);
//     elem.AddNewElementL("child", nsDef).AddNewAttributeL("attr","...value...");
//     // the result is
//        ...
//        <elem xmlns:data="http://../Data" data:location="...">
//           <data:child attr="...value..."/>
//        </elem>
//        ...
//     //
// @endcode    
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngElement::AddNewAttributeUsePrefixL(
    const TDesC8& aLocalName,
    const TDesC8& aValue,
    const TDesC8& aPrefix )
    {
    _LIT8(KXml,"xml");
    TXmlEngAttr attr = AddNewAttributeL(aLocalName, aValue);
    // try to locate namespace binding for the same prefix
    xmlChar* prefix = xmlCharFromDesC8L(aPrefix);
    xmlNsPtr ns = xmlSearchNs(
                    LIBXML_ELEMENT->doc,
                    LIBXML_ELEMENT,
                    prefix);
    delete prefix;                
    
    if (!ns)
        {
        // OOM may happen ONLY for "xml" prefix search
        OOM_IF(!aPrefix.Compare(KXml));
        WRONG_USE_OF_API; // Prefix is not bound
        }
    INTERNAL_ATTRPTR(attr)->ns = ns;
    return attr;
}

// ---------------------------------------------------------------------------------------------
// Creates new attributes using namespace in the scope, which has specified URI
//   
// Almost the same as AddNewAttributeUsePrefixL(...) but does lookup by namespace URI
//
// @return - NULL attribute if namespace declaration is not found OR newly added to the end of
//     attribute list attribute of this element.
//
// @see AddNewAttributeUsePrefixL(TDesC8,TDesC8,TDesC8)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngElement::AddNewAttributeWithNsL(
    const TDesC8& aLocalName,
    const TDesC8& aValue,
    const TDesC8& aNsUri )
    {
    _LIT8(KXmlNs,"http://www.w3.org/XML/1998/namespace");
    // try to locate namespace binding for the same prefix
    xmlChar* nsp = xmlCharFromDesC8L(aNsUri);
    xmlNsPtr ns = xmlSearchNsByHref(
                    LIBXML_ELEMENT->doc,
                    LIBXML_ELEMENT,
                    nsp);
    delete nsp;
    
    if (!ns)
        {
        // OOM may happen ONLY for "xml" prefix search
        OOM_IF(!aNsUri.Compare(KXmlNs));
        TXmlEngAttr attr;
        return attr;
        }
    
    TXmlEngAttr attr = AddNewAttributeL(aLocalName, aValue);
        
    INTERNAL_ATTRPTR(attr)->ns = ns;
    return attr;
    }

// ---------------------------------------------------------------------------------------------
// Adds child element with no namespace
//
// @param aName name of the element
//
// Results in adding element with aName and no prefix.
//
// This method is the best for creation of non-namespace based documents
// or document fragments, where no default namespace declared.
//
// It may be used also as a method for  adding element from default namespace,
// BUT namespace will be assigned ONLY after serialization of the current
// document and parsing it back into a DOM tree!! If you need that default namespace
// was inherited by new element immediately use:
// @code
//     ...
//     TXmlEngNamespace defns = element.DefaultNamespace();
//     TXmlEngElement newEl = element.AddNewElementL("Name",defns);
//     ...
// @endcode
//
// If truly undefined namespace for the element is required, then <b>DO NOT USE</b>
// this method if there is a default namespace in the scope!
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::AddNewElementL(
    const TDesC8& aName )
    {
    if ( !iInternal )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if ( aName.Length() <= 0 ) 
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    xmlChar* name = xmlCharFromDesC8L(aName);
    xmlNodePtr element = xmlNewNode(NULL, name);
    delete name;
    //
    OOM_IF_NULL(element);
    //
    element->parent = LIBXML_ELEMENT;
    element->doc = LIBXML_ELEMENT->doc;
    // Link element as the last child
    xmlNodePtr prev = LIBXML_ELEMENT->last;
    LIBXML_ELEMENT->last = element;
    element->prev = prev;
    if (prev)
        {
        prev->next = element;
        }
    if (!(LIBXML_ELEMENT->children))
        {
        LIBXML_ELEMENT->children = element;
        }
    return TXmlEngElement(element);
    }

// ---------------------------------------------------------------------------------------------
// Creates new child element with provided name, prefix and namespace URI
//
// New namespace declaration will be attached to the parent (this) element and used
// as namespace for newly created child element. If such binding already exists
// (same prefix is bound to same URI), it will be reused. If the prefix is already
// bound to some another namespace URI, it will be rebound by the new namespace
// declaration node.
//
// @param aLocalName Name of the element
// @param aNsUri     URI of element's namespace
// @param aPrefix    Prefix of the element
// @return Created element node (and added as the last child of its parent)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::AddNewElementL(
    const TDesC8& aLocalName,
    const TDesC8& aNsUri,
    const TDesC8& aPrefix )
    {
    TXmlEngElement element = AddNewElementL(aLocalName);
    TXmlEngNamespace nsDef = AddNamespaceDeclarationL(aNsUri, aPrefix);
    INTERNAL_NODEPTR(element)->ns = INTERNAL_NSPTR(nsDef);
    return element;
    }

// ---------------------------------------------------------------------------------------------
// Creates new child element with provided name and namespace declaration
//
// @param aLocalName Name of the element
// @param aNsDef     Handle of the namespace declaration, that must be retrieved from
//                  one of the ascendant nodes of the new elements (and its prefix
//                  should not be remapped to another namespace URI for the scope
//                  of the new element)
// @return    Created element node (and added as the last child of its parent)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement  TXmlEngElement::AddNewElementL(
    const TDesC8& aLocalName,
    TXmlEngNamespace aNsDef )
    {
    TXmlEngElement element = AddNewElementL(aLocalName);
    INTERNAL_NODEPTR(element)->ns = INTERNAL_NSPTR(aNsDef);
    return element;
    }

// ---------------------------------------------------------------------------------------------
// Adds child element with same namespace (and prefix if present) as parent element has
//
// @param aLocalName element's name
// @return New element that was added to the end of children list of its parent (this element)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::AddNewElementSameNsL(
    const TDesC8& aLocalName )
    {
    TXmlEngElement element = AddNewElementL(aLocalName);
    INTERNAL_NODEPTR(element)->ns = LIBXML_ELEMENT->ns;
    return element;
    }

// ---------------------------------------------------------------------------------------------
// Performs lookup for the namespace declaration for specified prefix and
// adds new child element with found namespace.
//
// The assumption is that prefix is bound, otherwise run-time error
// (Symbian's Leave or exception) occurs
//
// @note   Use this method only if there is a binding for the given prefix.
//
// @param aLocalName element's name
// @param aPrefix    prefix to use
// @return new TXmlEngElement that was added to the end of children list of its parent (this element)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::AddNewElementUsePrefixL(
    const TDesC8& aLocalName,
    const TDesC8& aPrefix )
    {
    _LIT8(KXml,"xml");
    TXmlEngElement element = AddNewElementL(aLocalName);
    // Can fail only with XML's namespace
    xmlChar* pref = xmlCharFromDesC8L(aPrefix);
    xmlNsPtr ns = xmlSearchNs(
                    LIBXML_ELEMENT->doc,
                    LIBXML_ELEMENT,
                    pref);
    delete pref;
    //
    OOM_IF(!ns && !aPrefix.Compare(KXml));
    //
    INTERNAL_NODEPTR(element)->ns = ns;
    return element;
    }

// ---------------------------------------------------------------------------------------------
// Performs lookup for the namespace declaration for specified namespace URI and
// adds new child element with found namespace.
//
// The assumption is that namespace with given URI was declared,
// otherwise run-time error (Symbian' Leave or exception) occurs
//
// @note Use this method only if namespace declaration for the provided URI exists.
//
// @param aLocalName element's name
// @param aNsUri     namespace of element
// @return new TXmlEngElement that was added to the end of children list of its parent (this element)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::AddNewElementWithNsL(
    const TDesC8& aLocalName,
    const TDesC8& aNsUri )
    {
    _LIT8(KXmlNs,"http://www.w3.org/XML/1998/namespace");
    TXmlEngElement element = AddNewElementL(aLocalName);
    /*xmlNsPtr ns = xmlSearchNsByHref(
                    LIBXML_ELEMENT->doc,
                    LIBXML_ELEMENT,
                    CAST_DOMSTRING_TO_XMLCHAR(aNsUri));
    */
    TXmlEngNamespace ns = LookupNamespaceByUriL(aNsUri);
    // May fail only with XML's namespace
    OOM_IF(ns.IsNull() && !aNsUri.Compare(KXmlNs));
    //
    INTERNAL_NODEPTR(element)->ns = INTERNAL_NSPTR(ns);
    return element;
    }

// ---------------------------------------------------------------------------------------------
// Creates new child element; if there is no a prefix binding for new element's namespace,
// a namespace decaration is created with generated prefix at specified element.
//
// @param aLocalName    Name of the element to create
// @param aNsUri        Namespace URI of the new element
// @param aNsDeclTarget An element where namespace declaraton should be placed
//                     if there is a needed to create new namespace declaration;
//                     NULL is used to specify the created element itself
//
// As aNsDeclTarget any ascendant of the new node may be provided:
// @code
//     el.AddNewElementAutoPrefixL(tagName,uri,NULL); // declare on the new element
//     el.AddNewElementAutoPrefixL(tagName,uri,el);   // declare on the parent element
//     el.AddNewElementAutoPrefixL(tagName,uri,doc.DocumentElement()); // declare on the root element
//     ...
// @endcode
//
// @note
//     The farther namespace declaration up in the document tree,
//     the longer time namespace declaration lookups take.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::AddNewElementAutoPrefixL(
    const TDesC8& aLocalName,
    const TDesC8& aNsUri,
    TXmlEngElement aNsDeclTarget )
    {
    TXmlEngElement elem = AddNewElementL(aLocalName);
    if (aNsDeclTarget.IsNull())
        aNsDeclTarget = elem;
    TXmlEngNamespace ns = aNsDeclTarget.FindOrCreateNsDeclL(aNsUri);
    INTERNAL_NODEPTR(elem)->ns = INTERNAL_NSPTR(ns);
    return elem;
    }

// ---------------------------------------------------------------------------------------------
// Retrieves list of attribute nodes of the element
//   
// @param aList - a node list object to initialize
//
// Passed by reference list of nodes is initialized and after call to
// Attributes(..) is ready for use with HasNext() and Next() methods:
//
// @code
//     ...
//     TXmlEngElement root = doc.DocumentElement();
//     TXmlEngNodeList<TXmlEngAttr>    attlist;
//     root.GetAttributes(attlist);
//     while (attlist.HasNext())
//           processAttribute(attlist.Next());
//     ...
// @endcode
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::GetAttributes(
    RXmlEngNodeList<TXmlEngAttr>& aList ) const
    {
    //pjj18
    aList.Open( LIBXML_ELEMENT->properties,
                  TXmlEngNode::EAttribute);
    }

// ---------------------------------------------------------------------------------------------
// Retrieves list of child elements of the element
//
// @param aList - a node list object to initialize
//
// Passed by reference list of nodes is initialized and after the call
// it is ready for use with HasNext() and Next() methods:
//   
// @note Returned list is a "filtered view" of the underlying
//     list of all element's children (with text nodes, comments
//     processing instructions, etc.)
// ---------------------------------------------------------------------------------------------
//  
EXPORT_C void TXmlEngElement::GetChildElements(
    RXmlEngNodeList<TXmlEngElement>& aList ) const
    {
    //pjj18
    aList.Open( LIBXML_ELEMENT->children,
                  TXmlEngNode::EElement);
    }

// ---------------------------------------------------------------------------------------------
// @return Basic contents of the element
//
// This method may be used in most cases, when element has only simple text content
// (without entity references embedded)
//
// If element's contents is mixed (other types of nodes present), only contents of
// first child node is returned if it is a TXmlEngTextNode node. For getting mixed contents of the
// element of contents with entity references, WholeTextValueCopyL() should be used.
//
// @see TXmlEngNode::WholeTextContentsCopyL()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngElement::Text() const
    {
    if (LIBXML_ELEMENT && LIBXML_ELEMENT->children && LIBXML_ELEMENT->children->type == XML_TEXT_NODE)
        {
        return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_ELEMENT->children->content)).PtrC8();
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------------------------
// Adds text as a child of the element.
//
// At the moment, content may be only added, replaced (set) and read from element.
//
// @param aString text to be added as element's content.
//
// @note There may be several TXmlEngTextNode and TXmlEngEntityReference nodes added actually,
//      depending on the aString value
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::AddTextL(
    const TDesC8& aString )
    {
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    xmlNodePtr last, newNode, tmp;
    last = LIBXML_ELEMENT->last;
    xmlChar* text = xmlCharFromDesC8L(aString);
    newNode = xmlNewTextLen(NULL, aString.Size());
    if(!newNode)
        {
        delete text;
        OOM_HAPPENED;
        }
    newNode->content = text;

    tmp = xmlAddChild(LIBXML_ELEMENT, newNode);
    if (xmlOOMFlag())
        {
        xmlFreeNode(newNode);
        OOM_HAPPENED;
        }

    if (tmp == newNode &&
        last && last->next == newNode)
        {
        OOM_IF_NULL(xmlTextMerge(last, newNode));
        }
    }

// ---------------------------------------------------------------------------------------------
// Ad Xml:Id attribute to the element
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngElement::AddXmlIdL(const TDesC8& aLocalName,
                                   const TDesC8& aValue,
                                   TXmlEngNamespace aNs)
    {
    if(IsNull())
    	{
        User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    
    TXmlEngAttr attr;
    xmlChar* value = xmlCharFromDesC8L(aValue);
    xmlAttrPtr tmp = xmlGetID(LIBXML_ELEMENT->doc, value);
    if(tmp)
        {
        delete value;
        return attr;
        }
    
    attr = AddNewAttributeL(aLocalName, aValue);
    if(aNs.NotNull())
        {
        TXmlEngNamespace tmp = LookupNamespaceByPrefixL(aNs.Prefix());
        if(!tmp.IsSameNode(aNs))
            {
            attr.Remove();
            delete value;
            return attr;
            }
        INTERNAL_ATTRPTR(attr)->ns = INTERNAL_NSPTR(aNs);
        }
    
    if(attr.IsNull())
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
        
    if(!xmlAddID(NULL, LIBXML_ELEMENT->doc, value, INTERNAL_ATTRPTR(attr)))
        {
        delete value;
        attr.Remove();
        XmlEngOOMTestL();
        }
    delete value;
    return attr;
    }

// ---------------------------------------------------------------------------------------------
// Sets text contents for the element.
// Any child nodes are removed.
// Same as TXmlEngNode::SetValueL(const TDesC8&)
//
// @see TXmlEngNode::SetValueL(const TDesC8&)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::SetTextL(const TDesC8& aString)
    {
    AsAttr().SetValueL(aString);
    }

// ---------------------------------------------------------------------------------------------
// Sets text content of the element from escaped string.
// @see TXmlEngAttr::SetEscapedValueL(const TDesC8&)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::SetEscapedTextL(const TDesC8& aEscapedString)
    {
    AsAttr().SetEscapedValueL(aEscapedString);
    }


// -------------------------------------------------------------------------------------
// Sets new element value exactly as presented in the string.
// Predefined entities are not converted into characters they represent.
// Any child nodes are removed.
// -------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::SetTextNoEncL(const TDesC8& aNotEncString)
	{
    AsAttr().SetValueNoEncL(aNotEncString);	
	}

// -------------------------------------------------------------------------------------
// Appends new text node with the value exactly as presented in the string.
// Predefined entities are not converted into characters they represent.
// Child nodes are not removed.  
// -------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::AppendTextNoEncL(const TDesC8& aNotEncString)
	{
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}

    xmlNodePtr prop = LIBXML_ELEMENT;
    xmlNodePtr new_ch;

	if (aNotEncString.Length())
        {
		xmlChar* value = xmlCharFromDesC8L(aNotEncString);
        new_ch = xmlNewText(NULL);
        if(!new_ch)
            {
            delete value;
            OOM_HAPPENED;
            };

        new_ch->name = xmlStringTextNoenc;
        new_ch->content = value;
        new_ch->parent = (xmlNodePtr) prop;
       	xmlNodePtr last = prop->last;
       	if(last)
       		{
       		last->next = new_ch;
       		new_ch->prev = last;
       		}
       	else
       		{
       		prop->children = new_ch;
       		}
       	prop->last = new_ch;
        }   
	}
	
	
// ---------------------------------------------------------------------------------------------
// Adds default namespace declaration.
//
// @param aNsUri   Namespace URI;  both NULL and "" (empty string) are allowed to represent UNDEFINED NAMSPACE
//
// Same result as with AddNamespaceDeclarationL(aNsUri, NULL), but additionally
// element's namespace modified (if it has no prefix and there were no default
// namespace declaration in the scope) to the new default one.
//
// @return Handle to the created namespace declaration (NULL for UNDEFINED NAMESPACE)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::SetDefaultNamespaceL(
    const TDesC8& aNsUri)
    {
    TXmlEngNamespace nsDef = AddNamespaceDeclarationL(aNsUri,KNullDesC8);
    if (!LIBXML_ELEMENT->ns)
        LIBXML_ELEMENT->ns = INTERNAL_NSPTR(nsDef);
    return nsDef;
    }

// ---------------------------------------------------------------------------------------------
// Undeclares any default namespace for current element and its descendants.
//
// If there is already some default namespace,  <i>xmlns=""</i> namespace
// declaration is added. Otherwise, nothing happens, since element with no
// prefix in such scope is automaticaly considered as out of any namespace.
//
// The side effect of this method is that namespace of the current element
// may change from previous <b>default</b> namespace to NULL TXmlEngNamespace, which is
// considered an absence of namespace.
//
// If the element has prefix (i.e. not having default namespace), 
// then the only effect for the element is undeclaration of existing default namespace. 
//
// If element is in the scope of another <i>xmlns=""</i> undeclaration, no
// actions are taken.
//
// @note
//     Use AddNamespaceDeclarationL(NULL,NULL) to force creation of
//     xmlns=""  declaration within scope of another such declaration 
//     (otherwise unneccessary/duplicate declarations are not created)
//
// @note
//     This method should be called on elements before adding children,
//     because default namespace undeclaration is not spread into its subtree and 
//     descedants' default namespaces are not reset to NULL. This should be taken into 
//     account if later some processing on the subtree occurs.
//     However, after serialization and deserialization, undeclared default namespace will
//     affect whole element's subtree correctly.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::SetNoDefaultNamespaceL()
    {
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    TXmlEngNamespace defns = LookupNamespaceByPrefixL(KNullDesC8);
    if (defns.IsUndefined()) // NULL or Uri is NULL
        {
        // we are already in the scope of xmlns="" , so do nothing
        return;
        }

    // There is some default namespace in the scope.
    // Add local "undefined" namespace and change element's namespace
    // if it is the default one
    AddNamespaceDeclarationL(KNullDesC8, KNullDesC8);
    TXmlEngNamespace myNs(LIBXML_ELEMENT->ns);
    if (myNs.IsDefault())
        {
        // replace default namespace with undefined
        LIBXML_ELEMENT->ns = NULL; // NULL means "NO NAMESPACE"(I.E. "UNDEFINED NAMESPACE")
        }
    }

// ---------------------------------------------------------------------------------------------
// Finds namespace declaration that has specific prefix in the scope for given node
//
// Prefix "" or NULL are considered the same, meaning "<b>NO PREFIX</b>".
// If namespace declaration for "no prefix" is searched, then default namespace is returned. 
//
// @return Namespace handler, which may be NULL if prefix is not bound.
//
// NULL result for "no prefix" means that default namespace is undefined.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::LookupNamespaceByPrefixL(
    const TDesC8& aPrefix ) const
    {
    // OOM may happen ONLY during a lookup for {"xml","http://www.w3.org/XML/1998/namespace"}

    xmlChar* pref = xmlCharFromDesC8L(aPrefix);
    // Allow "" instead of NULL
    if(pref && !*pref)
        {
        delete pref;
        pref = NULL;
        }
        
    xmlNsPtr ns = xmlSearchNs(
                        LIBXML_ELEMENT->doc,
                        LIBXML_ELEMENT,
                        pref
                );
    delete pref;
    // Default namespace undeclarations are never returned  
    if (ns && ns->href && !*ns->href)
        ns = NULL;
    return TXmlEngNamespace(ns);
    }

// ---------------------------------------------------------------------------------------------
// Finds namespace declaration that has specific namespace URI
// in the scope for the given node.
//
// @param aUri  Namespace URI, for which namespace declaration is searched
// @return Handler to the namespace declaration that binds given namespace URI to some prefix 
//         or NULL if the binding is not active at the scope of this element.
//
// NULL value of aUri is equivalent to "" and means "<b>UNDEFINED NAMESPACE</b>". 
// For such URI  a NULL namespace handle is always returned even if there is
// namespace undeclaration, which has "" URI (and NULL prefix).
//
// <b>Hint:</b><p>
// Use returned instance of TXmlEngNamespace as aNsDef argument to element's methods
// that create new element's child elements and attributes. The same handler
// may be used on more deep descentants of the reference element (and doing
// this way will generally increase performance of DOM tree construction).<br />
// <span class="color:red;">However</span>, if namespace bindings are not controlled
// for element's children and prefix, which is bound to the search namespace, is
// rebound to some other namespace URI, then reusing namespace may lead to
// unwanted result.
//
// Consider an example:
// @code
//     TXmlEngElement root = doc.DocumentElement();
//     TXmlEngNamespace targetNs = root.AddNamespaceDeclarationL("http://example.com/","ex");
//     TXmlEngElement el_1 = root.AddNewElementL("outer", targetNs);
//     TXmlEngElement el_2 = el_1.AddNewElementL("inner"); // element without prefix
//
//     // NOTE: prefix "ex" is not bound to "http://example.com/" anymore
//     el_2.AddNamespaceDeclarationL("http://whatever.com/","ex");
//     TXmlEngElement el_3 = el_2.AddNewElementL("problem", targetNs);
//     ...
// @endcode
//
// The sought result was (showing expanded names of elements):
// @code
//     --> "root"
//         --> {"http://example.com/","outer"}
//         --> "inner"
//             -->{"http://example.com/","problem"}
//                 ...
//             <--
//         <-- "inner"
//         <-- {"http://example.com/","outer"}
//         ...
//     <-- </root>
// @endcode
// and it may look that it has been achieved. Indeed, if namespace of element "problem"
// was queried, it would have URI "http://example.com/" and prefix "ex".
// However, if namespace URI was looked up by "problem"'s prefix, it would be
// "http://whatever.com/". We have created illegal DOM tree.
//
// The actual DOM tree in serialized form will be:
// @code
//     <root>
//         <ex:outer xmlns:ex="http://example.com/">
//            <inner xmlns:ex="http://example.com/">
//                 <ex:problem>
//                 ...
//                 </ex:problem>
//            </inner>
//         </ex:outer>
//        ...
//     </root>
// @endcode
//
// So, reuse of namespace handlers should be performed with special care.
//
// @note
//     At the moment it is possible to retrieve namespace declaration nodes
//     whose prefixes were rebound. Be careful when use returned TXmlEngNamespace object
//     for creation of new elements. In later releases, this method will perform
//     safe lookup. And so far, it is better to make check that prefix of returned
//     namespace declaration has not rebound:
// @code
//     TXmlEngNamespace ns = element.LookupNamespaceByUri("a_uri");
//     if (element.LookupNamespaceByPrefix(ns.Prefix()).IsSameNode(ns)){
//         ... // now it is safe to create new elements by using "ns"
//         element.AddNewElementL("product",ns);
//         ...
//     }
// @endcode
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::LookupNamespaceByUriL(
    const TDesC8& aUri ) const
    {
    // OOM may happen ONLY during a lookup for {"xml","http://www.w3.org/XML/1998/namespace"}

    xmlChar* uri = xmlCharFromDesC8L(aUri);

	if(!uri || !*uri)
		{
		delete uri;
		return NULL;
		}
	else
		{
        TXmlEngNamespace ns = xmlSearchNsByHref(
                            LIBXML_ELEMENT->doc,
                            LIBXML_ELEMENT,
                            uri);	
        delete uri;	
		if(LookupNamespaceByPrefixL(ns.Prefix()).IsSameNode(ns))
			{
			return ns;
			}
		else
			{
			return NULL; //the binding is not active at the scope of this element
			}
		}
    }

// ---------------------------------------------------------------------------------------------
// Retrieves implicitly declared on every XML infoset binding
// of 'xml' prefix to XML's namespace URI:
// "http://www.w3.org/XML/1998/namespace"
//
// @return Handler to {xml,"http://www.w3.org/XML/1998/namespace"} prefix
//            binding in the current document
//
// The result should be used for creating attributes beloging to the XML namespace
// (xml:lang, xml:space, xml:id , etc.)
//
// DO NOT USE methods LookupNamespaceByUri(TDesC8) and LookupNamespaceByPrefix(TDesC8)
// (with "http://www.w3.org/XML/1998/namespace" and "xml" arguments) for retrieving
// namespace node, since in a case of [possible] memory allocation fault
// NULL result is returned (and breaks your program silently)
//
// @note   Normally 'xml' prefix is bound to XML namespace URI in the document
//         node, BUT if current node is not a part of the document tree yet,
//         the requested namespace declaration WILL BE ADDED to the current node.
//         This is the reason why the method may fail in OOM conditions.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::TheXMLNamespaceL() const
    {
    xmlNsPtr ns = xmlSearchNs(
                        LIBXML_ELEMENT->doc,
                        LIBXML_ELEMENT,
                        BAD_CAST "xml");
    OOM_IF_NULL(ns);
    return ns;
    }

// ---------------------------------------------------------------------------------------------
// Performs search of namespace handler in the scope of the element. This method will
// create new namespace declaration on the element if such namespace is not available.
//
// @param aNsUri   Searched namespace
// @param aPrefix  Prefix to use for <b>new</b> namespace declaration (if it is to be created)
//
// @return    TXmlEngNamespace handler that may be used to create new attributes and child elements of
//             the element. The namespace may be one of those existed previously or was created
//
// @see LookupNamespacebyUriL
// @note
//     Be sure not to use the result of this method for non-descendants of the element or in situations
//     when prefix overlapping might occur (read also about general considerations of attributes
//     and elements creation using namespace handlers)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::FindOrCreateNsDeclL(
    const TDesC8& aNsUri,
    const TDesC8& aPrefix )
    {
    TXmlEngNamespace ns = LookupNamespaceByUriL(aNsUri);
    if (aNsUri.Length() && ns.IsNull())
        {
        // Some (not the undefined one) namespace was not declared yet 
        ns = AddNamespaceDeclarationL(aNsUri, aPrefix);
        }
    else
        {
        // There is a namespacedeclaration,
        // check that it is not "shielded" (its prefix is not rebound)
        if (! LookupNamespaceByPrefixL(ns.Prefix()).IsSameNode(ns))
            {
            // it was rebound, so we rebound it again
            ns = AddNamespaceDeclarationL(aNsUri, aPrefix);
            }
        }
    return ns;
}

// ---------------------------------------------------------------------------------------------
// Performs search on the element and its ascendants for any namespace declaration
// with given URI and create a new namespace declaration with some (unique) prefix
// if the search was not successful.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::FindOrCreateNsDeclL(
    const TDesC8& aNsUri )
    {
    _LIT8(KAutoPrefix,"_pr%d");
    const TInt KMaxPrefLength = 20;
    TBuf8<KMaxPrefLength> prefix;
    
    TXmlEngNamespace ns = LookupNamespaceByUriL(aNsUri);
    if (ns.NotNull())
        return ns;

    TBool ready = false;
    TUint ind = 0;
    
    while(!ready)
        {
        // generate prefix
        ++ind;
        prefix.Format(KAutoPrefix,ind);
                
        // check
        ns = LookupNamespaceByPrefixL(prefix);
        ready = ns.IsNull();
        }
    return AddNamespaceDeclarationL(aNsUri, prefix);
    }

// ---------------------------------------------------------------------------------------------
// Adds namespace declaration to the current element, a binding of prefix to namespace URI.
//
// If same namespace declaration exists (same prefix and URI), redundant namespace declaration
// will not be created.
//
// Both NULL or "" (empty string) may be used for "UNDEFINED URI" and "NO PREFIX" values of arguments. 
//
// @return  A handle to the created (or found, if there is such) namespace declaration node.
//          If namespace undeclaration is being created, NULL handle is returned -- it can be
//          used in node-creation methods that take namespace handle as an argument.
//
// @note   Undeclaring of default namespace (xmlns="") is supported by
//         SetNoDefaultNamespace() method
//
// @see SetNoDefaulNamespace()
//
// @note   By adding namespace declaration that rebinds prefix mapping (or default namespace)
//         used by nodes lower in the tree, document tree may become
//         wrongly constructed, because references to namespace declaration are
//         not updated. However, after serialization the document will have
//         desired structure.
//         Use this method with care!
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngElement::AddNamespaceDeclarationL(
    const TDesC8& aNsUri,
    const TDesC8& aPrefix )
    {
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if(!aNsUri.Length() && aPrefix.Length())
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
        
    // Now we allow NULL instead of ""
    xmlChar* nsUri = NULL;
    if(aNsUri.Length())
        {
        nsUri = xmlCharFromDesC8L(aNsUri);
        }
    else
        {
        nsUri = (xmlChar*)new(ELeave) TUint8[1];
        *(Mem::Copy((TAny*)nsUri, aNsUri.Ptr(), aNsUri.Length())) = 0;
        }
        
    CleanupStack::PushL(nsUri);
    
    // Now we allow "" instead of NULL
    xmlChar* pref = NULL;
    if(aPrefix.Length())
        {
        pref = xmlCharFromDesC8L(aPrefix);
        }
    
    xmlNsPtr ns = xmlNewNs(
                    LIBXML_ELEMENT,
                    nsUri,
                    pref);
    
    delete pref;                
    CleanupStack::PopAndDestroy(nsUri);
    
    TEST_OOM_FLAG;
    if (!ns)
        {
        // OOM or Already exists
        if (HasNsDeclarationForPrefixL(aPrefix))
            {
            return FindOrCreateNsDeclL(aNsUri); // unique prefix will be generated
            }
        }
    return TXmlEngNamespace(ns);
    }

// ---------------------------------------------------------------------------------------------
// Checks whether a prefix has been bound in this element (not in one of its ascendants)
//
// Use this method for preventig prefix-name collision in a element node
//
// @return TRUE if there is already namespace declaration that uses aPrefix on this element
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngElement::HasNsDeclarationForPrefixL(
    const TDesC8& aPrefix ) const
    {
    xmlChar* prefix;
    if(!aPrefix.Length())
        {
        prefix = NULL;
        }
    else
        {
        prefix = xmlCharFromDesC8L(aPrefix);
        }
    // Convert "" into NULL
    TXmlEngConstString pref((char*) prefix);
    xmlNsPtr ns = LIBXML_ELEMENT->nsDef;
    while (ns)
        if(pref.Equals((char*)ns->prefix))
            {
            delete prefix;
            return true;
            }            
        else
            ns = ns->next;
    delete prefix;
    return false;
    }

// ---------------------------------------------------------------------------------------------
// Copies the element with its attributes, but not child nodes
//
// If context is preserved, then all namespace declarations that are in the element are
// writen to element's start tag too.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement TXmlEngElement::ElementCopyNoChildrenL(
    TBool aPreserveNsContext ) const
    {
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    xmlNodePtr ncopy = xmlStaticCopyNode(LIBXML_ELEMENT, NULL, NULL, 0 /* shallow copy */);
    if (!ncopy) // for a copy of element node, it's not neccessary to check OOM flag
        {
        OOM_HAPPENED;
        }
    // OOM will be checked in the very end of methods
    if (LIBXML_ELEMENT->properties)
        {
        ncopy->properties = xmlCopyPropList(ncopy, LIBXML_ELEMENT->properties);
        }
    if (aPreserveNsContext)
        {
        if (LIBXML_ELEMENT->nsDef)
            {
            ncopy->nsDef = xmlCopyNamespaceList(LIBXML_ELEMENT->nsDef);
            }
        }
    // DONE: OOM: check OOM flag
    if (xmlOOMFlag())
        {
        xmlFreeNode(ncopy);
        OOM_HAPPENED;
        }
    TXmlEngElement el(ncopy);
    OwnerDocument().TakeOwnership(el);
    return el;
    }

// ---------------------------------------------------------------------------------------------
// Resets element's content: all child nodes are removed
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::RemoveChildren()
    {
    //
    if(!LIBXML_ELEMENT)
        {
        return;
        }
    xmlFreeNodeList(LIBXML_ELEMENT->children);
    LIBXML_ELEMENT->children = NULL;
    LIBXML_ELEMENT->last = NULL;
    }

// ---------------------------------------------------------------------------------------------
// Resets element's attributes
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::RemoveAttributes()
    {
    if(!LIBXML_ELEMENT)
        {
        return;
        }
    //
    xmlFreePropList(LIBXML_ELEMENT->properties);
    LIBXML_ELEMENT->properties = NULL;
    }

// ---------------------------------------------------------------------------------------------
// Resets all namespace declarations made in the element
//
// @note There can be references to these namespace declaration from elsewhere!
//       Use ReconcileNamespacesL() to fix that.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::RemoveNamespaceDeclarations()
    {
    if (!LIBXML_ELEMENT)
        {
        return;
        }
    xmlNsPtr ns = LIBXML_ELEMENT->nsDef;
    while(ns)
        {
        ResetNs(LIBXML_NODE,ns);
        ns = ns->next;
        }
    
    xmlFreeNsList(LIBXML_ELEMENT->nsDef);
    LIBXML_ELEMENT->nsDef = NULL;
    }

// ---------------------------------------------------------------------------------------------
// Copies attributes from another element
//
// It may be a very convenient method for initializing element with a set of predefined attributes.
//
// @note
//     Namespaces of the this element may need to be reconciled if copied attributes
//     belong to any namespace that is not declared on some ascendant of this node.
//
// @see ReconcileNamespacesL()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::CopyAttributesL(
    TXmlEngElement aSrc )
    {
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    if (INTERNAL_NODEPTR(aSrc)->properties)
        {
        xmlAttrPtr plist = xmlCopyPropList(LIBXML_ELEMENT, INTERNAL_NODEPTR(aSrc)->properties);
        OOM_IF_NULL(plist);

        if (LIBXML_ELEMENT->properties)
            {
            xmlAttrPtr last = LIBXML_ELEMENT->properties;
            while (last->next)
                {
                last = last->next;
                }
            last->next = plist;
            }
        else
            {
            LIBXML_ELEMENT->properties = plist;
            }
        }
    }

// ---------------------------------------------------------------------------------------------
// Copies a list of elements.
//
// Elements are appended to the element's children list.
//
// @note Namespaces of the this element may need to be reconciled after copy operation
// @see  ReconcileNamespacesL()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::CopyChildrenL(
    TXmlEngElement aSrc )
    {
    if ( !LIBXML_ELEMENT )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if ( aSrc.IsNull() )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    if (INTERNAL_NODEPTR(aSrc)->children)
        {
        xmlNodePtr plist = xmlStaticCopyNodeList(
                                INTERNAL_NODEPTR(aSrc)->children,
                                LIBXML_ELEMENT->doc,
                                LIBXML_ELEMENT);
        OOM_IF_NULL(plist);
        xmlNodePtr last = plist;
        while (last->next)
            {
            last = last->next;
            }
        if (LIBXML_ELEMENT->children)
            {
            LIBXML_ELEMENT->last->next = plist;
            plist->prev = LIBXML_ELEMENT->last;
            }
        else
            {
            LIBXML_ELEMENT->children = plist;
            }
        LIBXML_ELEMENT->last = last;
        }
    }

// ---------------------------------------------------------------------------------------------
// Removes attribute with given name and namespace URI(if such exists).
// Memory allocated for the attribute is freed.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::RemoveChildElementsL(
    const TDesC8& aLocalName,
    const TDesC8& aNamespaceUri )
    {
    if (!LIBXML_ELEMENT)
        {
        return;
        }
    //pjj18
    RXmlEngNodeList<TXmlEngElement> eList;
    
    eList.OpenL(
            LIBXML_ELEMENT->children,
            TXmlEngNode::EElement,
            aLocalName,
            aNamespaceUri);

    TXmlEngElement el;
    while ((el = eList.Next()).NotNull())
        el.Remove();
    eList.Close();
    }

// ---------------------------------------------------------------------------------------------
// Renames the element.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::RenameElementL(
	const TDesC8& aLocalName, 
	const TDesC8& aNamespaceUri, 
	const TDesC8& aPrefix)
	{

        xmlNodePtr element = INTERNAL_NODEPTR(*this);

    	XE_ASSERT_DEBUG( element->type == XML_ELEMENT_NODE );

       	const xmlChar* oldName = element->name;
       	
       	XE_ASSERT_ALWAYS(aLocalName.Length());
       	
       	element->name = xmlCharFromDesC8L(aLocalName);
       	
       	xmlFree((void*)oldName );
       	
        if (aNamespaceUri.Length() || aPrefix.Length())
            {
            TXmlEngNamespace ns = (*this).FindOrCreateNsDeclL(aNamespaceUri, aPrefix);
            element->ns = INTERNAL_NSPTR(ns);
            }
         
	}

// ========================================================================================

// DOM Level 3 Core


// ---------------------------------------------------------------------------------------------
// Links attribute into tree
//
// The replaced attribute node is not returned and just deleted
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::SetAttributeNodeL(
    TXmlEngAttr aNewAttr )
    {
    AppendChildL(aNewAttr);
    xmlReconciliateNs(LIBXML_ELEMENT->doc, LIBXML_NODE);
    }

// ---------------------------------------------------------------------------------------------
// Returns value of attribute with given name and namespace URI
//  
// @param aLocalName       - local name of attribute node
// @param aNamespaceUri    - namespace URI of attribute
// ---------------------------------------------------------------------------------------------
//   
EXPORT_C TPtrC8 TXmlEngElement::AttributeValueL(
    const TDesC8& aLocalName,
    const TDesC8& aNamespaceUri ) const
    {
    TXmlEngAttr attr = AttributeNodeL(aLocalName, aNamespaceUri);
    if( attr.IsNull() )
    	return KNullDesC8();
    else
    	return attr.Value();
    }

// ---------------------------------------------------------------------------------------------
// Sets value of attribute; attribute is created if there is no such attribute yet
//
// @note
//     If prefix is not NULL (or ""), then namespace URI may not be empty
//     see http://www.w3.org/TR/REC-xml-names/#ns-decl (Definition #3)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::SetAttributeL(
    const TDesC8& aLocalName,
    const TDesC8& aValue,
    const TDesC8& aNamespaceUri,
    const TDesC8& aPrefix )
    {
    if ( aLocalName.Length() <= 0 )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}

    TXmlEngAttr attr = AttributeNodeL(aLocalName, aNamespaceUri);
    if (attr.NotNull())
        {
        attr.SetValueL(aValue);
        }
    else
        {
        AddNewAttributeL(aLocalName, aValue, aNamespaceUri, aPrefix);
        }
    }

// ---------------------------------------------------------------------------------------------
// Removes attribute with given name and namespace URI(if such exists).
// Memory allocated for the attribute is freed.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::RemoveAttributeL(
    const TDesC8& aLocalName,
    const TDesC8& aNamespaceUri )
    {
    TXmlEngAttr attr = AttributeNodeL(aLocalName, aNamespaceUri);
    xmlRemoveProp(INTERNAL_ATTRPTR(attr));
    }

// ---------------------------------------------------------------------------------------------
// Retrieves attribute node from specific namespace by its name.
// 
// @return Attribute node with matching namespace URI and name
// @note Use "" (empty string) for namespace name, not NULL (for <i>undefined namespace</i>)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngElement::AttributeNodeL(
    const TDesC8& aLocalName,
    const TDesC8& aNamespaceUri ) const
    {
    //pjj18
    RXmlEngNodeList<TXmlEngAttr> attList;
        
    attList.OpenL(
            LIBXML_ELEMENT->properties,
            TXmlEngNode::EAttribute,
            aLocalName,
            aNamespaceUri);
    TXmlEngAttr attr = attList.Next();
    attList.Close();
    return attr;
    }

// ---------------------------------------------------------------------------------------------
// Initializes list of child elements with matching name and namespace URI.
//
// @note This method does not lists all descedants of the element, only child elements
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngElement::GetElementsByTagNameL(
    RXmlEngNodeList<TXmlEngElement>& aList,
    const TDesC8& aLocalName,
    const TDesC8& aNamespaceUri ) const
    {
    aList.OpenL(
        LIBXML_ELEMENT->children,
        TXmlEngNode::EElement,
        aLocalName,
        aNamespaceUri);
    }

