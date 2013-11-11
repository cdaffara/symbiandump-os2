/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides a single header for the cXML Parser
*
*/



#ifndef _CXML_FILE_IF_H_
#define _CXML_FILE_IF_H_

#include <xml/cxml/cxml_proj.h>

#ifdef __cplusplus
extern "C" {
#endif

  /** ----------------------------------------------------------------------- **
    @function:     CXML_File_Read
    @synopsis:    Read the contents from the file and store in a buffer.
    @scope:       public      

    @parameters: 
        [in] fileName
                  

        [out] CXML_Uint8* buf
                  Buffer to store the file contents and buffer need to freed from the
                  calling function.
				  (-1): In case of error.


        [out] CXML_Uint32* bufferLen
                       Length of the Buffer        

    @description: Read contents from the file and store these in a buffer to be 
                         read by the cXML parser.

    @returns:   
                  In case of file  related failures:  Error codes is returned defined in 
				                                      nwx_status.h
                  NW_STAT_FAILURE : For any general error.
                  NW_STAT_SUCCESS: For success
 -----------------------------------------------------------------------------------**/

IMPORT_C
NW_Status_t  CXML_Read_From_File(NW_Byte* fileName,NW_Uint8** buf, NW_Int32* bufferLen);

#ifdef __cplusplus 
} /* extern "C" { */
#endif /* __cplusplus */

#endif //_CXML_FILE_IF_H_
