//--------------------------------------------------------------------
// $Id: gemsfit_task.cpp 789 2012-12-19 17:08:55Z kulik $
//
/// \file gemsfit_task.cpp
/// Implementation of TGfitTask class functionality - advanced
/// interface between GEM IPM and GEMSFIT node array
/// working with one DATACH structure and arrays of DATABR structures
//
// Copyright (C) 2013 G.D.Miron, D.Kulik, S.V.Dmytrieva
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

#include "gemsfit_task.h"
#include "io_arrays.h"
#include "gdatastream.h"
#include "gemsfit_iofiles.h"
#include "keywords.h"
#include "gemsfit_global_functions.h"
//#include "gemsfit_nested_functions.h"
#include <cmath>
#include "gemsfit_target_functions.h"
#include <iomanip>


#ifndef __unix
#include <io.h>
#endif


using namespace std;

// Main function that calculates the residual
double TGfitTask::get_residual(int exp, TGfitTask::TargetFunction::obj_fun &objfun, int &count)
{
    double residual = 0.0;
    count = 0;

    // loop trough objective function
    /// Target function
    if ((objfun.exp_phase !="NULL") && (this->experiments[exp]->expphases.size() > 0))
    {
        // loop trough all phases
        for (unsigned int p=0; p<this->experiments[exp]->expphases.size(); ++p)
        {
            if ((objfun.exp_CT == keys::IC) /*&& (objfun->exp_property =="NULL")*/)
            {
                // loop trough all elements
                for (unsigned int e=0; e<this->experiments[exp]->expphases[p]->phIC.size(); ++e)
                {
                    if ((this->experiments[exp]->expphases[p]->phIC[e]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phIC[e]->Qunit;
                        check_unit(exp, p, e, objfun.exp_unit, this );
                        residual =  residual_phase_elem (exp, p, e, objfun, this);
                        count++;
                    }
                }
            } else
                if ((objfun.exp_CT == keys::MR) /*&& (objfun->exp_property =="NULL")*/)
                {
                    // loop trough all elements
                    for (unsigned int f=0; f<this->experiments[exp]->expphases[p]->phMR.size(); ++f)
                    {
                        if ((this->experiments[exp]->expphases[p]->phMR[f]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {
                            // check for unit
//                                    check_unit(i, p, e, objfun->exp_unit, this );
                            residual = residual_phase_elemMR (exp, p, f, objfun, this);
                            count++;
                        }
                    }
                } else
                if ((objfun.exp_CT == keys::property) && (this->experiments[exp]->expphases[p]->phprop.size() > 0) /*&& (objfun->exp_dcomp == "NULL")*/)
                {
                // loop trough all properties
                for (unsigned int pp = 0; pp< this->experiments[exp]->expphases[p]->phprop.size(); ++pp)
                {
                    if ((this->experiments[exp]->expphases[p]->phprop[pp]->property == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phprop[pp]->Qunit;
                        check_prop_unit(exp, p, pp, objfun.exp_unit, this );
                        residual =  residual_phase_prop (exp, p, pp, objfun, this);
                        count++;
                    }
                }
            } else
                if (/*(objfun->exp_property !="NULL") &&*/ (this->experiments[exp]->expphases[p]->phDC.size() > 0) && (objfun.exp_CT == keys::DC))
                {
                    // loop trough all dependent components
                    for (unsigned int dc = 0; dc< this->experiments[exp]->expphases[p]->phDC.size(); ++dc)
                    {
                        if ((this->experiments[exp]->expphases[p]->phDC[dc]->DC == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {
                            // loop trough all dep comp properties
                            for (unsigned int dcp = 0; dcp < this->experiments[exp]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                            {
                                if (this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->property == objfun.exp_DCP)
                                {
                                    // check for unit
                                    if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit;
                                    check_unit_dcomp(exp, p, dc,dcp, objfun.exp_unit, this );
                                    residual = residual_phase_dcomp (exp, p, dc, dcp, objfun, this);
                                    count++;
                                }
                            }
                        }
                    }
                }
        }
    }

    return residual;
}

// function for counting the total numbers of residuals that will be calculated
int TGfitTask::get_number_of_residuals( )
{
    // loop trough objective function
    int count = 0, nr_res=0;
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    /// Target function
    // Loop trough all experiments
        for (unsigned int i=0; i<this->experiments.size(); ++i)
        {
            get_residual(i, aTfun[i].objfun[j],count);
            nr_res +=count;
        }
    }
    return nr_res;
}

double TGfitTask::get_sum_of_residuals( )
{
    // loop trough objective function
    // think about pararelizing it
    double residual = 0.0;
    int count = 0;

    // Loop trough target function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    // Loop trough all experiments
        for (unsigned int i=0; i<this->experiments.size(); ++i)
        {
           residual = residual + get_residual(i, aTfun[i].objfun[j],count);
        }
    }
    return residual;
}

void TGfitTask::set_average_objfun ()
{
    double average = 0.0;
    int count = 0;
    minimum_value = 1e9;

    for (unsigned int j = 0; j<Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i <aTfun.size(); i++)
        {
            if (aTfun[i].objfun[j].isComputed)
            {
                average += aTfun[i].objfun[j].results.measured_value;
                if (aTfun[i].objfun[j].results.measured_value < minimum_value)
                    minimum_value = aTfun[i].objfun[j].results.measured_value;
                count++;
            }
        }
        average = average / count;
        Tfun->objfun[j].meas_average = average;
        average = 0.0;
        count = 0;
    }

//#ifdef useomp
//    omp_set_num_threads(this->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int j = 0; j<Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i <aTfun.size(); i++)
        {
            if (aTfun[i].objfun[j].isComputed)
            {
                aTfun[i].objfun[j].meas_average = Tfun->objfun[j].meas_average;
            }
        }
    }
}


void TGfitTask::add_MC_scatter( vector<double> scatter)
{
    int count = 0;
    // loop trough objective function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    /// Target function
    // Loop trough all experiments
        for (unsigned int i=0; i<this->experiments.size(); ++i)
        {
            if ((this->Tfun->objfun[j].exp_phase !="NULL") && (this->experiments[i]->expphases.size() > 0))
            {
                // loop trough all phases
                for (unsigned int p=0; p<this->experiments[i]->expphases.size(); ++p)
                {
                    if ((Tfun->objfun[j].exp_CT == keys::IC) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                    {
                        // loop trough all elements
                        for (unsigned int e=0; e<this->experiments[i]->expphases[p]->phIC.size(); ++e)
                        {
                            if ((this->experiments[i]->expphases[p]->phIC[e]->comp == this->Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                            {
                                this->experiments[i]->expphases[p]->phIC[e]->Qnt = scatter[count];

                                ++count;
                            }
                        }
                    } else
                        if ((Tfun->objfun[j].exp_CT == keys::MR) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                        {
                            // loop trough all elements
                            for (unsigned int f=0; f<this->experiments[i]->expphases[p]->phMR.size(); ++f)
                            {
                                if ((this->experiments[i]->expphases[p]->phMR[f]->comp == this->Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                                {
                                    this->experiments[i]->expphases[p]->phMR[f]->Qnt = scatter[count];

                                    ++count;
                                }
                            }
                        } else

                        if ((Tfun->objfun[j].exp_CT == keys::property) && (this->experiments[i]->expphases[p]->phprop.size() > 0) /*&& (this->Tfun->objfun[j]->exp_dcomp == "NULL")*/)
                        {
                        // loop trough all properties
                        for (unsigned int pp = 0; pp< this->experiments[i]->expphases[p]->phprop.size(); ++pp)
                        {
                            if ((this->experiments[i]->expphases[p]->phprop[pp]->property == Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                            {
                                this->experiments[i]->expphases[p]->phprop[pp]->Qnt = scatter[count];

                                ++count;
                            }
                        }
                    } else
                        if (/*(Tfun->objfun[j]->exp_property !="NULL") &&*/ (this->experiments[i]->expphases[p]->phDC.size() > 0) && (Tfun->objfun[j].exp_CT == keys::DC))
                        {
                            // loop trough all dependent components
                            for (unsigned int dc = 0; dc< this->experiments[i]->expphases[p]->phDC.size(); ++dc)
                            {
                                if ((this->experiments[i]->expphases[p]->phDC[dc]->DC == Tfun->objfun[j].exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j].exp_phase ))
                                {
                                    // loop trough all dep comp properties
                                    for (unsigned int dcp = 0; dcp < this->experiments[i]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                                    {
                                        if (this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->property == Tfun->objfun[j].exp_DCP)
                                        {
                                            this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = scatter[count];

                                            ++count;
                                        }
                                    }
                                }
                            }
                        }
                }
            }
        }
    }
}

void TGfitTask::get_addout_calc (int exp, TGfitTask::TargetFunction::obj_fun &addout )
{
    // loop trough objective function
    /// Target function
    if ((addout.exp_phase !="NULL") &&  (addout.exp_CT == keys::IC) /*&& (objfun->exp_property =="NULL")*/)
    {
       residual_phase_elem (exp, -1, -1,  addout, this);
    }

    if ((addout.exp_CT == keys::MR) /*&& (objfun->exp_property =="NULL")*/)
    {
       residual_phase_elemMR (exp,  -1, -1, addout, this);
    }

    if (addout.exp_CT == keys::property)
    {
       residual_phase_prop (exp,  -1,-1, addout, this);
    }

    if (addout.exp_CT == keys::DC)
    {
       residual_phase_dcomp (exp, -1, -1, -1, addout, this);
    }
}


void TGfitTask::set_weights ()
{
    // For onjective functions
//#ifdef useomp
//    omp_set_num_threads(this->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned i = 0; i<aTfun.size(); i++)
    {
        for (unsigned j = 0; j < aTfun[i].objfun.size(); j++)
        {
            if (Opti->OptUserWeight)
                aTfun[i].objfun[j].weight = aTfun[i].objfun[j].weight * experiments[i]->weight;
            else
                aTfun[i].objfun[j].weight = aTfun[i].objfun[j].weight;

        }

    }

    // For nested functions
//#ifdef useomp
//    omp_set_num_threads(this->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned i = 0; i<aTfun.size(); i++)
    {
        for (unsigned j = 0; j < aTfun[i].nestfun.size(); j++)
        {
            if (Opti->OptUserWeight)
                aTfun[i].nestfun[j].weight = aTfun[i].nestfun[j].weight * experiments[i]->weight;
            else
                aTfun[i].nestfun[j].weight = aTfun[i].nestfun[j].weight;
        }

    }
}


void TGfitTask::set_results ( TGfitTask::TargetFunction::obj_fun &objfun, double computed, double measured, double Weighted_Tfun_residual, double Tfun_residual, double weight )
{
    objfun.results.computed_value = computed;
    objfun.results.measured_value = measured;
    objfun.results.residual = measured - computed;
    objfun.results.weight = weight;
    objfun.results.Tfun_residual = Tfun_residual;
    objfun.results.WTfun_residual = Weighted_Tfun_residual;
}

void TGfitTask:: print_global_results ()
{
//    gpf->fres << "experiment,,,unit,measured,computed,residual," <<"residual_"+Tfun->type <<  endl;
    // GEMSFIT results file for all test runs. Keeps a log of all runs. The file has to be deleted manually.
    string path_ = gpf->FITFile();
    gpf->fres.open(path_.c_str(), ios::trunc);
    if( gpf->fres.fail() )
    { cout<<"Results fileopen error"<<endl; exit(1); }

    int prec = 12;
    setprecision(prec);
    scientific(gpf->fres);
    gpf->fres.setf(ios::fixed);

    vector<string> header;
    header.push_back(keys::expsample);
    header.push_back(keys::expdataset);
    header.push_back(keys::sT);
    header.push_back(keys::sP);

//    gpf->fres << " <" << this->param_file <<"> <" << this->anNodes<<" samples> " <<endl;

    for (unsigned int i = 0; i <aTfun.size(); i++)
    {
        for (unsigned j = 0; j <Tfun->addout.size(); j++)
        {
            if (Tfun->addout[j].Otype == keys::meas)
                // before get_residual function was used
            get_addout_meas(i, aTfun[i].addout[j] );
        }
    }

    for (unsigned int i = 0; i <aTfun.size(); i++)
    {
        for (unsigned j = 0; j <Tfun->addout.size(); j++)
        {
            if (Tfun->addout[j].Otype == keys::calc)
            get_addout_calc(i, aTfun[i].addout[j] );
        }
    }


    for (unsigned i = 0; i < Tfun->objfun.size(); i++)
    {
        string temp ="";
        temp += Tfun->objfun[i].exp_phase + "." + Tfun->objfun[i].exp_CN ;
        header.push_back(temp + "." + keys::meas);
        header.push_back(temp + "." + keys::calc);
        header.push_back("residual");
//        header.push_back("residual."+ Tfun->type);
    }

    for (unsigned i = 0; i < Tfun->addout.size(); i++)
    {
        string temp ="";
        temp += Tfun->addout[i].exp_phase + "." + Tfun->addout[i].exp_CN;
        if (Tfun->addout[i].Otype == keys::calc)
        header.push_back(temp + "." + keys::calc);
        if (Tfun->addout[i].Otype == keys::meas)
        header.push_back(temp + "." + keys::meas);

    }

    for (unsigned i = 0; i<header.size(); i++)
    {
        gpf->fres << header[i] << ",";
    }

    gpf->fres << endl;

    for (unsigned i = 0; i<aTfun.size(); i++)
    {
        gpf->fres << experiments[i]->sample <<",";
        gpf->fres << experiments[i]->expdataset <<",";
        gpf->fres << experiments[i]->sT <<",";
        gpf->fres << experiments[i]->sP <<",";
        for (unsigned j=0; j < Tfun->objfun.size(); j++)
        {
            if (aTfun[i].objfun[j].isComputed)
            {
                gpf->fres << setprecision(prec) << aTfun[i].objfun[j].results.measured_value <<","<<
                             setprecision(prec) << aTfun[i].objfun[j].results.computed_value << ","<<
                             setprecision(prec) << aTfun[i].objfun[j].results.residual<<","/*<< aTfun[i].objfun[j].results.WTfun_residual << ","*/;
            } else
                gpf->fres << ","<< ","<<","/*<< ","*/;

        }

        for (unsigned j=0; j < Tfun->addout.size(); j++)
        {
            if (aTfun[i].addout[j].isComputed)
            {
                if (aTfun[i].addout[j].Otype == keys::calc)
                gpf->fres << setprecision(prec) << aTfun[i].addout[j].results.computed_value <<",";
                if (aTfun[i].addout[j].Otype == keys::meas)
                gpf->fres << setprecision(prec) << aTfun[i].addout[j].results.measured_value <<",";
            } else
                gpf->fres << ",";
        }

        gpf->fres << endl;
    }

    gpf->fres.close();

}

void TGfitTask:: print_nested_results ()
{
    gpf->fnfres.open(gpf->FITnfunFile().c_str(), ios::trunc);
    if( gpf->fnfres.fail() )
    { cout<<"Nested Results fileopen error"<<endl; exit(1); }

    gpf->fnfres << "sample,phase,name,unit,measured,computed,residual";

    for (unsigned int i= 0; i<Opti->nest_optv.Pindex.size(); i++)
    {
        if (Opti->nest_optv.Ptype[i] == "bIC")
        gpf->fnfres << "," << NodT[0]->xCH_to_IC_name(Opti->nest_optv.Pindex[i]);
        if (Opti->nest_optv.Ptype[i] == "TK")
        gpf->fnfres << "," << "TKelvin";
        if (Opti->nest_optv.Ptype[i] == "P")
        gpf->fnfres << "," << "Pbar";
    }

    if (Opti->nest_optv.Lparams.size() > 0)
        for (unsigned int i = 0; i<Opti->nest_optv.Lparams.size(); i++)
        {
            gpf->fnfres << "," << NodT[0]->xCH_to_IC_name(Opti->nest_optv.Lparams[i]->index);
        }

    gpf->fnfres << endl;


    setprecision(12);
    scientific(gpf->fnfres);


    gpf->fnfres.setf(ios::fixed);

    for (unsigned int i=0; i<aTfun.size(); i++)
    {
        for (unsigned int j = 0; j <aTfun[i].nestfun.size(); j++)
        {
            if (aTfun[i].nestfun[j].isComputed)
            {
                gpf->fnfres << experiments[i]->sample <<","<< aTfun[i].nestfun[j].exp_phase <<","<< aTfun[i].nestfun[j].exp_CN <<","<< aTfun[i].nestfun[j].exp_unit <<","<<
                             aTfun[i].nestfun[j].results.measured_value <<","<< aTfun[i].nestfun[j].results.computed_value << ","<< aTfun[i].nestfun[j].results.residual;

                for (unsigned int p= 0; p<Opti->nest_optv.Pindex.size(); p++)
                {
                    gpf->fnfres << "," << Opti->nest_optv.e_opt[p]->val[i] ;
                }

                if (Opti->nest_optv.Lparams.size() > 0)
                    for (unsigned int l = 0; l<Opti->nest_optv.Lparams.size(); l++)
                    {
                        gpf->fnfres << "," << Opti->nest_optv.Lparams[l]->e_val[i];
                    }
                gpf->fnfres << endl;
            }
        }
    }

    gpf->fnfres.close();

}


void TGfitTask::get_addout_meas(int exp, TGfitTask::TargetFunction::obj_fun &objfun )
{
    // loop trough objective function
    /// Target function
    if ((objfun.exp_phase !="NULL") && (this->experiments[exp]->expphases.size() > 0))
    {
        // loop trough all phases
        for (unsigned int p=0; p<this->experiments[exp]->expphases.size(); ++p)
        {
            if ((objfun.exp_CT == keys::IC) /*&& (objfun->exp_property =="NULL")*/)
            {
                // loop trough all elements
                for (unsigned int e=0; e<this->experiments[exp]->expphases[p]->phIC.size(); ++e)
                {
                    if ((this->experiments[exp]->expphases[p]->phIC[e]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phIC[e]->Qunit;
                        objfun.results.measured_value = this->experiments[exp]->expphases[p]->phIC[e]->Qnt;
                        objfun.isComputed = true;
                        ///
                    }
                }
            } else
                if ((objfun.exp_CT == keys::MR) /*&& (objfun->exp_property =="NULL")*/)
                {
                    // loop trough all elements
                    for (unsigned int f=0; f<this->experiments[exp]->expphases[p]->phMR.size(); ++f)
                    {
                        if ((this->experiments[exp]->expphases[p]->phMR[f]->comp == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {

                            objfun.results.measured_value = this->experiments[exp]->expphases[p]->phMR[f]->Qnt;
                            objfun.isComputed = true;
                            ///

                        }
                    }
                } else
                if ((objfun.exp_CT == keys::property) && (this->experiments[exp]->expphases[p]->phprop.size() > 0) /*&& (objfun->exp_dcomp == "NULL")*/)
                {
                // loop trough all properties
                for (unsigned int pp = 0; pp< this->experiments[exp]->expphases[p]->phprop.size(); ++pp)
                {
                    if ((this->experiments[exp]->expphases[p]->phprop[pp]->property == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                    {
                        // check for unit
                        if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phprop[pp]->Qunit;
                        objfun.results.measured_value = this->experiments[exp]->expphases[p]->phprop[pp]->Qnt;
                        objfun.isComputed = true;
                        ///
                    }
                }
            } else
                if (/*(objfun->exp_property !="NULL") &&*/ (this->experiments[exp]->expphases[p]->phDC.size() > 0) && (objfun.exp_CT == keys::DC))
                {
                    // loop trough all dependent components
                    for (unsigned int dc = 0; dc< this->experiments[exp]->expphases[p]->phDC.size(); ++dc)
                    {
                        if ((this->experiments[exp]->expphases[p]->phDC[dc]->DC == objfun.exp_CN) && (this->experiments[exp]->expphases[p]->phase == objfun.exp_phase ))
                        {
                            // loop trough all dep comp properties
                            for (unsigned int dcp = 0; dcp < this->experiments[exp]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                            {
                                if (this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->property == objfun.exp_DCP)
                                {
                                    // check for unit
                                    if (objfun.exp_unit == "NULL") objfun.exp_unit = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit;
                                    objfun.results.measured_value = this->experiments[exp]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                                    objfun.isComputed = true;
                                    ///
                                }
                            }
                        }
                    }
                }
        }
    }

}
