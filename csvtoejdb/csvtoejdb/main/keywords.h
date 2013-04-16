#ifndef KEYWORDS_H
#define KEYWORDS_H


//             Keyword                                  (level)			Type            Comment
// ---------------------------------------------------------------------------------------------------------------------------------------------------------

const char *experiments = "experiments"; ///               0          collection        data for experimental samples
// data for experimental samples
const char *expsample = "sample"; ///                      1		  string 	        ID of this experimental sample
const char *expdataset = "expdataset"; ///                 1		  string		    ID of set of experimental data
const char *sT = "sT"; ///                                 1		  float		        temperature for this experimental sample
const char *Tunit = "Tunit"; ///   	                       1 		  string		    units of temperature { 'C' (default) or 'K' or 'F' }
const char *sP = "sP"; ///                                 1		  float		        pressure for this experimental sample
const char *Punit = "Punit"; ///		                   1       	  string		    units of pressure { 'bar' (default) or 'kPa' or 'MPa' or 'GPa' }
const char *sV = "sV"; /// 		                           1       	  float		        (system) volume for this experimental sample
const char *Vunit = "Vunit"; ///	                       1		  string		    units of volume { 'cm3' (default) or 'dm3' or 'm3' }
const char *sbcomp = "sbcomp"; ///        		           1       	  array		        defines bulk composition of chemical system for this experiment
    const char *comp = "comp"; ///                         2 		  string		    formula defining PCO stoichiometry (GEM formula syntax)
    const char *bQnt = "bQnt"; ///	                       2       	  float		        quantity (to be added to system bulk composition)
    const char *Qerror = "Qerror"; ///	                   2	      float 		    error (uncertainty) of quantity in the same units
    const char *Qunit = "Qunit"; ///	                   2  	      string 		    units of measurement of quantity { 'g' or 'mol' (default) or … }

const char *expphases = "expphases"; ///	               1    	  array		        data for phases characterised (measured) in this experiment
    const char *phase = "phase"; ///	                   2	      string		    phase ID (name)
    const char *phcomp = "phcomp"; ///	                   2   	      array		        measured composition of the phase
        const char *element = "element"; ///	           3	      string 		    name of chemical element (e.g. 'Al')
        const char *eQnt = "eQnt"; ///	                   3	      float		        measured quantity/concentration of element (in Qunit)
    const char *phspecies = "phspecies"; ///  	           2	      array		        chemical species (end member, phase component)
        const char *species = "species"; ///                 3          string            name of chemical species (end member, phase component)
        const char *sQnt = "sQnt"; ///	                   3	      float		        measured quantity/concentration of phase species
                        // with Qerror and Qunit
    const char *phprop = "phprop"; ///	                   2   	      array		        known bulk properties of the phase
        const char *pH = "pH"; /// 	                       3          float		        pH (for aqueous phase only)
        const char *Eh = "Eh"; ///	                       3	      float		        Eh (for aqueous phase only)
        const char *IS = "IS"; ///	                       3	      float 		    Ionic strength (molal)
        const char *all = "all"; ///	                   3	      float		        Alkalinity (aqueous phase only)
        const char *pV = "pV"; ///	                       3	      float		        volume of phase
        const char *pQnt = "pQnt"; ///	                   3	      float		        Quantity of this phase in the sample system
        const char *sArea = "sArea"; ///                   3 	      float		        specific surface area of the phase

const char *expdatarefs = "expdatarefs"; ///               0     	  collection 	    components (recipe in oxides; e.g. SiO2, H2O, Al2O3, etc.)
//const char *expdataset = "expdataset"; /// 		       1	      string		    ID of experimental dataset e.g. 'Kennedy1950',
const char *Prange = "Prange"; ///		                   1	      struct   	        pressure range of experiments in a dataset
    const char *Pmin = "Pmin"; ///	                       2	      float		        minimum pressure in the range
    const char *Pmax = "Pmax"; ///	                       2	      float		        maximum pressure in the range
const char *Vrange = "Vrange"; ///	                       1		  struct		    volume range
    const char *Vmin = "Vmin"; ///	                       2	      float
    const char *Vmax = "Vmax"; ///	                       2	      float
const char *Trange = "Trange"; ///	                       1		  struct		    temparature range
    const char *Tmin = "Tmin"; ///	                       2	      float
    const char *Tmax = "Tmax"; ///	                       2	      float
const char *reference = "reference"; ///	               1	      string		    bib.reference(s) for the experimental dataset
const char *comment	= "commnet"; ///	                   1	      string		    comment
const char *file = "file"; ///	                           1		  string 		    file name(s), e.g. image, word,
const char *url = "url"; ///	                           1		  string 		    URL to source of experimental data on web
//----------------------------------------------------------------------------------------------------------------------------------------------------------

// general keywords for error and unit present in the header of the input file
    const char *_error = "error";
    const char *_unit = "unit";


#endif // KEYWORDS_H
