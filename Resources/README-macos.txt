# GEM-Fits v2

Welcome to GEM-Fits v2 program!

GEMSFITS Parameter Optimization and Inverse Modelling

## How to run GEM-Fits

* To run GEM-Fits application, execute the the gem-fits-shell.app

At the first attempt to start gem-selektor.app results in security alert about "unidentified developer", which requires to go in Settings Privacy & Security and give a special permission to run the program. 
Go to Settings / Privacy and Security, scroll down, and confirm to run gem-fits-shell.app 

Another way to open a blocked app is to locate the app in a Finder window. 

    Open the Finder.
    Locate the app (it might be in the Applications folder, or it might still be in your downloads folder).
    Ctrl-click or right-click on the app.
    Select Open from the resultant menu and the app will be opened anyway, and an exception will be created for opening it normally (i.e. by double-clicking) in the future.

The next runs can be started by clicking on gem-selector.app in Finder or its index in the dock. It may be necessary to go to Settings / Privacy and Security, scroll down, and confirm to run gem-fits-shell.app once.

* Alternatively you can lauch a terminal (In the Finder , open the /Applications/Utilities folder, then double-click Terminal) and execute the following:

```sh
./runshell.sh
```

* For more details about command line parameters, see into `runshell.sh`. Edit the file `runshell.sh` (with any simple text editor) in order to ensure that GEMFits command line parameters point to correct locations of the program resources and of modeling projects.

## Where are my projects?

Modeling projects (including the test projects bundled with this installation, which are copied there automatically on first start) live in `~/Library/GemFits/projects` in your home folder.

macOS hides the `Library` folder in Finder by default. To see it, open Finder, select your home folder, then go to `View -> Show View Options` and tick `Show Library Folder` (or press `Cmd+Shift+.` in any Finder window to toggle hidden files/folders).

## Briefly about GEM-Fits v2

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

Can fit internally consistent input parameters of chemical thermodynamic models against experimental data.

Coupled with the GEM-Standalone code for Gibbs Energy Minimization calculation of equilibrium states.

Uses the NLOpt nonlinear optimization library.

Allows for bound, (non-)linear (in-)equality constrained minimization of weighted squared residuals.

Written in C/C++, available with source code, and fully parallelizable on HPC architectures.

Provides tools for statistical analysis with thorough evaluation of fitted parameters' uncertainty.

Results of fitting and statistics can be visualized and saved in various graphical formats.

Can import, manage, and query extensive sets of experimental data accumulated in NoSQL database files.


Permission to use the GEM-Fits software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Fits code, please, cite the web page (http://gems.web.psi.ch) and the following papers:

Miron G.D., Kulik D.A., Dmytrieva S.V., Wagner T. (2015): GEMSFITS: Code package for optimization of geochemical model parameters and inverse modeling. Applied Geochemistry 55, 28-45. doi.

DISCLAIMER: thermodynamic data bases in test modeling projects provided in this installation are provisional and undocumented. The Development Team accepts no liability of any kind for any losses or inconveniences that may result from use of the data provided in modeling projects for testing and educational purposes. No guarantee is given that the input data in test projects are, in fact, accurate. 

**Enjoy GEM-Fits!**

(c) 2003-2026 GEMS Development Team

gems2.support@psi.ch

http://gems.web.psi.ch

http://gems.web.psi.ch
 

