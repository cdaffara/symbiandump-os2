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

#ifndef __INITEMPLATE_H__
#define __INITEMPLATE_H__

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <s32file.h>
#include <e32des16.h>

namespace BSUL
{
//Line Type enumeration //
enum TLineType
	{	
	EComment, 
	ESection, 
	EKeyValue
	};

//utils template class//
static void GetBufferL(RFs& aFs,const TDesC& aFileName,HBufC8*& aBufferPtr)
	{
	//open the file for reading
	RFile file;
	User::LeaveIfError(file.Open(aFs,aFileName,EFileShareReadersOnly));
	
	//only process the file if it exists		
	TInt filesize;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Size(filesize)); 
	aBufferPtr=HBufC8::NewLC(filesize + 2); //In case '\r\n' needs to be appended
	TPtr8 asWritableBuffer(aBufferPtr->Des());
	User::LeaveIfError(file.Read(0,asWritableBuffer,filesize));
	
	//check if '\n' is present
	if( filesize > 0 && asWritableBuffer[filesize - 1] != '\n' )
		{
		asWritableBuffer.Append(_L8("\r\n"));
		}
	//pop the buffer
	CleanupStack::Pop();
	//close the file
	CleanupStack::PopAndDestroy();	
	}

template <class TLexX>
TBool IsEosAfterSpace(TLexX& aLex)
	{
	aLex.SkipSpace();
	return (aLex.Eos());
	}

template<class TPtrCX>
void RemoveComment(TPtrCX& aLine)
	{
	//Trim anything on the rhs of any inline comment ';' or '#'
	//in this case we need to locate which is the first occuring from left
	TInt semiColon=aLine.Locate(';');
	TInt hash=aLine.Locate('#');
	if (semiColon!=KErrNotFound || hash!=KErrNotFound)
		{
		if (semiColon!=KErrNotFound && hash!=KErrNotFound)
			{
			aLine.Set(aLine.Left(semiColon<hash?semiColon:hash));				
			}
		else
			{
			aLine.Set(aLine.Left(semiColon!=KErrNotFound?semiColon:hash));
			}	
		}	
	}

template <class TPtrCX,class TLexX>
void RemoveWSBeforeAndAfter(const TPtrCX& aString,TPtrCX& aOutput)
	{
	TLexX parser(aString);
	parser.SkipSpaceAndMark();
	TInt offset=parser.Offset();
	while(!parser.Eos())
		{
		//now skip for whitespace and check end of string					
		//if this is eos we are done unget to mark and break
		parser.SkipCharacters();
		parser.Mark();
		if (IsEosAfterSpace(parser))
			{
			parser.UnGetToMark();
			break;			
			}		
		}
	TInt endOffset=parser.MarkedOffset();
	aOutput.Set(aString.Mid(offset,endOffset-offset));		
	}

template<class TPtrCX,class TLexX>
TInt CountTokens(TPtrCX aLine)
	{
	TLexX myLexer(aLine);
	TInt tokenCount=0;
	while (!myLexer.Eos())
		{
		TPtrCX next = myLexer.NextToken();
		if (next.Size())
			{
			tokenCount++;
			}
		}
	return tokenCount;
	}

static void CorruptIfL(TBool aIsCorrupt)
	{
	if (aIsCorrupt)
		{
		User::Leave(KErrCorrupt);
		}
	}

template<class HBufCX, class TPtrCX>
TPtrCX ExtractLineFromBuffer(const TPtrCX& aFileBuffer, TInt aStartOffset)
	{
	TChar delim('\n');
	TPtrCX partialLine = aFileBuffer.Mid(aStartOffset, aFileBuffer.Length()- aStartOffset);
	TInt delimOffset= partialLine.Locate(delim);
	if (delimOffset==KErrNotFound)
		{
		return partialLine;
		}
	return partialLine.Left(delimOffset+1);
	}

//ini line template class //
template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
NONSHARABLE_CLASS(CIniLine) : public CBase 
{
public:
	static CIniLine* NewL(const TDesCX& aLine);
	static CIniLine* NewLC(const TDesCX& aLine);
	static CIniLine* NewLC(HBufCX* aLine, TLineType aTypeOfLine);
	static CIniLine* NewFromSectionNameLC(const TDesCX& aSectionName);
	static CIniLine* NewFromKeyValueLineLC(const TDesCX& aKeyName, const TDesCX& aKeyValue);
	
	~CIniLine();		
	TLineType LineType() const;
	void InsertBefore(CIniLine* aLine);
	const HBufCX& LineBuffer() const;
	TBool operator==(const CIniLine&) const;
public:
	TDblQueLink iLink;
private:
	TLineType DetermineTypeOfLine(const TDesCX& aLine);
	void ConstructL(const TDesCX& aLine);
	CIniLine(HBufCX* aLine, TLineType aLineType);
	CIniLine();
private:
	HBufCX*     iLine;
	TLineType   iLineType; 
};

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>* CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::NewL(const TDesCX& aLine)
	{
	CIniLine* self=CIniLine::NewLC(aLine);
	CleanupStack::Pop(self);
	return self;	
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>* CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::NewLC(const TDesCX& aLine)
	{
	CIniLine* self=new (ELeave) CIniLine();
	CleanupStack::PushL(self);
	self->ConstructL(aLine);
	return self;	
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>* CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::NewLC(HBufCX* aLine, TLineType aTypeOfLine)
	{
	CIniLine* self=new (ELeave) CIniLine(aLine, aTypeOfLine);
	CleanupStack::PushL(self);
	return self;	
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>	
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::CIniLine(HBufCX* aLine, TLineType aTypeOfLine):iLine(aLine), iLineType(aTypeOfLine)
	{}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>	
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::CIniLine()
	{}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>	
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::~CIniLine()
	{
	iLink.Deque();
	delete iLine;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
void CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::ConstructL(const TDesCX& aLine)
	{
	HBufCX* myHBuf = aLine.AllocL();
	this->iLine = myHBuf;
	this->iLineType = this->DetermineTypeOfLine(aLine);
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
TLineType CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::DetermineTypeOfLine(const TDesCX& aLine)
	{
	// First recognized character wins. Otherwise return a comment.
	for (TInt i=0; i< aLine.Length();i++)
		{
		TText myChar = aLine[i];
		switch (myChar)
			{
			case TText('#'):
			case ';':
				return EComment;
			
			case TText('['):
				return ESection;
				
			case TText('='):
				return EKeyValue;
				
			default:
				break;
			}
		}
	return EComment;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>* CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::NewFromSectionNameLC(const TDesCX& aSectionName)
	{
	//Fabricate a section name.
	HBufCX* myBuffer=HBufCX::NewLC(aSectionName.Size()+ 6);	//6= "2 cr's and \r" + "[]".
	TPtrX myBuf (myBuffer->Des());
	myBuf.Append(_L("\r\n["));
	myBuf.Append(aSectionName);
	myBuf.Append(_L("]\r\n"));
	CIniLine* myLine = CIniLine::NewLC(myBuffer, ESection);
	CleanupStack::Pop(2);
	CleanupStack::PushL(myLine);
	return myLine;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>* CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::NewFromKeyValueLineLC(const TDesCX& aKeyName, const TDesCX& aKeyValue)
	{
	//Fabricate a key = value line.
	HBufCX* myBuffer=HBufCX::NewLC(aKeyName.Size()+ aKeyValue.Size() + 3);	//3= "1 cr + \r" + "=".
	TPtrX myBuf (myBuffer->Des());
	myBuf.Set(myBuffer->Des());
	myBuf.Append(aKeyName);
	myBuf.Append('=');
	myBuf.Append(aKeyValue);
	myBuf.Append(_L("\r\n"));
	CIniLine* myLine = CIniLine::NewLC(myBuffer, EKeyValue);
	CleanupStack::Pop(2);
	CleanupStack::PushL(myLine);
	return myLine;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>
TLineType CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::LineType() const
	{
	return iLineType;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>	
void CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::InsertBefore(CIniLine* aLine)
	{
	aLine->iLink.AddBefore(&(this->iLink));
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>	
const HBufCX& CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::LineBuffer() const
	{
	return *iLine;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class TPtrX>	
TBool CIniLine<TDesCX,HBufCX,TPtrCX,TPtrX>::operator==(const CIniLine& aLine)const
	{
	if (LineType() != aLine.LineType())
		return EFalse;

	if ((LineBuffer()).Compare(aLine.LineBuffer()) != 0)
		return EFalse;
	return ETrue;
	}

//ini key template class//
template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX, class TPtrX,class TLexX>
NONSHARABLE_CLASS(CIniKey): public CBase
{
public:
	static CIniKey* NewL(const TDesCX& aKeyName,const TDesCX& aKeyValue);
	static CIniKey* NewLC(const TDesCX& aKeyName,const TDesCX& aKeyValue);
	static CIniKey* NewLC(CIniLineX* aLine);
	static TInt CompareKey(const CIniKey& aFirstKey,const CIniKey& aSecondKey);
	
	void SetKeyValue(const TDesCX& aKeyValue);	
	TPtrCX KeyName() const;
	TPtrCX KeyValue() const;
	CIniLineX* LineSrc() const;
	~CIniKey();
private:
	CIniKey(const TDesCX& aKeyName, const TDesCX& aKeyValue, CIniLineX* aLine);
	CIniKey(const TDesCX& aKeyName, const TDesCX& aKeyValue);
	static CIniKey* ParseKeyLineL(CIniLineX* aLine);
private:
	TPtrCX iKeyName;
	TPtrCX iKeyValue;
	CIniLineX* iLineSrc;
};

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>* CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::NewL(const TDesCX& aKeyName,const TDesCX& aKeyValue)
	{
	CIniKey* self=new (ELeave) CIniKey(aKeyName, aKeyValue);
	return self;		
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>* CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::NewLC(const TDesCX& aKeyName,const TDesCX& aKeyValue)
	{
	CIniKey* self=CIniKey::NewL(aKeyName,aKeyValue);
	CleanupStack::PushL(self);
	return self;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>* CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::NewLC(CIniLineX* aLine)
	{
	CIniKey* self=ParseKeyLineL(aLine);
	CleanupStack::PushL(self);
	self->iLineSrc=aLine;
	return self;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>	
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::CIniKey(const TDesCX& aKeyName, const TDesCX& aKeyValue, CIniLineX* aLine):iKeyName(aKeyName),iKeyValue(aKeyValue),iLineSrc(aLine)
	{	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>	
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::CIniKey(const TDesCX& aKeyName, const TDesCX& aKeyValue):iKeyName(aKeyName),iKeyValue(aKeyValue),iLineSrc(NULL)
	{	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::~CIniKey()
	{}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
TPtrCX CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::KeyName() const
	{ 	
	return iKeyName;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
TPtrCX CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::KeyValue() const
	{ 	
	return iKeyValue;
	}
	
template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>
CIniLineX* CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::LineSrc() const
	{ 	
	return iLineSrc;
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>	
void CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::SetKeyValue(const TDesCX& aKeyValue)
	{	
	iKeyValue.Set(aKeyValue);
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>	
TInt CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::CompareKey(const CIniKey& aFirstKey,const CIniKey& aSecondKey)
	{ 	
	return (aFirstKey.KeyName()).Compare(aSecondKey.KeyName());	
	}

template<class TDesCX,class HBufCX,class TPtrCX, class CIniLineX,class TPtrX,class TLexX>	
CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>* CIniKey<TDesCX,HBufCX,TPtrCX,CIniLineX,TPtrX,TLexX>::ParseKeyLineL(CIniLineX* aIniLine)
	{
	const HBufCX& myLine = aIniLine->LineBuffer();
	//Get the first occurence of '=' sign
	TInt equalOffset=myLine.Locate('=');
	CorruptIfL(equalOffset==KErrNotFound);

	TInt tokenCount = CountTokens<TPtrCX, TLexX>(myLine.Left(equalOffset));
	CorruptIfL(tokenCount != 1);
	
	//construct a TLex for checking the lhs of equal sign
	TLexX parser;
	parser.Assign(myLine.Left(equalOffset));
	parser.SkipSpaceAndMark();
	parser.SkipCharacters();
	TPtrCX keyname=parser.MarkedToken();
	TBool check = (keyname.Length()==0);
	CorruptIfL(check);	
			
	//strip off any white space before and after the value
	TInt edgeOfValue=equalOffset;
	while(++edgeOfValue < myLine.Length())
		{
		TChar t = (myLine)[edgeOfValue];
		if (!t.IsSpace())
			{
			break;
			}
		}
	TPtrCX keyvalue;
	if (edgeOfValue < myLine.Length())
		{
		keyvalue.Set(myLine.Mid(edgeOfValue, (myLine.Length()-edgeOfValue-1)));
		}
	
	TInt hashOffset=keyvalue.Locate('#');
	if (hashOffset>0)
		{
		keyvalue.Set(keyvalue.Left(hashOffset));
		}
	TInt commentOffset=keyvalue.Locate(';');
	if (commentOffset > 0)
		{
		keyvalue.Set(keyvalue.Left(commentOffset));
		}
	
	TInt index = keyvalue.Length()-1;
	if (index > 0)
		{
		while (index > 0)
			{
			TChar t = keyvalue[index];
			if (t.IsSpace())
				{
				index--;
				}
			else
				{
				break;
				}
			}
		if (index < (keyvalue.Length()-1))
			{
			keyvalue.Set(keyvalue.Left(index+1));
			}
		}
		
	CIniKey* myKey = new (ELeave) CIniKey(keyname,keyvalue, aIniLine);
	return myKey;
	}

//section template class//
template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX,class CIniLineX,class TPtrX, class TLexX>
NONSHARABLE_CLASS(CIniSection): public CBase 
{
public:
	static CIniSection* NewL(const TDesCX& aSectionName);
	static CIniSection* NewLC(const TDesCX& aSectionName);
	static CIniSection* NewL(const TDesCX& aSectionName, CIniLineX* aLineSrc);
	static CIniSection* NewLC(CIniLineX* aLineSrc);
	static TInt CompareSection(const CIniSection& aFirstSection,const CIniSection& aSecondSection);
	~CIniSection();

	void InsertKeyL(const TDesCX& aKeyName,const TDesCX& aKeyValue);
	void InsertKeyL(const CIniKeyX* aKey);
	void RemoveKeyL(const TDesCX& aKeyName);

	TPtrCX KeyValueL(const TDesCX& aKeyName) const;	
	TInt KeyCount() const;
	CIniKeyX* FindKeyL(const TDesCX& aKeyName) const;	
	TPtrCX SectionName() const;
	
	CIniLineX* SrcLine();
	void SetSrcLine(CIniLineX* aLineSrc);
	
	//to be used to access individual item
	const CIniKeyX* Key(TInt aIndex) const {return iKeyArray[aIndex];}
	void ReserveSpaceInKeyArrayL(){iKeyArray.ReserveL(1);}
private:
	CIniSection(TPtrCX aSectionName);
	CIniSection(TPtrCX aSectionName, CIniLineX* aLine);
	static TPtrCX ParseSectionLineL(const HBufCX& aLine);
	
private:
	TPtrCX iSectionName;
	RPointerArray<CIniKeyX> iKeyArray;
	CIniLineX* iLineSrc;		//If no document object exists, then this object is not used.
};

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>* CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::NewL(const TDesCX& aSectionName)
	{
	CIniSection* self=new (ELeave) CIniSection(aSectionName);
	return self;
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>* CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::NewL(const TDesCX& aSectionName,CIniLineX* aLineSrc)
	{
	CIniSection* self=new (ELeave) CIniSection(aSectionName, aLineSrc);
	return self;
	}
	
template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>* CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::NewLC(const TDesCX& aSectionName)
	{
	CIniSection* self=new (ELeave) CIniSection(aSectionName);
	CleanupStack::PushL(self);
	return self;
	}	

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>* CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::NewLC(CIniLineX* aLineSrc)
	{
	const HBufCX& buffer = aLineSrc->LineBuffer();
	TPtrCX sectionName = ParseSectionLineL(buffer);
	CIniSection* self=new (ELeave) CIniSection(sectionName, aLineSrc);
	CleanupStack::PushL(self);
	return self;
	}	

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::CIniSection(TPtrCX aSectionName, CIniLineX* aLine):iSectionName(aSectionName), iLineSrc(aLine)
	{ 
	iKeyArray.Reset();
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::CIniSection(TPtrCX aSectionName):iSectionName(aSectionName)
	{ 
	iKeyArray.Reset();
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::~CIniSection()
	{ 
	iKeyArray.ResetAndDestroy();
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX, class TLexX>	
void CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::SetSrcLine(CIniLineX* aLineSrc)
	{ 	
	if (iLineSrc)
		{
		delete iLineSrc;
		}
	iLineSrc = aLineSrc;
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
TInt CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::KeyCount() const
	{ 	
	return iKeyArray.Count(); 	
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>	
TPtrCX CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::SectionName() const
	{ 	
	return iSectionName;	
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>	
CIniLineX* CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::SrcLine()
	{ 	
	return iLineSrc; 	
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>	
TInt CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::CompareSection(const CIniSection& aFirstSection,const CIniSection& aSecondSection)
	{ 	
	return aFirstSection.SectionName().Compare(aSecondSection.SectionName());	 	
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX, class TLexX>	
void CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::InsertKeyL(const TDesCX& aKeyName,const TDesCX& aKeyValue)
	{
	CIniKeyX* newKey=CIniKeyX::NewLC(aKeyName,aKeyValue);
	InsertKeyL(newKey);
	CleanupStack::Pop(newKey);
	}	

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX, class TLexX>	
void CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::InsertKeyL(const CIniKeyX* aKey)
	{
	iKeyArray.InsertInOrderL(aKey,TLinearOrder<CIniKeyX>(CIniKeyX::CompareKey));
	}	

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
void CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::RemoveKeyL(const TDesCX& aKeyName)
	{
	CIniKeyX* key=FindKeyL(aKeyName);
	CIniLineX* myLine = key->LineSrc();
	TInt index=iKeyArray.FindInOrderL(key,TLinearOrder<CIniKeyX>(CIniKeyX::CompareKey));
	iKeyArray.Remove(index);
	delete key;
	delete myLine;
	}	

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX, class TLexX>
CIniKeyX* CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::FindKeyL(const TDesCX& aKeyName) const
	{
	CIniKeyX* key=CIniKeyX::NewLC(aKeyName, aKeyName);
	TInt index=iKeyArray.FindInOrderL(key,TLinearOrder<CIniKeyX>(CIniKeyX::CompareKey));
	CleanupStack::PopAndDestroy(key);
	return iKeyArray[index];
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
TPtrCX CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::KeyValueL(const TDesCX& aKeyName) const
	{
	CIniKeyX* key=FindKeyL(aKeyName);
	return key->KeyValue();
	}

template<class TDesCX,class HBufCX,class TPtrCX,class CIniKeyX, class CIniLineX, class TPtrX,class TLexX>
TPtrCX CIniSection<TDesCX,HBufCX,TPtrCX,CIniKeyX,CIniLineX,TPtrX,TLexX>::ParseSectionLineL(const HBufCX& aLine)	
	{
	//find the last terminating bracket ']'
	//anything in between is considered the section name
	TInt endBracket=aLine.Locate(']');
	//Check if the terminating bracket exits. Leave with an error code if not found.
	CorruptIfL(endBracket == KErrNotFound);
	
	TInt startBracket=aLine.Locate('[');
	CorruptIfL(startBracket == KErrNotFound);
	
	TPtrCX sectionName=aLine.Mid(startBracket+1, (endBracket-startBracket)-1);
	
	//corrupt if empty section e.g []
	TBool check = (sectionName.Length()==0);
	CorruptIfL(check);
	
	TLexX lex(sectionName);
	
	//Check for any white space within section name	
	//check any white spaces directly after the '['
	lex.SkipSpace();
	check = (lex.Offset()>lex.MarkedOffset());
	CorruptIfL(check);			
	lex.SkipCharacters();
	
	//At this stage we can extract the section name
	sectionName.Set(lex.MarkedToken());
	return (sectionName);
	}

//iterator template class//
template<class TDesCX,class TPtrCX,class CIniSectionX,class CIniDocumentX,class CIniKeyX>
NONSHARABLE_CLASS(CIniSecIterImplX): public CBase
{
public:
	CIniSecIterImplX():iCurrentIndex(0),iSection(NULL){}
	TBool Next(TPtrCX& aKeyName,TPtrCX& aKeyValue);
	void Reset(){iCurrentIndex=0;}
	TBool End(){return iCurrentIndex>=iSection->KeyCount();}
public:	
	TInt iCurrentIndex;
	CIniSectionX* iSection;	
};

template<class TDesCX,class TPtrCX,class CIniSectionX,class CIniDocumentX,class CIniKeyX>
TBool CIniSecIterImplX<TDesCX,TPtrCX,CIniSectionX,CIniDocumentX,CIniKeyX>::Next(TPtrCX& aKeyName,TPtrCX& aKeyValue)
	{
	if (iSection)
		{
		if (iCurrentIndex<iSection->KeyCount())
			{
			const CIniKeyX* key=iSection->Key(iCurrentIndex);
			aKeyName.Set(key->KeyName());
			aKeyValue.Set(key->KeyValue());
			iCurrentIndex++;
			return ETrue;
			}
		}
	return EFalse;
	}


//inidocument template class//
template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
NONSHARABLE_CLASS(CIniDocumentTmplX) : public CBase 
{
public:
	CIniDocumentTmplX(RFs& aFs,TBool aNarrow);
	~CIniDocumentTmplX();
	void FlushL(const TDesC& aFileName);
	
	TInt GetSectionList(RArray<TPtrCX>& aSectionList) const;
	void GetKeyValueL(const TDesCX& aSectionName,const TDesCX& aKeyName,TPtrCX& aKeyValue) const;
	CIniSectionX* SectionL(const TDesCX& aSectionName) const;
	
	CIniSectionX* AddSectionL(const TDesCX& aSectionName, CIniLineX* aLine);
	CIniSectionX* AddSectionL(const TDesCX& aSectionName);
	void AddSectionL(const CIniSectionX* aSection);
	
	void RemoveSectionL(const TDesCX& aSectionName);
	void SetKeyL(const TDesCX& aSectionName,const TDesCX& aKeyName,const TDesCX& aKeyValue);
	void RemoveKeyL(const TDesCX& aSectionName,const TDesCX& aKeyName);	
	void RemoveLineL(CIniLineX* aLine);
	void InsertKeyValueIntoQueueL(CIniLineX* aKeyValue, CIniSectionX& aSection);
	void AppendIntoQueue(CIniLineX* aSection);
	TBool CompareDocs(CIniDocumentTmplX& aDoc);
	
private:
	CIniSectionX* FindSectionL(const TDesCX& aSectionName) const;
	void ConstructL(const TDesC& aFileName);
	
private:
	TBool iNarrow;		
	RFs& iFs;
	TDblQue<CIniLineX> iDocument;
	RPointerArray<CIniSectionX> iSectionArray;	
};

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX, class CIniLineX,class CIniKeyX>
CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::CIniDocumentTmplX(RFs& aFs,TBool aNarrow):iNarrow(aNarrow),iFs(aFs)
	{
	iSectionArray.Reset();
	iDocument.SetOffset(_FOFF(CIniLineX, iLink));
	iDocument.Reset();
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
TBool CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::CompareDocs(CIniDocumentTmplX& aDoc)
	{
	CIniLineX* lineFromA; // This object is a. The passed in one is b.
	CIniLineX* lineFromB;
	TDblQueIter<CIniLineX> itera(this->iDocument);
	TDblQueIter<CIniLineX> iterb(aDoc.iDocument);
	
	TBool NotDoneYet= ETrue;
	while (NotDoneYet)
		{
		lineFromA = itera++;
		lineFromB = iterb++;
		if ((lineFromA == NULL) || (lineFromB == NULL))
			break;
		
		if (!(*lineFromA == *lineFromB))
			{
			return EFalse;
			}
		}
	
	if ( (lineFromA) || (lineFromB))
		return EFalse;
	
	if ((this->iSectionArray.Count()) != (aDoc.iSectionArray.Count()))
		{
		return EFalse;
		}
	
	for (TInt i=0; i<this->iSectionArray.Count(); i++)
		{
		TPtrCX secNameA = this->iSectionArray[i]->SectionName();
		TPtrCX secNameB = aDoc.iSectionArray[i]->SectionName();		
		if ((secNameA.Compare(secNameB))!= 0)
			return EFalse;
		
		if ((this->iSectionArray[i]->KeyCount()) != (aDoc.iSectionArray[i]->KeyCount()))
			return EFalse;
		
		lineFromA = this->iSectionArray[i]->SrcLine();
		lineFromB = aDoc.iSectionArray[i]->SrcLine();
		
		if (!(*lineFromA == *lineFromB))
			{
			return EFalse;
			}
		
		for (TInt k=0; k<this->iSectionArray[i]->KeyCount(); k++)
			{
			const CIniKeyX* keyA = this->iSectionArray[i]->Key(k);
			const CIniKeyX* keyB = aDoc.iSectionArray[i]->Key(k);
			if (keyA->KeyName().Compare(keyB->KeyName()) != 0)
					return EFalse;
			if (keyA->KeyValue().Compare(keyB->KeyValue()) != 0)
				return EFalse;
			
			lineFromA = keyA->LineSrc();
			lineFromB = keyB->LineSrc();
			
			if (!(*lineFromA == *lineFromB))
				{
				return EFalse;
				}
			}
		}

	return ETrue;
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX, class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::AppendIntoQueue(CIniLineX* aLine)
	{
	if (iDocument.IsEmpty())
		{
		iDocument.AddFirst(*aLine);
		}
	else
		{
		iDocument.AddLast(*aLine);	//always at the end of the list.
		}
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX, class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::InsertKeyValueIntoQueueL(CIniLineX* aKeyValue,CIniSectionX& aSection)
	{
	// Find the end of the section (if exists).
	CIniLineX* lastEntry;
	CIniLineX* line;
	TDblQueIter<CIniLineX> iter(iDocument);
	
	lastEntry = aSection.SrcLine();	//by default insert into queue after this line entry.
	iter.Set(*lastEntry);	// search after this point for a keyvalue line.
	iter++;	//point to next line
	while ((line = iter++) != NULL)
		{
		lastEntry=line;
		TLineType typeOfLine = line->LineType();
		if (typeOfLine == ESection)
			{
			break;
			}
		}
	
	if ( line == NULL )
		{
		AppendIntoQueue(aKeyValue);
		}
	else
		{
		if (iDocument.IsLast(lastEntry))
			{
			AppendIntoQueue(aKeyValue);
			}
		else
			{
			lastEntry->InsertBefore(aKeyValue);
			}
		}
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX, class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::FlushL(const TDesC& aFileName)
	{
	//we need to get the path for the target file
	TParse originFile;
	User::LeaveIfError(originFile.Set(aFileName,NULL,NULL));

	//Need to ensure path exists
	TInt error=iFs.MkDirAll(originFile.DriveAndPath());
	if (error!=KErrAlreadyExists)
		{
		User::LeaveIfError(error);
		}
	
	//Constructing the temp file name with the same directory and path
	TFileName tempFile;
	RFile file;
	User::LeaveIfError(file.Temp(iFs,originFile.DriveAndPath(),tempFile,EFileWrite));
	CleanupClosePushL(file);
		
	TPtrC8 bufPtr; 	
	if (!iNarrow)
		{
		TChar myChar(0xFEFF);
		const unsigned char* myChar8=reinterpret_cast<const unsigned char*>(&myChar);
		bufPtr.Set(myChar8, 2);
		User::LeaveIfError(file.Write(bufPtr));
		}
	
	TDblQueIter<CIniLineX> iter(iDocument);
	CIniLineX* line;
	while ((line = iter++) != NULL)
		{
		const HBufCX& tempBuffer=line->LineBuffer();
		const TUint8* rawptr8=reinterpret_cast<const TUint8*>(tempBuffer.Ptr());	
		if (iNarrow)
			{
			bufPtr.Set(rawptr8,tempBuffer.Length());
			}
		else
			{
			bufPtr.Set(rawptr8,tempBuffer.Length()*2);
			}
		User::LeaveIfError(file.Write(bufPtr));
		}
	User::LeaveIfError(file.Flush());
	CleanupStack::PopAndDestroy();
	
	//Finally try replacing or creating a new file
	//depending on whether file exist or not
	TEntry entry;
	if (iFs.Entry(aFileName,entry)==KErrNone)
		{
		User::LeaveIfError(iFs.Replace(tempFile,aFileName));
		}
	else
		{
		User::LeaveIfError(iFs.Rename(tempFile,aFileName));		
		}
	}
	
template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX, class CIniLineX,class CIniKeyX>	
TInt CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::GetSectionList(RArray<TPtrCX>& aSectionList) const
	{
	//Reset the list first
	aSectionList.Reset();
	for (TInt i=0;i<iSectionArray.Count();i++)
		{
		TPtrCX nameptr=iSectionArray[i]->SectionName();
		TInt ret=aSectionList.Append(nameptr);
		//If error half way fo the copying, reset the list first
		if (ret!=KErrNone)
			{
			aSectionList.Reset();
			return ret;
			}
		}
	return KErrNone;	
	}
	
template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX, class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::GetKeyValueL(const TDesCX& aSectionName,const TDesCX& aKeyName,TPtrCX& aKeyValue) const
	{
	//Find if section exists
	CIniSectionX* section = SectionL(aSectionName);
	aKeyValue.Set(section->KeyValueL(aKeyName));
	}
	 
template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
CIniSectionX* CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::SectionL(const TDesCX& aSectionName) const
	{
	CIniSectionX* section = FindSectionL(aSectionName);
	return section;	
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
CIniSectionX* CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::AddSectionL(const TDesCX& aSectionName)
	{
	//Find if section exists
	CIniSectionX* section = NULL;
	TRAPD(ret, section = FindSectionL(aSectionName));
	if ((ret == KErrNone) && (section != NULL))
		User::Leave(KErrAlreadyExists);
	
	//Fabricate objects upfront and on the cleanup stack to ensure their deletion if OOM conditions occur.
	CIniLineX* myLine = CIniLineX::NewFromSectionNameLC(aSectionName);
	AppendIntoQueue(myLine);
	
	// reserve 1 slot for the new section name to provoke the out of memory condition 
	// before actually inserting the object to avoid having to go back and remove it if it fails.
	this->iSectionArray.ReserveL(1);	
	
	// If we have not left, we are sure adding a section should not leave due to OOM.
	CIniSectionX* newSection=AddSectionL(aSectionName, myLine);
	CleanupStack::Pop(myLine);
	return newSection;
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
CIniSectionX* CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::AddSectionL(const TDesCX& aSectionName, CIniLineX* aLine)
	{
	CIniSectionX* newSection=CIniSectionX::NewLC(aSectionName);
	AddSectionL(newSection);
	newSection->SetSrcLine(aLine);
	CleanupStack::Pop(newSection);
	return newSection;
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::AddSectionL(const CIniSectionX* aSection)
	{
	iSectionArray.InsertInOrderL(aSection,TLinearOrder<CIniSectionX>(CIniSectionX::CompareSection));
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::RemoveSectionL(const TDesCX& aSectionName)
	{
	CIniSectionX* section = FindSectionL(aSectionName);
	CIniLineX* line = section->SrcLine();
	ASSERT(line);	//This should never happen ie. the Key object must have a line associated with it.
	
	// remove the lines after to the start of the next section
	TDblQueIter<CIniLineX> iter(iDocument);
	iter.Set(*line);	// search after this point for a keyvalue line.
	TBool atStartofSection = ETrue;
	while ((line = iter++) != NULL)
		{
		TLineType typeOfLine = line->LineType();
		if ((typeOfLine == ESection))
			{
			if (!atStartofSection)
				{
				break;
				}
			else
				{
				delete line;
				atStartofSection = EFalse;
				}
			}
		else
			{
			delete line;		//includes comment lines which are only in the document object. Otherwise you could just ~section.
			}
		}
	
	TInt index=this->iSectionArray.FindInOrderL(section,TLinearOrder<CIniSectionX>(CIniSectionX::CompareSection));
	iSectionArray.Remove(index);		
	delete section;
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::SetKeyL(const TDesCX& aSectionName,const TDesCX& aKeyName,const TDesCX& aKeyValue)	
	{
	//Find if section exists
	CIniSectionX* section = NULL;
	TRAPD(ret, section = FindSectionL(aSectionName));
	if (ret == KErrNone)
		{
		TRAPD(ret2, RemoveKeyL(aSectionName, aKeyName));
		if ((ret2 != KErrNone) && (ret2 != KErrNotFound))
			{
			User::Leave(ret2);
			}
		
		CIniLineX* myLine2 = CIniLineX::NewFromKeyValueLineLC(aKeyName, aKeyValue);
		CIniKeyX* myKey = CIniKeyX::NewLC(myLine2);
		InsertKeyValueIntoQueueL(myLine2, *section);
		
		// reserve 1 slot in the key array to provoke the out of memory condition
		// before inserting the key to avoid having to go back and remove the key if OOM occurs.
		section->ReserveSpaceInKeyArrayL();
		section->InsertKeyL(myKey);
		
		CleanupStack::Pop(2);
		return;
		}
	if (ret!=KErrNotFound)
		{
		User::Leave(ret);
		}
	
	//Fabricate objects upfront and on the cleanup stack to ensure their deletion if OOM conditions occur.
	CIniLineX* myLine = CIniLineX::NewFromSectionNameLC(aSectionName);
	AppendIntoQueue(myLine);
	CIniLineX* myLine3 = CIniLineX::NewFromKeyValueLineLC(aKeyName, aKeyValue);
	AppendIntoQueue(myLine3);
	CIniKeyX* myKey = CIniKeyX::NewLC(myLine3);
	
	// reserve 1 slot for the new section name to provoke the out of memory condition 
	// before actually inserting the object to avoid having to go back and remove it if it fails.
	this->iSectionArray.ReserveL(1);	
	
	// If we have not left, we are sure adding a section should not leave due to OOM.
	CIniSectionX* newSection=AddSectionL(aSectionName, myLine);
	
	// reserve 1 slot in the key array of the newly created empty section to provoke the out of memory condition
	// before inserting the key to avoid having to go back and remove the key if OOM occurs.
	TRAPD(err, newSection->ReserveSpaceInKeyArrayL());
	if (err == KErrNoMemory)
		{
		// In this case, we have an empty newSection object to be removed.
		TInt index=this->iSectionArray.FindInOrder(newSection,TLinearOrder<CIniSectionX>(CIniSectionX::CompareSection));
		iSectionArray.Remove(index);
		delete newSection;
		CleanupStack::PopAndDestroy(3);
		User::Leave(KErrNoMemory);
		}
	else
		{
		if (err!= KErrNone)
			User::Leave(err);
		}
	
	// we are now sure the next inserts will not fail due to OOM.
	newSection->InsertKeyL(myKey);
	CleanupStack::Pop(3);
	}
	
template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
void CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::RemoveKeyL(const TDesCX& aSectionName,const TDesCX& aKeyName)
	{
	//Find if section exists
	CIniSectionX* section = FindSectionL(aSectionName);
	section->RemoveKeyL(aKeyName);
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
CIniSectionX* CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::FindSectionL(const TDesCX& aSectionName) const
	{
	CIniSectionX* newSection=CIniSectionX::NewLC(aSectionName);
	TInt index=iSectionArray.FindInOrderL(newSection,TLinearOrder<CIniSectionX>(CIniSectionX::CompareSection));
	CleanupStack::PopAndDestroy(newSection);
	return iSectionArray[index];
	}

template<class TDesCX,class TPtrCX,class CIniSectionX,class TLexX,class TPtrX,class HBufCX,class CIniLineX,class CIniKeyX>
CIniDocumentTmplX<TDesCX,TPtrCX,CIniSectionX,TLexX,TPtrX,HBufCX,CIniLineX,CIniKeyX>::~CIniDocumentTmplX()
	{
	CIniLineX* line;
	TDblQueIter<CIniLineX> iter(iDocument);
	
	while ((line = iter++) != NULL)
		{
		delete line;
		}
	iSectionArray.ResetAndDestroy();
	}

//light weight template class//
template<class TDesCX,class TPtrCX,class HBufCX,class TLexX,class TPtrX,class TBufX,class CIniLineX>
NONSHARABLE_CLASS(CIniFileImplX): public CBase	
{
public:
	CIniFileImplX():iBuffer(NULL) {}
	TInt FindVar(const TDesCX& aSection,const TDesCX& aKey,TPtrCX& aValue);
	void ProcessBufferL(const TDesCX& aPtr);
	~CIniFileImplX()
		{ 
		delete iBuffer;
		}
private:	
	HBufCX* iBuffer;
};

template<class TDesCX,class TPtrCX,class HBufCX,class TLexX,class TPtrX,class TBufX,class CIniLineX>
void CIniFileImplX<TDesCX,TPtrCX,HBufCX,TLexX,TPtrX,TBufX,CIniLineX>::ProcessBufferL(const TDesCX& aPtr)
	{
	iBuffer = HBufCX::NewL(aPtr.Length() + sizeof('\n'));	//1 for cr.
	TPtrX bufferPtr(iBuffer->Des());
	TLexX lineParser(aPtr);
	while (!lineParser.Eos())
		{
		//Get line by line which is terminated by \n
		lineParser.SkipSpaceAndMark();
		while (!lineParser.Eos() && lineParser.Peek()!='\n')
			{
			lineParser.Inc();
			}
		TPtrCX line=lineParser.MarkedToken();
		//Now append the LHS of the comment to the buffer		
		RemoveComment(line);

		//trim any white space before section,key- value and comments
		TLexX removeWs(line);
		removeWs.SkipSpace();
		if (!removeWs.Eos())
			{
			bufferPtr.Append(line);
			//this added own delimiter is useful for syntax checking
			bufferPtr.Append('\n');	
			}
		}
	}

template<class TDesCX,class TPtrCX,class HBufCX,class TLexX,class TPtrX,class TBufX,class CIniLineX>
TInt CIniFileImplX<TDesCX,TPtrCX,HBufCX,TLexX,TPtrX,TBufX,CIniLineX>::FindVar(const TDesCX& aSection,const TDesCX& aKey,TPtrCX& aValue)
	{
	//first find the start of the section [aSectionName]
	//5 here is []\n and the wildcard *\n[sectionname]*
	HBufCX* section=HBufCX::New(aSection.Length()+5);
	if (!section)
		return KErrNoMemory;
	TPtrX sectionPtr(section->Des());
	sectionPtr.Append(_L("*\n["));
	sectionPtr.Append(aSection);
	sectionPtr.Append(_L("]*"));

	// Find a match purely on the section and not on the value, i.e. checking for a match on
	// [SECTION] does not find a match on key=[SECTION].

	TBool firstSection=ETrue;
	// Matching on first section is different as it should be the first
	// entry, i.e. no *\n before [aSection]* .
	TInt sectionStart=iBuffer->Match(sectionPtr.Mid(2));
	if (sectionStart==KErrNotFound)
		{
		firstSection=EFalse;
		// Then try to match on any section after the first, i.e. *\n[aSection]* .
		sectionStart=iBuffer->Match(sectionPtr);	
		}
	delete section;
	if (sectionStart!=KErrNotFound)
		{
		// There will be always \n added after section, key or value.
		TPtrCX searchBoundary(iBuffer->Mid(sectionStart+aSection.Length()+(firstSection?2:3)));
		searchBoundary.Set(searchBoundary.Mid(searchBoundary.Locate('\n')));
		//now need to locate the start of next section or end of file
		TBufX matchPattern;
		matchPattern.Append(_L("*\n[*]*"));
		TInt endSearchPos=searchBoundary.Match(matchPattern);
		if (endSearchPos!=KErrNotFound)	
			{
			//if can find next section block reduce the search boundary
			searchBoundary.Set(searchBoundary.Left(++endSearchPos));
			}
		//now time to find the first exact match key i.e "key" does not match "subkey"
		TInt equalOffset;
		//search through the boundary for matching key
		while ((equalOffset=searchBoundary.Locate('='))!=KErrNotFound)
			{
			//skip over the \n char
			searchBoundary.Set(searchBoundary.Mid(1));
			TLexX keyCheck(searchBoundary.Left(--equalOffset));
			keyCheck.SkipSpaceAndMark();
			keyCheck.SkipCharacters();
			TPtrCX key(keyCheck.MarkedToken());		
			keyCheck.SkipSpace();
			if (keyCheck.Eos() && key.Compare(aKey)==0)
				{
				TInt location = searchBoundary.Locate('\n');
				__ASSERT_DEBUG(location >= 0, User::Invariant());
				//trim any white space before and after the value
				TPtrCX rhs(searchBoundary.Mid(equalOffset+1,location-equalOffset-1));
				RemoveWSBeforeAndAfter<TPtrCX,TLexX>(rhs,aValue);					
				return KErrNone;
				}
			//update the next search boundary
			searchBoundary.Set(searchBoundary.Mid(searchBoundary.Locate('\n')));
			}
		}
	//cannot find here
	return KErrNotFound;	
	}

}//namespace BSUL
#endif//__BAINIPARSERIMPL_H__
