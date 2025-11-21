# GEM-Fits v2

Welcome to GEM-Fits v2 program!

GEMSFITS Parameter Optimization and Inverse Modelling

## How to run GEM-Fits

* To run GEM-Fits appilcation for the first time execute:

```
./rungems3.sh
```

* To add the icon for the GEM-Fits application to the launcher, edit the desktop entry file `./GemsFits-app/share/applications/gem-fits-shell.desktop` that contains a description of the application that includes information about its icon. Change the path to the actual location of gem-fits-shell executable. The files are typically executable and can be placed in specific directories like  `~/.local/share/applications`. 
Then copy folder `./GemsFits-app/share` to  `~/.local`

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
 

