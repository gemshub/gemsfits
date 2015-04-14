To start gemsfits run rungemsfit.sh script. if the script is not 
executable, open a terminal, cd to that directory, and run 
   	$ chmod u+x ./rungemsfits.sh.

Before runing the first fitting task check if the path to the 
gemsfits2.exe is correctly defined in the "Help" "Preferences...", 
by clicking "..." and navigating to the correct place of the gemsfit2 
executable file found in gemsfits-app/ folder.

Note that before updating/re-installing GEMSFITS to the same location, 
the user will have to backup all your project subfolders in the "Tests" 
folder because they will be overwritten with those in the installer having 
identical names.


---------------------------------------------------------------------------------
To run gemsfit2 code outside the GUI directly using the terminal. Currently
works only by having the contents of the fititng project on the same level 
as gemsfit2 executable. 

Make rungesmfit2.sh executable:
	$ chmod u+x ./rungemsfit2.sh.

Copy the contents of the porject you want to fit (i.e. /GEMS, /EJDB and the
already prepared/generated *.json task specifiaction file) inside the gemsfits-app 
folder.

Navigate inside the gemsfits-app folder and run:
To create a template task specification file 
	$ ./rungemsfits.sh -initj ./GEMS/<GEMS3K input file list name*-dat.lst> </<name-of-task-file>.json
To run a fitting task
	$ ./rungemsfits.sh -run 1 ./<name-of-task-file>.json





 
