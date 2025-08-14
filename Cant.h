#pragma once

#include "Header.h"



class CantBuilder
{
public:
   CantBuilder(double start_station,const DataConverter& dataConverter,Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file);
   void Build(LX::Cant* pCant);

private:
   const DataConverter& m_DataConverter;
   Ifc4x3_add2::IfcAlignment* m_Alignment = nullptr;
   Ifc4x3_add2::IfcAlignmentCant* m_CantAlignment = nullptr;
   IfcHierarchyHelper<Ifc4x3_add2>& m_file;
   double m_StartStation;

   void ProcessCant(LX::CantStation* pCantStation,LX::CantStation* pNextCantStation);
};
