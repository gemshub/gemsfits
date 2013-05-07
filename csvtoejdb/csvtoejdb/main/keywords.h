#ifndef KEYWORDS_H
#define KEYWORDS_H

//namespace keys
//{
//                    Keyword                                  (level)			Type            Comment
// ---------------------------------------------------------------------------------------------------------------------------------------------------------

static const char *experiments = "experiments"; ///               0          collection         data for experimental samples
// data for experimental samples
static const char *expsample = "sample"; ///                      1		     string 	        ID of this experimental sample
static const char *expdataset = "expdataset"; ///                 1		     string		        ID of set of experimental data
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
    static const char *phspecies = "phspecies"; ///  	          2	         array		        chemical species (end member, phase component)
        static const char *species = "species"; ///               3          string             name of chemical species (end member, phase component)
        static const char *sQnt = "sQnt"; ///	                  3	         float		        measured quantity/concentration of phase species
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
// optimization specific
    static const int bperc = 10; ///                                        integer             default up/lo bound percentage of the parameters

//}
#endif // KEYWORDS_H
