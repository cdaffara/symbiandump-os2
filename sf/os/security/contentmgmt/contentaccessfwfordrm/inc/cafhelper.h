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

#ifndef __CAFHELPER_H__
#define __CAFHELPER_H__

#include <e32base.h>
#include <f32file.h>

namespace ContentAccess
	{
	//forward declaration
	class MCAFHelperInterface;
	
	/**
	 CAF client application can use this class to load cafhelper.dll and get access to entry function pointer
	 of the dll,using which the appropriate error handling APIs can be called.

	 @publishedPartner
	 @released
	 */
	 	
	NONSHARABLE_CLASS(CCAFHelper) : public CBase
		{
	public:
		
		/** 
		 Constructs a new CCAFHelper object.
		 
		 @return 		A pointer to an instance of CCAFHelper class.
		 @capability	All -TCB. 
		 */
		 
		IMPORT_C static CCAFHelper* NewL();
		
		/** 
		 Constructs a new CCAFHelper object and puts a pointer to the new object 
	 	 onto the cleanup stack..
		 
		 @return		A pointer to an instance of CCAFHelper class.
		 @capability	All -TCB. 
		 */
		 
		IMPORT_C static CCAFHelper* NewLC();
		
		/**
		Accessor method which returns an entry point function pointer of cafhelper.dll,
		using which the error handling APIs can be accessed.
		
		@return		Reference to an instance of MCafHelperInterface class.
		@capability	All -TCB. 
		*/
		
		IMPORT_C MCAFHelperInterface& operator()() const;
		
		// virtual destructor.		
		virtual ~CCAFHelper(); 
		
			
		private:
		
		CCAFHelper();
		void ConstructL();
		
		private:
		
		RLibrary iLibrary;
		MCAFHelperInterface* iHelper;
		};

	} // namespace ContentAccess
	
#endif // __CAFHELPER_H__

