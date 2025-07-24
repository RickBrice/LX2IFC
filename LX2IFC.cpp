// LX2IFC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include "LX2IFC.h"
#include "Alignment.h"
#include "Units.h"

#include <string>
#include <cstdlib>

#include <fstream>

//WORKING HERE
//Need to do the following
//* Keep track of unit system at the LX2IFC class level
//* Need to call CantValue function to convert the input cant from LX, which is millimeter or inch, into the IFC file units
//* Need to keep LX unit information at the LX2IFC class level
//* Need to have functions to convert values from LX to IFC. For example, Direction is measured CCW for north and Angle is CCW from E.
//  Also, direction and angle can have different units of measure (say degrees and gradian) but there is only one angular unit in IFC.
//  There are various different "length" types in LX (width, height, etc) each of which can have different units of measure, but there is only one in IFC
//* Check the Finnish Rail XML files... the units for direction/angle say grad, but it seems like the are actually degrees.

#include <numbers>
double DataConverter::convertCant(double c) const
{
	if (m_bSIUnits)
		return c / 1000.; // cant is in millimeters, so convert to meters
	else
		return c / 12.; // cant is in inches, so convert to feet
}

double DataConverter::convertPlaneAngleToRadian(double d)
{
	double factor = 1.;
	switch (m_directionUnit)
	{
	case LX::EnumAngularType::k_null:
		break;

	case LX::EnumAngularType::k_radians:
		break;

	case LX::EnumAngularType::k_grads:
		factor = std::numbers::pi / 200.;
		break;

	case LX::EnumAngularType::k_decimal_degrees:
		factor = std::numbers::pi / 180.;
		break;

	case LX::EnumAngularType::k_decimal_dd_mm_ss:
		break;
	}

	d *= factor; // convert to radians

	return d;
}


double DataConverter::convertRadianToPlaneAngle(double d)
{
	double factor = 1.;
	switch (m_directionUnit)
	{
	case LX::EnumAngularType::k_null:
		break;

	case LX::EnumAngularType::k_radians:
		break;

	case LX::EnumAngularType::k_grads:
		factor = std::numbers::pi / 200.;
		break;

	case LX::EnumAngularType::k_decimal_degrees:
		factor = std::numbers::pi / 180.;
		break;

	case LX::EnumAngularType::k_decimal_dd_mm_ss:
		break;
	}

	d /= factor; // convert from radians

	return d;
}

double DataConverter::convertDirectionToPlaneAngle(double d)
{
	double offset = 0.;
	double max;
	switch (m_directionUnit)
	{
	case LX::EnumAngularType::k_null:
		break;

	case LX::EnumAngularType::k_radians:
		offset = std::numbers::pi / 2;
		max = 2 * std::numbers::pi;
		break;

	case LX::EnumAngularType::k_grads:
		offset = 100.;
		max = 400;
		break;

	case LX::EnumAngularType::k_decimal_degrees:
		offset = 90.;
		max = 360.;
		break;

	case LX::EnumAngularType::k_decimal_dd_mm_ss:
		break;
	}

	d += offset; // changes from North=0 to East=0

	// normalize the angle if needed
	if (max <= d)
		d -= max;
	else if (d < 0)
		d += max;

	return d;
}

class EventSink : public LX::IParserEventSink
{
public:
	virtual LX::IParserEventSink::ActionCode onEvent(LX::IParserEventSink::Severity eSeverity,
		LX::IParserEventSink::EventCode eCode,
		const wchar_t* strCurrentElement,
		int nCurrentElementLen,
		int nLineNumber,
		const wchar_t* strMessage)
	{
		std::wcout << strMessage << std::endl;
		return LX::IParserEventSink::ActionCode::kContinue;
	}
};

LX2IFC::LX2IFC()
{
}

void LX2IFC::Convert(std::string filename)
{
   USES_CONVERSION;

	std::string input_file(filename);
	input_file += ".xml";

	EventSink event_sink;

	auto LxDoc = LX::createDocumentObject();
	LxDoc->loadXml(A2W(input_file.c_str()), &event_sink);
	auto landXML = LxDoc->rootObject();
	auto* pProject = landXML->getProject();

	IfcHierarchyHelper<Ifc4x3_add2> file;
	auto unit_assignment = Units(landXML->getUnits(), file);
	Ifc4x3_add2::IfcRepresentationContext::list::ptr rep_contexts(new Ifc4x3_add2::IfcRepresentationContext::list);
	auto* project = new Ifc4x3_add2::IfcProject(IfcParse::IfcGlobalId(), nullptr, std::string(W2A(pProject->getName().c_str())), boost::none, boost::none, boost::none, boost::none, rep_contexts, unit_assignment);

	//auto project = file.addProject();
	auto site = file.addSite(project);

	auto geometric_representation_context = file.getRepresentationContext(std::string("Model")); // creates the representation context if it doesn't already exist

	auto axis_model_representation_subcontext = new Ifc4x3_add2::IfcGeometricRepresentationSubContext(std::string("Axis"), std::string("Model"), geometric_representation_context, boost::none, Ifc4x3_add2::IfcGeometricProjectionEnum::IfcGeometricProjection_MODEL_VIEW, boost::none);
	file.addEntity(axis_model_representation_subcontext);





	// IFC 4.1.5.1 alignment is referenced in spatial structure of an IfcSpatialElement. In this case IfcSite is the highest level IfcSpatialElement
	// https://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/concepts/Object_Connectivity/Alignment_Spatial_Reference/content.html
	// IfcSite <-> IfcRelReferencedInSpatialStructure <-> IfcAlignment
	// This means IfcAlignment is not part of the IfcSite (it is not an aggregate component) but instead IfcAlignment is used within
	// the IfcSite by reference. This implies an IfcAlignment can traverse many IfcSite instances within an IfcProject
	Ifc4x3_add2::IfcSpatialReferenceSelect::list::ptr list_alignments_referenced_in_site(new Ifc4x3_add2::IfcSpatialReferenceSelect::list);

	auto& alignmentsCollection = landXML->Alignments();
	auto alignmentCollectionIter = alignmentsCollection.iterator();
	while (!alignmentCollectionIter->atEnd())
	{
		auto alignments = alignmentCollectionIter->current();
		auto& alignmentCollection = alignments->Alignment();
		auto alignmentIter = alignmentCollection.iterator();

		if (alignments->hasValue_Name())
			std::wcout << alignments->getName().c_str() << std::endl;

		if (alignmentIter)
		{
			while (!alignmentIter->atEnd())
			{
				auto alignment = alignmentIter->current();
				list_alignments_referenced_in_site->push(Alignment(alignment, file));
				alignmentIter->next();
			}
		}

		alignmentCollectionIter->next();
	}
	auto rel_referenced_in_spatial_structure = new Ifc4x3_add2::IfcRelReferencedInSpatialStructure(IfcParse::IfcGlobalId(), nullptr, boost::none, boost::none, list_alignments_referenced_in_site, site);
	file.addEntity(rel_referenced_in_spatial_structure);

	std::string output_file(filename);
	output_file += ".ifc";
	std::ofstream ofs(output_file);
	ofs << file;
}
