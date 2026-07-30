# GEM-Fits v2

Welcome to GEM-Fits v2 program!

GEMSFITS Parameter Optimization and Inverse Modelling

## How to run GEM-Fits

* To run GEM-Fits appilcation for the first time execute:

```
runshell.bat
```

If windows will show you a warning message "Windows protected your PC", click "More info" and "Run anyway" (This will only be asked the first time).

Once GEM-Fits is running you can pin the program to the Task bar by right-clicking on its icon (on the task bar) and chose pin to task bar. After you close GEM-Fits, the script will create a Start Menu and Desktop shortcut. After the first run you can always start gems using its shortcut.

Use `runshell.bat` (or the Start Menu/Desktop shortcut it creates) to start GEM-Fits. If that
doesn't work for some reason, you can also run `gem-fits-shell.exe` directly from
`GemFits-app\bin` - but if this machine has another Qt installation (Anaconda, Qt Creator, etc.)
that sets a `QT_QPA_PLATFORM_PLUGIN_PATH` or `QT_PLUGIN_PATH` environment variable, GEM-Fits may
fail to start that way with `Could not find the Qt platform plugin "windows"`, since `runshell.bat`
and its shortcuts point those variables at the plugins bundled with this installation before
launching, while the raw exe does not.

## Where are my projects?

Modeling projects (including the test projects bundled with this installation, which are copied there automatically on first start) live in `Library\GemFits\projects` in your user home folder (e.g. `C:\Users\<user>\Library\GemFits\projects`). Unlike on macOS, this `Library` folder is not hidden, so it can be opened directly in File Explorer or via the address bar.

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
 

