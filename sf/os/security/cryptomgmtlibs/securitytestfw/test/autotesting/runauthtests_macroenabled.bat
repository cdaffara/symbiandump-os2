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
	
md c:\tauth
copy /s z:\tauth\* c:\tauth

testexecute \tauth\tauthsvr\scripts\tpostmarketplugin_macroenabled.script
move c:\logs\testexecute\tpostmarketplugin_macroenabled.htm e:\testresults\tpostmarketplugin_macroenabled.htm

attrib c:\tauth\*.* -r
del c:\tauth\*.*
rmdir c:\tauth

