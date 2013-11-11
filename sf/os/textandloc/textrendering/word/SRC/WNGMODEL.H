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


#if !defined(__WNGMODEL_H__)
#define __WNGMODEL_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__S32STD_H__)
#include <s32std.h>
#endif

//
// Classed decalred in this file
class CWordModel;
class CWordProfileModel;
//
// Classes referenced
class RReadStream;
class RWriteStream;
class CSecureStore;
class CParaFormatLayer;
class CCharFormatLayer;
class CRichText;
class CStyleList;
class CPrintSetup;
class MFieldFileNameInfo;
class MFieldNumPagesInfo;
class CStreamStore;
class CStreamDictionary;

/** Type for an array of style short cuts.

@see CWordModel::StyleShortCutList() 
@internalComponent
*/
typedef CArrayFixFlat<TChar> CStyleShortCutList;



class CWordModel : public CBase 
/** Word processor engine interface.

This is essentially a thin layer over the rich text support (CRichText etc.) 
provided by the Application Framework's ETEXT component. 
@internalComponent
*/
	{
public:
	IMPORT_C static CWordModel* NewL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aFieldNumPagesInfo,const TDesC& aDriverPath);
	IMPORT_C static CWordModel* NewL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aFieldNumPagesInfo);
	// Performs no initialisation at all.
	//
	IMPORT_C ~CWordModel();
	//
	// Save/Load
#ifndef __SECURE_API__
	IMPORT_C void StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic,const CSecurityBase* aSecurity)const;
	IMPORT_C void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic,const CSecurityBase* aSecurity,
						   const MFieldFileNameInfo* aFileNameInfo=NULL,const MFieldNumPagesInfo* aNumPagesInfo=NULL,
						   MPictureFactory* aPictureFactory=NULL);
	IMPORT_C void RestoreMinimalL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic,const CSecurityBase* aSecurity,
						   const MFieldFileNameInfo* aFileNameInfo=NULL,const MFieldNumPagesInfo* aNumPagesInfo=NULL,
						   MPictureFactory* aPictureFactory=NULL);
#else						   
	IMPORT_C void StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic,const TAny* aSecurity)const;
	IMPORT_C void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic,const TAny* aSecurity,
						   const MFieldFileNameInfo* aFileNameInfo=NULL,const MFieldNumPagesInfo* aNumPagesInfo=NULL,
						   MPictureFactory* aPictureFactory=NULL);
	IMPORT_C void RestoreMinimalL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic,const TAny* aSecurity,
						   const MFieldFileNameInfo* aFileNameInfo=NULL,const MFieldNumPagesInfo* aNumPagesInfo=NULL,
						   MPictureFactory* aPictureFactory=NULL);
#endif

	//
	// Handles
	inline CRichText* Text();
	inline CStyleList* StyleList();
	inline CPrintSetup* PrintSetup();
	inline CArrayFix<TInt>* PageTable();
	inline CStyleShortCutList* StyleShortCutList();
	inline TChar& NormalStyleShortCut();
protected:
	IMPORT_C CWordModel();
	IMPORT_C void ConstructL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aNumPagesInfo);
	IMPORT_C void ConstructMinimalL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aFieldNumPagesInfo,const TDesC& aDriverPath);
	void ConstructGlobalLayersL();
	//
	TStreamId StoreStylesL(CStreamStore& aStore)const;
	void RestoreStylesL(const CStreamStore& aStore,TStreamId aId);
	//
	TStreamId StoreFieldDataL(CStreamStore& aStore)const;
	void RestoreFieldDataL(const CStreamStore& aStore,TStreamId aId);
#ifndef __SECURE_API__
	TStreamId StoreTextDataL(CStreamStore& aStore,const CSecurityBase* aSecurity)const;
	void RestoreTextDataL(const CStreamStore& aStore,TStreamId aId,const CSecurityBase* aSecurity);
#else
	TStreamId StoreTextDataL(CStreamStore& aStore,const TAny* aSecurity)const;
	void RestoreTextDataL(const CStreamStore& aStore,TStreamId aId,const TAny* aSecurity);
#endif

	TStreamId StoreMarkupDataL(CStreamStore& aStore)const;
	void RestoreMarkupDataL(const CStreamStore& aStore,TStreamId aId);
private:
	void InitPrintSetupL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aNumPagesInfo,const TDesC& aDriverPath);
private:
	CParaFormatLayer* iParaFormatLayer;
	CCharFormatLayer* iCharFormatLayer;
	CRichText* iText;
	CStyleList* iStyleList;
	CPrintSetup* iPrintSetup;
	CStyleShortCutList* iStyleShortCutList;
	TChar iNormalStyleShortCut;
	CArrayFix<TInt>* iPageTable;
	};


#include "WNGMODEL.INL"


#endif
