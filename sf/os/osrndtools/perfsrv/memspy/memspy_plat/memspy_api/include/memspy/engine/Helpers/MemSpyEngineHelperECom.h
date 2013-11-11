/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYENGINEHELPERECOM_H
#define MEMSPYENGINEHELPERECOM_H

// System includes
#include <e32base.h>
#include <badesca.h>
#include <ecom/ecom.h>
#include <xml/contenthandler.h> // MContentHandler mix in class
#include <xml/parser.h>

// Literal constants
_LIT( KMemSpyEComInterfaceIdFileName, "MemSpyEComInterfaceIds.xml" );

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineEComCategory;
class CMemSpyEngineEComInterface;
class CMemSpyEngineEComImplementation;
class CCnvCharacterSetConverter;

// Namespaces referenced
using namespace Xml;


NONSHARABLE_CLASS( CMemSpyEngineHelperECom ) : public CBase, public MContentHandler, public MDesCArray
    {
public:
    static CMemSpyEngineHelperECom* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperECom();

private:
    CMemSpyEngineHelperECom( CMemSpyEngine& aEngine );
    void ConstructL();
    void ConstructInternalL();
    void ConstructFromFileL();

public: // Internal API
    REComSession& EComSession();
    inline TInt Count() const { return iCategories.Count(); }
    IMPORT_C CMemSpyEngineEComCategory& At( TInt aIndex );
    IMPORT_C TInt IndexOf( const CMemSpyEngineEComCategory& aEntry );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Internal methods
    HBufC* FindEComXmlFileNameLC();
    void ConvertL( const TDesC8& aInput, TDes16& aOutput );
    void ParseL( const TDesC& aFileName );

private: // XML helper functions
    void OnChildSectionCategoryL( const RAttributeArray& aAttributes );
    void OnChildSectionInterfaceL( const RAttributeArray& aAttributes );

private: // From MContentHandler
	void OnStartDocumentL( const RDocumentParameters& aDocParam, TInt aErrorCode );
	void OnEndDocumentL( TInt aErrorCode );
	void OnStartElementL( const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode );
	void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );
	void OnContentL( const TDesC8& aBytes, TInt aErrorCode );
	void OnStartPrefixMappingL( const RString& aPrefix, const RString& aUri, TInt aErrorCode );
	void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );
	void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );
	void OnSkippedEntityL( const RString& aName, TInt aErrorCode );
	void OnProcessingInstructionL( const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode );
	void OnError( TInt aErrorCode );
	TAny* GetExtendedInterface( const TInt32 aUid );

private:
    CMemSpyEngine& iEngine;
    REComSession iEComSession;
    TInt iParserErrorCode;
    TInt iCharconvConverterState;
    TBool iSeenMasterSection;
    CParser* iParser;
    CCnvCharacterSetConverter* iConverter;
    RPointerArray<CMemSpyEngineEComCategory> iCategories;
    };







NONSHARABLE_CLASS( CMemSpyEngineEComCategory ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineEComCategory* NewLC( CMemSpyEngineHelperECom& aHelper, const TDesC& aName );
    ~CMemSpyEngineEComCategory();

private:
    CMemSpyEngineEComCategory( CMemSpyEngineHelperECom& aHelper );
    void ConstructL( const TDesC& aName );

public: // API
    inline const TDesC& Name() const { return *iName; }
    inline const TDesC& Caption() const { return *iCaption; }
    inline REComSession& EComSession() { return iHelper.EComSession(); }
    inline TInt Count() const { return iInterfaces.Count(); }
    IMPORT_C CMemSpyEngineEComInterface& At( TInt aIndex );
    IMPORT_C TInt IndexOf( const CMemSpyEngineEComInterface& aEntry );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

public: // Iternal API
    void CreateInterfaceL( const TDesC& aName, TUid aUid );
    void BuildCaptionL();

private:
    CMemSpyEngineHelperECom& iHelper;
    RPointerArray<CMemSpyEngineEComInterface> iInterfaces;
    HBufC* iName;
    HBufC* iCaption;
    };








NONSHARABLE_CLASS( CMemSpyEngineEComInterface ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineEComInterface* NewLC( CMemSpyEngineEComCategory& aCategory, const TDesC& aName, TUid aUid );
    ~CMemSpyEngineEComInterface();

private:
    CMemSpyEngineEComInterface( CMemSpyEngineEComCategory& aCategory, TUid aUid );
    void ConstructL( const TDesC& aName );

public: // API
    inline const TDesC& Name() const { return *iName; }
    inline const TDesC& Caption() const { return *iCaption; }
    inline TInt Count() const { return iImplementations.Count(); }
    inline CMemSpyEngineEComCategory& Category() const { return iCategory; }
    IMPORT_C CMemSpyEngineEComImplementation& At( TInt aIndex );
    IMPORT_C TInt IndexOf( const CMemSpyEngineEComImplementation& aEntry );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Data members
    TUid iUid;
    CMemSpyEngineEComCategory& iCategory;
    RPointerArray<CMemSpyEngineEComImplementation> iImplementations;
    HBufC* iName;
    HBufC* iCaption;
    };










NONSHARABLE_CLASS( CMemSpyEngineEComImplementation ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineEComImplementation* NewLC( CMemSpyEngineEComInterface& aInterface, CImplementationInformation* aInfo );
    ~CMemSpyEngineEComImplementation();

private:
    CMemSpyEngineEComImplementation( CMemSpyEngineEComInterface& aInterface );
    void ConstructL( CImplementationInformation* aInfo );

public: // API
    inline const TDesC& Name() const { return *iName; }
    inline const TDesC& Caption() const { return *iCaption; }
    inline CMemSpyEngineEComInterface& Interface() const { return iInterface; }
    inline CImplementationInformation& Info() const { return *iInfo; }

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Internal methods
    static HBufC* CopyAndCropLC( const TDesC8& aText );

private: // Data members
    CMemSpyEngineEComInterface& iInterface;
    CImplementationInformation* iInfo;
    CDesCArray* iInfoText;
    HBufC* iName;
    HBufC* iCaption;
    };



#endif
