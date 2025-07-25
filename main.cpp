// LX2IFC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "LX2IFC.h"

int main(int argc, char** argv)
{
   USES_CONVERSION;

	if (argc != 2) {
		std::cout << "usage: LX2IFC filename" << std::endl;
		return 1;
	}



	LX2IFC converter;

	// Hard coding this for now, because the FTIA files use CW as positive angle from North
	// instead of CCW as indicated in the LandXML documention
	converter.m_DataConverter.m_DirectionAngleOverride = DataConverter::DirectionAngleOverride::CW;

	converter.Convert(argv[1]);
}
