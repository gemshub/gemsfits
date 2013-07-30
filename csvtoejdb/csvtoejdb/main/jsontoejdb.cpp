# include "jsontoejdb.h"
# include "keywords.h"
# include <boost/lexical_cast.hpp>

using namespace keys;

void jsontoejdb( string data_, EJDB *jb, EJCOLL *coll, bson_oid_t oid)
{
    json_t *root; json_t *data1; json_t *data; json_t *data2;
    json_t *data3; json_t *data4; json_t *data5; json_t *data6;
    json_error_t jerror;

    bson exp;
    bson_init(&exp);

    const char * JSON = data_.c_str();
    root = json_loads(JSON, 0, &jerror);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
    }
    else
    {
        data = json_object_get(root, expsample);
        if(json_is_string(data))
        bson_append_string(&exp, expsample, json_string_value(data));
        data = json_object_get(root, expdataset);
        if(json_is_string(data))
        bson_append_string(&exp, expdataset, json_string_value(data));
        data = json_object_get(root, Tunit);
        if(json_is_string(data))
        bson_append_string(&exp, Tunit, json_string_value(data));
        data = json_object_get(root, Punit);
        if(json_is_string(data))
        bson_append_string(&exp, Punit, json_string_value(data));
        data = json_object_get(root, Vunit);
        if(json_is_string(data))
        bson_append_string(&exp, Vunit, json_string_value(data));

        data = json_object_get(root, sT);
        if(json_is_integer(data))
        bson_append_int(&exp, sT, json_integer_value(data));
        data = json_object_get(root, sP);
        if(json_is_integer(data))
        bson_append_int(&exp, sP, json_integer_value(data));
        data = json_object_get(root, sV);
        if(json_is_integer(data))
        bson_append_int(&exp, sV, json_integer_value(data));
        data = json_object_get(root, Weight);
        if(json_is_integer(data))
        bson_append_int(&exp, Weight, json_integer_value(data));

        // 2nd level - bulk composition of chemical system for this experiment
        // array of components
        //++ START array sbcomp ++//
        data = json_object_get(root, sbcomp);
        if(json_is_array(data))
        {
            bson_append_start_array(&exp, sbcomp);
            for (int i=0; i < json_array_size(data); i++)
            {
                data1 = json_array_get(data, i);
                if (json_is_object(data1))
                {
                    bson_append_start_object(&exp, boost::lexical_cast<string>(i).c_str());
                    data2 = json_object_get(data1, comp);
                    if(json_is_string(data2))
                    bson_append_string(&exp, comp, json_string_value(data2));
                    data2 = json_object_get(data1, bQnt);
                    if(json_is_real(data2) || json_is_integer(data2) )
                    bson_append_double(&exp, bQnt, boost::lexical_cast<double>(json_integer_value(data2)));
                }
                bson_append_finish_object(&exp);
            }
            bson_append_finish_array(&exp);
        }

        // 2nd level - data for phases charactrised/measured in this experiment
        //++ START array expphases ++//
        data = json_object_get(root, expphases);
        if(json_is_array(data))
        {
            bson_append_start_array(&exp, expphases);
            for (int i=0; i < json_array_size(data); i++)
            {
                data1 = json_array_get(data, i);
                if (json_is_object(data1))
                {
                    bson_append_start_object(&exp, boost::lexical_cast<string>(i).c_str());
                    data2 = json_object_get(data1, phase);
                    if(json_is_string(data2))
                    bson_append_string(&exp, phase, json_string_value(data2));

                    //++ START array phprop ++//
                    data2 = json_object_get(data1, phprop);
                    if(json_is_array(data2))
                    {
                        bson_append_start_array(&exp, phprop);
                        for (int i=0; i < json_array_size(data2); i++)
                        {
                            data3 = json_array_get(data2, i);
                            if (json_is_object(data3))
                            {
                                bson_append_start_object(&exp, boost::lexical_cast<string>(i).c_str());
                                data4 = json_object_get(data3, property);
                                if(json_is_string(data4))
                                bson_append_string(&exp, property, json_string_value(data4));
                                data4 = json_object_get(data3, pQnt);
                                if(json_is_real(data4))
                                bson_append_double(&exp, pQnt, json_real_value(data4));
                                data4 = json_object_get(data3, Qerror);
                                if(json_is_real(data4))
                                bson_append_double(&exp, Qerror, json_real_value(data4));
                                data4 = json_object_get(data3, Qunit);
                                if(json_is_real(data4))
                                bson_append_double(&exp, Qunit, json_real_value(data4));
                            }
                            bson_append_finish_object(&exp);
                        }
                        bson_append_finish_array(&exp);
                    }

                    //++ START array phcomp ++//
                    data2 = json_object_get(data1, phcomp);
                    if(json_is_array(data2))
                    {
                        bson_append_start_array(&exp, phcomp);
                        for (int i=0; i < json_array_size(data2); i++)
                        {
                            data3 = json_array_get(data2, i);
                            if (json_is_object(data3))
                            {
                                bson_append_start_object(&exp, boost::lexical_cast<string>(i).c_str());
                                data4 = json_object_get(data3, element);
                                if(json_is_string(data4))
                                bson_append_string(&exp, element, json_string_value(data4));
                                data4 = json_object_get(data3, eQnt);
                                if(json_is_real(data4))
                                bson_append_double(&exp, eQnt, json_real_value(data4));
                                data4 = json_object_get(data3, Qerror);
                                if(json_is_real(data4))
                                bson_append_double(&exp, Qerror, json_real_value(data4));
                                data4 = json_object_get(data3, Qunit);
                                if(json_is_real(data4))
                                bson_append_double(&exp, Qunit, json_real_value(data4));
                            }
                            bson_append_finish_object(&exp);
                        }
                        bson_append_finish_array(&exp);
                    }


                    //++ START array phspecies ++//
                    data2 = json_object_get(data1, phspecies);
                    if(json_is_array(data2))
                    {
                        bson_append_start_array(&exp, phspecies);
                        for (int i=0; i < json_array_size(data2); i++)
                        {
                            data3 = json_array_get(data2, i);
                            if (json_is_object(data3))
                            {
                                bson_append_start_object(&exp, boost::lexical_cast<string>(i).c_str());
                                data4 = json_object_get(data3, species);
                                if(json_is_string(data4))
                                bson_append_string(&exp, species, json_string_value(data4));

                                // Dcomp properties
                                //++ START array phprop ++//
                                data4 = json_object_get(data3, dcompprop);
                                if(json_is_array(data4))
                                {
                                    bson_append_start_array(&exp, dcompprop);
                                    for (int i=0; i < json_array_size(data4); i++)
                                    {
                                        data5 = json_array_get(data4, i);
                                        if (json_is_object(data5))
                                        {
                                            bson_append_start_object(&exp, boost::lexical_cast<string>(i).c_str());
                                            data6 = json_object_get(data5, property);
                                            if(json_is_string(data6))
                                            bson_append_string(&exp, property, json_string_value(data6));
                                            data6 = json_object_get(data5, pQnt);
                                            if(json_is_real(data6))
                                            bson_append_double(&exp, pQnt, json_real_value(data6));
                                            data6 = json_object_get(data5, Qerror);
                                            if(json_is_real(data6))
                                            bson_append_double(&exp, Qerror, json_real_value(data6));
                                            data6 = json_object_get(data5, Qunit);
                                            if(json_is_real(data6))
                                            bson_append_double(&exp, Qunit, json_real_value(data6));
                                        }
                                        bson_append_finish_object(&exp);
                                    }
                                    bson_append_finish_array(&exp);
                                }
                            }
                            bson_append_finish_object(&exp);
                        }
                        bson_append_finish_array(&exp);
                    }
                }
                bson_append_finish_object(&exp);
            }
            bson_append_finish_array(&exp);
        }
    }

    bson_finish(&exp);
    ejdbsavebson(coll, &exp, &oid); // saving the document in the database
    bson_destroy(&exp);
}
