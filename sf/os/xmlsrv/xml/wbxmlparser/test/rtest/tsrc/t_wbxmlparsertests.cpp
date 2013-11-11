// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32test.h>

#include <xml/parserfeature.h>
#include <xml/xmlframeworkerrors.h>

#include "stabilitytestclass.h"
#include "testdocuments.h"
_LIT  (KTestDocument0, "c:\\system\\XmlTest\\SyncML\\1.1\\add-to-client\\FromServer0.wbxml");
_LIT  (KTestDocument1, "c:\\system\\XmlTest\\SyncML\\1.1\\add-to-client\\FromServer1.wbxml");
_LIT  (KTestDocument2, "c:\\system\\XmlTest\\SyncML\\1.1\\add-to-server\\FromServer1.wbxml");
_LIT  (KTestDocument3, "c:\\system\\XmlTest\\SyncML\\1.1\\Atomic\\FromServer1.wbxml");
_LIT  (KTestDocument4, "c:\\system\\XmlTest\\Wml\\1.1\\AllElements.wmlc");
_LIT  (KTestDocument5, "c:\\system\\XmlTest\\Wml\\1.1\\CharEntities.wmlc");
_LIT  (KTestDocument6, "c:\\system\\XmlTest\\Wml\\1.1\\data.wmlc");
_LIT  (KTestDocument7, "c:\\system\\XmlTest\\Wml\\1.1\\http___www.bbc.co.uk_mobile_sportheads1.wmlc");
_LIT  (KTestDocument8, "c:\\system\\XmlTest\\Wml\\1.1\\mob.wmlc");
_LIT  (KTestDocument9, "c:\\system\\XmlTest\\Wml\\1.1\\Variables.wmlc");
_LIT  (KTestDocument10,"c:\\system\\XmlTest\\Wml\\1.1\\wireless_char.wmlc");
_LIT  (KTestDocument11,"c:\\system\\XmlTest\\Wml\\Codepage\\wml_data_attr_copepage_255.wmlc");

// Some characters had to have their string representation amended.
// i.e. dir paths should be seperated by '\\'
// carriage return (0D) replaced with \r
// line feed (0A) replaced with \n
// double quotes (") replaced with \"
// single quotes (') replaced with \'

_LIT  (KDataComparison0_1, "<SyncML xmlns=\'syncml:syncml1.1\'>111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111133333333333333333333333333333333333333333333333332<SyncHdr xmlns=\'syncml:syncml1.1\'><VerDTD xmlns=\'syncml:syncml1.1\'>1.1</VerDTD><VerProto xmlns=\'syncml:syncml1.1\'>SyncML/1.1</VerProto>");
_LIT  (KDataComparison0_2, "<SessionID xmlns=\'syncml:syncml1.1\'>1871351294</SessionID><MsgID xmlns=\'syncml:syncml1.1\'>1</MsgID><Target xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison0_3, "<LocURI xmlns=\'syncml:syncml1.1\'>63157234976619000</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML</LocURI>");
_LIT  (KDataComparison0_4, "</Source><RespURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML?f1sess=194.200.144.243sync1871351294</RespURI><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison0_5, "<MaxMsgSize xmlns=\'syncml:metinf\'>12288</MaxMsgSize></Meta></SyncHdr><SyncBody xmlns=\'syncml:syncml1.1\'><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>1</CmdID>");
_LIT  (KDataComparison0_6, "<MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>0</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>SyncHdr</Cmd>");
_LIT  (KDataComparison0_7, "<TargetRef xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML</TargetRef><SourceRef xmlns=\'syncml:syncml1.1\'>63157234976619000</SourceRef>");
_LIT  (KDataComparison0_8, "<Data xmlns=\'syncml:syncml1.1\'>212</Data></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>2</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison0_9, "<CmdRef xmlns=\'syncml:syncml1.1\'>1</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Put</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison0_10, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison0_11, "<Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010510T120024Z</Next></Anchor></Data></Item></Status>");
_LIT  (KDataComparison0_12, "<Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>3</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>2</CmdRef>");
_LIT  (KDataComparison0_13, "<Cmd xmlns=\'syncml:syncml1.1\'>Get</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks2</TargetRef><SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks2.dat</SourceRef>");
_LIT  (KDataComparison0_14, "<Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'><Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010510T120024Z</Next>");
_LIT  (KDataComparison0_15, "</Anchor></Data></Item></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>4</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison0_16, "<CmdRef xmlns=\'syncml:syncml1.1\'>3</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Alert</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison0_17, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison0_18, "<Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010509T105003Z</Next></Anchor></Data></Item></Status>");
_LIT  (KDataComparison0_19, "<Alert xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>5</CmdID><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison0_20, "<Target xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison0_21, "<LocURI xmlns=\'syncml:syncml1.1\'>./bookmarks</LocURI></Source><Meta xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Last xmlns=\'syncml:metinf\'>0</Last>");
_LIT  (KDataComparison0_22, "<Next xmlns=\'syncml:metinf\'>20010509T105145</Next></Anchor></Meta></Item></Alert><Final xmlns=\'syncml:syncml1.1\'></Final></SyncBody></SyncML>");


_LIT  (KDataComparison1_1, "<SyncML xmlns=\'syncml:syncml1.1\'><SyncHdr xmlns=\'syncml:syncml1.1\'><VerDTD xmlns=\'syncml:syncml1.1\'>1.1</VerDTD><VerProto xmlns=\'syncml:syncml1.1\'>SyncML/1.1</VerProto>");
_LIT  (KDataComparison1_2, "<SessionID xmlns=\'syncml:syncml1.1\'>1871351294</SessionID><MsgID xmlns=\'syncml:syncml1.1\'>1</MsgID><Target xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison1_3, "<LocURI xmlns=\'syncml:syncml1.1\'>63157234976619000</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML</LocURI>");
_LIT  (KDataComparison1_4, "</Source><RespURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML?f1sess=194.200.144.243sync1871351294</RespURI><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison1_5, "<MaxMsgSize xmlns=\'syncml:metinf\'>12288</MaxMsgSize></Meta></SyncHdr><SyncBody xmlns=\'syncml:syncml1.1\'><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>1</CmdID>");
_LIT  (KDataComparison1_6, "<MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>0</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>SyncHdr</Cmd>");
_LIT  (KDataComparison1_7, "<TargetRef xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML</TargetRef><SourceRef xmlns=\'syncml:syncml1.1\'>63157234976619000</SourceRef>");
_LIT  (KDataComparison1_8, "<Data xmlns=\'syncml:syncml1.1\'>212</Data></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>2</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison1_9, "<CmdRef xmlns=\'syncml:syncml1.1\'>1</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Put</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison1_10, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison1_11, "<Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010510T120024Z</Next></Anchor></Data></Item></Status>");
_LIT  (KDataComparison1_12, "<Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>3</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>2</CmdRef>");
_LIT  (KDataComparison1_13, "<Cmd xmlns=\'syncml:syncml1.1\'>Get</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks2</TargetRef><SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks2.dat</SourceRef>");
_LIT  (KDataComparison1_14, "<Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'><Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010510T120024Z</Next>");
_LIT  (KDataComparison1_15, "</Anchor></Data></Item></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>4</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison1_16, "<CmdRef xmlns=\'syncml:syncml1.1\'>3</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Alert</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison1_17, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison1_18, "<Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010509T105003Z</Next></Anchor></Data></Item></Status>");
_LIT  (KDataComparison1_19, "<Alert xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>5</CmdID><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison1_20, "<Target xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison1_21, "<LocURI xmlns=\'syncml:syncml1.1\'>./bookmarks</LocURI></Source><Meta xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Last xmlns=\'syncml:metinf\'>0</Last>");
_LIT  (KDataComparison1_22, "<Next xmlns=\'syncml:metinf\'>20010509T105145</Next></Anchor></Meta></Item></Alert><Final xmlns=\'syncml:syncml1.1\'></Final></SyncBody></SyncML>");


_LIT  (KDataComparison2_1, "<SyncML xmlns=\'syncml:syncml1.1\'><SyncHdr xmlns=\'syncml:syncml1.1\'><VerDTD xmlns=\'syncml:syncml1.1\'>1.1</VerDTD><VerProto xmlns=\'syncml:syncml1.1\'>SyncML/1.1</VerProto>");
_LIT  (KDataComparison2_2, "<SessionID xmlns=\'syncml:syncml1.1\'>43049786</SessionID><MsgID xmlns=\'syncml:syncml1.1\'>1</MsgID><Target xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison2_3, "<LocURI xmlns=\'syncml:syncml1.1\'>63156883059050000</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison2_4, "<LocURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML</LocURI></Source>");
_LIT  (KDataComparison2_5, "<RespURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML?f1sess=194.200.144.243sync43049786</RespURI><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison2_6, "<MaxMsgSize xmlns=\'syncml:metinf\'>12288</MaxMsgSize></Meta></SyncHdr><SyncBody xmlns=\'syncml:syncml1.1\'><Status xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison2_7, "<CmdID xmlns=\'syncml:syncml1.1\'>1</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>0</CmdRef>");
_LIT  (KDataComparison2_8, "<Cmd xmlns=\'syncml:syncml1.1\'>SyncHdr</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>http://208.184.103.62/servlet/fde.sync.SyncML</TargetRef>");
_LIT  (KDataComparison2_9, "<SourceRef xmlns=\'syncml:syncml1.1\'>63156883059050000</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data></Status><Status xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison2_10, "<CmdID xmlns=\'syncml:syncml1.1\'>2</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>1</CmdRef>");
_LIT  (KDataComparison2_11, "<Cmd xmlns=\'syncml:syncml1.1\'>Put</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison2_12, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data>");
_LIT  (KDataComparison2_13, "<Item xmlns=\'syncml:syncml1.1\'><Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010510T120024Z</Next>");
_LIT  (KDataComparison2_14, "</Anchor></Data></Item></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>3</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison2_15, "<CmdRef xmlns=\'syncml:syncml1.1\'>2</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Get</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison2_16, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data>");
_LIT  (KDataComparison2_17, "<Item xmlns=\'syncml:syncml1.1\'><Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010510T120024Z</Next>");
_LIT  (KDataComparison2_18, "</Anchor></Data></Item></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>4</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison2_19, "<CmdRef xmlns=\'syncml:syncml1.1\'>3</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Alert</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison2_20, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data>");
_LIT  (KDataComparison2_21, "<Item xmlns=\'syncml:syncml1.1\'><Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010501T172705Z</Next>");
_LIT  (KDataComparison2_22, "</Anchor></Data></Item></Status><Alert xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>5</CmdID><Data xmlns=\'syncml:syncml1.1\'>200</Data>");
_LIT  (KDataComparison2_23, "<Item xmlns=\'syncml:syncml1.1\'><Target xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</LocURI></Target>");
_LIT  (KDataComparison2_24, "<Source xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>./bookmarks</LocURI></Source>");
_LIT  (KDataComparison2_25, "<Meta xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Last xmlns=\'syncml:metinf\'>0</Last><Next xmlns=\'syncml:metinf\'>20010501T172843</Next></Anchor>");
_LIT  (KDataComparison2_26, "</Meta></Item></Alert><Results xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>6</CmdID><CmdRef xmlns=\'syncml:syncml1.1\'>2</CmdRef>");
_LIT  (KDataComparison2_27, "<Meta xmlns=\'syncml:syncml1.1\'><Type xmlns=\'syncml:metinf\'>application/vnd.syncml-devinf+xml</Type></Meta><TargetRef xmlns=\'syncml:syncml1.1\'>./devinf11</TargetRef>");
_LIT  (KDataComparison2_28, "<Item xmlns=\'syncml:syncml1.1\'><Source xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>./devinf11</LocURI></Source>");
_LIT  (KDataComparison2_29, "<Meta xmlns=\'syncml:syncml1.1\'><Type xmlns=\'syncml:metinf\'>application/vnd.syncml-devinf+xml</Type></Meta>");
_LIT  (KDataComparison2_30, "<Data xmlns=\'syncml:syncml1.1\'>2006A1D2D2F2F53594E434D4C2F2F44544420446576496E6620312E312F2F454E4A653312E3101513667573696F6E4F6E652C20496E632E0155353796E634D4C2046444");
_LIT  (KDataComparison2_31, "5015E3312E300149346312053796E634D4C205346444520312E30014B373657276657201475D32E2F43414C454E444152014C343616C656E646172015A463746578742F782D7663616C656E64617201643312E");
_LIT  (KDataComparison2_32, "3001162463746578742F782D7663616C656E64617201643312E300115F60331016033201603330111475D32E2F434F4E5441435453014C3436F6E7461637473015A463746578742F782D766361726401643322");
_LIT  (KDataComparison2_33, "E3101162463746578742F782D766361726401643322E310115F60331016033201603330111475D32E2F582D46312D424F4F4B4D41524B014C3426F6F6B6D61726B73015A463746578742F782D66312D626F6F6");
_LIT  (KDataComparison2_34, "B6D61726B01643312E3001162463746578742F782D66312D626F6F6B6D61726B01643312E300115F6033101603320160333011145463746578742F782D7663616C656E64617201583424547494E01633564341");
_LIT  (KDataComparison2_35, "4C454E4441520158356455253494F4E01633312E300158353554D4D41525901483636872015834445534352495054494F4E014836368720158343415445474F524945530148363687201583445453544152540");
_LIT  (KDataComparison2_36, "14836461746574696D65015834454454E44014836461746574696D650158341414C41524D014836461746574696D6501583434C41535301483636872015835252554C450148363687201583454E44016335643");
_LIT  (KDataComparison2_37, "414C454E44415201463746578742F782D766361726401583424547494E0163356434152440158356455253494F4E01633322E31015834E0148363687201583464E01483636872015835449544C450148363687");
_LIT  (KDataComparison2_38, "2015834F5247014836368720158354454C0148363687201573564F4943450148363687201573464158014836368720157343454C4C0148363687201573574F524B0148363687201573484F4D45014836368720");
_LIT  (KDataComparison2_39, "1583454D41494C014836368720158355524C01483636872015834E4F54450148363687201583434C4153530148363687201583454E4401633564341524401463746578742F782D66312D626F6F6B6D61726B01");
_LIT  (KDataComparison2_40, "583424547494E016334631424F4F4B4D41524B015834E414D45014836368720158355524C01483636872015834445534352495054494F4E01483636872015834C4153545649534954454444415445014836461");
_LIT  (KDataComparison2_41, "746574696D6501583454E44016334631424F4F4B4D41524B0111</Data></Item></Results><Final xmlns=\'syncml:syncml1.1\'></Final></SyncBody></SyncML>");


_LIT  (KDataComparison3_1, "<SyncML xmlns=\'syncml:syncml1.1\'><SyncHdr xmlns=\'syncml:syncml1.1\'><VerDTD xmlns=\'syncml:syncml1.1\'>1.1</VerDTD><VerProto xmlns=\'syncml:syncml1.1\'>SyncML/1.1</VerProto>");
_LIT  (KDataComparison3_2, "<SessionID xmlns=\'syncml:syncml1.1\'>33459702</SessionID><MsgID xmlns=\'syncml:syncml1.1\'>1</MsgID><Target xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_3, "<LocURI xmlns=\'syncml:syncml1.1\'>63158186431599000</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_4, "<LocURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.90/servlet/fde.sync.SyncML</LocURI></Source>");
_LIT  (KDataComparison3_5, "<RespURI xmlns=\'syncml:syncml1.1\'>http://208.184.103.90/servlet/fde.sync.SyncML?f1sess=194.200.144.243fuse10518377320</RespURI><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_6, "<MaxMsgSize xmlns=\'syncml:metinf\'>12288</MaxMsgSize></Meta></SyncHdr><SyncBody xmlns=\'syncml:syncml1.1\'><Status xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_7, "<CmdID xmlns=\'syncml:syncml1.1\'>1</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>0</CmdRef>");
_LIT  (KDataComparison3_8, "<Cmd xmlns=\'syncml:syncml1.1\'>SyncHdr</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>http://208.184.103.90/servlet/fde.sync.SyncML</TargetRef>");
_LIT  (KDataComparison3_9, "<SourceRef xmlns=\'syncml:syncml1.1\'>63158186431599000</SourceRef><Data xmlns=\'syncml:syncml1.1\'>212</Data></Status><Status xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_10, "<CmdID xmlns=\'syncml:syncml1.1\'>2</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef><CmdRef xmlns=\'syncml:syncml1.1\'>1</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Put</Cmd>");
_LIT  (KDataComparison3_11, "<Data xmlns=\'syncml:syncml1.1\'>200</Data></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>3</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison3_12, "<CmdRef xmlns=\'syncml:syncml1.1\'>2</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Get</Cmd><SourceRef xmlns=\'syncml:syncml1.1\'>./devinf11</SourceRef>");
_LIT  (KDataComparison3_13, "<Data xmlns=\'syncml:syncml1.1\'>200</Data></Status><Status xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>4</CmdID><MsgRef xmlns=\'syncml:syncml1.1\'>1</MsgRef>");
_LIT  (KDataComparison3_14, "<CmdRef xmlns=\'syncml:syncml1.1\'>3</CmdRef><Cmd xmlns=\'syncml:syncml1.1\'>Alert</Cmd><TargetRef xmlns=\'syncml:syncml1.1\'>./bookmarks</TargetRef>");
_LIT  (KDataComparison3_15, "<SourceRef xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</SourceRef><Data xmlns=\'syncml:syncml1.1\'>200</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_16, "<Data xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'><Next xmlns=\'syncml:metinf\'>20010607T125033Z</Next></Anchor></Data></Item></Status>");
_LIT  (KDataComparison3_17, "<Alert xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>5</CmdID><Data xmlns=\'syncml:syncml1.1\'>201</Data><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_18, "<Target xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat?from_server</LocURI></Target><Source xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_19, "<LocURI xmlns=\'syncml:syncml1.1\'>./bookmarks</LocURI></Source><Meta xmlns=\'syncml:syncml1.1\'><Anchor xmlns=\'syncml:metinf\'>");
_LIT  (KDataComparison3_20, "<Last xmlns=\'syncml:metinf\'>20010522T090813</Last><Next xmlns=\'syncml:metinf\'>20010607T125258</Next></Anchor></Meta></Item></Alert><Results xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_21, "<CmdID xmlns=\'syncml:syncml1.1\'>6</CmdID><CmdRef xmlns=\'syncml:syncml1.1\'>2</CmdRef><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_22, "<Type xmlns=\'syncml:metinf\'>application/vnd.syncml-devinf+xml</Type></Meta><TargetRef xmlns=\'syncml:syncml1.1\'>./devinf11</TargetRef><Item xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_23, "<Source xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>./devinf11</LocURI></Source><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_24, "<Type xmlns=\'syncml:metinf\'>application/vnd.syncml-devinf+xml</Type></Meta><Data xmlns=\'syncml:syncml1.1\'>2006A1D2D2F2F53594E434D4C2F2F44544420446576496E6620312E312");
_LIT  (KDataComparison3_25, "F2F454E4A653312E3101513667573696F6E4F6E652C20496E632E0155353796E634D4C20464445015E3312E300149346312053796E634D4C205346444520312E30014B373657276657201475D32E2F43616C");
_LIT  (KDataComparison3_26, "656E646172014C343616C656E646172015A463746578742F782D7663616C656E64617201643312E3001162463746578742F782D7663616C656E64617201643312E300115F6033101603320111475D32E2F43");
_LIT  (KDataComparison3_27, "6F6E7461637473014C3436F6E7461637473015A463746578742F782D766361726401643322E3101162463746578742F782D766361726401643322E310115F6033101603320111475D32E2F426F6F6B6D6172");
_LIT  (KDataComparison3_28, "6B73014C3426F6F6B6D61726B73015A463746578742F546573745479706501643312E3001162463746578742F546573745479706501643312E300115F603310160332011145463746578742F782D7663616C");
_LIT  (KDataComparison3_29, "656E64617201583424547494E016335643414C454E4441520158356455253494F4E01633312E300158353554D4D41525901483636872015834445534352495054494F4E014836368720158343415445474F5");
_LIT  (KDataComparison3_30, "2494553014836368720158344545354415254014836461746574696D65015834454454E44014836461746574696D650158341414C41524D014836461746574696D6501583434C41535301483636872015834");
_LIT  (KDataComparison3_31, "54E44016335643414C454E44415201463746578742F782D766361726401583424547494E0163356434152440158356455253494F4E01633322E31015834E0148363687201583464E01483636872015835449");
_LIT  (KDataComparison3_32, "544C4501483636872015834F5247014836368720158354454C0148363687201573564F4943450148363687201573464158014836368720157343454C4C0148363687201573574F524B014836368720157348");
_LIT  (KDataComparison3_33, "4F4D450148363687201583454D41494C014836368720158355524C01483636872015834E4F54450148363687201583434C4153530148363687201583454E4401633564341524401463746578742F782D6631");
_LIT  (KDataComparison3_34, "2D626F6F6B6D61726B01583424547494E016334631424F4F4B4D41524B015834E414D45014836368720158355524C01483636872015834445534352495054494F4E01483636872015834C415354564953495");
_LIT  (KDataComparison3_35, "4454444415445014836461746574696D6501583454E44016334631424F4F4B4D41524B0111</Data></Item></Results><Sync xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_36, "<CmdID xmlns=\'syncml:syncml1.1\'>7</CmdID><Target xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>C:\\System\\Data\\SyncML\\bookmarks1.dat</LocURI></Target>");
_LIT  (KDataComparison3_37, "<Source xmlns=\'syncml:syncml1.1\'><LocURI xmlns=\'syncml:syncml1.1\'>./bookmarks</LocURI></Source><Atomic xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_38, "<CmdID xmlns=\'syncml:syncml1.1\'>50</CmdID><Add xmlns=\'syncml:syncml1.1\'><CmdID xmlns=\'syncml:syncml1.1\'>8</CmdID><Meta xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_39, "<Type xmlns=\'syncml:metinf\'>text/x-f1-bookmark</Type></Meta><Item xmlns=\'syncml:syncml1.1\'><Source xmlns=\'syncml:syncml1.1\'>");
_LIT  (KDataComparison3_40, "<LocURI xmlns=\'syncml:syncml1.1\'>fd279764-1dd1-11b2-80eb-ff2b70e4fe89</LocURI></Source>");
_LIT  (KDataComparison3_41, "<Data xmlns=\'syncml:syncml1.1\'>BEGIN:F1-BOOKMARK\r\nVERSION:1.0\r\nNAME:Server\'s Bookmark\r\nURL:www.servers-place.com\r\nEND:F1-BOOKMARK\r\n</Data></Item></Add>");
_LIT  (KDataComparison3_42, "</Atomic></Sync><Final xmlns=\'syncml:syncml1.1\'></Final></SyncBody></SyncML>");

_LIT  (KDataComparison4_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><head xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><meta xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"Felix\" content=\"jees\"></meta></head>");
_LIT  (KDataComparison4_2, "<template xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' onenterbackward=\"#card1\" ontimer=\"#card1\">");
_LIT  (KDataComparison4_3, "<do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"accept\" label=\"template do\" name=\"doName\" optional=\"true\" xml:lang=\"lang\"><prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></prev></do>");
_LIT  (KDataComparison4_4, "<onevent xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"onenterforward\"><refresh xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></refresh></onevent></template>");
_LIT  (KDataComparison4_5, "<card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card1\" class=\"cards\" title=\"Title\" newcontext=\"true\" ordered=\"true\" xml:lang=\"english\">");
_LIT  (KDataComparison4_6, "<onevent xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"ontimer\"><noop xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></noop></onevent>");
_LIT  (KDataComparison4_7, "<onevent xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"onenterbackward\"><go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#card2\" sendreferer=\"false\" method=\"get\" accept-charset=\"charset\">");
_LIT  (KDataComparison4_8, "</go></onevent><timer xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"clock\" value=\"100\"></timer><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison4_9, "<fieldset xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"fs\"> #PCDATA <em xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>ephasized</em><strong xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>strong</strong>");
_LIT  (KDataComparison4_10, "<b xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><i xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><u xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> Biu </u></i></b></fieldset>");
_LIT  (KDataComparison4_11, "<big xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> big </big><small xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> tiny </small><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison4_12, "<img xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' alt=\"alternative \" src=\"pic\" localsrc=\"localPic\" vspace=\"10\" hspace=\"20\" align=\"middle\" height=\"1000\" width=\"200\" xml:lang=\"lang\"></img>");
_LIT  (KDataComparison4_13, "<anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"other.document\" method=\"post\">");
_LIT  (KDataComparison4_14, "<postfield xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"data\" value=\"value\"></postfield></go></anchor><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#card2\">link</a>");
_LIT  (KDataComparison4_15, "<table xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"Table\" align=\"center\" column=\"1\"><tr xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><td xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> Stuff </td>");
_LIT  (KDataComparison4_16, "</tr></table><input xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"var\" type=\"password\" value=\"def\" format=\"*M\" emptyok=\"true\" size=\"20\" maxlength=\"100\" tabindex=\"3\" title=\"input\"></input>");
_LIT  (KDataComparison4_17, "<select xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><option xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' value=\"1\" title=\"first\" onpick=\"#card2\"> Yeah </option>");
_LIT  (KDataComparison4_18, "<optgroup xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"Group1\"><option xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' value=\"2\"> Yeah <onevent xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"onpick\">");
_LIT  (KDataComparison4_19, "<refresh xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></refresh></onevent></option></optgroup></select></p><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"prev\">");
_LIT  (KDataComparison4_20, "<prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><setvar xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"variable\" value=\"varValue\"></setvar></prev></do></card></wml>");

_LIT  (KDataComparison5_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card1\" title=\"Character entitie$\">");
_LIT  (KDataComparison5_2, "<onevent xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"onenterforward\"><refresh xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison5_3, "<setvar xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"var\" value=\"\" & \' < >   ­ $\"></setvar></refresh></onevent>");
_LIT  (KDataComparison5_4, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> Quote: \"<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> Ampersand: &<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison5_5, "</br> Apos: \'<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> Lesser: <<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> Greater: >");
_LIT  (KDataComparison5_6, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> NBSP:  <br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> S-hyp: ­<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison5_7, "</br> $<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison5_8, "</br>[[0x82 :  Entities in attribute: var]]<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card></wml>");

_LIT  (KDataComparison6_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card1\" title=\"Crystal Demo\">");
_LIT  (KDataComparison6_2, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"center\"><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison6_3, "</br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><big xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><b xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> Welcome</b>");
_LIT  (KDataComparison6_4, "</big><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> to the<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><big xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison6_5, "<b xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Mib\'s World!</b></big><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison6_6, "<do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"accept\" label=\"Next\"><go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#card2\"></go></do></p></card>");
_LIT  (KDataComparison6_7, "<card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card2\" title=\"Next card\"><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"left\">");
_LIT  (KDataComparison6_8, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><em xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Well, that\'s pretty much it.</em>");
_LIT  (KDataComparison6_9, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison6_10, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"file://c|/wml-events-do-7.wml\"></go></anchor><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#card1\">Prev card</a>");
_LIT  (KDataComparison6_11, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"prev\" label=\"Go back\">");
_LIT  (KDataComparison6_12, "<prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></prev></do></p><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison6_13, "<img xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' alt=\"image\" src=\"demo.mbm\" align=\"bottom\"></img><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card></wml>");

_LIT  (KDataComparison7_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><head xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><meta xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' http-equiv=\"Cache-Control\" content=\"max-age=300\"></meta></head>");
_LIT  (KDataComparison7_2, "<card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"options\" label=\"BBC Homepage\"><go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"mainmenu.wml\"></go></do>");
_LIT  (KDataComparison7_3, "<do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"prev\" label=\"Back\"><prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></prev></do><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison7_4, "<b xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Sport Headlines</b><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison7_5, "<small xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>at 14:45 on 31/5/2000</small><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison7_6, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"TOP STORIES\" href=\"storiesheads1.wml\">TOP STORIES</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_7, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"FOOTBALL\" href=\"footballheads1.wml\">FOOTBALL</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_8, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"GOLF\" href=\"golfheads1.wml\">GOLF</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_9, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"CRICKET\" href=\"cricketheads1.wml\">CRICKET</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_10, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"MOTORSPORT\" href=\"motorsportheads1.wml\">MOTORSPORT</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_11, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"RUGBY UNION\" href=\"rugbyunionheads1.wml\">RUGBY UNION</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_12, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"RUGBY LEAGUE\" href=\"rugbyleagueheads1.wml\">RUGBY LEAGUE</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_13, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"TENNIS\" href=\"tennisheads1.wml\">TENNIS</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison7_14, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"SNOOKER\" href=\"snookerheads1.wml\">SNOOKER</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p>");
_LIT  (KDataComparison7_15, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"center\"><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"SportAlert\" href=\"http://bbc.co.uk/cgi-bin/alert/prgterse.pl?subject=18 \">BBC Sport programmes Alert</a></p>");
_LIT  (KDataComparison7_16, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"center\"><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"Sports Headlines\" href=\"sportheads1.wml\">Sports Headlines</a></p>");
_LIT  (KDataComparison7_17, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"center\"><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"BBC Homepage\" href=\"mainmenu.wml\">BBC Homepage</a></p></card></wml>");

_LIT  (KDataComparison8_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><template xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"options\" label=\"iii.co.uk home\">");
_LIT  (KDataComparison8_2, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/index.wml#index\"></go></do><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"prev\" name=\"Back\" label=\"back\">");
_LIT  (KDataComparison8_3, "<prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></prev></do></template><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card1\" title=\"Quote/Graph\"><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison8_4, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>1. Enter Stock Code<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#quotes\"></go></anchor>");
_LIT  (KDataComparison8_5, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>2. Name Search<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#namesearch\"></go></anchor></p>");
_LIT  (KDataComparison8_6, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>_____________<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"go..\">");
_LIT  (KDataComparison8_7, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/#index\"></go>Main Menu</anchor><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"go..\">");
_LIT  (KDataComparison8_8, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/.wml\"></go>Help</anchor><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card>");
_LIT  (KDataComparison8_9, "<card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"quotes\" title=\"Quote/Graph\"><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Enter Stock Code<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison8_10, "<input xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"text\" name=\"s\"></input>[[0x80 : price.wml?s=s]]<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"\">Get Quote</a>");
_LIT  (KDataComparison8_11, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>[[0x80 : graph.wml?s=s]]<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"\">Get 12 month</a>");
_LIT  (KDataComparison8_12, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>[[0x80 : graph.wml?s=s]]<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"&p=1d\">Get Intraday</a></p>");
_LIT  (KDataComparison8_13, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>_____________<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"go..\">");
_LIT  (KDataComparison8_14, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/#index\"></go>Main Menu</anchor><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"go..\">");
_LIT  (KDataComparison8_15, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/.wml\"></go>Help</anchor><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card>");
_LIT  (KDataComparison8_16, "<card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"namesearch\" title=\"Name Search\"><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Enter search term:");
_LIT  (KDataComparison8_17, "<input xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"text\" name=\"n\"></input>[[0x80 : name.wml?n=n]]<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"\">Search</a></p>");
_LIT  (KDataComparison8_18, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>_____________<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"go..\">");
_LIT  (KDataComparison8_19, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/#index\"></go>Main Menu</anchor><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' title=\"go..\">");
_LIT  (KDataComparison8_20, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"/.wml\"></go>Help</anchor><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card></wml>");

_LIT  (KDataComparison9_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card1\" title=\"Title\"><onevent xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"onenterforward\">");
_LIT  (KDataComparison9_2, "<refresh xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><setvar xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' name=\"var\" value=\"VarVal\"></setvar></refresh></onevent>");
_LIT  (KDataComparison9_3, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>[[0x82 :  Plain variable: var]]<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>[[0x82 :  NoEsc: var]]<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison9_4, "</br>[[0x80 :  Escape: var]]<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>[[0x81 :  UnEsc: var]]<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card></wml>");

_LIT  (KDataComparison10_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><template xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"accept\" name=\"Prev\" label=\"Back\">");
_LIT  (KDataComparison10_2, "<prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></prev></do><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"options\" name=\"Home\" label=\"Home\">");
_LIT  (KDataComparison10_3, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://wap.rtsnetworks\"></go></do></template><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"index\" ontimer=\"#menu\">");
_LIT  (KDataComparison10_4, "<timer xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' value=\"20\"></timer><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison10_5, "<img xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' src=\"images/RTSe_slogan.wbmp\" alt=\"Welcome !\" align=\"middle\"></img></p></card>");
_LIT  (KDataComparison10_6, "<card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"menu\" title=\"RTSe WAP demos\"><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>");
_LIT  (KDataComparison10_7, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://193.94.194.178/RouteWAP/\">» RouteWAP</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_8, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://193.94.194.178/OutlookBridge/start.asp\">» Outlook Bridge</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_9, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://193.94.194.165/wap_demos.wml#card1\">» HKL Timetables</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_10, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://sodium.rtsnetworks.com:8080/apps/ytv/\">» YTV Timetables</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_11, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://193.94.194.165/wap_demos.wml#card2\">» Wireless Investor</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_12, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"http://193.94.194.165/hangman/hangman.wml\">» HangMan</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_13, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"contact_us.wml\">Contact Us</a><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison10_14, "<a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"whats_new.wml\">What\'s New</a></p></card></wml>");

_LIT  (KDataComparison11_1, "<wml xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' xmlns=\'-//WAPFORUM//DTD WML 1.1 ATTRIBUTE SWITCH TEST//EN\' id=\"card1\" TestAttribute=\"Crystal Demo1\" TestAttribute=\"Crystal Demo2\">");
_LIT  (KDataComparison11_2, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"center\"><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison11_3, "<big xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><b xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'> Welcome</b></big><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br> to the<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison11_4, "<big xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><b xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Mib\'s World!</b></big><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"accept\" label=\"Next\">");
_LIT  (KDataComparison11_5, "<go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#card2\"></go></do></p></card><card xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' id=\"card2\" title=\"Next card\"><p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' align=\"left\">");
_LIT  (KDataComparison11_6, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><em xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'>Well, that\'s pretty much it.</em><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br>");
_LIT  (KDataComparison11_7, "<anchor xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><go xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"file://c|/wml-events-do-7.wml\"></go></anchor><a xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' href=\"#card1\">Prev card</a>");
_LIT  (KDataComparison11_8, "<br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br><do xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' type=\"prev\" label=\"Go back\"><prev xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></prev></do></p>");
_LIT  (KDataComparison11_9, "<p xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'><img xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\' alt=\"image\" src=\"demo.mbm\" align=\"bottom\"></img><br xmlns=\'-//WAPFORUM//DTD WML 1.1//EN\'></br></p></card></wml>");


static RTest test(_L("t_wbxmlparsertests"));


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3726
@SYMTestCaseDesc		    Parsing wml documents.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parse wml documents and compare the output with the expected output.
@SYMTestExpectedResults 	Parsed output and expected output should match.
@SYMREQ 		 		 	REQ0000
*/
static void ParserTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3726 ParserTestsL tests... "));

	__UHEAP_MARK;

	TBuf16<8000> *expectedOutput = new(ELeave) TBuf16<8000>;
	CleanupStack::PushL(expectedOutput);

	CStabilityTestClass* parserTest = CStabilityTestClass::NewL(test, EFalse, KMaxChunkSize);
	CleanupStack::PushL(parserTest);

	parserTest->iOutput->Zero();

// Test0

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison0_1);
	expectedOutput->Append(KDataComparison0_2);
	expectedOutput->Append(KDataComparison0_3);
	expectedOutput->Append(KDataComparison0_4);
	expectedOutput->Append(KDataComparison0_5);
	expectedOutput->Append(KDataComparison0_6);
	expectedOutput->Append(KDataComparison0_7);
	expectedOutput->Append(KDataComparison0_8);
	expectedOutput->Append(KDataComparison0_9);
	expectedOutput->Append(KDataComparison0_10);
	expectedOutput->Append(KDataComparison0_11);
	expectedOutput->Append(KDataComparison0_12);
	expectedOutput->Append(KDataComparison0_13);
	expectedOutput->Append(KDataComparison0_14);
	expectedOutput->Append(KDataComparison0_15);
	expectedOutput->Append(KDataComparison0_16);
	expectedOutput->Append(KDataComparison0_17);
	expectedOutput->Append(KDataComparison0_18);
	expectedOutput->Append(KDataComparison0_19);
	expectedOutput->Append(KDataComparison0_20);
	expectedOutput->Append(KDataComparison0_21);
	expectedOutput->Append(KDataComparison0_22);

	parserTest->ParseEntryL(KTestDocument0(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument0(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test1

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison1_1);
	expectedOutput->Append(KDataComparison1_2);
	expectedOutput->Append(KDataComparison1_3);
	expectedOutput->Append(KDataComparison1_4);
	expectedOutput->Append(KDataComparison1_5);
	expectedOutput->Append(KDataComparison1_6);
	expectedOutput->Append(KDataComparison1_7);
	expectedOutput->Append(KDataComparison1_8);
	expectedOutput->Append(KDataComparison1_9);
	expectedOutput->Append(KDataComparison1_10);
	expectedOutput->Append(KDataComparison1_11);
	expectedOutput->Append(KDataComparison1_12);
	expectedOutput->Append(KDataComparison1_13);
	expectedOutput->Append(KDataComparison1_14);
	expectedOutput->Append(KDataComparison1_15);
	expectedOutput->Append(KDataComparison1_16);
	expectedOutput->Append(KDataComparison1_17);
	expectedOutput->Append(KDataComparison1_18);
	expectedOutput->Append(KDataComparison1_19);
	expectedOutput->Append(KDataComparison1_20);
	expectedOutput->Append(KDataComparison1_21);
	expectedOutput->Append(KDataComparison1_22);

	parserTest->ParseEntryL(KTestDocument1(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument1(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test2

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison2_1);
	expectedOutput->Append(KDataComparison2_2);
	expectedOutput->Append(KDataComparison2_3);
	expectedOutput->Append(KDataComparison2_4);
	expectedOutput->Append(KDataComparison2_5);
	expectedOutput->Append(KDataComparison2_6);
	expectedOutput->Append(KDataComparison2_7);
	expectedOutput->Append(KDataComparison2_8);
	expectedOutput->Append(KDataComparison2_9);
	expectedOutput->Append(KDataComparison2_10);
	expectedOutput->Append(KDataComparison2_11);
	expectedOutput->Append(KDataComparison2_12);
	expectedOutput->Append(KDataComparison2_13);
	expectedOutput->Append(KDataComparison2_14);
	expectedOutput->Append(KDataComparison2_15);
	expectedOutput->Append(KDataComparison2_16);
	expectedOutput->Append(KDataComparison2_17);
	expectedOutput->Append(KDataComparison2_18);
	expectedOutput->Append(KDataComparison2_19);
	expectedOutput->Append(KDataComparison2_20);
	expectedOutput->Append(KDataComparison2_21);
	expectedOutput->Append(KDataComparison2_22);
	expectedOutput->Append(KDataComparison2_23);
	expectedOutput->Append(KDataComparison2_24);
	expectedOutput->Append(KDataComparison2_25);
	expectedOutput->Append(KDataComparison2_26);
	expectedOutput->Append(KDataComparison2_27);
	expectedOutput->Append(KDataComparison2_28);
	expectedOutput->Append(KDataComparison2_29);
	expectedOutput->Append(KDataComparison2_30);
	expectedOutput->Append(KDataComparison2_31);
	expectedOutput->Append(KDataComparison2_32);
	expectedOutput->Append(KDataComparison2_33);
	expectedOutput->Append(KDataComparison2_34);
	expectedOutput->Append(KDataComparison2_35);
	expectedOutput->Append(KDataComparison2_36);
	expectedOutput->Append(KDataComparison2_37);
	expectedOutput->Append(KDataComparison2_38);
	expectedOutput->Append(KDataComparison2_39);
	expectedOutput->Append(KDataComparison2_40);
	expectedOutput->Append(KDataComparison2_41);

	parserTest->ParseEntryL(KTestDocument2(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument2(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test3

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison3_1);
	expectedOutput->Append(KDataComparison3_2);
	expectedOutput->Append(KDataComparison3_3);
	expectedOutput->Append(KDataComparison3_4);
	expectedOutput->Append(KDataComparison3_5);
	expectedOutput->Append(KDataComparison3_6);
	expectedOutput->Append(KDataComparison3_7);
	expectedOutput->Append(KDataComparison3_8);
	expectedOutput->Append(KDataComparison3_9);
	expectedOutput->Append(KDataComparison3_10);
	expectedOutput->Append(KDataComparison3_11);
	expectedOutput->Append(KDataComparison3_12);
	expectedOutput->Append(KDataComparison3_13);
	expectedOutput->Append(KDataComparison3_14);
	expectedOutput->Append(KDataComparison3_15);
	expectedOutput->Append(KDataComparison3_16);
	expectedOutput->Append(KDataComparison3_17);
	expectedOutput->Append(KDataComparison3_18);
	expectedOutput->Append(KDataComparison3_19);
	expectedOutput->Append(KDataComparison3_20);
	expectedOutput->Append(KDataComparison3_21);
	expectedOutput->Append(KDataComparison3_22);
	expectedOutput->Append(KDataComparison3_23);
	expectedOutput->Append(KDataComparison3_24);
	expectedOutput->Append(KDataComparison3_25);
	expectedOutput->Append(KDataComparison3_26);
	expectedOutput->Append(KDataComparison3_27);
	expectedOutput->Append(KDataComparison3_28);
	expectedOutput->Append(KDataComparison3_29);
	expectedOutput->Append(KDataComparison3_30);
	expectedOutput->Append(KDataComparison3_31);
	expectedOutput->Append(KDataComparison3_32);
	expectedOutput->Append(KDataComparison3_33);
	expectedOutput->Append(KDataComparison3_34);
	expectedOutput->Append(KDataComparison3_35);
	expectedOutput->Append(KDataComparison3_36);
	expectedOutput->Append(KDataComparison3_37);
	expectedOutput->Append(KDataComparison3_38);
	expectedOutput->Append(KDataComparison3_39);
	expectedOutput->Append(KDataComparison3_40);
	expectedOutput->Append(KDataComparison3_41);
	expectedOutput->Append(KDataComparison3_42);

	parserTest->ParseEntryL(KTestDocument3(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument3(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test4

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison4_1);
	expectedOutput->Append(KDataComparison4_2);
	expectedOutput->Append(KDataComparison4_3);
	expectedOutput->Append(KDataComparison4_4);
	expectedOutput->Append(KDataComparison4_5);
	expectedOutput->Append(KDataComparison4_6);
	expectedOutput->Append(KDataComparison4_7);
	expectedOutput->Append(KDataComparison4_8);
	expectedOutput->Append(KDataComparison4_9);
	expectedOutput->Append(KDataComparison4_10);
	expectedOutput->Append(KDataComparison4_11);
	expectedOutput->Append(KDataComparison4_12);
	expectedOutput->Append(KDataComparison4_13);
	expectedOutput->Append(KDataComparison4_14);
	expectedOutput->Append(KDataComparison4_15);
	expectedOutput->Append(KDataComparison4_16);
	expectedOutput->Append(KDataComparison4_17);
	expectedOutput->Append(KDataComparison4_18);
	expectedOutput->Append(KDataComparison4_19);
	expectedOutput->Append(KDataComparison4_20);

	parserTest->ParseEntryL(KTestDocument4(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument4(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test5

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison5_1);
	expectedOutput->Append(KDataComparison5_2);
	expectedOutput->Append(KDataComparison5_3);
	expectedOutput->Append(KDataComparison5_4);
	expectedOutput->Append(KDataComparison5_5);
	expectedOutput->Append(KDataComparison5_6);
	expectedOutput->Append(KDataComparison5_7);
	expectedOutput->Append(KDataComparison5_8);

	parserTest->ParseEntryL(KTestDocument5(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument5(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test6

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison6_1);
	expectedOutput->Append(KDataComparison6_2);
	expectedOutput->Append(KDataComparison6_3);
	expectedOutput->Append(KDataComparison6_4);
	expectedOutput->Append(KDataComparison6_5);
	expectedOutput->Append(KDataComparison6_6);
	expectedOutput->Append(KDataComparison6_7);
	expectedOutput->Append(KDataComparison6_8);
	expectedOutput->Append(KDataComparison6_9);
	expectedOutput->Append(KDataComparison6_10);
	expectedOutput->Append(KDataComparison6_11);
	expectedOutput->Append(KDataComparison6_12);
	expectedOutput->Append(KDataComparison6_13);

	parserTest->ParseEntryL(KTestDocument6(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument6(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test7

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison7_1);
	expectedOutput->Append(KDataComparison7_2);
	expectedOutput->Append(KDataComparison7_3);
	expectedOutput->Append(KDataComparison7_4);
	expectedOutput->Append(KDataComparison7_5);
	expectedOutput->Append(KDataComparison7_6);
	expectedOutput->Append(KDataComparison7_7);
	expectedOutput->Append(KDataComparison7_8);
	expectedOutput->Append(KDataComparison7_9);
	expectedOutput->Append(KDataComparison7_10);
	expectedOutput->Append(KDataComparison7_11);
	expectedOutput->Append(KDataComparison7_12);
	expectedOutput->Append(KDataComparison7_13);
	expectedOutput->Append(KDataComparison7_14);
	expectedOutput->Append(KDataComparison7_15);
	expectedOutput->Append(KDataComparison7_16);
	expectedOutput->Append(KDataComparison7_17);

	parserTest->ParseEntryL(KTestDocument7(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument7(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test8

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison8_1);
	expectedOutput->Append(KDataComparison8_2);
	expectedOutput->Append(KDataComparison8_3);
	expectedOutput->Append(KDataComparison8_4);
	expectedOutput->Append(KDataComparison8_5);
	expectedOutput->Append(KDataComparison8_6);
	expectedOutput->Append(KDataComparison8_7);
	expectedOutput->Append(KDataComparison8_8);
	expectedOutput->Append(KDataComparison8_9);
	expectedOutput->Append(KDataComparison8_10);
	expectedOutput->Append(KDataComparison8_11);
	expectedOutput->Append(KDataComparison8_12);
	expectedOutput->Append(KDataComparison8_13);
	expectedOutput->Append(KDataComparison8_14);
	expectedOutput->Append(KDataComparison8_15);
	expectedOutput->Append(KDataComparison8_16);
	expectedOutput->Append(KDataComparison8_17);
	expectedOutput->Append(KDataComparison8_18);
	expectedOutput->Append(KDataComparison8_19);
	expectedOutput->Append(KDataComparison8_20);

	parserTest->ParseEntryL(KTestDocument8(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument8(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test9

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison9_1);
	expectedOutput->Append(KDataComparison9_2);
	expectedOutput->Append(KDataComparison9_3);
	expectedOutput->Append(KDataComparison9_4);

	parserTest->ParseEntryL(KTestDocument9(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument9(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test10

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison10_1);
	expectedOutput->Append(KDataComparison10_2);
	expectedOutput->Append(KDataComparison10_3);
	expectedOutput->Append(KDataComparison10_4);
	expectedOutput->Append(KDataComparison10_5);
	expectedOutput->Append(KDataComparison10_6);
	expectedOutput->Append(KDataComparison10_7);
	expectedOutput->Append(KDataComparison10_8);
	expectedOutput->Append(KDataComparison10_9);
	expectedOutput->Append(KDataComparison10_10);
	expectedOutput->Append(KDataComparison10_11);
	expectedOutput->Append(KDataComparison10_12);
	expectedOutput->Append(KDataComparison10_13);
	expectedOutput->Append(KDataComparison10_14);

	parserTest->ParseEntryL(KTestDocument10(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument10(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();


	// Test11

	expectedOutput->Zero();
	expectedOutput->Append(KDataComparison11_1);
	expectedOutput->Append(KDataComparison11_2);
	expectedOutput->Append(KDataComparison11_3);
	expectedOutput->Append(KDataComparison11_4);
	expectedOutput->Append(KDataComparison11_5);
	expectedOutput->Append(KDataComparison11_6);
	expectedOutput->Append(KDataComparison11_7);
	expectedOutput->Append(KDataComparison11_8);
	expectedOutput->Append(KDataComparison11_9);

	parserTest->ParseEntryL(KTestDocument11(), &CStabilityTestClass::TestWholeL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();

	parserTest->ParseEntryL(KTestDocument11(), &CStabilityTestClass::TestChunkL);
	test(*expectedOutput == *parserTest->iOutput);

	parserTest->iOutput->Zero();



	CleanupStack::PopAndDestroy(parserTest);
	CleanupStack::PopAndDestroy(expectedOutput);

	__UHEAP_MARKEND;
	}

static void MainL()
	{
	ParserTestsL();
	}

TInt E32Main()
	{

	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test (r==KErrNone);
	delete c;
	test.End();
	test.Close();
	__UHEAP_MARKEND;

	return KErrNone;
	}

