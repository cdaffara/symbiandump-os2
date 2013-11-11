// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef OMXILCOMMAND_H
#define OMXILCOMMAND_H


#include <openmax/il/khronos/v1_x/OMX_Core.h>

class TOmxILCommand
	{

public:

	OMX_COMMANDTYPE iCommandType;
	OMX_U32 iParam1;
	OMX_PTR ipCommandData;

	// Constructor
	inline TOmxILCommand(
		OMX_COMMANDTYPE aCommandType,
		OMX_U32 aParam1,
		OMX_PTR apCommandData);

	};


inline
TOmxILCommand::TOmxILCommand(
	OMX_COMMANDTYPE aCommandType,
	OMX_U32 aParam1,
	OMX_PTR apCommandData
	)
	:
	iCommandType(aCommandType),
	iParam1(aParam1),
	ipCommandData(apCommandData)
	{
	}

#endif // OMXILCOMMAND_H
