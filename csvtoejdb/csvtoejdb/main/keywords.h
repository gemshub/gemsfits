#ifndef KEYWORDS_H
#define KEYWORDS_H

namespace keys
{
//                    Keyword                                  (level)			Type            Comment
// ---------------------------------------------------------------------------------------------------------------------------------------------------------

static const char *experiments = "experiments"; ///               0          collection         data for experimental samples
// data for experimental samples
static const char *expsample = "sample"; ///                      1		     string 	        ID of this experimental sample
static const char *expdataset = "expdataset"; ///                 1		     string		        ID of set of experimental data
static const char *Weight = "Weight"; ///                         1          double             artificial assigned weight
static const char *sT = "sT"; ///                                 1		     float		        temperature for this experimental sample
static const char *Tunit = "Tunit"; ///   	                      1          string		        units of temperature { 'C' (default) or 'K' or 'F' }
static const char *sP = "sP"; ///                                 1		     float		        pressure for this experimental sample
static const char *Punit = "Punit"; ///		                      1          string		        units of pressure { 'bar' (default) or 'kPa' or 'MPa' or 'GPa' }
static const char *sV = "sV"; /// 		                          1       	 float		        (system) volume for this experimental sample
static const char *Vunit = "Vunit"; ///	                          1		     string		        units of volume { 'cm3' (default) or 'dm3' or 'm3' }
static const char *sbcomp = "sbcomp"; ///        		          1       	 array		        defines bulk composition of chemical system for this experiment
    static const char *comp = "comp"; ///                         2 		 string		        formula defining PCO stoichiometry (GEM formula syntax)
    static const char *bQnt = "bQnt"; ///	                      2       	 float		        quantity (to be added to system bulk composition)
    static const char *Qerror = "Qerror"; ///	                  2	         float 		        error (uncertainty) of quantity in the same units
    static const char *Qunit = "Qunit"; ///	                      2  	     string 		    units of measurement of quantity { 'g' or 'mol' (default) or … }

static const char *expphases = "expphases"; ///	                  1    	     array		        data for phases characterised (measured) in this experiment
    static const char *phase = "phase"; ///	                      2	         string		        phase ID (name)
    static const char *phcomp = "phcomp"; ///	                  2   	     array		        measured composition of the phase
        static const char *element = "element"; ///	              3	         string 		    name of chemical element (e.g. 'Al')
        static const char *eQnt = "eQnt"; ///	                  3	         float		        measured quantity/concentration of element (in Qunit)
    static const char *phspecies = "phdcomps"; ///  	          2	         array		        chemical species (end member, phase component)
        static const char *dcompprop = "dcompprop"; ///
        static const char *species = "species";               /// 3          string             name of chemical species (end member, phase component)
        static const char *dcomp = "dcomp"; ///
        static const char *actcoef = "@coef"; ///	              3	         float		        measured quantity/concentration of phase species
                        // with Qerror and Qunit
    static const char *phprop = "phprop"; ///	                  2   	     array		        known bulk properties of the phase
        static const char *property = "property"; ///
        static const char *pH = "pH"; /// 	                      3          float		        pH (for aqueous phase only)
        static const char *Eh = "Eh"; ///	                      3	         float		        Eh (for aqueous phase only)
        static const char *IS = "IS"; ///	                      3	         float 		        Ionic strength (molal)
        static const char *all = "all"; ///	                      3	         float		        Alkalinity (aqueous phase only)
        static const char *pV = "pV"; ///	                      3	         float		        volume of phase
        static const char *pQnt = "pQnt"; ///	                  3	         float		        Quantity of this phase in the sample system
        static const char *sArea = "sArea"; ///                   3 	     float		        specific surface area of the phase
        static const char *RHO = "RHO"; ///                       3          double             density of the phase

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
static const char *comment	= "commnet"; ///	                  1	         string		        comment
static const char *file = "file"; ///	                          1		     string 		    file name(s), e.g. image, word,
static const char *url = "url"; ///	                              1		     string 		    URL to source of experimental data on web
//----------------------------------------------------------------------------------------------------------------------------------------------------------

// general keywords for error and unit present in the header of the input file
    static const char *_error = "error";
    static const char *_unit = "unit";

// kewords for adjusted parameters marked in the input file
    static const char *IV = "IV"; ///                                       double              initial value
    static const char *UB = "UB"; ///                                       double              upper boundary
    static const char *LB = "LB"; ///                                       double              lower boundary
    // Reaction specific
    static const char *Ref = "Ref"; ///                                     string              reference for logK
    static const char *logK = "logK"; ///                                   double              reaction constant
    static const char *nC = "nC"; ///                                       integer             number of components involved in the reaction
                        // species
    static const char *RC = "RC"; ///                                       array               array of components names
    static const char *Rcoef = "Rcoef"; ///                                 array               array of reaction coeficients
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
        static const char *EPH = "EPH"; ///                                 string              experiment phase
        static const char *EN = "EN"; ///                                   string              experiment phase element
        static const char *PPH = "PPH"; ///                                 string              experiment pahse (for phase property)
        static const char *EP = "EP"; ///                                   string              experiment pahse property
        static const char *Eunit = "Eunit"; ///                             string              experiment element unit
        static const char *PEunit = "PEunit"; ///                           string              experiment property unit
        static const char *DCPH = "DCPH"; ///                               string              experiment dependent comp phase
        static const char *DC = "DC"; ///                                   string              dependent component
        static const char *DCP = "DCP"; ///                                 string              dependent comp property
        static const char *DCPunit = "DCPunit"; ///                         string              dependent comp property unit



// Name of Target Functions and weights
static const char *lsq = "lsq"; ///                                         string              least square function (measured-computed)^2
static const char *lsq_norm = "lsq_norm"; ///                               string              normalized to experimental average least square
static const char *inverr = "inverr"; ///                                   string              inverse weight as 1/error
static const char *inverr2 = "inverr2"; ///                                 string              inverse weight as 1/error^2
static const char *inverr3 = "inverr3"; ///                                 string              inverse weight as 1/measured^2
static const char *inverr_norm = "inverr_norm"; ///                         string              normalized <Xmeas>/error

// Kaywords for units
static const char *molal = "molal"; ///                                     string              molality moles/kg
static const char *loga = "loga"; ///                                       string              log(molality)
static const char *_loga = "-loga"; ///                                     string              -log(consntration) used for pH
static const char *gram = "g"; ///
static const char *kgram = "kg"; ///
static const char *m3 = "m3"; ///
static const char *cm3 = "cm3"; ///
static const char *Simolfrac = "Simolfrac"; ///
static const char *molfrac = "molfrac"; ///
static const char *g_cm3 = "g/cm3"; ///
}
#endif // KEYWORDS_H
