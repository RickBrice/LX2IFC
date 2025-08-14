#include "LX2IFC.h"
#include <numbers>

Ifc4x3_add2::IfcAlignment* LX2IFC::Alignment(LX::Alignment* lxalignment, IfcHierarchyHelper<Ifc4x3_add2>& file)
{
	USES_CONVERSION;

	auto site = file.getSingle<Ifc4x3_add2::IfcSite>();

	auto local_placement = site->ObjectPlacement();
	if (!local_placement)
	{
		local_placement = file.addLocalPlacement();
	}

	std::string name = "Unknown";
	if (lxalignment->hasValue_Name())
		name = W2A(lxalignment->getName().c_str());

	std::cout << "Alignment " << name << std::endl;

	auto alignment = new Ifc4x3_add2::IfcAlignment(IfcParse::IfcGlobalId(), nullptr, name, boost::none, boost::none, local_placement, nullptr, boost::none);
	file.addEntity(alignment);

	// IFC 4.1.4.1.1 "Every IfcAlignment must be related to IfcProject using the IfcRelAggregates relationship"
	// https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/concepts/Object_Composition/Aggregation/Alignment_Aggregation_To_Project/content.html
	// IfcProject <-> IfcRelAggregates <-> IfcAlignment
	auto project = file.getSingle<Ifc4x3_add2::IfcProject>();
	file.addRelatedObject<Ifc4x3_add2::IfcRelAggregates>(project, alignment);


	auto horizontal_alignment = new Ifc4x3_add2::IfcAlignmentHorizontal(IfcParse::IfcGlobalId(), nullptr, boost::none, boost::none, boost::none, nullptr, nullptr/*representation*/);
	file.addEntity(horizontal_alignment);
	file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(alignment, horizontal_alignment);

	//auto site = file.getSingle<typename Schema::IfcSite>();
	//file.relatePlacements(site, horizontal_alignment);

	//auto start = lxalignment->getStart();
	auto coord_geom = lxalignment->getCoordGeom();

	auto iter = coord_geom->GeomList().iterator();
	while (!iter->atEnd())
	{
		auto obj = iter->current();
		iter->next();

		auto line = dynamic_cast<LX::Line*>(obj);
		auto spiral = dynamic_cast<LX::Spiral*>(obj);
		auto curve = dynamic_cast<LX::Curve*>(obj);
		auto chain = dynamic_cast<LX::Chain*>(obj);
		auto irregularLine = dynamic_cast<LX::IrregularLine*>(obj);

		if (line)
		{
		   if (line->hasValue_Name())
			   std::wcout << _T("   ") << _T("Line ") << line->getName().c_str() << std::endl;
		   else
			  std::wcout << _T("   ") << _T("Line ") << std::endl;

		   boost::optional<std::string> name, desc;
		   if (line->hasValue_Name())
			  name = W2A(line->getName().c_str());

		   if (line->hasValue_Desc())
			  desc = W2A(line->getDesc().c_str());

			auto start = line->getStart();
			auto [sx, sy] = m_DataConverter.get_coordinates(start);
			auto p = file.addDoublet<Ifc4x3_add2::IfcCartesianPoint>(sx,sy);
			double dir;
			//if (line->hasValue_Dir())
			//{
			//	dir = m_DataConverter.convertDirectionToPlaneAngle(line->getDir());
			//}
			//else
			{
				auto end = line->getEnd();
				auto [ex, ey] = m_DataConverter.get_coordinates(end);
				dir = m_DataConverter.convertRadianToPlaneAngle(atan2(ey - sy, ex - sx));
			}
			auto start_radius = 0.;
			auto end_radius = 0.;
			auto length = line->getLength();
			auto predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_LINE;

			auto design_parameters = new Ifc4x3_add2::IfcAlignmentHorizontalSegment(
				boost::none, boost::none, p, dir, start_radius, end_radius, length, boost::none, predefined_type);

			auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(
				IfcParse::IfcGlobalId(), nullptr, name, desc, boost::none, nullptr, nullptr, design_parameters);

			file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(horizontal_alignment, alignment_segment);
		}
		else if (spiral)
		{
			std::wcout << _T("   ") << _T("Spiral ") << spiral->getName().c_str() << std::endl; 
			boost::optional<std::string> name, desc;
			if (spiral->hasValue_Name())
			   name = W2A(spiral->getName().c_str());

			if (spiral->hasValue_Desc())
			   desc = W2A(spiral->getDesc().c_str());

			double sign = spiral->getRot() == LX::EnumClockwise::k_cw ? -1. : 1.;
			auto start = spiral->getStart();
			auto [sx, sy] = m_DataConverter.get_coordinates(start);
			auto p = file.addDoublet<Ifc4x3_add2::IfcCartesianPoint>(sx, sy);
			double dir;
			//if (spiral->hasValue_DirStart())
			//{
			//	dir = m_DataConverter.convertDirectionToPlaneAngle(spiral->getDirStart());
			//}
			//else
			{
				auto pi = spiral->getPI();
				auto [pix, piy] = m_DataConverter.get_coordinates(pi);
				dir = m_DataConverter.convertRadianToPlaneAngle(atan2(piy - sy, pix - sx));
			}
			auto start_radius = sign*spiral->getRadiusStart();
			auto end_radius = sign*spiral->getRadiusEnd();
			auto length = spiral->getLength();
			auto predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_CLOTHOID;

			if (fabs(start_radius) == DBL_MAX)
				start_radius = 0.;

			if (fabs(end_radius) == DBL_MAX)
				end_radius = 0.;

			switch (spiral->getSpiType())
			{
			case LX::EnumSpiralType::k_null:
				std::wcout << _T("*** Unknown spiral ***") << std::endl;
			   break;

			case LX::EnumSpiralType::k_biquadratic:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_HELMERTCURVE;
			   break;

			case LX::EnumSpiralType::k_bloss:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_BLOSSCURVE;
			   break;

			case LX::EnumSpiralType::k_clothoid:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_CLOTHOID;
			   break;

			case LX::EnumSpiralType::k_cosine:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_COSINECURVE;
			   break;

			case LX::EnumSpiralType::k_cubic:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_CUBIC;
			   break;

			case LX::EnumSpiralType::k_sinusoid:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_SINECURVE;
			   break;

			case LX::EnumSpiralType::k_revBiquadratic:
				std::wcout << _T("*** Unknown spiral - revBiquadratic ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_revBloss:
				std::wcout << _T("*** Unknown spiral - revBloss ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_revCosine:
				std::wcout << _T("*** Unknown spiral - revCosine ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_revSinusoid:
				std::wcout << _T("*** Unknown spiral - revSinusoid ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_sineHalfWave:
			   // LandXML Spiral Document (1.1 Schema) - Sine Half-Wavelength is an approximate of a Cosine Spiral
				predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_COSINECURVE;
				break;
			case LX::EnumSpiralType::k_biquadraticParabola:
			   predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_HELMERTCURVE;
			   break;
			case LX::EnumSpiralType::k_cubicParabola:
				predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_CUBIC;
				break;
			case LX::EnumSpiralType::k_japaneseCubic:
				predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_VIENNESEBEND;
				break;
			case LX::EnumSpiralType::k_radioid:
				std::wcout << _T("*** Unknown spiral - radioid ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_weinerBogen:
				std::wcout << _T("*** Unknown spiral - weinerBogen ***") << std::endl;
				break;
			}

			auto design_parameters = new Ifc4x3_add2::IfcAlignmentHorizontalSegment(
			   boost::none, boost::none, p, dir, start_radius, end_radius, length, boost::none, predefined_type);

			auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(
			   IfcParse::IfcGlobalId(), nullptr, name, desc, boost::none, nullptr, nullptr, design_parameters);

			file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(horizontal_alignment, alignment_segment);
		}
		else if (curve)
		{
			std::wcout << _T("   ") << _T("Curve ") << curve->getName().c_str() << std::endl;
			boost::optional<std::string> name, desc;
			if (curve->hasValue_Name())
			   name = W2A(curve->getName().c_str());

			if (curve->hasValue_Desc())
			   desc = W2A(curve->getDesc().c_str());

			double sign = curve->getRot() == LX::EnumClockwise::k_cw ? -1. : 1.;
			auto start = curve->getStart();
			auto [sx, sy] = m_DataConverter.get_coordinates(start);
			auto p = file.addDoublet<Ifc4x3_add2::IfcCartesianPoint>(sx, sy);

			auto center = curve->getCenter();
			auto [cx, cy] = m_DataConverter.get_coordinates(center);

			auto end = curve->getEnd();
			auto [ex, ey] = m_DataConverter.get_coordinates(end);

			auto radius = sqrt((sx - cx) * (sx - cx) + (sy - cy) * (sy - cy));
			if (curve->hasValue_Radius())
			{
			   radius = curve->getRadius();
			}

			auto dir = atan2(sy - cy, sx - cx); // direction of radial line
			dir += sign*std::numbers::pi / 2; // rotate by 90 to get direction of tangent line
			dir = m_DataConverter.convertRadianToPlaneAngle(dir);

			auto start_radius = sign*curve->getRadius();
			auto end_radius = start_radius;

			auto length = curve->getLength();
			auto predefined_type = Ifc4x3_add2::IfcAlignmentHorizontalSegmentTypeEnum::IfcAlignmentHorizontalSegmentType_CIRCULARARC;

			auto design_parameters = new Ifc4x3_add2::IfcAlignmentHorizontalSegment(
			   boost::none, boost::none, p, dir, start_radius, end_radius, length, boost::none, predefined_type);

			auto alignment_segment = new Ifc4x3_add2::IfcAlignmentSegment(
			   IfcParse::IfcGlobalId(), nullptr, name, desc, boost::none, nullptr, nullptr, design_parameters);

			file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(horizontal_alignment, alignment_segment);
		}
		else if (chain)
		{
			std::wcout << _T("   ") << _T("Chain ") << chain->getName().c_str() << std::endl;
		}
		else if (irregularLine)
		{
			std::wcout << _T("   ") << _T("Irregular Line ") << irregularLine->getName().c_str() << std::endl;
		}
	}

	double start_station = 0.0;
	if (lxalignment->hasValue_StaStart())
	{
	   start_station = lxalignment->getStaStart();
	}

	Profile(start_station,lxalignment, alignment, file);
	Cant(start_station, lxalignment, alignment, file);


    StationReferent(alignment, file, start_station, boost::none, boost::none);

	auto eq_iter = lxalignment->StaEquation().iterator();
	while (!eq_iter->atEnd())
	{
		auto eq = (LX::StaEquation*)eq_iter->current();
		auto station = eq->getStaAhead();
        boost::optional<double> incoming;
		if (eq->hasValue_StaBack() && !std::isnan(eq->getStaBack()))
		{
			incoming = eq->getStaBack();
		}
		if (station != eq->getStaInternal())
		{
			std::cout << "Station and Internal Assumption Violated" << std::endl;
		}
		//auto internal_station = eq->getStaInternal(); // this is the station without breaks - may need to skip this and compute it from the IfcOpenShell side
		//// this value is needed for the linear placement
		boost::optional<std::string> desc;
		if (eq->hasValue_Desc())
		{
		   desc = std::string(W2A(eq->getDesc().c_str()));
		}
        StationReferent(alignment, file, station, incoming, desc);
		eq_iter->next();
	}

	return alignment;
}