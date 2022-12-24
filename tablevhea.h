#ifndef TABLEVHEA_H
#define TABLEVHEA_H

#include "types.h"

class VHEATable : public TableTTF
{
    Q_OBJECT
public:
    VHEATable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    Version16Dot16	version;//!<	Version number of the vertical header table; 0x00010000 for version 1.0
    //1.0
    //union{
    //    int16	ascent;
    //    int16	vertTypoAscender;
    //};
    int16	ascent;//!<	Distance in FUnits from the centerline to the previous line’s descent.
    int16	descent;//!<	Distance in FUnits from the centerline to the next line’s ascent.
    int16	lineGap;//!<	Reserved; set to 0
    //1.1
    //int16	vertTypoAscender;//!<	The vertical typographic ascender for this font. It is the distance in FUnits from the vertical center baseline to the right edge of the design space for CJK / ideographic glyphs (or “ideographic em box”).
    ////It is usually set to (head.unitsPerEm)/2. For example, a font with an em of 1000 FUnits will set this field to 500. See the Baseline tags section of the OpenType Layout Tag Registry for a description of the ideographic em-box.
    //int16	vertTypoDescender;//!<	The vertical typographic descender for this font. It is the distance in FUnits from the vertical center baseline to the left edge of the design space for CJK / ideographic glyphs.
    ////It is usually set to -(head.unitsPerEm/2). For example, a font with an em of 1000 FUnits will set this field to -500.
    //int16	vertTypoLineGap;//!<	The vertical typographic gap for this font. An application can determine the recommended line spacing for single spaced vertical text for an OpenType font by the following expression: ideo embox width + vhea.vertTypoLineGap
    int16	advanceHeightMax;//!<	The maximum advance height measurement -in FUnits found in the font. This value must be consistent with the entries in the vertical metrics table.
    int16	minTopSideBearing;//!<	The minimum top sidebearing measurement found in the font, in FUnits. This value must be consistent with the entries in the vertical metrics table.
    int16	minBottomSideBearing;//!<	The minimum bottom sidebearing measurement found in the font, in FUnits. This value must be consistent with the entries in the vertical metrics table.
    int16	yMaxExtent;//!<	Defined as yMaxExtent = max(tsb + (yMax - yMin)).
    int16	caretSlopeRise;//!<	The value of the caretSlopeRise field divided by the value of the caretSlopeRun Field determines the slope of the caret. A value of 0 for the rise and a value of 1 for the run specifies a horizontal caret. A value of 1 for the rise and a value of 0 for the run specifies a vertical caret. Intermediate values are desirable for fonts whose glyphs are oblique or italic. For a vertical font, a horizontal caret is best.
    int16	caretSlopeRun;//!<	See the caretSlopeRise field. Value=1 for nonslanted vertical fonts.
    int16	caretOffset;//!<	The amount by which the highlight on a slanted glyph needs to be shifted away from the glyph in order to produce the best appearance. Set value equal to 0 for nonslanted fonts.
    int16	reserved1;//!<	Set to 0.
    int16	reserved2;//!<	Set to 0.
    int16	reserved3;//!<	Set to 0.
    int16	reserved4;//!<	Set to 0.
    int16	metricDataFormat;//!<	Set to 0.
    uint16	numOfLongVerMetrics;//!<	Number of advance heights in the vertical metrics table.
};

class vMetrics{
public:
    uint16	advanceHeight;//!<	The advance height of the glyph. Unsigned integer in FUnits
    int16	topSideBearing;//!<	The top sidebearing of the glyph. Signed integer in FUnits.
};

class VMTXTable:public TableTTF{
    Q_OBJECT
public:
    VMTXTable(QObject *parent=nullptr);
    void read(QDataStream& dt, uint32 offset);
    void save(QDataStream& dt);
    void show();
    list<vMetrics> verticalMetrics;
};

#endif // TABLEVHEA_H
