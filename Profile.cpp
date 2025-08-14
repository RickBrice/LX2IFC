#include "LX2IFC.h"
#include "Profile.h"

std::pair<double, double> get_station_elevation(LX::DoubleCollection* pObj)
{
   return { pObj->at(0),pObj->at(1) };
}

template <class T>
boost::optional<std::string> get_name(T* pObj)
{
   boost::optional<std::string> name;
   if (pObj->hasValue_Desc())
   {
	  USES_CONVERSION;
	  name = std::string(W2A(pObj->getDesc().c_str()));
   }
   return name;
}


void LX2IFC::Profile(double start_station,LX::Alignment* lxalignment, Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file)
{
   ProfileBuilder builder(start_station,file);

   auto& profile_collection = lxalignment->Profile();
   auto profileIter = profile_collection.iterator();
   while (!profileIter->atEnd())
   {
	  auto lxprofile = profileIter->current();
	  LX::String strProfileName = lxprofile->hasValue_Name() ? lxprofile->getName() : LX::String(_T("<Unnamed>"));
	  std::wcout << _T("   ") << strProfileName << std::endl;
	  profileIter->next();

	  auto& profile_alignment_collection = lxprofile->ProfAlign();
	  auto profile_alignment_iter = profile_alignment_collection.iterator();
	  while (!profile_alignment_iter->atEnd())
	  {
		 auto profile_alignment = profile_alignment_iter->current();

		 auto name = get_name(profile_alignment);

		 auto vertical_alignment = new Ifc4x3_add2::IfcAlignmentVertical(IfcParse::IfcGlobalId(), nullptr, name, boost::none, boost::none, nullptr, nullptr);
		 file.addEntity(vertical_alignment);
		 file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(alignment, vertical_alignment);

		 builder.Build(profile_alignment,vertical_alignment);

		 profile_alignment_iter->next();
	  }
   }
}


ProfileBuilder::ProfileBuilder(double start_station,IfcHierarchyHelper<Ifc4x3_add2>& file) : m_StartStation(start_station),m_file(file)
{
}

void ProfileBuilder::Build(LX::ProfAlign* pProfAlign, Ifc4x3_add2::IfcAlignmentVertical* vertical_alignment)
{
   m_VerticalAlignment = vertical_alignment;

   auto pIter = pProfAlign->VertGeomList().iterator();
   LX::Object* pObj = pIter->current();
   LX::PVI* pPVI = dynamic_cast<LX::PVI*>(pObj); // first is always a PVI

   pIter->next();
   LX::Object* pNextObj = pIter->current();
   auto prevEnd = StartGradient(pPVI, pNextObj);
   pObj = pNextObj;

   while (!pIter->atEnd())
   {
	  pIter->next();
	  pNextObj = (pIter->atEnd() ? nullptr : pIter->current());

	  LX::Feature* pFeature = dynamic_cast<LX::Feature*>(pNextObj);
	  if (pFeature)
		 break;

	  prevEnd = ProcessProfileElement(prevEnd, pObj, pNextObj);

	  pObj = pNextObj;
   }
}

ProfileBuilder::EndPoint ProfileBuilder::ProcessProfileElement(EndPoint prevEnd,LX::Object* pObj, LX::Object* pNextObj)
{
   if (pNextObj == nullptr)
   {
	  auto pPVI = dynamic_cast<LX::PVI*>(pObj);
	  EndGradient(prevEnd,pPVI);
   }
   else
   {
	  LX::PVI* pPVI = dynamic_cast<LX::PVI*>(pObj);
	  LX::ParaCurve* pParaCurve = dynamic_cast<LX::ParaCurve*>(pObj);
	  LX::UnsymParaCurve* pUnsymParaCurve = dynamic_cast<LX::UnsymParaCurve*>(pObj);
	  LX::CircCurve* pCircCurve = dynamic_cast<LX::CircCurve*>(pObj);

	  if (pPVI)
	  {
		 prevEnd = PVI(prevEnd, pPVI);
	  }
	  else if (pParaCurve)

	  {
		 prevEnd = ParaCurve(prevEnd, pParaCurve, pNextObj);
	  }
	  else if (pUnsymParaCurve)
	  {
		 prevEnd = UnsymParaCurve(prevEnd, pUnsymParaCurve, pNextObj);
	  }
	  else if (pCircCurve)
	  {
		 prevEnd = CircCurve(prevEnd, pCircCurve, pNextObj);
	  }
	  else 
		 assert(false);
   }

   return prevEnd;
}

ProfileBuilder::EndPoint ProfileBuilder::StartGradient(LX::PVI* pPVI, LX::Object* pNextObj)
{
   auto [pvi_station, pvi_elevation] = get_station_elevation(pPVI);

   double half_curve_length = 0;

   double next_station, next_elevation;
   LX::PVI* pNextPVI = dynamic_cast<LX::PVI*>(pNextObj);
   LX::ParaCurve* pNextParaCurve = dynamic_cast<LX::ParaCurve*>(pNextObj);
   LX::UnsymParaCurve* pNextUnsymParaCurve = dynamic_cast<LX::UnsymParaCurve*>(pNextObj);
   LX::CircCurve* pNextCircCurve = dynamic_cast<LX::CircCurve*>(pNextObj);
   if (pNextPVI)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextPVI);
   }
   else if (pNextParaCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextParaCurve);
	  half_curve_length = pNextParaCurve->getLength() / 2;
   }
   else if (pNextUnsymParaCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextUnsymParaCurve);
	  half_curve_length = pNextUnsymParaCurve->getLengthIn();
   }
   else if (pNextCircCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextCircCurve);
	  //double g1 = (next_elevation - pvi_elevation) / (next_station - pvi_station);
	  //double theta1 = atan(g1);

	  //double l = pNextCircCurve->getLength();
	  //double r = pNextCircCurve->getRadius();
	  //double delta = l / r;

	  //double radius = pNextCircCurve->getRadius();
	  //double T = fabs(radius * tan(delta / 2));

	  //half_curve_length = T * cos(theta1);

	  half_curve_length = pNextCircCurve->getLength() / 2;
   }
   else
   {
	  assert(false);
   }

   double gradient = (next_elevation - pvi_elevation) / (next_station - pvi_station);

   double length = next_station - half_curve_length - pvi_station;

   double start_dist_along = pvi_station - m_StartStation;
   auto name = get_name(pPVI);
   AddGradient(start_dist_along, pvi_elevation, gradient, length, name);

   double end_station = pvi_station + length;
   double end_elevation = pvi_elevation + gradient * length;
   return { end_station, end_elevation, gradient };
}

ProfileBuilder::EndPoint ProfileBuilder::PVI(EndPoint prevEnd, LX::PVI* pPVI)
{
   auto [pvi_station, pvi_elevation] = get_station_elevation(pPVI);
    
   auto [prev_segment_station, prev_segment_elevation, prev_segment_gradient] = prevEnd;

   double length = pvi_station - prev_segment_station;
   if (0. < length)
   {
	  double gradient = (pvi_elevation - prev_segment_elevation) / length;

	  double start_dist_along = prev_segment_station - m_StartStation;
	  auto name = get_name(pPVI);
	  AddGradient(start_dist_along, pvi_elevation, gradient, length, name);

	  double end_station = pvi_station + length;
	  double end_elevation = pvi_elevation + gradient * length;
	  return { end_station, end_elevation, gradient };
   }
   else
   {
	  return prevEnd;
   }
}

ProfileBuilder::EndPoint ProfileBuilder::ParaCurve(EndPoint prevEnd,LX::ParaCurve* pParaCurve, LX::Object* pNextObj)
{
   auto [pvi_station, pvi_elevation] = get_station_elevation(pParaCurve);
   double length = pParaCurve->getLength();

   double next_station, next_elevation;
   LX::PVI* pNextPVI = dynamic_cast<LX::PVI*>(pNextObj);
   LX::ParaCurve* pNextParaCurve = dynamic_cast<LX::ParaCurve*>(pNextObj);
   LX::UnsymParaCurve* pNextUnsymParaCurve = dynamic_cast<LX::UnsymParaCurve*>(pNextObj);
   LX::CircCurve* pNextCircCurve = dynamic_cast<LX::CircCurve*>(pNextObj);
   if (pNextPVI)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextPVI);
   }
   else if (pNextParaCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextParaCurve);
   }
   else if (pNextUnsymParaCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextUnsymParaCurve);
   }
   else if (pNextCircCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextCircCurve);
   }
   else
   {
	  assert(false);
   }

   double segment_start_station = pvi_station - length / 2;
   auto [prev_segment_station, prev_segment_elevation, prev_segment_gradient] = prevEnd;
   if (prev_segment_station < segment_start_station)
   {
	  // add gradient segment that fills the gap between the end of the previous and the start of current segment
	  AddGradient(prev_segment_station - m_StartStation, prev_segment_elevation, prev_segment_gradient, segment_start_station - prev_segment_station, boost::none);
   }

   double start_gradient = (pvi_elevation - prev_segment_elevation) / (pvi_station - prev_segment_station);
   double elevation = pvi_elevation - start_gradient * length / 2;
   double end_gradient = (next_elevation - pvi_elevation) / (next_station - pvi_station);

   double start_dist_along = segment_start_station - m_StartStation;
   auto name = get_name(pParaCurve);
   AddParaCurve(start_dist_along, elevation, start_gradient, end_gradient, length, name);

   double end_station = segment_start_station + length;
   double end_elevation = pvi_elevation + end_gradient * length / 2;
   return { end_station, end_elevation, end_gradient };
}

ProfileBuilder::EndPoint ProfileBuilder::UnsymParaCurve(EndPoint prevEnd, LX::UnsymParaCurve* pUnsymParaCurve, LX::Object* pNextObj)
{
   auto [pvi_station, pvi_elevation] = get_station_elevation(pUnsymParaCurve);
   double length_in = pUnsymParaCurve->getLengthIn();
   double length_out = pUnsymParaCurve->getLengthOut();


   double next_station = 0.;
   double next_elevation = 0.;
   LX::PVI* pNextPVI = dynamic_cast<LX::PVI*>(pNextObj);
   LX::ParaCurve* pNextParaCurve = dynamic_cast<LX::ParaCurve*>(pNextObj);
   LX::UnsymParaCurve* pNextUnsymParaCurve = dynamic_cast<LX::UnsymParaCurve*>(pNextObj);
   LX::CircCurve* pNextCircCurve = dynamic_cast<LX::CircCurve*>(pNextObj);
   if (pNextPVI)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextPVI);
   }
   else if (pNextParaCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextParaCurve);
   }
   else if (pNextUnsymParaCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextUnsymParaCurve);
   }
   else if (pNextCircCurve)
   {
	  std::tie(next_station, next_elevation) = get_station_elevation(pNextCircCurve);
   }
   else
   {
	  assert(false);
   }


   double segment_start_station = pvi_station - length_in;
   auto [prev_segment_station, prev_segment_elevation, prev_segment_gradient] = prevEnd;
   if (prev_segment_station < segment_start_station)
   {
	  // add gradient segment that fills the gap between the end of the previous and the start of current segment
	  AddGradient(prev_segment_station - m_StartStation, prev_segment_elevation, prev_segment_gradient, segment_start_station - prev_segment_station, boost::none);
   }

   double g1 = (pvi_elevation - prev_segment_elevation) / (pvi_station - prev_segment_station); // entry grade to composite curve
   double g2 = (next_elevation - pvi_elevation) / (next_station - pvi_station); // exit grade to composite curve
   double h = length_in * length_out * (g2 - g1) / (2 * (length_in + length_out)); // vertical distance from curve to PVI

   double pvi1_station = pvi_elevation - length_in / 2; // station of PVI of left curve
   double pvi1_elevation = pvi_elevation - g1 * length_in / 2; // elevation of PVI of left curve

   double transition_station = pvi_station; // station of transition point between left and right curves
   double transition_elevation = pvi_elevation + h; // elevation of transition point between left and right curves

   // left vertical curve
   double start_dist_along = segment_start_station - m_StartStation;
   double elevation = pvi_elevation - g1 * length_in;
   double start_gradient = g1;
   double end_gradient = (transition_elevation - pvi1_elevation) / (transition_station - pvi1_station);
   auto name = get_name(pUnsymParaCurve);
   AddParaCurve(start_dist_along, elevation, start_gradient, end_gradient, length_in,name);

   
   // right vertical curve
   start_dist_along = pvi_station - m_StartStation; // also = left curve start_dist_along + length_in
   elevation = transition_elevation;
   start_gradient = end_gradient; // start gradient of second curve if end gradient of first curve
   end_gradient = g2;
   AddParaCurve(start_dist_along, elevation, start_gradient, end_gradient, length_out,name);

   double end_station = pvi_station + length_out;
   double end_elevation = pvi_elevation + end_gradient * length_out;
   return { end_station, end_elevation, end_gradient };
}

ProfileBuilder::EndPoint ProfileBuilder::CircCurve(EndPoint prevEnd,LX::CircCurve* pCircCurve, LX::Object* pNextObj)
{
   auto [pvi_station,pvi_elevation] = get_station_elevation(pCircCurve);
   double length = pCircCurve->getLength();
   double radius = pCircCurve->getRadius();

   double delta = length / radius;

   double segment_start_station = pvi_station - length / 2;
   auto [prev_segment_station, prev_segment_elevation, prev_segment_gradient] = prevEnd;
   //double g1 = prev_segment_gradient;
   //double theta1 = atan(g1);

   double next_station, next_elevation;
   std::tie(next_station, next_elevation) = get_station_elevation(dynamic_cast<LX::DoubleCollection*>(pNextObj));
   ////double g2 = (next_elevation - pvi_elevation) / (next_station - pvi_station);
   ////double theta2 = atan(g2);
   //double g2 = g1 + delta;
   //double theta2 = atan(g2);

   ////double delta = theta2 - theta1;
   //double T = fabs(radius * tan(delta / 2));

   //double L1 = T * cos(theta1);
   //double L2 = T * cos(theta2);

   ////double length = fabs(radius * delta);


   //double segment_start_station = pvi_station - L1;
   if (prev_segment_station < segment_start_station)
   {
	  // add gradient segment that fills the gap between the end of the previous and the start of current segment
	  AddGradient(prev_segment_station - m_StartStation, prev_segment_elevation, prev_segment_gradient, segment_start_station - prev_segment_station, boost::none);
   }

   //double start_dist_along = segment_start_station - m_StartStation;
   //auto name = get_name(pCircCurve);
   //AddCircCurve(start_dist_along, prev_segment_elevation, g1, g2, L1+L2, radius,name);

   //double end_station = pvi_station + L2;
   //double end_elevation = pvi_elevation + g2 * L2;
   //return { end_station, end_elevation, g2 };

   double start_gradient = (pvi_elevation - prev_segment_elevation) / (pvi_station - prev_segment_station);
   double elevation = pvi_elevation - start_gradient * length / 2;
   double end_gradient = (next_elevation - pvi_elevation) / (next_station - pvi_station);

   double start_dist_along = segment_start_station - m_StartStation;
   auto name = get_name(pCircCurve);
   AddCircCurve(start_dist_along, elevation, start_gradient, end_gradient, length, radius, name);

   double end_station = pvi_station + length / 2;
   double end_elevation = pvi_elevation + end_gradient * length / 2;
   return { end_station, end_elevation,end_gradient };
}

void ProfileBuilder::EndGradient(EndPoint prevEnd,LX::PVI* pPVI)
{
   PVI(prevEnd, pPVI);
}

void ProfileBuilder::AddGradient(double start_dist_along, double elevation, double grade, double length,boost::optional<std::string> name)
{
   auto design_parameters = new Ifc4x3_add2::IfcAlignmentVerticalSegment(boost::none, boost::none, start_dist_along, length, elevation, grade, grade, boost::none, Ifc4x3_add2::IfcAlignmentVerticalSegmentTypeEnum::IfcAlignmentVerticalSegmentType_CONSTANTGRADIENT);
   auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(IfcParse::IfcGlobalId(), nullptr, name, boost::none, boost::none, nullptr, nullptr, design_parameters);
   m_file.addEntity(design_parameters);
   m_file.addEntity(alignment_segment);
   m_file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(m_VerticalAlignment,alignment_segment);
}

void ProfileBuilder::AddParaCurve(double start_dist_along, double elevation, double start_grade, double end_grade, double length, boost::optional<std::string> name)
{
   auto design_parameters = new Ifc4x3_add2::IfcAlignmentVerticalSegment(boost::none, boost::none, start_dist_along, length, elevation, start_grade, end_grade, boost::none, Ifc4x3_add2::IfcAlignmentVerticalSegmentTypeEnum::IfcAlignmentVerticalSegmentType_PARABOLICARC);
   auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(IfcParse::IfcGlobalId(), nullptr, name, boost::none, boost::none, nullptr, nullptr, design_parameters);
   m_file.addEntity(design_parameters);
   m_file.addEntity(alignment_segment);
   m_file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(m_VerticalAlignment, alignment_segment);
}

void ProfileBuilder::AddCircCurve(double start_dist_along, double elevation, double start_grade, double end_grade, double length,double radius, boost::optional<std::string> name)
{
   auto design_parameters = new Ifc4x3_add2::IfcAlignmentVerticalSegment(boost::none, boost::none, start_dist_along, length, elevation, start_grade, end_grade, radius, Ifc4x3_add2::IfcAlignmentVerticalSegmentTypeEnum::IfcAlignmentVerticalSegmentType_CIRCULARARC);
   auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(IfcParse::IfcGlobalId(), nullptr, name, boost::none, boost::none, nullptr, nullptr, design_parameters);
   m_file.addEntity(design_parameters);
   m_file.addEntity(alignment_segment);
   m_file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(m_VerticalAlignment, alignment_segment);
}
