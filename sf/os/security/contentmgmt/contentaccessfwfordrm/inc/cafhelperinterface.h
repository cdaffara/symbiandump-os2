/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

@publishedPartner
@released
*/

#ifndef __CAFHELPERINTERFACE_H__
#define __CAFHELPERINTERFACE_H__

#include <e32base.h>
#include <f32file.h>

namespace ContentAccess
	{
	
	// UID for CAF Helper Dll.
	const TUid KCAFHelperInterfaceUID = {0x20024480};
	
	_LIT(KCAFHelperLibraryName, "cafhelper.dll");
	
	/**
	 This interface defined by UID KCAFHelperInterfaceUID provides APIs to handle 
	 CAF error messages when CAF client application is not interested /unable to handle 
	 CAF errors.UI/device manufacturer will provide error handling functionality by 
	 implementing this interface class as a dll which is enforced to be named as cafhelper.dll.

	 @publishedPartner
	 @released
	 */
	 	
	class MCAFHelperInterface
		{
	public:
		
		/**  
		 This method handles the CAF error received for a given file.
		 @param aError		One of the CAF errors. 
		 @param aFileName 	File name or file URI for which the error is received
		 @return 			KErrNone if the error is handled successfully.Otherwise one of the other system-wide
							errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt HandleCAFErrorL(TInt aError, const TDesC& aFileName) = 0;
		
		/**  
		 This method handles the CAF error received for a given file .
		 @param aError		One of the CAF errors. 
		 @param aFileHandle File handle for which the error is received
		 @return 			KErrNone if the error is handled successfully.Otherwise one of the other system-wide
							errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/	
		virtual TInt HandleCAFErrorL(TInt aError, RFile& aFileHandle) = 0;
		
		/**  
		 This method handles the CAF error received for WMDRM content .
		 @param aError		One of the CAF errors. 
		 @param aHeaderData The header data of the content for which the error is received
		 @return 			KErrNone if the error is handled successfully.Otherwise one of the other system-wide
							errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt HandleCAFErrorL(TInt aError, const TDesC8& aHeaderData) = 0;
		
		virtual void Release() = 0;
		
		};
		
	} // namespace ContentAccess
	
#endif // __CAFHELPERINTERFACE_H__

