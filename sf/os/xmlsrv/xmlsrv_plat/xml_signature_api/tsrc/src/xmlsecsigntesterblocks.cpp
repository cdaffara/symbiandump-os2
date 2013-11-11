/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       Xmlsecurity signing tests
*
*/

//  CLASS HEADER
#include "xmlsecsigntester.h"

//  EXTERNAL INCLUDES
#include <StifParser.h>
#include <f32file.h>
#include <Stiftestinterface.h>
#include <s32file.h>
#include <e32svr.h>
#include <libc/sys/reent.h>
#include <utf.h>

//  INTERNAL INCLUDES
#include "xmlsecwinit.h"
#include "xmlsecwsign.h"
#include "xmlsecwerrors.h"
#include "inetprottextutils.h" 

#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengdomimplementation.h>


// MACROS
//#define ?macro ?macro_def
#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXmlSecTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CXmlSecSignTester::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CXmlSecTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CXmlSecSignTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		
		
		// Test cases for encryption
		ENTRY( "Test-NewLL", CXmlSecSignTester::CXmlSecSign_NewLL),
		ENTRY( "Test-NewLC", CXmlSecSignTester::CXmlSecSign_NewLCL),
		ENTRY( "Test-SignXmlDocumentLL", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentLL),
		ENTRY( "Test-SignXmlDocumentL_2L", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentL_2L),
		ENTRY( "Test-SignXmlDocumentL_3L", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentL_3L),
	  ENTRY( "Test-SignXmlDocumentKeyFromFileL_1L", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromFileL_1L),
	  ENTRY( "Test-SignXmlDocumentKeyFromFileL_2L", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromFileL_2L),
	  ENTRY( "Test-SignXmlDocumentKeyFromBufferL_1L", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromBufferL_1L),
	  ENTRY( "Test-SignXmlDocumentKeyFromBufferL_2L", CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromBufferL_2L),
	  ENTRY( "Test-SignXmlNodesLL", CXmlSecSignTester::CXmlSecSign_SignXmlNodesLL),
	  ENTRY( "Test-VerifyXmlNodeLL", CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeLL),
	  ENTRY( "Test-VerifyXmlNodeLL_2L", CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeL_2L),
	  ENTRY( "Test-VerifyXmlNodeKeyFromFileLL", CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeKeyFromFileLL),
	  ENTRY( "Test-VerifyXmlNodeKeyFromBufferLL", CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeKeyFromBufferLL),
	  ENTRY( "Test-VerifyXmlDocumentLL", CXmlSecSignTester::CXmlSecSign_VerifyXmlDocumentLL),
	  ENTRY( "Test-VerifyXmlDocumentKeyFromFileLL", CXmlSecSignTester::CXmlSecSign_VerifyXmlDocumentKeyFromFileLL),
	  ENTRY( "Test-VerifyXmlDocumentKeyFromBufferLL", CXmlSecSignTester::CXmlSecSign_VerifyXmlDocumentKeyFromBufferLL),
	  ENTRY( "Test-SetTemplateFromFileL_1L", CXmlSecSignTester::CXmlSecSign_SetTemplateFromFileL_1L),
	  ENTRY( "Test-SetTemplateFromFileL_2L", CXmlSecSignTester::CXmlSecSign_SetTemplateFromFileL_2L),
	  ENTRY( "Test-SetTemplateFromBufferLL", CXmlSecSignTester::CXmlSecSign_SetTemplateFromBufferLL),
	  ENTRY( "Test-SetTemplateLL", CXmlSecSignTester::CXmlSecSign_SetTemplateLL),
	  ENTRY( "Test-CreateTemplateL_1L", CXmlSecSignTester::CXmlSecSign_CreateTemplateL_1L),
	  ENTRY( "Test-CreateTemplateL_2L", CXmlSecSignTester::CXmlSecSign_CreateTemplateL_2L),
	  ENTRY( "Test-GetCurrentTemplateLL", CXmlSecSignTester::CXmlSecSign_GetCurrentTemplateLL),
	  ENTRY( "Test-SetKeyInfoL_1L", CXmlSecSignTester::CXmlSecSign_SetKeyInfoL_1L),
	  ENTRY( "Test-SetKeyInfoL_2L", CXmlSecSignTester::CXmlSecSign_SetKeyInfoL_2L),
	  ENTRY( "Test-SetKeyFromFileLL", CXmlSecSignTester::CXmlSecSign_SetKeyFromFileLL),
	  ENTRY( "Test-SetKeyFromBufferLL", CXmlSecSignTester::CXmlSecSign_SetKeyFromBufferLL),
	  ENTRY( "Test-SetCertFromFileLL", CXmlSecSignTester::CXmlSecSign_SetCertFromFileLL),
	  ENTRY( "Test-SetCertFromBufferLL", CXmlSecSignTester::CXmlSecSign_SetCertFromBufferLL),
	  ENTRY( "Test-AddTrustedCertFromFileLL", CXmlSecSignTester::CXmlSecSign_AddTrustedCertFromFileLL),
	  ENTRY( "Test-AddTrustedCertFromBufferLL", CXmlSecSignTester::CXmlSecSign_AddTrustedCertFromBufferLL),
		};

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    


HBufC8* CXmlSecSignTester::ReadFileL(const TDesC& aFileName)
{
	// open fs and file
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);

	RFile file;
	User::LeaveIfError(file.Open(fs, aFileName, EFileStream));
	CleanupClosePushL(file);
	
	TInt size=0;
	User::LeaveIfError(file.Size(size));
	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();
	User::LeaveIfError(file.Read(bufPtr));
	
	CleanupStack::Pop(buf);
	// cleanup
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);
	return buf;
}

// CONSTRUCTION
//------------------------------------------------------------------------------
void CXmlSecSignTester::CXmlSecSign_NewLL()
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewL();
    CleanupStack::PushL(sign);
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    }
//------------------------------------------------------------------------------
//
void CXmlSecSignTester::CXmlSecSign_NewLCL()
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentLL(CStifItemParser& aItem)
    {
    // key
   	TPtrC p_StrKey;
	  aItem.GetNextString(p_StrKey);
	  HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	  CleanupStack::PushL(p_BufStrKey8);
	  TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
	
    _LIT8(t4,"name");
    XmlSecPushL();

    CXmlSecSign* sign = CXmlSecSign::NewLC();
    //no document
    RXmlEngDocument doc;
    TXmlEngElement el;
    TRAPD(err,el=sign->SignXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrWrongParameter );
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    sign->SetTemplateL(doc);
    //bad template
    TRAP(err,el=sign->SignXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrTemplate );
    //no key
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate);
    TRAP(err,el=sign->SignXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrSign );
    //no key  
  
    TRAP(err,el=sign->SignXmlDocumentL(doc,FALSE));
    LOCAL_ASSERT( err == KErrSign );
   
    //ok
    sign->SetKeyFromFileL(p_StrKey8,t4,CXmlSecSign::ERSAPrivate);
    TRAP(err,el=sign->SignXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !el.Name().Compare(_L8("Signature")));
    el.Remove();
       
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentL_2L( CStifItemParser& aItem )
    {
    //key info
    TPtrC p_KeyInfo;
    aItem.GetNextString(p_KeyInfo);
    HBufC8* p_BufInf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_KeyInfo);
	CleanupStack::PushL(p_BufInf8);
	TPtrC8 p_KeyInfo8 = p_BufInf8->Des();
	
	XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();

    TXmlEngElement el;
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    sign->SetTemplateL(doc);
    //no key
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate);
    
    //key not ofund in keystore
    sign->SetKeyInfoL(p_KeyInfo8);
    TRAPD(err,el=sign->SignXmlDocumentL(doc,EFalse));
    LOCAL_ASSERT( err == KErrNotFound);
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufInf8);
    CloseSTDLIB();
    return KErrNone;
    }    
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentL_3L(  )
    {
    _LIT8(t4,"name");
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();

    TXmlEngElement el;
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    sign->SetTemplateL(doc);
    //no key
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate);

    sign->SetKeyInfoL(t4);
    TRAPD(err,el=sign->SignXmlDocumentL(doc,FALSE));
    LOCAL_ASSERT( err == KErrNone);

    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    
    CloseSTDLIB();
    return KErrNone;
    }        
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromFileL_1L(CStifItemParser& aItem)
    {
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
	
    _LIT8(t4,"name");
    XmlSecPushL();

    CXmlSecSign* sign = CXmlSecSign::NewLC();
    RXmlEngDocument doc;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    RXmlEngDocument doc3 = doc2.CloneDocumentL();
    TXmlEngElement el;
    TRAPD(err,el=sign->SignXmlDocumentKeyFromFileL(doc,doc3,p_StrKey8,t4,CXmlSecSign::EHMAC)); 
    doc3.Close();
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !el.Name().Compare(_L8("Signature")));
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromFileL_2L(CStifItemParser& aItem)
    {
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
	  
    _LIT8(t4,"name");
    XmlSecPushL();

    CXmlSecSign* sign = CXmlSecSign::NewLC();
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    TRAPD(err,el=sign->SignXmlDocumentKeyFromFileL(doc,p_StrKey8,t4,CXmlSecSign::EHMAC)); 
    
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !el.Name().Compare(_L8("Signature")));
   
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromBufferL_1L(CStifItemParser& aItem)
    {
    TPtrC p_StrKey;
    aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );

    _LIT8(t4,"name");
    XmlSecPushL();

    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    RXmlEngDocument doc3 = doc2.CloneDocumentL();
    
    TRAPD(err,el=sign->SignXmlDocumentKeyFromBufferL(doc,doc3,*p_BufStrKey8,t4,CXmlSecSign::EHMAC)); 
    doc3.Close();
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !el.Name().Compare(_L8("Signature")));
 
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SignXmlDocumentKeyFromBufferL_2L(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
    
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    
    TRAPD(err,el=sign->SignXmlDocumentKeyFromBufferL(doc,*p_BufStrKey8,t4,CXmlSecSign::EHMAC));  
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !el.Name().Compare(_L8("Signature")));
    
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//    
TInt CXmlSecSignTester::CXmlSecSign_SignXmlNodesLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"forsignnodesname");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();

    XmlSecPushL();
	CXmlSecSign* sign = CXmlSecSign::NewLC();
    //zero TXmlEngElements
    RXmlEngDocument doc;
    RArray<TXmlEngElement> arrayelem;
    TXmlEngElement el;
    TRAPD(err,el=sign->SignXmlNodesL(arrayelem));
    LOCAL_ASSERT( err == KErrWrongParameter );
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    
    arrayelem.Append(elem);
    TXmlEngElement elem2 = doc.CreateElementL(_L8("test2"));
    //doc.SetDocumentElement(elem2);
    arrayelem.Append(elem2);
 
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    //ok
    sign->SetKeyFromFileL(p_StrKey8,t4,CXmlSecSign::EHMAC);
    TRAP(err,el=sign->SignXmlNodesL(arrayelem));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !el.Name().Compare(_L8("Signature")));
     
    arrayelem.Close();
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeLL( CStifItemParser& aItem )
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
    
    TBool ver=EFalse;
    XmlSecPushL();
    
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);

    el=sign->SignXmlDocumentKeyFromFileL(doc,p_StrKey8,t4,CXmlSecSign::EHMAC); 
   
    //no signature in doc     
    TRAPD(err,ver=sign->VerifyXmlNodeL(el));
    LOCAL_ASSERT(!ver); 
    
    doc.DocumentElement().AppendChildL(el);
    //ok
    ver=sign->VerifyXmlNodeL(el);
    LOCAL_ASSERT(ver); 
    
    TRAP(err,ver=sign->VerifyXmlNodeL(el,CXmlSecSign::ERootCertChain));
    LOCAL_ASSERT( err == KErrVerify );
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//    
 TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeL_2L(CStifItemParser& aItem)
    {
    TPtrC p_Cert;
	aItem.GetNextString(p_Cert);
	HBufC8* p_CertBuf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cert);
	CleanupStack::PushL(p_CertBuf8);
	TPtrC8 p_Cert8 = p_CertBuf8->Des();
	  
    TPtrC p_TrustCert;
	aItem.GetNextString(p_TrustCert);
	HBufC8* p_TrustCertBuf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_TrustCert);
	CleanupStack::PushL(p_TrustCertBuf8);
	TPtrC8 p_TrustCert8 = p_TrustCertBuf8->Des();
	  
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
    
    TBool ver=FALSE;
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,TRUE);
    //sign->SetKeyInfoL(keyRSA1);
    sign->SetKeyFromFileL(p_StrKey8,p_StrKey8,CXmlSecSign::ERSAPrivate);
    sign->SetCertFromFileL(p_Cert8);
    
    el=sign->SignXmlDocumentL(doc); 
    
    doc.DocumentElement().AppendChildL(el);

    TRAPD(err,ver=sign->VerifyXmlNodeL(el,CXmlSecSign::ERootCertChain));
    LOCAL_ASSERT( err == KErrVerify );
    
    sign->AddTrustedCertFromFileL(p_TrustCert8);
    TRAP(err,ver=sign->VerifyXmlNodeL(el,CXmlSecSign::ERootCertChain));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT(ver);
    
    TRAP(err,ver=sign->VerifyXmlNodeL(el,CXmlSecSign::ECertStore));
    LOCAL_ASSERT( err == KErrVerify );
          
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CleanupStack::PopAndDestroy(p_TrustCertBuf8);
    CleanupStack::PopAndDestroy(p_CertBuf8);
    CloseSTDLIB();
    return KErrNone;
    }   
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeKeyFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
    
    TBool ver;
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    
    el=sign->SignXmlDocumentKeyFromFileL(doc,p_StrKey8,t4,CXmlSecSign::EHMAC); 
   
    doc.DocumentElement().AppendChildL(el);
    //ok
    ver=sign->VerifyXmlNodeKeyFromFileL(el,p_StrKey8,t4,CXmlSecSign::EHMAC);
    LOCAL_ASSERT(ver); 
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlNodeKeyFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	    
    TBool ver;
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    
    el=sign->SignXmlDocumentKeyFromBufferL(doc,*p_BufStrKey8,t4,CXmlSecSign::EHMAC); 
   
    doc.DocumentElement().AppendChildL(el);
    //ok
    ver=sign->VerifyXmlNodeKeyFromBufferL(el,*p_BufStrKey8,t4,CXmlSecSign::EHMAC);
    LOCAL_ASSERT(ver);
     
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlDocumentLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
    aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
    
    TBool ver;
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    
    TRAPD(err,ver=sign->VerifyXmlDocumentL(doc));
    LOCAL_ASSERT(err==KErrVerify);
    
    el=sign->SignXmlDocumentKeyFromFileL(doc,p_StrKey8,t4,CXmlSecSign::EHMAC); 
    
    doc.DocumentElement().AppendChildL(el);
    //ok
    ver=sign->VerifyXmlDocumentL(doc);
    LOCAL_ASSERT(ver); 
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlDocumentKeyFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
    
    TBool ver;
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    
    el=sign->SignXmlDocumentKeyFromFileL(doc,p_StrKey8,t4,CXmlSecSign::EHMAC); 
    
    doc.DocumentElement().AppendChildL(el);
    //ok
    ver=sign->VerifyXmlDocumentKeyFromFileL(doc,p_StrKey8,t4,CXmlSecSign::EHMAC);
    LOCAL_ASSERT(ver); 
   
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_VerifyXmlDocumentKeyFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC p_StrKey;
	aItem.GetNextString(p_StrKey);
	HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
	CleanupStack::PushL( p_BufStrKey8 );
	     
    TBool ver;
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
        
    RXmlEngDocument doc;
    TXmlEngElement el;
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument doc2 = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    
    el=sign->SignXmlDocumentKeyFromBufferL(doc,*p_BufStrKey8,t4,CXmlSecSign::EHMAC); 
   
    doc.DocumentElement().AppendChildL(el);
    //ok
    ver=sign->VerifyXmlDocumentKeyFromBufferL(doc,*p_BufStrKey8,t4,CXmlSecSign::EHMAC);
    LOCAL_ASSERT(ver);
     
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetTemplateFromFileL_1L(CStifItemParser& aItem)
    {
    TPtrC p_Template;
    aItem.GetNextString(p_Template);
    HBufC8* p_BufTem8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Template);
  	CleanupStack::PushL(p_BufTem8);
	TPtrC8 p_Template8 = p_BufTem8->Des();

    TPtrC p_WrongTemplate;
    aItem.GetNextString(p_WrongTemplate);
    HBufC8* p_WrongBufTem8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Template);
  	CleanupStack::PushL(p_WrongBufTem8);
	TPtrC8 p_WrongTemplate8 = p_WrongBufTem8->Des();

	XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    sign->SetTemplateFromFileL(p_Template8);
    RXmlEngDocument tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("doc")));
    
    TRAPD(err,sign->SetTemplateFromFileL(p_WrongTemplate8));
    LOCAL_ASSERT( err );
        
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_WrongBufTem8);
    CleanupStack::PopAndDestroy(p_BufTem8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//   
TInt CXmlSecSignTester::CXmlSecSign_SetTemplateFromFileL_2L(CStifItemParser& aItem)
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    TPtrC p_Template;
    aItem.GetNextString(p_Template);
    HBufC8* p_BufTem8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Template);
  	CleanupStack::PushL(p_BufTem8);
	TPtrC8 p_Template8 = p_BufTem8->Des();

    RFs fs;
    fs.Connect();
    sign->SetTemplateFromFileL(fs,p_Template8);
    fs.Close();
    RXmlEngDocument tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("doc")) );
    
    CleanupStack::PopAndDestroy(sign);
    CleanupStack::PopAndDestroy(p_BufTem8);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetTemplateFromBufferLL(  )
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    _LIT8(t1,"<doc></doc>");
    sign->SetTemplateFromBufferL(t1);
    RXmlEngDocument tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("doc")) );
    
    _LIT8(t2,"<doc>");
    TRAPD(err,sign->SetTemplateFromBufferL(t2));
    LOCAL_ASSERT( err );
    
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetTemplateLL(  )
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();

    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    sign->SetTemplateL(doc);
    CleanupStack::PopAndDestroy(&doc);
    RXmlEngDocument tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("test")) );
    doc.Close();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    elem = doc.CreateElementL(_L8("test2"));
    doc.SetDocumentElement(elem);
    
    sign->SetTemplateL(doc);
    CleanupStack::PopAndDestroy(&doc);
    tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("test2")) );
    doc.Close();
    RXmlEngDocument doc2;
    TRAPD(err, sign->SetTemplateL(doc2));
    LOCAL_ASSERT( err == KErrTemplate );
    
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_CreateTemplateL_1L(  )
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    RXmlEngDocument tem = sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE);
    LOCAL_ASSERT( tem.NotNull() );
   
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPublic);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,TRUE);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE,1);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE,2);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE,4);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE,1,_L8("tet"));
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE,1,_L8(""));
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,FALSE,1,_L8("tet"),FALSE);
    LOCAL_ASSERT( tem.NotNull() );
          
    CleanupStack::PopAndDestroy(sign); 
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_CreateTemplateL_2L(  )
    {

    _LIT8(t9,"dsig:Signature");
    _LIT8(pIdName,"id");
    _LIT8(pIdVal,"http://exem.com");
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDocument doc;
    TXmlEngElement el;    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    
    TXmlEngElement elem = doc.CreateElementL(_L8("doc"));
    
    TXmlEngNamespace ns = elem.LookupNamespaceByUriL(_L8("a_uri"));
    if (elem.LookupNamespaceByPrefixL(ns.Prefix()).IsSameNode(ns)){
    
       elem.AddNewElementL(_L8("product"),ns);
    }
    
    TXmlEngNamespace targetNs = elem.AddNamespaceDeclarationL(_L8("http://example.com/"),_L8("ex"));
    
    TXmlEngElement el_1 = elem.AddNewElementL(_L8("outer"), targetNs);
    TXmlEngElement el_2 = el_1.AddNewElementL(_L8("inner"));
    el_2.AddNamespaceDeclarationL(_L8("http://whatever.com/"),_L8("ex"));
    TXmlEngElement el_3 = el_2.AddNewElementL(_L8("problem"), targetNs);
    
    
    el_1.AddXmlIdL(pIdName, pIdVal );
    
    doc.SetDocumentElement(elem);
    RArray<TXmlEngElement> array;
    CleanupClosePushL(array);
    array.Append(elem);
    
    RXmlEngDocument tem;
    TRAPD(err,tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,array,t9));
    LOCAL_ASSERT( err == KErrIdUndefineNS);
       
    array.Append(el_1);
    array.Append(el_2);
    array.Append(el_3);
    
    TRAP(err,tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,array,pIdName,0,4,_L8("tet"),FALSE));
    LOCAL_ASSERT( err == 0);
    
    TRAP(err,tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,array,pIdName,0,2,_L8("tet"),FALSE));
    LOCAL_ASSERT( err == 0);
    
    TRAP(err,tem = sign->CreateTemplateL(CXmlSecSign::ERSAPrivate,array,pIdName,1,1,_L8("tet"),FALSE));
    LOCAL_ASSERT( err == 0);
    
    CleanupStack::PopAndDestroy(&array);
    
    CleanupStack::PopAndDestroy(&doc);  
    domImp.Close();   
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }    
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_GetCurrentTemplateLL(  )
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();

    RXmlEngDocument tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.IsNull() );
    sign->DestroyCurrentTemplate();
    tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.IsNull() );
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    
    sign->SetTemplateL(doc);
    tem = sign->CurrentTemplate();
    
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("test")) );
    
    sign->DestroyCurrentTemplate();
    tem = sign->CurrentTemplate();
    LOCAL_ASSERT( tem.IsNull() );
   
    CleanupStack::PopAndDestroy(&doc);    
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetKeyInfoL_1L(  )
    {
    _LIT8(t4,"name");
    _LIT8(t5,"");
    
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    
    //no template
    TRAPD(err,sign->SetKeyInfoL(t4));
    LOCAL_ASSERT( err == KErrTemplate );
    //null param
    sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    TRAP(err,sign->SetKeyInfoL(t5));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //ok
    sign->SetKeyInfoL(t4);
    //no keyinfo
    sign->SetTemplateL(doc);
    TRAP(err,sign->SetKeyInfoL(t4));
    LOCAL_ASSERT( err == KErrTemplate );
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetKeyInfoL_2L(  )
    {
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    TXmlEngElement elem1; 
    
    //no template
    TRAPD(err,sign->SetKeyInfoL(doc.AsElement()));
    LOCAL_ASSERT( err == KErrTemplate );
    sign->CreateTemplateL(CXmlSecSign::EHMAC,FALSE);
    //wrong element = null
    TRAP(err,sign->SetKeyInfoL(elem1));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //ok 
    sign->SetKeyInfoL(elem.CopyL());
    //no keyinfo node
    sign->SetTemplateL(doc);
    TRAP(err,sign->SetKeyInfoL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrTemplate );
     
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CloseSTDLIB();
    return KErrNone;
    }        
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetKeyFromFileLL(CStifItemParser& aItem)
    {
    TPtrC p_StrKeyhmac;
	aItem.GetNextString(p_StrKeyhmac);
	HBufC8* p_BufStrKeyhmac8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKeyhmac);
	CleanupStack::PushL( p_BufStrKeyhmac8 );
	TPtrC8 p_StrKeyhmac8 = p_BufStrKeyhmac8->Des();
    
	TPtrC p_StrKeyPriv;
	aItem.GetNextString(p_StrKeyPriv);
	HBufC8* p_BufStrKeyPriv8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKeyPriv);
	CleanupStack::PushL( p_BufStrKeyPriv8 );
	TPtrC8 p_StrKeyPriv8 = p_BufStrKeyPriv8->Des();
    
    TPtrC p_WrngStrKey;
	aItem.GetNextString(p_WrngStrKey);
	HBufC8* p_WrngBufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_WrngStrKey);
	CleanupStack::PushL( p_WrngBufStrKey8 );
	TPtrC8 p_WrngStrKey8 = p_WrngBufStrKey8->Des();
    
    _LIT8(t4,"name");
    _LIT8(t5,"");
    TPtrC p_StrKey;
    aItem.GetNextString(p_StrKey);
    HBufC8* p_BufStrKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKey);
    CleanupStack::PushL( p_BufStrKey8 );
    TPtrC8 p_StrKey8 = p_BufStrKey8->Des();
    
    _LIT8(t7,"namelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdfnamelongdf");
    _LIT8(t8,"name2");
    _LIT8(t9,"name3");
    _LIT8(t10,"name4");
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();

    //ok
    sign->SetKeyFromFileL(p_StrKeyhmac8,t4,CXmlSecSign::EHMAC);
    sign->SetKeyFromFileL(p_StrKeyPriv8,t7,CXmlSecSign::ERSAPrivate);
    sign->SetKeyFromFileL(p_StrKey8,t4,CXmlSecSign::ERSAPublic);                
    //bad public
    TRAPD(err,sign->SetKeyFromFileL(p_WrngStrKey8,t8,CXmlSecSign::ERSAPublic));
    LOCAL_ASSERT( err == KErrArgument ); 
    //bad private -> rsapub 
    TRAP(err,sign->SetKeyFromFileL(p_StrKey8,t9,CXmlSecSign::ERSAPrivate));
    LOCAL_ASSERT( err == KErrArgument );
    //bad private -> some data - hmac 
    TRAP(err,sign->SetKeyFromFileL(p_StrKeyhmac8,t10,CXmlSecSign::ERSAPrivate));
    LOCAL_ASSERT( err == KErrArgument  );
    //name=null
    TRAP(err,sign->SetKeyFromFileL(p_StrKeyPriv8,t5,CXmlSecSign::ERSAPrivate));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //name=null
    TRAP(err,sign->SetKeyFromFileL(p_StrKey8,t5,CXmlSecSign::ERSAPublic));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //filename=null
    TRAP(err,sign->SetKeyFromFileL(t5,t4,CXmlSecSign::ERSAPublic));
    LOCAL_ASSERT( err == KErrWrongParameter );
  
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKey8);
    CleanupStack::PopAndDestroy(p_WrngBufStrKey8);
    CleanupStack::PopAndDestroy(p_BufStrKeyPriv8);    
    CleanupStack::PopAndDestroy(p_BufStrKeyhmac8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetKeyFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t1,"01234567012345670123456701234567");
    _LIT8(t3,"423");
    _LIT8(t4,"name");
    _LIT8(t5,"");
           
    TPtrC p_StrKeyPriv;
    aItem.GetNextString(p_StrKeyPriv);
    HBufC8* p_BufStrKeyPriv8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKeyPriv);
    CleanupStack::PushL( p_BufStrKeyPriv8 );
        
    TPtrC p_StrKeyPub;
    aItem.GetNextString(p_StrKeyPub);
    HBufC8* p_BufStrKeyPub8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKeyPub);
    CleanupStack::PushL( p_BufStrKeyPub8 );
        
    XmlSecPushL();            
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    //ok
    sign->SetKeyFromBufferL(t1,t4,CXmlSecSign::EHMAC);
    sign->SetKeyFromBufferL(*p_BufStrKeyPriv8,t4,CXmlSecSign::ERSAPrivate);
    sign->SetKeyFromBufferL(*p_BufStrKeyPub8,t4,CXmlSecSign::ERSAPublic);
    
    //name=null
    TRAPD(err,sign->SetKeyFromBufferL(*p_BufStrKeyPriv8,t5,CXmlSecSign::ERSAPrivate));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //name=null
    TRAP(err,sign->SetKeyFromBufferL(*p_BufStrKeyPub8,t5,CXmlSecSign::ERSAPublic));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //buffer=null
    TRAP(err,sign->SetKeyFromBufferL(t5,t4,CXmlSecSign::ERSAPublic));
    LOCAL_ASSERT( err == KErrWrongParameter );      
    
    //bad public
    TRAP(err,sign->SetKeyFromBufferL(t3,t4,CXmlSecSign::ERSAPublic));
    LOCAL_ASSERT( err == KErrArgument);
    //bad private
   
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKeyPub8);
    CleanupStack::PopAndDestroy(p_BufStrKeyPriv8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetCertFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t3,"");
    _LIT8(t5,"name");
       
    TPtrC p_Cert;
    aItem.GetNextString(p_Cert);
    HBufC8* p_CertBuf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cert);
    CleanupStack::PushL(p_CertBuf8);
    TPtrC8 p_Cert8 = p_CertBuf8->Des();

    TPtrC p_StrKeyPriv;
    aItem.GetNextString(p_StrKeyPriv);
    HBufC8* p_BufStrKeyPriv8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKeyPriv);
    CleanupStack::PushL( p_BufStrKeyPriv8 );
    TPtrC8 p_StrKeyPriv8 = p_BufStrKeyPriv8->Des();
    
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    //no key set 
    TRAPD(err,sign->SetCertFromFileL(p_Cert8))
    LOCAL_ASSERT( err == KErrKey );
    //ok
    sign->SetKeyFromFileL(p_StrKeyPriv8,t5,CXmlSecSign::ERSAPrivate);
    sign->SetCertFromFileL(p_Cert8);              
    //no cert file
    TRAP(err,sign->SetCertFromFileL(t3))
    LOCAL_ASSERT( err == KErrCert );
   
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKeyPriv8);
    CleanupStack::PopAndDestroy(p_CertBuf8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_SetCertFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t5,"name");
    TPtrC p_Cert;
    aItem.GetNextString(p_Cert);
    HBufC8* p_CertBuf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cert);
    CleanupStack::PushL(p_CertBuf8);
    
    TPtrC p_StrKeyPriv;
    aItem.GetNextString(p_StrKeyPriv);
    HBufC8* p_BufStrKeyPriv8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_StrKeyPriv);
    CleanupStack::PushL( p_BufStrKeyPriv8 );
    TPtrC8 p_StrKeyPriv8 = p_BufStrKeyPriv8->Des();
    
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();
        
    //no key set 
    TRAPD(err,sign->SetCertFromBufferL(*p_CertBuf8))
    LOCAL_ASSERT( err == KErrKey );
    //ok
    sign->SetKeyFromFileL(p_StrKeyPriv8,t5,CXmlSecSign::ERSAPrivate);
    sign->SetCertFromBufferL(*p_CertBuf8);              
   
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufStrKeyPriv8);
    CleanupStack::PopAndDestroy(p_CertBuf8);
    CloseSTDLIB();
    return KErrNone;
    }       
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_AddTrustedCertFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t3,"");
    TPtrC p_Cert;
    aItem.GetNextString(p_Cert);
    HBufC8* p_CertBuf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cert);
    CleanupStack::PushL(p_CertBuf8);
    TPtrC8 p_Cert8 = p_CertBuf8->Des();
    
    XmlSecPushL();
    CXmlSecSign* sign = CXmlSecSign::NewLC();

    //ok
    sign->AddTrustedCertFromFileL(p_Cert8);              
    //no cert file
    TRAPD(err,sign->AddTrustedCertFromFileL(t3))
    LOCAL_ASSERT( err == KErrCert );
   
    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_CertBuf8);
    CloseSTDLIB();
    return KErrNone;
    }
//------------------------------------------------------------------------------
//
TInt CXmlSecSignTester::CXmlSecSign_AddTrustedCertFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t3,"");
    TPtrC p_Cert;
    aItem.GetNextString(p_Cert);
    HBufC8* p_CertBuf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cert);
    CleanupStack::PushL(p_CertBuf8);
    
    XmlSecPushL(); 
    CXmlSecSign* sign = CXmlSecSign::NewLC();
    
    //ok
    sign->AddTrustedCertFromBufferL(*p_CertBuf8);              
    //no cert file
    TRAPD(err,sign->AddTrustedCertFromBufferL(t3))
    LOCAL_ASSERT( err == KErrArgument );

    CleanupStack::PopAndDestroy(sign);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_CertBuf8);
    CloseSTDLIB();
    return KErrNone;
    }    
//  END OF FILE
