/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef EDITORCOMMANDS_H_
#define EDITORCOMMANDS_H_

#include "unified_editor.h"
#include "UniqueInstance.h"
#include "UndoSystem.h"
#include "EditorPlainTextCommands.h"

// all the combinable commands:
class CEditorCommandInsertTextAndFormat;
class CEditorCommandDeleteText;
class CEditorCommandDeleteCharFormat;
class CEditorCommandDeleteParFormat;

/**
 * Pointers to the repositories used by the undo system.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
struct TRepositories
	{
	CUniqueInstanceRepository<TTmCharFormat>*	iChar;
	CUniqueInstanceRepository<RTmParFormat>*	iPar;
	CUniqueInstanceRepository<TDes>*			iDes;
	};

//
// command prototypes
//
/**
 * Base class for command prototypes. These prototypes are used to generate
 * inverses. They do not need any copying to be sent into the undo system,
 * executed, then recycled. Some also store a pointer to the last generated
 * inverse so that they can add to it if two similar commands come along
 * together.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class CEditorCommandProto : public CEditorCommand
	{
	const TRepositories& iReps;
public:
	CEditorCommandProto(const TRepositories& aReps)
		: iReps(aReps) {}
	const TRepositories& Repositories() const { return iReps; }
	};

/**
 * Prototype command for creating a style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandCreateStyleProto) : public CEditorCommandProto
	{
	MUnifiedEditor&	iTarget;
	const RTmStyle*	iStyle;

public:
	CEditorCommandCreateStyleProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(const RTmStyle&);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for setting the attributes of a style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandChangeStyleProto) : public CEditorCommandProto
	{
	MUnifiedEditor&		iTarget;
	const RTmStyle*	iStyle;

public:
	CEditorCommandChangeStyleProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(const RTmStyle&);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for applying a style to a run of text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetStyleProto) : public CEditorCommandProto
	{
	MUnifiedEditor&			iTarget;
	TInt					iPos;
	TInt					iLength;
	const TDesC*			iName;

public:
	CEditorCommandSetStyleProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos, TInt aLength,
		const TDesC& aName);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for deleting a style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteStyleProto) : public CEditorCommandProto
	{
	MUnifiedEditor&				iTarget;
	const TDesC*			iName;

public:
	CEditorCommandDeleteStyleProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(const TDesC& aName);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for setting the character format of a run of text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetCharFormatProto) : public CEditorCommandProto
	{
	MUnifiedEditor&				iTarget;
	TInt						iPos;
	TInt						iLength;
	const TTmCharFormatLayer*	iFormat;

public:
	CEditorCommandSetCharFormatProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos,
		TInt aLength,
		const TTmCharFormatLayer& aFormat);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for setting the paragraph format of a run of text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetParFormatProto) : public CEditorCommandProto
	{
	MUnifiedEditor&					iTarget;
	TInt						iPos;
	TInt						iLength;
	const RTmParFormatLayer*	iFormat;

public:
	CEditorCommandSetParFormatProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos,
		TInt aLength,
		const RTmParFormatLayer& aFormat);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for inserting text with specified character and paragraph
 * format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandInsertProto) : public CEditorCommandProto
	{
	MUnifiedEditor&						iTarget;
	TInt								iPos;
	const TDesC*						iText;
	const TTmCharFormatLayer*			iCharFormat;
	const RTmParFormatLayer*			iParFormat;
	const TDesC*						iStyle;

public:
	CEditorCommandInsertProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos,
		const TDesC& aText,
		const TDesC* aStyle,
		const TTmCharFormatLayer* aCharFormat,
		const RTmParFormatLayer* aParFormat);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	TBool PrepareToAddInverseToLastL(UndoSystem::CSingleCommand& aLastCommand) const;
	void AddInverseToLast(UndoSystem::CSingleCommand& aLastCommand) const;
	};

/**
 * Prototype command for deleting text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteProto) : public CEditorCommandProto
	{
	enum { KMaxCombinableReinsertCharacters = 20 };

	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;
	// For adding inverse to last command
	mutable TBuf<KMaxCombinableReinsertCharacters>	iDeletedText;

public:
	CEditorCommandDeleteProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos, TInt aLength);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;

	TBool PrepareToAddInverseToLastL(UndoSystem::CSingleCommand& aLastCommand) const;
	void AddInverseToLast(UndoSystem::CSingleCommand& aLastCommand) const;
	};

/**
 * Prototype command for setting the base format.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandSetBaseFormatProto) : public CEditorCommandProto
	{
	MUnifiedEditor&			iTarget;
	const TTmCharFormat*	iChar;
	const RTmParFormat*		iPar;

public:
	CEditorCommandSetBaseFormatProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(const TTmCharFormat* aCharFormat, const RTmParFormat* aParFormat);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for deleting specific character formatting.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteCharFormatProto) : public CEditorCommandProto
	{
	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;

public:
	CEditorCommandDeleteCharFormatProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos, TInt aLength);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for deleting specific paragraph formatting.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeleteParFormatProto) : public CEditorCommandProto
	{
	MUnifiedEditor&	iTarget;
	TInt		iPos;
	TInt		iLength;

public:
	CEditorCommandDeleteParFormatProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos, TInt aLength);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for deleting a picture.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDeletePictureProto) : public CEditorCommandProto
	{
	MUnifiedEditor&	iTarget;
	TInt		iPos;

public:
	CEditorCommandDeletePictureProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos);

	TInt ExecuteL() const;
	};

/**
 * Prototype command for inserting a picture.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandInsertPictureProto) : public CEditorCommandProto
	{
	MUnifiedEditor&			iTarget;
	TInt					iPos;
	const TPictureHeader*	iPicture;

public:
	CEditorCommandInsertPictureProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(TInt aPos, const TPictureHeader& picture);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for renaming a style.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandRenameStyleProto) : public CEditorCommandProto
	{
	MUnifiedEditor&		iTarget;
	const TDesC*	iOldName;
	const TDesC*	iNewName;

public:
	CEditorCommandRenameStyleProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	void Set(const TDesC& aOldName, const TDesC& aNewName);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Prototype command for pasting text.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandPasteProto) : public CEditorCommandProto
	{
	TEditorPasteProtoImpl iImpl;

public:
	CEditorCommandPasteProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iImpl(aTarget) {}
	void Set(const CStreamStore& aStore,
		const CStreamDictionary& aStreamDictionary,
		TInt aPos);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

/**
 * Owner of a picture which will eventually pass on ownership.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class MPictureOwner
	{
public:
	/**
	 * Forgets the delegate that has been made next in line for
	 * the picture.
	 */
	virtual void ForgetDelegate() = 0;
	};

class CEditorCommandInsertPicture;

/**
 * Prototype command for deleting a picture from the text without
 * returning ownership.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CEditorCommandDestroyPictureProto) : public CEditorCommandProto,
	private MPictureOwner
	{
	MUnifiedEditor&								iTarget;
	TInt									iPos;
	/**
	 * Will own picture after us.
	 */
	mutable CEditorCommandInsertPicture*	iPictureOwnerDelegate;

	void ForgetDelegate();
public:
	CEditorCommandDestroyPictureProto(const TRepositories& aReps, MUnifiedEditor& aTarget)
		: CEditorCommandProto(aReps), iTarget(aTarget) {}
	~CEditorCommandDestroyPictureProto();
	void Set(TInt aPos);

	UndoSystem::CCommand* CreateInverseL() const;
	TInt ExecuteL() const;
	};

namespace UndoSystem
	{
	/**
	 * Finds the position of the first picture in the text. Returns
	 * 'KNotFound' if there are none. This will not return any
	 * picture characters that do not have attatched pictures.
	 *
	 * @internalComponent
	 * @since App-frameworks6.1
	 */
	TInt FindPicture(const MUnifiedEditor& aTarget, TInt aPos, TInt aLength);
	}

#endif	// EDITORCOMMANDS_H_
