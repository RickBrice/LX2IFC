#include "LX2IFC.h"
#include <numbers>

////auto project = file.addProject(); // Don't like the default units in IfcOpenShell so we have do build our own
///////////////////////////// The following is copied from IfcHierarchyHelper<Schema>::addProject and tweaked
//typename Schema::IfcUnit::list::ptr units(new typename Schema::IfcUnit::list);
//
//auto* unit1 = new Schema::IfcSIUnit(Schema::IfcUnitEnum::IfcUnit_LENGTHUNIT, boost::none, Schema::IfcSIUnitName::IfcSIUnitName_METRE);
//auto* unit2 = new Schema::IfcSIUnit(Schema::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT, boost::none, Schema::IfcSIUnitName::IfcSIUnitName_RADIAN);
//
//units->push(unit1);
//units->push(unit2);
//
//auto* unit_assignment = new Schema::IfcUnitAssignment(units);
//
//typename Schema::IfcRepresentationContext::list::ptr rep_contexts(new typename Schema::IfcRepresentationContext::list);
//auto* project = new Schema::IfcProject(IfcParse::IfcGlobalId(), owner_history, std::string("MyProject"), boost::none, boost::none, boost::none, boost::none, rep_contexts, unit_assignment);
//
//file.addEntity(unit1);
//file.addEntity(unit2);
//file.addEntity(unit_assignment);
//file.addEntity(project);

Ifc4x3_add2::IfcUnitAssignment* LX2IFC::Units(LX::Units* lxUnits, IfcHierarchyHelper<Ifc4x3_add2>& file)
{
   Ifc4x3_add2::IfcUnit::list::ptr units(new Ifc4x3_add2::IfcUnit::list);

   LX::Object* pSelectedUnits = lxUnits->getSelectedUnits();
   LX::Metric* pMetric = dynamic_cast<LX::Metric*>(pSelectedUnits);
   LX::Imperial* pImperial = dynamic_cast<LX::Imperial*>(pSelectedUnits);

   if (pMetric)
   {
      m_DataConverter.m_bSIUnits = true;
      m_DataConverter.m_EnumMetLinear = pMetric->getLinearUnit();
      switch (m_DataConverter.m_EnumMetLinear)
      {
      case LX::EnumMetLinear::k_centimeter:
         break;

      case LX::EnumMetLinear::k_kilometer:
         break;

      case LX::EnumMetLinear::k_meter:
      {
         auto* unit1 = new Ifc4x3_add2::IfcSIUnit(Ifc4x3_add2::IfcUnitEnum::IfcUnit_LENGTHUNIT, boost::none, Ifc4x3_add2::IfcSIUnitName::IfcSIUnitName_METRE);
         units->push(unit1);
         }
         break;

      case LX::EnumMetLinear::k_millimeter:
         break;

      default:
         //XML_THROW(_T("An unknown unit of measure of encountered"));
         break;
      }

      m_DataConverter.m_directionUnit = (pMetric->hasValue_DirectionUnit() ? pMetric->getDirectionUnit() : LX::EnumAngularType::k_radians);
      m_DataConverter.m_angleUnit = (pMetric->hasValue_AngularUnit() ? pMetric->getAngularUnit() : LX::EnumAngularType::k_radians);
      if (pMetric->hasValue_AngularUnit())
      {
         switch (pMetric->getAngularUnit())
         {
         case LX::EnumAngularType::k_null:
            break;

         case LX::EnumAngularType::k_radians:
            break;

         case LX::EnumAngularType::k_grads:
         {
            //m_DataConverter.m_directionUnit = LX::EnumAngularType::k_decimal_degrees;
            //auto dimexp = new Ifc4x3_add2::IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
            //auto unit2a = new Ifc4x3_add2::IfcSIUnit(Ifc4x3_add2::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT,
            //   boost::none,
            //   Ifc4x3_add2::IfcSIUnitName::IfcSIUnitName_RADIAN);
            //auto unit2b = new Ifc4x3_add2::IfcMeasureWithUnit(
            //   new Ifc4x3_add2::IfcPlaneAngleMeasure(std::numbers::pi / 180.), unit2a);
            //auto unit2 = new Ifc4x3_add2::IfcConversionBasedUnit(dimexp,
            //   Ifc4x3_add2::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT,
            //   "Degrees",
            //   unit2b);
            //units->push(unit2);
            auto dimexp = new Ifc4x3_add2::IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
            auto unit2a = new Ifc4x3_add2::IfcSIUnit(Ifc4x3_add2::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT,
               boost::none,
               Ifc4x3_add2::IfcSIUnitName::IfcSIUnitName_RADIAN);
            auto unit2b = new Ifc4x3_add2::IfcMeasureWithUnit(
               new Ifc4x3_add2::IfcPlaneAngleMeasure(std::numbers::pi/200.), unit2a);
            auto unit2 = new Ifc4x3_add2::IfcConversionBasedUnit(dimexp,
               Ifc4x3_add2::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT,
               "Gradian",
               unit2b);
            units->push(unit2);
            break;
         }

         case LX::EnumAngularType::k_decimal_degrees:
         {
            auto dimexp = new Ifc4x3_add2::IfcDimensionalExponents(0, 0, 0, 0, 0, 0, 0);
            auto unit2a = new Ifc4x3_add2::IfcSIUnit(Ifc4x3_add2::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT,
               boost::none,
               Ifc4x3_add2::IfcSIUnitName::IfcSIUnitName_RADIAN);
            auto unit2b = new Ifc4x3_add2::IfcMeasureWithUnit(
               new Ifc4x3_add2::IfcPlaneAngleMeasure(std::numbers::pi / 180.), unit2a);
            auto unit2 = new Ifc4x3_add2::IfcConversionBasedUnit(dimexp,
               Ifc4x3_add2::IfcUnitEnum::IfcUnit_PLANEANGLEUNIT,
               "Degrees",
               unit2b);
            units->push(unit2);
            break;
         }

         case LX::EnumAngularType::k_decimal_dd_mm_ss:
            break;
         }
      }
   }
   else if (pImperial)
   {
      m_DataConverter.m_bSIUnits = false;
      m_DataConverter.m_EnumImpLinear = pImperial->getLinearUnit();
      switch (m_DataConverter.m_EnumImpLinear)
      {
      case LX::EnumImpLinear::k_foot:
         break;

      case LX::EnumImpLinear::k_USSurveyFoot:
         break;

      case LX::EnumImpLinear::k_inch:
         break;

      case LX::EnumImpLinear::k_mile:
         break;

      default:
         //XML_THROW(_T("An unknown unit of measure of encountered"));
         break;
      }
   }
   else
   {
      //XML_THROW(_T("An unknown unit type of encountered"));
   }


   auto* unit_assignment = new Ifc4x3_add2::IfcUnitAssignment(units);
   file.addEntity(unit_assignment);
   return unit_assignment;
}
