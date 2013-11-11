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

buildrom.cmd -DDISABLE_UPS_OBSERVER_PLUGIN  -D_SERIAL_DOWNLOA -DUSE_STRONG_CRYPTOGRAPHY -DTEXT_ONLY_ROM -D_EABI=ARMV5 -DRVCT h4hrp textshell.oby ups.iby  tups.iby setcap.iby