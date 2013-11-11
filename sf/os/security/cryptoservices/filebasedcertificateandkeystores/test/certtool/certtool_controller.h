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
*
*/


#ifndef __CERTTOOL_CONTROLLER_H_
#define __CERTTOOL_CONTROLLER_H_

#include <f32file.h>
#include <barsc.h>
#include <x509cert.h>
#include <wtlscert.h>
#include <cctcertinfo.h>

#include "certtool_defcontroller.h"
#include "certtool_engine.h"
#include "keytool_view.h"
#include "controller.h"

class CKeyToolParameters;

/**
 * The controller in the MVC pattern for the Symbian OS certtool.
 */
class CCertToolController : public CController, public CertToolDefController
	{ 
	public:
		static CCertToolController* NewLC(MKeyToolView& aView);
	
		~CCertToolController();		
		
		/**
		 * Given a command code it delegates to the proper handler.
		 *
		 * @param aCommand The command code to be executed.
		 */
		void HandleCommandL(TInt aCommand, CKeyToolParameters* aParam);

	protected:
		void ConstructL();

	private:
		CCertToolController(MKeyToolView& aView);	

	protected:
		CCertToolEngine* iEngine;
	};
	
#endif    //__CERTTOOL_CONTROLLER_H_

