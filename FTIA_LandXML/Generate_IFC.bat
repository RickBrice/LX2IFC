rem ..\x64\Release\LX2IFC.exe 3C03_Geom_Kupittaa-Turku
rem python -m ifcpatch -i 3C03_Geom_Kupittaa-Turku.ifc -o 3C03_Geom_Kupittaa-Turku_patch_1.ifc -r AddGeometricRepresentationToAlignment
rem python -m ifcpatch -i 3C03_Geom_Kupittaa-Turku_patch_1.ifc -o 3C03_Geom_Kupittaa-Turku_patch_2.ifc -r AddZeroLengthSegmentToAlignment
rem python -m ifcpatch -i 3C03_Geom_Kupittaa-Turku_patch_2.ifc -o 3C03_Geom_Kupittaa-Turku_patch_3.ifc -r PatchStationReferentPosition
rem python -m ifcpatch -i 3C03_Geom_Kupittaa-Turku_patch_3.ifc -o 3C03_Geom_Kupittaa-Turku_patch_4.ifc -r AddLinearPlacementFallbackPosition

rem ..\x64\Release\LX2IFC.exe 3C03_Geom_TurkuAsema
rem python -m ifcpatch -i 3C03_Geom_TurkuAsema.ifc -o 3C03_Geom_TurkuAsema_patch_1.ifc -r AddGeometricRepresentationToAlignment
rem python -m ifcpatch -i 3C03_Geom_TurkuAsema_patch_1.ifc -o 3C03_Geom_TurkuAsemarem _patch_2.ifc -r AddZeroLengthSegmentToAlignment
rem python -m ifcpatch -i 3C03_Geom_TurkuAsema_patch_2.ifc -o 3C03_Geom_TurkuAsemarem _patch_3.ifc -r PatchStationReferentPosition
rem python -m ifcpatch -i 3C03_Geom_TurkuAsema_patch_3.ifc -o 3C03_Geom_TurkuAsema_patch_4.ifc -r AddLinearPlacementFallbackPosition

..\x64\Release\LX2IFC.exe 3C03_Geom_TurkuTavara
python -m ifcpatch -i 3C03_Geom_TurkuTavara.ifc -o 3C03_Geom_TurkuTavara_patch_1.ifc -r AddGeometricRepresentationToAlignment
python -m ifcpatch -i 3C03_Geom_TurkuTavara_patch_1.ifc -o 3C03_Geom_TurkuTavara_patch_2.ifc -r AddZeroLengthSegmentToAlignment
python -m ifcpatch -i 3C03_Geom_TurkuTavara_patch_2.ifc -o 3C03_Geom_TurkuTavara_patch_3.ifc -r PatchStationReferentPosition
python -m ifcpatch -i 3C03_Geom_TurkuTavara_patch_3.ifc -o 3C03_Geom_TurkuTavara_patch_4.ifc -r AddLinearPlacementFallbackPosition

rem ..\x64\Release\LX2IFC.exe BPaimio-Kupittaa_GK23_N2000_2020
rem python -m ifcpatch -i BPaimio-Kupittaa_GK23_N2000_2020.ifc -o BPaimio-Kupittaa_GK23_N2000_2020_patch_1.ifc -r AddGeometricRepresentationToAlignment
rem python -m ifcpatch -i BPaimio-Kupittaa_GK23_N2000_2020_patch_1.ifc -o BPaimio-Kupittaa_GK23_N2000_2020_patch_2.ifc -r AddZeroLengthSegmentToAlignment
rem python -m ifcpatch -i BPaimio-Kupittaa_GK23_N2000_2020_patch_2.ifc -o BPaimio-Kupittaa_GK23_N2000_2020_patch_3.ifc -r PatchStationReferentPosition
rem python -m ifcpatch -i BPaimio-Kupittaa_GK23_N2000_2020_patch_3.ifc -o BPaimio-Kupittaa_GK23_N2000_2020_patch_4.ifc -r AddLinearPlacementFallbackPosition