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



TBool TTabStop::operator!=(const TTabStop& aTabStop)const
/** Compares two tab stops for inequality. They are different if the twips 
position or alignment is different.

@param aTabStop The tab stop to compare with the current tab stop 
@return ETrue if the objects' twips position or alignment differs. EFalse if 
equal. */
	{return !(*this==aTabStop);}


TBool TParaBorder::operator!=(const TParaBorder& aParaBorder)const
/**  Compares two paragraph border sides for inequality. Two paragraph border 
sides are different if any of their data members are different.

@param aBorder The paragraph border to compare with the current border.
@return ETrue if the two paragraph border sides are different, 
EFalse if not. */
	{return !(*this==aParaBorder);}

 
TBool TBullet::operator!=(const TBullet& aBullet)const
/** Compares two bullet points for inequality. Two bullet points are unequal if 
any of their data members are different.

@param aBullet The bullet point to compare. 
@return ETrue if the two bullet points are different, EFalse if not. */
	{return !(*this==aBullet);}


 
void CParaFormat::RemoveAllTabs()
/** Removes all tab stops from the object. */
	{
	if (iTabList)
		{
		delete iTabList;
		iTabList = NULL;
		}
	}

 
TInt CParaFormat::TabCount()const
/** Gets a count of the total number of tab stops in the object's tab list. If 
the object has no tab list, returns zero.

@return The number of tab stops. */
	{return (iTabList)?iTabList->Count():0;}


 

TParaFormatMask::TParaFormatMask()
	:iGuard(0)
/** The default C++ constructor constructs a paragraph format mask, initialising 
all bits to zero. */
	{}

 
void TParaFormatMask::SetAttrib(TTextFormatAttribute aAttribute)
/** Sets a single attribute flag in the paragraph format mask.

@param aAttribute The attribute flag to set. */
	{iGuard|=(1<<aAttribute);}

 

void TParaFormatMask::ClearAttrib(TTextFormatAttribute aAttribute)
/** Clears a single attribute flag in the paragraph format mask.

@param aAttribute The attribute flag to clear. */
	{iGuard&=~(1<<aAttribute);}

 

TBool TParaFormatMask::AttribIsSet(TTextFormatAttribute aAttribute)const
/** Tests whether an attribute flag is set.

@param aAttribute The paragraph format attribute to test for. 
@return ETrue if the flag is set, EFalse if not. */
	{return (iGuard&(1<<aAttribute));}

 

TBool TParaFormatMask::IsNull()const
/** Tests whether the mask is NULL, indicating that no flags have been set.

@return ETrue if mask is NULL, EFalse if not NULL. */
	{return !iGuard;}

 
TBool TParaFormatMask::operator!=(const TParaFormatMask& aMask)const
/** Compares two paragraph format masks for inequality.

@param aMask The mask to compare. 
@return ETrue if any flags are different. EFalse if all flags are the same. */
	{return !(*this==aMask);}


 

TCharFormatMask::TCharFormatMask()
	:iGuard(0)
/** The default C++ constructor constructs a character format mask, initialising 
all bits to zero. */
	{}

 

void TCharFormatMask::SetAttrib(TTextFormatAttribute aAttribute)
/** Sets a single attribute flag in the character format mask.

@param aAttribute The attribute flag to set. */
	{iGuard|=(1<<((aAttribute-KMaxParaAttributes)-1));}

 
void TCharFormatMask::ClearAttrib(TTextFormatAttribute aAttribute)
/** Clears a single attribute flag in the character format mask.

@param aAttribute The attribute flag to clear. */
	{iGuard&=~(1<<((aAttribute-KMaxParaAttributes)-1));}

 

TBool TCharFormatMask::AttribIsSet(TTextFormatAttribute aAttribute)const
/** Tests whether an attribute flag is set.

@param aAttribute The character format attribute to test for. 
@return ETrue if the flag is set, EFalse if not. */
	{return iGuard&(1<<((aAttribute-KMaxParaAttributes)-1));}

 
TBool TCharFormatMask::IsNull()const
/** Tests whether the mask is NULL, indicating that no flags have been set.

@return ETrue if mask is NULL, EFalse if not NULL. */
	{return !iGuard;}

 
TBool TCharFormatMask::operator!=(const TCharFormatMask& aMask)const
/** Compares two paragraph format masks for inequality.

@param aMask The mask to compare. 
@return ETrue if any flags are different. EFalse if all flags are the same. */
	{return !(*this==aMask);}


