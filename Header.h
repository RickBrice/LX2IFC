#pragma once

#include <iostream>
#include <tchar.h>
#include <atlconv.h>

#undef max

#pragma warning(disable:4250)
#define HAS_SCHEMA_4x3_add2
#include <ifcparse/Ifc4x3_add2.h>
#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcparse/IfcAlignmentHelper.h>


#pragma warning(disable:4244)
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/xmlstring.hpp>

#include "LandXML.h"
#include "Document.h"

