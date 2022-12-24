#ifndef TYPES_H
#define TYPES_H
#include <QtGlobal>
#include <QString>
#include <QDataStream>
#include <QTextBrowser>
#include <../rizhi/rizhi.h>
#include<QObject>
#include<QtMath>
#include <algorithm>
using namespace std;

typedef quint8 uint8;
typedef qint8 int8;
typedef quint16 uint16;
typedef qint16 int16;
typedef quint32 uint32;
typedef qint32 int32;
typedef qint64 int64;
typedef quint64 uint64;
typedef qint16 FWORD;//int16 that describes a quantity in font design units.
typedef quint16 UFWORD;
typedef qint64 LONGDATETIME;
typedef uint16 Offset16;
typedef uint32 Offset32;

typedef QVector<uint16> u16vector;
typedef QVector<int16> s16vector;
typedef QVector<uint32> u32vector;
typedef QVector<int32> s32vector;
enum tbType{
    GPOS,
    GSUB
};

class TableTTF:public QObject
{
    Q_OBJECT
public:
    TableTTF(QObject *parent=nullptr);
signals:
    void datashow(QString,bool iserror=false);
    void jindu(int bfb1to100);
};

const uint8 pad1=0;
const quint64 sec19040101to19700101=2082844800;

class uint24
{
public:
    operator QString()const;
    void set(QString xx);
private:
    quint8 value[3];
};

class Version16Dot16
{
public:
    Version16Dot16();
    explicit Version16Dot16(uint32 x);
    operator QString()const;//隐式转换为const QString
    operator int32&();//隐式转换为int32
    int32 value;
};
class Fixed //32-bit signed fixed-point number (16.16)
{
public:
    Fixed();
    explicit Fixed(qint32 x);
    void operator =(double x);
    operator QString()const;
    qint32 value;
};

/// 16bit小数，2.14 2bit整数，14bit小数
class F2DOT14 //16-bit signed fixed number with the low 14 bits of fraction(2.14).
{
public:
    constexpr static double MOREN=1.0;
    //F2DOT14(const int16 f2dot14Value=0);
    F2DOT14(const F2DOT14& fd);
    F2DOT14(const double putongXiaoshu=0.0);
    qint16 value;
    void operator =(double putongXiaoshu);
    void operator =(const int16 f2dot14Value);
    bool operator ==(const F2DOT14 &other);
    //bool operator !=(int16 f2dot14Value);
    bool operator !=(const double putongXiaoshu);
    operator double()const;
private:
    double toDouble() const;
    double doubleValue;
};

class Tag//Array of four uint8s (length = 32 bits) used to identify a table,
                     //design-variation axis, script, language system, feature, or baseline
{
public:
    Tag();
    void operator =(QString x);
    operator QString()const;
    bool operator ==(const QString &other)const;
    union{
        quint32 data;
        char zifu[4];
    };
};

#endif // TYPES_H
