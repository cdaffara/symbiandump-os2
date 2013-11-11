@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem The stub..
call makesis -s -d%epocroot% EComPluginStub.pkg

rem  This will create the first the SIS and then a signed SIs for WINSCW
call makesis -d%epocroot% EComPlugin1WINSCW.pkg
call signsis -S EComPlugin1WINSCW.SIS EComPlugin1WINSCWs.sis certstore\centreproot.pem certstore\centreproot.key

rem  This will create the first the SIS and then a signed SIs for ARMV5
call makesis -d%epocroot% EComPlugin1ARM.pkg
call signsis -S EComPlugin1ARM.SIS EComPlugin1ARMs.sis certstore\centreproot.pem certstore\centreproot.key

rem  This will create the first the SIS and then a signed SIs for WINSCW
call makesis -d%epocroot% EComPlugin1UpgWINSCW.pkg
call signsis -S EComPlugin1UpgWINSCW.SIS EComPlugin1UpgWINSCWs.sis certstore\centreproot.pem certstore\centreproot.key

rem  This will create the first the SIS and then a signed SIs for ARMV5
call makesis -d%epocroot% EComPlugin1UpgARM.pkg
call signsis -S EComPlugin1UpgARM.SIS EComPlugin1UpgARMs.sis certstore\centreproot.pem certstore\centreproot.key



rem  This will create the first the SIS and then a signed SIs for WINSCW
call makesis -d%epocroot% EComPlugin2WINSCW.pkg
call signsis -S EComPlugin2WINSCW.SIS EComPlugin2WINSCWs.sis certstore\centreproot.pem certstore\centreproot.key

rem  This will create the first the SIS and then a signed SIs for ARMV5
call makesis -d%epocroot% EComPlugin2ARM.pkg
call signsis -S EComPlugin2ARM.SIS EComPlugin2ARMs.sis certstore\centreproot.pem certstore\centreproot.key





rem  This will create the first the SIS and then a signed SIs for WINSCW
call makesis -d%epocroot% EComPlugin3UpgWINSCW.pkg
call signsis -S EComPlugin3UpgWINSCW.SIS EComPlugin3UpgWINSCWs.sis certstore\centreproot.pem certstore\centreproot.key

rem  This will create the first the SIS and then a signed SIs for ARMV5
call makesis -d%epocroot% EComPlugin3UpgARM.pkg
call signsis -S EComPlugin3UpgARM.SIS EComPlugin3UpgARMs.sis certstore\centreproot.pem certstore\centreproot.key
