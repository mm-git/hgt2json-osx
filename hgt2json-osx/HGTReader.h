//============================================================================
// Name        : HGTReader.h
// Author      : mm
// Copyright   : This tool and Javascript code are licensed under the MIT license.
// Description :
//============================================================================
#ifndef HGTREADER_H_
#define HGTREADER_H_

#include "CommonInclude.h"
#include "Matrix.h"
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#define HGT_MATRIX_SIZE 1201

class HGTReader
{
public:
    HGTReader(std::string& hgtFile);
    virtual ~HGTReader();
    
private:
    Matrix<short>		demMatrix;
    boost::interprocess::file_mapping map;
    boost::interprocess::mapped_region view;
    
public:
    Matrix<short>* getMatrix(){
        return &demMatrix;
    }
    
private:
    bool readHGTFile(std::string &hgtFile);
};

#endif
