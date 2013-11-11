/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent 
*/
#ifndef __RANDOMIZEDACTIONS_H
#define __RANDOMIZEDACTIONS_H

#include <e32std.h>

class CRichText;
class CBandMaintainer;
class CBandValidator;
class CFbsScreenDevice;
class RFile;
class RReadStream;
class CTestPicture;

class CRandomizedAction : public CBase
	{
public:
	static CRandomizedAction* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput, RReadStream& aStream);
	static CRandomizedAction* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput);
	virtual TBool ExecuteL() = 0;
	virtual void ExternalizeL(RWriteStream& aStream) const = 0;
	inline TBool StopTest(){return iStop;}
protected:
	CRandomizedAction(CBandMaintainer& aTextViews, CRichText& aRichText, 
		CBandValidator& aValidator, RFile& aOutput);
	CBandMaintainer& iTextViews;
	CRichText& iRichText;
	CBandValidator& iValidator;
	RFile& iOutput;
	TBool iStop;
	};
	
class CRandomizedMove : public CRandomizedAction
	{
public:
	static CRandomizedMove* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	static CRandomizedMove* NewLC(	CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream);
	virtual TBool ExecuteL();
	virtual void ExternalizeL(RWriteStream& aStream) const;	
private:
	CRandomizedMove(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	void ConstructL();
	void ConstructL(RReadStream& aStream);
	TCursorPosition::TMovementType iMovement;
	};
	
class CRandomizedDeleteText : public CRandomizedAction
	{
public:
	static CRandomizedDeleteText* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	static CRandomizedDeleteText* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;	
	virtual TBool ExecuteL();
private:
	CRandomizedDeleteText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	void ConstructL();
	void ConstructL(RReadStream& aStream);
	TInt iPos;
	TInt iLength;
	};
	
class CRandomizedInsertText : public CRandomizedAction
	{
public:
	static CRandomizedInsertText* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput);
	static CRandomizedInsertText* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;	
	virtual TBool ExecuteL();
	~CRandomizedInsertText();
private:
	CRandomizedInsertText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	void ConstructL();
	void ConstructL(RReadStream& aStream);
	TInt iPos;
	RBuf iText;
	};

class CRandomizedReformatText : public CRandomizedAction
	{
public:
	static CRandomizedReformatText* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput);
	static CRandomizedReformatText* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, CFbsScreenDevice& aDevice, RFile& aOutput, RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;
	
	virtual TBool ExecuteL();
	
private:
	enum TReformatAction
		{
		EBoldify = 0,
		EChangeFont = 1,
		EChangeFontSize = 2
		};

	CRandomizedReformatText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, 
		CFbsScreenDevice& aDevice, RFile& aOutput);
	void ConstructL();
	void ConstructL(RReadStream& aStream);		
		
	void GetNewFontAttributes(TCharFormat& aCharFormat, TCharFormatMask& aCharFormatMask);
	void GetNewFont(TCharFormat& aCharFormat, TCharFormatMask& aCharFormatMask);
	void GetNewFontSize(TCharFormat& aCharFormat, TCharFormatMask& aCharFormatMask);

	CFbsScreenDevice& iDevice;
	TCharFormat iCharFormat;
	TCharFormatMask iCharFormatMask;
	TInt iPos;
	TInt iLength;
	TReformatAction iReformatAction;
	};
class CRandomizedInsertImage: public CRandomizedAction
	{
public:
	static CRandomizedInsertImage* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput);
	static CRandomizedInsertImage* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;	
	virtual TBool ExecuteL();
	~CRandomizedInsertImage();
private:
	CRandomizedInsertImage(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	void ConstructL();
	void ConstructL(RReadStream& aStream);
	TSize iPictureSize;
};

class CRandomizedSetVisibility: public CRandomizedAction
	{
public:
	static CRandomizedSetVisibility* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput);
	static CRandomizedSetVisibility* NewLC(CBandMaintainer& aTextViews, CRichText& aRichText, 
			CBandValidator& aValidator, RFile& aOutput, RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;	
	virtual TBool ExecuteL();
	~CRandomizedSetVisibility();
private:
	CRandomizedSetVisibility(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, RFile& aOutput);
	void ConstructL();
	void ConstructL(RReadStream& aStream);
	TBool iVisibility;
};
#endif
