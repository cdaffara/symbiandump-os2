/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* filetokenspatchdata.h
*
*/


#include <e32base.h>

/**
This 8-bit variable is used in enablement of aggregation of multiple certstores
present under filecertstore ROM drive private directory. It is used to override swicertstores's and filecertstore's
default behaviour of only loading and reading from permanent certstore files 'swicertstore.dat and 
'cacerts.dat' & 'certclients.dat' respectively. Compile time value is 0 - default behaviour. 
When value is 1 - SWI certstore will aggregate all the certstore files present under 
ROM drive resource directory and gives an abstraction of a single ROM swicertstore. Also this
will aggregate the highest version writable swicertstore under system drive resource directory
as of higher priority to with the ROM composite swicertstore. Similarly, filecertstore will aggregate
the multiple certstore files present under its ROM drive private directory and gives an abstraction of a single filecertstore.
In emulator platform we have the default behaviour for both swicertstore and filecertstore. 

This variable is changed at ROM build time using the patchdata obey keyword.
See Symbian OS Developer Library documentation on how to patch DLL data exports.
@file
@publishedPartner
@released
*/

IMPORT_C extern const TUint8 KAggregateCertStore;
