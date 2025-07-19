#include "Alignment.h"

Ifc4x3_add2::IfcAlignment* Alignment(LX::Alignment* lxalignment, IfcHierarchyHelper<Ifc4x3_add2>& file)
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

	// IFC 4.1.5.1 alignment is referenced in spatial structure of an IfcSpatialElement. In this case IfcSite is the highest level IfcSpatialElement
	// https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/concepts/Object_Connectivity/Alignment_Spatial_Reference/content.html
	// IfcSite <-> IfcRelReferencedInSpatialStructure <-> IfcAlignment
	// This means IfcAlignment is not part of the IfcSite (it is not an aggregate component) but instead IfcAlignment is used within
	// the IfcSite by reference. This implies an IfcAlignment can traverse many IfcSite instances within an IfcProject
	//file.addRelatedObject<Ifc4x3_add2::IfcRelReferencedInSpatialStructure>(site, alignment);


	auto horizontal_alignment = new Ifc4x3_add2::IfcAlignmentHorizontal(IfcParse::IfcGlobalId(), nullptr, boost::none, boost::none, boost::none, nullptr, nullptr/*representation*/);
	file.addEntity(horizontal_alignment);
	file.addRelatedObject<Ifc4x3_add2::IfcRelNests>(alignment, horizontal_alignment);

	//auto site = file.getSingle<typename Schema::IfcSite>();
	//file.relatePlacements(site, horizontal_alignment);

	auto start = lxalignment->getStart();
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
			auto x = start->at(0);
			auto y = start->at(1);
			auto p = new Ifc4x3_add2::IfcCartesianPoint({ x,y });
			auto dir = line->getDir();
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

			auto start = spiral->getStart();
			auto x = start->at(0);
			auto y = start->at(1);
			auto p = new Ifc4x3_add2::IfcCartesianPoint({ x,y });
			auto dir = spiral->getDirStart();
			auto start_radius = spiral->getRadiusStart();
			auto end_radius = spiral->getRadiusEnd();
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
				std::wcout << _T("*** Unknown spiral - sineHalfWave ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_biquadraticParabola:
				std::wcout << _T("*** Unknown spiral - biquadraticParabola ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_cubicParabola:
				std::wcout << _T("*** Unknown spiral - cubicParabola ***") << std::endl;
				break;
			case LX::EnumSpiralType::k_japaneseCubic:
				std::wcout << _T("*** Unknown spiral - japeneseCubic ***") << std::endl;
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

			auto start = curve->getStart();
			auto x = start->at(0);
			auto y = start->at(1);
			auto p = new Ifc4x3_add2::IfcCartesianPoint({ x,y });
			auto dir = curve->getDirStart();
			auto start_radius = curve->getRadius();
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


	auto& profile_collection = lxalignment->Profile();
	auto profileIter = profile_collection.iterator();
	while (!profileIter->atEnd())
	{
		auto profile = profileIter->current();
		LX::String strProfileName = profile->hasValue_Name() ? profile->getName() : LX::String(_T("<Unnamed>"));
		std::wcout << _T("   ") << strProfileName << std::endl;
		profileIter->next();

		auto& profile_alignment_collection = profile->ProfAlign();
		auto profile_alignment_iter = profile_alignment_collection.iterator();
		while (!profile_alignment_iter->atEnd())
		{
			auto profile_alignment = profile_alignment_iter->current();
			auto& vert_geom = profile_alignment->VertGeomList();
			auto iter = vert_geom.iterator();
			while (!iter->atEnd())
			{
				auto obj = iter->current();
				auto pvi = dynamic_cast<LX::PVI*>(obj);
				auto para_curve = dynamic_cast<LX::ParaCurve*>(obj);
				auto unsym_para_curve = dynamic_cast<LX::UnsymParaCurve*>(obj);
				auto circ_curve = dynamic_cast<LX::CircCurve*>(obj);
				if (pvi)
				{
					std::wcout << _T("        ") << _T("PVI ") << std::endl;
				}
				else if (para_curve)
				{
					std::wcout << _T("        ") << _T("ParaCurve ") << std::endl;
				}
				else if (unsym_para_curve)
				{
					std::wcout << _T("        ") << _T("UnsymParaCurve ") << std::endl;
				}
				else if (circ_curve)
				{
					std::wcout << _T("        ") << _T("CircCurve ") << std::endl;
				}
				iter->next();
			}
			profile_alignment_iter->next();
		}
	}

	return alignment;
}