//============================================================================
// Name        : DEMMatrix.cpp
// Author      : mm
// Copyright   : This codes are licensed under the MIT license.
// Create      : 2011/11/04
// Description :
//============================================================================
#include "DEMMatrix.h"
#include "AltitudeMatrix.h"
#include "Dct.h"
#include <boost/format.hpp>
#include <cctype>
#include <fstream>

DEMMatrix::DEMMatrix() {
	for(long i=0; i<180; i++){
		for(long j=0; j<360; j++){
			demFileMatrix[i][j].pHGTData = 0;
		}
	}
	demFileCount = 0;
}

DEMMatrix::~DEMMatrix() {
}

bool DEMMatrix::readDEMFolder(std::string& inputFolder) {
	boost::filesystem::directory_iterator end;

	for (boost::filesystem::directory_iterator it(inputFolder); it != end; it++) {
		if (boost::filesystem::is_directory(*it)) {
			continue;
		}

		std::string filename = it->path().filename().generic_string();
		std::transform(filename.begin(), filename.end(), filename.begin(), (int(*)(int))std::toupper);boost
		::regex reg("[SN]\\d{2}[WE]\\d{3}\\.HGT");
		if (!boost::regex_match(filename, reg)) {
			std::cout << "[WARNING] invalid file detected. (" << filename << ")" << std::endl;
			continue;
		}

		// 0-180:S90-S01,N00-N90 , 0-360:W180-W001,E000-E179
		long lat;
		if (filename[0] == 'S') {
			lat = 90 - atoi(filename.substr(1, 2).c_str());
		} else {
			lat = 90 + atoi(filename.substr(1, 2).c_str());
		}
		if (lat < 0 || lat >= 180) {
			std::cout << "[WARNING] invalid file detected. (" << filename << ")"	<< std::endl;
			continue;
		}
		long lng;
		if (filename[3] == 'W') {
			lng = 180 - atoi(filename.substr(4, 3).c_str());
		} else {
			lng = 180 + atoi(filename.substr(4, 3).c_str());
		}
		if (lng < 0 || lng >= 360) {
			std::cout << "[WARNING] invalid file detected. (" << filename << ")" << std::endl;
			continue;
		}
        demFileMatrix[lat][lng].fileName = inputFolder + filename;
        demFileCount++;
	}

	std::cout << "The number of DEM files : " << demFileCount << std::endl;

	return true;
}

#define TIF(x) if(!x.pHGTData){ x.pHGTData = new HGTReader(x.fileName);}
#define DELETE_POINTER(x) if(x){delete x; x=0;}
bool DEMMatrix::convertToJson(
	std::string&	inputFolder,
	std::string&	outputFolder,
	unsigned long	smoothingCount,
    long            dctRadius,
	bool			fullConvert)
{
	long i;
	long j;
	long j1;
	long j2;
	long j3;
    long convertCount = 0;
    long skipCount = 0;
    long nothing = 0;

	AltitudeMatrix* pAltitude = new AltitudeMatrix();

    if(dctRadius >= 0){
        Dct::prepare();
    }
    
	// Actual latitude range is between -80 to 80, so checking 'i' omit.
	for(i=1; i<179; i++){
		for(j=0; j<360; j++){
			try{
				if(demFileMatrix[i][j].fileName == ""){
                    nothing++;
					continue;
				}

				if(smoothingCount > 0){
                    std::string smoothOutputFolder;
                    if(dctRadius >=0){
                        smoothOutputFolder = outputFolder + "/dct/";
                    }
                    else{
                        smoothOutputFolder = outputFolder + "/smooth/";
                    }
                    if(!fullConvert && isExistOutput(smoothOutputFolder, demFileMatrix[i][j].fileName)){
                        skipCount++;
                        continue;
                    }
                    
                    j1 = (j == 0 ? 359 : j-1);
					j2 = j;
					j3 = (j == 359 ? 0 : j+1);

                    TIF(demFileMatrix[i-1][j1]);
					TIF(demFileMatrix[i-1][j2]);
					TIF(demFileMatrix[i-1][j3]);
					TIF(demFileMatrix[i][j1]);
					TIF(demFileMatrix[i][j2]);
					TIF(demFileMatrix[i][j3]);
					TIF(demFileMatrix[i+1][j1]);
					TIF(demFileMatrix[i+1][j2]);
					TIF(demFileMatrix[i+1][j3]);

                    pAltitude->setDEMMatrix(
                        demFileMatrix[i-1][j1].pHGTData->getMatrix(),
                        demFileMatrix[i-1][j2].pHGTData->getMatrix(),
                        demFileMatrix[i-1][j3].pHGTData->getMatrix(),
                        demFileMatrix[i][j1].pHGTData->getMatrix(),
                        demFileMatrix[i][j2].pHGTData->getMatrix(),
                        demFileMatrix[i][j3].pHGTData->getMatrix(),
                        demFileMatrix[i+1][j1].pHGTData->getMatrix(),
                        demFileMatrix[i+1][j2].pHGTData->getMatrix(),
                        demFileMatrix[i+1][j3].pHGTData->getMatrix()
                    );
                    if(dctRadius>=0){
                        pAltitude->doDCT(dctRadius);
                        pAltitude->doSmoothing(smoothingCount, true);
                    }
                    else{
                        pAltitude->doSmoothing(smoothingCount);
                        pAltitude->verify(demFileMatrix[i][j2].pHGTData->getMatrix());
                    }
                    pAltitude->setBaseFile(demFileMatrix[i][j].fileName);
//                    pAltitude->compare();
                    pAltitude->writeJSON(smoothOutputFolder);

					DELETE_POINTER(demFileMatrix[i-1][j1].pHGTData);
					DELETE_POINTER(demFileMatrix[i][j1].pHGTData);
					DELETE_POINTER(demFileMatrix[i+1][j1].pHGTData);
					DELETE_POINTER(demFileMatrix[i-1][j2].pHGTData);
					DELETE_POINTER(demFileMatrix[i][j2].pHGTData);
					DELETE_POINTER(demFileMatrix[i+1][j2].pHGTData);
					DELETE_POINTER(demFileMatrix[i-1][j3].pHGTData);
					DELETE_POINTER(demFileMatrix[i][j3].pHGTData);
					DELETE_POINTER(demFileMatrix[i+1][j3].pHGTData);

                    convertCount++;
				}
                else if(dctRadius >= 0){
                    std::string dctOutputFolder = outputFolder + "/dct/";
                    if(!fullConvert && isExistOutput(dctOutputFolder, demFileMatrix[i][j].fileName)){
                        skipCount++;
                        continue;
                    }

                    TIF(demFileMatrix[i][j]);
                    pAltitude->setDEMMatrix(demFileMatrix[i][j].pHGTData->getMatrix());
                    pAltitude->doDCT(dctRadius);
//                        pAltitude->compare();
                    pAltitude->setBaseFile(demFileMatrix[i][j].fileName);
                    pAltitude->writeJSON(dctOutputFolder);
                    DELETE_POINTER(demFileMatrix[i][j].pHGTData);
                    convertCount++;
                }
				else{
                    std::string originalOutputFolder = outputFolder + "/org/";
                    if(!fullConvert && isExistOutput(originalOutputFolder, demFileMatrix[i][j].fileName)){
                        skipCount++;
                        continue;
                    }

                    TIF(demFileMatrix[i][j]);
                    pAltitude->setDEMMatrix(demFileMatrix[i][j].pHGTData->getMatrix());
                    pAltitude->setBaseFile(demFileMatrix[i][j].fileName);
                    pAltitude->writeJSON(originalOutputFolder);
                    DELETE_POINTER(demFileMatrix[i][j].pHGTData);
                    convertCount++;
				}
			}
			catch(...){
				std::cout << "[ERROR] Converting failed. (" << i << "," << j << ")" << std::endl;
			}
		}
	}
	delete pAltitude;

    long total = convertCount + skipCount + nothing;
    std::cout << "Total : " << total << " Converted : " << convertCount << " Skipped : " << skipCount << " Nothing : " << nothing << std::endl;

	return true;
}

bool DEMMatrix::writeArea(std::string& outputFolder)
{
	std::cout << "Writing area file ..." << std::endl;

	std::string outputFileName = outputFolder + AREA_FILE;
	std::ofstream outStream(outputFileName.c_str(), std::ios::out |  std::ios::binary );
	if(!outStream){
		return false;
	}

	outStream << "{\"a\" : [" << std::endl;
	for(long i=0; i<180; i++){
		std::string lineString = "\"";
		unsigned char shift = 0x80;
		unsigned char hex = 0;

		for(long j=0; j<360; j++){
			if(demFileMatrix[i][j].fileName != ""){
				hex |= shift;
			}
			shift >>= 1;
			if(shift == 0){
				lineString += (boost::format("%|02x|") % (int)(unsigned char)(hex)).str();
				hex = 0;
				shift = 0x80;
			}
		}

		if(i == 179){
			lineString += "\"";
		}
		else{
			lineString += "\",";
		}
		outStream << lineString << std::endl;
	}
	outStream << "]}" << std::endl;
	outStream.close();

	return true;
}

bool DEMMatrix::isExistOutput(
	std::string& outputFolder,
	std::string& demFile)
{
	std::string outputFileName = boost::filesystem::path(demFile).stem().generic_string();

	outputFileName = outputFolder + outputFileName.substr(0, 7) + std::string("M00.json");

	if(boost::filesystem::exists(boost::filesystem::path(outputFileName))){
		return true;
	}

	return false;
}
