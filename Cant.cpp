#include "LX2IFC.h"
#include "Cant.h"

//
//template <class T>
//std::pair<double, double> get_pvi(T* pObj)
//{
//   return { pObj->at(0),pObj->at(1) };
//}
//
template <class T>
boost::optional<std::string> get_name(T* pObj)
{
   boost::optional<std::string> name;
   if (pObj->hasValue_Name())
   {
	  USES_CONVERSION;
	  name = std::string(W2A(pObj->getName().c_str()));
   }
   return name;
}


void LX2IFC::Cant(LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file)
{
   CantBuilder builder(m_DataConverter,alignment,file);

   LX::Cant* cant = lxalignment->getCant();
   if (cant)
   {
	  builder.Build(cant);
   }
}


CantBuilder::CantBuilder(const DataConverter& dataConverter, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file) :
	m_DataConverter(dataConverter), m_Alignment(alignment), m_file(file), m_StartStation(0.0), m_CantAlignment(nullptr)
{
}

void CantBuilder::Build(LX::Cant* pCant)
{
   auto& cants = pCant->CantStation();
   auto pIter = cants.iterator();
   if (pIter->atEnd())
	  return;

   auto name = get_name(pCant);
   double gauge = pCant->getGauge();
   auto cant_alignment = new Ifc4x3_add2::IfcAlignmentCant(IfcParse::IfcGlobalId(), nullptr, name, boost::none, boost::none, nullptr, nullptr, gauge);
   m_file.addEntity(cant_alignment);
   m_file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(m_Alignment, cant_alignment);

   m_CantAlignment = cant_alignment;


   LX::CantStation* pCantStation = pIter->current();
   m_StartStation = pCantStation->getStation();
   pIter->next();

   do
   {
	  LX::CantStation* pNextCantStation = pIter->current();
	  ProcessCant(pCantStation, pNextCantStation);
	  pCantStation = pNextCantStation;
	  pIter->next();
   } while (!pIter->atEnd());
}

void CantBuilder::ProcessCant(LX::CantStation* pCantStation, LX::CantStation* pNextCantStation)
{
   double station = pCantStation->getStation();
   double next_station = pNextCantStation->getStation();
   double start_dist_along = station - m_StartStation;
   double length = next_station - station;
   double start_cant = pCantStation->getAppliedCant();
   double end_cant = pNextCantStation->getAppliedCant();

   start_cant = m_DataConverter.convertCant(start_cant);
   end_cant = m_DataConverter.convertCant(end_cant);

   double start_cant_left, start_cant_right;
   double end_cant_left, end_cant_right;
   if (pCantStation->hasValue_Curvature())
   {
	  if (pCantStation->getCurvature() == LX::EnumClockwise::k_ccw)
	  {
		 // curve is to the left
		 start_cant_left = 0;
		 start_cant_right = start_cant;
		 end_cant_left = 0;
		 end_cant_right = end_cant;
	  }
	  else
	  {
		 start_cant_left = start_cant;
		 start_cant_right = 0;
		 end_cant_left = end_cant;
		 end_cant_right = 0;
	  }
   }

   Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::Value predefined_type;
   if (start_cant == end_cant)
   {
	  predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_CONSTANTCANT;
   }
   else if (!pCantStation->hasValue_TransitionType())
   {
	  predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_LINEARTRANSITION;
   }
   else
   {
	  switch (pCantStation->getTransitionType())
	  {
	  case LX::EnumSpiralType::k_null:
		 std::wcout << _T("*** Unknown cant ***") << std::endl;
		 break;

	  case LX::EnumSpiralType::k_biquadratic:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_HELMERTCURVE;
		 break;

	  case LX::EnumSpiralType::k_bloss:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_BLOSSCURVE;
		 break;

	  case LX::EnumSpiralType::k_clothoid:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_LINEARTRANSITION;
		 break;

	  case LX::EnumSpiralType::k_cosine:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_COSINECURVE;
		 break;

	  case LX::EnumSpiralType::k_cubic:
		 std::wcout << _T("*** Unknown cant - k_cubic ***") << std::endl;
		 break;

	  case LX::EnumSpiralType::k_sinusoid:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_SINECURVE;
		 break;

	  case LX::EnumSpiralType::k_revBiquadratic:
		 std::wcout << _T("*** Unknown cant - revBiquadratic ***") << std::endl;
		 break;
	  case LX::EnumSpiralType::k_revBloss:
		 std::wcout << _T("*** Unknown cant - revBloss ***") << std::endl;
		 break;
	  case LX::EnumSpiralType::k_revCosine:
		 std::wcout << _T("*** Unknown cant - revCosine ***") << std::endl;
		 break;
	  case LX::EnumSpiralType::k_revSinusoid:
		 std::wcout << _T("*** Unknown cant - revSinusoid ***") << std::endl;
		 break;
	  case LX::EnumSpiralType::k_sineHalfWave:
		 // LandXML Spiral Document (1.1 Schema) - Sine Half-Wavelength is an approximate of a Cosine Spiral
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_COSINECURVE;
		 break;
	  case LX::EnumSpiralType::k_biquadraticParabola:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_HELMERTCURVE;
		 break;
	  case LX::EnumSpiralType::k_cubicParabola:
		 std::wcout << _T("*** Unknown cant - cubicParabola ***") << std::endl;
		 break;
	  case LX::EnumSpiralType::k_japaneseCubic:
		 predefined_type = Ifc4x3_add2::IfcAlignmentCantSegmentTypeEnum::IfcAlignmentCantSegmentType_VIENNESEBEND;
		 break;
	  case LX::EnumSpiralType::k_radioid:
		 std::wcout << _T("*** Unknown cant - radioid ***") << std::endl;
		 break;
	  case LX::EnumSpiralType::k_weinerBogen:
		 std::wcout << _T("*** Unknown cant - weinerBogen ***") << std::endl;
		 break;
	  }


   }


   auto design_parameters = new Ifc4x3_add2::IfcAlignmentCantSegment(boost::none, boost::none, start_dist_along, length, start_cant_left, end_cant_left, start_cant_right, end_cant_right, predefined_type);
   auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(IfcParse::IfcGlobalId(), nullptr, boost::none, boost::none, boost::none, nullptr, nullptr, design_parameters);
   m_file.addEntity(design_parameters);
   m_file.addEntity(alignment_segment);
   m_file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(m_CantAlignment, alignment_segment);
}
