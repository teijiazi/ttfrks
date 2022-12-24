#ifndef TABLES1_H
#define TABLES1_H

#include "types.h"

class HEADTable:public TableTTF{//必须
    Q_OBJECT
signals:
    void setFontDate(LONGDATETIME create,uint16 unitsPerEm);
public:
    HEADTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    uint16	majorVersion;//	Major version number of the font header table — set to 1.
    uint16	minorVersion;//	Minor version number of the font header table — set to 0.
    Fixed	fontRevision;//	Set by font manufacturer.
    uint32	checksumAdjustment;//	To compute: set it to 0, sum the entire font as uint32, then store 0xB1B0AFBA - sum. If the font is used as a component in a font collection file, the value of this field will be invalidated by changes to the file structure and font table directory, and must be ignored.
    uint32	magicNumber;//	Set to 0x5F0F3CF5.
    uint16	flags;//	Bit 0: Baseline for font at y=0.
    //Bit 1: Left sidebearing point at x=0 (relevant only for TrueType rasterizers) — see the note below regarding variable fonts.
    //Bit 2: Instructions may depend on point size.
    //Bit 3: Force ppem to integer values for all internal scaler math; may use fractional ppem sizes if this bit is clear. It is strongly recommended that this be set in hinted fonts.
    //Bit 4: Instructions may alter advance width (the advance widths might not scale linearly).
    //Bit 5: This bit is not used in OpenType, and should not be set in order to ensure compatible behavior on all platforms. If set, it may result in different behavior for vertical layout in some platforms. (See Apple’s specification for details regarding behavior in Apple platforms.)
    //Bits 6–10: These bits are not used in Opentype and should always be cleared. (See Apple’s specification for details regarding legacy used in Apple platforms.)
    //Bit 11: Font data is “lossless” as a result of having been subjected to optimizing transformation and/or compression (such as e.g. compression mechanisms defined by ISO/IEC 14496-18, MicroType Express, WOFF 2.0 or similar) where the original font functionality and features are retained but the binary compatibility between input and output font files is not guaranteed. As a result of the applied transform, the DSIG table may also be invalidated.
    //Bit 12: Font converted (produce compatible metrics).
    //Bit 13: Font optimized for ClearType™. Note, fonts that rely on embedded bitmaps (EBDT) for rendering should not be considered optimized for ClearType, and therefore should keep this bit cleared.
    //Bit 14: Last Resort font. If set, indicates that the glyphs encoded in the 'cmap' subtables are simply generic symbolic representations of code point ranges and don’t truly represent support for those code points. If unset, indicates that the glyphs encoded in the 'cmap' subtables represent proper support for those code points.
    //Bit 15: Reserved, set to 0.
    uint16	unitsPerEm;//	Set to a value from 16 to 16384. Any value in this range is valid. In fonts that have TrueType outlines, a power of 2 is recommended as this allows performance optimizations in some rasterizers.
    LONGDATETIME	created;//	Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone.
    LONGDATETIME	modified;//	Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone.
    int16	xMin;//	Minimum x coordinate across all glyph bounding boxes.
    int16	yMin;//	Minimum y coordinate across all glyph bounding boxes.
    int16	xMax;//	Maximum x coordinate across all glyph bounding boxes.
    int16	yMax;//	Maximum y coordinate across all glyph bounding boxes.
    uint16	macStyle;//	Bit 0: Bold (if set to 1);
    //Bit 1: Italic (if set to 1)
    //Bit 2: Underline (if set to 1)
    //Bit 3: Outline (if set to 1)
    //Bit 4: Shadow (if set to 1)
    //Bit 5: Condensed (if set to 1)
    //Bit 6: Extended (if set to 1)
    //Bits 7–15: Reserved (set to 0).
    uint16	lowestRecPPEM;//	Smallest readable size in pixels.
    int16	fontDirectionHint;//	Deprecated (Set to 2).
    //0: Fully mixed directional glyphs;
    //1: Only strongly left to right;
    //2: Like 1 but also contains neutrals;
    //-1: Only strongly right to left;
    //-2: Like -1 but also contains neutrals.
    int16	indexToLocFormat;//	0 for short offsets (Offset16), 1 for long (Offset32).
    int16	glyphDataFormat;//	0 for current format.
    void jisuan(qint64 riqi1970, uint16 emsize, QString banben,
                int16 vxMin, int16 vyMin, int16 vxMax, int16 vyMax, bool lsbAtY, int panose3);
};
class HHEATable:public TableTTF{//必须
    Q_OBJECT
public:
    HHEATable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    uint16	majorVersion;//	Major version number of the horizontal header table — set to 1.
    uint16	minorVersion;//	Minor version number of the horizontal header table — set to 0.
    FWORD	ascender;//	Typographic ascent—see note below.
    FWORD	descender;//	Typographic descent—see note below.
    FWORD	lineGap;//	Typographic line gap.Negative LineGap values are treated as zero in some legacy platform implementations.
    UFWORD	advanceWidthMax;//	Maximum advance width value in 'hmtx' table.
    FWORD	minLeftSideBearing;//	Minimum left sidebearing value in 'hmtx' table for glyphs with contours (empty glyphs should be ignored).
    FWORD	minRightSideBearing;//	Minimum right sidebearing value; calculated as min(aw - (lsb + xMax - xMin)) for glyphs with contours (empty glyphs should be ignored).
    FWORD	xMaxExtent;//	Max(lsb + (xMax - xMin)).
    int16	caretSlopeRise;//	Used to calculate the slope of the cursor (rise/run); 1 for vertical.
    int16	caretSlopeRun;//	0 for vertical.
    int16	caretOffset;//	The amount by which a slanted highlight on a glyph needs to be shifted to produce the best appearance. Set to 0 for non-slanted fonts
    int16	reserved1;//	set to 0
    int16	reserved2;//	set to 0
    int16	reserved3;//	set to 0
    int16	reserved4;//	set to 0
    int16	metricDataFormat;//	0 for current format.
    uint16	numberOfHMetrics;//	Number of hMetric entries in 'hmtx' table
    void jisuan(UFWORD vAdvanceWidthMax, uint16 noh, FWORD xMaxExt,
                FWORD vMinLsb, FWORD vMinRsb, int16 vAscent, int16 vDscent, int16 sGap);
};
class MAXPTable:public TableTTF{//必须
    //ttf ver=1.0
    Q_OBJECT
public:
    MAXPTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    Version16Dot16	version;//	0x00010000 for version 1.0.
    uint16	numGlyphs;//	The number of glyphs in the font.
    uint16	maxPoints;//	Maximum points in a non-composite glyph.
    uint16	maxContours;//	Maximum contours in a non-composite glyph.
    uint16	maxCompositePoints;//	Maximum points in a composite glyph.
    uint16	maxCompositeContours;//	Maximum contours in a composite glyph.
    uint16	maxZones;//	1 if instructions do not use the twilight zone (Z0), or 2 if instructions do use Z0; should be set to 2 in most cases.
    uint16	maxTwilightPoints;//	Maximum points used in Z0.
    uint16	maxStorage;//	Number of Storage Area locations.
    uint16	maxFunctionDefs;//	Number of FDEFs, equal to the highest function number + 1.
    uint16	maxInstructionDefs;//	Number of IDEFs.
    uint16	maxStackElements;//	Maximum stack depth across Font Program ('fpgm' table), CVT Program ('prep' table) and all glyph instructions (in the 'glyf' table).
    uint16	maxSizeOfInstructions;//	Maximum byte count for glyph instructions.
    uint16	maxComponentElements;//	Maximum number of components referenced at “top level” for any composite glyph.
    uint16	maxComponentDepth;//	Maximum levels of recursion; 1 for simple components.
    void jisuan(uint16 tuxingZongshu, uint16 jdLk, uint16 jdDian, uint16 fzLk, uint16 fzDian);
};
class OS2Table:public TableTTF{//必须
    //version34结构相同 最新version5
    Q_OBJECT
public:
    OS2Table(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    int16 fzZhongxian();
    uint16	version;
    int16	xAvgCharWidth;
    uint16	usWeightClass;
    uint16	usWidthClass;
    uint16	fsType;
    int16	ySubscriptXSize;
    int16	ySubscriptYSize;
    int16	ySubscriptXOffset;
    int16	ySubscriptYOffset;
    int16	ySuperscriptXSize;
    int16	ySuperscriptYSize;
    int16	ySuperscriptXOffset;
    int16	ySuperscriptYOffset;
    int16	yStrikeoutSize;
    int16	yStrikeoutPosition;
    int16	sFamilyClass;
    uint8	panose[10];
    uint32	ulUnicodeRange1;
    uint32	ulUnicodeRange2;
    uint32	ulUnicodeRange3;
    uint32	ulUnicodeRange4;
    Tag	achVendID;
    uint16	fsSelection;
    uint16	usFirstCharIndex;
    uint16	usLastCharIndex;
    int16	sTypoAscender;
    int16	sTypoDescender;
    int16	sTypoLineGap;
    uint16	usWinAscent;
    uint16	usWinDescent;
    uint32	ulCodePageRange1;
    uint32	ulCodePageRange2;
    int16	sxHeight;
    int16	sCapHeight;
    uint16	usDefaultChar;
    uint16	usBreakChar;
    uint16	usMaxContext;
    void jisuan(uint16 emsize, uint32 u1, uint32 u2, uint32 u3, uint32 u4,
                uint16 cKaishi, uint16 cJieshu, int16 houdu_, uint16 avgAW);
    void xinjian(uint16 emsize);
    void updateShangxiaBianjie(uint16 newEmsize);
signals:

};


#endif // TABLES1_H


