/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Xmlsecurity encryption tests
*
*/
//  CLASS HEADER
#include "xmlsecencrypttester.h"

//  EXTERNAL INCLUDES
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <libc/sys/reent.h>
#include <utf.h>

//  INTERNAL INCLUDES
#include "xmlsecwinit.h"
#include "xmlsecwencrypt.h"
#include "xmlsecwerrors.h"
#include "XmlEngdom.h"
#include "XmlEngDOMParser.h"


// MACROS
//#define ?macro ?macro_def
#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXmlSecEncryptTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CXmlSecEncryptTester::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CXmlSecEncryptTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		
		
		// Test cases for encryption
		ENTRY( "Test-NewLL", CXmlSecEncryptTester::CXmlSecEncrypt_NewLL),
		ENTRY( "Test-NewLC", CXmlSecEncryptTester::CXmlSecEncrypt_NewLCL),
		ENTRY( "Test-EncryptDataLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptDataLL),
		ENTRY( "Test-EncryptXmlDocumentLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlDocumentLL),
		ENTRY( "Test-EncryptXmlDocumentKeyFromFileLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlDocumentKeyFromFileLL),
	    ENTRY( "Test-EncryptXmlDocumentKeyFromBufferLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlDocumentKeyFromBufferLL),
	    ENTRY( "Test-EncryptXmlNodeLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlNodeLL),
	    ENTRY( "Test-EncryptXmlNodeKeyFromFileLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlNodeKeyFromFileLL),
	    ENTRY( "Test-EncryptXmlNodeKeyFromBufferLL", CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlNodeKeyFromBufferLL),
	    ENTRY( "Test-DecryptXmlNodeLL", CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlNodeLL),
	    ENTRY( "Test-DecryptXmlNodeKeyFromFileLL", CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlNodeKeyFromFileLL),
	    ENTRY( "Test-DecryptXmlNodeKeyFromBufferLL", CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlNodeKeyFromBufferLL),
	    ENTRY( "Test-DecryptXmlDocumentLL", CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlDocumentLL),
	    ENTRY( "Test-DecryptXmlDocumentKeyFromFileLL", CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlDocumentKeyFromFileLL),
  	    ENTRY( "Test-DecryptXmlDocumentKeyFromBufferLL", CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlDocumentKeyFromBufferLL),
	    ENTRY( "Test-SetTemplateFromFileL_1L", CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateFromFileL_1L),
	    ENTRY( "Test-SetTemplateFromFileL_2L", CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateFromFileL_2L),
	    ENTRY( "Test-SetTemplateFromBufferLL", CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateFromBufferLL),
	    ENTRY( "Test-SetTemplateLL", CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateLL),
	    ENTRY( "Test-CreateTemplateLL", CXmlSecEncryptTester::CXmlSecEncrypt_CreateTemplateLL),
	    ENTRY( "Test-GetCurrentTemplateLL", CXmlSecEncryptTester::CXmlSecEncrypt_GetCurrentTemplateLL),
	    ENTRY( "Test-SetKeyFromFileLL", CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyFromFileLL),
	    ENTRY( "Test-SetKeyFromBufferLL", CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyFromBufferLL),
	    ENTRY( "Test-SetKeyInfoL_1L", CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyInfoL_1L),
	    ENTRY( "Test-SetKeyInfoL_2L", CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyInfoL_2L),
      };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

void CXmlSecEncryptTester::CXmlSecEncrypt_NewLL(  )
    {
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewL();
    CleanupStack::PushL(enc);
    //LOCAL_ASSERT( enc );
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    }
//--------------------------------------------------------------------------------
//
void CXmlSecEncryptTester::CXmlSecEncrypt_NewLCL(  )
    {
   	XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    //LOCAL_ASSERT( enc );
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptDataLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);
    TPtrC8 pt5(KNullDesC8());
    
    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();

    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();
  
    TPtrC pTem;
    aItem.GetNextString(pTem);
    HBufC8* p_BufTem8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pTem);
    CleanupStack::PushL( p_BufTem8 );
    TPtrC8 pTem8 = p_BufTem8->Des();
        
   	XmlSecPushL();    
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument doc;
    
    TRAPD(err,doc = enc->EncryptDataL(pt4));
    LOCAL_ASSERT( err == KErrTemplate );
    
    enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::EData);
    TRAP(err,doc = enc->EncryptDataL(pt4));
    LOCAL_ASSERT( err == KErrEncrypt );

    enc->SetKeyFromFileL(p_AesKey8,pt4,CXmlSecEncrypt::EAES256);
    enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::EData);
    doc = enc->EncryptDataL(pt4);
    LOCAL_ASSERT( doc.NotNull() );
    doc.Close();
    
    enc->SetKeyFromFileL(p_DesKey8,pt4,CXmlSecEncrypt::E3DES);
    enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::EData);
    doc = enc->EncryptDataL(pt4);
    LOCAL_ASSERT( doc.NotNull() );
    doc.Close();
    
    TRAP(err,doc = enc->EncryptDataL(pt5));
    LOCAL_ASSERT( err == KErrWrongParameter );
        
    enc->SetKeyFromFileL(p_AesKey8,pt4,CXmlSecEncrypt::EAES256);
    enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::EData);
    TRAP(err,doc = enc->EncryptDataL(pt4));
    LOCAL_ASSERT( err == KErrEncrypt );
    
    TRAP(err,doc = enc->EncryptDataL(pt4));
    LOCAL_ASSERT( err == KErrEncrypt );
    
    TRAP(err,doc = enc->EncryptDataL(pt4));
    LOCAL_ASSERT( err == KErrEncrypt );
    
    enc->SetTemplateFromFileL(pTem8);
    enc->SetKeyFromFileL(p_AesKey8,pt4,CXmlSecEncrypt::EAES256);
    TRAP(err,doc = enc->EncryptDataL(pt4));
    LOCAL_ASSERT( err == KErrTemplate );
                
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufTem8);
    CleanupStack::PopAndDestroy(p_BufAesKey8);
    CleanupStack::PopAndDestroy(p_BufDesKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlDocumentLL( CStifItemParser& aItem )
    {
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);

    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();

   	XmlSecPushL();    
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument doc;
    TRAPD(err,enc->EncryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrWrongParameter );
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    enc->SetKeyFromFileL(p_DesKey8,pt4,CXmlSecEncrypt::E3DES);
    enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::ENode);
    enc->EncryptXmlDocumentL(doc);
    LOCAL_ASSERT( doc.NotNull() );
    LOCAL_ASSERT( doc.DocumentElement().Name().Compare(_L8("test")) );
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufDesKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlDocumentKeyFromFileLL( CStifItemParser& aItem )
    {
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);

    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();

   	XmlSecPushL();    
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument doc;
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument temp = enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::ENode).CloneDocumentL();
    enc->DestroyCurrentTemplate();
    enc->EncryptXmlDocumentKeyFromFileL(doc,temp,p_DesKey8,pt4,CXmlSecEncrypt::E3DES);
    LOCAL_ASSERT( doc.NotNull() );
    LOCAL_ASSERT( doc.DocumentElement().Name().Compare(_L8("test")) );
    temp.Close();
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufDesKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//   
TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlDocumentKeyFromBufferLL(  )
    {
    _LIT8(t1,"012345670123456701234567");
    TPtrC8 pt1(t1);
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);
    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument doc;
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument temp = enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::ENode).CloneDocumentL();
    enc->DestroyCurrentTemplate();
    enc->EncryptXmlDocumentKeyFromBufferL(doc,temp,pt1,pt4,CXmlSecEncrypt::E3DES);
    LOCAL_ASSERT( doc.NotNull() );
    LOCAL_ASSERT( doc.DocumentElement().Name().Compare(_L8("test")) );
    temp.Close();
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//   
 TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlNodeLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);

    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();
    
    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();
    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
        
    TRAPD(err,enc->EncryptXmlNodeL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrWrongParameter );
            
    TXmlEngElement elem = doc.CreateElementL(_L8("doc"));
    TXmlEngComment com = doc.CreateCommentL(_L8("test"));
    doc.AppendChildL(com);
    doc.SetDocumentElement(elem);
    TXmlEngElement elem2 = doc.CreateElementL(_L8("elem1"));
    elem2.SetTextL(_L8("testtext"));
    elem.AppendChildL(elem2);
    elem2 = doc.CreateElementL(_L8("elem2"));
    elem.AppendChildL(elem2);
    
    TRAP(err,enc->EncryptXmlNodeL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrTemplate );
    
    enc->SetTemplateL(doc);
    TRAP(err,enc->EncryptXmlNodeL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrTemplate );
    
    RXmlEngDocument tmp = doc.CloneDocumentL();
    enc->SetKeyFromFileL(p_DesKey8,pt4,CXmlSecEncrypt::E3DES);
    enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::ENode);
    enc->EncryptXmlNodeL(tmp.DocumentElement());
    LOCAL_ASSERT( tmp.NotNull() );
    LOCAL_ASSERT( tmp.DocumentElement().Name().Compare(_L8("doc")) );
    tmp.Close();
    
    tmp = doc.CloneDocumentL();
    enc->SetKeyFromFileL(p_AesKey8,pt4,CXmlSecEncrypt::EAES256);
    enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::ENode);
    enc->EncryptXmlNodeL(tmp.DocumentElement().FirstChild().AsElement());
    LOCAL_ASSERT( tmp.NotNull() );
    LOCAL_ASSERT( !tmp.DocumentElement().Name().Compare(_L8("doc")) );
    LOCAL_ASSERT( tmp.DocumentElement().FirstChild().Name().Compare(_L8("elem1")) );
    tmp.Close();
    
    tmp = doc.CloneDocumentL();
    enc->SetKeyFromFileL(p_AesKey8,pt4,CXmlSecEncrypt::EAES256);
    enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::ENodeContent);
    enc->EncryptXmlNodeL(tmp.DocumentElement().FirstChild().AsElement());
    LOCAL_ASSERT( tmp.NotNull() );
    LOCAL_ASSERT( !tmp.DocumentElement().Name().Compare(_L8("doc")) );
    LOCAL_ASSERT( !tmp.DocumentElement().FirstChild().Name().Compare(_L8("elem1")) );
    tmp.Close();
               
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufAesKey8);
    CleanupStack::PopAndDestroy(p_BufDesKey8);
    return KErrNone;
    }
 
//--------------------------------------------------------------------------------
//    
TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlNodeKeyFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);

    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();
    
   	XmlSecPushL();
   	CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument doc;
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("doc"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument temp = enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::ENode).CloneDocumentL();
    enc->DestroyCurrentTemplate();
    enc->EncryptXmlNodeKeyFromFileL(doc.DocumentElement(),temp,p_DesKey8,pt4,CXmlSecEncrypt::E3DES);
    LOCAL_ASSERT( doc.NotNull() );
    LOCAL_ASSERT( doc.DocumentElement().Name().Compare(_L8("doc")) );
    temp.Close();
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufDesKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//    
TInt CXmlSecEncryptTester::CXmlSecEncrypt_EncryptXmlNodeKeyFromBufferLL()
    {
    _LIT8(t1,"012345670123456701234567");
    TPtrC8 pt1(t1);
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);
    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument doc;
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("doc"));
    doc.SetDocumentElement(elem);
    RXmlEngDocument temp = enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::ENode).CloneDocumentL();
    enc->DestroyCurrentTemplate();
    enc->EncryptXmlNodeKeyFromBufferL(doc.DocumentElement(),temp,pt1,pt4,CXmlSecEncrypt::E3DES);
    LOCAL_ASSERT( doc.NotNull() );
    LOCAL_ASSERT( doc.DocumentElement().Name().Compare(_L8("doc")) );
    temp.Close();
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }


//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlNodeLL(CStifItemParser& aItem)
    {
    _LIT8(t1n,"name");
    TPtrC8 pt1n(t1n);

    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();
    
    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();
    
    TPtrC p_AesDoc;
    aItem.GetNextString(p_AesDoc);

    TPtrC p_DesDoc;
    aItem.GetNextString(p_DesDoc);

    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDOMParser parser;
    parser.Open(domImp);
    
    RXmlEngDocument doc ; 
    HBufC8* buf = NULL;
    TRAPD(err,buf=enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrWrongParameter );
    
    doc=parser.ParseFileL(p_AesDoc);
    
    TRAP(err,buf = enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrDecrypt );

    enc->SetKeyFromFileL(p_AesKey8,pt1n,CXmlSecEncrypt::EAES256);
    TRAP(err,buf = enc->DecryptXmlNodeL(doc.AsElement()));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1") ));
    LOCAL_ASSERT( buf == NULL );
    doc.Close();

    enc->SetKeyFromFileL(p_AesKey8,pt1n,CXmlSecEncrypt::EAES256);
    doc = parser.ParseFileL(p_AesDoc);
    TRAP(err,buf = enc->DecryptXmlNodeL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1")) );
    LOCAL_ASSERT( buf == NULL );
    doc.Close();
    enc->SetKeyFromFileL(p_DesKey8,pt1n,CXmlSecEncrypt::E3DES);
    doc = parser.ParseFileL(p_DesDoc);
    TRAP(err,buf = enc->DecryptXmlNodeL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().Name().Compare(_L8("doc") ));
    LOCAL_ASSERT( buf == NULL );
    doc.Close();
    parser.Close();
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufAesKey8);
    CleanupStack::PopAndDestroy(p_BufDesKey8);    
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlNodeKeyFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t1n,"name");
    TPtrC8 pt1n(t1n);
    
    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();
    
    TPtrC p_AesDoc;
    aItem.GetNextString(p_AesDoc);

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDOMParser parser;
    parser.Open(domImp);
    RXmlEngDocument doc = parser.ParseFileL(p_AesDoc);
    HBufC8* buf = NULL;
    TRAPD(err,buf = enc->DecryptXmlNodeKeyFromFileL(doc.DocumentElement(),p_AesKey8,pt1n,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1")) );
    LOCAL_ASSERT( buf == NULL );
    
    doc.Close();
    parser.Close();
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufAesKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlNodeKeyFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t1a,"01234567012345670123456701234567");
    TPtrC8 pt1a(t1a);
    _LIT8(t1n,"name");
    TPtrC8 pt1n(t1n);
    
    TPtrC p_AesDoc;
    aItem.GetNextString(p_AesDoc);

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDOMParser parser;
    parser.Open(domImp);
    RXmlEngDocument doc = parser.ParseFileL(p_AesDoc);
    HBufC8* buf = NULL;
    TRAPD(err,buf = enc->DecryptXmlNodeKeyFromBufferL(doc.DocumentElement(),pt1a,pt1n,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1")) );
    LOCAL_ASSERT( buf == NULL );
    doc.Close();
    parser.Close();
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlDocumentLL(CStifItemParser& aItem)
    {
    _LIT8(t1n,"name");
    TPtrC8 pt1n(t1n);
    _LIT8(t3,"name");
    TPtrC8 pt3(t3);
    
    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();

    TPtrC p_AesElem;
    aItem.GetNextString(p_AesElem);

    TPtrC p_AesData;
    aItem.GetNextString(p_AesData);
    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDOMParser parser;
    parser.Open(domImp);
    
    RXmlEngDocument doc;
    TRAPD(err,enc->DecryptXmlNodeL(doc.AsElement()));
    LOCAL_ASSERT( err == KErrWrongParameter );
        
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    
    TRAP(err,enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrWrongParameter );
            
    TXmlEngElement elem = doc.CreateElementL(_L8("doc"));
    doc.SetDocumentElement(elem);
    TXmlEngElement elem2 = doc.CreateElementL(_L8("elem1"));
    elem2.SetTextL(_L8("testtext"));
    elem.AppendChildL(elem2);
    elem2 = doc.CreateElementL(_L8("elem2"));
    elem.AppendChildL(elem2);
    
    TRAP(err,enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrWrongParameter );
    doc.Close();

    doc = parser.ParseFileL(p_AesElem);
    HBufC8* buf = NULL;
    TRAP(err,buf = enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrDecrypt );
    
    enc->SetKeyFromFileL(p_AesKey8,pt1n,CXmlSecEncrypt::EAES256);
    TRAP(err,buf = enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1")) );
    LOCAL_ASSERT( buf == NULL );
    doc.Close();
    enc->SetKeyFromFileL(p_AesKey8,pt1n,CXmlSecEncrypt::EAES256);
    doc = parser.ParseFileL(p_AesData);
    TRAP(err,buf = enc->DecryptXmlDocumentL(doc));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !buf->Compare(pt3) );
    
    doc.Close();
    parser.Close();
    domImp.Close();    
    CleanupStack::PopAndDestroy(&doc);
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufAesKey8);  
    delete buf;
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlDocumentKeyFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t1n,"name");
    TPtrC8 pt1n(t1n);
    _LIT8(t3,"name");
    TPtrC8 pt3(t3);

    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();

    TPtrC p_AesElem;
    aItem.GetNextString(p_AesElem);

    TPtrC p_AesData;
    aItem.GetNextString(p_AesData);

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDOMParser parser;
    parser.Open(domImp);
    
    RXmlEngDocument doc;
    CleanupClosePushL(doc);
    doc = parser.ParseFileL(p_AesElem);
    HBufC8* buf = NULL;
    TRAPD(err,buf = enc->DecryptXmlDocumentKeyFromFileL(doc,p_AesKey8,pt1n,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1")) );
    LOCAL_ASSERT( buf == NULL );
    doc.Close();
    doc = parser.ParseFileL(p_AesData);
    TRAP(err,buf = enc->DecryptXmlDocumentKeyFromFileL(doc,p_AesKey8,pt1n,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !buf->Compare(pt3) );
    delete buf;
    doc.Close();
    parser.Close();
    domImp.Close();        
    CleanupStack::PopAndDestroy(&doc);
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufAesKey8);      
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_DecryptXmlDocumentKeyFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t1,"01234567012345670123456701234567");
    TPtrC8 pt1(t1);
    _LIT8(t1n,"name");
    TPtrC8 pt1n(t1n);
    _LIT8(t3,"name");
    TPtrC8 pt3(t3);
    
    TPtrC p_AesElem;
    aItem.GetNextString(p_AesElem);

    TPtrC p_AesData;
    aItem.GetNextString(p_AesData);

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDOMParser parser;
    parser.Open(domImp);
    
    RXmlEngDocument doc;
    CleanupClosePushL(doc);
    doc = parser.ParseFileL(p_AesElem);
    HBufC8* buf = NULL;
    TRAPD(err,buf = enc->DecryptXmlDocumentKeyFromBufferL(doc,pt1,pt1n,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !doc.DocumentElement().FirstChild().NextSibling().Name().Compare(_L8("elem1")) );
    LOCAL_ASSERT( buf == NULL );
    doc.Close();
    doc = parser.ParseFileL(p_AesData);
    TRAP(err,buf = enc->DecryptXmlDocumentKeyFromBufferL(doc,pt1,pt1n,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrNone );
    LOCAL_ASSERT( !buf->Compare(pt3) );
    delete buf;
    doc.Close();
            
    CleanupStack::PopAndDestroy(&doc);
    parser.Close();
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateFromFileL_1L(CStifItemParser& aItem)
    {
    TPtrC pTem;
    aItem.GetNextString(pTem);
    HBufC8* p_BufTem8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pTem);
    CleanupStack::PushL( p_BufTem8 );
    TPtrC8 pTem8 = p_BufTem8->Des();

    TPtrC pTemWrng;
    aItem.GetNextString(pTemWrng);
    HBufC8* p_BufTemWrng8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pTemWrng);
    CleanupStack::PushL( p_BufTemWrng8 );
    TPtrC8 pTemWrng8 = p_BufTemWrng8->Des();

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();

    enc->SetTemplateFromFileL(pTem8);
    RXmlEngDocument tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("doc")) );
    
    TRAPD(err,enc->SetTemplateFromFileL(pTemWrng8));
    LOCAL_ASSERT( err );
    
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufTemWrng8);
    CleanupStack::PopAndDestroy(p_BufTem8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//   
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateFromFileL_2L(CStifItemParser& aItem)
    {
    TPtrC pTem;
    aItem.GetNextString(pTem);
    HBufC8* p_BufTem8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pTem);
    CleanupStack::PushL( p_BufTem8 );
    TPtrC8 pTem8 = p_BufTem8->Des();
    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
        
    RFs fs;
    fs.Connect();
    enc->SetTemplateFromFileL(fs,pTem8);
    fs.Close();
    RXmlEngDocument tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("doc")) );
    
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufTem8);    
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateFromBufferLL()
    {
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    _LIT8(t1,"<doc></doc>");
    TPtrC8 pt1(t1);
    enc->SetTemplateFromBufferL(pt1);
    RXmlEngDocument tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("doc")) );
    
    _LIT8(t2,"<doc>");
    TPtrC8 pt2(t2);
    TRAPD(err,enc->SetTemplateFromBufferL(pt2));
    LOCAL_ASSERT( err );
    
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetTemplateLL()
    {
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();

    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    enc->SetTemplateL(doc);
    CleanupStack::PopAndDestroy(&doc);
    RXmlEngDocument tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("test") ));
    
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    elem = doc.CreateElementL(_L8("test2"));
    doc.SetDocumentElement(elem);
    
    enc->SetTemplateL(doc);
    CleanupStack::PopAndDestroy(&doc);
    tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("test2") ));
    RXmlEngDocument doc2;
    TRAPD(err, enc->SetTemplateL(doc2));
    LOCAL_ASSERT( err == KErrTemplate );
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_CreateTemplateLL()
    {
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    RXmlEngDocument tem = enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::ENode);
    LOCAL_ASSERT( tem.NotNull() );
    
    tem = enc->CreateTemplateL(CXmlSecEncrypt::E3DES,CXmlSecEncrypt::EData);
    LOCAL_ASSERT( tem.NotNull() );
    
    CleanupStack::PopAndDestroy(enc);        
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_GetCurrentTemplateLL()
    {
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();

    RXmlEngDocument tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.IsNull() );
    enc->DestroyCurrentTemplate();
    tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.IsNull() );
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    
    enc->SetTemplateL(doc);
    tem = enc->CurrentTemplate();
    
    LOCAL_ASSERT( tem.NotNull() );
    LOCAL_ASSERT( !tem.DocumentElement().Name().Compare(_L8("test") ));
        
    enc->DestroyCurrentTemplate();
    
    tem = enc->CurrentTemplate();
    LOCAL_ASSERT( tem.IsNull() );
    
    CleanupStack::PopAndDestroy(&doc);    
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyFromFileLL(CStifItemParser& aItem)
    {
    _LIT8(t1,"e:\\testing\\data\\xmlsecurity\\encrypt\\deskey.bin");
    TPtrC8 pt1(t1);
    _LIT8(t2,"e:\\testing\\data\\xmlsecurity\\encrypt\\aeskey.bin");
    TPtrC8 pt2(t2);
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);
    TPtrC8 pt5(KNullDesC8());

    TPtrC p_DesKey;
    aItem.GetNextString(p_DesKey);
    HBufC8* p_BufDesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_DesKey);
    CleanupStack::PushL( p_BufDesKey8 );
    TPtrC8 p_DesKey8 = p_BufDesKey8->Des();
    
    TPtrC p_AesKey;
    aItem.GetNextString(p_AesKey);
    HBufC8* p_BufAesKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_AesKey);
    CleanupStack::PushL( p_BufAesKey8 );
    TPtrC8 p_AesKey8 = p_BufAesKey8->Des();

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    //ok
    enc->SetKeyFromFileL(p_DesKey8,pt4,CXmlSecEncrypt::E3DES);
    enc->SetKeyFromFileL(p_AesKey8,pt4,CXmlSecEncrypt::EAES256); 
    //filename=null
    TRAPD(err,enc->SetKeyFromFileL(pt5,pt4,CXmlSecEncrypt::EAES256))
    LOCAL_ASSERT( err == KErrWrongParameter );
      
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufAesKey8);
    CleanupStack::PopAndDestroy(p_BufDesKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyFromBufferLL(CStifItemParser& aItem)
    {
    _LIT8(t1,"012345678901234567890123");
    TPtrC8 pt1(t1);
    _LIT8(t2,"01234567890123456789012345678901");
    TPtrC8 pt2(t2);
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);
    TPtrC8 pt5(KNullDesC8());

    TPtrC p_WrngKey;
    aItem.GetNextString(p_WrngKey);
    HBufC8* p_BufWrngKey8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_WrngKey);
    CleanupStack::PushL( p_BufWrngKey8 );
    TPtrC8 p_WrngKey8 = p_BufWrngKey8->Des();

    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    //ok
    enc->SetKeyFromBufferL(pt1,pt4,CXmlSecEncrypt::E3DES);
    enc->SetKeyFromBufferL(pt2,pt4,CXmlSecEncrypt::EAES256);
    //filename=null
    TRAPD(err,enc->SetKeyFromBufferL(pt5,pt4,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrWrongParameter );
    
    TRAP(err,enc->SetKeyFromBufferL(p_WrngKey8,pt4,CXmlSecEncrypt::EAES256));
    LOCAL_ASSERT( err == KErrKey );
    
    TRAP(err,enc->SetKeyFromBufferL(p_WrngKey8,pt4,CXmlSecEncrypt::E3DES));
    LOCAL_ASSERT( err == KErrKey);
    
    TRAP(err,enc->SetKeyFromBufferL(pt1,pt5,CXmlSecEncrypt::E3DES));
    LOCAL_ASSERT( err == KErrNone);
    
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    CleanupStack::PopAndDestroy(p_BufWrngKey8);
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyInfoL_1L(  )
    {
    _LIT8(t4,"name");
    TPtrC8 pt4(t4);
    TPtrC8 pt5(KNullDesC8());
    
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    
    //no template
    TRAPD(err,enc->SetKeyInfoL(pt4));
    LOCAL_ASSERT( err == KErrTemplate );
    //null param
    enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::EData);
    TRAP(err,enc->SetKeyInfoL(pt5));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //ok
    enc->SetKeyInfoL(pt4);
    //no keyinfo
    enc->SetTemplateL(doc);
    TRAP(err,enc->SetKeyInfoL(pt4));
    LOCAL_ASSERT( err == KErrTemplate );
    
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }
//--------------------------------------------------------------------------------
//
TInt CXmlSecEncryptTester::CXmlSecEncrypt_SetKeyInfoL_2L()
    {
    XmlSecPushL();
    CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
    
    RXmlEngDOMImplementation domImp;
    domImp.OpenL();
    RXmlEngDocument doc;
    doc.OpenL(domImp);
    CleanupClosePushL(doc);
    TXmlEngElement elem = doc.CreateElementL(_L8("test"));
    doc.SetDocumentElement(elem);
    TXmlEngElement elem1; 
    
    //no template
    TRAPD(err,enc->SetKeyInfoL(doc.AsElement()));
    LOCAL_ASSERT( err == KErrTemplate );
    enc->CreateTemplateL(CXmlSecEncrypt::EAES256,CXmlSecEncrypt::EData);
    //wrong element = null
    TRAP(err,enc->SetKeyInfoL(elem1));
    LOCAL_ASSERT( err == KErrWrongParameter );
    //ok 
    enc->SetKeyInfoL(elem.CopyL());
    //no keyinfo node
    enc->SetTemplateL(doc);
    TRAP(err,enc->SetKeyInfoL(doc.DocumentElement()));
    LOCAL_ASSERT( err == KErrTemplate );
     
    CleanupStack::PopAndDestroy(&doc);
    domImp.Close();
    CleanupStack::PopAndDestroy(enc);
    XmlSecPopAndDestroy();
    return KErrNone;
    }    
//  END OF FILE
