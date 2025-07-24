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
	converter.Convert(argv[1]);
}
