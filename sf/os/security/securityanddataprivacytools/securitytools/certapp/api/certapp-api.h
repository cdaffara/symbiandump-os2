#ifndef __CERTAPP_API_H__
#define __CERTAPP_API_H__/*
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
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifdef  __cplusplus
extern "C" {
#endif
/**
   RunCertApp

This API will run certapp as separate process and returns the 
result. This approach is taken so that the caller is protected from 
any possible certapp failure. Even if certapp crashes, RunCertApp will 
return without crashing the caller. 
 
Certapp guarantees to return a non-zero status if it fails, which 
means you don't need to parse its output to deduce if it worked or 
not. 
 
The certapp executable must be in the current search path. 
 
The caller will be blocked whilst certapp runs. 
 
There are two logging arguments, aProgress and aErrors. These may be 
set to either a filename to write the output to or to "-" which means 
standard output. If both logging arguments are identical, then the 
output will be merged to a single location. 
 
Progress and warning messages are written to aProgress. 
 
Fatal errors will be logged to aErrors, and RunCertApp will return a
non-zero number. An error code of -1 means the certapp executable
failed or was not found, other error codes indicate problems launching
the executable.
 
The argc argument should be set to the count of the argv arguments. 
 
The argv argument should be set to an array of const char * ptrs to 
the remaining arguments. 
 
The argv arguments maybe any of the arguments accepted by the certapp 
executable, run "certapp -h" for further information. 
*/
int RunCertApp(const char *aProgress, const char *aErrors,
			   int argc, char **argv);

#ifdef  __cplusplus
}
#endif

#endif
