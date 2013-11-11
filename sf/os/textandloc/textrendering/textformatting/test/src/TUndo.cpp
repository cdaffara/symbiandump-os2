/*
* Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* TUndo.cpp test file for UndoSystem classes
*
*/


#include <e32test.h>

#include "UndoSystem.h"
#include "UndoSystemImpl.h"
#include "EditorUndo.h"
#include "EditorPlainTextUndo.h"
#include <txtetext.h>
#include <conpics.h>
#include <s32mem.h>
#include <s32ucmp.h>
#include "TGraphicsContext.h"
#include "form_and_etext_editor.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtclipboard.h>
#include "txtfmlyr_internal.h"
#endif

#include "tundo.h"

#define UNUSED_VAR(a) a = a

using namespace UndoSystem;

namespace
{
CTUndoStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)
}

//
//
//  logger
//
//

class CLogger : public CBase
	{
public:
	virtual void Output(const TDesC& aText) = 0;
	void Output(TInt a)
		{
		TBuf<16> num(_L("<"));
		num.AppendNum(a);
		num.Append(_L(">"));
		Output(num);
		}
	};

namespace
{
CLogger& operator<<(CLogger& aLog, const TDesC& aText)
	{
	aLog.Output(aText);
	return aLog;
	}
CLogger& operator<<(CLogger& aLog, TInt aVal)
	{
	aLog.Output(aVal);
	return aLog;
	}
}

class CCheckingLogger : public CLogger
	{
	const TDesC*	iCheck;
	TInt			iPos;
	TBool			iFailed;
public:
	void SetCheckString(const TDesC& aCheck) { iCheck = &aCheck; iPos = 0; iFailed = EFalse; }
	TBool Failed() { return iFailed; }
	TBool Passed() { return !iFailed && iCheck && iCheck->Length() == iPos; }
	void Output(const TDesC& aText)
		{
		if (!iCheck || iFailed)
			return;
		TInt length = aText.Length();
		TInt maxLength = iCheck->Length() - iPos;
		TPtrC mid = iCheck->Mid(iPos, length < maxLength? length : maxLength);
		if (aText.Compare(mid) != 0)
			iFailed = ETrue;
		else
			iPos += aText.Length();
		}
	};

class CStoringLogger : public CLogger
	{
	HBufC*	iStore;
	TInt	iMaxLength;
public:
	~CStoringLogger() { delete iStore; }
	HBufC* GetStore() { HBufC* r = iStore; iStore = 0; iMaxLength = 0; return r; }
	void Output(const TDesC& aText)
		{
		if (!iStore)
			{
			iStore = HBufC::NewL(50);
			iMaxLength = 50;
			}
		while (iMaxLength < aText.Length() + iStore->Length())
			{
			iMaxLength *= 2;
			iStore = iStore->ReAllocL(iMaxLength);
			}
		iStore->Des().Append(aText);
		};
	};

//
//
//  commands
//
//

// internal commands
class CCommandOffset;
class CCommandToggle;
class CTestCommand : public CSingleCommand
	{
public:
	TUid FamilyUid() const
		{
		return TUid::Uid(12345);
		}
	virtual CCommandOffset* CastToCCommandOffset() { return 0; }
	virtual CCommandToggle* CastToCCommandToggle() { return 0; }
	};

// Offset
class CCommandOffset : public CTestCommand
	{
	TInt	iOffset;
	TInt*	iTarget;
	CLogger* iLogger;
	CCommandOffset() {}
public:
	static CCommandOffset* NewL(TInt aOffset, TInt* aTarget, CLogger* aLogger = 0)
		{
		CCommandOffset* r = new(ELeave) CCommandOffset;
		r->iOffset = aOffset;
		r->iTarget = aTarget;
		r->iLogger = aLogger;
		return r;
		}
	void SetLogger(CLogger* aLogger) { iLogger = aLogger; }
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("offset") << iOffset;
		*iTarget += iOffset;
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		return NewL(-iOffset, iTarget, iLogger);
		}
	void Add(TInt aOffset) { iOffset += aOffset; }
	CCommandOffset* CastToCCommandOffset() { return this; }
	};

// Negate (only if iTog is ETrue!)
class CCommandToggle : public CTestCommand
	{
	TBool iTog;
	TInt* iTarget;
	CLogger* iLogger;
	CCommandToggle() {}
public:
	static CCommandToggle* NewL(TBool aTog, TInt* aTarget, CLogger* aLogger = 0)
		{
		CCommandToggle* r = new(ELeave) CCommandToggle;
		r->iTog = aTog;
		r->iTarget = aTarget;
		r->iLogger = aLogger;
		return r;
		}
	void SetLogger(CLogger* aLogger) { iLogger = aLogger; }
	TInt ExecuteL() const
		{
		if (iLogger)
			{
			(*iLogger) << _L("negate") << (iTog? 1:0);
			}
		if (iTog)
			*iTarget = -*iTarget;
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		return NewL(iTog, iTarget, iLogger);
		}
	void Add(TBool aTog)
		{
		if (aTog)
			iTog = !iTog;
		}
	CCommandToggle* CastToCCommandToggle() { return this; }
	};

// command prototypes
class CCommandIncProto : public CTestCommand
	{
	TInt*		iTarget;
	CLogger*	iLogger;
	CCommandIncProto() {}
public:
	static CCommandIncProto* NewL(TInt* aTarget, CLogger* aLogger = 0)
		{
		CCommandIncProto* r = new(ELeave) CCommandIncProto;
		r->iTarget = aTarget;
		r->iLogger = aLogger;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("inc<>");
		++*iTarget;
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		return CCommandOffset::NewL(-1, iTarget, iLogger);
		}
	TBool PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
		{
		if (aLastCommand.FamilyUid() != TUid::Uid(12345))
			return EFalse;
		return !!static_cast<CTestCommand&>(aLastCommand).CastToCCommandOffset();
		}
	void AddInverseToLast(CSingleCommand& aLastCommand) const
		{
		CCommandOffset* c =
			static_cast<CTestCommand&>(aLastCommand).CastToCCommandOffset();
		ASSERT(c);
		c->Add(-1);
		}
	};

class CCommandDecProto : public CTestCommand
	{
	TInt*		iTarget;
	CLogger*	iLogger;
	CCommandDecProto() {}
public:
	static CCommandDecProto* NewL(TInt* aTarget, CLogger* aLogger = 0)
		{
		CCommandDecProto* r = new(ELeave) CCommandDecProto;
		r->iTarget = aTarget;
		r->iLogger = aLogger;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("dec<>");
		--*iTarget;
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		return CCommandOffset::NewL(1, iTarget, iLogger);
		}
	TBool PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
		{
		if (aLastCommand.FamilyUid() != TUid::Uid(12345))
			return EFalse;
		return !!static_cast<CTestCommand&>(aLastCommand).CastToCCommandOffset();
		}
	void AddInverseToLast(CSingleCommand& aLastCommand) const
		{
		CCommandOffset* c =
			static_cast<CTestCommand&>(aLastCommand).CastToCCommandOffset();
		ASSERT(c);
		c->Add(1);
		}
	};

class CCommandNegProto : public CTestCommand
	{
	TInt*		iTarget;
	CLogger*	iLogger;
	CCommandNegProto() {}
public:
	static CCommandNegProto* NewL(TInt* aTarget, CLogger* aLogger = 0)
		{
		CCommandNegProto* r = new(ELeave) CCommandNegProto;
		r->iTarget = aTarget;
		r->iLogger = aLogger;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("neg<>");
		*iTarget = -*iTarget;
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		return CCommandToggle::NewL(ETrue, iTarget, iLogger);
		}
	TBool PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const
		{
		if (aLastCommand.FamilyUid() != TUid::Uid(12345))
			return EFalse;
		return !!static_cast<CTestCommand&>(aLastCommand).CastToCCommandToggle();
		}
	void AddInverseToLast(CSingleCommand& aLastCommand) const
		{
		CCommandToggle* c =
			static_cast<CTestCommand&>(aLastCommand).CastToCCommandToggle();
		ASSERT(c);
		c->Add(ETrue);
		}
	};

// command whose inverse is a batch command
class CCommandDecThenNegProto : public CTestCommand
	{
	TInt*		iTarget;
	CLogger*	iLogger;
	CCommandDecThenNegProto() {}
public:
	static CCommandDecThenNegProto* NewL(TInt* aTarget, CLogger* aLogger = 0)
		{
		CCommandDecThenNegProto* r = new(ELeave) CCommandDecThenNegProto;
		r->iTarget = aTarget;
		r->iLogger = aLogger;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("decneg<>");
		*iTarget = -(*iTarget - 1);
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		CBatchCommand* batch = CBatchCommand::NewLC();
		batch->PushL(CCommandOffset::NewL(1, iTarget, iLogger));
		batch->PushL(CCommandToggle::NewL(ETrue, iTarget, iLogger));
		CleanupStack::Pop(batch);
		return batch;
		}
	};

class CCommandCannotDo : public CTestCommand
	{
	CLogger*	iLogger;
public:
	static CCommandCannotDo* NewL(CLogger* aLogger)
		{
		CCommandCannotDo* r = new(ELeave) CCommandCannotDo;
		r->iLogger = aLogger;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("nodo<>");
		return KErrNotSupported;
		}
	CCommand* CreateInverseL() const
		{
		return 0;
		}
	};

class CCommandCannotInvert : public CTestCommand
	{
	CLogger*	iLogger;
public:
	static CCommandCannotInvert* NewL(CLogger* aLogger)
		{
		CCommandCannotInvert* r = new(ELeave) CCommandCannotInvert;
		r->iLogger = aLogger;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("noinv<>");
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		if (iLogger)
			(*iLogger) << _L("noinvfail.");
		User::Leave(KErrNotSupported);
		return 0;
		}
	};

class CCommandLeavesInvert : public CTestCommand
	{
	CLogger*	iLogger;
public:
	mutable TBool iFail;
	static CCommandLeavesInvert* NewL(CLogger* aLogger)
		{
		CCommandLeavesInvert* r = new(ELeave) CCommandLeavesInvert;
		r->iLogger = aLogger;
		r->iFail = ETrue;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iLogger)
			(*iLogger) << _L("leaveinv<>");
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		if (iFail)
			{
			iFail = EFalse;
			if (iLogger)
				(*iLogger) << _L("noinvfail.");
			User::Leave(KErrNotFound);
			}
		return 0;
		}
	};

class CCommandNoMemory : public CTestCommand
	{
	CLogger*	iLogger;
public:
	TBool iFailInvert;
	TBool iFailAddToLast;
	TBool iFailExecute;
	mutable TBool iLogExecuteFailed;

	static CCommandNoMemory* NewL(CLogger* aLogger)
		{
		CCommandNoMemory* r = new(ELeave) CCommandNoMemory;
		r->iLogger = aLogger;
		r->iFailInvert		= ETrue;
		r->iFailAddToLast	= ETrue;
		r->iFailExecute		= ETrue;
		r->iLogExecuteFailed= ETrue;
		return r;
		}
	TInt ExecuteL() const
		{
		if (iFailExecute)
			{
			if (iLogger && iLogExecuteFailed)
				(*iLogger) << _L("nomemfailexe.");
			iLogExecuteFailed = EFalse;
			User::Leave(KErrNoMemory);
			}
		if (iLogger)
			(*iLogger) << _L("nomem<>");
		return KErrNone;
		}
	CCommand* CreateInverseL() const
		{
		if (iFailInvert)
			{
			if (iLogger)
				(*iLogger) << _L("nomemfailinv.");
			User::Leave(KErrNoMemory);
			}
		return 0;
		}
	TBool PrepareToAddInverseToLastL(CSingleCommand&) const
		{
		if (iFailAddToLast)
			{
			if (iLogger)
				(*iLogger) << _L("nomemfailadd.");
			User::Leave(KErrNoMemory);
			}
		return EFalse;
		}
	};

// this gatekeeper refuses non-undoable requests
class CRefuserGatekeeper : public CBase, public MNotUndoableGatekeeper
	{
public:
	TBool RetryOutOfMemoryL(TInt)
		{
		return EFalse;
		}
	TBool AllowNotUndoableL(TInt)
		{
		return EFalse;
		}
	};

// this gatekeeper permits all non-undoable requests
// (not just KErrNotSupported and KErrNoMemory)
class CPermitterGatekeeper : public CBase, public MNotUndoableGatekeeper
	{
public:
	TBool RetryOutOfMemoryL(TInt)
		{
		return EFalse;
		}
	TBool AllowNotUndoableL(TInt)
		{
		return ETrue;
		}
	};

// this gatekeeper makes the CCommandNoMemory fail less the more times it is called
class CMemoryReclaimGatekeeper : public CRefuserGatekeeper
	{
public:
	CCommandNoMemory* iTarget;
	CMemoryReclaimGatekeeper(CCommandNoMemory* aTarget = 0) : iTarget(aTarget) {}
	TBool RetryOutOfMemoryL(TInt aNumRetries)
		{
		if (aNumRetries == 0)
			{
			iTarget->iFailAddToLast = EFalse;
			return ETrue;
			}
		if (aNumRetries == 1)
			{
			iTarget->iFailInvert = EFalse;
			return ETrue;
			}
		if (aNumRetries == 2)
			{
			iTarget->iFailExecute = EFalse;
			return ETrue;
			}
		return EFalse;
		}
	};


//
//
//	Editor
//
//

// a cut-down set of attributes for testing purposes
class CUndoTestPicture : public CPicture
	{
	TBuf<1> iDesc;
public:
	CUndoTestPicture(TInt aChar) : iDesc(1) { iDesc[0] = static_cast<TText>(aChar); }
	void Draw(CGraphicsContext&, const TPoint&, const TRect&, MGraphicsDeviceMap*) const {}
	void ExternalizeL(RWriteStream&) const {}
	void GetOriginalSizeInTwips(TSize&) const {}
	TPtrC Description() const { return iDesc; }
	};

struct TTestAttributes
	{
	TInt8 iCharFlags;
	TInt8 iParFlags;
	TInt8 iStyle;
	TTestAttributes() : iCharFlags(0), iParFlags(0), iStyle(-1) {}
	TBool operator==(TTestAttributes& a)
		{
		return iCharFlags == a.iCharFlags && iParFlags == a.iParFlags && iStyle == a.iStyle;
		}
	};
CLogger& operator<<(CLogger& log, TTestAttributes& at)
	{
	TBuf<3> buf(_L("Aa0"));
	buf[0] = (TText)(buf[0] + (at.iCharFlags & 15));
	buf[1] = (TText)(buf[1] + (at.iParFlags & 15));
	buf[2] = (TText)(buf[2] + at.iStyle);
	return log << buf;
	}
// Test editor, badly behaved if something leaves.
// The only formats supported are bold, italic, keep together and keep with next.
class CTestEditor : public CBase, public MUnifiedEditor,
	public MUnifiedEditor::MStyleSupport,
	public MUnifiedEditor::MPictureSupport,
	public MUnifiedEditor::MClipboardSupport
	{
	TTestAttributes iBase;
	TTestAttributes iStyles[10];
	TBuf<KMaxParagraphStyleName> iStyleNames[10];
	TInt iNumStyles;
	CArrayFix<TTestAttributes>* iAttrs;
	CBufSeg* iText;
	CArrayFix<CUndoTestPicture*>* iPics;

	CTestEditor* ConstructL()
		{
		iText = CBufSeg::NewL(5 * sizeof(TText));
		iAttrs = new(ELeave) CArrayFixFlat<TTestAttributes>(5);
		iPics = new(ELeave) CArrayFixFlat<CUndoTestPicture*>(5);
		return this;
		}
	TInt Style(const TDesC& aName) const
		{
		for (int i = 0; i != iNumStyles; ++i)
			if (aName == iStyleNames[i])
				return i;
		return -1;
		}
	TInt InsertStylePos(const TDesC& aName) const
		{
		int i;
		for (i = 0; i != iNumStyles; ++i)
			if (aName.Compare(iStyleNames[i]) <= 0)
				return i;
		return i;
		}
	void ReassignStyles(TInt aFrom, TInt aTo)
		{
		ASSERT(-1 <= aTo && aTo < iNumStyles);
		ASSERT(0 <= aFrom && aFrom < iNumStyles);
		if (aTo == aFrom)
			return;
		TInt setTo	= aTo;
		if (aTo == -1)
			aTo = iNumStyles - 1;
		TInt low	= aFrom;
		TInt high	= aTo;
		TInt delta	= -1;
		if (aTo < aFrom)
			{
			low		= aTo;
			high	= aFrom;
			delta	= 1;
			}
		TInt len = DocumentLength();

		int i;
		for (i = 0; i != len; ++i)
			{
			TTestAttributes* attr = &iAttrs->At(i);
			if (aFrom == attr->iStyle)
				attr->iStyle = static_cast<TInt8>(setTo);
			else if (low <= attr->iStyle && attr->iStyle <= high)
				attr->iStyle = static_cast<TInt8>(attr->iStyle + delta);
			}
		for (i = aFrom; i != aTo; i -= delta)
			{
			iStyles[i] = iStyles[i - delta];
			iStyleNames[i] = iStyleNames[i - delta];
			}
		}
	void DoDeleteStyle(TInt aForDeletion)
		{
		ASSERT(aForDeletion < iNumStyles);
		ReassignStyles(aForDeletion, -1);
		--iNumStyles;
		}
	void DoAddStyle(const TDesC& aNewName, const TTestAttributes& aAttr)
		{
		ASSERT(iNumStyles < 10);
		TInt pos = InsertStylePos(aNewName);
		++iNumStyles;
		ReassignStyles(iNumStyles - 1, pos);
		iStyles[pos] = aAttr;
		iStyleNames[pos] = aNewName;
		}
	static void BoldToAttr(TTestAttributes& aAttr, const TTmCharFormat& aCFormat)
		{
		if (aCFormat.iFontSpec.IsBold())
			aAttr.iCharFlags |= 1;
		else
			aAttr.iCharFlags &= ~1;
		}
	static void ItalicToAttr(TTestAttributes& aAttr, const TTmCharFormat& aCFormat)
		{
		if (aCFormat.iFontSpec.IsItalic())
			aAttr.iCharFlags |= 2;
		else
			aAttr.iCharFlags &= ~2;
		}
	static void CharFormatToAttr(TTestAttributes& aAttr, const TTmCharFormat& aCFormat)
		{
		aAttr.iCharFlags = 0;
		BoldToAttr(aAttr, aCFormat);
		ItalicToAttr(aAttr, aCFormat);
		}
	static void CharLayerToAttr(TTestAttributes& aAttr, const TTmCharFormatLayer& aCLayer)
		{
		if (aCLayer.iMask.iFlags & TTmCharFormatMask::EBold)
			{
			aAttr.iCharFlags |= 4;
			BoldToAttr(aAttr, aCLayer.iFormat);
			}
		if (aCLayer.iMask.iFlags & TTmCharFormatMask::EItalic)
			{
			aAttr.iCharFlags |= 8;
			ItalicToAttr(aAttr, aCLayer.iFormat);
			}
		}
	static void KeepTogetherToAttr(TTestAttributes& aAttr, const RTmParFormat& aPFormat)
		{
		if (aPFormat.iFlags & RTmParFormat::EKeepTogether)
			aAttr.iParFlags |= 1;
		else
			aAttr.iParFlags &= ~1;
		}
	static void KeepWithNextToAttr(TTestAttributes& aAttr, const RTmParFormat& aPFormat)
		{
		if (aPFormat.iFlags & RTmParFormat::EKeepWithNext)
			aAttr.iParFlags |= 2;
		else
			aAttr.iParFlags &= ~2;
		}
	static void ParFormatToAttr(TTestAttributes& aAttr, const RTmParFormat& aPFormat)
		{
		aAttr.iParFlags = 0;
		KeepTogetherToAttr(aAttr, aPFormat);
		KeepWithNextToAttr(aAttr, aPFormat);
		}
	static void ParLayerToAttr(TTestAttributes& aAttr, const RTmParFormatLayer& aPLayer)
		{
		if (aPLayer.iMask.iFlags & TTmParFormatMask::EKeepTogether)
			{
			aAttr.iParFlags |= 4;
			KeepTogetherToAttr(aAttr, aPLayer.iFormat);
			}
		if (aPLayer.iMask.iFlags & TTmParFormatMask::EKeepWithNext)
			{
			aAttr.iParFlags |= 8;
			KeepWithNextToAttr(aAttr, aPLayer.iFormat);
			}
		}
	static void BoldAttrToCharFormat(TTmCharFormat& aCFormat, const TTestAttributes& aAttr)
		{
		if (aAttr.iCharFlags & 1)
			aCFormat.iFontSpec.SetBold(ETrue);
		}
	static void ItalicAttrToCharFormat(TTmCharFormat& aCFormat, const TTestAttributes& aAttr)
		{
		if (aAttr.iCharFlags & 2)
			aCFormat.iFontSpec.SetItalic(ETrue);
		}
	static void ResetCharFormat(TTmCharFormat& aCFormat)
		{
		TTmCharFormat c;
		aCFormat = c;
		}
	static void AttrToCharFormat(TTmCharFormat& aCFormat, const TTestAttributes& aAttr)
		{
		ResetCharFormat(aCFormat);
		BoldAttrToCharFormat(aCFormat, aAttr);
		ItalicAttrToCharFormat(aCFormat, aAttr);
		}
	static void MergeAttrToCharLayer(TTmCharFormatLayer& aCLayer, const TTestAttributes& aAttr)
		{
		if (aAttr.iCharFlags & 4)
			{
			aCLayer.iMask.iFlags |= TTmCharFormatMask::EBold;
			BoldAttrToCharFormat(aCLayer.iFormat, aAttr);
			}
		if (aAttr.iCharFlags & 8)
			{
			aCLayer.iMask.iFlags |= TTmCharFormatMask::EItalic;
			ItalicAttrToCharFormat(aCLayer.iFormat, aAttr);
			}
		}
	static void AttrToCharLayer(TTmCharFormatLayer& aCLayer, const TTestAttributes& aAttr)
		{
		ResetCharFormat(aCLayer.iFormat);
		aCLayer.iMask.iFlags = 0;
		MergeAttrToCharLayer(aCLayer, aAttr);
		}
	static void ResetParFormat(RTmParFormat& aPFormat)
		{
		RTmParFormat p;
		CleanupClosePushL(p);
		aPFormat.CopyL(p);
		CleanupStack::PopAndDestroy();
		}
	static void KeepTogetherAttrToParFormat(RTmParFormat& aPFormat, const TTestAttributes& aAttr)
		{
		if (aAttr.iParFlags & 1)
			aPFormat.iFlags |= RTmParFormat::EKeepTogether;
		}
	static void KeepWithNextAttrToParFormat(RTmParFormat& aPFormat, const TTestAttributes& aAttr)
		{
		if (aAttr.iParFlags & 2)
			aPFormat.iFlags |= RTmParFormat::EKeepWithNext;
		}
	static void AttrToParFormat(RTmParFormat& aPFormat, const TTestAttributes& aAttr)
		{
		ResetParFormat(aPFormat);
		KeepTogetherAttrToParFormat(aPFormat, aAttr);
		KeepWithNextAttrToParFormat(aPFormat, aAttr);
		}
	static void MergeAttrToParLayer(RTmParFormatLayer& aPLayer, const TTestAttributes& aAttr)
		{
		if (aAttr.iParFlags & 4)
			{
			aPLayer.iMask.iFlags |= TTmParFormatMask::EKeepTogether;
			KeepTogetherAttrToParFormat(aPLayer.iFormat, aAttr);
			}
		if (aAttr.iParFlags & 8)
			{
			aPLayer.iMask.iFlags |= TTmParFormatMask::EKeepWithNext;
			KeepWithNextAttrToParFormat(aPLayer.iFormat, aAttr);
			}
		}
	static void AttrToParLayer(RTmParFormatLayer& aPLayer, const TTestAttributes& aAttr)
		{
		ResetParFormat(aPLayer.iFormat);
		aPLayer.iMask.iFlags = 0;
		MergeAttrToParLayer(aPLayer, aAttr);
		}
public:
	~CTestEditor()
		{
		delete iAttrs;
		// workaround for CBufSeg bug
		if (0 < iText->Size())
			iText->Delete(iText->Size() - 1, 1);
		delete iText;
		delete iPics;
		}
	static CTestEditor* NewL()	{ return (new(ELeave) CTestEditor)->ConstructL(); }
	void Reset()
		{
		iAttrs->Reset();
		iText->Reset();
		iPics->Reset();
		iNumStyles = 0;
		}
	void AlterGranularityL(TInt aNewGranularity)
		{
		CBufSeg* newIText = CBufSeg::NewL(aNewGranularity * sizeof(TText));
		CleanupStack::PushL(newIText);
		TBuf8<32> transfer;
		TInt pos = 0;
		while (pos < iText->Size())
			{
			TInt length = transfer.MaxLength();
			if (iText->Size() - pos < length)
				length = iText->Size() - pos;
			iText->Read(pos, transfer, length);
			newIText->InsertL(pos, transfer, length);
			pos += transfer.Length();
			}
		CleanupStack::Pop(newIText);
		// workaround for CBufSeg bug
		if (0 < iText->Size())
			iText->Delete(iText->Size() - 1, 1);
		delete iText;
		iText = newIText;
		}
	void Print(CLogger& log)
		{
		TInt length = DocumentLength();
		int i;
		log << _L("text{");
		for (i = 0; i < length;)
			{
			TPtrC seg;
			GetText(i, seg);
			TInt picPos = seg.Locate(CEditableText::EPictureCharacter);
			if (0 < picPos)
				{
				// shorten seg to just before the picture character
				TPtrC temp(seg.Ptr(), picPos);
				seg.Set(temp);
				}
			if (0 == picPos)
				{
				CUndoTestPicture* pic = iPics->At(i);
				if (pic)
					log << _L("{pic:") << pic->Description() << _L("}");
				else
					log << _L("{nopic}");
				++i;
				}
			else
				{
				log << seg;
				i += seg.Length();
				}
			}
		log << _L("} styles{");
		for(i = 0; i != iNumStyles; ++i)
			{
			if (i)
				log << _L(", ");
			log << iStyleNames[i] << _L(":") << iStyles[i];
			}
		log << _L("} attr{");
		for (i = 0; i != length; ++i)
			log << iAttrs->At(i);
		log << _L("} ");
		}
	MTmOptionalInterface* Interface(TUint aId)
		{
		if (aId == KUidMUnifiedEditorStyleSupport)
			return static_cast<MUnifiedEditor::MStyleSupport*>(this);
		if (aId == KUidMUnifiedEditorPictureSupport)
			return static_cast<MUnifiedEditor::MPictureSupport*>(this);
		if (aId == KUidMUnifiedEditorClipboardSupport)
			return static_cast<MUnifiedEditor::MClipboardSupport*>(this);
		return 0;
		}

	void InsertTextL(TInt aPos, const TDesC& aText,
					 const TDesC* aStyle,
					 const TTmCharFormatLayer* aCharFormat,
					 const RTmParFormatLayer* aParFormat)
		{
		TTestAttributes attr;
		attr.iStyle = aStyle? (TInt8)Style(*aStyle) : (TInt8)-1;
		if (aCharFormat)
			CharLayerToAttr(attr, *aCharFormat);
		if (aParFormat)
			ParLayerToAttr(attr, *aParFormat);
		iText->InsertL(aPos * sizeof(TText), aText.Ptr(), aText.Length() * sizeof(TText));
		iAttrs->InsertL(aPos, attr, aText.Length());
		CUndoTestPicture* nullPic = 0;
		iPics->InsertL(aPos, nullPic, aText.Length());
		}
	void DeleteTextL(TInt aPos, TInt aLength)
		{
		iText->Delete(aPos * sizeof(TText), aLength * sizeof(TText));
		iAttrs->Delete(aPos, aLength);
		for (int i = aPos; i != aPos + aLength; ++i)
			delete iPics->At(i);
		iPics->Delete(aPos, aLength);
		}
	void SetBaseFormatL(const TTmCharFormat& aCharFormat,const RTmParFormat& aParFormat)
		{
		CharFormatToAttr(iBase, aCharFormat);
		ParFormatToAttr(iBase, aParFormat);
		}
	void SetCharFormatL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat)
		{
		TInt end = aPos + aLength;
		if (DocumentLength() < end)
			end = DocumentLength();
		for (; aPos < end; ++aPos)
			CharLayerToAttr(iAttrs->At(aPos), aFormat);
		}
	void SetParFormatL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat)
		{
		TInt end = aPos + aLength;
		if (DocumentLength() < end)
			end = DocumentLength();
		for (; aPos < end; ++aPos)
			ParLayerToAttr(iAttrs->At(aPos), aFormat);
		}
	void DeleteCharFormatL(TInt aPos,TInt aLength)
		{
		TInt end = aPos + aLength;
		if (DocumentLength() < end)
			end = DocumentLength();
		for (; aPos < end; ++aPos)
			iAttrs->At(aPos).iCharFlags = 0;
		}
	void DeleteParFormatL(TInt aPos,TInt aLength)
		{
		TInt end = aPos + aLength;
		if (DocumentLength() < end)
			end = DocumentLength();
		for (; aPos < end; ++aPos)
			iAttrs->At(aPos).iParFlags = 0;
		}
	TInt CreateStyleL(const RTmStyle& aStyle)
		{
		TInt styleNo = Style(aStyle.iName);
		if (0 <= styleNo)
			return KErrAlreadyExists;
		TTestAttributes newAttr;
		CharLayerToAttr(newAttr, aStyle.iCharFormat);
		ParLayerToAttr(newAttr, aStyle.iParFormat);
		DoAddStyle(aStyle.iName, newAttr);
		return KErrNone;
		}
	TInt ChangeStyleL(const RTmStyle& aStyle)
		{
		TInt styleNo = Style(aStyle.iName);
		if (styleNo < 0)
			return KErrNotFound;
		iStyles[styleNo] = TTestAttributes();
		CharLayerToAttr(iStyles[styleNo], aStyle.iCharFormat);
		ParLayerToAttr(iStyles[styleNo], aStyle.iParFormat);
		return KErrNone;
		}
	TInt SetStyleL(TInt aPos, TInt aLength, const TDesC& aName)
		{
		TInt styleNo(-1);
		if (aName.Length())
			{
			styleNo = Style(aName);
			if (styleNo < 0)
				return KErrNotFound;
			}
		TInt end = aPos + aLength;
		for (; aPos < end; ++aPos)
			iAttrs->At(aPos).iStyle = (TInt8)styleNo;
		return KErrNone;
		}
	TInt RenameStyleL(const TDesC& aOldName, const TDesC& aNewName)
		{
		TInt oldNo = Style(aOldName);
		if (oldNo < 0)
			return KErrNotFound;
		TTestAttributes temp = iStyles[oldNo];
		TInt newNo = InsertStylePos(aNewName);
		if (oldNo < newNo)
			--newNo;
		ReassignStyles(oldNo, newNo);
		iStyles[newNo] = temp;
		iStyleNames[newNo] = aNewName;
		return KErrNone;
		}
	TInt DeleteStyleL(const TDesC& aName)
		{
		TInt n = Style(aName);
		if (n < 0)
			return KErrNotFound;
		DoDeleteStyle(n);
		return KErrNone;
		}
	void InsertPictureL(TInt aPos, const TPictureHeader& aPictureIn)
		{
		TBuf<1> picChar(1);
		picChar[0] = CEditableText::EPictureCharacter;
		InsertTextL(aPos, picChar, 0, 0, 0);
		iPics->At(aPos) = static_cast<CUndoTestPicture*>(aPictureIn.iPicture.AsPtr());
		}
	void DropPictureL(TInt aPos)
		{
		TPtrC ptr;
		GetText(aPos, ptr);
		if (ptr[0] == CEditableText::EPictureCharacter)
			{
			iPics->At(aPos) = 0;
			DeleteTextL(aPos, 1);
			}
		}
	void Picture(TInt aPos, TPictureHeader& aPictureOut) const
		{
		CPicture* pic = iPics->At(aPos);
		aPictureOut.iPictureType = KUidXzePictureType;
		aPictureOut.iPicture = pic;
		}
	TInt DocumentLength() const
		{
		return iText->Size() / sizeof(TText);
		}
	void GetText(TInt aPos, TPtrC& aText) const
		{
		iText->Compress();
		if (DocumentLength() <= aPos)
			aPos = DocumentLength();
		TPtr8 ptr = iText->Ptr(aPos * sizeof(TText));
		aText.Set((TText*)ptr.Ptr(), ptr.Length()/sizeof(TText));
		}
	void GetBaseFormatL(TTmCharFormat& aCharFormat, RTmParFormat& aParFormat) const
		{
		AttrToCharFormat(aCharFormat, iBase);
		AttrToParFormat(aParFormat, iBase);
		}
	void GetCharFormat(TInt aPos, TFormatLevel aLevel,
					   TTmCharFormatLayer& aFormat, TInt& aRunLength) const
		{
		TInt length = DocumentLength();
		if (length <= aPos)
			{
			aRunLength = 0;
			return;
			}
		TTestAttributes attr = iAttrs->At(aPos);
		if (aLevel == ESpecific)
			{
			AttrToCharLayer(aFormat, attr);
			}
		else
			{
			AttrToCharLayer(aFormat, iBase);
			MergeAttrToCharLayer(aFormat, attr);
			}
		TInt pos = aPos + 1;
		while (pos < length && attr == iAttrs->At(pos))
			++pos;
		aRunLength = pos - aPos;
		}
	void GetParFormatL(TInt aPos, TFormatLevel aLevel,
					   RTmParFormatLayer& aFormat, TInt& aRunLength) const
		{
		TInt length = DocumentLength();
		if (length <= aPos)
			{
			aRunLength = 0;
			return;
			}
		TTestAttributes attr = iAttrs->At(aPos);
		if (aLevel == ESpecific)
			{
			AttrToParLayer(aFormat, attr);
			}
		else
			{
			AttrToParLayer(aFormat, iBase);
			MergeAttrToParLayer(aFormat, attr);
			}
		TInt pos = aPos + 1;
		while (pos < length && attr == iAttrs->At(pos))
			++pos;
		aRunLength = pos - aPos;
		}
	TInt StyleCount() const { return iNumStyles; }
	void GetStyle(TInt aPos, TPtrC& aName, TInt& aRunLength) const
		{
		TInt length = DocumentLength();
		if (aPos < 0 || length <= aPos)
			{
			aName.Set(iStyleNames[0].Ptr(), 0);
			aRunLength = 0;
			return;
			}
		TInt styleNo = iAttrs->At(aPos).iStyle;
		if (styleNo < 0)
			aName.Set(iStyleNames[0].Ptr(), 0);
		else
			aName.Set(iStyleNames[styleNo]);
		TInt pos = aPos + 1;
		while (pos < length && iAttrs->At(pos).iStyle == styleNo)
			++pos;
		aRunLength = pos - aPos;
		return;
		}
	TInt GetStyleByNameL(const TDesC& aName, RTmStyle& aStyle) const
		{
		return GetStyleByIndexL(Style(aName), aStyle);
		}
	TInt GetStyleByIndexL(TInt aIndex, RTmStyle& aStyle) const
		{
		if (aIndex < 0 || iNumStyles <= aIndex)
			return KErrNotFound;
		aStyle.iName = iStyleNames[aIndex];
		AttrToParLayer(aStyle.iParFormat, iStyles[aIndex]);
		AttrToCharLayer(aStyle.iCharFormat, iStyles[aIndex]);
		return KErrNone;
		}
	void CopyToStoreL(CStreamStore& aStore, CStreamDictionary& aDictionary,
		TInt aPos, TInt aLength) const
		{
		ASSERT(aPos + aLength <= DocumentLength());
		if (aLength <= 0)
			return;
		RStoreWriteStream stream;
		TStreamId id = stream.CreateLC(aStore);
		stream.WriteInt32L(aLength);
		RBufReadStream input_stream(*iText, aPos * sizeof(TText));
		TMemoryStreamUnicodeSource source(input_stream);
		TUnicodeCompressor c;
		c.CompressL(stream, source, KMaxTInt, aLength);
		input_stream.Close();
		stream.CommitL();
		aDictionary.AssignL(KClipboardUidTypePlainText, id);
		CleanupStack::PopAndDestroy();		// close stream

		// now write out formatting in our own bizarre format
		//...
		// in actual fact this probably wouldn't test that much, so I won't
		// bother right now.
		}
	void PasteFromStoreL(const CStreamStore& aStore,
		const CStreamDictionary& aDictionary, TInt aPos)
		{
		ASSERT(aPos <= DocumentLength());
		TStreamId id = aDictionary.At(KClipboardUidTypePlainText);
		RStoreReadStream stream;
		stream.OpenLC(aStore, id);
		TInt length = stream.ReadInt32L();
		RBufWriteStream bufferStream;
		bufferStream.Insert(*iText, aPos * sizeof(TText));
		TMemoryStreamUnicodeSink sink(bufferStream);
		TUnicodeExpander e;
		e.ExpandL(sink, stream, length);
		bufferStream.CommitL();
		bufferStream.Close();
		CleanupStack::PopAndDestroy();	// close stream

		// and if we get round to adding some formatting to the copy method,
		// then we should deal with it here also
		//...
		// but not today. Just add the appropriate spaces into all the structures.
		TTestAttributes attr;
		iAttrs->InsertL(aPos, attr, length);
		CUndoTestPicture* nullPic = 0;
		iPics->InsertL(aPos, nullPic, length);
		}
	};

CLogger& operator<<(CLogger& log, CTestEditor& ed) { ed.Print(log); return log; }

// 1 - CCommandStack test
TInt ExecuteStackL(CCommandStack& a)
	{
	while (a.Top())
		{
		CSingleCommand* single = a.Top()->Single();
		if (!single)
			{
            TESTPRINT(_L("CCommandStack : stack unexpectedly contained batches"));
			a.Reset();
			return 1;
			}
		single->ExecuteL();
		delete single;
		a.Pop();
		}
	return 0;
	}
TInt CheckLog(CCheckingLogger& a)
	{
	if (a.Passed())
		return 0;
	TESTPRINT(_L("CCommandStack... : log failed"));
	return 1;
	}
TInt CheckTop(CCommandStack& aStack, CCommand* aTop)
	{
	if (aStack.Top() != aTop)
		{
        TESTPRINT(_L("CCommandStack : unexpected item at top of stack"));
		return 1;
		}
	return 0;
	}
TInt CheckCount(CCommandStack& aStack, TInt aExpectedCount)
	{
	if (aStack.Count() != aExpectedCount)
		{
	    TESTPRINT(_L("CCommandStack : stack an unexpected size"));
		return 1;
		}
	return 0;
	}
TInt CheckPop(CCommandStack& aStack)
	{
	CCommand* check = aStack.Top();
	if (aStack.Pop() != check)
		{
        TESTPRINT(_L("CCommandStack : Pop() does not match Top()"));
		return 1;
		}
	return 0;
	}
void AddStuffL(CCommandStack& aStack, TInt* aTarget, CLogger* aLog)
	{
	TInt startCount = aStack.Count();
	CheckTop(aStack, 0);
	aStack.PrepareToPushL(1);
	CheckCount(aStack, startCount);
	CheckTop(aStack, 0);
	CheckCount(aStack, startCount);
	CCommand* temp = CCommandIncProto::NewL(aTarget, aLog);
	aStack.Push(temp);
	CheckCount(aStack, startCount + 1);
	CheckTop(aStack, temp);
	aStack.PrepareToPushL(2);
	CheckCount(aStack, startCount + 1);
	CheckTop(aStack, temp);
	CheckCount(aStack, startCount + 1);
	CheckTop(aStack, temp);
	aStack.PrepareToPushL(1);
	aStack.PrepareToPushL(3);
	CheckCount(aStack, startCount + 1);
	CheckTop(aStack, temp);
	temp = CCommandDecProto::NewL(aTarget, aLog);
	CheckCount(aStack, startCount + 1);
	aStack.Push(temp);
	CheckCount(aStack, startCount + 2);
	CheckTop(aStack, temp);
	CheckTop(aStack, temp);
	CheckCount(aStack, startCount + 2);
	CheckTop(aStack, temp);
	temp = CCommandIncProto::NewL(aTarget, aLog);
	aStack.Push(temp);
	CheckTop(aStack, temp);
	CheckCount(aStack, startCount + 3);
	aStack.PrepareToPushL(1);
	CheckTop(aStack, temp);
	aStack.PrepareToPushL(2);
	CheckTop(aStack, temp);
	temp = CCommandNegProto::NewL(aTarget, aLog);
	CheckCount(aStack, startCount + 3);
	aStack.Push(temp);
	CheckCount(aStack, startCount + 4);
	CheckTop(aStack, temp);
	CheckCount(aStack, startCount + 4);
	CheckTop(aStack, temp);
	temp = CCommandIncProto::NewL(aTarget, aLog);
	CheckCount(aStack, startCount + 4);
	aStack.Push(temp);
	CheckTop(aStack, temp);
	CheckCount(aStack, startCount + 5);
	}
void TestCCommandStackL()
	{
	__UHEAP_MARK;
	TInt target;
	CCheckingLogger* log = new(ELeave) CCheckingLogger;

	CCommandStack* stack = CCommandStack::NewL();

	AddStuffL(*stack, &target, log);

	log->SetCheckString(_L("inc<>neg<>inc<>dec<>inc<>"));
	ExecuteStackL(*stack);
	CheckLog(*log);

	CheckCount(*stack, 0);
	CCommand* temp = CCommandIncProto::NewL(&target, log);
	CheckTop(*stack, 0);
	stack->PrepareToPushL(1);
	CheckCount(*stack, 0);
	CheckTop(*stack, 0);
	CheckCount(*stack, 0);
	stack->Push(temp);
	CheckCount(*stack, 1);
	stack->PrepareToPushL(1);
	CheckCount(*stack, 1);
	CCommand* next = CCommandDecProto::NewL(&target, log);
	stack->Push(next);
	CheckCount(*stack, 2);
	stack->PrepareToPushL(1);
	CheckPop(*stack);
	stack->PrepareToPushL(1);
	CheckCount(*stack, 1);
	CheckTop(*stack, temp);
	CheckCount(*stack, 1);
	stack->Push(next);
	stack->PrepareToPushL(1);
	CheckCount(*stack, 2);
	CheckCount(*stack, 2);
	CheckPop(*stack);
	CheckCount(*stack, 1);
	CheckTop(*stack, temp);
	delete next;

	stack->Reset();
	CheckCount(*stack, 0);

	AddStuffL(*stack, &target, log);
	stack->PruneTo(3);
	CheckCount(*stack, 3);
	log->SetCheckString(_L("inc<>neg<>inc<>"));
	ExecuteStackL(*stack);
	CheckLog(*log);

	AddStuffL(*stack, &target, log);
	stack->PrepareToPushL(1);
	CheckCount(*stack, 5);
	stack->PruneTo(2);
	CheckCount(*stack, 2);
	log->SetCheckString(_L("inc<>neg<>"));
	ExecuteStackL(*stack);
	CheckLog(*log);

	delete stack;
	delete log;

	__UHEAP_MARKENDC(0);
	}

// 2 - CBatchCommand test
void ExecuteBatchL(CBatchCommand& a)
	{
	while (a.Top())
		{
		CSingleCommand* single = a.Top();
		single->ExecuteL();
		if (a.Pop() != single)
			{
            TESTPRINT(_L("CBatchCommand : Pop() didn't match Top()"));
            TESTPOINT(0);
			}
		delete single;
		}
	TESTPOINT(1);
	}
void CheckTop(CBatchCommand& aBatch, CCommand* aTop)
	{
	if (aBatch.Top() != aTop)
		{
        TESTPRINT(_L("CCommandBatch : unexpected item at top of stack"));
        TESTPOINT(0);
		}
	TESTPOINT(1);
	}
void TestCBatchCommandL()
	{
	__UHEAP_MARK;
	TInt target = 0;
	CCheckingLogger* log = new(ELeave) CCheckingLogger;

	CBatchCommand* batch = CBatchCommand::NewL();

	CBatchCommand* b1 = CBatchCommand::NewL();
	CBatchCommand* b2 = CBatchCommand::NewL();
	CBatchCommand* b3 = CBatchCommand::NewL();

	CCommand* s1 = CCommandIncProto::NewL(&target, log);
	CCommand* s2 = CCommandDecProto::NewL(&target, log);
	CCommand* s3 = CCommandNegProto::NewL(&target, log);
	CCommand* s4 = CCommandIncProto::NewL(&target, log);
	CCommand* s5 = CCommandDecProto::NewL(&target, log);
	CCommand* s6 = CCommandNegProto::NewL(&target, log);
	CCommand* s7 = CCommandIncProto::NewL(&target, log);
	CCommand* s8 = CCommandDecProto::NewL(&target, log);
	CCommand* s9 = CCommandNegProto::NewL(&target, log);

	b2->PrepareToPushL(s4);
	b2->Push(s4);
	b2->PrepareToPushL(s8);
	b2->Push(s8);
	b2->PrepareToPushL(s2);
	b2->PrepareToPushL(s2);
	b2->Push(s2);

	b3->PrepareToPushL(s3);
	b3->PrepareToPushL(s9);
	b3->Push(s9);
	b3->PrepareToPushL(s3);
	b3->Push(s3);
	b3->PrepareToPushL(s7);
	b3->Push(s7);

	b1->PrepareToPushL(s6);
	b1->Push(s6);
	b1->PrepareToPushL(s5);
	b1->Push(s5);
	b1->PrepareToPushL(b3);
	b1->Push(b3);
	b1->PrepareToPushL(b1);
	b1->PrepareToPushL(s1);
	b1->PrepareToPushL(s1);
	b1->PrepareToPushL(b2);

	batch->PrepareToPushL(b2);
	batch->Push(b2);
	batch->PrepareToPushL(s1);
	batch->PrepareToPushL(s1);
	batch->PrepareToPushL(b1);
	batch->Push(b1);
	batch->PrepareToPushL(s1);
	batch->Push(s1);

	CheckTop(*batch, s1);
	batch->Pop();
	CheckTop(*batch, s7);
	batch->PrepareToPushL(s1);
	CheckTop(*batch, s7);
	batch->Push(s1);
	CheckTop(*batch, s1);
	batch->PrepareToPushL(s1);
	CheckTop(*batch, s1);
	batch->Pop();
	CheckTop(*batch, s7);
	batch->Pop();
	CheckTop(*batch, s3);
	batch->Pop();
	CheckTop(*batch, s9);
	batch->Pop();
	CheckTop(*batch, s5);
	batch->Pop();
	CheckTop(*batch, s6);
	batch->PrepareToPushL(s5);
	CheckTop(*batch, s6);
	batch->Push(s5);
	CheckTop(*batch, s5);
	b3 = CBatchCommand::NewL();
	b3->PrepareToPushL(s9);
	CheckTop(*b3, 0);
	b3->Push(s9);
	CheckTop(*b3, s9);
	b3->PrepareToPushL(s3);
	CheckTop(*b3, s9);
	b3->Push(s3);
	CheckTop(*b3, s3);
	b3->PrepareToPushL(s7);
	CheckTop(*b3, s3);
	b3->Push(s7);
	CheckTop(*b3, s7);
	batch->PrepareToPushL(b3);
	CheckTop(*batch, s5);
	batch->Push(b3);
	CheckTop(*batch, s7);
	batch->PrepareToPushL(s1);
	batch->Push(s1);

	log->SetCheckString(_L("inc<>inc<>neg<>neg<>dec<>neg<>dec<>dec<>inc<>"));
	ExecuteBatchL(*batch);
	CheckLog(*log);

	delete log;
	delete batch;

	__UHEAP_MARKENDC(0);
	}

// 3 - CCommandHistory test
void ExecuteHistoryL(CCommandHistory& aHistory, CLogger& aLog)
	{
	while (aHistory.Top())
		{
		if (aHistory.Top()->Single())
			{
			aHistory.Top()->Single()->ExecuteL();
			}
		else
			{
			CBatchCommand* batch = aHistory.Top()->Batch();
			TESTPOINT(batch != 0);
			aLog << _L("batch{");
			ExecuteBatchL(*batch);
			aLog << _L("}");
			}
		delete aHistory.Pop();
		}
	}
void TestCCommandHistoryL()
	{
	__UHEAP_MARK;

	CCommandHistory* history = CCommandHistory::NewL();
	CCheckingLogger* log = new(ELeave) CCheckingLogger;
	TInt target;

	CCommand* p;
	history->SetMaxItems(5);
	p = CCommandDecProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	p = CCommandIncProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	p = CCommandDecProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	p = CCommandNegProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	history->BeginBatchLC();
	p = CCommandIncProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	p = CCommandDecProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	p = CCommandNegProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	CleanupStack::PopAndDestroy();
	p = CCommandDecProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);
	CBatchCommand* batch = CBatchCommand::NewL();
	p = CCommandDecProto::NewL(&target, log);
	batch->PrepareToPushL(p);
	batch->Push(p);
	p = CCommandNegProto::NewL(&target, log);
	batch->PrepareToPushL(p);
	batch->Push(p);
	p = CCommandIncProto::NewL(&target, log);
	batch->PrepareToPushL(p);
	batch->Push(p);
	history->PrepareToAddCommandL(batch);
	history->AddCommand(batch);
	p = CCommandNegProto::NewL(&target, log);
	history->PrepareToAddCommandL(p);
	history->AddCommand(p);

	log->SetCheckString(_L("neg<>batch{inc<>neg<>dec<>}dec<>batch{neg<>dec<>inc<>}neg<>"));
	ExecuteHistoryL(*history, *log);
	CheckLog(*log);

	delete log;
	delete history;

	__UHEAP_MARKENDC(0);
	}

// 4 - CCommandManager test
void TestCanUndo(const CCommandManager& aMan)
	{
	if (aMan.CanUndo())
		{
        TESTPOINT(1);
		return;
		}
	TESTPRINT(_L("CCommandManager : unexpectedly could not undo"));
	TESTPOINT(0);
	}
void TestCanRedo(const CCommandManager& aMan)
	{
	if (aMan.CanRedo())
		{
        TESTPOINT(1);
		return;
		}
	TESTPRINT(_L("CCommandManager : unexpectedly could not redo"));
	TESTPOINT(0);
	}
void TestCannotUndo(const CCommandManager& aMan)
	{
	if (!aMan.CanUndo())
		{
        TESTPOINT(1);
		return;
		}
	TESTPRINT(_L("CCommandManager : unexpectedly could undo"));
	TESTPOINT(0);
	}
void TestCannotRedo(const CCommandManager& aMan)
	{
	if (!aMan.CanRedo())
		{
        TESTPOINT(1);
		return;
		}
	TESTPRINT(_L("CCommandManager : unexpectedly could undo"));
	TESTPOINT(0);
	}
void SetUpTestL(CCommandManager& aMan, CSingleCommand& aCommand, TInt* aTarget, CLogger* aLogger)
	{
	CCommandIncProto* inc = CCommandIncProto::NewL(aTarget, aLogger);
	CleanupStack::PushL(inc);
	CCommandNegProto* neg = CCommandNegProto::NewL(aTarget, aLogger);
	CleanupStack::PushL(neg);
	CCommandDecProto* dec = CCommandDecProto::NewL(aTarget, aLogger);
	CleanupStack::PushL(dec);

	aMan.ExecuteL(*inc);
	aMan.BeginBatchLC();
	aMan.ExecuteL(*neg);
	aMan.ExecuteL(aCommand);
	aMan.ExecuteL(*dec);
	CleanupStack::PopAndDestroy();		// close batch
	aMan.ExecuteL(*neg);
	aMan.UndoL();

	CleanupStack::PopAndDestroy(dec);
	CleanupStack::PopAndDestroy(neg);
	CleanupStack::PopAndDestroy(inc);
	}
TInt CheckErrorCode(TInt aErr, TInt aExpected)
	{
	if (aErr == aExpected)
		return 0;
	if (aErr == KErrNone)
	    TESTPRINT(_L("CCommandManager : no leave where one was expected"));
	else
	    TESTPRINT(_L("CCommandManager : unexpected leave code"));
	return 1;
	}
void TestCCommandManagerL()
	{
	__UHEAP_MARK;

	CCommandManager* manager = CCommandManager::NewL();
	CCheckingLogger* log = new(ELeave) CCheckingLogger;
	TInt target = 0;
	CRefuserGatekeeper* refuser = new(ELeave) CRefuserGatekeeper;
	CPermitterGatekeeper* permitter = new(ELeave) CPermitterGatekeeper;
	CMemoryReclaimGatekeeper* reclaimer = new(ELeave) CMemoryReclaimGatekeeper;

	TestCannotUndo(*manager);
	TestCannotRedo(*manager);

	CCommandIncProto* inc = CCommandIncProto::NewL(&target, log);
	CCommandDecProto* dec = CCommandDecProto::NewL(&target, log);
	CCommandNegProto* neg = CCommandNegProto::NewL(&target, log);

	log->SetCheckString(_L("inc<>neg<>inc<>dec<>neg<>negate<1>"));
	SetUpTestL(*manager, *inc, &target, log);
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("offset<0>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("offset<1>"));
	manager->RedoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("negate<1>offset<0>"));
	manager->RedoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("negate<1>"));
	manager->RedoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCannotRedo(*manager);
	log->SetCheckString(_L("negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("inc<>"));
	manager->ExecuteL(*inc);
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCannotRedo(*manager);
	log->SetCheckString(_L("offset<-1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCanRedo(*manager);
	log->SetCheckString(_L("negate<1>offset<1>"));
	manager->RedoL();
	CheckLog(*log);
	TestCanUndo(*manager);
	TestCannotRedo(*manager);

	manager->ResetUndo();
	TestCannotUndo(*manager);
	TestCannotRedo(*manager);

	// test coalescence

	log->SetCheckString(_L("inc<>inc<>inc<>inc<>inc<>inc<>"));

	manager->ExecuteL(*inc);
	manager->ExecuteL(*inc);
	manager->BeginBatchLC();
	manager->ExecuteL(*inc);
	manager->ExecuteL(*inc);
	CleanupStack::PopAndDestroy();		// close batch
	manager->ExecuteL(*inc);
	manager->ExecuteL(*inc);
	CheckLog(*log);

	log->SetCheckString(_L("offset<-4>offset<-2>"));
	manager->UndoL();
	manager->UndoL();
	CheckLog(*log);

	log->SetCheckString(_L("offset<2>offset<4>"));
	manager->RedoL();
	manager->RedoL();
	TestCannotRedo(*manager);
	CheckLog(*log);

	manager->ResetUndo();
	TestCannotUndo(*manager);
	TestCannotRedo(*manager);

	// test command with batch inverse
	log->SetCheckString(_L("inc<>decneg<>inc<>"));
	CCommandDecThenNegProto* dnp = CCommandDecThenNegProto::NewL(&target, log);
	manager->ExecuteL(*inc);
	manager->ExecuteL(*dnp);
	manager->ExecuteL(*inc);
	CheckLog(*log);
	delete dnp;
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("negate<1>offset<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	manager->ResetUndo();

	// Test case when undo is not supported
	// 1. execution is permitted
	log->SetCheckString(_L("inc<>neg<>noinvfail.noinv<>dec<>neg<>negate<1>"));
	CCommandCannotInvert* noInv = CCommandCannotInvert::NewL(log);
	SetUpTestL(*manager, *noInv, &target, log);
	CheckLog(*log);
	TestCannotUndo(*manager);
	log->SetCheckString(_L("negate<1>"));
	manager->RedoL();
	CheckLog(*log);
	TestCannotRedo(*manager);
	manager->ResetUndo();

	//2. execution is supressed
	manager->SetGatekeeper(refuser);
	log->SetCheckString(_L("inc<>neg<>noinvfail.dec<>neg<>negate<1>"));
	SetUpTestL(*manager, *noInv, &target, log);
	CheckLog(*log);
	delete noInv;
	log->SetCheckString(_L("offset<1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();
	manager->SetGatekeeper(0);

	// Test case when execution fails (with returned error code)
	CCommandCannotDo* noDo = CCommandCannotDo::NewL(log);
	log->SetCheckString(_L("inc<>neg<>nodo<>dec<>neg<>negate<1>"));
	SetUpTestL(*manager, *noDo, &target, log);
	delete noDo;
	log->SetCheckString(_L("offset<1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();

	// Test case when inversion fails (not inversion is reported as impossible)
	// 1. when execution is permitted
	manager->SetGatekeeper(permitter);
	log->SetCheckString(_L("inc<>neg<>noinvfail.leaveinv<>dec<>neg<>negate<1>"));
	CCommandLeavesInvert* leaveInv = CCommandLeavesInvert::NewL(log);
	SetUpTestL(*manager, *leaveInv, &target, log);
	CheckLog(*log);
	TestCannotUndo(*manager);
	log->SetCheckString(_L("negate<1>"));
	manager->RedoL();
	CheckLog(*log);
	TestCannotRedo(*manager);
	manager->ResetUndo();

	// 2. when execution is supressed
	manager->SetGatekeeper(refuser);
	log->SetCheckString(_L("inc<>neg<>noinvfail.dec<>neg<>negate<1>"));
	leaveInv->iFail = ETrue;
	TRAPD(err, SetUpTestL(*manager, *leaveInv, &target, log));
	CheckErrorCode(err, KErrNone);
	CheckLog(*log);
	log->SetCheckString(_L("offset<1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();

	// 3. when execution is terminated by leaving
	manager->SetGatekeeper(0);
	log->SetCheckString(_L("inc<>neg<>noinvfail."));
	leaveInv->iFail = ETrue;
	TRAP(err, SetUpTestL(*manager, *leaveInv, &target, log));
	CheckErrorCode(err, KErrNotFound);
	CheckLog(*log);
	delete leaveInv;
	log->SetCheckString(_L("negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();

	// Test case when inversion runs out of memory
	// 1. when execution is permitted with no undo
	manager->SetGatekeeper(permitter);
	log->SetCheckString(_L("inc<>neg<>nomemfailadd.nomem<>dec<>neg<>negate<1>"));
	CCommandNoMemory* noMem = CCommandNoMemory::NewL(log);
	noMem->iFailExecute = EFalse;
	SetUpTestL(*manager, *noMem, &target, log);
	CheckLog(*log);
	TestCannotUndo(*manager);
	log->SetCheckString(_L("negate<1>"));
	manager->RedoL();
	CheckLog(*log);
	TestCannotRedo(*manager);
	manager->ResetUndo();

	// 2. when execution is supressed
	manager->SetGatekeeper(refuser);
	log->SetCheckString(_L("inc<>neg<>nomemfailadd.dec<>neg<>negate<1>"));
	TRAP(err, SetUpTestL(*manager, *noMem, &target, log));
	CheckErrorCode(err, KErrNone);
	CheckLog(*log);
	log->SetCheckString(_L("offset<1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();
	manager->SetGatekeeper(0);

	// 3. when memory is reclaimed
	reclaimer->iTarget = noMem;
	manager->SetGatekeeper(reclaimer);
	log->SetCheckString(_L("inc<>neg<>nomemfailadd.nomemfailinv.nomem<>dec<>neg<>negate<1>"));
	TRAP(err, SetUpTestL(*manager, *noMem, &target, log));
	CheckErrorCode(err, KErrNone);
	CheckLog(*log);
	log->SetCheckString(_L("offset<1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();
	manager->SetGatekeeper(0);

	// Test when execution runs out of memory
	// 1. with no reclaimation
	noMem->iFailAddToLast	= EFalse;
	noMem->iFailInvert		= EFalse;
	noMem->iFailExecute		= ETrue;
	noMem->iLogExecuteFailed= ETrue;
	log->SetCheckString(_L("inc<>neg<>nomemfailexe."));
	TRAP(err, SetUpTestL(*manager, *noMem, &target, log));
	CheckErrorCode(err, KErrNoMemory);
	CheckLog(*log);
	TestCannotRedo(*manager);
	log->SetCheckString(_L("negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();
	// 2. with reclaimation
	noMem->iFailAddToLast	= EFalse;
	noMem->iFailInvert		= EFalse;
	noMem->iFailExecute		= ETrue;
	noMem->iLogExecuteFailed= ETrue;
	reclaimer->iTarget = noMem;
	manager->SetGatekeeper(reclaimer);
	log->SetCheckString(_L("inc<>neg<>nomemfailexe.nomem<>dec<>neg<>negate<1>"));
	TRAP(err, SetUpTestL(*manager, *noMem, &target, log));
	CheckErrorCode(err, KErrNone);
	CheckLog(*log);
	log->SetCheckString(_L("offset<1>negate<1>"));
	manager->UndoL();
	CheckLog(*log);
	log->SetCheckString(_L("offset<-1>"));
	manager->UndoL();
	CheckLog(*log);
	TestCannotUndo(*manager);
	manager->ResetUndo();
	manager->SetGatekeeper(0);
	delete noMem;

	delete inc;
	delete dec;
	delete neg;
	delete reclaimer;
	delete refuser;
	delete permitter;
	delete log;
	manager->Release();

	__UHEAP_MARKENDC(0);
	}

//
//
//	Tests involving CTestEditor
//
//

void CheckEditorLog(CCheckingLogger& a)
	{
	if (a.Passed())
		{
	    TESTPOINT(1);
		return;
		}
	TESTPRINT(_L("EditorUndo : log failed"));
	TESTPOINT(0);
	}

void TestPlainText(CTestEditor& aTestEditor, MUnifiedEditor& aUndoEditor,
				   CCommandManager& aCommandManager)
	{
	CCheckingLogger* check = new(ELeave) CCheckingLogger;
	CStoringLogger* log = new(ELeave) CStoringLogger;

	//
	// general inserting and deleting text
	//
	aUndoEditor.InsertTextL(0, _L("Hello world!"), 0, 0, 0);

	aCommandManager.ResetUndo();

	aTestEditor.Print(*log);
	HBufC* helloWorldLog = log->GetStore();

	check->SetCheckString(*helloWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);
	aUndoEditor.InsertTextL(5, _L(" lovely"), 0, 0, 0);
	aTestEditor.Print(*log);
	HBufC* helloLovelyWorldLog = log->GetStore();
	aUndoEditor.DeleteTextL(10, 8);
	aTestEditor.Print(*log);
	HBufC* helloLoveLog = log->GetStore();
	aCommandManager.UndoL();
	check->SetCheckString(*helloLovelyWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);
	aCommandManager.UndoL();
	check->SetCheckString(*helloWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);
	aCommandManager.RedoL();
	check->SetCheckString(*helloLovelyWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);
	aCommandManager.RedoL();
	check->SetCheckString(*helloLoveLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);
	aCommandManager.UndoL();
	check->SetCheckString(*helloLovelyWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);
	aCommandManager.UndoL();
	check->SetCheckString(*helloWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aUndoEditor.InsertTextL(6, _L("w"), 0, 0, 0);
	aUndoEditor.InsertTextL(7, _L("h"), 0, 0, 0);
	aUndoEditor.InsertTextL(8, _L("at's"), 0, 0, 0);
	aUndoEditor.InsertTextL(12, _L(" "), 0, 0, 0);
	aUndoEditor.InsertTextL(13, _L("w"), 0, 0, 0);
	aUndoEditor.InsertTextL(14, _L("i"), 0, 0, 0);
	aUndoEditor.InsertTextL(6, _L("there "), 0, 0, 0);
	aUndoEditor.InsertTextL(21, _L("t"), 0, 0, 0);
	aUndoEditor.InsertTextL(22, _L("h"), 0, 0, 0);
	aUndoEditor.InsertTextL(23, _L(" "), 0, 0, 0);
	aTestEditor.Print(*log);
	HBufC* textLog0 = log->GetStore();
	aUndoEditor.InsertTextL(24, _L("the"), 0, 0, 0);	// first of next
	aUndoEditor.InsertTextL(27, _L(" "), 0, 0, 0);
	aUndoEditor.InsertTextL(28, _L("d "), 0, 0, 0);
	aUndoEditor.InsertTextL(28, _L("ol"), 0, 0, 0);
	aTestEditor.Print(*log);
	HBufC* textLog1 = log->GetStore();

	aCommandManager.UndoL();
	check->SetCheckString(*textLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*helloWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*textLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*textLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	// check coalescence of insertions
	aTestEditor.AlterGranularityL(5);
	aUndoEditor.DeleteTextL(22, 1);
	aUndoEditor.DeleteTextL(21, 1);
	aUndoEditor.DeleteTextL(20, 1);
	aUndoEditor.DeleteTextL(19, 1);
	aUndoEditor.DeleteTextL(18, 1);
	aUndoEditor.DeleteTextL(18, 1);
	aUndoEditor.DeleteTextL(15, 3);		// this will coalesce
	aUndoEditor.DeleteTextL(6, 9);		// this won't, as it does not fit in one command
	aTestEditor.Print(*log);
	HBufC* delLog0 = log->GetStore();
	aUndoEditor.DeleteTextL(4, 1);
	aTestEditor.Print(*log);
	HBufC* delLog1 = log->GetStore();
	aUndoEditor.DeleteTextL(8, 2);
	aUndoEditor.DeleteTextL(8, 1);	// should coalesce
	aUndoEditor.DeleteTextL(8, 1);	// should coalesce
	aTestEditor.Print(*log);
	HBufC* delLog3 = log->GetStore();

	aCommandManager.UndoL();
	check->SetCheckString(*delLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*delLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	aCommandManager.UndoL();
	check->SetCheckString(*textLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	aCommandManager.RedoL();
	check->SetCheckString(*delLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*delLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*delLog3);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*delLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*delLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	aCommandManager.UndoL();
	check->SetCheckString(*textLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	aCommandManager.UndoL();
	check->SetCheckString(*helloWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.ResetUndo();
	delete delLog0;
	delete delLog1;
	delete delLog3;

	// Check adding large amounts of text
	aTestEditor.AlterGranularityL(32);
	aUndoEditor.InsertTextL(0, _L("123456789"), 0, 0, 0);
	aUndoEditor.InsertTextL(0, _L("223456789"), 0, 0, 0);
	aTestEditor.Print(*log);
	HBufC* largeLog0 = log->GetStore();
	aUndoEditor.InsertTextL(0, _L("3234567890"), 0, 0, 0);
	aUndoEditor.InsertTextL(0, _L("4234567890"), 0, 0, 0);
	aTestEditor.Print(*log);
	HBufC* largeLog1 = log->GetStore();
	aUndoEditor.InsertTextL(0, _L("523456789"), 0, 0, 0);
	aTestEditor.Print(*log);
	HBufC* largeLog2 = log->GetStore();

	aCommandManager.UndoL();
	check->SetCheckString(*largeLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*largeLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*helloWorldLog);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*largeLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*largeLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*largeLog2);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*largeLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*largeLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*helloWorldLog);
 	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	aCommandManager.RedoL();
	aCommandManager.RedoL();

	// test copy and paste
	MUnifiedEditor::MClipboardSupport* ci = aUndoEditor.ClipboardSupport();
	ASSERT(ci);

	CBufStore* clipboardBuffer = CBufStore::NewL(100);
	CStreamDictionary* clipboardDictionary = CStreamDictionary::NewL();

	ci->CopyToStoreL(*clipboardBuffer, *clipboardDictionary, 5, 40);
	aTestEditor.Print(*log);
	HBufC* clipLog0 = log->GetStore();
	ci->PasteFromStoreL(*clipboardBuffer, *clipboardDictionary, 2);
	aTestEditor.Print(*log);
	HBufC* clipLog1 = log->GetStore();
	ci->PasteFromStoreL(*clipboardBuffer, *clipboardDictionary, 55);
	aTestEditor.Print(*log);
	HBufC* clipLog2 = log->GetStore();
	ci->PasteFromStoreL(*clipboardBuffer, *clipboardDictionary, 23);
	aTestEditor.Print(*log);
	HBufC* clipLog3 = log->GetStore();

	aCommandManager.UndoL();
	check->SetCheckString(*clipLog2);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*clipLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*clipLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*clipLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*clipLog2);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.RedoL();
	check->SetCheckString(*clipLog3);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*clipLog2);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*clipLog1);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	aCommandManager.UndoL();
	check->SetCheckString(*clipLog0);
	aTestEditor.Print(*check);
	CheckEditorLog(*check);

	delete clipLog0;
	delete clipLog1;
	delete clipLog2;
	delete clipLog3;
	delete clipboardDictionary;
	delete clipboardBuffer;

	delete textLog0;
	delete textLog1;

	delete largeLog0;
	delete largeLog1;
	delete largeLog2;

	delete helloWorldLog;
	delete helloLovelyWorldLog;
	delete helloLoveLog;
	delete log;
	delete check;
	}

// This class merely splits the test function into little functions
// to help out the MW compiler
class TestEditorUndo
	{
	CCheckingLogger* check;
	CStoringLogger* log;
	CTestEditor* testEd;
	CCommandManager* manager;
	CEditorPlainTextWithUndo* plainEd;
	CEditorWithUndo* ed;
	TTmCharFormatMask charBMask;
	TTmCharFormatMask charIMask;
	TTmCharFormatMask charBIMask;
	TOpenFontFaceAttribBase attrib;
	TTmCharFormat charB;
	TTmCharFormat charIB;
	TTmCharFormat charI;
	TTmParFormatMask parTMask;
	TTmParFormatMask parNMask;
	TTmParFormatMask parTNMask;
	RTmParFormat par0;
	RTmParFormat parT;
	RTmParFormat parN;
	RTmParFormat parTN;
	TTmCharFormatLayer charLayer;
	RTmParFormatLayer parLayer;
	RTmStyle style1;
	RTmStyle style2;
	HBufC* charLog0;
	HBufC* charLog1;
	HBufC* charLog2;
	HBufC* charLog3;
	HBufC* textLog0;
	HBufC* textLog1;
	HBufC* textLog2;
	HBufC* textLog3;
	HBufC* parLog0;
	HBufC* parLog1;
	HBufC* parLog2;
	HBufC* parLog3;
	HBufC* delLog0;
	HBufC* delLog1;
	HBufC* delLog2;
	HBufC* delLog3;
	HBufC* styleLog1;
	HBufC* styleLog2;
	HBufC* styleLog3;
	HBufC* styleLog4;
	HBufC* styleLog5;
	HBufC* styleLog6;
	HBufC* styleLog7;
	HBufC* styleLog8;
	HBufC* styleLog9;
	HBufC* styleLog10;
	HBufC* styleLog11;
	HBufC* styleLog12;
	HBufC* styleLog13;
	HBufC* picLog0;
	HBufC* picLog1;
	HBufC* picLog2;
	HBufC* picLog3;
	HBufC* picLog4;
	HBufC* picLog5;
	HBufC* bookMarkLog0;
	HBufC* bookMarkLog1;
	HBufC* bookMarkLog2;
	HBufC* bookMarkLog3;
	HBufC* bookMarkLog4;
	HBufC* bookMarkLog5;
public:
	void Test1L();
	void Test2L();
	void Test3L();
	void Test4L();
	void Test5L();
	void Test6L();
	void Test7L();
	void Test8L();
	};

void TestEditorUndo::Test1L()
	{
	check = new(ELeave) CCheckingLogger;
	log = new(ELeave) CStoringLogger;
	testEd = CTestEditor::NewL();
	manager = CCommandManager::NewL();
	plainEd = CEditorPlainTextWithUndo::NewL(*testEd, manager);

	TestPlainText(*testEd, *plainEd, *manager);
	ed = CEditorWithUndo::NewL(*testEd, manager);
	testEd->DeleteTextL(0, testEd->DocumentLength());
	manager->ResetUndo();

	delete plainEd;
	plainEd = 0;

	TestPlainText(*testEd, *ed, *manager);
	manager->Release();
	}

void TestEditorUndo::Test2L()
	{
	// char and par formats
	charBMask.iFlags = TTmCharFormatMask::EBold;
	charIMask.iFlags = TTmCharFormatMask::EItalic;
	charBIMask.iFlags = TTmCharFormatMask::EItalic | TTmCharFormatMask::EBold;
	attrib.SetBold(ETrue);
	charB.iFontSpec.SetAttrib(attrib);
	attrib.SetItalic(ETrue);
	charIB.iFontSpec.SetAttrib(attrib);
	attrib.SetBold(EFalse);
	charI.iFontSpec.SetAttrib(attrib);

	parTMask.iFlags = TTmParFormatMask::EKeepTogether;
	parNMask.iFlags = TTmParFormatMask::EKeepWithNext;
	parTNMask.iFlags = TTmParFormatMask::EKeepTogether | TTmParFormatMask::EKeepWithNext;
	parT.iFlags = RTmParFormat::EKeepTogether;
	parN.iFlags = RTmParFormat::EKeepWithNext;
	parTN.iFlags = RTmParFormat::EKeepWithNext | RTmParFormat::EKeepTogether;

	charLayer.iFormat = charB;
	charLayer.iMask = charBMask;
	ed->SetCharFormatL(0, 5, charLayer);
	testEd->Print(*log);
	charLog0 = log->GetStore();

	charLayer.iFormat = charI;
	charLayer.iMask = charIMask;
	ed->SetCharFormatL(3, 9, charLayer);
	testEd->Print(*log);
	charLog1 = log->GetStore();

	charLayer.iFormat = charB;
	charLayer.iMask = charBIMask;
	ed->SetCharFormatL(2, 5, charLayer);
	testEd->Print(*log);
	charLog2 = log->GetStore();

	ed->DeleteCharFormatL(1, 10);
	testEd->Print(*log);
	charLog3 = log->GetStore();

	ed->UndoL();
	check->SetCheckString(*charLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*charLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*charLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*charLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*charLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*charLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	parLayer.iMask = parTMask;
	parLayer.iFormat.CopyL(parT);
	ed->SetParFormatL(5, 7, parLayer);
	testEd->Print(*log);
	parLog0 = log->GetStore();

	parLayer.iMask = parTNMask;
	parLayer.iFormat.CopyL(parN);
	ed->SetParFormatL(0, 7, parLayer);
	testEd->Print(*log);
	parLog1 = log->GetStore();

	ed->DeleteParFormatL(4, 4);
	testEd->Print(*log);
	parLog2 = log->GetStore();

	parLayer.iMask = parNMask;
	parLayer.iFormat.CopyL(parN);
	ed->SetParFormatL(3, 6, parLayer);
	testEd->Print(*log);
	parLog3 = log->GetStore();

	ed->UndoL();
	check->SetCheckString(*parLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*parLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*parLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*charLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*parLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*parLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*parLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*parLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);
	}

void TestEditorUndo::Test3L()
	{
	// check coalescence of deletions
	charLayer.iMask = charIMask;
	charLayer.iFormat = charI;
	parLayer.iMask = parNMask;
	parLayer.iFormat.CopyL(parN);
	ed->InsertTextL(6, _L("w"), 0, &charLayer, &parLayer);
	ed->InsertTextL(7, _L("h"), 0, &charLayer, &parLayer);
	ed->InsertTextL(8, _L("at's"), 0, &charLayer, &parLayer);
	ed->InsertTextL(12, _L(" "), 0, &charLayer, &parLayer);
	ed->InsertTextL(13, _L("w"), 0, &charLayer, &parLayer);
	ed->InsertTextL(14, _L("i"), 0, &charLayer, &parLayer);
	ed->InsertTextL(6, _L("there "), 0, &charLayer, &parLayer);
	ed->InsertTextL(21, _L("t"), 0, &charLayer, &parLayer);
	ed->InsertTextL(22, _L("h"), 0, &charLayer, &parLayer);
	ed->InsertTextL(23, _L(" "), 0, &charLayer, &parLayer);
	testEd->Print(*log);
	textLog0 = log->GetStore();
	ed->InsertTextL(24, _L("the"), 0, &charLayer, &parLayer);	// first of next?
	ed->InsertTextL(27, _L(" "), 0, &charLayer, &parLayer);
	testEd->Print(*log);
	textLog1 = log->GetStore();
	charLayer.iMask = charBIMask;
	ed->InsertTextL(28, _L("ol"), 0, &charLayer, &parLayer);
	testEd->Print(*log);
	textLog2 = log->GetStore();
	parLayer.iMask = parTNMask;
	ed->InsertTextL(30, _L("d "), 0, &charLayer, &parLayer);
	testEd->Print(*log);
	textLog3 = log->GetStore();

	ed->UndoL();
	check->SetCheckString(*textLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*parLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*textLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*textLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);
	ed->ResetUndo();
	}

void TestEditorUndo::Test4L()
	{
	// check coalescence of insertions
	testEd->AlterGranularityL(5);
	ed->DeleteTextL(22, 1);
	ed->DeleteTextL(21, 1);
	ed->DeleteTextL(20, 1);
	ed->DeleteTextL(19, 1);
	ed->DeleteTextL(18, 1);
	ed->DeleteTextL(18, 1);
	ed->DeleteTextL(15, 3);		// this will coalesce
	ed->DeleteTextL(6, 9);		// this won't, as it does not fit in one command
	testEd->Print(*log);
	delLog0 = log->GetStore();
	ed->DeleteTextL(4, 1);
	testEd->Print(*log);
	delLog1 = log->GetStore();
	ed->DeleteTextL(8, 2);
	ed->DeleteTextL(8, 1);	// should coalesce
	testEd->Print(*log);
	delLog2 = log->GetStore();
	ed->DeleteTextL(8, 1);	// should fail to coalesce
	testEd->Print(*log);
	delLog3 = log->GetStore();

	ed->UndoL();
	check->SetCheckString(*delLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*delLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*delLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	ed->UndoL();
	check->SetCheckString(*textLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	ed->RedoL();
	check->SetCheckString(*delLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*delLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*delLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*delLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*delLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*delLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*delLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	ed->UndoL();
	check->SetCheckString(*textLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);
	ed->ResetUndo();
	delete delLog0;
	delete delLog1;
	delete delLog2;
	delete delLog3;
	}

void TestEditorUndo::Test5L()
	{
	// Check adding large amounts of text
	testEd->AlterGranularityL(32);
	ed->InsertTextL(0, _L("123456789"), 0, 0, 0);
	ed->InsertTextL(0, _L("223456789"), 0, 0, 0);
	testEd->Print(*log);
	delete textLog0;
	textLog0 = log->GetStore();
	ed->InsertTextL(0, _L("3234567890"), 0, 0, 0);
	ed->InsertTextL(0, _L("4234567890"), 0, 0, 0);
	testEd->Print(*log);
	delete textLog1;
	textLog1 = log->GetStore();
	ed->InsertTextL(0, _L("523456789"), 0, 0, 0);
	testEd->Print(*log);
	delete textLog2;
	textLog2 = log->GetStore();

	ed->UndoL();
	check->SetCheckString(*textLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*textLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*textLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*textLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*textLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*textLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*textLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*textLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*textLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);
	}

void TestEditorUndo::Test6L()
	{
	// test style manipulation
	style1.iName = _L("author");
	style2.iName = _L("title");
	style2.iNextStyleName = _L("author");
	style1.iCharFormat.iFormat = charI;
	style1.iCharFormat.iMask = charIMask;
	style1.iParFormat.iFormat.CopyL(parT);
	style1.iParFormat.iMask = parTNMask;
	style2.iCharFormat.iFormat = charB;
	style2.iCharFormat.iMask = charBIMask;
	style2.iParFormat.iFormat.CopyL(parN);
	style2.iParFormat.iMask = parNMask;

	ed->StyleSupport()->CreateStyleL(style1);
	testEd->Print(*log);
	styleLog1 = log->GetStore();
	TInt retval = ed->StyleSupport()->SetStyleL(1, 3, _L("author"));
	testEd->Print(*log);
	styleLog2 = log->GetStore();
	if (retval != KErrNone)
		{
        TESTPRINT(_L("EditorUndo : apply style failed"));
        TESTPOINT(0);
		}
	TPtrC testStyleName;
	TInt testStyleRunLength;
	ed->StyleSupport()->GetStyle(1, testStyleName, testStyleRunLength);
	if (testStyleRunLength != 3 || testStyleName != style1.iName)
		{
        TESTPRINT(_L("EditorUndo : apply style failed"));
		TESTPOINT(0);
		}
	ed->InsertTextL(5, _L(","), &style1.iName, 0, 0);
	testEd->Print(*log);
	styleLog3 = log->GetStore();
	ed->StyleSupport()->CreateStyleL(style2);
	testEd->Print(*log);
	styleLog4 = log->GetStore();
	ed->StyleSupport()->SetStyleL(2, 7, _L("title"));
	testEd->Print(*log);
	styleLog5 = log->GetStore();
	ed->StyleSupport()->SetStyleL(10, 4, _L("title"));
	testEd->Print(*log);
	styleLog6 = log->GetStore();
	ed->StyleSupport()->SetStyleL(8, 4, _L("author"));
	testEd->Print(*log);
	styleLog7 = log->GetStore();
	style1.iCharFormat.iFormat = charB;
	style1.iCharFormat.iMask = charBMask;
	ed->StyleSupport()->ChangeStyleL(style1);
	testEd->Print(*log);
	styleLog8 = log->GetStore();
 	ed->StyleSupport()->RenameStyleL(_L("author"), _L("version"));
	style1.iName = _L("version");
	testEd->Print(*log);
	styleLog9 = log->GetStore();
	retval = ed->StyleSupport()->SetStyleL(10, 1, _L("version"));
	testEd->Print(*log);
	styleLog10 = log->GetStore();
	if (retval != KErrNone)
		{
        TESTPRINT(_L("EditorUndo : rename style failed"));
		TESTPOINT(0);
		}
	ed->StyleSupport()->GetStyle(1, testStyleName, testStyleRunLength);
	if (testStyleRunLength != 1 || testStyleName != style1.iName)
		{
        TESTPRINT(_L("EditorUndo : rename or apply style failed"));
		TESTPOINT(0);
		}
	ed->StyleSupport()->RenameStyleL(_L("title"), _L("zip"));
	style2.iName = _L("zip");
	testEd->Print(*log);
	styleLog11 = log->GetStore();
	ed->StyleSupport()->SetStyleL(0, 6, _L("zip"));
	testEd->Print(*log);
	styleLog12 = log->GetStore();
	ed->StyleSupport()->DeleteStyleL(_L("zip"));
	testEd->Print(*log);
	styleLog13 = log->GetStore();
	ed->InsertTextL(0, _L("Well "), &style1.iName, 0, 0);

	ed->UndoL();
	check->SetCheckString(*styleLog13);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog12);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog11);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog10);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog9);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog8);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog7);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog6);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog5);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog4);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*styleLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*textLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog4);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog5);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog6);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog7);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog8);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog9);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog10);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog11);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog12);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*styleLog13);
	testEd->Print(*check);
	CheckEditorLog(*check);

	// probably need some more style tests that test the full range of
	// attributes that a style may have.
	//...

	delete textLog0;
	delete textLog1;
	delete textLog2;
	delete parLog0;
	delete parLog1;
	delete parLog2;
	delete parLog3;
	delete charLog0;
	delete charLog1;
	delete charLog2;
	delete charLog3;
	delete styleLog1;
	delete styleLog2;
	delete styleLog3;
	delete styleLog4;
	delete styleLog5;
	delete styleLog6;
	delete styleLog7;
	delete styleLog8;
	delete styleLog9;
	delete styleLog10;
	delete styleLog11;
	delete styleLog12;
	delete styleLog13;

	delete textLog3;
	}

void TestEditorUndo::Test7L()
	{
	// test picture manipulation
	TPictureHeader pic;
	pic.iPictureType = KUidXzePictureType;
	testEd->Print(*log);
	picLog0 = log->GetStore();
	pic.iPicture = new (ELeave) CUndoTestPicture('A');
	ed->PictureSupport()->InsertPictureL(5, pic);
	testEd->Print(*log);
	picLog1 = log->GetStore();
	pic.iPicture = new (ELeave) CUndoTestPicture('B');
	ed->PictureSupport()->InsertPictureL(8, pic);
	testEd->Print(*log);
	picLog2 = log->GetStore();
	pic.iPicture = new (ELeave) CUndoTestPicture('C');
	ed->PictureSupport()->InsertPictureL(9, pic);
	testEd->Print(*log);
	picLog3 = log->GetStore();
	pic.iPicture = new (ELeave) CUndoTestPicture('D');
	ed->PictureSupport()->InsertPictureL(12, pic);
	ed->StyleSupport()->SetStyleL(6, 2, style1.iName);
	ed->SetCharFormatL(8, 3, charLayer);
	ed->SetParFormatL(7, 7, parLayer);
	testEd->Print(*log);
	picLog4 = log->GetStore();
	ed->DeleteTextL(5, 8);
	testEd->Print(*log);
	picLog5 = log->GetStore();

	ed->UndoL();
	check->SetCheckString(*picLog4);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	ed->UndoL();
	ed->UndoL();
	ed->UndoL();
	check->SetCheckString(*picLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*picLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*picLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->UndoL();
	check->SetCheckString(*picLog0);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*picLog1);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*picLog2);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*picLog3);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	ed->RedoL();
	ed->RedoL();
	ed->RedoL();
	check->SetCheckString(*picLog4);
	testEd->Print(*check);
	CheckEditorLog(*check);

	ed->RedoL();
	check->SetCheckString(*picLog5);
	testEd->Print(*check);
	CheckEditorLog(*check);

	style1.Close();
	style2.Close();
	parLayer.Close();
	par0.Close();
	parT.Close();
	parN.Close();
	parTN.Close();
	delete picLog0;
	delete picLog1;
	delete picLog2;
	delete picLog3;
	delete picLog4;
	delete picLog5;
	}

void TestEditorUndo::Test8L()
	{
	// test bookmarking
	for (TInt i = 0; i != 7; ++i)
		{
		testEd->Reset();
		ed->ResetUndo();
		if (i == 0)
			manager->SetBookmark();
		testEd->Print(*log);
		bookMarkLog0 = log->GetStore();
		ed->InsertTextL(0, _L("Hallo"), 0, 0, 0);	// hallo
		if (i == 1)
			manager->SetBookmark();
		testEd->Print(*log);
		bookMarkLog1 = log->GetStore();
		ed->DeleteTextL(2, 1);	// halo
		if (i == 2)
			manager->SetBookmark();
		testEd->Print(*log);
		bookMarkLog2 = log->GetStore();
		manager->BeginBatchLC();
		ed->DeleteTextL(3, 1);	// hal
		ed->InsertTextL(3, _L("t, who goes there?"), 0, 0, 0);	// halt, who goes there?
		if (i == 3)
			manager->SetBookmark();		// should not get set
		ed->DeleteTextL(9, 5);		// halt, who there?
		CleanupStack::PopAndDestroy();
		if (i == 4)
			manager->SetBookmark();
		testEd->Print(*log);
		bookMarkLog3 = log->GetStore();
		ed->InsertTextL(0, _L("Oi"), 0, 0, 0);
		if (i == 5)
			manager->SetBookmark();
		testEd->Print(*log);
		bookMarkLog4 = log->GetStore();
		ed->InsertTextL(2, _L("! "), 0, 0, 0);
		testEd->Print(*log);
		bookMarkLog5 = log->GetStore();
		if (i == 6)
			manager->SetBookmark();

		ed->UndoL();
		// coalescence should have happenned unless there is a bookmark
		// in the way.
		if (i == 5)
			{
            TESTPOINT(manager->IsAtBookmark());
			check->SetCheckString(*bookMarkLog4);
			testEd->Print(*check);
			CheckEditorLog(*check);
			ed->UndoL();
			}
		if (i == 4)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog3);
		testEd->Print(*check);
		CheckEditorLog(*check);
		ed->UndoL();
		if (i == 2)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog2);
		testEd->Print(*check);
		CheckEditorLog(*check);
		ed->UndoL();
		if (i == 1)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog1);
		testEd->Print(*check);
		CheckEditorLog(*check);
		ed->UndoL();
		if (i == 0)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog0);
		testEd->Print(*check);
		CheckEditorLog(*check);
		TESTPOINT(!ed->CanUndo());
		ed->RedoL();
		if (i == 1)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog1);
		testEd->Print(*check);
		CheckEditorLog(*check);
		ed->RedoL();
		if (i == 2)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog2);
		testEd->Print(*check);
		CheckEditorLog(*check);
		ed->RedoL();
		if (i == 4)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());
		check->SetCheckString(*bookMarkLog3);
		testEd->Print(*check);
		CheckEditorLog(*check);
		ed->RedoL();
		if (i == 5)
			{
            TESTPOINT(manager->IsAtBookmark());
			check->SetCheckString(*bookMarkLog4);
			testEd->Print(*check);
			CheckEditorLog(*check);
			ed->RedoL();
			}
		TESTPOINT(!ed->CanRedo());
		if (i == 6)
		    TESTPOINT(manager->IsAtBookmark());
		else
		    TESTPOINT(!manager->IsAtBookmark());

		delete bookMarkLog0;
		delete bookMarkLog1;
		delete bookMarkLog2;
		delete bookMarkLog3;
		delete bookMarkLog4;
		delete bookMarkLog5;
		}

	delete ed;
	delete testEd;
	delete log;
	delete check;
	}

void TestEditorUndoL()
	{
	__UHEAP_MARK;

	TestEditorUndo t;
	t.Test1L();
	t.Test2L();
	t.Test3L();
	t.Test4L();
	t.Test5L();
	t.Test6L();
	t.Test7L();
	t.Test8L();

	__UHEAP_MARKENDC(0);
	}
// integration of command manager with multiple editors
void TestMultipleEditorsL()
	{
	__UHEAP_MARK;

	CCheckingLogger* check = new(ELeave) CCheckingLogger;
	CStoringLogger* log = new(ELeave) CStoringLogger;

	CTestEditor* testEd0 = CTestEditor::NewL();
	CTestEditor* testEd1 = CTestEditor::NewL();
	CTestEditor* testEd2 = CTestEditor::NewL();
	CTestEditor* testEd3 = CTestEditor::NewL();
	CCommandManager* manager = CCommandManager::NewL();

	CEditorPlainTextWithUndo* ed0 =
		CEditorPlainTextWithUndo::NewL(*testEd0, manager);
	CEditorWithUndo* ed1 = CEditorWithUndo::NewL(*testEd1, manager);
	CEditorPlainTextWithUndo* ed2 =
		CEditorPlainTextWithUndo::NewL(*testEd2, manager);
	CEditorWithUndo* ed3 = CEditorWithUndo::NewL(*testEd3, manager);
	manager->Release();

	// Testing the API's of CEditorPlainTextWithUndo
	TTmCharFormatMask charBIMask;
	charBIMask.iFlags = TTmCharFormatMask::EItalic | TTmCharFormatMask::EBold;
	TOpenFontFaceAttribBase attrib;
	TTmCharFormat charB;
	attrib.SetBold(ETrue);
	charB.iFontSpec.SetAttrib(attrib);
	TTmCharFormatLayer charLayer;
	charLayer.iFormat = charB;
	charLayer.iMask = charBIMask;
	TTmParFormatMask parTMask;
	parTMask.iFlags = TTmParFormatMask::EKeepTogether;
	RTmParFormat parT;
	parT.iFlags = RTmParFormat::EKeepTogether;
	RTmParFormatLayer parLayer;
	parLayer.iMask = parTMask;

	//Setting the base, character and paragraph format and then inserting the text
	ed0->SetBaseFormatL(charB,parT);
	ed0->SetCharFormatL(0, 1, charLayer);
	ed0->SetParFormatL(0, 1, parLayer);

	// The main thing to check is that no commands coalesce that have
	// different targets which would if their targets matched. The
	// commands that can coalesce are Delete Text, Delete Plain Text,
	// Insert Text, Insert Plain Text, Delete Character Format, Delete
	// Paragraph Format.
	ed0->InsertTextL(0, _L("ab"), 0, 0, 0);
	testEd0->Print(*log);
	HBufC* log00 = log->GetStore();
	ed2->InsertTextL(0, _L("cd"), 0, 0, 0);
	testEd2->Print(*log);
	HBufC* log20 = log->GetStore();
	ed1->InsertTextL(0, _L("ef"), 0, 0, 0);
	testEd1->Print(*log);
	HBufC* log10 = log->GetStore();
	ed3->InsertTextL(0, _L("gh"), 0, 0, 0);
	testEd3->Print(*log);
	HBufC* log30 = log->GetStore();

	manager->UndoL();
	check->SetCheckString(*log10);
	testEd1->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log20);
	testEd2->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log00);
	testEd0->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	TestCannotUndo(*manager);

	manager->RedoL();
	check->SetCheckString(*log00);
	testEd0->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log20);
	testEd2->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log10);
	testEd1->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log30);
	testEd3->Print(*check);
	CheckEditorLog(*check);
	TestCannotRedo(*manager);

	ed0->DeleteTextL(1, 1);
	testEd0->Print(*log);
	HBufC* log01 = log->GetStore();
	ed2->DeleteTextL(0, 1);
	testEd2->Print(*log);
	HBufC* log21 = log->GetStore();
	ed3->DeleteTextL(0, 1);
	testEd3->Print(*log);
	HBufC* log31 = log->GetStore();
	ed1->DeleteTextL(0, 1);
	testEd1->Print(*log);
	HBufC* log11 = log->GetStore();

	manager->UndoL();
	check->SetCheckString(*log31);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log21);
	testEd2->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log01);
	testEd0->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log30);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log01);
	testEd0->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log21);
	testEd2->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log31);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log11);
	testEd1->Print(*check);
	CheckEditorLog(*check);
	TestCannotRedo(*manager);

	parLayer.iFormat.CopyL(parT);

	//Getting the base format to check if it is set accordingly
	TTmCharFormat charB1;
	RTmParFormat parT1;
	ed0->GetBaseFormatL(charB1,parT1);
	TESTPOINT(charB1==charB);
	TESTPOINT(parT1==parT);

	//Getting the character format
	TTmCharFormatLayer charLayer1;
	MUnifiedEditor::TFormatLevel level=MUnifiedEditor::EEffective;
	TInt runLen=10;
	ed0->GetCharFormat(0,level,charLayer1,runLen);

	//Getting the paragraph format
	RTmParFormatLayer parLayer1;
	ed0->GetParFormatL(0,level,parLayer1,runLen);

	//Getting the text
	TPtrC text;
	ed0->GetText(0,text);
	TESTPOINT(text==_L("a"));

	//Deleting the formating
	ed0->DeleteCharFormatL(0,1);
	ed0->DeleteParFormatL(0,1);

	// To test CEditorCommandSetBaseFormat class
	// SetBaseFormatL calls CEditorCommandSetBaseFormatProto::CreateInverseL() which in turn calls CEditorCommandSetBaseFormat::NewL().
	ed1->SetBaseFormatL(charB,parT);
	ed1->SetCharFormatL(0, 1, charLayer);

	testEd1->Print(*log);
	HBufC* log12 = log->GetStore();
	ed3->SetCharFormatL(0, 1 ,charLayer);
	testEd3->Print(*log);
	HBufC* log32 = log->GetStore();
	ed3->SetParFormatL(0, 1, parLayer);
	testEd3->Print(*log);
	HBufC* log33 = log->GetStore();
	ed1->SetParFormatL(0, 1, parLayer);
	testEd1->Print(*log);
	HBufC* log13 = log->GetStore();

	manager->UndoL();
	check->SetCheckString(*log33);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log32);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->UndoL();
	check->SetCheckString(*log12);
	testEd1->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log32);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log33);
	testEd3->Print(*check);
	CheckEditorLog(*check);

	manager->RedoL();
	check->SetCheckString(*log13);
	testEd1->Print(*check);
	CheckEditorLog(*check);

	parLayer.Close();
	parT.Close();
	delete log00;
	delete log10;
	delete log20;
	delete log30;
	delete log01;
	delete log11;
	delete log21;
	delete log31;
	delete log12;
	delete log13;
	delete log32;
	delete log33;
	manager->ResetUndo();
	delete ed0;
	delete ed1;
	delete ed2;
	delete ed3;
	delete testEd0;
	delete testEd1;
	delete testEd2;
	delete testEd3;
	delete log;
	delete check;

	__UHEAP_MARKENDC(0);
	}

//
//
//  Main
//
//

TVerdict CTUndoStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("TUndo - Undo system"));
    
    __UHEAP_MARK;
    TESTPRINT(_L("@SYMTestCaseID:SYSLIB-FORM-LEGACY-UNDO-0001 Undo System Tests: "));
	
	// test of general undo system components
	TestCCommandStackL();
	TestCBatchCommandL();
	TestCCommandHistoryL();
	TestCCommandManagerL();

	// test of editor undo components
	TestEditorUndoL();

	// test that command manager and multiple editors integrate correctly
	TestMultipleEditorsL();

	__UHEAP_MARKENDC(0);
	
	return TestStepResult();
	}

