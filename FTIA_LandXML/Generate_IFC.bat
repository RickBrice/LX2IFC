..\x64\Release\LX2IFC.exe 3C03_Geom_Kupittaa-Turku
python -m ifcpatch -i 3C03_Geom_Kupittaa-Turku.ifc -o 3C03_Geom_Kupittaa-Turku_patch_1.ifc -r AddGeometricRepresentationToAlignment
python -m ifcpatch -i 3C03_Geom_Kupittaa-Turku_patch_1.ifc -o 3C03_Geom_Kupittaa-Turku_patch_2.ifc -r AddZeroLengthSegmentToAlignment

..\x64\Release\LX2IFC.exe 3C03_Geom_TurkuAsema
python -m ifcpatch -i 3C03_Geom_TurkuAsema.ifc -o 3C03_Geom_TurkuAsema_patch_1.ifc -r AddGeometricRepresentationToAlignment
python -m ifcpatch -i 3C03_Geom_TurkuAsema_patch_1.ifc -o 3C03_Geom_TurkuAsema_patch_2.ifc -r AddZeroLengthSegmentToAlignment

..\x64\Release\LX2IFC.exe 3C03_Geom_TurkuTavara
python -m ifcpatch -i 3C03_Geom_TurkuTavara.ifc -o 3C03_Geom_TurkuTavara_patch_1.ifc -r AddGeometricRepresentationToAlignment
python -m ifcpatch -i 3C03_Geom_TurkuTavara_patch_1.ifc -o 3C03_Geom_TurkuTavara_patch_2.ifc -r AddZeroLengthSegmentToAlignment

..\x64\Release\LX2IFC.exe BPaimio-Kupittaa_GK23_N2000_2020
python -m ifcpatch -i BPaimio-Kupittaa_GK23_N2000_2020.ifc -o BPaimio-Kupittaa_GK23_N2000_2020_patch_1.ifc -r AddGeometricRepresentationToAlignment
python -m ifcpatch -i BPaimio-Kupittaa_GK23_N2000_2020_patch_1.ifc -o BPaimio-Kupittaa_GK23_N2000_2020_patch_2.ifc -r AddZeroLengthSegmentToAlignment