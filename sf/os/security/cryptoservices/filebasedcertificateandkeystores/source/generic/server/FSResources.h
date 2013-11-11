/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* FSResources class that contains resources (locale-specific strings
* and so on) needed by the file token server
*
*/


/**
 @file 
 @internalTechnology
*/

#include <barsc.h>
#include <bautils.h>
#include <barsread.h>
#include <ct/rcpointerarray.h>

class FSResources
	{
 public:
	/// If the resources aren't initialised, initialise them
	static void InitialiseL();
	/// Free all resources.
	static void Cleanup();

	/// Returns an array of strings. The strings are identified by
	/// enums in TStrings
	inline static const CDesCArray& Strings();

	/// The meaning of each of the strings returned by Strings
	enum TStrings
		{
		/// The label associated with the passphrase for decrypting
		/// imported keys
		EImportPassphraseLabel = 0,
		EExportPassphraseLabel =1,
		EPinGLabel = 2,
		ECreatePinGLabel = 3
		};

 private:
	static CDesCArray* iStrings;
	};

inline const CDesCArray& FSResources::Strings()
	{
	return *iStrings;
	}
