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

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "omxilsymbiancomponentloader.h"
#include "omxiluids.hrh"
#include <openmax/il/core/omxilloaderif.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <string.h>
#include <delimitedparser8.h>
#include <openmax/il/core/omxilloaderif.hrh>
#include <openmax/il/loader/omxilcomponentif.hrh>
#include "log.h"

OMX_ERRORTYPE OmxInitializeComponentLoader(OMX_LOADERTYPE *loader);
OMX_ERRORTYPE OmxUnInitializeComponentLoader(OMX_LOADERTYPE *loader);
OMX_ERRORTYPE OmxLoadComponent(	OMX_LOADERTYPE *loader,
								OMX_OUT OMX_HANDLETYPE* pHandle,
								OMX_IN  OMX_STRING cComponentName,
								OMX_IN  OMX_PTR pAppData,
								OMX_IN  OMX_CALLBACKTYPE* pCallBacks);
OMX_ERRORTYPE OmxUnloadComponent( OMX_LOADERTYPE *loader, 
								  OMX_HANDLETYPE hComponent);
OMX_ERRORTYPE OmxComponentNameEnum(	OMX_LOADERTYPE *loader,
							        OMX_OUT OMX_STRING cComponentName,
							        OMX_IN OMX_U32 nNameLength,
							        OMX_IN  OMX_U32 nIndex);
OMX_ERRORTYPE OmxGetRolesOfComponent( OMX_LOADERTYPE *loader,
							          OMX_IN OMX_STRING compName,
							          OMX_INOUT OMX_U32 *pNumRoles,
							          OMX_OUT OMX_U8 **roles);
OMX_ERRORTYPE OmxGetComponentsOfRole( OMX_LOADERTYPE *loader,
							          OMX_IN OMX_STRING role,
							          OMX_INOUT OMX_U32 *pNumComps,
							          OMX_INOUT OMX_U8  **compNames);



OMX_ERRORTYPE OmxInitializeComponentLoader(OMX_LOADERTYPE *loader)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->InitializeComponentLoader();
	}

OMX_ERRORTYPE OmxUnInitializeComponentLoader(OMX_LOADERTYPE *loader)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->UnInitializeComponentLoader(loader);
	}

OMX_ERRORTYPE OmxLoadComponent(	OMX_LOADERTYPE *loader,
								OMX_OUT OMX_HANDLETYPE* pHandle,
								OMX_IN  OMX_STRING cComponentName,
								OMX_IN  OMX_PTR pAppData,
								OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->LoadComponent(pHandle,cComponentName,pAppData,pCallBacks);
	}

OMX_ERRORTYPE OmxUnloadComponent( OMX_LOADERTYPE *loader, 
								  OMX_HANDLETYPE hComponent)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->UnloadComponent(hComponent);
	}

OMX_ERRORTYPE OmxComponentNameEnum(	OMX_LOADERTYPE *loader,
							        OMX_OUT OMX_STRING cComponentName,
							        OMX_IN OMX_U32 nNameLength,
							        OMX_IN  OMX_U32 nIndex)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->ComponentNameEnum(cComponentName,nNameLength,nIndex);
	}

OMX_ERRORTYPE OmxGetRolesOfComponent( OMX_LOADERTYPE *loader,
							          OMX_IN OMX_STRING compName,
							          OMX_INOUT OMX_U32 *pNumRoles,
							          OMX_OUT OMX_U8 **roles)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->GetRolesOfComponent(compName,pNumRoles,roles);
	}

OMX_ERRORTYPE OmxGetComponentsOfRole( OMX_LOADERTYPE *loader,
							          OMX_IN OMX_STRING role,
							          OMX_INOUT OMX_U32 *pNumComps,
							          OMX_INOUT OMX_U8  **compNames)
	{
	return ((CSymbianOmxComponentLoader*)((OMX_LOADERTYPE*)loader)->pLoaderPrivate)->GetComponentsOfRole(role,pNumComps,compNames);
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::InitializeComponentLoader()
	{
	RImplInfoPtrArray ecomArray;
	TRAPD(error,REComSession::ListImplementationsL(TUid::Uid(KUidOmxILSymbianComponentIf),ecomArray));
	if( error != KErrNone )
		{
		ecomArray.ResetAndDestroy();
		return OMX_ErrorInsufficientResources;
		}
	TInt index;
	CImplementationInformation* info;

	TOmxComponentInfo component;
	
	iComponentNameList.Reset();
	
	const TInt KEcomArrayCount = ecomArray.Count();
	for( index=0; index<KEcomArrayCount && error == KErrNone; ++index )
		{
		info = ecomArray[index];
		component.iUid = info->ImplementationUid();
		if( info->DisplayName().Length() >= OMX_MAX_STRINGNAME_SIZE || 
			info->DataType().Length() >= OMX_MAX_ROLESBUFFER_SIZE )
			{
			error = KErrBadName;
			break;
			}

		component.iComponentName.Copy(info->DisplayName());
		component.iRoles.Copy(info->DataType());

		error = iComponentNameList.Append(component);
		if(error)
			{
			break;
			}
		}
	ecomArray.ResetAndDestroy();
	if( error != KErrNone )
		{
		if( error == KErrNoMemory )
			{
			return OMX_ErrorInsufficientResources;
			}

		return OMX_ErrorUndefined;
		}
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::UnInitializeComponentLoader(OMX_LOADERTYPE * /*loader*/)
	{
	iComponentNameList.Reset();
	iComponents.ResetAndDestroy();
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::LoadComponent(	OMX_HANDLETYPE* pHandle,
								OMX_STRING cComponentName,
								OMX_PTR pAppData,
								OMX_CALLBACKTYPE* pCallBacks)
	{
	TInt index;
    TPtrC8 lComponentName(reinterpret_cast<TUint8 *>(cComponentName), strlen(cComponentName));
    COmxILComponentIf* component = NULL;
	OMX_ERRORTYPE error = OMX_ErrorComponentNotFound;

    const TInt KComponentNameListCount = iComponentNameList.Count();
    for( index = 0; index < KComponentNameListCount; ++index )
		{
		TOmxComponentInfo& comp = iComponentNameList[index];
		DEBUG_PRINTF3(_L8("CSymbianOmxComponentLoader::LoadComponent : comp [%index] [%S]"), index, &comp.iComponentName);
		if( comp.iComponentName.CompareF(lComponentName) == 0 )
			{
			TRAPD(err, component = COmxILComponentIf::CreateImplementationL(iComponentNameList[index].iUid));
			if( err != KErrNone )
				{
				if ( err == KErrNoMemory )
					{
					return OMX_ErrorInsufficientResources;
					}
				else
					{
					return OMX_ErrorInvalidComponent;
					}
				}

			*pHandle = component->Handle();
			if( !*pHandle )
				{
				return OMX_ErrorInvalidComponent;
				}

			error = (static_cast<OMX_COMPONENTTYPE*>(*pHandle))->SetCallbacks(*pHandle, pCallBacks, pAppData);
			
			if(error != OMX_ErrorNone)
				{
				(static_cast<OMX_COMPONENTTYPE*>(*pHandle))->ComponentDeInit(*pHandle);
				delete component;
				return error;
				}
			if( iComponents.Append(component) != KErrNone )
				{
				(static_cast<OMX_COMPONENTTYPE*>(*pHandle))->ComponentDeInit(*pHandle);
				delete component;
				return OMX_ErrorInsufficientResources;
				}
			return error;
			}
		}
    return error;
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::UnloadComponent( OMX_HANDLETYPE hComponent)
	{
	TInt index;
	COmxILComponentIf* component;

	const TInt KComponentsCount = iComponents.Count();
	for( index = 0; index < KComponentsCount; ++index )
		{
		component = iComponents[index];
		if( component->Handle() == hComponent )
			{
			(static_cast<OMX_COMPONENTTYPE*>(hComponent))->ComponentDeInit( hComponent );
			delete component;
			iComponents.Remove(index);
			return OMX_ErrorNone;
			}
		}
	return OMX_ErrorComponentNotFound;
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::ComponentNameEnum(	OMX_STRING aComponentName,
							        OMX_U32 aNameLength,
							        OMX_U32 aIndex)
	{
	ASSERT(aComponentName);

	const TInt KIndexForPopulateList = 0;
	if ( aIndex == KIndexForPopulateList )
		{
		RImplInfoPtrArray ecomArray;
		TRAPD(error, REComSession::ListImplementationsL(TUid::Uid(KUidOmxILSymbianComponentIf), ecomArray));
		if (error != KErrNone)
			{
			return OMX_ErrorUndefined;
			}
		TInt index;
		iComponentNameList.Reset();

		const TInt KEcomArrayCount = ecomArray.Count();
		// Create Controller Implementation Information for each entry
		for (index=0; index<KEcomArrayCount; ++index)
			{
			CImplementationInformation& info = *(ecomArray[index]);
			TOmxComponentInfo component;
			
			component.iUid = info.ImplementationUid();
			if( info.DisplayName().Length() >= OMX_MAX_STRINGNAME_SIZE || 
				info.DataType().Length() >= OMX_MAX_ROLESBUFFER_SIZE )
				{
				error = KErrBadName;
				break;
				}
			component.iComponentName.Copy(info.DisplayName());
			component.iRoles.Copy(info.DataType());
			error = iComponentNameList.Append(component);
			if( error != KErrNone )
				{
				break;
				}
			}
		ecomArray.ResetAndDestroy();
		if( error != KErrNone )
			{
			return OMX_ErrorUndefined;
			}
		}
	if (aIndex<iComponentNameList.Count())
		{
		TOmxComponentInfo& comp = iComponentNameList[aIndex];
		TPtr8 ptr((TUint8*)aComponentName, 0, aNameLength);
		ptr.Copy(comp.iComponentName);
		ptr.PtrZ();
		}
	else
		{
		return OMX_ErrorNoMore;
		}
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::GetRolesOfComponent( OMX_STRING compName,
							          OMX_U32 *pNumRoles,
							          OMX_U8 **roles)
	{
	TInt index, roleindex;

	ASSERT(pNumRoles);
    if( !compName )
    	{
    	return OMX_ErrorBadParameter;
    	}
		
    TPtrC8 tComponentName(reinterpret_cast<TUint8 *>(compName), strlen(compName));		

    OMX_ERRORTYPE error = OMX_ErrorInvalidComponentName;
    const TInt KComponentNameListCount = iComponentNameList.Count();
    for( index = 0, roleindex = 0; index < KComponentNameListCount; ++index )
		{
		if( iComponentNameList[index].iComponentName.Compare(tComponentName) == 0 )
			{
			char parser[OMX_MAX_ROLESBUFFER_SIZE], *p;
			size_t size;
			TInt i, j;

			size = iComponentNameList[index].iRoles.Length();

			strncpy(parser, 
					reinterpret_cast<char*>(const_cast<unsigned char*>(iComponentNameList[index].iRoles.Ptr())),
					size);
			parser[size]=0;

			for( i=0, j=0, p=parser; i<size; ++i, ++j, ++p )
				{
				if( *p == ',' )
					{
					if( roles && (roleindex < *pNumRoles) )
						{
						roles[roleindex][j] = 0;
						}
					if( i < size )
						{
						++roleindex;
						j=-1;
						}
					}
				else
					{
					if( roles && (roleindex < *pNumRoles) )
						{
						roles[roleindex][j] = *p;
						}
					}
				}
			if( roles && (roleindex < *pNumRoles) )
				{
				roles[roleindex][j] = 0;
				}
			++roleindex;
			error = OMX_ErrorNone;
			break;
			}
		}
    *pNumRoles = roleindex;
    return error;
	}

OMX_ERRORTYPE CSymbianOmxComponentLoader::GetComponentsOfRole( OMX_STRING role,
							          OMX_U32 *pNumComps,
							          OMX_U8  **compNames)
	{
	TInt index, compindex;
    TPtrC8 lRole(reinterpret_cast<TUint8 *>(role), strlen(role));

	ASSERT(pNumComps);

    TInt KComponentNameListCount = iComponentNameList.Count();
    for( index = 0, compindex = 0; index < KComponentNameListCount; ++index )
		{
		if( iComponentNameList[index].iRoles.Find(lRole) != KErrNotFound )
			{
			if( compNames != NULL )
				{
				if( compNames[compindex]!= NULL )
					{
		            strncpy((char*)compNames[compindex],(char*)iComponentNameList[index].iComponentName.Ptr(),iComponentNameList[index].iComponentName.Length());
		            compNames[compindex][iComponentNameList[index].iComponentName.Size()]=0;
					}
				else
					{
					return OMX_ErrorBadParameter;
					}
				}
			++compindex;
			}
		}
    *pNumComps = compindex;
    return OMX_ErrorNone;
	}

OMX_HANDLETYPE CSymbianOmxComponentLoader::Handle()
	{
	return iHandle;
	}

CSymbianOmxComponentLoader::CSymbianOmxComponentLoader()
	{
	}

CSymbianOmxComponentLoader::~CSymbianOmxComponentLoader()
	{
	delete iHandle;
	}

void CSymbianOmxComponentLoader::ConstructL()
	{
	iHandle = new(ELeave) OMX_LOADERTYPE;
	
	iHandle->InitializeComponentLoader = &::OmxInitializeComponentLoader;
	iHandle->UnInitializeComponentLoader = &::OmxUnInitializeComponentLoader;
	iHandle->LoadComponent = &::OmxLoadComponent;
	iHandle->UnloadComponent = &::OmxUnloadComponent;
	iHandle->ComponentNameEnum = &::OmxComponentNameEnum;
	iHandle->GetRolesOfComponent = &::OmxGetRolesOfComponent;
	iHandle->GetComponentsOfRole = &::OmxGetComponentsOfRole;
	
	iHandle->pLoaderPrivate = this;
	}

CSymbianOmxComponentLoader* CSymbianOmxComponentLoader::NewL()
	{
	CSymbianOmxComponentLoader* self = new(ELeave) CSymbianOmxComponentLoader;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}

// ECOM
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidOmxILSymbianComponentLoader, CSymbianOmxComponentLoader::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
