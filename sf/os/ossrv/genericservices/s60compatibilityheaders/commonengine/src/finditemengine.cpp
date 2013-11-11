// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDE FILES
#include "finditemengine.h"

EXPORT_C CFindItemEngine* CFindItemEngine::NewL( const TDesC& aText,  const CFindItemEngine::TFindItemSearchCase aSearchCase )
	{ return reinterpret_cast<CFindItemEngine*>(CTulAddressStringTokenizer::NewL(aText, aSearchCase)); }

EXPORT_C CFindItemEngine* CFindItemEngine::NewL( const TDesC& aText,  const CFindItemEngine::TFindItemSearchCase aSearchCase, const TInt aMinNumbers )
	{ return reinterpret_cast<CFindItemEngine*>(CTulAddressStringTokenizer::NewL(aText, aSearchCase, aMinNumbers)); }

EXPORT_C CFindItemEngine::~CFindItemEngine()
	{ delete reinterpret_cast<CTulAddressStringTokenizer*>(this); }

EXPORT_C TBool CFindItemEngine::Item( CFindItemEngine::SFoundItem& aItem )
	{ return reinterpret_cast<CTulAddressStringTokenizer*>(this)->Item(reinterpret_cast<CTulAddressStringTokenizer::SFoundItem&>(aItem)); }

EXPORT_C TBool CFindItemEngine::NextItem( CFindItemEngine::SFoundItem& aItem )
	{ return reinterpret_cast<CTulAddressStringTokenizer*>(this)->NextItem(reinterpret_cast<CTulAddressStringTokenizer::SFoundItem&>(aItem)); }

EXPORT_C TBool CFindItemEngine::PrevItem( CFindItemEngine::SFoundItem& aItem )
	{ return reinterpret_cast<CTulAddressStringTokenizer*>(this)->PrevItem(reinterpret_cast<CTulAddressStringTokenizer::SFoundItem&>(aItem)); }

EXPORT_C const CArrayFixFlat<CFindItemEngine::SFoundItem>* CFindItemEngine::ItemArray() const
	{ return reinterpret_cast<const CArrayFixFlat<CFindItemEngine::SFoundItem>*>(reinterpret_cast<const CTulAddressStringTokenizer*>(this)->ItemArray()); }

EXPORT_C TInt CFindItemEngine::Position() const
	{ return reinterpret_cast<const CTulAddressStringTokenizer*>(this)->Position(); }

EXPORT_C void CFindItemEngine::ResetPosition()
	{ reinterpret_cast<CTulAddressStringTokenizer*>(this)->ResetPosition(); }

EXPORT_C TInt CFindItemEngine::ItemCount() const
	{ return reinterpret_cast<const CTulAddressStringTokenizer*>(this)->ItemCount(); }

EXPORT_C TInt CFindItemEngine::DoNewSearchL( const TDesC& aText, CFindItemEngine::TFindItemSearchCase aSearchCase)
	{ return reinterpret_cast<CTulAddressStringTokenizer*>(this)->DoNewSearchL(aText, aSearchCase); }

EXPORT_C TInt CFindItemEngine::DoNewSearchL( const TDesC& aText, CFindItemEngine::TFindItemSearchCase aSearchCase,  TInt aMinNumbers )
	{ return reinterpret_cast<CTulAddressStringTokenizer*>(this)->DoNewSearchL(aText, aSearchCase, aMinNumbers ); }


//  End of File
