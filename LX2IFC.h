#pragma once

#include "Header.h"

class DataConverter
{
public:
   double convertCant(double c) const;
   double convertDirectionToPlaneAngle(double d);
   double convertPlaneAngleToRadian(double d);
   double convertRadianToPlaneAngle(double d);

   bool m_bSIUnits = true;

   LX::EnumImpLinear::Type m_EnumImpLinear;
   LX::EnumMetLinear::Type m_EnumMetLinear;

   LX::EnumAngularType::Type m_directionUnit;
   LX::EnumAngularType::Type m_angleUnit;
};

class LX2IFC
{
public:
   LX2IFC();
   void Convert(std::string filename);

private:
   DataConverter m_DataConverter;

   Ifc4x3_add2::IfcUnitAssignment* Units(LX::Units* lxUnits, IfcHierarchyHelper<Ifc4x3_add2>& file);
   Ifc4x3_add2::IfcAlignment* Alignment(LX::Alignment* lxalignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Profile(LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Cant(LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);

};