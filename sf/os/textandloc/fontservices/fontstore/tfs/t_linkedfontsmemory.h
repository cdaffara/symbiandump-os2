/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef T_LINKEDFONTSMEMORY_H_
#define T_LINKEDFONTSMEMORY_H_

#include "TGraphicsHarness.h"

class CTLinkedFontsMemoryStep : public CTGraphicsStep
	{
public:
	CTLinkedFontsMemoryStep();
protected:
	// From CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTLinkedFontsMemoryStep, "TLinkedFontsMemory");

#endif /*T_LINKEDFONTSMEMORY_H_*/
