// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// XPath configuraion functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHCONFIGURATION_H
#define XMLENGXPATHCONFIGURATION_H

#include <xml/dom/xmlengxpathextensionfunction.h>

/**
This class holds a name, a namespace URI and a pointer to a XPath extension
function (either native or defined via MXmlEngXPathExtensionFunction interface).

@see XmlEngXPathConfiguration
@see MXmlEngXPathExtensionFunction
*/
class TXmlEngExtensionFunctionDescriptor
    {
public:
	/** Default constructor */
	IMPORT_C TXmlEngExtensionFunctionDescriptor();

public:
	/** Extension function (Pointer to MXmlEngXPathExtensionFunction) */
    void* iFunc;
	/** Function name */
    const char* iName;
	/** Second name */
    const char* iNamespaceUri;     
	/** Reserved */
	TInt iReserved;
    };

/**
This object is used to support extension functions used within the XPath library.
Extension functions may be added or removed.  This object can also determine whether
an extension function is supported.

Extension functions are stored in a TXmlEngExtensionFunctionDescriptor object which
holds the extension function (a MXmlEngXPathExtensionFunction instance), the
function name and the function namespace.  An extension function can be added
to the library using XmlEngXPathConfiguration::AddExtensionFunctionL().

When an extension function is to be evaluated,
MXmlEngXPathExtensionFunction::Evaluate() is called with a
MXmlEngXPathEvaluationContext instance which holds the function arguments and
evaluation result.

@see MXmlEngXPathExtensionFunction
@see MXmlEngXPathEvaluationContext
@see TXmlEngExtensionFunctionDescriptor
*/
class XmlEngXPathConfiguration
    {
public:
    /** 
	Unused 
	@internalComponent
	*/
    IMPORT_C static void Unused_Func1(TBool aEnable);

    /**
    Disables support of any previously registered extension functions
    and switches to support only the XPath Function Library.
	@leave KErrNoMemory Memory allocation failure
    */
    IMPORT_C static void ResetExtensionFunctionsL();

    /**
    Check if XPath extension function is supported.
    
    @param aFunc Function name
    @param aNsUri Function namespace URI
    @return ETrue if supported, EFalse otherwise
	@leave - One of the system-wide error codes
    */
    IMPORT_C static TBool IsFunctionSupportedL(const TDesC8& aFunc, const TDesC8& aNsUri);

    /**
    Registers an extension function at the global scope.

    @param aFuncDes Function descriptor.
	@leave KXmlEngErrWrongUseOfAPI Error initializing XPath context
	@leave KXmlEngErrXPathResult Error adding function 
	@leave - One of the system-wide error codes
    */
    IMPORT_C static void AddExtensionFunctionL(const TXmlEngExtensionFunctionDescriptor& aFuncDes);    

    /**
    Registers a set of extension functions at the global scope.
    
    @param aFuncVector Vector with function descriptors.  Ownership is not transferred.
	@param aSize The size of the vector
	@leave KXmlEngErrWrongUseOfAPI Error initializing XPath context
	@leave KXmlEngErrXPathResult Error adding function 
	@leave - One of the system-wide error codes
    */
    IMPORT_C static void AddExtensionFunctionVectorL (const RArray<TXmlEngExtensionFunctionDescriptor>& aFuncVector, TUint aSize);    

    /**
    Registers a native extension function at the global scope.

    @param aNativeFuncDes Function descriptor.
	@leave KXmlEngErrWrongUseOfAPI Error initializing XPath context
	@leave KXmlEngErrXPathResult Error adding function 
	@leave - One of the system-wide error codes
    */
    IMPORT_C static void AddNativeExtensionFunctionL(const TXmlEngExtensionFunctionDescriptor& aNativeFuncDes);    

    /**
    Registers a set of native extension functions at the global scope.

    @param aNativeFuncVector Function descriptor.
	@param aSize The size of the vector
	@leave KXmlEngErrWrongUseOfAPI Error initializing XPath context
	@leave KXmlEngErrXPathResult Error adding function 
	@leave - One of the system-wide error codes
    */
    IMPORT_C static void AddNativeExtensionFunctionVectorL(const RArray<TXmlEngExtensionFunctionDescriptor>& aNativeFuncVector, TUint aSize);    

    /**
	Deregisters an extension function.  No error is generated if the extension
	function is not found.

    @param aFuncDes Function descriptor.
    */
    IMPORT_C static void RemoveExtensionFunction(const TXmlEngExtensionFunctionDescriptor& aFuncDes);    

    /**
	Deregisters a set of extension functions.  No error is generated if an
	extension function in the vector is not found.

    @param aFuncVector Vector with function descriptors.
	@param aSize The size of the vector
    */
    IMPORT_C static void RemoveExtensionFunctionVector(const RArray<TXmlEngExtensionFunctionDescriptor>& aFuncVector, TUint aSize);    
    };

#endif /* XMLENGXPATHCONFIGURATION_H */

