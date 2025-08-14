# GEM-Selektor v.3

Welcome to GEM-Selektor v3 program!

GEM-Selektor v.3 - the interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a built-in GEMS3K chemical solver.

## How to run GEM-Selektor

* To run GEM-Selektor application (Linux x86_64), open a terminal and execute the following:

(ensure that the file is executable by right-click -> Properties -> Premissions "Allow executing file as a program")

```sh
./rungems3.sh
```

If you have issues with starting GEMS due to segmentation fault, conflicting Linux system libraries, unsupported distro, etc. it is recommended that you build GEM-Selesktor from its source code as documented here: https://github.com/gemshub/GEMSGUI?tab=readme-ov-file#building-using-conda

* For more details about command line parameters, see into `rungems3.sh`. Edit the file `rungems3.sh` (with any simple text editor) in order to ensure that GEMS3 command line parameters point to correct locations of the program resources and of modeling projects.

* To add the icon for the GEM-Selector application to the launcher, edit the desktop entry file `./Gems3-app/share/applications/gem-selektor.desktop` that contains a description of the application that includes information about its icon. Change the path to the actual location of gem-selektor executable. The files are typically executable and can be placed in specific directories like  `~/.local/share/applications`. 
Then copy folder `./Gems3-app/share` to  `~/.local`

## Briefly about GEM-Selektor v.3

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;

* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers:

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.

* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

DISCLAIMER: thermodynamic data bases in test modeling projects provided in this installation are provisional and undocumented. The Development Team accepts no liability of any kind for any losses or inconveniences that may result from use of the data provided in modeling projects for testing and educational purposes. No guarantee is given that the input data in test projects are, in fact, accurate. 

**Enjoy GEM-Selektor!**

(c) 2003-2025 GEMS Development Team

gems2.support@psi.ch

http://gems.web.psi.ch
 

