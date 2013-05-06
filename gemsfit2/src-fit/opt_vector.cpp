#include "opt_vector.h"
#include "node.h"
#include "gemsfit_iofiles.h"

opti_vector::opti_vector( )
{
    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();
    int index_species;
    // call GEM_init     --> read in input files
    if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
       {
            cout << gpf->GEMS3LstFilePath().c_str() << endl;
            cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
            exit(1);
       }
    this->h_RDc = false;

    get_gems_fit_DCH_txt( node, this );
    get_gems_fit_DBR_txt( node, this );
    get_gems_fit_multi_txt( node, this );

    if (h_RDc)
    {
        for (int j = 0; j < this->reactions.size(); ++j )
        {
            index_species = node->DC_name_to_xCH( this->reactions[j]->Dc_name.c_str() );
            if( index_species < 0 )
            {
                throw index_species;
            }
            else
            {
                this->reactions[j]->DcIndex = index_species;
            }

            for (int i=0; i<this->reactions[j]->rdc_species.size(); ++i )
            {
            // Get form GEMS the index of to_fit_species of interest
            try
            {
                index_species = node->DC_name_to_xCH( this->reactions[j]->rdc_species[i].c_str() );
                if( index_species < 0 )
                {
                    throw index_species;
                }
                else
                {
                    this->reactions[j]->rdc_species_ind.push_back(index_species);
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

}
