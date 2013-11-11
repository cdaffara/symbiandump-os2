// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the setup utility step for copying rights objects into the test stream agents
// 
//

#include "tsetuprightsobjects.h"

using namespace StreamAccess;

CScafSetupRightsObjects::CScafSetupRightsObjects(CScafServer& aParent): CScafStep(aParent)
/**
 * Constructor
 */
	{
	SetTestStepName(KScafSetupRightsObjectsStep);
	}

CScafSetupRightsObjects::~CScafSetupRightsObjects()
/**
 * Destructor
 */
	{
	}

TVerdict CScafSetupRightsObjects::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Setup Rights Objects ***"));
	
	TPtrC privateFolderPath;
	
	TName fPrivateFolderPath;
	
	fPrivateFolderPath.Format(KScafConfigPrivateFolderPath, 0);
	
	if(!GetStringFromConfig(ConfigSection(),fPrivateFolderPath,privateFolderPath))
		{
		privateFolderPath.Set(KStaPrivateFolder);
		}
	
	//Clear the contents of the stream agents private directory
	TRAPD(err,CleanAgentsPrivateFolderL(privateFolderPath));
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("*** CleanAgentsPrivateFolderL() Leave : %d  ***"),err);
		User::Leave(err);
		}
	
	/* Rights object identifier is formatted with the index in order to copy the list of rights
	 * object files stated within the configuration, into the stream agents private
	 * directory
	 */
	TInt index=0;
	TName roIdentifier; 
	roIdentifier.Format(KScafConfigRoName, index);
	
	// Filename of the current rights object
	TPtrC roName;									
	
	while (GetStringFromConfig(ConfigSection(),roIdentifier,roName))
		{
		//Copy the rights object into the private directory of the stream agent
		TRAP(err,CopyFile2AgentsPrivateFolderL(iParent.Fs(),roName, privateFolderPath));
		
		if(err != KErrNone)
			{
			ERR_PRINTF3(_L("*** PREAMBLE - CopyFile2AgentsPrivateFolderL() Leave: %S - %d  ***"),&roName,err);
			User::Leave(err);
			}
		
		// increment the index and reformat the identifier for the next filename
		index++;
		roIdentifier.Format(KScafConfigRoName, index);
		}
	
	return TestStepResult();	
	}
