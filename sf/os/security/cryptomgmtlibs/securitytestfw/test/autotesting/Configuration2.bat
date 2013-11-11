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
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins disable_all
perl %SECURITYSOURCEDIR%\os\security\cryptoservices\certificateandkeymgmt\twtlscert\scripts\batchfiles\certstorePlugins enable tadditionalstores.dll

copy \epoc32\wins\c\tcertstore\certstores\test1certstore.dat \epoc32\wins\c\system\data\test1certstore.dat
copy \epoc32\wins\c\tcertstore\certstores\test2certstore.dat \epoc32\wins\c\system\data\test2certstore.dat

attrib -R \epoc32\wins\c\system\data\test1certstore.dat
attrib -R \epoc32\wins\c\system\data\test2certstore.dat

copy \epoc32\winscw\c\tcertstore\certstores\test1certstore.dat \epoc32\winscw\c\system\data\test1certstore.dat
copy \epoc32\winscw\c\tcertstore\certstores\test2certstore.dat \epoc32\winscw\c\system\data\test2certstore.dat

attrib -R \epoc32\winscw\c\system\data\test1certstore.dat
attrib -R \epoc32\winscw\c\system\data\test2certstore.dat
