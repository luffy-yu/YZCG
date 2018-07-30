#include "algorithms.h"
#include "matrix.h"
#include "inverse.h"
#include <QDebug>

using namespace std;
using namespace splab;

typedef double Type;

double *CreateRotateMatrix(double phi, double omega, double kappa)
{
    double *rotate = new double[9];
    rotate[0] =  cos(phi) * cos(kappa) - sin(phi) * sin(omega) * sin(kappa);
    rotate[1] = -cos(phi) * sin(kappa) - sin(phi) * sin(omega) * cos(kappa);
    rotate[2] = -sin(phi) * cos(omega);
    rotate[3] =  cos(omega) * sin(kappa);
    rotate[4] =  cos(omega) * cos(kappa);
    rotate[5] = -sin(omega);
    rotate[6] =  sin(phi) * cos(kappa) + cos(phi) * sin(omega) * sin(kappa);
    rotate[7] = -sin(phi) * sin(kappa) + cos(phi) * sin(omega) * cos(kappa);
    rotate[8] =  cos(phi) * cos(omega);
    return rotate;
}

bool runResectionEuler(ResectionStruct rs)
{
    const int pointcount = rs.pointcount;
    const double *interior = rs.interior;
    const double *scale = rs.scale;
    const double *limit = rs.limit;
    const double *pointpos = rs.pointpos;
    double *rotatematrix = rs.rotatematrix;
    double *exterior = rs.exterior;
    double *m0 = rs.m0;
    double *matrix = rs.matrix;
    double *precision = rs.precision;
    //A L矩阵
    Matrix<Type> A(2 * pointcount,6,0.0);
    Matrix<Type> L(2 * pointcount,1,0.0);
    //ATA的逆
    Matrix<Type> IATA(6,6,0.0);
    //改正数矩阵
    Matrix<Type> Xv(6,1,0.0);
    //控制点矩阵
    Matrix<Type> PointMat(pointcount,5,pointpos);

    //外方位元素初始化
    for(int i = 0;i < 6;i++)
    {
        exterior[i] = 0.0;
    }
    for(int i = 0;i < pointcount;i++)
    {
        PointMat[i][0] /= 1000.0;
        PointMat[i][1] /= 1000.0;
        exterior[0] += PointMat[i][2];
        exterior[1] += PointMat[i][3];
        exterior[2] += PointMat[i][4];
    }
    exterior[0] /= pointcount;
    exterior[1] /= pointcount;
    exterior[2] = exterior[2] / pointcount + (*scale) * interior[2] / 1000.0;

    //初始化参数
    double *Xs = new double(exterior[0]);
    double *Ys = new double(exterior[1]);
    double *Zs = new double(exterior[2]);
    double *phi = new double(0.0);
    double *omega = new double(0.0);
    double *kappa = new double(0.0);
    double *x0 = new double(interior[0] / 1000.0);
    double *y0 = new double(interior[1] / 1000.0);
    double *f = new double(interior[2] / 1000.0);
    //迭代次数
    int *count = new int(0);

    double *X = new double(0.0);
    double *Y = new double(0.0);
    double *Z = new double(0.0);

    double *rorate = NULL;
    do
    {
        (*count)++;
        rorate = CreateRotateMatrix(*phi,*omega,*kappa);
        for(int i = 0; i < pointcount; i++)
        {
            (*X) = rorate[0] * (PointMat[i][2] - (*Xs)) + rorate[3] * (PointMat[i][3] - (*Ys)) + rorate[6] * (PointMat[i][4] - (*Zs));
            (*Y) = rorate[1] * (PointMat[i][2] - (*Xs)) + rorate[4] * (PointMat[i][3] - (*Ys)) + rorate[7] * (PointMat[i][4] - (*Zs));
            (*Z) = rorate[2] * (PointMat[i][2] - (*Xs)) + rorate[5] * (PointMat[i][3] - (*Ys)) + rorate[8] * (PointMat[i][4] - (*Zs));
            L[2*i + 0][0]   = PointMat[i][0] -((*x0) - (*f) * (*X) / (*Z));
            L[2*i + 1][0] = PointMat[i][1] -((*y0) - (*f) * (*Y) / (*Z));

            A[2*i+0][0]  = (rorate[0] * (*f) + rorate[2] * (PointMat[i][0] - (*x0))) / (*Z);
            A[2*i+0][1]  = (rorate[3] * (*f) + rorate[5] * (PointMat[i][0] - (*x0))) / (*Z);
            A[2*i+0][2]  = (rorate[6] * (*f) + rorate[8] * (PointMat[i][0] - (*x0))) / (*Z);
            A[2*i+0][3]  = (PointMat[i][1] - (*y0)) * sin(*omega) - ((PointMat[i][0] - (*x0)) * ((PointMat[i][0] - (*x0)) * cos(*kappa) - (PointMat[i][1] - (*y0)) * sin(*kappa)) / (*f) + (*f) * cos(*kappa)) * cos(*omega);

            A[2*i+0][4]  = -(*f) * sin(*kappa) - (PointMat[i][0] - (*x0)) * ((PointMat[i][0] - (*x0)) * sin(*kappa) + (PointMat[i][1] - (*y0)) * cos(*kappa)) / (*f);
            A[2*i+0][5]  = PointMat[i][1] - (*y0);
            A[2*i+1][0]  = (rorate[1] * (*f) + rorate[2] * (PointMat[i][1] - (*y0))) / (*Z);
            A[2*i+1][1]  = (rorate[4] * (*f) + rorate[5] * (PointMat[i][1] - (*y0))) / (*Z);
            A[2*i+1][2]  = (rorate[7] * (*f) + rorate[8] * (PointMat[i][1] - (*y0))) / (*Z);

            A[2*i+1][3]  = -(PointMat[i][0] - (*x0)) * sin(*omega) - ((PointMat[i][1] - (*y0)) * ((PointMat[i][0] - (*x0)) * cos(*kappa) - (PointMat[i][1] - (*y0)) * sin(*kappa)) / (*f) - (*f) * sin(*kappa)) * cos(*omega);
            A[2*i+1][4] = -(*f) * cos(*kappa) - (PointMat[i][1] - (*y0)) * ((PointMat[i][0] - (*x0)) * sin(*kappa) + (PointMat[i][1] - (*y0)) * cos(*kappa)) / (*f);
            A[2*i+1][5] = -(PointMat[i][0] - (*x0));
        }
        Matrix<Type> AT,ATA;

        AT = trT(A);
        ATA = AT * A;
        IATA = inv(ATA);

        //当逆矩阵不存在时
        if(IATA.size() == 0)
        {
            delete Xs;
            delete Ys;
            delete Zs;
            delete phi;
            delete omega;
            delete kappa;
            delete x0;
            delete y0;
            delete f;
            delete count;
            delete X;
            delete Y;
            delete Z;
            delete []rorate;

            return false;
        }
        Matrix<Type> ATL;
        ATL = AT * L;
        Xv = IATA * ATL;

        (*Xs) += Xv[0][0];
        (*Ys) += Xv[1][0];
        (*Zs) += Xv[2][0];
        (*phi) += Xv[3][0];
        (*omega) += Xv[4][0];
        (*kappa) += Xv[5][0];
    }while(fabs(Xv[0][0]) > (*limit) ||
           fabs(Xv[1][0]) > (*limit) ||
           fabs(Xv[2][0]) > (*limit) ||
           fabs(Xv[3][0]) > (*limit) ||
           fabs(Xv[4][0]) > (*limit) ||
           fabs(Xv[5][0]) > (*limit));
    //外方位元素
    exterior[0] = *Xs;
    exterior[1] = *Ys;
    exterior[2] = *Zs;
    exterior[3] = *phi;
    exterior[4] = *omega;
    exterior[5] = *kappa;
    //旋转矩阵
    rotatematrix[0] = rorate[0];
    rotatematrix[1] = rorate[1];
    rotatematrix[2] = rorate[2];
    rotatematrix[3] = rorate[3];
    rotatematrix[4] = rorate[4];
    rotatematrix[5] = rorate[5];
    rotatematrix[6] = rorate[6];
    rotatematrix[7] = rorate[7];
    rotatematrix[8] = rorate[8];

#ifdef QT_DEBUG
    qDebug()<<"Resection Euler rotatematrix:";
    for(int i = 0;i < 9;i++){
        qDebug()<<rotatematrix[i]<<rs.rotatematrix[i];
    }
#endif

    Matrix<Type> AXv,V,VT,VTV,D;
    AXv = A * Xv;
    V = AXv - L;
    VT = trT(V);
    VTV = VT * V;
    //单位权中误差的平方
    double *m02 = new double(VTV[0][0] / (2 * pointcount - 6));
    *m0 = sqrt(*m02);
    //方差-协方差矩阵
    D = (*m02) * IATA;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6;j++)
        {
            matrix[i * 6 + j] = D[i][j];
        }
        precision[i] = sqrt(D[i][i]);
    }

    delete Xs;
    delete Ys;
    delete Zs;
    delete phi;
    delete omega;
    delete kappa;
    delete x0;
    delete y0;
    delete f;
    delete count;
    delete X;
    delete Y;
    delete Z;
    delete []rorate;

    delete m02;

    return true;
}

bool runResectionQuaternion(ResectionStruct rs)
{
    const int pointcount = rs.pointcount;
    const double *interior = rs.interior;
    const double *scale = rs.scale;
    const double *limit = rs.limit;
    const double *pointpos = rs.pointpos;
    double *rotatematrix = rs.rotatematrix;
    double *exterior = rs.exterior;
    double *m0 = rs.m0;
    double *matrix = rs.matrix;
    double *precision = rs.precision;

    //控制点矩阵
    Matrix<Type> pointMat(pointcount,5,pointpos);
    //外方位元素初始化
    for (int i = 0; i < 6;i++) {
        exterior[i] = 0.0;
    }
    for (int i = 0;i < pointcount;i++) {
        pointMat[i][0] /= 1000.0;
        pointMat[i][1] /= 1000.0;
        exterior[0] += pointMat[i][2];
        exterior[1] += pointMat[i][3];
        exterior[2] += pointMat[i][4];
    }
    exterior[0] /= pointcount;
    exterior[1] /= pointcount;
    exterior[2] = exterior[2] / pointcount + (*scale) * interior[2] / 1000.0;

    //初始化参数
    double *Xs = new double(exterior[0]);
    double *Ys = new double(exterior[1]);
    double *Zs = new double(exterior[2]);
    double *q0 = new double(1.0);
    double *qx = new double(0.0);
    double *qy = new double(0.0);
    double *qz = new double(0.0);

    //临时变量
    //double *x0 = new double(interior[0] / 1000.0);
    //double *y0 = new double(interior[1] / 1000.0);
    double *f = new double(interior[2] / 1000.0);
    double *Xa = new double(0.0);
    double *Ya = new double(0.0);
    double *Za = new double(0.0);
    double *x = new double(0.0);
    double *y = new double(0.0);
    double *X = new double(0.0);
    double *Y = new double(0.0);
    double *Z = new double(0.0);
    double *w = new double(0.0);
    double *a1 = new double(0.0);
    double *a2 = new double(0.0);
    double *a3 = new double(0.0);
    double *b1 = new double(0.0);
    double *b2 = new double(0.0);
    double *b3 = new double(0.0);
    double *c1 = new double(0.0);
    double *c2 = new double(0.0);
    double *c3 = new double(0.0);
    //V = Bx - L;Cx + Wx = 0
    Matrix<Type> B(2 * pointcount,7,0.0);
    Matrix<Type> L(2 * pointcount,1,0.0);
    Matrix<Type> C(1,7,0.0);
    Matrix<Type> Xv(8,1,0.0);
    Matrix<Type> BT,BTB,BTL;
    Matrix<Type> CT;
    Matrix<Type> N(8,8,0.0);
    Matrix<Type> W(8,1,0.0);
    Matrix<Type> NI;
    do {
        *a1 = pow(*q0,2) + pow(*qx,2) - pow(*qy,2) - pow(*qz,2);
        *a2 = 2 *(*qx**qy - *q0**qz);
        *a3 = 2 *(*qx**qz + *q0**qy);
        *b1 = 2 *(*qx**qy + *q0**qz);
        *b2 = pow(*q0,2) - pow(*qx,2) + pow(*qy,2) -pow(*qz,2);
        *b3 = 2 *(*qy**qz - *q0**qx);
        *c1 = 2 *(*qx**qz - *q0**qy);
        *c2 = 2 *(*qy**qz + *q0**qx);
        *c3 = pow(*q0,2) - pow(*qx,2) - pow(*qy,2) + pow(*qz,2);

        for (int i = 0;i < pointcount;i++) {
            *x = pointMat[i][0];
            *y = pointMat[i][1];
            *X = pointMat[i][2];
            *Y = pointMat[i][3];
            *Z = pointMat[i][4];

            *Xa = (*a1) * ((*X) - (*Xs)) + (*b1) * ((*Y) - (*Ys)) + (*c1) * ((*Z) - (*Zs));
            *Ya = (*a2) * ((*X) - (*Xs)) + (*b2) * ((*Y) - (*Ys)) + (*c2) * ((*Z) - (*Zs));
            *Za = (*a3) * ((*X) - (*Xs)) + (*b3) * ((*Y) - (*Ys)) + (*c3) * ((*Z) - (*Zs));

            B[2 * i + 0][0] = -((*f)*((*Xa)*(*a3) - (*Za)*(*a1)))/pow((*Za),2);
            B[2 * i + 0][1] = -((*f)*((*Xa)*(*b3) - (*Za)*(*b1)))/pow((*Za),2);
            B[2 * i + 0][2] = -((*f)*((*Xa)*(*c3) - (*Za)*(*c1)))/pow((*Za),2);
            B[2 * i + 0][3] = ((*Xa)*(*f)*(2*(*qy)*((*X) - (*Xs)) - 2*(*qx)*((*Y) - (*Ys)) + 2*(*q0)*((*Z) - (*Zs))))/pow((*Za),2) - ((*f)*(2*(*q0)*((*X) - (*Xs)) + 2*(*qz)*((*Y) - (*Ys)) - 2*(*qy)*((*Z) - (*Zs))))/(*Za);
            B[2 * i + 0][4] = - ((*f)*(2*(*qx)*((*X) - (*Xs)) + 2*(*qy)*((*Y) - (*Ys)) + 2*(*qz)*((*Z) - (*Zs))))/(*Za) - ((*Xa)*(*f)*(2*(*q0)*((*Y) - (*Ys)) - 2*(*qz)*((*X) - (*Xs)) + 2*(*qx)*((*Z) - (*Zs))))/pow((*Za),2);
            B[2 * i + 0][5] = ((*f)*(2*(*qy)*((*X) - (*Xs)) - 2*(*qx)*((*Y) - (*Ys)) + 2*(*q0)*((*Z) - (*Zs))))/(*Za) + ((*Xa)*(*f)*(2*(*q0)*((*X) - (*Xs)) + 2*(*qz)*((*Y) - (*Ys)) - 2*(*qy)*((*Z) - (*Zs))))/pow((*Za),2);
            B[2 * i + 0][6] = ((*Xa)*(*f)*(2*(*qx)*((*X) - (*Xs)) + 2*(*qy)*((*Y) - (*Ys)) + 2*(*qz)*((*Z) - (*Zs))))/pow((*Za),2) - ((*f)*(2*(*q0)*((*Y) - (*Ys)) - 2*(*qz)*((*X) - (*Xs)) + 2*(*qx)*((*Z) - (*Zs))))/(*Za);

            B[2 * i + 1][0] = -((*f)*((*Ya)*(*a3) - (*Za)*(*a2)))/pow((*Za),2);
            B[2 * i + 1][1] = -((*f)*((*Ya)*(*b3) - (*Za)*(*b2)))/pow((*Za),2);
            B[2 * i + 1][2] = -((*f)*((*Ya)*(*c3) - (*Za)*(*c2)))/pow((*Za),2);
            B[2 * i + 1][3] = ((*Ya)*(*f)*(2*(*qy)*((*X) - (*Xs)) - 2*(*qx)*((*Y) - (*Ys)) + 2*(*q0)*((*Z) - (*Zs))))/pow((*Za),2) - ((*f)*(2*(*q0)*((*Y) - (*Ys)) - 2*(*qz)*((*X) - (*Xs)) + 2*(*qx)*((*Z) - (*Zs))))/(*Za);
            B[2 * i + 1][4] = - ((*f)*(2*(*qy)*((*X) - (*Xs)) - 2*(*qx)*((*Y) - (*Ys)) + 2*(*q0)*((*Z) - (*Zs))))/(*Za) - ((*Ya)*(*f)*(2*(*q0)*((*Y) - (*Ys)) - 2*(*qz)*((*X) - (*Xs)) + 2*(*qx)*((*Z) - (*Zs))))/pow((*Za),2);
            B[2 * i + 1][5] = ((*Ya)*(*f)*(2*(*q0)*((*X) - (*Xs)) + 2*(*qz)*((*Y) - (*Ys)) - 2*(*qy)*((*Z) - (*Zs))))/pow((*Za),2) - ((*f)*(2*(*qx)*((*X) - (*Xs)) + 2*(*qy)*((*Y) - (*Ys)) + 2*(*qz)*((*Z) - (*Zs))))/(*Za);
            B[2 * i + 1][6] = ((*f)*(2*(*q0)*((*X) - (*Xs)) + 2*(*qz)*((*Y) - (*Ys)) - 2*(*qy)*((*Z) - (*Zs))))/(*Za) + ((*Ya)*(*f)*(2*(*qx)*((*X) - (*Xs)) + 2*(*qy)*((*Y) - (*Ys)) + 2*(*qz)*((*Z) - (*Zs))))/pow((*Za),2);

            L[2 * i + 0][0] = *x - (-(*f) * (*Xa) / (*Za));
            L[2 * i + 1][0] = *y - (-(*f) * (*Ya) / (*Za));
        }
        *w = 1.0 / 2 * (pow(*q0,2) + pow(*qx,2) + pow(*qy,2) + pow(*qz,2) - 1);
        C[0][3] = (*q0);
        C[0][4] = (*qx);
        C[0][5] = (*qy);
        C[0][6] = (*qz);

        //Matrix<Type> BT,BTB,BTL;
        BT = trT(B);
        BTB = BT * B;
        BTL = BT * L;
        //Matrix<Type> CT;
        CT = trT(C);

        //Matrix<Type> N(8,8,0.0);
        for (int i = 0;i < 7;i++) {
            for (int j = 0;j < 7;j++) {
                N[i][j] = BTB[i][j];
            }
            N[i][7] = CT[i][0];
            N[7][i] = C[0][i];
        }
        //Matrix<Type> W(8,1,0.0);
        for (int i = 0;i < 7;i++) {
            W[i][0] = BTL[i][0];
        }
        W[7][0] = -(*w);
        //Matrix<Type> NI;
        NI = inv(N);
        if (NI.size() == 0) {
            delete Xs;
            delete Ys;
            delete Zs;
            delete q0;
            delete qx;
            delete qy;
            delete qz;
            //delete x0;
            //delete y0;
            delete f;
            delete Xa;
            delete Ya;
            delete Za;
            delete x;
            delete y;
            delete X;
            delete Y;
            delete Z;
            delete w;
            delete a1;
            delete a2;
            delete a3;
            delete b1;
            delete b2;
            delete b3;
            delete c1;
            delete c2;
            delete c3;

            return false;
        }
        Xv = NI * W;

        *Xs += Xv[0][0];
        *Ys += Xv[1][0];
        *Zs += Xv[2][0];
        *q0 += Xv[3][0];
        *qx += Xv[4][0];
        *qy += Xv[5][0];
        *qz += Xv[6][0];

    }while(fabs(Xv[0][0]) > (*limit) ||
           fabs(Xv[1][0]) > (*limit) ||
           fabs(Xv[2][0]) > (*limit) ||
           fabs(Xv[3][0]) > (*limit) ||
           fabs(Xv[4][0]) > (*limit) ||
           fabs(Xv[5][0]) > (*limit) ||
           fabs(Xv[6][0]) > (*limit));
    exterior[0] = *Xs;
    exterior[1] = *Ys;
    exterior[2] = *Zs;

    *a1 = pow(*q0,2) + pow(*qx,2) - pow(*qy,2) - pow(*qz,2);
    *a2 = 2 *(*qx**qy - *q0**qz);
    *a3 = 2 *(*qx**qz + *q0**qy);
    *b1 = 2 *(*qx**qy + *q0**qz);
    *b2 = pow(*q0,2) - pow(*qx,2) + pow(*qy,2) -pow(*qz,2);
    *b3 = 2 *(*qy**qz - *q0**qx);
    *c1 = 2 *(*qx**qz - *q0**qy);
    *c2 = 2 *(*qy**qz + *q0**qx);
    *c3 = pow(*q0,2) - pow(*qx,2) - pow(*qy,2) + pow(*qz,2);

    exterior[3] = atan(-(*a3)/(*c3));
    exterior[4] = -(*b3);
    exterior[5] = (*b1)/(*b2);

    rotatematrix[0] = *a1;
    rotatematrix[1] = *a2;
    rotatematrix[2] = *a3;
    rotatematrix[3] = *b1;
    rotatematrix[4] = *b2;
    rotatematrix[5] = *b3;
    rotatematrix[6] = *c1;
    rotatematrix[7] = *c2;
    rotatematrix[8] = *c3;

    //精度评定
    Matrix<Type> Xv2(7,1,0.0);
    for (int i = 0;i < 7;i++) {
        Xv2[i][0] = Xv[i][0];
    }
    Matrix<Type> BXv2,V,VT,VTV;
    BXv2 = B * Xv2;
    V = BXv2 - L;
    VT = trT(V);
    VTV = VT * V;
    //VTPV/(n - (u - s)) u个参数，s个联系数
    double *m02 = new double(VTV[0][0] / (2 *pointcount - (7 - 1)));
    *m0 = sqrt(*m02);
    Matrix<Type> NBBI,NBBICT,CNBBI,NCC,NCCI,Qxx,NBBICTNCCI,NBBICTNCCIC,NBBICTNCCICNBBI;
    NBBI = inv(BTB);
    if (NBBI.size() == 0)
        NBBI.resize(7,7);
    NBBICT = NBBI * CT;

    CNBBI = C * NBBI;
    NCC = CNBBI * CT;
    NCCI = inv(NCC);
    if (NCCI.size() == 0)
        NCCI.resize(1,1);

    NBBICTNCCI = NBBICT * NCCI;
    NBBICTNCCIC = NBBICTNCCI * C;
    NBBICTNCCICNBBI = NBBICTNCCIC * NBBI;
    Qxx = NBBI - NBBICTNCCICNBBI;

    Matrix<Type> D;
    D = (*m02) * Qxx;
    precision[0] = sqrt(D[0][0]);
    precision[1] = sqrt(D[1][1]);
    precision[2] = sqrt(D[2][2]);
    precision[3] = sqrt(pow(-((2*(*qy))/(*c3) - (2*(*q0)*(*a3))/pow((*c3),2))/(pow((*a3)/(*c3),2) + 1),2) * D[3][3] +
            pow(-((2*(*qz))/(*c3) + (2*(*qx)*(*a3))/pow((*c3),2))/(pow((*a3)/(*c3),2) + 1),2) * D[4][4] +
            pow(-((2*(*q0))/(*c3) + (2*(*qy)*(*a3))/pow((*c3),2))/(pow((*a3)/(*c3),2) + 1),2) * D[5][5] +
            pow(-((2*(*qx))/(*c3) - (2*(*qz)*(*a3))/pow((*c3),2))/(pow((*a3)/(*c3),2) + 1),2) * D[6][6]);
    precision[4] = sqrt(pow(2*(*qx),2) * D[3][3] +
            pow(2*(*q0),2) * D[4][4] +
            pow(-2*(*qz),2) * D[5][5] +
            pow(-2*(*qy),2) * D[6][6]);
    precision[5] = sqrt(pow((2*(*qz))/(*b2) - (2*(*q0)*(*b1))/pow((*b2),2),2) * D[3][3] +
            pow((2*(*qy))/(*b2) + (2*(*qx)*(*b1))/pow((*b2),2),2) * D[4][4] +
            pow((2*(*qx))/(*b2) - (2*(*qy)*(*b1))/pow((*b2),2),2) * D[5][5] +
            pow((2*(*q0))/(*b2) + (2*(*qz)*(*b1))/pow((*b2),2),2) * D[6][6]);

    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6;j++)
        {
            matrix[i * 6 + j] = 0.0;
        }
    }

    delete Xs;
    delete Ys;
    delete Zs;
    delete q0;
    delete qx;
    delete qy;
    delete qz;
    //delete x0;
    //delete y0;
    delete f;
    delete Xa;
    delete Ya;
    delete Za;
    delete x;
    delete y;
    delete X;
    delete Y;
    delete Z;
    delete w;
    delete a1;
    delete a2;
    delete a3;
    delete b1;
    delete b2;
    delete b3;
    delete c1;
    delete c2;
    delete c3;

    delete m02;

    return true;
}

bool runIntersectionAdj(IntersectionStruct is)
{
    const int pointcount = is.pointcount;
    const double *interior = is.interior;
    const double *samepoint = is.samepoint;
    const double *leftexterior = is.leftexterior;
    const double *rightexterior = is.rightexterior;
    double *result = is.result;

    //内方位元素
    double *x0 = new double(interior[0]);
    double *y0 = new double(interior[1]);
    double *f = new double(interior[2]);
    //旋转矩阵
    double *leftrotate = CreateRotateMatrix(leftexterior[3],leftexterior[4],leftexterior[5]);
    double *rightrotate = CreateRotateMatrix(rightexterior[3],rightexterior[4],rightexterior[5]);
    //结果
    double *res = new double[pointcount * 3];

    for(int i = 0;i < pointcount;i++)
    {
        Matrix<Type> A(4,3,0.0);
        Matrix<Type> L(4,1,0.0);

        A[0][0] = leftrotate[2] * (samepoint[i * 4 + 0] - (*x0)) + leftrotate[0] * (*f);
        A[0][1] = leftrotate[5] * (samepoint[i * 4 + 0] - (*x0)) + leftrotate[3] * (*f);
        A[0][2] = leftrotate[8] * (samepoint[i * 4 + 0] - (*x0)) + leftrotate[6] * (*f);

        A[1][0] = leftrotate[2] * (samepoint[i * 4 + 1] - (*y0)) + leftrotate[1] * (*f);
        A[1][1] = leftrotate[5] * (samepoint[i * 4 + 1] - (*y0)) + leftrotate[4] * (*f);
        A[1][2] = leftrotate[8] * (samepoint[i * 4 + 1] - (*y0)) + leftrotate[7] * (*f);

        A[2][0] = rightrotate[2] * (samepoint[i * 4 + 2] - (*x0)) + rightrotate[0] * (*f);
        A[2][1] = rightrotate[5] * (samepoint[i * 4 + 2] - (*x0)) + rightrotate[3] * (*f);
        A[2][2] = rightrotate[8] * (samepoint[i * 4 + 2] - (*x0)) + rightrotate[6] * (*f);

        A[3][0] = rightrotate[2] * (samepoint[i * 4 + 3] - (*y0)) + rightrotate[1] * (*f);
        A[3][1] = rightrotate[5] * (samepoint[i * 4 + 3] - (*y0)) + rightrotate[4] * (*f);
        A[3][2] = rightrotate[8] * (samepoint[i * 4 + 3] - (*y0)) + rightrotate[7] * (*f);

        L[0][0] = A[0][0] * leftexterior[0] + A[0][1] * leftexterior[1] + A[0][2] * leftexterior[2];
        L[1][0] = A[1][0] * leftexterior[0] + A[1][1] * leftexterior[1] + A[1][2] * leftexterior[2];
        L[2][0] = A[2][0] * rightexterior[0] + A[2][1] * rightexterior[1] + A[2][2] * rightexterior[2];
        L[3][0] = A[3][0] * rightexterior[0] + A[3][1] * rightexterior[1] + A[3][2] * rightexterior[2];

        Matrix<Type> AT,ATA,IATA;
        AT = trT(A);
        ATA = AT * A;
        IATA = inv(ATA);
        if(IATA.size() == 0)
        {
            res[i * 3 + 0] = -1;
            res[i * 3 + 1] = -1;
            res[i * 3 + 2] = -1;

            continue;
        }
        Matrix<Type> ATL,X;
        ATL = AT * L;
        X = IATA * ATL;
        res[i * 3 + 0] = X[0][0];
        res[i * 3 + 1] = X[1][0];
        res[i * 3 + 2] = X[2][0];
    }
    for(int i = 0;i < pointcount * 3;i++)
    {
        result[i] = res[i];
    }
    delete x0;
    delete y0;
    delete f;

    delete []leftrotate;
    delete []rightrotate;
    delete []res;

    return true;
}

bool runIntersectionPro(IntersectionStruct is)
{
    const int pointcount = is.pointcount;
    const double *interior = is.interior;
    const double *samepoint = is.samepoint;
    const double *leftexterior = is.leftexterior;
    const double *rightexterior = is.rightexterior;
    double *result = is.result;

    //左片旋转矩阵
    double *leftrotate = CreateRotateMatrix(leftexterior[3],leftexterior[4],leftexterior[5]);
    Matrix<Type> mat1(3,3,leftrotate);
    //右片旋转矩阵
    double *rightrotate = CreateRotateMatrix(rightexterior[3],rightexterior[4],rightexterior[5]);
    Matrix<Type> mat2(3,3,rightrotate);
    //像空间辅助坐标系中的坐标
    Matrix<Type> XYZ1(3,1,0.0);
    Matrix<Type> XYZ2(3,1,0.0);
    //像空间坐标系中的坐标
    Matrix<Type> xynf1(3,1,0.0);
    Matrix<Type> xynf2(3,1,0.0);
    //基线向量
    Matrix<Type> B(3,1,0.0);
    B[0][0] = rightexterior[0] - leftexterior[0];
    B[1][0] = rightexterior[1] - leftexterior[1];
    B[2][0] = rightexterior[2] - leftexterior[2];
    //点投影系数
    double *N1 = new double(0.0);
    double *N2 = new double(0.0);
    //-f
    double *nf = new double(-interior[2]);
    //地面点空间坐标
    double *res = new double[pointcount * 3];

    xynf1[2][0] = *nf;
    xynf2[2][0] = *nf;

    for(int i = 0;i < pointcount;i++)
    {
        xynf1[0][0] = samepoint[i * 4 + 0] - interior[0];
        xynf1[1][0] = samepoint[i * 4 + 1] - interior[1];

        xynf2[0][0] = samepoint[i * 4 + 2] - interior[0];
        xynf2[1][0] = samepoint[i * 4 + 3] - interior[1];

        XYZ1 = mat1 * xynf1;
        XYZ2 = mat2 * xynf2;

        *N1 = (B[0][0] * XYZ2[2][0] - B[2][0] * XYZ2[0][0]) / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);
        *N2 = (B[0][0] * XYZ1[2][0] - B[2][0] * XYZ1[0][0]) / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);

        res[i * 3 + 0] = ((leftexterior[0] + (*N1) * XYZ1[0][0]) +(rightexterior[0] + (*N2) * XYZ2[0][0])) / 2;
        res[i * 3 + 1] = ((leftexterior[1] + (*N1) * XYZ1[1][0]) +(rightexterior[1] + (*N2) * XYZ2[1][0])) / 2;
        res[i * 3 + 2] = ((leftexterior[2] + (*N1) * XYZ1[2][0]) +(rightexterior[2] + (*N2) * XYZ2[2][0])) / 2;
    }
    for(int i = 0; i < pointcount * 3;i++)
    {
        result[i] = res[i];
    }

    delete []leftrotate;
    delete []rightrotate;

    delete N1;
    delete N2;
    delete nf;
    delete []res;

    return true;
}

bool runRelativeOrientationS(RelativeOrientationStruct ros)
{

    const int pointcount = ros.pointcount;
    const double *interior = ros.interior;
    const double *m = ros.m;
    const double *limit = ros.limit;
    const double *samepoint = ros.samepoint;
    double *result = ros.result;
    double *precision = ros.precision;
    double *modelpos = ros.modelpos;

    //相对定向元素初始化
    double *phi1 = new double(0.0);
    double *kappa1 = new double(0.0);
    double *phi2 = new double(0.0);
    double *omega2 = new double(0.0);
    double *kappa2 = new double(0.0);

    Matrix<Type> A(pointcount,5,0.0);
    Matrix<Type> L(pointcount,1,0.0);
    Matrix<Type> IATA(5,5,0.0);
    Matrix<Type> Xv(5,1,0.0);
    Matrix<Type> XYZ1(3,1,0.0);
    Matrix<Type> XYZ2(3,1,0.0);

    Matrix<Type> Leftxynf(3,1,0.0);
    Matrix<Type> Rightxynf(3,1,0.0);

    Leftxynf[2][0] = -interior[2];
    Rightxynf[2][0] = -interior[2];

    Matrix<Type> Point(pointcount,4,samepoint);
    for(int i = 0;i < pointcount;i++)
    {
        Point[i][0] -= interior[0];
        Point[i][1] -= interior[1];
        Point[i][2] -= interior[0];
        Point[i][3] -= interior[1];
    }
    int *count = new int(0);//迭代次数
    do
    {
        (*count)++;

        double *leftrotate = CreateRotateMatrix(*phi1,0.0,*kappa1);
        double *rightrotate = CreateRotateMatrix(*phi2,*omega2,*kappa2);

        Matrix<Type> leftmat(3,3,leftrotate);
        Matrix<Type> rightmat(3,3,rightrotate);

        for(int i = 0;i < pointcount;i++)
        {
            Leftxynf[0][0] = Point[i][0];
            Leftxynf[1][0] = Point[i][1];

            Rightxynf[0][0] = Point[i][2];
            Rightxynf[1][0] = Point[i][3];

            XYZ1 = leftmat * Leftxynf;
            XYZ2 = rightmat * Rightxynf;

            A[i][0] =  (XYZ1[0][0] * XYZ2[1][0]) / XYZ1[2][0];
            A[i][1] = -XYZ1[0][0];
            A[i][2] = -(XYZ2[0][0] * XYZ1[1][0]) / XYZ2[2][0];
            A[i][3] = -(XYZ2[2][0] + (XYZ1[1][0] * XYZ2[1][0]) / XYZ2[2][0]);
            A[i][4] =  XYZ2[0][0];

            L[i][0] = -interior[2] * (XYZ1[1][0] / XYZ1[2][0] - XYZ2[1][0] / XYZ2[2][0]);
        }
        Matrix<Type> AT,ATA;
        AT = trT(A);
        ATA = AT * A;

        IATA = inv(ATA);
        if(IATA.size() == 0)
        {
            delete phi1;
            delete kappa1;
            delete phi2;
            delete omega2;
            delete kappa2;

            delete count;

            delete leftrotate;
            delete rightrotate;

            return false;
        }
        Matrix<Type> ATL;

        ATL = AT * L;
        Xv = IATA * ATL;

        *phi1 += Xv[0][0];
        *kappa1 += Xv[1][0];
        *phi2 += Xv[2][0];
        *omega2 += Xv[3][0];
        *kappa2 += Xv[4][0];

        delete leftrotate;
        delete rightrotate;

    }while(fabs(Xv[0][0]) > *limit ||
           fabs(Xv[1][0]) > *limit ||
           fabs(Xv[2][0]) > *limit ||
           fabs(Xv[3][0]) > *limit ||
           fabs(Xv[4][0]) > *limit);

    result[0] = *phi1;
    result[1] = *kappa1;
    result[2] = *phi2;
    result[3] = *omega2;
    result[4] = *kappa2;
    //计算精度
    Matrix<Type> AXv,V,VT,VTV,D;
    AXv = A * Xv;
    V = AXv - L;
    VT = trT(V);
    VTV = VT * V;
    //单位权中误差的平方
    double *m02 = new double(VTV[0][0] / (pointcount - 5));
    //double *m0 = sqrt(*m02);
    //方差 - 协方差矩阵
    D = (*m02) * IATA;
    for(int i = 0;i < 5;i++)
    {
        precision[i] = sqrt(D[i][i]);
    }
    //计算模型点坐标
    double *lrotate = CreateRotateMatrix(*phi1,0,*kappa1);
    double *rrotate = CreateRotateMatrix(*phi2,*omega2,*kappa2);
    Matrix<Type> rotatel(3,3,lrotate);
    Matrix<Type> rotater(3,3,rrotate);

    double *N1 = new double(0.0);
    double *N2 = new double(0.0);
    double *b = new double(Point[0][0] - Point[0][2]);
    //qDebug()<<"modelpos";
    for(int i = 0;i < pointcount;i++)
    {
        Leftxynf[0][0] = Point[i][0];
        Leftxynf[1][0] = Point[i][1];

        Rightxynf[0][0] = Point[i][2];
        Rightxynf[1][0] = Point[i][3];

        XYZ1 = rotatel * Leftxynf;
        XYZ2 = rotater * Rightxynf;

        *N1 = (*b) * XYZ2[2][0] / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);
        *N2 = (*b) * XYZ1[2][0] / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);

        modelpos[i * 3 + 0] = (*m) * (*N1) * XYZ1[0][0];
        modelpos[i * 3 + 1] = (1/2.0) * (*m) * ((*N1) * XYZ1[1][0] + (*N2) * XYZ2[1][0] + 0);//by = 0
        modelpos[i * 3 + 2] = (*m) * interior[2] + (*m) * (*N1) * XYZ1[2][0];
        //qDebug()<<modelpos[i * 3 + 0]<<"-"<<modelpos[i * 3 + 1]<<"-"<<modelpos[i * 3 + 2];
    }

    delete phi1;
    delete kappa1;
    delete phi2;
    delete omega2;
    delete kappa2;

    delete count;

    delete m02;
    delete []lrotate;
    delete []rrotate;

    delete N1;
    delete N2;
    delete b;

    return true;
}

bool runRelativeOrientationC(RelativeOrientationStruct ros)
{
    const int pointcount = ros.pointcount;
    const double *interior = ros.interior;
    const double *m = ros.m;
    const double *limit = ros.limit;
    const double *samepoint = ros.samepoint;
    double *result = ros.result;
    double *precision = ros.precision;
    double *modelpos = ros.modelpos;

    //相对定向元素初始化
    double *mu = new double(0.0);
    double *nu = new double(0.0);
    double *phi = new double(0.0);
    double *omega = new double(0.0);
    double *kappa = new double(0.0);

    Matrix<Type> A(pointcount,5,0.0);
    Matrix<Type> L(pointcount,1,0.0);
    Matrix<Type> IATA(5,5,0.0);
    Matrix<Type> Xv(5,1,0.0);
    Matrix<Type> XYZ1(3,1,0.0);
    Matrix<Type> XYZ2(3,1,0.0);

    Matrix<Type> Leftxynf(3,1,0.0);
    Matrix<Type> Rightxynf(3,1,0.0);

    Leftxynf[2][0] = -interior[2];
    Rightxynf[2][0] = -interior[2];

    Matrix<Type> Point(pointcount,4,samepoint);
    double *bx = new double(Point[0][0] - Point[0][2]);
    //double *bx = new double(14.4288);
    double *by = new double(0.0);
    double *bz = new double(0.0);
    double *N1 = new double(0.0);
    double *N2 = new double(0.0);

    for(int i = 0;i < pointcount;i++)
    {
        Point[i][0] -= interior[0];
        Point[i][1] -= interior[1];
        Point[i][2] -= interior[0];
        Point[i][3] -= interior[1];
    }

    int *count = new int(0);//迭代次数
    double *leftrotate = CreateRotateMatrix(0.0,0.0,0.0);
    Matrix<Type> leftmat(3,3,leftrotate);
    do
    {
        (*count)++;
        double *rightrotate = CreateRotateMatrix(*phi,*omega,*kappa);
        Matrix<Type> rightmat(3,3,rightrotate);
        for(int i = 0;i < pointcount;i++)
        {
            Leftxynf[0][0] = Point[i][0];
            Leftxynf[0][1] = Point[i][1];

            Rightxynf[0][0] = Point[i][2];
            Rightxynf[1][0] = Point[i][3];

            XYZ1 = leftmat * Leftxynf;
            XYZ2 = rightmat * Rightxynf;

            *by = (*bx) * (*mu);
            *bz = (*bx) * (*nu);

            *N1 = ((*bx) * XYZ2[2][0] - (*bz) * XYZ2[0][0]) / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);
            *N2 = ((*bx) * XYZ1[2][0] - (*bz) * XYZ1[0][0]) / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);

            A[i][0] =  (*bx);
            A[i][1] = -(XYZ2[1][0] / XYZ2[2][0]) * (*bx);
            A[i][2] = -(XYZ2[0][0] * XYZ2[1][0] / XYZ2[2][0]) * (*N2);
            A[i][3] = -(XYZ2[2][0] + XYZ2[1][0] * XYZ2[1][0] / XYZ2[2][0]) * (*N2);
            A[i][4] = XYZ2[0][0] * (*N2);

            L[i][0] = (*N1) * XYZ1[1][0] - (*N2) * XYZ2[1][0] - (*by);
        }

        Matrix<Type> AT,ATA;
        AT = trT(A);
        ATA = AT * A;
        IATA = inv(ATA);
        if(IATA.size() == 0)
        {
            delete bx;
            delete by;
            delete bz;
            delete N1;
            delete N2;
            delete count;

            delete leftrotate;
            delete rightrotate;

            return false;
        }

        Matrix<Type> ATL;
        ATL = AT * L;
        Xv = IATA * ATL;

        *mu += Xv[0][0];
        *nu += Xv[1][0];
        *phi += Xv[2][0];
        *omega += Xv[3][0];
        *kappa += Xv[4][0];

        delete rightrotate;

    }while(fabs(Xv[0][0]) > (*limit) ||
           fabs(Xv[1][0]) > (*limit) ||
           fabs(Xv[2][0]) > (*limit) ||
           fabs(Xv[3][0]) > (*limit) ||
           fabs(Xv[4][0]) > (*limit));

    result[0] = (*m) * (*bx) * tan(*mu);
    result[1] = (*m) * (*bx) / cos(*mu) * tan(*nu);
    result[2] = *phi;
    result[3] = *omega;
    result[4] = *kappa;

    //计算精度
    Matrix<Type> AXv,V,VT,VTV,D;
    AXv = A * Xv;
    V = AXv - L;
    VT = trT(V);
    VTV = VT * V;
    //单位权中误差的平方
    double *m02 = new double(VTV[0][0] / (pointcount - 5));
    //double *m0 = sqrt(*m02);
    //方差 - 协方差矩阵
    D = (*m02) * IATA;
    for(int i = 0;i < 5;i++)
    {
        precision[i] = sqrt(D[i][i]);
    }
    //计算模型点坐标
    double *rrotate = CreateRotateMatrix(*phi,*omega,*kappa);
    Matrix<Type> rightmat(3,3,rrotate);
    for(int i = 0;i < pointcount;i++)
    {
        Leftxynf[0][0] = Point[i][0];
        Leftxynf[0][1] = Point[i][1];

        Rightxynf[0][0] = Point[i][2];
        Rightxynf[1][0] = Point[i][3];

        XYZ1 = leftmat * Leftxynf;
        XYZ2 = rightmat * Rightxynf;

        *by = (*bx) * (*mu);
        *bz = (*bx) * (*nu);

        *N1 = ((*bx) * XYZ2[2][0] - (*bz) * XYZ2[0][0]) / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);
        *N2 = ((*bx) * XYZ1[2][0] - (*bz) * XYZ1[0][0]) / (XYZ1[0][0] * XYZ2[2][0] - XYZ2[0][0] * XYZ1[2][0]);

        modelpos[i * 3 + 0] = (*m) * (*N1) * XYZ1[0][0];
        modelpos[i * 3 + 1] = (1/2.0) * (*m) * ((*N1) * XYZ1[1][0] + (*N2) * XYZ2[1][0] + (*by));//by = 0
        modelpos[i * 3 + 2] = (*m) * interior[2] + (*m) * (*N1) * XYZ1[2][0];
    }
    delete mu;
    delete nu;
    delete phi;
    delete omega;
    delete kappa;

    delete bx;
    delete by;
    delete bz;
    delete N1;
    delete N2;
    delete count;

    delete []leftrotate;
    delete m02;

    delete []rrotate;

    return true;
}

bool runAbsoluteOrientation(AbsoluteOrientationStruct aos)
{
    const int pointcount = aos.pointcount;
    const double *limit = aos.limit;
    const double *modelpos = aos.modelpos;
    const double *controlpos = aos.controlpos;
    double *result = aos.result;
    double *precision = aos.precision;
    double *groundpos = aos.groundpos;
    double *deltaxyz = aos.deltaxyz;

    //将控制点地面测量坐标转换为地面摄测坐标
    double *scalem = new double(0.0);
    double *scalec = new double(0.0);
    *scalem = sqrt((modelpos[0] - modelpos[3]) * (modelpos[0] - modelpos[3]) +
            (modelpos[1] - modelpos[4]) * (modelpos[1] - modelpos[4]) +
            (modelpos[2] - modelpos[5]) * (modelpos[2] - modelpos[5]));

    *scalec = sqrt((controlpos[0] - controlpos[3]) * (controlpos[0] - controlpos[3]) +
            (controlpos[1] - controlpos[4]) * (controlpos[1] - controlpos[4]) +
            (controlpos[2] - controlpos[5]) * (controlpos[2] - controlpos[5]));

    Matrix<Type> modelmat(pointcount,3,modelpos);//摄测坐标
    Matrix<Type> controlmat(pointcount,3,controlpos);//控制点地面测量坐标
    for(int i = 0;i < pointcount;i++)
    {
        for(int j = 0;j < 3;j++)
        {
            modelmat[i][j] *= ((*scalec) / (*scalem));//地面摄测坐标
        }
    }

    delete scalem;
    delete scalec;

    //初始化绝对定向元素
    double *deltaX = new double(0.0);
    double *deltaY = new double(0.0);
    double *deltaZ = new double(0.0);
    double *lambda = new double(1.0);
    double *phi = new double(0.0);
    double *omega = new double(0.0);
    double *kappa = new double(0.0);
    //计算地面摄测坐标重心和重心化地面摄测坐标
    double *sumx = new double(0.0);
    double *sumy = new double(0.0);
    double *sumz = new double(0.0);
    for(int i = 0;i < pointcount;i++)
    {
        *sumx += controlmat[i][0];
        *sumy += controlmat[i][1];
        *sumz += controlmat[i][2];
    }

    double *avgxc = new double(*sumx / (pointcount + 0.0));
    double *avgyc = new double(*sumy / (pointcount + 0.0));
    double *avgzc = new double(*sumz / (pointcount + 0.0));
    for(int i = 0;i < pointcount;i++)
    {
        controlmat[i][0] -= (*avgxc);
        controlmat[i][1] -= (*avgyc);
        controlmat[i][2] -= (*avgzc);
    }
    //计算摄测坐标重心和重心化摄测坐标
    for(int i = 0;i < pointcount;i++)
    {
        *sumx += modelmat[i][0];
        *sumy += modelmat[i][1];
        *sumz += modelmat[i][2];
    }
    double *avgxm = new double(*sumx / (pointcount + 0.0));
    double *avgym = new double(*sumy / (pointcount + 0.0));
    double *avgzm = new double(*sumz / (pointcount + 0.0));
    for(int i = 0;i < pointcount;i++)
    {
        modelmat[i][0] -= (*avgxm);
        modelmat[i][1] -= (*avgym);
        modelmat[i][2] -= (*avgzm);
    }
    //误差方程式
    Matrix<Type> A(3 * pointcount,7,0.0);
    for(int i = 0;i < pointcount;i++)
    {
        A[i * 3 + 0][0] =  1.0;
        A[i * 3 + 0][1] =  0.0;
        A[i * 3 + 0][2] =  0.0;
        A[i * 3 + 0][3] =  modelmat[i][0];
        A[i * 3 + 0][4] = -modelmat[i][2];
        A[i * 3 + 0][5] =  0.0;
        A[i * 3 + 0][6] = -modelmat[i][1];
        A[i * 3 + 1][0] =  0.0;
        A[i * 3 + 1][1] =  1.0;
        A[i * 3 + 1][2] =  0.0;
        A[i * 3 + 1][3] =  modelmat[i][1];
        A[i * 3 + 1][4] =  0.0;
        A[i * 3 + 1][5] = -modelmat[i][2];
        A[i * 3 + 1][6] =  modelmat[i][0];
        A[i * 3 + 2][0] =  0.0;
        A[i * 3 + 2][1] =  0.0;
        A[i * 3 + 2][2] =  1.0;
        A[i * 3 + 2][3] =  modelmat[i][2];
        A[i * 3 + 2][4] =  modelmat[i][0];
        A[i * 3 + 2][5] =  modelmat[i][1];
        A[i * 3 + 2][6] =  0.0;
    }
    Matrix<Type> AT,ATA,IATA;
    AT = trT(A);
    ATA = AT * A;
    IATA = inv(ATA);
    if(IATA.size() == 0)
    {
        delete deltaX;
        delete deltaY;
        delete deltaZ;
        delete lambda;
        delete phi;
        delete omega;
        delete kappa;
        delete sumx;
        delete sumy;
        delete sumz;
        delete avgxc;
        delete avgyc;
        delete avgzc;
        delete avgxm;
        delete avgym;
        delete avgzm;

        return false;
    }
    Matrix<Type> L(3 * pointcount,1,0.0);
    Matrix<Type> ATL,Xv;
    int *count = new int(0);
    do
    {
        (*count)++;
        double *rmat = CreateRotateMatrix(*phi,*omega,*kappa);
        for(int i = 0;i < pointcount;i++)
        {
            L[i * 3 + 0][0] = controlmat[i][0] - (*lambda) * (rmat[0] * modelmat[i][0] + rmat[1] * modelmat[i][1] + rmat[2] * modelmat[i][2]) - (*deltaX);
            L[i * 3 + 1][0] = controlmat[i][1] - (*lambda) * (rmat[3] * modelmat[i][0] + rmat[4] * modelmat[i][1] + rmat[5] * modelmat[i][2]) - (*deltaY);
            L[i * 3 + 2][0] = controlmat[i][2] - (*lambda) * (rmat[6] * modelmat[i][0] + rmat[7] * modelmat[i][1] + rmat[8] * modelmat[i][2]) - (*deltaZ);
        }
        ATL = AT * L;
        Xv = IATA * ATL;

        *deltaX += Xv[0][0];
        *deltaY += Xv[1][0];
        *deltaZ += Xv[2][0];
        *lambda *= (1.0 + Xv[3][0]);
        *phi += Xv[4][0];
        *omega += Xv[5][0];
        *kappa += Xv[6][0];

        delete rmat;

    }while(fabs(Xv[0][0]) > (*limit) ||
           fabs(Xv[1][0]) > (*limit) ||
           fabs(Xv[2][0]) > (*limit) ||
           fabs(Xv[3][0]) > (*limit) ||
           fabs(Xv[4][0]) > (*limit) ||
           fabs(Xv[5][0]) > (*limit) ||
           fabs(Xv[6][0]) > (*limit)
           );

    result[0] = *deltaX;
    result[1] = *deltaY;
    result[2] = *deltaZ;
    result[3] = *lambda;
    result[4] = *phi;
    result[5] = *omega;
    result[6] = *kappa;
    //计算精度

    Matrix<Type> AXv,V,VT,VTV,D;
    AXv = A * Xv;
    V = AXv - L;
    VT = trT(V);
    VTV = VT * V;

    //单位权中误差的平方
    double *m02 = new double(VTV[0][0] / (3 * pointcount - 7));
    D = (*m02) * IATA;
    for(int i = 0;i < 7;i++)
    {
        precision[i] = sqrt(D[i][i]);
    }

    //将模型点坐标转换为地面摄测坐标 并 计算误差
    double *rmat = CreateRotateMatrix(result[4],result[5],result[6]);
    for(int i = 0;i < pointcount;i++)
    {
        groundpos[i * 3 + 0] = (*lambda) * (rmat[0] * modelmat[i][0] + rmat[1] * modelmat[i][1] + rmat[2] * modelmat[i][2]) + (*deltaX) + (*avgxc);
        groundpos[i * 3 + 1] = (*lambda) * (rmat[3] * modelmat[i][0] + rmat[4] * modelmat[i][1] + rmat[5] * modelmat[i][2]) + (*deltaY) + (*avgyc);
        groundpos[i * 3 + 2] = (*lambda) * (rmat[6] * modelmat[i][0] + rmat[7] * modelmat[i][1] + rmat[8] * modelmat[i][2]) + (*deltaZ) + (*avgzc);

        deltaxyz[i * 3 + 0] = groundpos[i * 3 + 0] - controlpos[i * 3 + 0];
        deltaxyz[i * 3 + 1] = groundpos[i * 3 + 1] - controlpos[i * 3 + 1];
        deltaxyz[i * 3 + 2] = groundpos[i * 3 + 2] - controlpos[i * 3 + 2];
    }

    delete deltaX;
    delete deltaY;
    delete deltaZ;
    delete lambda;
    delete phi;
    delete omega;
    delete kappa;
    delete sumx;
    delete sumy;
    delete sumz;
    delete avgxc;
    delete avgyc;
    delete avgzc;
    delete avgxm;
    delete avgym;
    delete avgzm;

    delete m02;
    delete []rmat;

    return true;
}


void releaseResectionStruct(ResectionStruct rs)
{
    delete []rs.interior;
    delete rs.scale;
    delete rs.limit;
    delete []rs.pointpos;
    delete []rs.rotatematrix;
    delete []rs.exterior;
    delete rs.m0;
    delete []rs.matrix;
    delete []rs.precision;

    rs.interior = NULL;
    rs.scale = NULL;
    rs.limit = NULL;
    rs.pointpos = NULL;
    rs.rotatematrix = NULL;
    rs.exterior = NULL;
    rs.m0 = NULL;
    rs.matrix = NULL;
    rs.precision = NULL;
}


void releaseIntersectionStruct(IntersectionStruct is)
{
    delete []is.interior;
    delete []is.samepoint;
    delete []is.leftexterior;
    delete []is.rightexterior;
    delete []is.result;

    is.interior = NULL;
    is.samepoint = NULL;
    is.leftexterior = NULL;
    is.rightexterior = NULL;
    is.result = NULL;
}


void releaseRelativeOrientationStruct(RelativeOrientationStruct ros)
{
    delete []ros.interior;
    delete ros.m;
    delete ros.limit;
    delete []ros.samepoint;
    delete []ros.result;
    delete []ros.precision;
    delete []ros.modelpos;

    ros.interior = NULL;
    ros.m = NULL;
    ros.limit = NULL;
    ros.samepoint = NULL;
    ros.result = NULL;
    ros.precision = NULL;
    ros.modelpos = NULL;
}


void releaseAbsoluteOrientationStruct(AbsoluteOrientationStruct aos)
{
    delete aos.limit;
    delete []aos.modelpos;
    delete []aos.controlpos;
    delete []aos.result;
    delete []aos.precision;
    delete []aos.groundpos;
    delete []aos.deltaxyz;

    aos.limit = NULL;
    aos.modelpos = NULL;
    aos.controlpos = NULL;
    aos.result = NULL;
    aos.precision = NULL;
    aos.groundpos = NULL;
    aos.deltaxyz = NULL;
}
