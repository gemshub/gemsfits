#include "opt_vector.h"
#include "gemsfit_iofiles.h"

opti_vector::opti_vector( )
{
    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();
    // call GEM_init     --> read in input files
    if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
       {
            cout << gpf->GEMS3LstFilePath().c_str() << endl;
            cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
            exit(1);
       }
    this->h_RDc = false;

    get_gems_fit_DCH_txt( node, this ); // reading DCH parameters
    get_gems_fit_DBR_txt( node, this ); // reading DBR parameters
    get_gems_fit_multi_txt( node, this ); // reading multi parameters

    // getting indexes of components in the DCH

    if (h_RDc)
    {
        get_RDc_indexes (node, this);
    }

}


void opti_vector::get_RDc_indexes (TNode *node, opti_vector *ov)
{
    int index_species;
    for (int j = 0; j < ov->reactions.size(); ++j )
    {
        index_species = node->DC_name_to_xCH( ov->reactions[j]->Dc_name.c_str() );
        if( index_species < 0 )
        {
            throw index_species;
        }
        else
        {
            ov->reactions[j]->DcIndex = index_species;
        }

        for (int i=0; i<ov->reactions[j]->rdc_species.size(); ++i )
        {
            // Get form GEMS the index of to_fit_species of interest
            try
            {
                index_species = node->DC_name_to_xCH( ov->reactions[j]->rdc_species[i].c_str() );
                if( index_species < 0 )
                {
                    throw index_species;
                }
                else
                {
                    ov->reactions[j]->rdc_species_ind.push_back(index_species);
                }
            }
            catch( long e )
            {
                cout<<endl;
                cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
                cout<<" Can not proceed ... Bailing out now ... "<<endl;
                cout<<endl;
                exit(1);
            }
        }
    }
}
