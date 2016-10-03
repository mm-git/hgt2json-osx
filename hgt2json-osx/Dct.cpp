//
//  Dct.cpp
//  DEM2JSON-OSX
//
//  Created by mm on 2013/12/30.
//  Copyright (c) 2013年 Individual. All rights reserved.
//

#include "Dct.h"

double Dct::factor0;
double Dct::factor1;
double Dct::cosMatrix[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
double Dct::iCosMatrix[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];

Dct::Dct(){
    originalMatrix = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    dctResult = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    iDctResult = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
}

Dct::~Dct(){
    delete[] originalMatrix;
    delete[] dctResult;
    delete[] iDctResult;
}

void Dct::prepare(){
    long xy;
    long u;

    std::cout << "Preparing dct data..." << std::endl;
    
    factor0 = (double)sqrt(1.0/(double)(MATRIX_SIZE+4));
    factor1 = (double)sqrt(2.0/(double)(MATRIX_SIZE+4));
    
    for(u=0;u<MATRIX_SIZE+4;u++){
        for(xy=0;xy<MATRIX_SIZE+4;xy++){
            cosMatrix[u*(MATRIX_SIZE+4)+xy] = cos(M_PI * (2*u+1)*xy/(double)(2*(MATRIX_SIZE+4)));
            if(u==0){
                iCosMatrix[u*(MATRIX_SIZE+4)+xy] = 0.0;
            }
            else{
                iCosMatrix[u*(MATRIX_SIZE+4)+xy] = cos(M_PI * (2*xy+1)*u/(double)(2*(MATRIX_SIZE+4)));
            }
        }
    }
}

void Dct::doDCT(Matrix<short> *originMatrix, long (*resultMatrix)[MATRIX_SIZE+4], long dctRadius){
    long x;
    long y;
    
    for(y=0; y<MATRIX_SIZE+4; y++){
        for(x=0; x<MATRIX_SIZE+4; x++){
            originalMatrix[y*(MATRIX_SIZE+4)+x] = (double)originMatrix->getMatrixValue(x, y);
        }
    }
    
    dct();
    lpf(dctRadius);
    idct();

    for(y=0; y<MATRIX_SIZE+4; y++){
        for(x=0; x<MATRIX_SIZE+4; x++){
            resultMatrix[y][x] = (long)iDctResult[y*(MATRIX_SIZE+4)+x];
        }
    }
}

void Dct::dct(){
    long x;
    long y;
    double* xDctResult = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    double* xDctTrans = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    double* yDctTrans = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    
    std::cout << "Doing dct x axis ..." << std::endl;
    
    vDSP_mmulD(originalMatrix, 1, cosMatrix, 1, xDctResult, 1, MATRIX_SIZE+4, MATRIX_SIZE+4, MATRIX_SIZE+4);
    for(y=0; y<MATRIX_SIZE+4; y++){
        for(x=0; x<MATRIX_SIZE+4; x++){
            if(x == 0){
                xDctResult[y*(MATRIX_SIZE+4)+x] *= factor0;
            }
            else{
                xDctResult[y*(MATRIX_SIZE+4)+x] *= factor1;
            }
        }
    }
    
    std::cout << "Doing dct y axis ..." << std::endl;
    
    vDSP_mtransD(xDctResult, 1, xDctTrans, 1, MATRIX_SIZE+4, MATRIX_SIZE+4);
    vDSP_mmulD(xDctTrans, 1, cosMatrix, 1, yDctTrans, 1, MATRIX_SIZE+4, MATRIX_SIZE+4, MATRIX_SIZE+4);
    vDSP_mtransD(yDctTrans, 1, dctResult, 1, MATRIX_SIZE+4, MATRIX_SIZE+4);
    for(x=0; x<MATRIX_SIZE+4; x++){
        for(y=0; y<MATRIX_SIZE+4; y++){
            if(y == 0){
                dctResult[y*(MATRIX_SIZE+4)+x] *= factor0;
            }
            else{
                dctResult[y*(MATRIX_SIZE+4)+x] *= factor1;
            }
        }
    }

    delete[] xDctResult;
    delete[] xDctTrans;
    delete[] yDctTrans;
}

void Dct::lpf(long dctRadius){
    long x;
    long y;

    std::cout << "Doing dct lpf ..." << std::endl;

    for(y=0; y<MATRIX_SIZE+4; y++){
        for(x=0; x<MATRIX_SIZE+4; x++){
            if(sqrt(x*x+y*y)>=dctRadius+2){
                dctResult[y*(MATRIX_SIZE+4)+x]=0.0;
            }
        }
    }
}

void Dct::idct(){
    long x;
    long y;
    double* xIDctResult = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    double* xIDctTrans = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    double* yIDctTrans = new double[(MATRIX_SIZE+4) * (MATRIX_SIZE+4)];
    
    std::cout << "Doing idct x axis ..." << std::endl;
    
    vDSP_mmulD(dctResult, 1, iCosMatrix, 1, xIDctResult, 1, MATRIX_SIZE+4, MATRIX_SIZE+4, MATRIX_SIZE+4);
    for(y=0; y<MATRIX_SIZE+4; y++){
        for(x=0; x<MATRIX_SIZE+4; x++){
            xIDctResult[y*(MATRIX_SIZE+4)+x] = xIDctResult[y*(MATRIX_SIZE+4)+x] * factor1 +
                                               dctResult[y*(MATRIX_SIZE+4)] * factor0;
        }
    }

    std::cout << "Doing idct y axis ..." << std::endl;

    vDSP_mtransD(xIDctResult, 1, xIDctTrans, 1, MATRIX_SIZE+4, MATRIX_SIZE+4);
    vDSP_mmulD(xIDctTrans, 1, iCosMatrix, 1, yIDctTrans, 1, MATRIX_SIZE+4, MATRIX_SIZE+4, MATRIX_SIZE+4);
    vDSP_mtransD(yIDctTrans, 1, iDctResult, 1, MATRIX_SIZE+4, MATRIX_SIZE+4);
    for(x=0; x<MATRIX_SIZE+4; x++){
        for(y=0; y<MATRIX_SIZE+4; y++){
            iDctResult[y*(MATRIX_SIZE+4)+x] = iDctResult[y*(MATRIX_SIZE+4)+x] * factor1 +
                                              xIDctResult[x] * factor0;
        }
    }

    delete[] xIDctResult;
    delete[] xIDctTrans;
    delete[] yIDctTrans;
}