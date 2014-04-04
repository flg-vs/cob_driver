/****************************************************************
 *
 * Copyright (c) 2010
 *
 * Fraunhofer Institute for Manufacturing Engineering	
 * and Automation (IPA)
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Project name: care-o-bot
 * ROS stack name: cob_driver
 * ROS package name: cob_sick_s300
 * Description:
 *								
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *			
 * Author: Christian Connette, email:christian.connette@ipa.fhg.de
 * Supervised by: Christian Connette, email:christian.connette@ipa.fhg.de
 *
 * Date of creation: Jan 2009
 * ToDo:
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Fraunhofer Institute for Manufacturing 
 *       Engineering and Automation (IPA) nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License LGPL as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License LGPL for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License LGPL along with this program. 
 * If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************/

#include <cob_sick_s300/ScannerSickS300.h>

#include <stdint.h>

//-----------------------------------------------

typedef unsigned char BYTE;

const double ScannerSickS300::c_dPi = 3.14159265358979323846;
unsigned char ScannerSickS300::m_iScanId = 7;

const unsigned short crc_LookUpTable[256]
	   = { 
	   0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 
	   0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 
	   0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 
	   0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 
	   0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 
	   0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 
	   0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 
	   0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 
	   0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 
	   0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 
	   0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 
	   0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 
	   0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 
	   0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 
	   0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 
	   0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 
	   0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 
	   0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, 
	   0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 
	   0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 
	   0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 
	   0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
	   0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 
	   0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, 
	   0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 
	   0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 
	   0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 
	   0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 
	   0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 
	   0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 
	   0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 
	   0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 
	 }; 

unsigned int TelegramParser::createCRC(uint8_t *ptrData, int Size)
{ 
	int CounterWord; 
	unsigned short CrcValue=0xFFFF;

	for (CounterWord = 0; CounterWord < Size; CounterWord++) 
	{ 
		CrcValue = (CrcValue << 8) ^ crc_LookUpTable[ (((uint8_t)(CrcValue >> 8)) ^ *ptrData) ]; 
		ptrData++; 
	} 

	return (CrcValue); 
}

//-----------------------------------------------
ScannerSickS300::ScannerSickS300()
{
	// allows to set different Baud-Multipliers depending on used SerialIO-Card
	m_dBaudMult = 1.0;

	// init scan with zeros
	m_iPosReadBuf2 = 0;
	
	m_actualBufferSize = 0;

}


//-------------------------------------------
ScannerSickS300::~ScannerSickS300()
{
	m_SerialIO.closeIO();
}


// ---------------------------------------------------------------------------
bool ScannerSickS300::open(const char* pcPort, int iBaudRate, int iScanId=7)
{
    int bRetSerial;

	// update scan id (id=8 for slave scanner, else 7)
	m_iScanId = iScanId;
	
	// initialize Serial Interface
	m_SerialIO.setBaudRate(iBaudRate);
	m_SerialIO.setDeviceName(pcPort);
	m_SerialIO.setBufferSize(READ_BUF_SIZE - 10 , WRITE_BUF_SIZE -10 );
	m_SerialIO.setHandshake(SerialIO::HS_NONE);
	m_SerialIO.setMultiplier(m_dBaudMult);
	bRetSerial = m_SerialIO.openIO();
	m_SerialIO.setTimeout(0.0);
	m_SerialIO.SetFormat(8, SerialIO::PA_NONE, SerialIO::SB_ONE);

    if(bRetSerial == 0)
    {
	    // Clears the read and transmit buffer.
	    m_iPosReadBuf2 = 0;
	    m_SerialIO.purge();
	    return true;
    }
    else
    {
        return false;
    }
}


//-------------------------------------------
void ScannerSickS300::purgeScanBuf()
{
	m_iPosReadBuf2 = 0;
	m_SerialIO.purge();
}


//-------------------------------------------
void ScannerSickS300::resetStartup()
{
}


//-------------------------------------------
void ScannerSickS300::startScanner()
{
}


//-------------------------------------------
void ScannerSickS300::stopScanner()
{
}


//-----------------------------------------------
bool ScannerSickS300::getScan(std::vector<double> &vdDistanceM, std::vector<double> &vdAngleRAD, std::vector<double> &vdIntensityAU, unsigned int &iTimestamp, unsigned int &iTimeNow, const bool debug)
{
	bool bRet = false;
	int i;
	int iNumRead2 = 0;
	std::vector<ScanPolarType> vecScanPolar;

	if(SCANNER_S300_READ_BUF_SIZE-2-m_actualBufferSize<=0)
		m_actualBufferSize=0;

	iNumRead2 = m_SerialIO.readBlocking((char*)m_ReadBuf+m_actualBufferSize, SCANNER_S300_READ_BUF_SIZE-2-m_actualBufferSize);
	if(iNumRead2<=0) return false;

	m_actualBufferSize = m_actualBufferSize + iNumRead2;

	// Try to find scan. Searching backwards in the receive queue.
	for(i=m_actualBufferSize; i>=0; i--)
	{
		// parse through the telegram until header with correct scan id is found
		if(tp_.parseHeader(m_ReadBuf+i, m_actualBufferSize-i, m_iScanId, debug))
		{
			tp_.readDistRaw(m_ReadBuf+i, m_viScanRaw);
			if(m_viScanRaw.size()>0) {
				// Scan was succesfully read from buffer
				bRet = true;
				int old = m_actualBufferSize;
				m_actualBufferSize -= tp_.getCompletePacketSize()+i;
				for(int i=0; i<old-m_actualBufferSize; i++)
				    m_ReadBuf[i] = m_ReadBuf[i+old-m_actualBufferSize];
				break;
			}
		}
	}
	
	PARAM_MAP::const_iterator param = m_Params.find(tp_.getField());
	if(bRet && param!=m_Params.end())
	{
		// convert data into range and intensity information
		convertScanToPolar(param, m_viScanRaw, vecScanPolar);

		// resize vectors to size of Scan
		vdDistanceM.resize(vecScanPolar.size());
		vdAngleRAD.resize(vecScanPolar.size());
		vdIntensityAU.resize(vecScanPolar.size());
		// assign outputs
		for(unsigned int i=0; i < vecScanPolar.size(); i++)
		{
			vdDistanceM[i] = vecScanPolar[i].dr;
			vdAngleRAD[i] = vecScanPolar[i].da;
			vdIntensityAU[i] = vecScanPolar[i].di;
		}	
	}

	return bRet;
}

//-------------------------------------------
void ScannerSickS300::convertScanToPolar(const PARAM_MAP::const_iterator param, std::vector<int> viScanRaw,
							std::vector<ScanPolarType>& vecScanPolar )
{	
	double dDist;
	double dAngle, dAngleStep;
	double dIntens;

	vecScanPolar.resize(viScanRaw.size());
	dAngleStep = fabs(param->second.dStopAngle - param->second.dStartAngle) / double(viScanRaw.size() - 1) ;
	
	for(size_t i=0; i<viScanRaw.size(); i++)
	{
		dDist = double ((viScanRaw[i] & 0x1FFF) * param->second.dScale);

		dAngle = param->second.dStartAngle + i*dAngleStep;
		dIntens = double(viScanRaw[i] & 0x2000);

		vecScanPolar[i].dr = dDist;
		vecScanPolar[i].da = dAngle;
		vecScanPolar[i].di = dIntens;
	}
}
