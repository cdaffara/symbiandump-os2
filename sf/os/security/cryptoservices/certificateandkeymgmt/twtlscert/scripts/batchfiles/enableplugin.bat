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

cd \epoc32\winscw\c\twtlscert\scripts\batchfiles

perl certstorePlugins disable_all
perl certstorePlugins enable MIDP2CertStore.dll
perl certstorePlugins enable swicertstoreplugin.dll
perl certstorePlugins enable tadditionalstores.dll
perl certstorePlugins enable tadditionalstoressoftware.dll
perl certstorePlugins enable tDeviceImmutablestores.dll
perl certstorePlugins enable thwsimstores.dll
perl certstorePlugins enable thwuiccstores.dll
perl certstorePlugins enable thwwimstores.dll
perl certstorePlugins enable filecertstore.dll

del /s /f /q \epoc32\RELEASE\WINSCW\UDEB\plugins_backup\
del /s /f /q \epoc32\RELEASE\WINSCW\UREL\plugins_backup\