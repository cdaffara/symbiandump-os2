/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/** ----------------------------------------------------------------------- **
    @package:     NW_WBXML

    @synopsis:    default

    @description: Wbxml token definitions

 ** ----------------------------------------------------------------------- **/

#ifndef NW_PARSER_WBXML_OPAQUE_H
#define NW_PARSER_WBXML_OPAQUE_H

#include <xml/cxml/cxml_proj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_Opaque

    @synopsis:    WBXML opaque data.

    @scope:       public
    @variables:
       NW_Uint32 length
                  Data length.

       NW_Byte* data
                  Data.

    @description: WBXML opaque data.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_Opaque_s {
  NW_Uint32 length;
  NW_Byte* data;
} NW_WBXML_Opaque_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Opaque_new

    @synopsis:    Constructor.

    @scope:       public

    @description: Constructor.

    @returns:     NW_WBXML_Opaque_t*
                  Pointer to empty structure.

 ** ----------------------------------------------------------------------- **/
NW_WBXML_Opaque_t* NW_WBXML_Opaque_new (void);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Opaque_delete

    @synopsis:    Destructor.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Opaque_t* opaque
                  The opaque data.

    @description: Destructor. Deletes the opaque structure. The referenced
                  memory is not deleted.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t NW_WBXML_Opaque_delete (NW_WBXML_Opaque_t* opaque);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Opaque_construct

    @synopsis:    Constructor.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Opaque_t* opaque
                  Structure created with the "New".

       [in] NW_Byte* data
                  Opaque data.

       [in] NW_Uint32 length
                  Length of data.

    @description: Constructor. Initializes a structure created with the "New".
                  This is really an initializer, not a constructor in the C++ 
                  or Java sense.

    @returns:     NW_Status_t
                  default

 ** ----------------------------------------------------------------------- **/
NW_Status_t NW_WBXML_Opaque_construct (NW_WBXML_Opaque_t* opaque, 
                                       NW_Byte* data, 
                                       NW_Uint32 length);
#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_PARSER_WBXML_OPAQUE_H */
