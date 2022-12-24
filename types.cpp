#include <types.h>


TableTTF::TableTTF(QObject *parent):QObject(parent){

}

uint24::operator QString() const
{
    return "";
}

void uint24::set(QString xx){Q_UNUSED(xx);value[0]=0;}


Version16Dot16::Version16Dot16(){}

Version16Dot16::Version16Dot16(uint32 x):value(x)
{

}

Version16Dot16::operator int32&()
{
    return value;
}

Version16Dot16::operator QString() const
{
    //这个格式只用在maxp vhea post三个表中
    //比如0x00011000  1.1
    return QString("%1.%2").arg(value >> 16).arg((value&0xffff)>>12);
}

Fixed::Fixed(){}
Fixed::Fixed(qint32 x):value(x){}
void Fixed::operator =(double x){
    value=(int(x))<<16;
    double xs=x-int(x);
    for(int i=3;i>=0;i--){
        double n=xs*16;
        int jg=int(n);
        value+=jg<<(i*4);
        xs=n-jg;
    }
}

Fixed::operator QString() const
{
    uint32 vx=value&0xffff;
    double x=((vx&0xf000)>>12)/16.0+((vx&0xf00)>>8)/256.0+((vx&0xf0)>>4)/4096.0+(vx&0xf)/65536.0;
    return QString("%1").arg(((value >> 16)+x),0,'f',3);
}

F2DOT14::F2DOT14(const double putongXiaoshu)
{
    doubleValue=putongXiaoshu;
    int zhengshu=floor(putongXiaoshu);
    double xs=16384*(putongXiaoshu-zhengshu);
    value=(zhengshu<<14)+(int)(xs+0.5);
}

F2DOT14::F2DOT14(const F2DOT14 &fd):value(fd.value)
{
    doubleValue=toDouble();
}


double F2DOT14::toDouble() const
{
    return (value >> 14)+(value&0x3fff)/16384.0;
}
void F2DOT14::operator =(double putongXiaoshu){
    int zhengshu=floor(putongXiaoshu);
    double xs=16384*(putongXiaoshu-zhengshu);
    value=(zhengshu<<14)+(int)(xs+0.5);
    doubleValue=putongXiaoshu;
}
void F2DOT14::operator=(const int16 f2dot14Value){
    value=f2dot14Value;
    doubleValue=toDouble();
}

bool F2DOT14::operator ==(const F2DOT14& other)
{
    return value==other.value;
}
//bool F2DOT14::operator !=(int16 f2dot14Value)
//{
//    return value!=f2dot14Value;
//}

bool F2DOT14::operator !=(const double putongXiaoshu)
{
    return doubleValue!=putongXiaoshu;
}

F2DOT14::operator double() const
{
    return doubleValue;
}

Tag::Tag()
{
    this->data=0x20202020;
}

void Tag::operator =(QString x){
    zifu[3]=x[0].unicode();
    zifu[2]=x[1].unicode();
    zifu[1]=x[2].unicode();
    zifu[0]=x[3].unicode();
}

bool Tag::operator ==(const QString &other) const
{
    return other==this->operator QString();
}

Tag::operator QString() const
{
    return QString("%1%2%3%4").arg(QChar(zifu[3])).arg(QChar(zifu[2]))
            .arg(QChar(zifu[1])).arg(QChar(zifu[0]));
}

