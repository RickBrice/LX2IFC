import ifcopenshell
import ifcopenshell.api.alignment
import ifcopenshell.util.alignment

file = ifcopenshell.open("D:\\LX2IFC\\FTIA_LandXML\\3C03_Geom_TurkuTavara_patch_1.ifc")

ifcopenshell.util.alignment.append_zero_length_segments(file)
