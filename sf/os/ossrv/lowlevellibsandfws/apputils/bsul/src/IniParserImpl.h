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
// @internalComponent
// 
//

#ifndef __INIPARSERIMPL_H__
#define __INIPARSERIMPL_H__

#include <e32std.h>
#include <f32file.h>
#include "IniTemplate.h"
#include "inifile.h"

namespace BSUL
{

//typedef for 8 and 16 bit
typedef CIniLine<TDesC8,HBufC8,TPtrC8,TPtr8> CIniLine8;
typedef CIniLine<TDesC16,HBufC16,TPtrC16,TPtr16> CIniLine16;

typedef CIniKey<TDesC8,HBufC8,TPtrC8,CIniLine8,TPtr8,TLex8> CIniKey8;
typedef CIniKey<TDesC16,HBufC16,TPtrC16,CIniLine16,TPtr16,TLex16> CIniKey16;

typedef CIniSection<TDesC8,HBufC8,TPtrC8,CIniKey8,CIniLine8,TPtr8,TLex8> CIniSection8;
typedef CIniSection<TDesC16,HBufC16,TPtrC16,CIniKey16,CIniLine16,TPtr16,TLex16> CIniSection16;

typedef CIniSecIterImplX<TDesC8,TPtrC8,CIniSection8,CIniDocument8,CIniKey8> CIniSecIterTmpl8;
typedef CIniSecIterImplX<TDesC16,TPtrC16,CIniSection16,CIniDocument16,CIniKey16> CIniSecIterTmpl16;

typedef CIniDocumentTmplX<TDesC8,TPtrC8,CIniSection8,TLex8,TPtr8,HBufC8,CIniLine8,CIniKey8> CIniDocumentTmpl8;
typedef CIniDocumentTmplX<TDesC16,TPtrC16,CIniSection16,TLex16,TPtr16,HBufC16,CIniLine16,CIniKey16> CIniDocumentTmpl16;

typedef TBuf8<6> TPattern8;
typedef TBuf16<6> TPattern16;

typedef CIniFileImplX<TDesC8,TPtrC8,HBufC8,TLex8,TPtr8,TPattern8,CIniLine8> CIniFileTmpl8;
typedef CIniFileImplX<TDesC16,TPtrC16,HBufC16,TLex16,TPtr16,TPattern16,CIniLine16> CIniFileTmpl16;

//8 bit light
NONSHARABLE_CLASS(CIniFile8Impl): public CBase
	{
public:
	static CIniFile8Impl* NewL(RFs& aFs,const TDesC& aFileName);
	~CIniFile8Impl(){delete iTempImpl;}
	TInt FindVar(const TDesC8& aSection,const TDesC8& aKey,TPtrC8& aValue);
private:
	CIniFile8Impl(){}
	void ConstructL(RFs& aFs,const TDesC& aFileName);
	CIniFileTmpl8* iTempImpl;	
	};
	
//16 bit light	
NONSHARABLE_CLASS(CIniFile16Impl): public CBase
	{
public:
	static CIniFile16Impl* NewL(RFs& aFs,const TDesC& aFileName,TBool aConvert8To16=EFalse);
	~CIniFile16Impl(){delete iTempImpl;}
	TInt FindVar(const TDesC16& aSection,const TDesC16& aKey,TPtrC16& aValue);
private:
	CIniFile16Impl(){}
	void ConstructL(RFs& aFs,const TDesC& aFileName,TBool aConvert8To16);
	CIniFileTmpl16* iTempImpl;	
	};
	
//8 bit
NONSHARABLE_CLASS(CIniSecIter8Impl): public CBase
{
public:
	static CIniSecIter8Impl* NewL(const TDesC8& aSectionName,const CIniDocument8* aIniDocument);
	TBool Next(TPtrC8& aKeyName,TPtrC8& aKeyValue);
	void Reset();
	TBool End(){return iTempImpl->End();}
	~CIniSecIter8Impl(){delete iTempImpl;}
private:
	CIniSecIter8Impl(){}
	void ConstructL(const TDesC8& aSectionName,const CIniDocument8* aIniDocument);
private:	
	CIniSecIterTmpl8* iTempImpl;
};

//16 bit
NONSHARABLE_CLASS(CIniSecIter16Impl): public CBase
{
public:
	static CIniSecIter16Impl* NewL(const TDesC16& aSectionName,const CIniDocument16* aIniDocument);
	TBool Next(TPtrC16& aKeyName,TPtrC16& aKeyValue);
	void Reset();
	TBool End(){return iTempImpl->End();}
	~CIniSecIter16Impl(){delete iTempImpl;}	
private:
	CIniSecIter16Impl(){}
	void ConstructL(const TDesC16& aSectionName,const CIniDocument16* aIniDocument);
private:	
	CIniSecIterTmpl16* iTempImpl;	
};

//8 bit
NONSHARABLE_CLASS(CIniDocument8Impl): public CBase
{
public:
	static CIniDocument8Impl* NewL(RFs& aFs,const TDesC& aFileName);
	~CIniDocument8Impl();
	void FlushL(const TDesC& aFileName);
	
	TInt GetSectionList(RArray<TPtrC8>& aSectionList) const;
	void GetKeyValueL(const TDesC8& aSectionName,const TDesC8& aKeyName,TPtrC8& aKeyValue) const;
	CIniSection8* SectionL(const TDesC8& aSectionName) const;
	
	void AddSectionL(const TDesC8& aSectionName);
	void RemoveSectionL(const TDesC8& aSectionName);
	void SetKeyL(const TDesC8& aSectionName,const TDesC8& aKeyName,const TDesC8& aKeyValue);
	void RemoveKeyL(const TDesC8& aSectionName,const TDesC8& aKeyName);
	TBool CompareDocs(CIniDocument8Impl& aDoc);
private:
	CIniDocument8Impl(){}
	void ConstructL(RFs& aFs,const TDesC& aFileName);
private:
	CIniDocumentTmpl8* iTempImpl;
	
};

//16 bit
NONSHARABLE_CLASS(CIniDocument16Impl): public CBase
{
public:
	static CIniDocument16Impl* NewL(RFs& aFs,const TDesC& aFileName);
	~CIniDocument16Impl();
	void FlushL(const TDesC& aFileName);
	
	TInt GetSectionList(RArray<TPtrC16>& aSectionList) const;
	void GetKeyValueL(const TDesC16& aSectionName,const TDesC16& aKeyName,TPtrC16& aKeyValue) const;
	CIniSection16* SectionL(const TDesC16& aSectionName) const;

	void AddSectionL(const TDesC16& aSectionName);
	void RemoveSectionL(const TDesC16& aSectionName);
	void SetKeyL(const TDesC16& aSectionName,const TDesC16& aKeyName,const TDesC16& aKeyValue);
	void RemoveKeyL(const TDesC16& aSectionName,const TDesC16& aKeyName);
	TBool CompareDocs(CIniDocument16Impl& aDoc);
private:
	CIniDocument16Impl(){}
	void ConstructL(RFs& aFs,const TDesC& aFileName);	
private:
	CIniDocumentTmpl16* iTempImpl;
};

}//namespace BSUL

#endif//__INIPARSERIMPL_H__
