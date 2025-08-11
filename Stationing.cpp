#include "LX2IFC.h"

void LX2IFC::StationReferent(Ifc4x3_add2::IfcAlignment* alignment, IfcHierarchyHelper<Ifc4x3_add2>& file, double station, boost::optional<double> incoming)
{
   // Note that referent placement is required, but it can't be created without the geometric representation of the alignment
   // This LX2IFC converter is only creating business logic. The IfcOpenShell API/IfcPatch recipes will need to fix this
   Ifc4x3_add2::IfcLinearPlacement* referent_placement = nullptr;

   // Create referent
   std::ostringstream oss;
   if(incoming.has_value())
   {
      oss << "Station Equation (" << *alignment->Name() << ")" << std::endl;
   }
   else
   {
      oss << "Start of alignment " << *alignment->Name() << std::endl;
   }
   auto start_station_referent = new Ifc4x3_add2::IfcReferent(IfcParse::IfcGlobalId(), nullptr, oss.str(), boost::none, boost::none, referent_placement, nullptr, Ifc4x3_add2::IfcReferentTypeEnum::IfcReferentType_STATION);

   // Define properties for Pset_Stationing
   typename aggregate_of<Ifc4x3_add2::IfcProperty>::ptr pset_station_properties(new aggregate_of<Ifc4x3_add2::IfcProperty>());
   pset_station_properties->push(new Ifc4x3_add2::IfcPropertySingleValue(std::string("Station"), boost::none, new Ifc4x3_add2::IfcLengthMeasure(station), nullptr));

   if (incoming.has_value())
   {
      auto value = *incoming;
      pset_station_properties->push(new Ifc4x3_add2::IfcPropertySingleValue(std::string("Incoming_Station"), boost::none, new Ifc4x3_add2::IfcLengthMeasure(value), nullptr));
   }

   // Create Pset and assign properties
   auto property_set = new Ifc4x3_add2::IfcPropertySet(IfcParse::IfcGlobalId(), nullptr, std::string("Pset_Stationing"), boost::none, pset_station_properties);
   file.addEntity(property_set);

   // Assign the property set to the referent
   typename aggregate_of<Ifc4x3_add2::IfcObjectDefinition>::ptr referents(new aggregate_of<Ifc4x3_add2::IfcObjectDefinition>());
   referents->push(start_station_referent);

   auto rel_defines_by_properties = new Ifc4x3_add2::IfcRelDefinesByProperties(IfcParse::IfcGlobalId(), nullptr, std::string("Relates start station properties to referent"), boost::none, referents, property_set);
   file.addEntity(rel_defines_by_properties);

   // search every alignment nest for one that contains an IfcReferent.
   Ifc4x3_add2::IfcRelNests* referent_nest = nullptr;
   auto nests = alignment->IsNestedBy();
   for (auto n : *nests)
   {
      auto related_objects = n->RelatedObjects();
      for (auto object : *related_objects)
      {
         if (object->as<Ifc4x3_add2::IfcReferent>())
         {
            // found the referent nest
            referent_nest = n;
            referent_nest->RelatedObjects()->push(start_station_referent);
            break;
         }
      }
   }
   if(referent_nest == nullptr)
   {
      // referent nest not found, create a new one
      referent_nest = new Ifc4x3_add2::IfcRelNests(IfcParse::IfcGlobalId(), nullptr, std::string("Referent Nest"), boost::none, alignment, referents);
      file.addEntity(referent_nest);
   }
}
