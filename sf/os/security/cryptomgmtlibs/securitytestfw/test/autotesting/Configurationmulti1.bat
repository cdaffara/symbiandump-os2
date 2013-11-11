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

cd \

rem Certstore Configuration multiple certstore 1 setup script

perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins disable_all
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable filecertstore.dll
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable thwsimstores.dll
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable thwuiccstores.dll
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable thwwimstores.dll
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable tadditionalstoressoftware.dll