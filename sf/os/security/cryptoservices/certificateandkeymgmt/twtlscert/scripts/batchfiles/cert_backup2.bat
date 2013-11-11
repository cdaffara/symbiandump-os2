@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

call \epoc32\winscw\c\twtlscert\scripts\batchfiles\cert_backup1.bat

rem runring \tcertstore\scripts\unifiedcertstore3-conf2.script 
cd \
cd epoc32\release\winscw\udeb
t_certstore -Dtextshell -- \tcertstore\scripts\unifiedcertstore3-conf2.script \logs\testexecute\tcertstore_conf2a.3.htm
