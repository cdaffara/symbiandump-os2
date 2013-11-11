@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
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

copy ..\release\armv5\urel\t_ecom.exe .
copy ..\release\armv5\urel\TPCopyFileUtility.exe .
copy ..\release\armv5\urel\TPDeleteFileUtility.exe .
md ecom
copy ..\release\armv5\urel\TestProduct_PlugIn.dll ecom
copy ..\data\Z\ecom\TestProduct_PlugIn.RSC ecom

call makesis t_ecom.pkg t_ecom.sis

call signsis -s t_ecom.sis t_ecom.sis zeus.cer SymbianACS.key caforstat
