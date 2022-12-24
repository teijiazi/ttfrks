#include<tables1.h>
#include<QDateTime>

HEADTable::HEADTable(QObject *parent):TableTTF(parent),majorVersion(1),minorVersion(0),
    fontRevision(0x00010000),magicNumber(0x5F0F3CF5),macStyle(0),lowestRecPPEM(12),
    fontDirectionHint(2),indexToLocFormat(1),glyphDataFormat(0){}
void HEADTable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>majorVersion>>minorVersion
            >>fontRevision.value>>checksumAdjustment>>magicNumber>>flags>>unitsPerEm
            >>created>>modified>>xMin>>yMin>>xMax>>yMax>>macStyle>>lowestRecPPEM>>fontDirectionHint
            >>indexToLocFormat>>glyphDataFormat;
    setFontDate(created,unitsPerEm);
};
void HEADTable::save(QDataStream& dt){
    checksumAdjustment=0;
    dt<<majorVersion<<minorVersion
     <<fontRevision.value<<checksumAdjustment<<magicNumber<<flags<<unitsPerEm
    <<created<<modified<<xMin<<yMin<<xMax<<yMax<<macStyle<<lowestRecPPEM<<fontDirectionHint
    <<indexToLocFormat<<glyphDataFormat;
}
void HEADTable::jisuan(qint64 riqi1970,uint16 emsize,QString banben,
                       int16 vxMin,int16 vyMin,int16 vxMax,int16 vyMax,bool lsbAtY,int panose3){
    Q_UNUSED(banben);
    majorVersion=1;
    minorVersion=0;
    fontRevision=banben.toDouble();
    checksumAdjustment=0;
    magicNumber=0x5F0F3CF5;
    flags=lsbAtY?0x000b:0x0009;//00000000 00001011 00001001
    unitsPerEm=emsize;
    created=riqi1970+sec19040101to19700101;
    modified=created;
    xMin=vxMin;
    yMin=vyMin;
    xMax=vxMax;
    yMax=vyMax;
    macStyle=0;
    if(panose3==5|| panose3==7){//扩展
        macStyle=0x0040;//bit6 11111111 1101 1111
    }else if(panose3==6|| panose3==8){//紧缩
        macStyle=0x0020;//bit5
    }
    lowestRecPPEM=12;
    fontDirectionHint=2;
    indexToLocFormat=1;
    glyphDataFormat=0;
}
void HEADTable::show()
{
    QString flagx;
    for(int i=0;i<16;i++){
        flagx+=(flags&(int)pow(2,i))?"x ":"- ";
    }
    datashow(QString("majorVersion:%1\t\t[1]\r\n"
                     "minorVersion:%2\t\t[0]\r\n"
                     "fontRevision:%3\t\r\n"
                     "checksumAdjustment:%4\t\t\r\n"
                     "magicNumber:%5\r\n"
                     "flags:%6\t\t%22\r\n"
                     "    Bit 0: 基线在y=0.\t\t%23\r\n"
                     "    Bit 1: lsb在Y轴.\t\t%24\r\n"
                     "    Bit 2: 指令可能取决于点的大小\t%25\r\n"
                     "    Bit 3: 强制ppem为整数值\t%26\r\n"
                     "    Bit 4: 指令可能会改变前进宽度\t%27\r\n"
                     "    Bit 5: 未使用\t\t%28\r\n"
                     "    Bits 6–10: 未使用\t\t%29\r\n"
                     "    Bit 11: 无损字体\t\t%30\r\n"
                     "    Bit 12: 字体转换\t\t%31\r\n"
                     "    Bit 13: 为ClearType优化的字体\t%32\r\n"
                     "    Bit 14: 万不得已的字体\t%33\r\n"
                     "    Bit 15: 保留\t\t%34\r\n"
                     "unitsPerEm:%7\r\n"
                     "created:%8\t\t%19\r\n"
                     "modified:%9\t\t%20\r\n"
                     "xMin:%10\r\n"
                     "yMin:%11\r\n"
                     "xMax:%12\r\n"
                     "yMax:%13\r\n"
                     "macStyle:%14\r\n"
                     "    Bit 0: 黑体\t\t%35\r\n"
                     "    Bit 1: 斜体\t\t%36\r\n"
                     "    Bit 2: 下划线\t\t%37\r\n"
                     "    Bit 3: 轮廓\t\t%38\r\n"
                     "    Bit 4: 阴影\t\t%39\r\n"
                     "    Bit 5: 压缩\t\t%40\r\n"
                     "    Bit 6: 扩展\t\t%41\r\n"
                     "    Bits 7–15: 未使用\t\t%42\r\n"
                     "lowestRecPPEM:%15\r\n"
                     "fontDirectionHint:%16\t\t[2 弃用]\r\n"
                     "indexToLocFormat:%17\t\t%21\r\n"
                     "glyphDataFormat:%18\t\t[0]\r\n")
             .arg(majorVersion).arg(minorVersion).arg(fontRevision)
             .arg(checksumAdjustment,8,16,QChar('0')).arg(magicNumber,8,16,QChar('0'))
             .arg(flags,4,16,QChar('0')).arg(unitsPerEm)
             .arg(created).arg(modified).arg(xMin).arg(yMin).arg(xMax).arg(yMax)
             .arg(macStyle,4,16,QChar('0')).arg(lowestRecPPEM).arg(fontDirectionHint).arg(indexToLocFormat)
             .arg(glyphDataFormat)
             .arg(QDateTime::fromSecsSinceEpoch(created -sec19040101to19700101).toString("yyyy.MM.dd hh:mm:ss.zzz"))
             .arg(QDateTime::fromSecsSinceEpoch(modified-sec19040101to19700101).toString("yyyy.MM.dd hh:mm:ss.zzz"))
             .arg(indexToLocFormat==0?"Offset16":"Offset32")
             .arg(flagx)
             .arg(flags&0x0001?"✓":"×")//23
             .arg(flags&0x0002?"✓":"×")//24
             .arg(flags&0x0004?"✓":"×")//25
             .arg(flags&0x0008?"✓":"×")//26
             .arg(flags&0x0010?"✓":"×")//27
             .arg(flags&0x0020?"✓":"×")//28
             .arg(flags&0x07c0?"✓":"×")//29 00000111 11000000
             .arg(flags&0x0800?"✓":"×")//30
             .arg(flags&0x1000?"✓":"×")//31
             .arg(flags&0x2000?"✓":"×")//32
             .arg(flags&0x4000?"✓":"×")//33
             .arg(flags&0x8000?"✓":"×")//34
             .arg(macStyle&0x0001?"✓":"×")//35
             .arg(macStyle&0x0002?"✓":"×")//34
             .arg(macStyle&0x0004?"✓":"×")//34
             .arg(macStyle&0x0008?"✓":"×")//34
             .arg(macStyle&0x0010?"✓":"×")//34
             .arg(macStyle&0x0020?"✓":"×")//34
             .arg(macStyle&0x0040?"✓":"×")//34
             .arg(macStyle&0xff80?"✓":"×")//34 00000000 00000000
             );
}

HHEATable::HHEATable(QObject *parent):TableTTF(parent),majorVersion(1),minorVersion(0),
    reserved1(0),reserved2(0),reserved3(0),reserved4(0),metricDataFormat(0){}
void HHEATable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>majorVersion>>minorVersion>>ascender>>descender>>lineGap>>
            advanceWidthMax>>minLeftSideBearing>>minRightSideBearing>>
            xMaxExtent>>caretSlopeRise>>caretSlopeRun>>caretOffset>>
            reserved1>>reserved2>>reserved3>>reserved4>>metricDataFormat>>numberOfHMetrics;
};
void HHEATable::save(QDataStream& dt){
    dt<<majorVersion<<minorVersion<<ascender<<descender<<lineGap<<
        advanceWidthMax<<minLeftSideBearing<<minRightSideBearing<<
        xMaxExtent<<caretSlopeRise<<caretSlopeRun<<caretOffset<<
        reserved1<<reserved2<<reserved3<<reserved4<<metricDataFormat<<numberOfHMetrics;
    //不需要补零
}
void HHEATable::show(){
    datashow(QString("majorVersion:%1\t\t[1]\r\n"
                     "minorVersion:%2\t\t[0]\r\n"
                     "ascender:%3\t\t[苹果规范]\r\n"
                     "descender:%4\t\t[苹果规范]\r\n"
                     "lineGap:%5\t\t[苹果规范]\r\n"
                     "advanceWidthMax:%6\r\n"
                     "minLeftSideBearing:%7\r\n"
                     "minRightSideBearing:%8\t\t\r\n"
                     "xMaxExtent:%9\t\t\r\n"
                     "caretSlopeRise:%10\t\t[1]\r\n"
                     "caretSlopeRun:%11\t\t[0]\r\n"
                     "caretOffset:%12\t\t[0]\r\n"
                     "metricDataFormat:%13\t\t[0]\r\n"
                     "numberOfHMetrics:%14\r\n"
                     )
             .arg(majorVersion).arg(minorVersion).arg(ascender,6)
             .arg(descender,5).arg(lineGap,7)
             .arg(advanceWidthMax).arg(minLeftSideBearing)
             .arg(minRightSideBearing).arg(xMaxExtent).arg(caretSlopeRise)
             .arg(caretSlopeRun).arg(caretOffset).arg(metricDataFormat)
             .arg(numberOfHMetrics));
}
void HHEATable::jisuan(UFWORD vAdvanceWidthMax,uint16 noh,FWORD xMaxExt,
                       FWORD vMinLsb, FWORD vMinRsb,int16 vAscent,int16 vDscent,int16 sGap){
    majorVersion=1;
    minorVersion=0;
    ascender=vAscent;
    descender=vDscent;
    lineGap=sGap;
    advanceWidthMax=vAdvanceWidthMax;
    minLeftSideBearing=vMinLsb;
    minRightSideBearing=vMinRsb;
    xMaxExtent=xMaxExt;
    caretSlopeRise=1;caretSlopeRun=0;caretOffset=0;
    reserved1=reserved2=reserved3=reserved4=0;
    metricDataFormat=0;
    numberOfHMetrics=noh;
}

MAXPTable::MAXPTable(QObject *parent):TableTTF(parent),numGlyphs(0){}
void MAXPTable::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>version.value>>numGlyphs>>maxPoints>>maxContours>>maxCompositePoints>>
            maxCompositeContours>>maxZones>>maxTwilightPoints>>maxStorage>>
            maxFunctionDefs>>maxInstructionDefs>>maxStackElements>>
            maxSizeOfInstructions>>maxComponentElements>>maxComponentDepth;
};
void MAXPTable::save(QDataStream& dt){
    dt<<version.value<<numGlyphs<<maxPoints<<maxContours<<maxCompositePoints<<
        maxCompositeContours<<maxZones<<maxTwilightPoints<<maxStorage<<
        maxFunctionDefs<<maxInstructionDefs<<maxStackElements<<
        maxSizeOfInstructions<<maxComponentElements<<maxComponentDepth;
    //不需要补零
}
void MAXPTable::show(){
    datashow(QString("version:%1\t\t[1.0]\r\n"
                     "numGlyphs:%2\t\t\r\n"
                     "maxPoints:%3\t\r\n"
                     "maxContours:%4\t\t\r\n"
                     "maxCompositePoints:%5\r\n"
                     "maxCompositeContours:%6\r\n"
                     "maxZones:%7\r\n"
                     "maxTwilightPoints:%8\t\t\r\n"
                     "maxStorage:%9\t\t\r\n"
                     "maxFunctionDefs:%10\r\n"
                     "maxInstructionDefs:%11\r\n"
                     "maxStackElements:%12\r\n"
                     "maxSizeOfInstructions:%13\r\n"
                     "maxComponentElements:%14\r\n"
                     "maxComponentDepth:%15\r\n")
             .arg((QString)version).arg(numGlyphs).arg(maxPoints)
             .arg(maxContours).arg(maxCompositePoints)
             .arg(maxCompositeContours).arg(maxZones)
             .arg(maxTwilightPoints).arg(maxStorage).arg(maxFunctionDefs)
             .arg(maxInstructionDefs).arg(maxStackElements).arg(maxSizeOfInstructions)
             .arg(maxComponentElements).arg(maxComponentDepth));
}
void MAXPTable::jisuan(uint16 tuxingZongshu,
                       uint16 jdLk,uint16 jdDian,uint16 fzLk,uint16 fzDian){
    numGlyphs=tuxingZongshu;
    version.value=0x00010000;//truetype
    maxPoints=jdDian;
    maxContours=jdLk;
    maxCompositePoints=fzDian;
    maxCompositeContours=fzLk;
    maxZones=1;//没有使用指令
    maxTwilightPoints=0;
    maxStorage=0;//
    maxFunctionDefs=0;//
    maxInstructionDefs=0;
    maxStackElements=0;//
    maxSizeOfInstructions=0;
    maxComponentElements=0;
    maxComponentDepth=0;
}


OS2Table::OS2Table(QObject *parent):TableTTF(parent){}
void OS2Table::read(QDataStream& dt, uint32 offset){
    dt.device()->seek(offset);
    dt>>version>>xAvgCharWidth>>usWeightClass>>usWidthClass>>fsType>>
            ySubscriptXSize>>ySubscriptYSize>>ySubscriptXOffset>>
            ySubscriptYOffset>>ySuperscriptXSize>>ySuperscriptYSize>>
            ySuperscriptXOffset>>ySuperscriptYOffset>>yStrikeoutSize>>
            yStrikeoutPosition>>sFamilyClass;
    for(int i=0;i<10;i++)dt>>panose[i];
    dt>>ulUnicodeRange1>>ulUnicodeRange2>>ulUnicodeRange3>>
            ulUnicodeRange4>>achVendID.data>>fsSelection>>usFirstCharIndex>>
            usLastCharIndex>>sTypoAscender>>sTypoDescender>>sTypoLineGap>>
            usWinAscent>>usWinDescent>>ulCodePageRange1>>ulCodePageRange2>>
            sxHeight>>sCapHeight>>usDefaultChar>>usBreakChar>>usMaxContext;
};
int16 OS2Table::fzZhongxian(){
    return usWinAscent-(usWinAscent+usWinDescent)/2;
}
void OS2Table::save(QDataStream& dt){
    dt<<version<<xAvgCharWidth<<usWeightClass<<usWidthClass<<fsType<<
        ySubscriptXSize<<ySubscriptYSize<<ySubscriptXOffset<<
        ySubscriptYOffset<<ySuperscriptXSize<<ySuperscriptYSize<<
        ySuperscriptXOffset<<ySuperscriptYOffset<<yStrikeoutSize<<
        yStrikeoutPosition<<sFamilyClass;
    for(int i=0;i<10;i++)dt<<panose[i];
    dt<<ulUnicodeRange1<<ulUnicodeRange2<<ulUnicodeRange3<<
        ulUnicodeRange4<<achVendID.data<<fsSelection<<usFirstCharIndex<<
        usLastCharIndex<<sTypoAscender<<sTypoDescender<<sTypoLineGap<<
        usWinAscent<<usWinDescent<<ulCodePageRange1<<ulCodePageRange2<<
        sxHeight<<sCapHeight<<usDefaultChar<<usBreakChar<<usMaxContext;
    //不需要补零
}
void OS2Table::show(){
    QString fsTypeStr[]={
        "可安装","","受限许可","","可打印","","","","可编辑"
    };
    QString widthClassStr[]={
        "极紧缩50%","超紧缩62.5%","紧缩75%","半紧缩87.5%","常规100%","半展开112.5%","展开125%","超展开150","极展开200%"
    };
    QString weightStr[]={
        "细1/9","超轻2/9","轻3/9","普通4/9","中等5/9","半粗6/9","粗7/9","超粗8/9","黑9/9"
    };
    QString ibmFamily[][17]={
        {"No Classification"},
        {"Oldstyle Serifs",
         "No Classification",
         "IBM Rounded Legibility",
         "Garalde",
         "Venetian",
         "Modified Venetian",
         "Dutch Modern",
         "Dutch Traditional",
         "Contemporary",
         "Calligraphic",
         "","","","","","",//9-14
         "Miscellaneous"},
        {"Transitional Serifs",
         "No Classification",
         "Direct Line",
         "Script",
         "","","","","","","","","","","","",//3-14
         "Miscellaneous"},
        {"Modern Serifs",
         "No Classification",
         "Italian",
         "Script",
         "","","","","","","","","","","","",//3-14
         "Miscellaneous"},
        {"Clarendon Serifs",
         "No Classification",
         "Clarendon",
         "Modern",
         "Traditional",
         "Newspaper",
         "Stub Serif",
         "Monotone",
         "Typewriter",
         "","","","","","","",//8-14
         "Miscellaneous"},
        {"Slab Serifs",
         "No Classification",
         "Monotone",
         "Humanist",
         "Geometric",
         "Swiss",
         "Typewriter",
         "","","","","","","","","",//6-14
         "Miscellaneous"},
        {""},
        {"Freeform Serifs",
         "No Classification",
         "Modern",
         "","","","","","","","","","","","","",//2-14
         "Miscellaneous"},
        {"Sans Serif",
         "No Classification",
         "IBM Neo-grotesque Gothic",
         "Humanist",
         "Low-x Round Geometric",
         "High-x Round Geometric",
         "Neo-grotesque Gothic",
         "Modified Neo-grotesque Gothic",
         "","",//7-8
         "Typewriter Gothic",
         "Matrix",
         "","","","",//11-14
         "Miscellaneous"},
        {"Ornamentals",
         "No Classification",
         "Engraver",
         "Black Letter",
         "Decorative",
         "Three Dimensional",
         "","","","","","","","","","",//5-14
         "Miscellaneous"},
        {"Scripts",
         "No Classification",
         "Uncial",
         "Brush Joined",
         "Formal Joined",
         "Monotone Joined",
         "Calligraphic",
         "Brush Unjoined",
         "Formal Unjoined",
         "Monotone Unjoined",
         "","","","","","",//9-14
         "Miscellaneous"},
        {""},
        {"Symbolic",
         "No Classification",
         "","",//1-2
         "Mixed Serif",
         "","",//4-5
         "Oldstyle Serif",
         "Neo-grotesque Sans Serif",
         "","","","","","","",//8-14
         "Miscellaneous"},
        {""},
        {""}
    };
    datashow(QString("version:%1\t\t\t[0~5,推荐4，5]\r\n").arg(version)
             +QString("xAvgCharWidth:%1\t\t\r\n").arg(xAvgCharWidth)
             +QString("usWeightClass:%1\t\t%2\r\n").arg(usWeightClass).arg(weightStr[usWeightClass/100-1])
            +QString("usWidthClass:%1\t\t%2\r\n").arg(usWidthClass).arg(widthClassStr[usWidthClass-1])
            +QString("fsType:%1\t\t\t%2\r\n").arg(fsType).arg(fsTypeStr[fsType])
            +QString("ySubscriptXSize:%1\t\t\r\n").arg(ySubscriptXSize)
            +QString("ySubscriptYSize:%1\t\t\r\n").arg(ySubscriptYSize)
            +QString("ySubscriptXOffset:%1\t\t\r\n").arg(ySubscriptXOffset)
            +QString("ySubscriptYOffset:%1\t\t\r\n").arg(ySubscriptYOffset)
            +QString("ySuperscriptXSize:%1\t\t\r\n").arg(ySuperscriptXSize)
            +QString("ySuperscriptYSize:%1\t\t\r\n").arg(ySuperscriptYSize)
            +QString("ySuperscriptXOffset:%1\t\t\r\n").arg(ySuperscriptXOffset)
            +QString("ySuperscriptYOffset:%1\t\t\r\n").arg(ySuperscriptYOffset)
            +QString("yStrikeoutSize:%1\t\t[与post中下划线厚度相同]\r\n").arg(yStrikeoutSize)
            +QString("yStrikeoutPosition:%1\t\t\r\n").arg(yStrikeoutPosition)
            +QString("sFamilyClass:0x%1\t\t%2\r\n").arg(sFamilyClass,4,16,QChar('0')).arg(
                ibmFamily[(sFamilyClass&0xff00)>>8][0]+","+ibmFamily[(sFamilyClass&0xff00)>>8][(sFamilyClass&0x00ff)+1])
            +QString("panose[0]:%1\t\t\r\n").arg(panose[0])
            +QString("panose[1]:%1\t\t\r\n").arg(panose[1])
            +QString("panose[2]:%1\t\t\r\n").arg(panose[2])
            +QString("panose[3]:%1\t\t\r\n").arg(panose[3])
            +QString("panose[4]:%1\t\t\r\n").arg(panose[4])
            +QString("panose[5]:%1\t\t\r\n").arg(panose[5])
            +QString("panose[6]:%1\t\t\r\n").arg(panose[6])
            +QString("panose[7]:%1\t\t\r\n").arg(panose[7])
            +QString("panose[8]:%1\t\t\r\n").arg(panose[8])
            +QString("panose[9]:%1\t\t\r\n").arg(panose[9])
            +QString("ulUnicodeRange1:0x%1\t\t\r\n").arg(ulUnicodeRange1,8,16,QChar('0'))
            +QString("ulUnicodeRange2:0x%1\t\t\r\n").arg(ulUnicodeRange2,8,16,QChar('0'))
            +QString("ulUnicodeRange3:0x%1\t\t\r\n").arg(ulUnicodeRange3,8,16,QChar('0'))
            +QString("ulUnicodeRange4:0x%1\t\t\r\n").arg(ulUnicodeRange4,8,16,QChar('0'))
            +QString("    0 基础拉丁语\t\t%1\r\n").arg(ulUnicodeRange1&0x01?"✓":"×")
            +QString("    1 拉丁语-1 补充\t\t%1\r\n").arg(ulUnicodeRange1&0x02?"✓":"×")
            +QString("    2 拉丁语扩展A\t\t%1\r\n").arg(ulUnicodeRange1&0x04?"✓":"×")
            +QString("    3 拉丁语扩展B\t\t%1\r\n").arg(ulUnicodeRange1&0x08?"✓":"×")
            +QString("    29 拉丁语扩展附加CD\t%1\r\n").arg(ulUnicodeRange1&0x20000000?"✓":"×")//29
            +QString("    30 希腊语扩展\t\t%1\r\n").arg(ulUnicodeRange1&0x40000000?"✓":"×")//30
            +QString("    31 标点\t\t\t%1\r\n").arg(ulUnicodeRange1&0x80000000?"✓":"×")//31
            +QString("    32 上标和下标\t\t%1\r\n").arg(ulUnicodeRange2&0x01?"✓":"×")//32
            +QString("    33 货币符号\t\t%1\r\n").arg(ulUnicodeRange2&0x02?"✓":"×")//33
            +QString("    35 类字母符号\t\t%1\r\n").arg(ulUnicodeRange2&0x08?"✓":"×")//35
            +QString("    36 数字\t\t\t%1\r\n").arg(ulUnicodeRange2&0x10?"✓":"×")//36
            +QString("    38 数学符号\t\t%1\r\n").arg(ulUnicodeRange2&0x40?"✓":"×")//38
            +QString("    48 中日韩符号和标点\t%1\r\n").arg(ulUnicodeRange2&0x00010000?"✓":"×")//48
            +QString("    51 以前的bpmf\t\t%1\r\n").arg(ulUnicodeRange2&0x00080000?"✓":"×")//51
            +QString("    55 CJK兼容\t\t%1\r\n").arg(ulUnicodeRange2&0x00800000?"✓":"×")//55
            +QString("    57 有BMP外的字符\t\t%1\r\n").arg(ulUnicodeRange2&0x02000000?"✓":"×")
            +QString("    59 中日韩统一表意文字\t%1\r\n").arg(ulUnicodeRange2&0x08000000?"✓":"×")//59
            +QString("    61 中日韩笔画、文字\t%1\r\n").arg(ulUnicodeRange2&0x20000000?"✓":"×")//61
            +QString("    65 竖排标点\t\t%1\r\n").arg(ulUnicodeRange3&0x00000002?"✓":"×")//65
            +QString("    68 全角\t\t\t%1\r\n").arg(ulUnicodeRange3&0x00000010?"✓":"×")//68
            +QString("    89 数学符号\t\t%1\r\n").arg(ulUnicodeRange3&0x02000000?"✓":"×")
            +QString("    99 易经卦符\t\t%1\r\n").arg(ulUnicodeRange4&0x00000008?"✓":"×")
            +QString("    102 古希腊数字\t\t%1\r\n").arg(ulUnicodeRange4&0x00000040?"✓":"×")
            +QString("    109 太玄经\t\t%1\r\n").arg(ulUnicodeRange4&0x00002000?"✓":"×")
            +QString("    110 楔形文字\t\t%1\r\n").arg(ulUnicodeRange4&0x00004000?"✓":"×")
            +QString("    111 算筹计数\t\t%1\r\n").arg(ulUnicodeRange4&0x00008000?"✓":"×")
            +QString("    122 麻将\t\t%1\r\n").arg(ulUnicodeRange4&0x04000000?"✓":"×")
            +QString("achVendID:%1\t\t0x%2\r\n").arg(achVendID).arg(achVendID.data,0,16)
            +QString("fsSelection:0x%1\t\t\r\n").arg(fsSelection,4,16,QChar('0'))
            +QString("    0 斜体\t\t%1\r\n").arg(fsSelection&0x01?"✓":"×")
            +QString("    1 下划线\t%1\r\n").arg(fsSelection&0x02?"✓":"×")
            +QString("    2 负片\t\t%1\r\n").arg(fsSelection&0x04?"✓":"×")
            +QString("    3 空心字\t%1\r\n").arg(fsSelection&0x08?"✓":"×")
            +QString("    4 删除线\t%1\r\n").arg(fsSelection&0x10?"✓":"×")
            +QString("    5 粗体\t\t%1\r\n").arg(fsSelection&0x20?"✓":"×")
            +QString("    6 常规\t\t%1\r\n").arg(fsSelection&0x40?"✓":"×")
            +QString("    7 使用sTypo_\t%1\r\n").arg(fsSelection&0x80?"✓":"×")
            +QString("    8 name表有21 22\t%1\r\n").arg(fsSelection&0x100?"✓":"×")
            +QString("    9 倾斜\t\t%1\r\n").arg(fsSelection&0x200?"✓":"×")
            +QString("usFirstCharIndex:0x%1\t\t\r\n").arg(usFirstCharIndex,4,16,QChar('0'))
            +QString("usLastCharIndex:0x%1\t\t\r\n").arg(usLastCharIndex,4,16,QChar('0'))
            +QString("sTypoAscender:%1\t\t\r\n").arg(sTypoAscender)
            +QString("sTypoDescender:%1\t\t\r\n").arg(sTypoDescender)
            +QString("sTypoLineGap:%1\t\t\r\n").arg(sTypoLineGap)
            +QString("usWinAscent:%1\t\t\r\n").arg(usWinAscent)
            +QString("usWinDescent:%1\t\t\r\n").arg(usWinDescent)
            +QString("ulCodePageRange1:0x%1\t\r\n").arg(ulCodePageRange1,8,16,QChar('0'))
            +QString("ulCodePageRange2:0x%1\t\t\r\n").arg(ulCodePageRange2,8,16,QChar('0'))
            +QString("    0 拉丁语1 1252\t\t%1\r\n").arg(ulCodePageRange1&0x00000001?"✓":"×")
            +QString("    1 拉丁语2-东欧1250\t\t%1\r\n").arg(ulCodePageRange1&0x00000002?"✓":"×")
            +QString("    2 西里尔1251\t\t%1\r\n").arg(ulCodePageRange1&0x00000004?"✓":"×")
            +QString("    3 希腊语1253\t\t%1\r\n").arg(ulCodePageRange1&0x00000008?"✓":"×")
            +QString("    4 土耳其语1254\t\t%1\r\n").arg(ulCodePageRange1&0x00000010?"✓":"×")
            +QString("    18 中文GBK 936\t\t%1\r\n").arg(ulCodePageRange1&0x00040000?"✓":"×")
            +QString("    20 繁体BIG5 950\t\t%1\r\n").arg(ulCodePageRange1&0x00100000?"✓":"×")
            +QString("    29 Mac字符（美国罗马人）\t%1\r\n").arg(ulCodePageRange1&0x20000000?"✓":"×")
            +QString("    31 符号字符\t\t%1\r\n").arg(   ulCodePageRange1&0x80000000?"✓":"×")
            +QString("sxHeight:%1\t\t\t%2\r\n").arg(sxHeight).arg(sxHeight==0?"[未使用]":"")
            +QString("sCapHeight:%1\t\t%2\r\n").arg(sCapHeight).arg(sCapHeight==0?"[未使用]":"")
            +QString("usDefaultChar:0x%1\t\t0=图0作为默认字符\r\n").arg(usDefaultChar,4,16,QChar('0'))
            +QString("usBreakChar:0x%1\t\t0x20=空格作为分割符号\r\n").arg(usBreakChar,4,16,QChar('0'))
            +QString("usMaxContext:%1\t\t2=只有字距调整功能\r\n").arg(usMaxContext));
}
void OS2Table::jisuan(uint16 emsize,uint32 u1,uint32 u2,uint32 u3, uint32 u4,
                      uint16 cKaishi, uint16 cJieshu,int16 houdu_,uint16 avgAW){
    Q_UNUSED(avgAW);
    enum{
        //记事本 格式/字体--字形名称，使用此参数命名，与panose[3]的值无关，与head.macStyle无关
        FWIDTH_cong1kaishiYouxiao,
        FWIDTH_ULTRA_CONDENSED,//	50    极紧缩
        FWIDTH_EXTRA_CONDENSED,//	62.5  超紧缩
        FWIDTH_CONDENSED      ,//	75    紧缩
        FWIDTH_SEMI_CONDENSED, //	87.5  半紧缩
        FWIDTH_NORMAL,         //	100   常规
        FWIDTH_SEMI_EXPANDED,  //	112.5 半展开
        FWIDTH_EXPANDED,       //	125   展开
        FWIDTH_EXTRA_EXPANDED, //	150   超展开
        FWIDTH_ULTRA_EXPANDED, //	200   极展开
    };
    version=4;
    xAvgCharWidth=256;//avgAW;//
    usWeightClass=400;//Regular
    switch (this->panose[3]) {
    case 5://展开
        usWidthClass=FWIDTH_EXPANDED;break;
    case 6://紧缩
        usWidthClass=FWIDTH_CONDENSED;break;
    case 7://极展开
        usWidthClass=FWIDTH_ULTRA_EXPANDED;break;
    case 8://极紧缩
        usWidthClass=FWIDTH_ULTRA_CONDENSED;break;
    default:
        usWidthClass=FWIDTH_NORMAL;//100% 正常
    }
    fsType=8;//0=可安装 8=可编辑
    yStrikeoutSize=houdu_;//下划线厚度
    int xs=emsize/256;
    ySubscriptXSize=xs*128;
    ySubscriptYSize=xs*128;
    ySubscriptXOffset=0;
    ySubscriptYOffset=xs*16;
    ySuperscriptXSize=xs*128;
    ySuperscriptYSize=xs*128;
    ySuperscriptXOffset=0;
    ySuperscriptYOffset=xs*128;
    yStrikeoutPosition=xs*65;
    sTypoAscender=usWinAscent;
    sTypoDescender=-usWinDescent;
    sTypoLineGap=xs*36;
    sxHeight=xs*118;
    sCapHeight=xs*176;
    //
    sFamilyClass=0;//没有分类
    ulUnicodeRange1=u1;
    ulUnicodeRange2=u2;
    ulUnicodeRange3=u3;
    ulUnicodeRange4=u4;
    achVendID="    ";//4个空格
    fsSelection=0x0040;//0x00C0=常规+sType,0x0040=常规
    usFirstCharIndex=cKaishi;
    usLastCharIndex=cJieshu;
    ulCodePageRange1=0x00040001;//拉丁1+简体中文
    ulCodePageRange2=0;//拉丁1+简体中文
    usDefaultChar=0;//glyph id=0做默认字符
    usBreakChar=0x20;//空格做分隔符
    usMaxContext=0;//a font which has only a pair kerning feature should set this field to 2.
}
void OS2Table::xinjian(uint16 emsize){
    updateShangxiaBianjie(emsize);
    const uint8 x[]={2,1,6,1,3,1,1,1,1,1};
    for(int i=0;i<10;i++){
        panose[i]=x[i];
    }
}
void OS2Table::updateShangxiaBianjie(uint16 newEmsize){
    int xs=newEmsize/256;
    usWinAscent=xs*220;
    usWinDescent=xs*36;
    sxHeight=xs*118;
    sCapHeight=xs*176;
}
