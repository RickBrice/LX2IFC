#pragma once

#include "Header.h"


class ProfileBuilder
{
public:
   ProfileBuilder(double start_station,IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Build(LX::ProfAlign* pProfAlign, Ifc4x3_add2::IfcAlignmentVertical* vertical_alignment);

private:
   Ifc4x3_add2::IfcAlignmentVertical* m_VerticalAlignment = nullptr;
   IfcHierarchyHelper<Ifc4x3_add2>& m_file;

   double m_StartStation;

   using EndPoint = std::tuple<double, double, double>;

   EndPoint ProcessProfileElement(EndPoint prevEnd, LX::Object* pObj, LX::Object* pNextObj);
   EndPoint StartGradient(LX::PVI* pPVI, LX::Object* pNextObj);
   EndPoint PVI(EndPoint prevEnd, LX::PVI* pPVI);
   EndPoint ParaCurve(EndPoint prevEnd, LX::ParaCurve* pParaCurve, LX::Object* pNextObj);
   EndPoint UnsymParaCurve(EndPoint prevEnd, LX::UnsymParaCurve* pUnsymParaCurve, LX::Object* pNextObj);
   EndPoint CircCurve(EndPoint prevEnd,LX::CircCurve* pCircCurve, LX::Object* pNextObj);
   void EndGradient(EndPoint prevEnd, LX::PVI* pPVI);

   void AddGradient(double start_dist_along, double elevation, double grade, double length, boost::optional<std::string> name);
   void AddParaCurve(double start_dist_along, double elevation, double start_grade, double end_grade, double length, boost::optional<std::string> name);
   void AddCircCurve(double start_dist_along, double elevation, double start_grade, double end_grade, double length, double radius, boost::optional<std::string> name);
};
