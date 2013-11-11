/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__WNGDOOR_H__)
#define __WNGDOOR_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__APAMDR_H__)
#include <apamdr.h>
#endif

//
// Classes defined
class CWordModelHeader;
class TWordModelHeaderFactory;
class CWordModelHeaderV2;
class TWordModelHeaderFactoryV2;
//
// Class referenced
class CWordModel;
class TWordModelStoreResolver;

/** Application UID for word processor application. 
@internalComponent
@deprecated
*/
const TUid KUidWordApp = {0x10003A64};


//
// Takes ownership of the referenced word model
//
class CWordModelHeaderV2 : public CApaModelHeader
/** Wrapper for the word processor application engine.

The class provides the knowledge for internalising and externalising an application 
model's data. It enables word processor documents to be embedded within rich 
text objects.

Compared to CWordModelHeader, it also provides restoration support for objects 
embedded within embedded word documents. 
@internalComponent
*/
	{
	friend class TWordModelHeaderFactoryV2;
public:
	IMPORT_C ~CWordModelHeaderV2();
public:	// from CApaModelHeader
	IMPORT_C void StoreL(CStreamStore& aStore,CStreamDictionary& aDict)const;
	IMPORT_C TApaAppIdentifier AppId()const;
	IMPORT_C void DetachFromStoreL(CPicture::TDetach aDegree);
public:
	IMPORT_C void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aDict);
	IMPORT_C CWordModel* Model();
	void SetFactory(MApaModelHeaderFactory* aFactory); // takes ownership
private:
	CWordModelHeaderV2(CWordModel* aModel);
private:
	CWordModel*	iModel;
	TWordModelStoreResolver* iResolver; // created during restore
	MApaModelHeaderFactory* iHeaderFactory;
	};


//
//

class TWordModelHeaderFactoryV2 : public MApaModelHeaderFactory
/** Factory to construct CWordModelHeaderV2 application wrapper objects.
@internalComponent
*/
	{
public:
	IMPORT_C TWordModelHeaderFactoryV2(const TFileName& aPrintDriverPath);
	IMPORT_C CApaModelHeader* NewHeaderL(const CStreamStore& aStore,const CStreamDictionary& aDict,
										const TApaAppIdentifier& aAppId)const;
private:
	TFileName iPrintDriverPath;
	};



class CWordModelHeaderV3 : public CApaModelHeader
/** Wrapper for the word processor application engine.

The class provides the knowledge for internalising and externalising an application 
model's data. It enables word processor documents to be embedded within rich 
text objects.

Like CWordModelHeaderV2, it provides restoration support for objects embedded 
within embedded word documents. Unlike that class, however, it does not restore 
printer setup information, or process header and footer information. 
@internalComponent
*/
	{
	friend class TWordModelHeaderFactoryV3;
public:
	IMPORT_C ~CWordModelHeaderV3();
public:	// from CApaModelHeader
	IMPORT_C void StoreL(CStreamStore& aStore,CStreamDictionary& aDict)const;
	IMPORT_C TApaAppIdentifier AppId()const;
	IMPORT_C void DetachFromStoreL(CPicture::TDetach aDegree);
public:
	IMPORT_C void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aDict);
	IMPORT_C CWordModel* Model();
	void SetFactory(MApaModelHeaderFactory* aFactory); // takes ownership
private:
	CWordModelHeaderV3(CWordModel* aModel);
private:
	CWordModel*	iModel;
	TWordModelStoreResolver* iResolver; // created during restore
	MApaModelHeaderFactory* iHeaderFactory;
	};


// Mimimal header factory for efficiency
// Does not restore header or footer information from the document
//
class TWordModelHeaderFactoryV3 : public MApaModelHeaderFactory
/** Factory to construct CWordModelHeaderV3 application wrapper objects. 
@internalComponent
*/
	{
public:
	IMPORT_C TWordModelHeaderFactoryV3();
	IMPORT_C CApaModelHeader* NewHeaderL(const CStreamStore& aStore,const CStreamDictionary& aDict,
										const TApaAppIdentifier& aAppId) const;
	};


#endif
