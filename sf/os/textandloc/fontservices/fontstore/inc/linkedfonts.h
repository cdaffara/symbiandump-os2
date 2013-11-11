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


#ifndef __LINKEDFONTS_H__
#define __LINKEDFONTS_H__

_LIT(KFNTSTORELinkedFontsPanicCategory,"LINKED_FONTS");

class CLinkedTypefaceBody;
class CLinkedTypefaceSpecificationBody;
class CLinkedTypefaceElementSpec;
class CLinkedTypefaceGroup;
class CFbsTypefaceStore;
class TLinkedTypefaceSpecificationArgs;

/**
This class contains the specification of a linked typeface. It is made up of groups and typefaces.

Groups (CLinkedTypefaceGroup) must contain one or more typeface; the attributes of the group control how the rasterizer
draws the typefaces within the group.

Typefaces (CLinkedTypefaceElementSpec) specify which individual typefaces make up the linked typeface. These are
referred to as "elements" of the linked typeface.

Once the linked typeface has been created users can use it as if it were any other typeface assuming there
is a rasteriser that supports this feature available. When selecting a character the rasterize searches
each typeface in turn from the lowest index to the highest. When drawing text, the first typeface to contain
the required character is used.

KMaxLinkedTypefaces and KMaxLinkedTypefaceGroups define the maximum number of typefaces and groups that can be
added to the specification.

This class is not thread safe.

WARNING: Class for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

@publishedAll
@released
@see CLinkedTypefaceElementSpec
@see CLinkedTypefaceGroup
@see KLinkedMaxTypefaces
@see KMaxLinkedTypefaceGroups
*/
NONSHARABLE_CLASS(CLinkedTypefaceSpecification) : public CBase
{
public:
	/**
	This function constructs the linked typeface specification and places it on the CleanupStack.
	
	The specification must contain at least one group and one typeface in order for registration to be successful.
	
	@param aName The name of the linked typeface. This name should not be the same as an existing typeface.
	
	@return A linked typeface specification object
	*/
	IMPORT_C static CLinkedTypefaceSpecification* NewLC(const TDesC& aTypefaceName);

	/**
	The name of this linked typeface
	@return The name of this linked typeface
	*/
	IMPORT_C TPtrC Name() const;

	/**
	The number of typeface that have been added to this object
	@return TInt The number of typeface that have been added to this object
	*/
	IMPORT_C TInt TypefaceCount() const;

	/**
	Returns the linked typeface at the specified index. 
	The first typeface is at index 0, the last at TypefaceCount()-1.
	@return The linked typeface.  Null is returned on error.
	@see CLinkedTypefaceSpecification::NumberOfTypefaces()
	*/
	IMPORT_C CLinkedTypefaceElementSpec* Typeface(TInt aIndex) const;

	/**
	Function to return the index of the canonical (metrics) typeface. The typeface can be
	retrieved by passing the index to Typeface().
	
	@return TInt The index to the canonical typeface
	@return KErrNotFound No canonical typeface exists
	@return KErrOverFlow There is more than one canonical typeface
	@see CLinkedTypefaceSpecification::Typeface()
	*/
	IMPORT_C TInt CanonicalIndex();
	
	/**
	Adds the supplied CLinkedTypefaceElementSpec to the Linked Typeface specification.
	The element is placed at the specified index. Adding a canonical element will only be successful
	if one has not already been added. The specification takes ownership of the element specification
	if adding is successful.
	
	@param aElementSpec The specification to be added.
	@param aIndex The position at which the element spec will be added.

	@see CLinkedTypefaceSpecification::AddTypefaceAtBackL()
	@see CLinkedTypefaceSpecification::RemoveTypeface()
	@see KMaxLinkedTypefaces
	
	@leave KErrArgument If the element is canonical and a canonical element already exists or if the group ID doesn't exist
	@leave KErrNoMemory There was not enough memory available to perform this operation
	@leave KErrOverflow The typeface specification is full; adding would exceed KMaxLinkedTypefaces
	*/
	IMPORT_C void AddTypefaceAtIndexL(CLinkedTypefaceElementSpec& aElementSpec, TInt aIndex);

	/**
	Adds the supplied CLinkedTypefaceElementSpec to the Linked Typeface specification.
	The element is placed at the back of the specification. Adding a canonical element will only be 
	successful if one has not already been added. The specification takes ownership of the element 
	specification if adding is successful.
	
	@param aElementSpec

	@see CLinkedTypefaceSpecification::AddTypefaceAtIndexL()
	@see CLinkedTypefaceSpecification::RemoveTypeface()
	@see KMaxLinkedTypefaces
	
	@leave KErrArgument If the element is canonical and a canonical element already exists or if the group ID doesn't exist
	@leave KErrNoMemory There was not enough memory available to perform this operation
	@leave KErrOverflow The typeface specification is full; adding would exceed KMaxLinkedTypefaces
	*/
	IMPORT_C void AddTypefaceAtBackL(CLinkedTypefaceElementSpec& aElementSpec);

	/**
	Removes the element spec at the specified index from this specification.
	This function deletes the element spec. If the element is the only one within its group
	the group is also removed and deleted from the specification.

	@param aIndex the index of the element spec to be removed

	@return TInt KErrNone if Removal was successful; KErrNotFound If the index is negative or larger than the number of typeface elements or the element doesn't exist
	*/
	IMPORT_C TInt RemoveTypeface(TInt aIndex);

	/**
	Creates the linked typeface so that it is available to use a normal typeface.

	@param aStore The Typeface Store the linked typeface should be registered with

	@panic EFbsPanicNoConnection There is no connection to the Font & Bitmap Server

	@leave KErrNotFound One or more of the typeface elements do not exist
	@leave KErrOverflow The linked typeface contains too many or 0 elements
	@leave KErrArgument The typeface has no or more than one canonical element
	@leave KErrAlreadyExists A typeface with the same name already exists
	@leave KErrNotSupported There is not a rasterizer available that supports linked typefaces
	*/
	IMPORT_C void RegisterLinkedTypefaceL(CFbsTypefaceStore& aStore);
	
	/**
	Retrieves the linked typeface specification from the server; the name of the typeface to
	retrieve should be specified at construction. All typefaces and groups contained in the
	specification are removed before retrieval of the new data. 
	
	@see CLinkedTypefaceSpecification::NewLC()
	
	@param aStore A pointer to the FontStore

	@leave KErrNotSupported There is not a rasterizer available that supports linked typefaces
	@leave KErrArgument The font specified does not exist or is not a linked typeface
	*/
	IMPORT_C void FetchLinkedTypefaceSpecificationL(CFbsTypefaceStore& aStore);
	
	/**
	 Adds the group specified to this specification.
	 The specification takes ownership of the group if addition is successful.
	 
	 @param aGroup The group to be added
	 
	 @see CLinkedTypefaceSpecification::RemoveLinkedTypefaceGroup()
	 
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
	IMPORT_C CLinkedTypefaceGroup* Group(TInt aGroupIndex) const;
	
	/**
	Returns the number of groups that this linked typeface specification contains.
	
	@see CLinkedTypefaceSpecification::Group()
	
	@return TInt Number of groups
	*/
	IMPORT_C TInt GroupCount() const;
	
	/**
	Default destructor
	*/
	IMPORT_C ~CLinkedTypefaceSpecification();
	
	/**
	 Removes and destroys all groups and elements from the specification.
	 
	 WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.
	 */
	IMPORT_C void Clear();
	
	/**
	 Update the specification of an existing linked typeface.
	 
	 @param aStore A typeface store
	 
	 @publishedPartner
	 */
	IMPORT_C void UpdateLinkedTypefaceL(CFbsTypefaceStore& aStore);
	
private:
	CLinkedTypefaceSpecification();
	void ConstructL(const TDesC& aTypefaceName);
	void DoAddTypefaceL(CLinkedTypefaceElementSpec& aElementSpec, TInt aPos);
	
private:
	CLinkedTypefaceSpecificationBody *iBody;
	};


/**
Class to specify an individual typeface within a linked typeface.

WARNING: Class for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

@see CLinkedTypefaceSpecification
@see CLinkedTypefaceGroup

@publishedAll
@released
*/
NONSHARABLE_CLASS(CLinkedTypefaceElementSpec) : public CBase
	{
public:
	/**
	Create a new linked typeface element specification. The name specified must be an existing typeface
	that should be added to the linked typeface. The group ID links this typeface to a linked typeface
	group.
	
	@see CLinkedTypefaceGroup
	
	@param aTypefaceName The typeface name for this element
	@param aGroupId The ID of the linked font group this element should belong to
	
	@return The newly created element spec
	*/
	IMPORT_C static CLinkedTypefaceElementSpec* NewLC(const TDesC& aTypefaceName, TInt aGroupId);

	/**
	Returns the typeface that this element represents within the linked typeface.
	
	@return	The element typeface name
	*/
	IMPORT_C TPtrC ElementName() const;
	
	/**
	Set whether the element is the canonical (metrics) font. 
	This is the font that provides all the metrics for the linked typeface.
	
	@param aIsCanonical ETrue if the element is canonical, EFalse otherwise
	*/
	IMPORT_C void SetCanonical(TBool aIsCanonical);
	/**
	@return	ETrue if the lement is canonical, EFalse otherwise
	*/
	IMPORT_C TBool Canonical() const;
	
	/**
	@return The group ID specified at construction.
	 */
	IMPORT_C TInt GroupId() const;
	
	/**
	Default Destructor
	*/
	IMPORT_C ~CLinkedTypefaceElementSpec();

private:
	/**
	Constructor
	*/
	CLinkedTypefaceElementSpec(TInt aGroupId);
	void ConstructL(const TDesC& aName);
	
private:
	HBufC*	iElementName;
	TBool	iIsCanonical;
	TInt	iGroupId;
	};

/**
This class contains parameters used to describe the groups that linked typeface elements are associated to.
These "metrics" apply to all fonts that are specified to be within the group.

WARNING: Class for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CLinkedTypefaceGroup) : public CBase
	{
public:
	/**
	Specifies the different ways in which a typeface belonging to this group can be scaled.
	 */
	enum TScalingOption
		{
		EScalingNone,
		EScalingUp,
		EScalingDown,
		EScalingUpOrDown,
		};
	/**
	Specifies whether the font baseline of fonts belonging to this group should be shifted or not.
	 */
	enum TBaselineShift
		{
		EBaselineShift,
		ENoBaselineShift,
		};
	/**
	Constructs a new linked typeface group with the specified ID and places it on the CleanupStack.
	
	By default the Antialiasing Threshold, Boldness Percentage and Italic Angle are set to -1.
	The rasterizer will apply its "best-guess" as to the value in this case.
	The default scaling option is EScalingNone; the default baseline shift is ENoBaselineShift.
	
	@param TInt The group ID of this group
	@return CLinkedTypefaceGroup* A newly constructed group
	*/
	IMPORT_C static CLinkedTypefaceGroup* NewLC(TInt aGroupId);
	
	/**
	The group ID specified at construction.
	
	@return TInt The group ID of this group
	 */
	IMPORT_C TInt GroupId() const;

	/**
	Sets the scaling option associated with the typefaces within this group.
	
	@param TScalingOption The enumeration value corresponding to the required scaling.
	
	@see CLinkedTypefaceGroup::TScalingOption
	*/
	IMPORT_C void SetScalingOption(TScalingOption aOption);
	
	/**
	The scaling option associated with the typefaces within this group.
	
	@see CLinkedTypefaceGroup::TScalingOption
	
	@return TScalingOption Scaling options
	 */
	IMPORT_C TScalingOption ScalingOption() const;
	
	/**
	Sets whether the baseline of typefaces within this group is shifted.
	
	@param TBaselineShift The enumeration value corresponding to the required shifting.
	
	@see CLinkedTypefaceGroup::TBaselineShift
	*/
	IMPORT_C void SetBaselineShift(TBaselineShift aShift);
	
	/**
	The baseline shift associated with the typefaces within this group.
	
	@return TBaselineShift The baseline shift
	*/
	IMPORT_C TBaselineShift BaselineShift() const;
	
	/**
	Sets the boldness percentage used by typefaces belonging to this group.
	
	The percentage is relative to the font size specified. For example if a
	font with an em height of 18 has a boldness percentage of 5% the resulting
	emboldened text	will be emboldened by 18 x (5 / 100) = 0.9 pixels.
	
	@param aPercentage The boldness percentage to be used by this group.
	 */
	IMPORT_C void SetBoldnessPercentage(TInt aPercentage);
	/**
	Returns the current boldness percentage for this typeface group.
	@return The current boldness percentage for this typeface group
	*/
	IMPORT_C TInt BoldnessPercentage() const;

	/**
	Sets the angle (slant) of italic text
	
	@param aAngle The angle (slant) of italic text
	 */
	IMPORT_C void SetItalicAngle(TInt aAngle);
	
	/**
	The angle (slant) of italic text
	
	@return The angle (slant) of italic text
	 */
	IMPORT_C TInt ItalicAngle() const;
	
	/**
	Default constructor
	*/
	~CLinkedTypefaceGroup();
	
private:
	CLinkedTypefaceGroup(TInt aGroupId);
	
private:
	TInt iGroupId;
	TScalingOption iScalingOption;
	TBaselineShift iBaselineShift;
	TInt iBoldnessPercentage;
	TInt iItalicAngle;
	};
#endif
