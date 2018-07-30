#ifndef ALGORITHMS_H
#define ALGORITHMS_H

/**
  * @author yuliuchuan
  * @date 2015-04-21
  * @brief 算法
  */

#include "utilities.h"

struct ResectionStruct
{
    int pointcount;//控制点个数
    double *interior;//内方位元素
    double *scale;//像片比例尺分母
    double *limit;//限差
    double *pointpos;//控制点
    double *rotatematrix;//旋转矩阵
    double *exterior;//外方位元素
    double *m0;//单位权中误差
    double *matrix;//方差-协方差矩阵
    double *precision;//精度
};

struct IntersectionStruct
{
    int pointcount;//点数
    double *interior;//内方位元素
    double *samepoint;//同名像点（不包含点名）
    double *leftexterior;//左片外方位元素
    double *rightexterior;//右片外方位元素
    double *result;//结果
};

struct RelativeOrientationStruct
{
    int pointcount;//点数
    double *interior;//内方位元素
    double *m;//像片比例尺分母
    double *limit;//限差
    double *samepoint;//同名像点
    double *result;//相对定向元素
    double *precision;//精度
    double *modelpos;//模型点坐标
};

struct AbsoluteOrientationStruct
{
    int pointcount;//点数
    double *limit;//限差
    double *modelpos;//模型点坐标
    double *controlpos;//控制点地面测量坐标
    double *result;//绝对定向元素
    double *precision;//精度
    double *groundpos;//地面摄测坐标
    double *deltaxyz;//误差
};

//phi-omega-kappa旋转矩阵
double *CreateRotateMatrix(double phi,double omega,double kappa);
//空间后方交会 欧拉角方法
bool runResectionEuler(ResectionStruct rs);
//空间后方交会 单位四元数方法
bool runResectionQuaternion(ResectionStruct rs);
//空间前方交会 - 平差方法
bool runIntersectionAdj(IntersectionStruct is);
//空间前方交会 - 投影系数法
bool runIntersectionPro(IntersectionStruct is);
//相对定向 - 单独法
bool runRelativeOrientationS(RelativeOrientationStruct ros);
//相对定向 - 连续法
bool runRelativeOrientationC(RelativeOrientationStruct ros);
//绝对定向
bool runAbsoluteOrientation(AbsoluteOrientationStruct aos);

void releaseResectionStruct(ResectionStruct rs);

void releaseIntersectionStruct(IntersectionStruct is);

void releaseRelativeOrientationStruct(RelativeOrientationStruct ros);

void releaseAbsoluteOrientationStruct(AbsoluteOrientationStruct aos);
#endif // ALGORITHMS_H
