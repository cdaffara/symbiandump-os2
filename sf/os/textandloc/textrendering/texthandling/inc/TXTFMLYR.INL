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





inline CParaFormatLayer* CParaFormatLayer::CloneL()const
/** Constructs and returns a new CParaFormatLayer with identical formatting to 
the current layer. Note that the function does not copy the original layer's 
based-on link - the new layer's based-on link is set to NULL, so that the 
two layers may not have the same effective formatting.

@return A new paragraph format layer cloned from the current layer. */
	{return STATIC_CAST(CParaFormatLayer*,DoCloneL());}




inline CCharFormatLayer* CCharFormatLayer::CloneL()const
/** Constructs and returns a new CCharFormatLayer with identical formatting to 
the current layer. Note that the function does not copy the original layer's 
based-on link - the new layer's based-on link is set to NULL, so that the 
two layers may not have the same effective formatting.

@return A new character format layer cloned from the current layer. */
	{return STATIC_CAST(CCharFormatLayer*,DoCloneL());}
