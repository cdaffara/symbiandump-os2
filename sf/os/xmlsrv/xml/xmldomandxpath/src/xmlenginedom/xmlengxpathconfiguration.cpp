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
// XPath configuraion functions
//

#include "xmlengdomdefs.h"
#include <xml/dom/xmlengxpathconfiguration.h>
#include "xmlengxpathevaluationcontext_impl.h"
#include "libxml2_globals_private.h"
#include <xml/dom/xmlengxpatherrors.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

const TInt KHashTableSize = 32;

// -----------------------------------------------------------------------------------------------------
// Common callback for all extension functions
//
// Prototyped by:
// typedef void (*xmlXPathFunction) (xmlXPathParserContextPtr ctxt, int nargs);
//
// Non-natively implemented XPath extension function cannot be registered in libxml2 XPath module 
// directly without using libxml2 XPath API.Thus, native  implementation is impossible
// when libxml2's APIs are hidden.
//
// That is why this callback is used: it is a front-end for function calls.
// Function pointers are registered in a separate storage (xmlXPathIntermediaryExtensionFunctionsHash)
// and are dynamically discovered during function call by function name and namespace uri.
// -----------------------------------------------------------------------------------------------------
//
void XmlEngineXpathCommonExtensionCallback(xmlXPathParserContextPtr ctxt, int nargs)
    {
    // Select function to call
    const xmlChar* KFunc = ctxt->context->function;
    const xmlChar* KFuncNs = ctxt->context->functionURI;
        
    void* funcPtr = xmlHashLookup2(
                        xmlXPathIntermediaryExtensionFunctionsHash,
                        KFunc,
                        KFuncNs);
    if (!funcPtr)
        {
        XP_ERROR(XPATH_UNKNOWN_FUNC_ERROR);
        }
    MXmlEngXPathExtensionFunction* extFunc = reinterpret_cast<MXmlEngXPathExtensionFunction*>(funcPtr);
    // Check arity
    TInt maxArity = extFunc->MaxArity();
    if (nargs < ((TInt)extFunc->MinArity()) || 
        (maxArity >= 0  && nargs > maxArity))
        {
        XP_ERROR(XPATH_INVALID_ARITY);
        }
    // Wrap arguments
    TXmlEngXPathEvaluationContextImpl context(ctxt, nargs); 
    if (!context.Initialize())
        {
        SET_OOM_FLAG;
        XP_ERROR(XPATH_MEMORY_ERROR);
        }
    // Call function
    MXmlEngXPathExtensionFunction::TXmlEngEvaluationStatus status = extFunc->Evaluate(&context);
        RXmlEngXPathResult res = context.Result();
        if (status != MXmlEngXPathExtensionFunction::ESucceeded)
            {
	        xmlXPathFreeObject(INTERNAL_XPATHOBJPTR(res));
            XP_ERROR(XPATH_XE_EXTENSION_FUNC_ERROR);
            }
        // Remove arguments from the evaluation stack
        
        for (TInt i = 0; i < nargs; i++)
            {
            xmlXPathFreeObject(valuePop(ctxt));
            }
       
        // Push the result to the stack
        valuePush(ctxt, INTERNAL_XPATHOBJPTR(res)); // Note: no need to check OOM flag -- later...

    }

// -----------------------------------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngExtensionFunctionDescriptor::TXmlEngExtensionFunctionDescriptor()
	{
	iFunc = NULL;
    iName = NULL;
    iNamespaceUri = NULL;     
	iReserved = 0;
	}

// -----------------------------------------------------------------------------------------------------
// Switches On or Off support of XForms extensions by XML Engine:
//     - instance() function
//
// @param aEnable  ETrue/EFalse to Enable/Disable additional functions
//
// @note
//     Currently, XForms extensions are always ON and this method does nothing.
// -----------------------------------------------------------------------------------------------------
//
EXPORT_C void XmlEngXPathConfiguration::Unused_Func1(TBool /*aEnable*/)
    {
    }

// -----------------------------------------------------------------------------------------------------
// Disables support of any previously registered extension functions
// and switches to support of only XPath Function Library.
// -----------------------------------------------------------------------------------------------------
//
EXPORT_C void XmlEngXPathConfiguration::ResetExtensionFunctionsL()
    {
    // Set the hash tables free:   
    //      xmlXPathIntermediaryExtensionFunctionsHash  and
    //      xmlXPathDefaultFunctionsHash
    //
    
    xmlHashTablePtr& fExtHash = xmlXPathIntermediaryExtensionFunctionsHash;
    xmlHashFree(fExtHash, NULL);
    fExtHash = NULL;
    //
    xmlHashTablePtr& fHash = xmlXPathDefaultFunctionsHash;
    xmlHashFree(fHash, NULL);
    fHash = NULL;

    // Fill the hash table with standard functions
    //
    // This reinitializes hash tables of XPath functions
    xmlXPathContextPtr ctxt = xmlXPathNewContext(NULL);
    OOM_IF_NULL(ctxt);
    xmlXPathFreeContext(ctxt);
    }
     
EXPORT_C TBool XmlEngXPathConfiguration::IsFunctionSupportedL(
    const TDesC8& aFunc, 
    const TDesC8& aNsUri )
    {
    if(!xmlXPathDefaultFunctionsHash)
        {
        return FALSE;
        }
	// If the function hashes are not initialized -- we should do it!
    
    
    
    

	
    
    xmlChar* func = xmlCharFromDesC8L(aFunc);
    CleanupStack::PushL(func);
    xmlChar* ns = NULL;
    if(aNsUri.Length())
        ns =  xmlCharFromDesC8L(aNsUri);
    void* test = xmlHashLookup2(
                                xmlXPathDefaultFunctionsHash, 
                                func, 
                                ns);
    TBool res = (NULL != test);
    delete ns;
    CleanupStack::PopAndDestroy(func);
    return res;
    }

EXPORT_C void XmlEngXPathConfiguration::AddExtensionFunctionL(const TXmlEngExtensionFunctionDescriptor& aFuncDes )
    {
    xmlHashTablePtr fHash = xmlXPathIntermediaryExtensionFunctionsHash;
    // store new function to XPath Extensions storage
    if (!fHash)
        {
        
        
        xmlXPathIntermediaryExtensionFunctionsHash = fHash = xmlHashCreate(KHashTableSize);
        OOM_IF_NULL(fHash);
        }
    
    
    
    //     Note: it is not critical, since redefinitions of core XPath function won't be found
    TInt res = xmlHashUpdateEntry2(
                    fHash, 
                    (const xmlChar*)aFuncDes.iName, 
                    (const xmlChar*)aFuncDes.iNamespaceUri, 
                    aFuncDes.iFunc, 
                    NULL);
    if (res == -1)
        {
        TEST_OOM_FLAG;
		User::Leave(KXmlEngErrXPathResult);	
        }
    // Note: for now it is fixed that once defined, an extension is available in all
    //       evaluations of XPath in this thread.
    xmlXPathDefineExtensionFunctionsGlobally = 1;
    // register common callback in XPath engine with new function's name
    if (!xmlXPathDefaultFunctionsHash)
        {
        // It was not initialized yet
        // This is temporal solution to force initialization
        xmlXPathContextPtr tmpCtxt = xmlXPathNewContext(NULL);
        OOM_IF_NULL(tmpCtxt);
        xmlXPathFreeContext(tmpCtxt);
        
        }

    if ( !xmlXPathDefaultFunctionsHash )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    res = xmlHashUpdateEntry2(
        xmlXPathDefaultFunctionsHash, 
        (const xmlChar*)aFuncDes.iName,
        (const xmlChar*)aFuncDes.iNamespaceUri,
        (void*)XmlEngineXpathCommonExtensionCallback,
        NULL);

    if (res == -1)
        {
        TEST_OOM_FLAG;
		User::Leave(KXmlEngErrXPathResult); 
        }
    // Now, XmlEngineXpathCommonExtensionCallback is called for registered function.
    // XmlEngineXpathCommonExtensionCallback performs lookup in 
    // xmlXPathIntermediaryExtensionFunctionsHash for a function pointer by function name
    // wraps XPath evaluation context (arguments) and calls Evaluate() method of
    // registered function
    }
     
EXPORT_C void XmlEngXPathConfiguration::AddExtensionFunctionVectorL (const RArray<TXmlEngExtensionFunctionDescriptor>& aFuncVector, TUint aSize)
    {
    for (TUint i = 0; i < aSize; i++)
        {
        AddExtensionFunctionL(aFuncVector[i]);
        }
    }

EXPORT_C void XmlEngXPathConfiguration::AddNativeExtensionFunctionL(const TXmlEngExtensionFunctionDescriptor& aNativeFuncDes )
    {
    if (!xmlXPathDefaultFunctionsHash)
        {
        // It was not initialized yet
        // This is temporal solution to force initialization
        xmlXPathContextPtr tmpCtxt = xmlXPathNewContext(NULL);
        OOM_IF_NULL(tmpCtxt);
        xmlXPathFreeContext(tmpCtxt);
        }
    
    if ( !xmlXPathDefaultFunctionsHash )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
        
    TInt res = xmlHashUpdateEntry2(
        xmlXPathDefaultFunctionsHash, 
        (const xmlChar*)aNativeFuncDes.iName,
        (const xmlChar*)aNativeFuncDes.iNamespaceUri,
        aNativeFuncDes.iFunc,
        NULL);

    if (res == -1)
        {
        TEST_OOM_FLAG;
		User::Leave(KXmlEngErrXPathResult); 
        }     
    }
     
EXPORT_C void XmlEngXPathConfiguration::AddNativeExtensionFunctionVectorL(const RArray<TXmlEngExtensionFunctionDescriptor>& aNativeFuncVector, TUint aSize)
    {
    for (TUint i = 0; i < aSize; i++)
        {
        AddNativeExtensionFunctionL(aNativeFuncVector[i]);
        }     
    }

EXPORT_C void XmlEngXPathConfiguration::RemoveExtensionFunction(const TXmlEngExtensionFunctionDescriptor& aFuncDes )
    {
    xmlHashRemoveEntry2(
        xmlXPathDefaultFunctionsHash, 
        (const xmlChar*)aFuncDes.iName,
        (const xmlChar*)aFuncDes.iNamespaceUri,
        NULL /* deallocator function */);
    }
     
EXPORT_C void XmlEngXPathConfiguration::RemoveExtensionFunctionVector(const RArray<TXmlEngExtensionFunctionDescriptor>& aFuncVector, TUint aSize )
    {
    for (TUint i = 0; i < aSize; i++)
        {
        RemoveExtensionFunction(aFuncVector[i]);
        }      
    }
