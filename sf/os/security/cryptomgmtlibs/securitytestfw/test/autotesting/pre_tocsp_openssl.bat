@rem
@rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

call md \epoc32\winscw\c\tswi
call md \epoc32\winscw\c\tswi\policies
call copy /Y  %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\tcertstore\scripts\swipolicy_ocsp_http_filter.ini \epoc32\winscw\c\tswi\policies\swipolicy_ocsp_http_filter.ini
call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\setupSwiPolicy.bat swipolicy_ocsp_http_filter.ini
call %SECURITYSOURCEDIR%\os\security\cryptomgmtlibs\securitytestfw\test\autotesting\clean_certstore.bat
