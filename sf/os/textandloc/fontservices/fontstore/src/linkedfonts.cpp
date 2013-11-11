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


#include <s32file.h>
#include <graphics/shapeimpl.h>
#include "FNTSTORE.H"
#include "OPENFONT.H"
#include "FNTBODY.H"
#include "FNTSTD.H"
#include <graphics/shaperparams.h>
#include "linkedfonts.h"
#include "linkedfontsprivate.h"
#include "openfontsprivate.h"
#include "fbs.h"
#include <openfontlinkedtypefaceelementspec.h>
#include <graphics/openfontlinkedtypefacespecification.h>

EXPORT_C CLinkedTypefaceSpecification* CLinkedTypefaceSpecification::NewLC(const TDesC& aTypefaceName)
	{
	CLinkedTypefaceSpecification* self = new (ELeave) CLinkedTypefaceSpecification();
	CleanupStack::PushL(self);
	self->ConstructL(aTypefaceName);
	return self;
	}

CLinkedTypefaceSpecification::CLinkedTypefaceSpecification()
	{
	}

void CLinkedTypefaceSpecification::ConstructL(const TDesC &aTypefaceName)
	{
	iBody = new (ELeave) CLinkedTypefaceSpecificationBody();

	TInt length = aTypefaceName.Length();
	if ((length > KMaxTypefaceNameLength) || (length == 0))
		User::Leave(KErrArgument);

	iBody->iLinkedTypefaceName = aTypefaceName.AllocL();
	}

EXPORT_C void CLinkedTypefaceSpecification::AddTypefaceAtIndexL(CLinkedTypefaceElementSpec& aElementSpec, TInt aIndex)
	{
	DoAddTypefaceL(aElementSpec, aIndex);
	}

EXPORT_C void CLinkedTypefaceSpecification::AddTypefaceAtBackL(CLinkedTypefaceElementSpec& aElementSpec)
	{
	DoAddTypefaceL(aElementSpec, iBody->iTypefaces.Count());
	}

EXPORT_C TInt CLinkedTypefaceSpecification::RemoveTypeface(TInt aIndex)
	{
	if  ( (aIndex < iBody->iTypefaces.Count()) && (aIndex >= 0) )
		{
		CLinkedTypefaceElementSpec* elementSpec = iBody->iTypefaces[aIndex];
		iBody->iTypefaces.Remove(aIndex);
		delete elementSpec;
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

EXPORT_C void CLinkedTypefaceSpecification::RegisterLinkedTypefaceL(CFbsTypefaceStore& aStore)
	{
	//Leave if there is no canonical element, or there are multiples.
	//The remaining validation must be performed in fntstore.
	User::LeaveIfError(CanonicalIndex());
	User::LeaveIfError(aStore.RegisterLinkedTypeface(*this));
	}

EXPORT_C void CLinkedTypefaceSpecification::FetchLinkedTypefaceSpecificationL(CFbsTypefaceStore& aStore)
	{	
	aStore.GetLinkedTypefaceL(*this);
	}

EXPORT_C TInt CLinkedTypefaceSpecification::TypefaceCount() const
	{
	return iBody->iTypefaces.Count();
	}

EXPORT_C TPtrC CLinkedTypefaceSpecification::Name() const
	{
	return *iBody->iLinkedTypefaceName;
	}

EXPORT_C CLinkedTypefaceSpecification::~CLinkedTypefaceSpecification()
	{
	delete iBody;
	}

EXPORT_C TInt CLinkedTypefaceSpecification::CanonicalIndex()
	{
	TInt index = KErrNotFound;
	for (TInt i = iBody->iTypefaces.Count()-1 ; i >= 0; i--)
		{
		if (iBody->iTypefaces[i]->Canonical())
			{
			if (index == KErrNotFound)
				index = i;
			else 
				return KErrOverflow;
			}
		}
	return index;
	}

EXPORT_C CLinkedTypefaceElementSpec* CLinkedTypefaceSpecification::Typeface(TInt aIndex) const
	{
	if ((aIndex >= iBody->iTypefaces.Count()) || (aIndex < 0))
		{
		return NULL;
		}
	else
		{
		return iBody->iTypefaces[aIndex];
		}
	}

void CLinkedTypefaceSpecification::DoAddTypefaceL(CLinkedTypefaceElementSpec& aElementSpec, TInt aPos)
	{
	if (iBody->iTypefaces.Count() == KMaxLinkedTypefaces)
		User::Leave(KErrOverflow);
	
	TInt err = iBody->iTypefaces.Find(&aElementSpec);

	if (err != KErrNotFound)
		User::Leave(KErrAlreadyExists);
	
	if (aElementSpec.Canonical() && CanonicalIndex() != KErrNotFound)
		User::Leave(KErrArgument);

	if (GroupById(aElementSpec.GroupId()) == NULL)
		User::Leave(KErrArgument);
	
	err = iBody->iTypefaces.Insert(&aElementSpec, aPos);

	User::LeaveIfError(err);
	}

EXPORT_C void CLinkedTypefaceSpecification::AddLinkedTypefaceGroupL(CLinkedTypefaceGroup& aGroup)
	{
	if (iBody->iGroups.Count() == KMaxLinkedTypefaceGroups)
		User::Leave(KErrOverflow);
	
	for (TInt i = iBody->iGroups.Count()-1 ; i >= 0 ; i--)
		{
		if (iBody->iGroups[i]->GroupId() == aGroup.GroupId())
			User::Leave(KErrAlreadyExists);
		}
	iBody->iGroups.AppendL(&aGroup);
	}

EXPORT_C TInt CLinkedTypefaceSpecification::RemoveLinkedTypefaceGroup(TInt aGroupId)
	{
	//Search for & remove the group if it exists
	CLinkedTypefaceGroup* remove = GroupById(aGroupId);
	//If the group has been found & removed remove all associated typefaces
	if (remove)
		{
		CLinkedTypefaceElementSpec* elementSpec;
		for (TInt i = iBody->iTypefaces.Count()-1 ; i >= 0 ; i--)
			{
			elementSpec = iBody->iTypefaces[i];
			if (elementSpec->GroupId() == aGroupId)
				RemoveTypeface(i);
			}
		iBody->iGroups.Remove(iBody->iGroups.Find(remove));
		delete remove;
		return KErrNone;
		}
	return KErrNotFound;
	}

EXPORT_C CLinkedTypefaceGroup* CLinkedTypefaceSpecification::GroupById(TInt aGroupId) const
	{
	TInt index;
	TBool found = EFalse;
	for (index = iBody->iGroups.Count()-1; index != -1 ; index--)
		{
		if (iBody->iGroups[index]->GroupId() == aGroupId)
			{
			found = ETrue;
			break;
			}
		}
	
	if (found)
		return iBody->iGroups[index];
	else
		return NULL;
	}

EXPORT_C CLinkedTypefaceGroup* CLinkedTypefaceSpecification::Group(TInt aGroupIndex) const
	{
	if ( (aGroupIndex >= 0) && (aGroupIndex < iBody->iGroups.Count()) )
		return iBody->iGroups[aGroupIndex];
	else
		return NULL;
	}

EXPORT_C TInt CLinkedTypefaceSpecification::GroupCount() const
	{
	return iBody->iGroups.Count();
	}

EXPORT_C void CLinkedTypefaceSpecification::Clear()
	{
	iBody->iGroups.ResetAndDestroy();
	iBody->iTypefaces.ResetAndDestroy();
	}

EXPORT_C void CLinkedTypefaceSpecification::UpdateLinkedTypefaceL(CFbsTypefaceStore& aStore)
	{
	User::LeaveIfError(aStore.UpdateLinkedTypeface(*this));
	}

CLinkedTypefaceSpecificationBody::CLinkedTypefaceSpecificationBody()
	{
	}

CLinkedTypefaceSpecificationBody::~CLinkedTypefaceSpecificationBody()
	{
	delete iLinkedTypefaceName;
	iTypefaces.ResetAndDestroy();
	iGroups.ResetAndDestroy();
	}

EXPORT_C CLinkedTypefaceElementSpec* CLinkedTypefaceElementSpec::NewLC(const TDesC& aElementName, TInt aGroupId)
	{
	CLinkedTypefaceElementSpec* self = new(ELeave)CLinkedTypefaceElementSpec(aGroupId);
	CleanupStack::PushL(self);
	self->ConstructL(aElementName);
	return self;
	}

EXPORT_C CLinkedTypefaceElementSpec::~CLinkedTypefaceElementSpec()
	{
	delete iElementName;
	}

EXPORT_C TPtrC CLinkedTypefaceElementSpec::ElementName() const
	{
	return *iElementName;
	}

EXPORT_C void CLinkedTypefaceElementSpec::SetCanonical(TBool aIsCanonical)
	{
	iIsCanonical = aIsCanonical;
	}

EXPORT_C TBool CLinkedTypefaceElementSpec::Canonical() const
	{
	return iIsCanonical;
	}

CLinkedTypefaceElementSpec::CLinkedTypefaceElementSpec(TInt aGroupId)
	{
	iGroupId = aGroupId;
	iIsCanonical = EFalse;
	}

EXPORT_C TInt CLinkedTypefaceElementSpec::GroupId() const
	{
	return iGroupId;
	}

EXPORT_C COpenFontLinkedTypefaceSpecification* COpenFontLinkedTypefaceSpecification::NewLC(const TDesC& aTypefaceName)
	{
	COpenFontLinkedTypefaceSpecification* self = new (ELeave) COpenFontLinkedTypefaceSpecification();
	CleanupStack::PushL(self);
	self->ConstructL(aTypefaceName);
	return self;
	}

EXPORT_C COpenFontLinkedTypefaceSpecification* COpenFontLinkedTypefaceSpecification::NewLC(const TLinkedTypefaceSpecificationArgs& aSpecArgs)
	{
	COpenFontLinkedTypefaceSpecification* self = new (ELeave) COpenFontLinkedTypefaceSpecification();
	CleanupStack::PushL(self);
	self->ConstructL(aSpecArgs);
	return self;
	}

COpenFontLinkedTypefaceSpecification::COpenFontLinkedTypefaceSpecification()
	{
	}

void COpenFontLinkedTypefaceSpecification::ConstructL(const TDesC &aTypefaceName)
	{
	TInt length = aTypefaceName.Length();
	if ((length > KMaxTypefaceNameLength) || (length == 0))
		User::Leave(KErrArgument);

	iLinkedTypefaceName = aTypefaceName.AllocL();
	}

void COpenFontLinkedTypefaceSpecification::ConstructL(const TLinkedTypefaceSpecificationArgs& aSpecArgs)
	{
	iLinkedTypefaceName = aSpecArgs.iName.AllocL();

	TInt i;
	
	for (i = 0; i != aSpecArgs.iGroupSize ; i++)
		{
		CLinkedTypefaceGroup* grp = CLinkedTypefaceGroup::NewLC(aSpecArgs.iGroups[i].iGroupId);
		grp->SetBaselineShift(aSpecArgs.iGroups[i].iBaselineShift);
		grp->SetBoldnessPercentage(aSpecArgs.iGroups[i].iBoldnessPercentage);
		grp->SetItalicAngle(aSpecArgs.iGroups[i].iItalicAngle);
		grp->SetScalingOption(aSpecArgs.iGroups[i].iScalingOption);
		AddLinkedTypefaceGroupL(*grp);
		CleanupStack::Pop(grp);
		}
	
	for (i = 0 ; i != aSpecArgs.iSize ; i++)
		{
		COpenFontLinkedTypefaceElementSpec* es = COpenFontLinkedTypefaceElementSpec::NewLC(aSpecArgs.iTypefaces[i].iName, aSpecArgs.iTypefaces[i].iGroupId);
		es->SetCanonical(aSpecArgs.iTypefaces[i].iIsCanonical);
		AddTypefaceAtBackL(*es);
		CleanupStack::Pop(es);
		}
	
	//ASSERT Can. Indexs match.
	}

EXPORT_C COpenFontLinkedTypefaceSpecification::~COpenFontLinkedTypefaceSpecification()
	{
	delete iLinkedTypefaceName;
	iTypefaces.ResetAndDestroy();
	iTypefaces.Close();
	iGroups.ResetAndDestroy();
	iGroups.Close();
	}

EXPORT_C void COpenFontLinkedTypefaceSpecification::AddTypefaceAtIndexL(COpenFontLinkedTypefaceElementSpec& aElementSpec, TInt aIndex)
	{
	DoAddTypefaceL(aElementSpec, aIndex);
	}

EXPORT_C void COpenFontLinkedTypefaceSpecification::AddTypefaceAtBackL(COpenFontLinkedTypefaceElementSpec& aElementSpec)
	{
	DoAddTypefaceL(aElementSpec, iTypefaces.Count());
	}

EXPORT_C TInt COpenFontLinkedTypefaceSpecification::RemoveTypeface(TInt aIndex)
	{
	if  ( (aIndex < iTypefaces.Count()) && (aIndex >= 0) )
		{
		COpenFontLinkedTypefaceElementSpec* elementSpec = iTypefaces[aIndex];
		iTypefaces.Remove(aIndex);

		delete elementSpec;
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

EXPORT_C TInt COpenFontLinkedTypefaceSpecification::TypefaceCount() const
	{
	return iTypefaces.Count();
	}

EXPORT_C TPtrC COpenFontLinkedTypefaceSpecification::Name() const
	{
	return *iLinkedTypefaceName;
	}

EXPORT_C TInt COpenFontLinkedTypefaceSpecification::CanonicalIndex() const
	{
	TInt index = KErrNotFound;
	for (TInt i = iTypefaces.Count()-1 ; i >= 0; i--)
		{
		if (iTypefaces[i]->Canonical())
			{
			if (index == KErrNotFound)
				index = i;
			else 
				return KErrOverflow;
			}
		}
	return index;
	}

EXPORT_C COpenFontLinkedTypefaceElementSpec* COpenFontLinkedTypefaceSpecification::Typeface(TInt aIndex) const
	{
	if ((aIndex >= iTypefaces.Count()) || (aIndex < 0))
		{
		return NULL;
		}
	else
		{
		return iTypefaces[aIndex];
		}
	}

void COpenFontLinkedTypefaceSpecification::DoAddTypefaceL(COpenFontLinkedTypefaceElementSpec& aElementSpec, TInt aPos)
	{
	if (iTypefaces.Count() == KMaxLinkedTypefaces)
		User::Leave(KErrOverflow);
	
	TInt err = iTypefaces.Find(&aElementSpec);

	if (err != KErrNotFound)
		User::Leave(KErrAlreadyExists);
	
	if (aElementSpec.Canonical() && CanonicalIndex() >= 0)
		User::Leave(KErrArgument);
	
	if (GroupById(aElementSpec.GroupId()) == NULL)
		User::Leave(KErrArgument);
	
	err = iTypefaces.Insert(&aElementSpec, aPos);

	User::LeaveIfError(err);
	}

EXPORT_C void COpenFontLinkedTypefaceSpecification::AddLinkedTypefaceGroupL(CLinkedTypefaceGroup& aGroup)
	{
	if (iGroups.Count() == KMaxLinkedTypefaceGroups)
		User::Leave(KErrOverflow);
	
	for (TInt i = iGroups.Count()-1 ; i >= 0 ; i--)
		{
		if (iGroups[i]->GroupId() == aGroup.GroupId())
			User::Leave(KErrAlreadyExists);
		}
	iGroups.AppendL(&aGroup);
	}

EXPORT_C TInt COpenFontLinkedTypefaceSpecification::RemoveLinkedTypefaceGroup(TInt aGroupId)
	{
	//Search for & remove the group if it exists
	CLinkedTypefaceGroup* remove = GroupById(aGroupId);
	//If the group has been found & removed remove all associated typefaces
	if (remove)
		{
		COpenFontLinkedTypefaceElementSpec* elementSpec;
		for (TInt i = iTypefaces.Count()-1 ; i >= 0 ; i--)
			{
			elementSpec = iTypefaces[i];
			if (elementSpec->GroupId() == aGroupId)
				RemoveTypeface(i);
			}
		iGroups.Remove(iGroups.Find(remove));
		delete remove;
		return KErrNone;
		}
	return KErrNotFound;
	}

EXPORT_C CLinkedTypefaceGroup* COpenFontLinkedTypefaceSpecification::GroupById(TInt aGroupId) const
	{
	TInt index;
	TBool found = EFalse;
	for (index = iGroups.Count()-1; index != -1 ; index--)
		{
		if (iGroups[index]->GroupId() == aGroupId)
			{
			found = ETrue;
			break;
			}
		}
	
	if (found)
		return iGroups[index];
	else
		return NULL;
	}

EXPORT_C const CLinkedTypefaceGroup* COpenFontLinkedTypefaceSpecification::Group(TInt aGroupIndex) const
	{
	if ( (aGroupIndex >= 0) && (aGroupIndex < iGroups.Count()) )
		return iGroups[aGroupIndex];
	else
		return NULL;
	}

EXPORT_C TInt COpenFontLinkedTypefaceSpecification::GroupCount() const
	{
	return iGroups.Count();
	}

COpenFontLinkedTypefaceSpecificationBody::COpenFontLinkedTypefaceSpecificationBody()
	{
	}

// destructor
COpenFontLinkedTypefaceSpecificationBody::~COpenFontLinkedTypefaceSpecificationBody()
	{
	delete iLinkedTypefaceName;
	iTypefaces.Reset();
	}

EXPORT_C COpenFontLinkedTypefaceElementSpec* COpenFontLinkedTypefaceElementSpec::NewLC(const TDesC& aElementName, TInt aGroupId)
	{
	COpenFontLinkedTypefaceElementSpec* self = new(ELeave)COpenFontLinkedTypefaceElementSpec(aGroupId);
	CleanupStack::PushL(self);
	self->ConstructL(aElementName);
	return self;
	}

EXPORT_C COpenFontLinkedTypefaceElementSpec::~COpenFontLinkedTypefaceElementSpec()
	{
	delete iElementName;
	delete iFileName;
	}

EXPORT_C TPtrC COpenFontLinkedTypefaceElementSpec::ElementName() const
	{
	return *iElementName;
	}

EXPORT_C void COpenFontLinkedTypefaceElementSpec::SetCanonical(TBool aIsCanonical)
	{
	iIsCanonical = aIsCanonical;
	}

EXPORT_C TBool COpenFontLinkedTypefaceElementSpec::Canonical() const
	{
	return iIsCanonical;
	}

EXPORT_C TInt COpenFontLinkedTypefaceElementSpec::GroupId() const
	{
	return iGroupId;
	}

EXPORT_C TPtrC COpenFontLinkedTypefaceElementSpec::FileName() const
	{
	return *iFileName;
	}

EXPORT_C void COpenFontLinkedTypefaceElementSpec::SetFileNameL(const TDesC& aFileName)
	{
	iFileName = aFileName.AllocL();
	}

COpenFontLinkedTypefaceElementSpec::COpenFontLinkedTypefaceElementSpec(TInt aGroupId)
	{
	iGroupId = aGroupId;
	}

void COpenFontLinkedTypefaceElementSpec::ConstructL(const TDesC& aName)
	{
	TInt length = aName.Length();
	if ((length == 0) || (length > KMaxTypefaceNameLength))
		User::Leave(KErrArgument);
	iElementName = aName.AllocL();
	}

void CLinkedTypefaceElementSpec::ConstructL(const TDesC& aName)
	{
	TInt length = aName.Length();
	if ((length == 0) || (length > KMaxTypefaceNameLength))
		User::Leave(KErrArgument);
	iElementName = aName.AllocL();
	}

EXPORT_C CLinkedTypefaceGroup* CLinkedTypefaceGroup::NewLC(TInt aGroupId)
	{
	CLinkedTypefaceGroup* self = new (ELeave) CLinkedTypefaceGroup(aGroupId);
	CleanupStack::PushL(self);
	return self;
	}

CLinkedTypefaceGroup::CLinkedTypefaceGroup(TInt aGroupId)
	{
	iGroupId = aGroupId;
	iScalingOption = EScalingNone;
	iBaselineShift = ENoBaselineShift;
	iBoldnessPercentage = -1;
	iItalicAngle = -1;
	}

CLinkedTypefaceGroup::~CLinkedTypefaceGroup()
	{
	}

EXPORT_C void CLinkedTypefaceGroup::SetScalingOption(CLinkedTypefaceGroup::TScalingOption aOption)
	{
	iScalingOption = aOption;
	}

EXPORT_C CLinkedTypefaceGroup::TScalingOption CLinkedTypefaceGroup::ScalingOption() const
	{
	return iScalingOption;
	}

EXPORT_C void CLinkedTypefaceGroup::SetBaselineShift(CLinkedTypefaceGroup::TBaselineShift aShift)
	{
	iBaselineShift = aShift;
	}

EXPORT_C CLinkedTypefaceGroup::TBaselineShift CLinkedTypefaceGroup::BaselineShift() const
	{
	return iBaselineShift;
	}

EXPORT_C void CLinkedTypefaceGroup::SetBoldnessPercentage(TInt aPercentage)
	{
	iBoldnessPercentage = aPercentage;
	}

EXPORT_C TInt CLinkedTypefaceGroup::BoldnessPercentage() const
	{
	return iBoldnessPercentage;
	}

EXPORT_C TInt CLinkedTypefaceGroup::GroupId() const
	{
	return iGroupId;
	}

EXPORT_C TInt CLinkedTypefaceGroup::ItalicAngle() const
	{
	return iItalicAngle;
	}

EXPORT_C void CLinkedTypefaceGroup::SetItalicAngle(TInt aAngle)
	{
	iItalicAngle = aAngle;
	}

EXPORT_C void TLinkedTypefaceSpecificationArgs::operator =(const CLinkedTypefaceSpecification& aRhs)
	{
	iSize = aRhs.TypefaceCount();
	iGroupSize = aRhs.GroupCount();
	iName = aRhs.Name();
	
	TInt i;
	for (i = 0 ; i != iSize ; i++)
		iTypefaces[i] = aRhs.Typeface(i);
	
	for (i = 0 ; i != iGroupSize ; i++)
		iGroups[i] = aRhs.Group(i);
	}

EXPORT_C void TLinkedTypefaceSpecificationArgs::operator =(const COpenFontLinkedTypefaceSpecification &aRhs)
	{
	iSize = aRhs.TypefaceCount();
	iGroupSize = aRhs.GroupCount();
	iName = aRhs.Name();
	
	TInt i;
	for (i = 0 ; i != iSize ; i++)
		iTypefaces[i] = aRhs.Typeface(i);
	
	for (i = 0 ; i != iGroupSize ; i++)
		iGroups[i] = aRhs.Group(i);
	}

void TLinkedTypefaceElementSpecArgs::operator = (const CLinkedTypefaceElementSpec* aRhs)
	{
	iIsCanonical = aRhs->Canonical();
	iGroupId = aRhs->GroupId();
	iName = aRhs->ElementName();
	}

void TLinkedTypefaceElementSpecArgs::operator = (const COpenFontLinkedTypefaceElementSpec* aRhs)
	{
	iIsCanonical = aRhs->Canonical();
	iGroupId = aRhs->GroupId();
	iName = aRhs->ElementName();
	}

void TLinkedTypefaceGroupArgs::operator =(const CLinkedTypefaceGroup* aRhs)
	{
	iGroupId = aRhs->GroupId();
	iScalingOption = aRhs->ScalingOption();
	iBaselineShift = aRhs->BaselineShift();
	iBoldnessPercentage = aRhs->BoldnessPercentage();
	iItalicAngle = aRhs->ItalicAngle();
	}
