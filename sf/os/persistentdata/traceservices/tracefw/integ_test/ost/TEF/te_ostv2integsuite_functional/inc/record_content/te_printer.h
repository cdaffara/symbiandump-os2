// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file te_printer.h
 @internalTechnology
*/
#ifndef TE_PRINTER_H
#define TE_PRINTER_H

class MPrinter
	{
public:
	virtual void Printf(const TDesC& aText)=0;
	virtual void Printf(const TDesC& aText, TInt aParam1)=0;
	virtual void Printf(const TDesC& aText, TInt aParam1, TInt aParam2)=0;
	virtual void Printf(const TDesC& aText, TInt aParam1, TInt aParam2, TInt aParam3)=0;
	};

#endif //TE_PRINTER_H

