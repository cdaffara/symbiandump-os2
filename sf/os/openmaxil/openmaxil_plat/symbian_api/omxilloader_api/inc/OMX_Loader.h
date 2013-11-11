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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef OMX_LOADER_H
#define OMX_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <openmax/il/khronos/v1_x/OMX_Core.h>

struct OMX_LOADERTYPE
	{
	/** The InitializeComponentLoader method is used to initialize the loader.
	It shall be the first call made into the loader and it should only be executed once
	until UnInitializeComponentLoader call is made.

    @return OMX_ERRORTYPE
        OMX_ErrorNone if the function is executed successfully,
        OMX_ErrorInsufficientResources if the component loader fails to construct due to an
        out of memory issue, OMX_ErrorUndefined or any other appropriate OMX error otherwise.
	*/
	OMX_ERRORTYPE (*InitializeComponentLoader)(OMX_LOADERTYPE *loader);

	/** The UnInitializeComponentLoader method is used to uninitialize the loader.
	It shall be the last call made into the loader and should be called after all
	OpenMAX-related resources have been released.

    @return OMX_ERRORTYPE
        It returns OMX_ErrorNone if the function is executed successfully
        or any other appropriate OMX error otherwise.
	*/
	OMX_ERRORTYPE (*UnInitializeComponentLoader)(OMX_LOADERTYPE *loader);

	/** The LoadComponent method will locate the component specified by the
    component name given, load that component into memory and then invoke
    the component's methods to create an instance of the component.

    @param [out] pHandle
        pointer to an OMX_HANDLETYPE pointer to be filled in by this method.
    @param [in] cComponentName
        pointer to a null terminated string with the component name.  The
        names of the components are strings less than 127 bytes in length
        plus the trailing null for a maximum size of 128 bytes.  An example
        of a valid component name is "OMX.TI.AUDIO.DSP.MIXER\0".  Names are
        assigned by the vendor, but shall start with "OMX." and then have
        the Vendor designation next.
    @param [in] pAppData
        pointer to an application defined value that will be returned
        during callbacks so that the application can identify the source
        of the callback.
    @param [in] pCallBacks
        pointer to a OMX_CALLBACKTYPE structure that will be passed to the
        component to initialize it with.
    @return OMX_ERRORTYPE
        It returns OMX_ErrorNone if the function is executed successfully
        or any other appropriate OMX error otherwise.
	*/
	OMX_ERRORTYPE (*LoadComponent)(	OMX_LOADERTYPE *loader,
									OMX_OUT OMX_HANDLETYPE* pHandle,
									OMX_IN  OMX_STRING cComponentName,
									OMX_IN  OMX_PTR pAppData,
									OMX_IN  OMX_CALLBACKTYPE* pCallBacks);


	/** The UnloadComponent method will free a handle allocated by the LoadComponent
    method.

    @param [in] hComponent
        Handle of the component to be accessed.  This is the component
        handle returned by the call to the GetHandle function.
    @return OMX_ERRORTYPE
        It returns OMX_ErrorNone if the function is executed successfully
        OMX_ErrorComponentNotFound if the component is not found in this loader;
        or any other appropriate OMX error otherwise.
	*/
	OMX_ERRORTYPE (*UnloadComponent)( OMX_LOADERTYPE *loader,
									  OMX_HANDLETYPE hComponent);

	/** The ComponentNameEnum method will enumerate through all the names of
    recognised valid components in the system. This function is provided
    as a means to detect all the components in the system run-time. There is
    no strict ordering to the enumeration order of component names, although
    each name will only be enumerated once.  If the loader supports run-time
    installation of new components, it is only requried to detect newly
    installed components when the first call to enumerate component names
    is made (i.e. when nIndex is 0x0).

    @param [out] cComponentName
        pointer to a null terminated string with the component name.  The
        names of the components are strings less than 127 bytes in length
        plus the trailing null for a maximum size of 128 bytes.  An example
        of a valid component name is "OMX.TI.AUDIO.DSP.MIXER\0".  Names are
        assigned by the vendor, but shall start with "OMX." and then have
        the Vendor designation next.
    @param [in] nNameLength
        number of characters in the cComponentName string.  With all
        component name strings restricted to less than 128 characters
        (including the trailing null) it is recomended that the caller
        provide a input string for the cComponentName of 128 characters.
    @param [in] nIndex
        number containing the enumeration index for the component.
        Multiple calls to OMX_ComponentNameEnum with increasing values
        of nIndex will enumerate through the component names in the
        system until OMX_ErrorNoMore is returned.  The value of nIndex
        is 0 to (N-1), where N is the number of valid installed components
        in the system.
    @return OMX_ERRORTYPE
        If the command successfully executes, the return code will be
        OMX_ErrorNone.  When the value of nIndex exceeds the number of
        components in the system minus 1, OMX_ErrorNoMore will be
        returned. Otherwise the appropriate OMX error will be returned.
	*/
	OMX_ERRORTYPE (*ComponentNameEnum)(	OMX_LOADERTYPE *loader,
								        OMX_OUT OMX_STRING cComponentName,
								        OMX_IN OMX_U32 nNameLength,
								        OMX_IN  OMX_U32 nIndex);

	/** The GetRolesOfComponent method will return the number of roles supported by the given
    component and (if the roles field is non-NULL) the names of those roles. The call will fail if
    an insufficiently sized array of names is supplied. To ensure the array is sufficiently sized the
    client should:
        * first call this function with the roles field NULL to determine the number of role names
        * second call this function with the roles field pointing to an array of names allocated
          according to the number returned by the first call.

    @param [in] compName
        This is the name of the component being queried about.
    @param [inout] pNumRoles
        This is used both as input and output.

        If roles is NULL, the input is ignored and the output specifies how many roles the component supports.

        If compNames is not NULL, on input it bounds the size of the input structure and
        on output, it specifies the number of roles string names listed within the roles parameter.
    @param [out] roles
        If NULL this field is ignored. If non-NULL this points to an array of 128-byte strings
        which accepts a list of the names of all standard components roles implemented on the
        specified component name.
    @return OMX_ERRORTYPE
        It returns OMX_ErrorNone if the function is executed successfully
        or any other appropriate OMX error otherwise.
	*/
	OMX_ERRORTYPE (*GetRolesOfComponent)( OMX_LOADERTYPE *loader,
								          OMX_IN OMX_STRING compName,
								          OMX_INOUT OMX_U32 *pNumRoles,
								          OMX_OUT OMX_U8 **roles);

	/** The GetComponentsOfRole method will return the number of components that support the given
    role and (if the compNames field is non-NULL) the names of those components. The call will fail if
    an insufficiently sized array of names is supplied. To ensure the array is sufficiently sized the
    client should:
        * first call this function with the compNames field NULL to determine the number of component names
        * second call this function with the compNames field pointing to an array of names allocated
          according to the number returned by the first call.

    @param [in] role
        This is generic standard component name consisting only of component class
        name and the type within that class (e.g. 'audio_decoder.aac').
    @param [inout] pNumComps
        This is used both as input and output.

        If compNames is NULL, the input is ignored and the output specifies how many components support
        the given role.

        If compNames is not NULL, on input it bounds the size of the input structure and
        on output, it specifies the number of components string names listed within the compNames parameter.
    @param [inout] compNames
        If NULL this field is ignored. If non-NULL this points to an array of 128-byte strings which accepts
        a list of the names of all physical components that implement the specified standard component name.
        Each name is NULL terminated. numComps indicates the number of names.
    @return OMX_ERRORTYPE
        It returns OMX_ErrorNone if the function is executed successfully
        or any other appropriate OMX error otherwise.
	*/
	OMX_ERRORTYPE (*GetComponentsOfRole)( OMX_LOADERTYPE *loader,
								          OMX_IN OMX_STRING role,
								          OMX_INOUT OMX_U32 *pNumComps,
								          OMX_INOUT OMX_U8  **compNames);
	OMX_PTR pLoaderPrivate;
	};

#define OMX_MAX_ROLESBUFFER_SIZE 256

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*OMX_LOADER_H*/
