#pragma once

#include "Header.h"

class DataConverter
{
public:
   enum class Directions {
      CCW_FromNorth, // per LandXML schema
      CW_FromNorth
   };

   enum class Points
   {
      NE, // points are North then East, per LandXML schema
      EN  // points are East then North
   };

   template <class T>
   std::pair<double, double> get_coordinates(T* p)
   {
      double X, Y;
      if (m_Points == Points::NE)
      {
         // data is in North than East format
         Y = p->at(0);
         X = p->at(1);
      }
      else
      {
         // data is in East then North format
         X = p->at(0);
         Y = p->at(1);
      }
      return { X, Y };
   }

   double convertCant(double c) const;
   double convertDirectionToPlaneAngle(double d);
   double convertPlaneAngleToRadian(double d);
   double convertRadianToPlaneAngle(double d);

   bool m_bSIUnits = true;

   LX::EnumImpLinear::Type m_EnumImpLinear;
   LX::EnumMetLinear::Type m_EnumMetLinear;

   LX::EnumAngularType::Type m_directionUnit;
   LX::EnumAngularType::Type m_angleUnit;

   Directions m_Directions = Directions::CCW_FromNorth;
   Points m_Points = Points::NE;
};

class LX2IFC
{
public:
   LX2IFC();
   void Convert(std::string filename);

   DataConverter m_DataConverter;

private:
   Ifc4x3_add2::IfcUnitAssignment* Units(LX::Units* lxUnits, IfcHierarchyHelper<Ifc4x3_add2>& file);
   Ifc4x3_add2::IfcAlignment* Alignment(LX::Alignment* lxalignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Profile(LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Cant(LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void StationReferent(Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file, double station, boost::optional<double> incoming,boost::optional<std::string> desc);
};