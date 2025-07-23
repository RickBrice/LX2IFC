#pragma once

#include "Header.h"

void Cant(LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);


class CantBuilder
{
public:
   CantBuilder(Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Build(LX::Cant* pCant);

private:
   Ifc4x3_add2::IfcAlignment* m_Alignment = nullptr;
   Ifc4x3_add2::IfcAlignmentCant* m_CantAlignment = nullptr;
   IfcHierarchyHelper<Ifc4x3_add2>& m_file;
   double m_StartStation;

   void ProcessCant(LX::CantStation* pCantStation,LX::CantStation* pNextCantStation);
};
