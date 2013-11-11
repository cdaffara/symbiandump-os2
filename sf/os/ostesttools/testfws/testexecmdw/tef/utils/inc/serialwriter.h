/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @Serial Writer header file
 @internalTechnology
*/

#ifndef SERIALWRITER_H
#define SERIALWRITER_H

#include <d32comm.h> // rbusdevcomm
#include <e32base.h>

NONSHARABLE_CLASS(CSerialWriter) 
	{
	public:
		static CSerialWriter* NewL();
		~CSerialWriter();
	public:
		TInt Settings(const TInt& aPortNumber);
		TInt Write(const TDesC8& aData);
		//Decoration methods 	
		TInt WriteDecorated(const TDesC& aText, TInt aSeverity = 0) ;
		TInt WriteDecorated(const TDesC8& aText, TInt aSeverity= 0)	; 
	private:
		void AddSeverity(TDes8& aLogBuffer, const TInt& aSeverity);
		void AddTime(TDes8& aLogBuffer) ; 
		CSerialWriter();
		void ConstructL();
		TInt Connect();
	    TInt Config();
		TInt Disconnect();
	private:
		RBusDevComm iComm;
	    TBool 		iReady;
		TInt 		iPortNum;
	};
	
	
#endif // SERIALWRITER_H
