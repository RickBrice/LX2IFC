#include "LX2IFC.h"

void LX2IFC::StationReferent(Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file, double station, boost::optional<double> incoming)
{
   // Note that referent placement is required, but it can't be created without the geometric representation of the alignment
   // This LX2IFC converter is only creating business logic. The IfcOpenShell API/IfcPatch recipes will need to fix this
   Ifc4x3_add2::IfcLinearPlacement* referent_placement = nullptr;

   // Create referent
   auto start_station_referent = new Ifc4x3_add2::IfcReferent(IfcParse::IfcGlobalId(), nullptr, std::string("Start of alignment station"), boost::none, boost::none, referent_placement, nullptr, Ifc4x3_add2::IfcReferentTypeEnum::IfcReferentType_STATION);

   // Define properties for Pset_Stationing
   typename aggregate_of<Ifc4x3_add2::IfcProperty>::ptr pset_station_properties(new aggregate_of<Ifc4x3_add2::IfcProperty>());
   pset_station_properties->push(new Ifc4x3_add2::IfcPropertySingleValue(std::string("Station"), boost::none, new Ifc4x3_add2::IfcLengthMeasure(station), nullptr));

   if (incoming.has_value())
   {
      pset_station_properties->push(new Ifc4x3_add2::IfcPropertySingleValue(std::string("Incoming_Station"), boost::none, new Ifc4x3_add2::IfcLengthMeasure(*incoming), nullptr));
   }

   // Create Pset and assign properties
   auto property_set = new Ifc4x3_add2::IfcPropertySet(IfcParse::IfcGlobalId(), nullptr, std::string("Pset_Stationing"), boost::none, pset_station_properties);
   file.addEntity(property_set);

   // Assign the property set to the referent
   typename aggregate_of<Ifc4x3_add2::IfcObjectDefinition>::ptr referents(new aggregate_of<Ifc4x3_add2::IfcObjectDefinition>());
   referents->push(start_station_referent);

   auto rel_defines_by_properties = new Ifc4x3_add2::IfcRelDefinesByProperties(IfcParse::IfcGlobalId(), nullptr, std::string("Relates start station properties to referent"), boost::none, referents, property_set);
   file.addEntity(rel_defines_by_properties);

   // WORKING HERE - This is where we need to figure out if the alignment has any IfcRelNests, create them, etc
   // and put the referent in the right place
}
