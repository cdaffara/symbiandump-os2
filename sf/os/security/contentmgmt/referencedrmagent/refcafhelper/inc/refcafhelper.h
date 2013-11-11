/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

@test
@internalComponent
*/

#ifndef __REFCAFHELPER_H__
#define __REFCAFHELPER_H__

#include <e32base.h>
#include <caf/cafhelperinterface.h>

using namespace ContentAccess;
/**
 This is a reference implementation of the polymorphic interface defined by 
 UID KCAFHelperInterfaceUID.
 
 @file 
 @test
 @internalComponent
*/
	 	
NONSHARABLE_CLASS(CReferenceCafHelper) : public CBase, public MCAFHelperInterface
	{
	public:
		
		IMPORT_C MCAFHelperInterface* NewCafHelperImplL();
		
		CReferenceCafHelper();
		
		/**  
		 This method handles the CAF error received for a given file.
		 @param aError		One of the CAF errors. 
		 @param aFileName 	File name or file URI for which the error is received
		 @return 			KErrNone if the error is handled successfully.Otherwise one of the other system-wide
							errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C virtual TInt HandleCAFErrorL(TInt aError, const TDesC& aFileName);
		
		/**  
		 This method handles the CAF error received for a given file .
		 @param aError		One of the CAF errors. 
		 @param aFileHandle File handle for which the error is received
		 @return 			KErrNone if the error is handled successfully.Otherwise one of the other system-wide
							errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/	
		IMPORT_C virtual TInt HandleCAFErrorL(TInt aError, RFile& aFileHandle);
		
		/**  
		 This method handles the CAF error received for WMDRM content .
		 @param aError		One of the CAF errors. 
		 @param aHeaderData The header data of the content for which the error is received
		 @return 			KErrNone if the error is handled successfully.Otherwise one of the other system-wide
							errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C virtual TInt HandleCAFErrorL(TInt aError, const TDesC8& aHeaderData);
		
		IMPORT_C virtual void Release();
		
		virtual ~CReferenceCafHelper();
		};

#endif

