# MicroMouse
This repository contains the following repositories and descriptions:

**Cal Poly SLO 1995 Micromouse Code** - MMTEST25.C program code that controls a right-wall follower. Team members Tom Roberts, Jeff Bain and two others (sorry I forgot your names) participated in Cal Poly San Luis Obispo hosted Micromouse district championships. We were the only participants so we took 1st place.

**Cal Poly SLO 1996 RoboRodentia Code** - ROBORodentia16.C program code that controls a modified micromouse robot that incorporated a front loader to pick up balls on the course. The robot then had to dump the balls at a specific location. Team members Tom Roberts and Jeff Bain. 1st place out of 5 teams.

**Cal Poly SLO 1996 Micromouse Code** - MAPPER_SLO_AT_SFSU.C program code that controls a micromouse robot. Team members Tom Roberts and Jeff Bain. 1st place at Cal State Northridge Region 6 (southern) district championships. 1st place at region 6 (northern) district championships at Cal State San Francisco. We were later disqualified since we were not allowed to officially compete in the northern district.

![](https://github.com/namgengr/MicroMouse/Cal Poly SLO 1996 Micromouse Code/blob/master/Images/CP_SLO_Micromouse_1996.jpg?raw=true)

**Simulator** - This simulator runs all sixteen direction preferences and compiles statistics for each. Find the direction preference that solves for the shortest path to the center of the maze. Or maybe your battery runtime is low and you want to find the direction preference that gives the minumum number of squares traversed to reach the center. You can also experiment by customizing your own maze using a simple text file.

Instructions:
1. Download .msi file from this repository. Install on your computer.
2. Use File/Open in the menu to select a .map maze file from the application directory.
3. Select Run in the menu. Application will run through sixteen direction preferences compiling data for each run. This will take anywhere from 3-5 seconds depending on the speed of your computer.
4. All sixteen direction preferences are selectible from the menu. Some direction preferences are more desireable than others based on run statistics.

In addition to the .map files located in the installation directory, you can create and simulate your own .map files.

.map file definitions
Line 1: maze title in quotes, example "Practice Maze"
Lines 2 through 17: 16 comma delimited integers

Integer definitions:
1 - west wall,
2 - east wall,
3 - west and east walls,
4 - south wall,
5 - west and south walls,
6 - south and east walls,
7 - west, south and east walls,
8 - north wall,
9 - west and north walls,
10 - east and north walls,
11 - west, east and north walls,
12 - south and north walls,

![](https://github.com/namgengr/MicroMouse/Simulator/blob/master/Images/solutiondone.gif?raw=true)

![](https://github.com/namgengr/MicroMouse/Simulator/blob/master/Images/shortest_path.gif?raw=true)
