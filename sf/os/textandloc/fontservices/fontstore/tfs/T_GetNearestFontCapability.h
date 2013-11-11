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




#ifndef __T_GETNEARESTFONTCAPABILITY_H__
#define __T_GETNEARESTFONTCAPABILITY_H__

#include "TGraphicsHarness.h"

class CTGetNearestFontCapabilityStep : public CTGraphicsStep
	{
public:
	CTGetNearestFontCapabilityStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};
	

_LIT(KTGetNearestFontCapabilityStep, "TGetNearestFontCapability");

#endif
