
cd build

del sp1.xml /F /Q
..\..\toolStack\misc\zmemmap.exe sp1.map
start excel sp1.xml

pause 0