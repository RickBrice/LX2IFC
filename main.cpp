// LX2IFC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "LX2IFC.h"

// * Add implementation for `<Chain > ` and `<IrregularLine > ` horizontal element types to the LX2IFC program. 
// `<Chain > ` could be decomposed into a sequence of `LINE` segments if approximate fidelity is acceptable

// Fix `<CircCurve > ` start station calculation in LX2IFC — 
// currently uses $s_{ PVI } - L / 2$ as an approximation; 
// the exact tangent length is $T = R\tan(\Delta / 2)$ where $\Delta = L / R$.
// Commented - out code in Profile.cpp already outlines the correct approach - this is a claude generated assesment.

int main(int argc, char** argv)
{
   USES_CONVERSION;

	if (argc != 2) {
		std::cout << "usage: LX2IFC filename_without_extension" << std::endl;
		return 1;
	}



	LX2IFC converter;

	// Hard coding this for now, because the FTIA files use CW as positive angle from North
	// instead of CCW as indicated in the LandXML documentation
	converter.m_DataConverter.m_Directions = DataConverter::Directions::CCW_FromNorth;
	converter.m_DataConverter.m_Points = DataConverter::Points::NE;

	converter.Convert(argv[1]);
}
