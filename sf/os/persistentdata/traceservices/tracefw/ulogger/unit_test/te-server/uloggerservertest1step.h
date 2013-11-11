/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 @file UloggerServerTest1Step.h
 @internalTechnology
*/
#if (!defined __ULOGGERSERVERTEST1_STEP_H__)
#define __ULOGGERSERVERTEST1_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"


class CUloggerServerTest1Step : public CTe_UloggerServerTestSuiteStepBase
	{
public:
	CUloggerServerTest1Step();
	~CUloggerServerTest1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	//test methods
	TInt Test1L();//CULoggerServer::NewLC
	TInt Test2L();//CULoggerServer::IncrementSessions
	TInt Test3L();//CULoggerServer::Stop
	TInt Test4L();//CULoggerServer::GetPrimaryFilters
	TInt Test5L();//CULoggerServer::GetSecondaryFilters
	TInt Test6L();//CULoggerServer::RemovePrimaryFilterfromConfig
	TInt Test6AL();//CULoggerServer::RemoveSecondaryFilterfromConfig
	TInt Test7L();//CULoggerServer::EnablePrimaryFilter
	TInt Test8L();//CULoggerServer::SetSecondaryFilter
	TInt Test9L();//CULoggerServer::Start
	TInt Test10L();//CULoggerServer::AddOutputPlugin
	TInt Test11L();//CULoggerServer::RemoveOutputPlugin
	TInt Test12L();//CULoggerServer::SetActiveOutputMedia
	TInt Test13L();//CULoggerServer::SetOutputPluginSettings
	TInt Test14L();//CULoggerServer::GetActiveOutput
	TInt Test15L();//CULoggerServer::GetInstalledPlugins
	TInt Test16L();//CULoggerServer::GetOutputPluginSettingsL
	TInt Test17L();//CULoggerServer::GetOutputChannelL
	TInt Test18L();//CULoggerServer::SetBufferSizeL
	TInt Test19L();//CULoggerServer::SetDataNotificationSizeL
	TInt Test20L();//CULoggerServer::GetBufferSize
	TInt Test21L();// CULoggerServer::GetDataNotificationSize
	TInt Test22L();//CULoggerServer::SetGlobalSecondaryFilters
	TInt Test23L();//CULoggerServer::ReadBufferL
	TInt Test24L();//CULoggerServer::DataNotification
	TInt Test25L();//CULoggerServer::Set buffer mode
	TInt Test26L();//CULoggerServer::UnconfigureCurrentChannel
	TInt Test27L();//CULoggerServer::DoPostProcessing
	TInt Test28L();//CULoggerServer::RemoveOutputPluginSettingsL
	TInt Test29L();//CULoggerServer::GetInstalledControlPlugins
	TInt Test30L();//CULoggerServer::SetActiveControlMedia + DeActivateControlMedia
	TInt Test31L();//CULoggerServer::ProcessCommandL ver1
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	TInt iErrors;
	TInt iMediaCount; //this variable will be used as an counter for "mediaX"
	};

_LIT(KUloggerServerTest1Step,"UloggerServerTest1Step");

#endif
