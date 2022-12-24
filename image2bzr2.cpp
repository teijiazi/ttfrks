#include "image2bzr2.h"

#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"

Image2Bzr2::Image2Bzr2(QObject *parent) : QObject(parent),xianshifenduan(false)
{
}

void Image2Bzr2::getLKfrom(QString &tpFilePath,uint16 ascent){
    cv::setUseOptimized(true);

    yuanshiHeibaitu = imread(tpFilePath.toLocal8Bit().toStdString());
    //yuanshiHeibaitu=original.clone();
    usWinAscent=ascent;
}

void  myFindContours(const Mat& graySrc, vector<vector<Dian>>& contours,
                    int retr = 1, int method = CHAIN_APPROX_SIMPLE)
{
    //cv::findContours会出现异常
    CvMat c_image = cvMat(graySrc);//cvMat.type=0x42424000
    Ptr<CvMemStorage> storage(cvCreateMemStorage());
    CvSeq* _ccontours = 0;
    cvFindContours(&c_image,storage, &_ccontours, sizeof(CvContour), retr, method);
    if (!_ccontours)
    {
        contours.clear();
        return;
    }
    Seq<CvSeq*> all_contours(cvTreeToNodeSeq(_ccontours, sizeof(CvSeq), storage));
    int total = (int)all_contours.size();

    SeqIterator<CvSeq*> it = all_contours.begin();
    for (int i = 0; i < total; i++, ++it)
    {
        CvSeq* c = *it;
        ((CvContour*)c)->color = (int)i;
        int count = (int)c->total;
        if(count<3){
            continue;
        }
        int* data = new int[count * 2];
        cvCvtSeqToArray(c, data);
        vector<Dian> dlb;
        for (int j = 0; j < count; j++)
        {
            dlb.push_back(Dian(data[j * 2], data[j * 2 + 1]));
        }
        contours.push_back(dlb);
        delete[] data;
    }
    storage.release();
}

void Image2Bzr2::xiugaiCanshu(Biji &biji,int &lks,
                              bool isGaosi,bool isFanse,
                              int menxian,double vzhixian,
                              uint lvbo,double vguaidian,
                              bool isFenduan,
                              int nihe){

    //menxian 3 5 7 ...自适应二值化阈值
    //moshi 1✓      2        3
    //     所有轮廓  完整层次  树
    //nihe 1      2    3   4     5     -1
    //     全部点  简单 L1  KCOS  运行  不修改
    //lvbo:中值滤波参数，(2*lvbo+1)>1为有效值,lvbo=0表示不使用中值滤波
    bc.isGaosi=isGaosi;
    bc.isFanse=isFanse;
    bc.menxian=menxian;
    if(nihe!=-1){
        bc.nihe=nihe;
    }
    bc.lvbo=2*lvbo+1;
    bc.cl=2*lvbo+1;
    bc.guaidian=vguaidian;
    bc.zhixian=vzhixian;
    xianshifenduan=isFenduan;
    Mat gray;
    cvtColor(yuanshiHeibaitu, gray,COLOR_RGB2GRAY);//灰度化
    int menxianJisuan=menxian*2+1;
    if(bc.cl>=menxianJisuan/2){
        //bc.cl=menxianJisuan/2;
        //medianBlur(gray,gray,2*bc.lvbo+1);
    }
    //还可以使用的函数： #compare, #inRange, #threshold , #Canny,得到二值图
    adaptiveThreshold(gray, gray, 255,
                      isGaosi?ADAPTIVE_THRESH_GAUSSIAN_C:ADAPTIVE_THRESH_MEAN_C,
                      isFanse?THRESH_BINARY:THRESH_BINARY_INV,
                      menxianJisuan,
                      bc.cl
                      );
    std::vector<std::vector <Dian> >contours;
    myFindContours(gray, contours, bc.moshi, bc.nihe); //找轮廓
    lks=contours.size();
    lisandian2Bezier(contours,biji);
}

void Image2Bzr2::piliangShiyong(Biji &biji,int &lks){

    //menxian 3 5 7 ...自适应二值化阈值
    //moshi 1✓      2        3
    //     所有轮廓  完整层次  树
    //nihe 1      2    3   4     5     -1
    //     全部点  简单 L1  KCOS  运行  不修改
    //lvbo:中值滤波参数，(2*lvbo+1)>1为有效值,lvbo=0表示不使用中值滤波
    Mat gray;
#ifdef opencvsrc
    cvtColor(yuanshiHeibaitu, gray,COLOR_RGB2GRAY);//灰度化 COLOR_RGB2GRAY
#else
    cvtColor(yuanshiHeibaitu, gray,COLOR_RGB2GRAY);//灰度化
#endif
    int menxianJisuan=bc.menxian*2+1;
    //还可以使用的函数： #compare, #inRange, #threshold , #Canny,得到二值图
    adaptiveThreshold(gray, gray, 255,
                      bc.isGaosi?ADAPTIVE_THRESH_GAUSSIAN_C:ADAPTIVE_THRESH_MEAN_C,
                      bc.isFanse?THRESH_BINARY:THRESH_BINARY_INV,
                      menxianJisuan,
                      bc.cl
                      );
    std::vector<std::vector <Dian> >contours;
    myFindContours(gray, contours, bc.moshi, bc.nihe); //找轮廓
    lks=contours.size();
    lisandian2Bezier(contours,biji);
}

double Image2Bzr2::juliB2AC(const Dian& a,const Dian& b,const Dian& c){
    //ac连线，计算b到ac的距离
    /// 点到直线距离公式
    /// 点的坐标(x,y),直线方程 Ax+By+C=0
    ///
    ///       | Ax+By+C|
    /// d = -------------
    ///       /‾‾‾‾‾‾‾‾
    ///      ✓   A²+B²
    ///
    if(a.x==c.x){
        return fabs(b.x-a.x);
    }

    double A=1.0*(c.y-a.y)/(c.x-a.x);
    double B=-1.0;
    double C=1.0*a.y-1.0*a.x*(c.y-a.y)/(c.x-a.x);
    double juli=fabs(A*b.x+B*b.y+C)/sqrt(pow(A,2)+pow(B,2));
    //snd<<juli<<"  |->a=("<<a.x<<","<<a.y<<"),b="<<b.x<<","<<b.y<<",c="<<c.x<<","<<c.y;
    return juli;
}

double Image2Bzr2::juliAB(const Dian& a,const Dian& b){
    //AB两点间距离
    return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2));
}

void Image2Bzr2::fenduan(vector<Dian> &bianjiedian, u16vector & tezhengdian){
    tezhengdian.append(0);
    bianjiedian[0].y=usWinAscent-bianjiedian[0].y;
    bianjiedian[1].y=usWinAscent-bianjiedian[1].y;
    for(uint vi=1;vi<bianjiedian.size();vi++){
        Dian &c=bianjiedian[0];
        if(vi+1<bianjiedian.size()){
            bianjiedian[vi+1].y=usWinAscent-bianjiedian[vi+1].y;
            c=bianjiedian[vi+1];
        }
        double czjl=juliB2AC(bianjiedian[tezhengdian.last()],bianjiedian[vi],c);
        if(czjl>bc.guaidian){
            tezhengdian.append(vi);
        }
    }
    //把起点补进来
    tezhengdian.append(tezhengdian.first());
}

void Image2Bzr2::zuoquxian(const vector<Dian> &bianjiedian,
                           const u16vector &tezhengdian,Biji&biji){
    for(int i=0;i+1<tezhengdian.length();i++){
        //两个拐点间一条线
        //准备做一条曲线 p0 p1 p2
        //曲线起点p0
        uint be=tezhengdian[i],en=tezhengdian[i+1];//两个连续拐点--曲线两端点
        Dian p0=bianjiedian[be],p2=bianjiedian[en];
        //起点加入biji
        bzr2 bz;
        bz.x=p0.x;
        bz.y=p0.y;
        bz.isControl=false;
        biji.push_back(bz);
        //两个拐点间无数据，不做曲线---不插入控制点，自动就是一条直线
        if(en==0){//最后一个拐点到起点的线条
            en=bianjiedian.size();
        }
        if(be+1==en){
            continue;
        }
        //检测是不是直线
        double pjjl=0;
        //snd<<"liangdian:p0="<<p0.x<<","<<p0.y<<",p2="<<p2.x<<","<<p2.y<<",be:"<<be<<",en:"<<en;
        for(uint ti=be+1;ti<en;ti++){
            double cxjl=juliB2AC(p0,bianjiedian[ti],p2);
            //snd<<"djl:"<<bianjiedian[ti].x<<","<<bianjiedian[ti].y<<",juli="<<cxjl;
            pjjl+=cxjl;
        }
        pjjl/=(en-be-1);
        if(pjjl<bc.zhixian){//平均距离小于1
            continue;
        }
        //计算tk
        vector<double> tk;
        double zongchang=0;
        for(uint ti=be;ti<en;ti++){
            //原始数据点两点间距离
            double jl=juliAB(bianjiedian[ti],(ti==bianjiedian.size()-1)?bianjiedian[0]:bianjiedian[ti+1]);
            zongchang+=jl;
            tk.push_back(zongchang);
        }
        double px=0,py=0,f1f1=0;
        for(uint ti=0,bjIdx=be+1;ti+1<tk.size();ti++,bjIdx++){
            tk[ti]/=zongchang;//算出tk
            px+=(bianjiedian[bjIdx].x-p0.x*pow(1-tk[ti],2)-p2.x*pow(tk[ti],2))*2*tk[ti]*(1-tk[ti]);
            py+=(bianjiedian[bjIdx].y-p0.y*pow(1-tk[ti],2)-p2.y*pow(tk[ti],2))*2*tk[ti]*(1-tk[ti]);
            f1f1+=2*tk[ti]*(1-tk[ti])*2*tk[ti]*(1-tk[ti]);
        }
        //插入控制点
        bz.x=px/f1f1;
        bz.y=py/f1f1;
        bz.isControl=true;
        biji.push_back(bz);
    }
    biji.last().lkEnd=true;
}

void Image2Bzr2::lisandian2Bezier(std::vector<std::vector<Dian> > &contours, Biji&biji){
    for(uint i=0;i<contours.size();i++){
        //分段
        u16vector fenduanidx;
        fenduan(contours[i],fenduanidx);
        //
        if(xianshifenduan){
            for(uint vi=0;vi<contours[i].size();vi++){
                bzr2 bz;
                bz.x=contours[i][vi].x;
                bz.y=contours[i][vi].y;
                bz.isControl=false;
                if(fenduanidx.indexOf(vi)!=-1){
                    bz.wojia=true;
                }
                biji.push_back(bz);
            }
            biji.last().lkEnd=true;
        }else{
            //拟合
            zuoquxian(contours[i],fenduanidx,biji);
        }
    }
}
