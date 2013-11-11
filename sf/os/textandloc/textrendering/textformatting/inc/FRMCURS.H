/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* FORM Cursor Navigation header file holding policy interface class.
*
*/


#ifndef __FRMCURS_H__
#define __FRMCURS_H__


#include <frmconst.h>
#include <frmconst_partner.h>


class CTmViewAnchoredLayout;
class TTmDocPos;

/**
This policy interface class maintains the cursor position and sets the 
policy for its movement.
@publishedPartner
@deprecated
*/
class MCursorPositioningPolicy

	{
public:

	//-- Interface enumerations ---------------------------------------------


	/** Types of delete operation. This is used when...???
	*/
	enum TDeleteType
		{
		EDeleteUndefined = 0,
		EDeleteBackspace = 1,
		EDeleteForward   = 2,

		EDeleteLast
		};
 
	/** Types of reformatting. This is used when...???
	*/
	enum TTextChangeType
		{
		ETextChangeUndefined = 0,
		/** Change that causes no change in cursor position. */
		ETextChangeNonLocal  = 1,
		/** Change that moves the cursor to its trailing edge. */
		ETextChangeTyping    = 2,

		// Maybe several more.
		// ...
		ETExtChangeLast
	};
 
	/** 
	Hint for positioning the visual cursor. Describes properties of the
	editing operation that is most likely to happen next. 
	*/
	enum TPosHint
		{
		EPosHintUndefined           = 0,
		/** Left to right typing expected. */
		EInsertStrongL2R = 1,
		/** Right to left typing expected. */
		EInsertStrongR2L = 2,

		// Maybe several more.
		//...
		EPosHintLast
		};

public:

	//-- Object creation/setup API ------------------------------------------

	/**
	*/
	virtual ~MCursorPositioningPolicy() {}

	/**
	Sets the document/layout object on which all the cursor navigation will 
	take place. 
	*/
	virtual void SetTmLayout(CTmViewAnchoredLayout* aLayout) = 0;


	//-- Cursor positioning API ---------------------------------------------
	
	/** 
	Gets the cursor position for display and insertion. 
	@return TTmDocPos value holding cursor document position.
	*/
	virtual const TTmDocPos& Pos() const = 0;

	/** 
	Sets the cursor position explicitly.
	@param aNewPos New position cursor is to be moved to.
	*/
	virtual void SetPos(const TTmDocPos& aNewPos) = 0;


	/** 
	@return ETrue if successful, EFalse otherwise e.g. no formatting	
	*/
	virtual void PosXyL(TPoint& aXy) const = 0;

	/** 
	Sets the X, Y co-ordinates to jump to. 
	@param aXy Layout co-ordinates to approx. move the cursor to.
			   On exit
	@return ETrue if successful, EFalse otherwise e.g. no formatting	
	*/
	virtual void SetPosByXyL(TPoint& aNewXy) = 0;

	/** 
	Allows change of cursor position in response to a hint as to 
	positioning. 
	@param aHint Text directional hint from client.
	*/
	virtual void SetPositioningHintL(TPosHint aHint) = 0;
	
	/** 
	Suggests a new latent X,Y position for Up/Down operations/scrolls. 
	@param aX X Layout co-ordinate for latent position of cursor.
	*/
	virtual void SetLatentX(TInt aX) = 0;

	/** 
	Suggests a new latent X,Y position for Up/Down operations/scrolls. 
	@param aY Y Layout co-ordinate for latent position of cursor.
	*/
	virtual void SetLatentY(TInt aY) = 0;

	/** 
	Suggests a new latent X,Y position for Up/Down operations/scrolls. 
	@param aXy X,Y Layout co-ordinate for latent position of cursor.
	*/
	virtual const TPoint& LatentXy() const = 0;

	/** 
	Suggests a new latent X,Y position for Up/Down operations/scrolls. 
	@param aXy X,Y Layout co-ordinate for latent position of cursor.
	*/
	virtual void SetLatentXy(const TPoint& aXy) = 0;

	/** 
	@leave 
	*/
	virtual const TPoint& UpdateLatentXyL() = 0;


	//--- Cursor navigation API ---------------------------------------------
	

	/**
	Moves the cursor up or down a line.
	@param aUp ETrue if wish to move up, EFalse to go down.
	@return TCursorMove Actual movement that took place.
	*/
	virtual TCursorMove UpDownL(TBool aUp) = 0;

	/** 
	Moves the cursor left or right a character position. 
	@param aDown ETrue if wish to move left, EFalse to go right.
	@return TCursorMove Actual movement that took place.
	*/
	virtual TCursorMove LeftRightL(TBool aLeft) = 0;

	/** 
	Moves the cursor to the start or to the end of the line. 
	@param aStart ETrue if wish to move to the start, EFalse to go to the end.
	@return TCursorMove Actual movement that took place.
	*/
	virtual TCursorMove LineStartEndL(TBool aStart) = 0;


	//--- Cursor reactionary API to changes ---------------------------------


	/** 
	Responds to total change of formatting. 
	*/
	virtual void HandleGlobalReformatL() = 0;

	/** 
	Responds to partial change of formatting.
	@param aStart ?
	@param aOldEnd ?
	@param aNewEnd ?
	@param aParaFormatChg ?
	@param aChgType ?
	*/
	virtual void HandleReformatL(TInt aStart, TInt aOldEnd, 
		TInt aNewEnd, TBool aParaFormatChg, TTextChangeType aChgType) = 0;
	

	//-- Misc/Unknown API ---------------------------------------------------


	/** 
	Gets the cursor position for delete. ???
	@param aDelType ?
	@param aStart ?
	@param aEnd ?
	*/
	virtual void GetDeletePosition(TDeleteType aDelType, TInt& aStart, 
		TInt& aEnd) const = 0;
		
	/**
	Place holder for future expansion when if more virtual methods 
	are requried.
	*/
	virtual void* ExtendedInterface(TUid aInterfaceId) = 0;

	};


#endif // __FRMCURS_H__ 
