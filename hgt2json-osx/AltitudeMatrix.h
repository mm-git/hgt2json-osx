//============================================================================
// Name        : AltitudeMatrix.h
// Author      : mm
// Copyright   : This tool and Javascript code are licensed under the MIT license.
// Description :
//============================================================================
#ifndef ALTITUDEMATRIX_H_
#define ALTITUDEMATRIX_H_

#include "CommonInclude.h"
#include "Matrix.h"

class AltitudeMatrix {
public:
	AltitudeMatrix();
	virtual ~AltitudeMatrix();

private:
	enum EMatrixType{
		eFactor1,
		eFactor4,
		eFactor6,
		eFactor16,
		eFactor24,
		eFactor36,
		eResult,
		eMaxTypeNum,
	};
	Matrix<short>   masterMatrix;
	long            matrix[eMaxTypeNum][MATRIX_SIZE+4][MATRIX_SIZE+4];
	unsigned long   smoothingCount;
    long            dctRadius;
	std::string     baseFileName;

public:
	void clear();
	bool setDEMMatrix(Matrix<short>* pMatrix11, Matrix<short>* pMatrix12, Matrix<short>* pMatrix13,
					  Matrix<short>* pMatrix21, Matrix<short>* pMatrix22, Matrix<short>* pMatrix23,
					  Matrix<short>* pMatrix31, Matrix<short>* pMatrix32, Matrix<short>* pMatrix33);
	bool setDEMMatrix(Matrix<short>* pDemMatrix);
	void doSmoothing(unsigned long smooothingCount,
                     bool afterDct = false);
    void doDCT(long dctRadius);
	bool setBaseFile(std::string& demFileName);
	void verify(Matrix<short>* pOrgMatrix);
    void compare();
	bool writeJSON(std::string& outputFolder);

private:
	char* heightEncode(short height);
};

#endif /* ALTITUDEMATRIX_H_ */
