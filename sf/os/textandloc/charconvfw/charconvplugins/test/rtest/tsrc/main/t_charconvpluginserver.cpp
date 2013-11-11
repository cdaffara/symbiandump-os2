/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_charconvpluginserver.h"
#include "t_big5.h"
#include "t_cp949.h"
#include "t_autodetect.h"
#include "t_win1250.h"
#include "t_win1251.h"
#include "t_win1253.h"
#include "t_win1254.h"
#include "t_win1255.h"
#include "t_win1256.h"
#include "t_win1257.h"
#include "t_win1258.h"
#include "t_win874.h"
#include "t_ucs2.h"
#include "t_turkishsingle.h"
#include "t_turkishlockingsingle.h"
#include "t_turkishlocking.h"
#include "t_tis_620.h"
#include "t_spanishgsmsingle.h"
#include "t_snm.h"
#include "t_replacement.h"
#include "t_portugueselockingsingle.h"
#include "t_portugueselocking.h"
#include "t_portuguesesingle.h"
#include "t_iso8859x.h"
#include "t_iscii.h"
#include "t_gb2312.h"
#include "t_gbk.h"
#include "t_hz.h"
#include "t_extendedsms.h"
#include "t_koi8r.h"
#include "t_koi8u.h"
#include "t_gb18030.h"
#include "t_multi_plugins.h"
#include "t_generalforeign.h"
#include "t_jis.h"
#include "t_builtin.h"
#include "t_j5_kddiau.h"
#include "t_j5_docomo.h"
#include "t_eucjpdirectmap.h"
#include "t_shiftjisdirectmap.h"
#include "t_eucjp_packed.h"
#include "t_eucjp_packed_2.h"
#include "t_iso2022jp.h"
#include "t_iso2022jp_2.h"
#include "t_iso2022jp1_2.h"
#include "t_shiftjis_kddiau.h"
#include "t_shiftjis_docomo.h"

/* Path to the script

c:\z:\t_TEF\t_TEF_All.script

*/

_LIT(KServerName,"t_charconvpluginserver");

CT_CharconvPluginsServer *CT_CharconvPluginsServer::NewL()
    {
	CT_CharconvPluginsServer * server = new (ELeave) CT_CharconvPluginsServer();
    CleanupStack::PushL(server);
    server->StartL(KServerName);
    CleanupStack::Pop(server);
    return server;
    }


LOCAL_C void MainL()
    {
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CT_CharconvPluginsServer *server = NULL;
    TRAPD(err,server = CT_CharconvPluginsServer::NewL());
    if(!err)
        {
        // Sync with the client and enter the active scheduler
        RProcess::Rendezvous(KErrNone);
        sched->Start();
        }
    delete server;
    delete sched;
    }


GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
        {
        return KErrNoMemory;
        }
    TRAPD(err,MainL());
    // This if statement is here just to shut up RVCT, which would otherwise warn
    // that err was set but never used
    if (err)
        {
        err = KErrNone;
        }
    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
    }

CTestStep *CT_CharconvPluginsServer::CreateTestStep(const TDesC& aStepName)
    {
    if(aStepName == KTestStep_T_BIG5)
        {
        return new CT_BIG5();
        }
    else if(aStepName == KTestStep_T_CP949)
        {
        return new CT_CP949();
        }
    else if(aStepName == KTestStep_T_AUTODETECT)
        {
        return new CT_AUTODETECT();
        }
    
    else if(aStepName == KTestStep_T_WIN1250)
        {
        return new CT_WIN1250();
        }
    else if(aStepName == KTestStep_T_WIN1251)
        {
        return new CT_WIN1251();
        }
    else if(aStepName == KTestStep_T_WIN1253)
        {
        return new CT_WIN1253();
        }
    else if(aStepName == KTestStep_T_WIN1254)
        {
        return new CT_WIN1254();
        }
    else if(aStepName == KTestStep_T_WIN1255)
        {
        return new CT_WIN1255();
        }
    else if(aStepName == KTestStep_T_WIN1256)
        {
        return new CT_WIN1256();
        }
    else if(aStepName == KTestStep_T_WIN1257)
        {
        return new CT_WIN1257();
        }
    else if(aStepName == KTestStep_T_WIN1258)
        {
        return new CT_WIN1258();
        }
    else if(aStepName == KTestStep_T_WIN874)
        {
        return new CT_WIN874();
        }
    else if(aStepName == KTestStep_T_UCS2)
        {
        return new CT_UCS2();
        }
    else if(aStepName == KTestStep_T_TURKISHSINGLE)
        {
        return new CT_TURKISHSINGLE();
        }
    else if(aStepName == KTestStep_T_TURKISHLOCKINGSINGLE)
        {
        return new CT_TURKISHLOCKINGSINGLE();
        }
    else if(aStepName == KTestStep_T_TURKISHLOCKING)
        {
        return new CT_TURKISHLOCKING();
        }
    else if(aStepName == KTestStep_T_TIS_620)
        {
        return new CT_TIS_620();
        }
    else if(aStepName == KTestStep_T_SPANISHGSMSINGLE)
        {
        return new CT_SPANISHGSMSINGLE();
        }
    else if(aStepName == KTestStep_T_SNM)
        {
        return new CT_SNM();
        }
    else if(aStepName == KTestStep_T_REPLACEMENT)
        {
        return new CT_REPLACEMENT();
        }
    else if(aStepName == KTestStep_T_PORTUGUESELOCKINGSINGLE)
        {
        return new CT_PORTUGUESELOCKINGSINGLE();
        }
    else if(aStepName == KTestStep_T_PORTUGUESELOCKING)
        {
        return new CT_PORTUGUESELOCKING();
        }
    else if(aStepName == KTestStep_T_PORTUGUESESINGLE)
        {
        return new CT_PORTUGUESESINGLE();
        }
    else if(aStepName == KTestStep_T_ISO8859X)
        {
        return new CT_ISO8859X();
        }
    else if(aStepName == KTestStep_T_ISCII)
        {
        return new CT_ISCII();
        }
    else if(aStepName == KTestStep_T_GB2312)
        {
        return new CT_GB2312();
        }
    else if(aStepName == KTestStep_T_GBK)
        {
        return new CT_GBK();
        }
    else if(aStepName == KTestStep_T_HZ)
        {
        return new CT_HZ();
        }
    else if(aStepName == KTestStep_T_EXTENDEDSMS)
        {
        return new CT_EXTENDEDSMS();
        }
    else if(aStepName == KTestStep_T_KOI8R)
        {
        return new CT_KOI8R();
        }
    else if(aStepName == KTestStep_T_KOI8U)
        {
        return new CT_KOI8U();
        }
    else if(aStepName == KTestStep_T_GB18030)
        {
        return new CT_GB18030();
        }
    else if(aStepName == KTestStep_T_MULTI_PLUGINS)
        {
        return new CT_MULTI_PLUGINS();
        }
    else if(aStepName == KTestStep_T_GENERALFOREIGN)
        {
        return new CT_GENERALFOREIGN();
        }
    else if(aStepName == KTestStep_T_JIS)
        {
        return new CT_JIS();
        }
    else if(aStepName == KTestStep_T_BUILTIN)
        {
        return new CT_BUILTIN();
        }
    else if(aStepName == KTestStep_T_J5_KDDIAU)
        {
        return new CT_J5_KDDIAU();
        }
    else if(aStepName == KTestStep_T_J5_DOCOMO)
        {
        return new CT_J5_DOCOMO();
        }
    else if(aStepName == KTestStep_T_EUCJPDIRECTMAP)
        {
        return new CT_EUCJPDIRECTMAP();
        }
    else if(aStepName == KTestStep_T_SHIFTJISDIRECTMAP)
        {
        return new CT_SHIFTJISDIRECTMAP();
        }
    else if(aStepName == KTestStep_T_EUCJP_PACKED)
        {
        return new CT_EUCJP_PACKED();
        }
    else if(aStepName == KTestStep_T_EUCJP_PACKED_2)
        {
        return new CT_EUCJP_PACKED_2();
        }
    else if(aStepName == KTestStep_T_ISO2022JP)
        {
        return new CT_ISO2022JP();
        }
    else if(aStepName == KTestStep_T_ISO2022JP_2)
        {
        return new CT_ISO2022JP_2();
        }
    else if(aStepName == KTestStep_T_ISO2022JP1_2)
        {
        return new CT_ISO2022JP1_2();
        }
    else if(aStepName == KTestStep_T_SHIFTJIS_KDDIAU)
        {
        return new CT_SHIFTJIS_KDDIAU();
        }
    else if(aStepName == KTestStep_T_SHIFTJIS_DOCOMO)
        {
        return new CT_SHIFTJIS_DOCOMO();
        }
    return NULL;
    }

