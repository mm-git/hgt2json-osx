//============================================================================
// Name        : HGTReader.cpp
// Author      : mm
// Copyright   : This tool and Javascript code are licensed under the MIT license.
// Description :
//============================================================================
#include "HGTReader.h"
#include <cctype>
#include <fstream>

HGTReader::HGTReader(std::string& hgtFile)
    : demMatrix()
{
    readHGTFile(hgtFile);
}

HGTReader::~HGTReader()
{    
}

bool HGTReader::readHGTFile(std::string& hgtFile)
{
    if(hgtFile == ""){
        return false;
    }
    
    std::cout << "Reading " << hgtFile << " ..." << std::endl;
    
    map = boost::interprocess::file_mapping(
                                            hgtFile.c_str(),
                                            boost::interprocess::read_only
                                            );
    view = boost::interprocess::mapped_region(
                                              map,
                                              boost::interprocess::read_only,
                                              0,
                                              HGT_MATRIX_SIZE*HGT_MATRIX_SIZE*sizeof(short));
    
    demMatrix.attach((short*)view.get_address(), HGT_MATRIX_SIZE, HGT_MATRIX_SIZE);
    
    return true;
}
