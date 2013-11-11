/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The specification of a linked typeface passed from font store to a rasterizer.
*
*/


#ifndef OPENFONTLINKEDTYPEFACESPECIFICATION_H_
#define OPENFONTLINKEDTYPEFACESPECIFICATION_H_

class COpenFontLinkedTypefaceElementSpec;
class CLinkedTypefaceGroup;
class TLinkedTypefaceSpecificationArgs;

/**
This class contains the specification of a linked typeface passed from font store to a rasterizer. It is made up of groups and typefaces.
A group (CLinkedTypefaceGroup) contains a number of attributes that can apply to similar typefaces
within the linked typeface. One or more typeface may belong to a group. Typefaces (COpenFontLinkedTypefaceElementSpec)
specifiy which typefaces are used within this linked font.

After creation, the linked typeface can be used as if it were any other typeface. This assumes that there
is a rasteriser that supports this feature available. When looking up characters the first typeface containing
the required character is used.

KMaxLinkedTypefaces and KMaxLinkedTypefaceGroups define the maximum number of typefaces and groups that can be
added to the specification.

@publishedPartner
@released
@see COpenFontLinkedTypefaceElementSpec
@see CLinkedTypefaceGroup
@see CLinkedTypefaceSpecification
@see CLinkedTypefaceElementSpec
*/
NONSHARABLE_CLASS(COpenFontLinkedTypefaceSpecification) : public CBase
	{
public:
	/**
		This function constructs the linked typeface specification and places it on the CleanupStack.
		
		@param aName The name of the linked typeface. This name should not be the same as an existing typeface.
		
		@return CLinkedTypefaceSpecification* A linked typeface specification object
		*/
	IMPORT_C static COpenFontLinkedTypefaceSpecification* NewLC(const TDesC& aName);

	/**
	Returns the name of this linked typeface
	@return The name of this linked typeface
	*/
	IMPORT_C TPtrC Name() const;
	
	/**
	Returns the number of typefaces that have been added to this specification
	@return The number of typefaces that have been added to this specification
	*/
	IMPORT_C TInt TypefaceCount() const;

	/**
	Returns the linked typeface at the specified index.
	
	@return The linked typeface; NULL if there is no typeface at the specified index
	@see COpenFontLinkedTypefaceSpecification::NumberOfTypefaces()
	*/
	IMPORT_C COpenFontLinkedTypefaceElementSpec* Typeface(TInt aIndex) const;

	/**
	Returns the index of the canonical (metrics) typeface.
	
	The can be retrieved by passing the index to Typeface().
	
	@return The index to the canonical typeface; KErrNoFound The canonical doesn't exist; KErrOverflow There is more than one canonical
	@see COpenFontLinkedTypefaceSpecification::Typeface()
	*/
	IMPORT_C TInt CanonicalIndex() const;
	
	/**
	Adds the supplied CLinkedTypefaceElementSpec to the Linked Typeface specification.
	The element is placed at the specified index. Adding a canonical element will only be successful
	if one has not already been added. The specification takes ownership of the element specification.
	
	@param aElementSpec The specification to be added.
	@param aIndex The position at which the element spec will be added.

	@see CLinkedTypefaceSpecification::AddTypefaceAtBackL()
	@see CLinkedTypefaceSpecification::RemoveTypeface()
	@see KMaxLinkedTypefaces
	
	@leave KErrArgument If the element is canonical and a canonical element already exists or if the group ID doesn't exist
	@leave KErrNoMemory There was not enough memory available to perform this operation
	@leave KErrOverflow The typeface specification is full; adding would exceed KMaxLinkedTypefaces
	*/
	IMPORT_C void AddTypefaceAtIndexL(COpenFontLinkedTypefaceElementSpec& aElementSpec, TInt aIndex);

	/**
	Adds the supplied CLinkedTypefaceElementSpec to the Linked Typeface specification.
	The element is placed at the back of the specification. Adding a canonical element will only be successful
	if one has not already been added. The specification takes ownership of the element specification.
	
	@param aElementSpec

	@see CLinkedTypefaceSpecification::AddTypefaceAtIndexL()
	@see CLinkedTypefaceSpecification::RemoveTypeface()
	@see KMaxLinkedTypefaces

	@leave KErrArgument If the element is canonical and a canonical element already exists or if the group ID doesn't exist
	@leave KErrNoMemory There was not enough memory available to perform this operation
	@leave KErrOverflow The typeface specification is full; adding would exceed KMaxLinkedTypefaces
	*/
	IMPORT_C void AddTypefaceAtBackL(COpenFontLinkedTypefaceElementSpec& aElementSpec);

	/**
	Removes the element spec at the specified index from this specification.
	This function deletes the element spec. If the element is the only one within its group
	the group is also removed and deleted from the specification.

	@param aIndex the index of the element spec to be removed

	@return KErrNotFound If the index is negative or larger than the number of typeface elements or the element doesn't exist
	*/
	IMPORT_C TInt RemoveTypeface(TInt aIndex);

	/**
	 Adds the group specified to this specification
	 The specification takes ownership of the group if addition is successful.
	 
	 @param aGroup The group to be added
	 
	 @see COpenFontLinkedTypefaceSpecification::RemoveLinkedTypefaceGroup()
	 
	 @leave KErrAlreadyExists A group with the same ID is already present in the specification
	 */
	IMPORT_C void AddLinkedTypefaceGroupL(CLinkedTypefaceGroup& aGroup);
	
	/**
	This will remove the group and all the elements associated with it.
	
	@param aGroupId The Id of the group to be removed
	*/
	IMPORT_C TInt RemoveLinkedTypefaceGroup(TInt aGroupId);
	
	/**
	Returns the linked typeface group with the specified group ID.
	
	@param aGroupId The linked typeface group ID
	@return The linked typeface group specified or NULL if it doesn't exist
	*/
	IMPORT_C CLinkedTypefaceGroup* GroupById(TInt aGroupId) const;
	
	/**
	Returns the linked typeface group at the specified index within the specification.
	The Index ranges from 0 to the value returned by NumberOfGroups()-1.
	
	@param aGroupIndex The linked typeface groups index within the specification
	@return The linked typeface group specified or NULL if it doesn't exist
	*/
	IMPORT_C const CLinkedTypefaceGroup* Group(TInt aIndex) const;
	
	/**
	Returns the number of groups that this linked typeface specification contains.
	
	@see CLinkedTypefaceSpecification::Group()
	
	@return TInt Number of groups
	*/
	IMPORT_C TInt GroupCount() const;

	/**
	Default destructor
	*/
	IMPORT_C ~COpenFontLinkedTypefaceSpecification();
	
	/**
	This function constructs the linked typeface specification from the passed TLinkedTypefaceSpecificationArgs and places it on the CleanupStack.
		
	WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.

	@param aSpecArgs The TLinkedTypefaceSpecificationArgs container passed from font and bitmap server	
	@return A linked typeface specification object
	*/
	IMPORT_C static COpenFontLinkedTypefaceSpecification* NewLC(const TLinkedTypefaceSpecificationArgs& aSpecArgs);

private:
	COpenFontLinkedTypefaceSpecification();
	void ConstructL(const TDesC& aName);
	void ConstructL(const TLinkedTypefaceSpecificationArgs& aSpecArgs);
	void DoAddTypefaceL(COpenFontLinkedTypefaceElementSpec& aElementSpec, TInt aPos);
private:
	RPointerArray <COpenFontLinkedTypefaceElementSpec> iTypefaces;
	RPointerArray <CLinkedTypefaceGroup> iGroups;
	HBufC* iLinkedTypefaceName;
	HBufC* iLinkedTypefaceFileName;
	};


#endif /*OPENFONTLINKEDTYPEFACESPECIFICATION_H_*/
