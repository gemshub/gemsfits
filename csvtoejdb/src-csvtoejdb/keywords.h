#ifndef KEYWORDS_H
#define KEYWORDS_H

namespace keys
{
//                    Keyword                                  (level)			Type            Comment
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
//
// General keywords used for qunatity, error and unit
static const char *Qnt = "Q";  ///                                                              qunatity
static const char *Qunit = "unit";  ///                                                        unit
static const char *Qerror = "error";  ///                                                      error

// Database structure
static const char *experiments = "experiments"; ///               0          collection         data for experimental samples
// data for experimental samples
static const char *expsample = "sample"; ///                      1		     string 	        ID of this experimental sample
static const char *usesamples = "usesamples";
static const char *skipsamples = "skipsamples";
static const char *skippair = "skippairs";
static const char *usepair = "usepairs";
static const char *expdataset = "expdataset"; ///                 1		     string		        ID of set of experimental data
static const char *usedatasets = "usedatasets"; ///                 1		     string		        ID of set of experimental data
static const char *skipdatasets = "skipdatasets"; ///               1		     string		        ID of set of experimental data
static const char *usedataset = "usedataset"; ///                 1		     string		        ID of set of experimental data
static const char *skipdataset = "skipdataset"; ///               1		     string		        ID of set of experimental data
static const char *Type = "Type";///
static const char *Weight = "Weight"; ///                         1          double             artificial assigned weight
static const char *sT = "sT"; ///                                 1		     float		        temperature for this experimental sample
static const char *Tunit = "Tunit"; ///   	                      1          string		        units of temperature { 'C' (default) or 'K' or 'F' }
static const char *sP = "sP"; ///                                 1		     float		        pressure for this experimental sample
static const char *Punit = "Punit"; ///		                      1          string		        units of pressure { 'bar' (default) or 'kPa' or 'MPa' or 'GPa' }
static const char *sV = "sV"; /// 		                          1       	 float		        (system) volume for this experimental sample
static const char *Vunit = "Vunit"; ///	                          1		     string		        units of volume { 'cm3' (default) or 'dm3' or 'm3' }

    // Describes bulk composition of the system
static const char *sbcomp = "sbcomp"; ///        		          1       	 array		        defines bulk composition of chemical system for this experiment
    static const char *comp = "comp"; ///                         2 		 string		        formula defining PCO stoichiometry (GEM formula syntax)
///                   *Qnt = "Q";                                 2       	 float		        quantity (to be added to system bulk composition)
///                   *Qerror = "Qerror";   	                  2	         float 		        error (uncertainty) of quantity in the same units
///                   *Qunit = "Qunit"; 	                      2  	     string 		    units of measurement of quantity { 'g' or 'mol' (default) or … }

    // Upper and Lower metastability constraints
static const char *UMC = "UMC"; ///                               1          array              array of Upper metastability constraints
///                     *Qnt = "Q";                               2          double
///                     *IC = "IC";                               2          string             name of dependent component
/// OR
///                     *DC = "DC";                               2          string             name of independent component

static const char *LMC = "LMC"; ///                               1          array              array of Lower metastability constraints
///                     *Qnt = "Q";                               2          double
///                     *IC = "IC";                               2          string             name of dependent component
/// OR
///                     *DC = "DC";                               2          string             name of independent component

    // Describes system phases
static const char *expphases = "expphases"; ///	                  1    	     array		        data for phases characterised (measured) in this experiment
    static const char *phase = "phase"; ///	                      2	         string		        phase ID (name)
        // Phase independent components
    static const char *phIC = "phIC"; ///	                      2   	     array		        array of measured composition of the phase
        static const char *IC = "IC";///                          3          string             name of independent component (element formula, e.g. Al)
    static const char *phMR = "phMR";///                          2          array              array of reported measurments in molar ratio
        static const char *MR = "MR"; ///                         3          string             molar ratio formula
///                       *eQnt = "Q";                            3	         double		        measured quantity/concentration of element (in Qunit)
///                       *Qunit = "Qunit";                       3          string             unit
        // Phase dependent components
    static const char *phDC = "phDC"; ///                         2	         array		        array of dependent components (end member, phase component)
        static const char *DC = "DC"; ///                         3          string             dependent component
        static const char *DCprop = "DCprop"; ///                 3          array              array of dependent components properties (Qnt, etc.)
///                             *property = "property";           4          string             name of the property (Qnt, @coef, etc.)
///                             *Qnt = "Q";                       4       	 float		        quantity
///                             *Qerror = "Qerror";   	          4	         float 		        error
///                             *Qunit = "Qunit"; 	              4  	     string 		    units of measurement of quantity
///
              static const char *actcoef = "@coef"; ///	        ->4	         string		        name of peorperty - activity coef
                        // with Qerror and Qunit
        // Phase properties
    static const char *phprop = "phprop"; ///	                  2   	     array		        known bulk properties of the phase
        static const char *property = "prop"; ///                 3          string             name of property category
        static const char *pH = "pH"; /// 	                    ->3          float		        name of property - pH (for aqueous phase only)
        static const char *Eh = "Eh"; ///	                    ->3	         float		        name of property - Eh (for aqueous phase only)
        static const char *IS = "IS"; ///	                    ->3	         float 		        name of property - Ionic strength (molal)
        static const char *all = "alk"; ///	                    ->3	         float		        name of property - Alkalinity (aqueous phase only)
        static const char *pV = "pV"; ///	                    ->3	         float		        name of property - volume of phase
        static const char *sArea = "sArea"; ///                 ->3 	     float		        name of property - specific surface area of the phase
        static const char *RHO = "RHO"; ///                     ->3          double             name of property - density of the phase
        static const char *Gex = "Gex"; ///                     ->3          double             name of property - excess Gibbs energy of mixing in the phase
        static const char *oscw = "oscw"; ///
        static const char *pe = "pe"; ///

///                       *Qnt = "Q";                             3       	 float		        quantity
///                       *Qerror = "Qerror";   	              3	         float 		        error
///                       *Qunit = "Qunit"; 	                  3  	     string 		    units of measurement of quantity


/// database not implemented yet
static const char *expdatarefs = "expdatarefs"; ///               0     	 collection 	    components (recipe in oxides; e.g. SiO2, H2O, Al2O3, etc.)
//const char *expdataset = "expdataset"; /// 		              1	         string		        ID of experimental dataset e.g. 'Kennedy1950',
static const char *Prange = "Prange"; ///		                  1	         struct   	        pressure range of experiments in a dataset
    static const char *Pmin = "Pmin"; ///	                      2	         float		        minimum pressure in the range
    static const char *Pmax = "Pmax"; ///	                      2	         float		        maximum pressure in the range
static const char *Vrange = "Vrange"; ///	                      1		     struct		        volume range
    static const char *Vmin = "Vmin"; ///	                      2	         float
    static const char *Vmax = "Vmax"; ///	                      2	         float
static const char *Trange = "Trange"; ///	                      1		     struct		        temparature range
    static const char *Tmin = "Tmin"; ///	                      2	         float
    static const char *Tmax = "Tmax"; ///	                      2	         float
static const char *reference = "reference"; ///	                  1	         string		        bib.reference(s) for the experimental dataset
static const char *comment	= "comment"; ///	                  1	         string		        comment
static const char *file = "file"; ///	                          1		     string 		    file name(s), e.g. image, word,
static const char *url = "url"; ///	                              1		     string 		    URL to source of experimental data on web
//----------------------------------------------------------------------------------------------------------------------------------------------------------

// general keywords for error and unit present in the header of the input csv file
    static const char *_error = "error";
    static const char *_unit = "unit";

// kewords for adjusted parameters marked in the input file
    static const char *IV = "IV"; ///                                       double              initial value
    static const char *UB = "UB"; ///                                       double              upper boundary
    static const char *LB = "LB"; ///                                       double              lower boundary
    // Reaction specific
    static const char *Ref = "Ref"; ///                                     string              reference for logK
    static const char *logK = "logK"; ///                                   double              reaction constant
    static const char *rDC = "rDC";///                                      string              rection dependent component
    static const char *nC = "nC"; ///                                       integer             number of components involved in the reaction
                        // species
    static const char *RC = "RC"; ///                                       array               array of components names
    static const char *Rcoef = "Rcoef"; ///                                 array               array of reaction coeficients
    static const char *order ="order"; ///                                  integer             order of the reactions
    // Linked parameters
    static const char *LEs = "LEs"; ///                                     array               array of linked elements
    static const char *LE = "LE"; ///                                       string              linked element
    static const char *Lcoef = "Lcoef"; ///                                 array               array of linking coeficients
// optimization specific
    static const int bperc = 10; ///                                        integer             default up/lo bound percentage of the parameters

// keywords DatTarget
    static const char *Target = "Target"; ///                               string              name of target function
    static const char *TT = "TT"; ///                                       string              target function type
    static const char *WT = "WT"; ///                                       string              weight type
    static const char *OFUN = "OFUN"; ///                                   array               Objective function
    static const char *NFUN = "NFUN"; ///
        static const char *CT = "CT"; ///                                   string              type of component/data to compare
//            static const char *property = "prop"; ///                           string              keyword for comparing properties with names such as pH, Q, IS, etc.
        static const char *CN = "CN"; ///                                   string              name of data to compare e.g. Al, pH, etc.
        static const char *EPH = "EPH"; ///                                 string              experiment phase
//        static const char *Eunit = "Unit"; ///                              string              experiment element unit
        static const char *DCP = "DCP"; ///                                 string              dependent comp property
        static const char *Ptype = "Ptype"; ///
        static const char *Tforumla ="Tformula"; ///                        string              name of titrants
        static const char *Telem ="Telem"; ///                              string              name of elements



// Name of Target Functions and weights
static const char *lsq = "lsq"; ///                                         string              least square function (measured-computed)^2
static const char *lsq_norm = "lsq_norm"; ///                               string              normalized to experimental average, least square
static const char *abs_dif = "abs_dif";///                                  string              form nested optimization use abs_dif
static const char *inverr = "inverr"; ///                                   string              inverse weight as 1/error
static const char *inverr2 = "inverr2"; ///                                 string              inverse weight as 1/error^2
static const char *inverr3 = "inverr3"; ///                                 string              inverse weight as 1/measured^2
static const char *inverr_norm = "inverr_norm"; ///                         string              normalized <Xmeas>/error

// Kaywords for units
static const char *molal = "molal"; ///                                     string              molality moles/kg
static const char *loga = "loga"; ///                                       string              log(activity)
static const char *logm = "logm"; ///                                       string              log(molality)
static const char *_loga = "-loga"; ///                                     string              -log(consntration) used for pH
static const char *gram = "g"; ///                                          string              mass, grams
static const char *kgram = "kg"; ///                                        string              mass, kg
static const char *mole = "mole";  ///                                      string              amount, moles
static const char *m3 = "m3"; ///                                           string              volume, m3
static const char *cm3 = "cm3"; ///                                         string              volume, cm3
static const char *Simolfrac = "simolfrac"; ///                             string              mole ratio to 1 mole of Si
static const char *molratio = "molratio"; ///                               string              mole ratio
static const char *molfrac = "molfrac"; ///                                 string              mole fraction
static const char *g_cm3 = "g/cm3"; ///                                     string              density, g/cm3
static const char *Volts = "Volts"; ///                                     string              Eh, V
static const char *J_mol = "J/mol"; ///                                     string              energy per mole, J/mol
static const char *kJ_mol = "kJ/mol"; ///                                   string              energy per mole, kJ/mol


static const char *aqueous = "aq_gen"; ///
}
#endif // KEYWORDS_H
