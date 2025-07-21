// LX2IFC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include "Header.h"
#include "Alignment.h"
#include "Units.h"

#include <string>
#include <cstdlib>

#include <fstream>


std::wstring convertToWString(const char* charStr) {
	size_t len = 0;
	mbstowcs_s(&len, nullptr, 0, charStr, _TRUNCATE); // Get required length
	std::wstring wstr(len, L'\0');
	mbstowcs_s(&len, &wstr[0], len, charStr, _TRUNCATE);
	return wstr;
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

int main(int argc, char** argv)
{
   USES_CONVERSION;

	if (argc != 2) {
		std::cout << "usage: LX2IFC filename" << std::endl;
		return 1;
	}

	std::string input_file(argv[1]);
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

	std::string output_file(argv[1]);
	output_file += ".ifc";
	std::ofstream ofs(output_file);
	ofs << file;
}
