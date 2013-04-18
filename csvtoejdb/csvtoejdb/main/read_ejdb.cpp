# include "read_ejdb.h"
# include <iostream>

using namespace std;

void bson_read_raw(FILE *f, const char *data) {
    bson_iterator i, j;
    const char *key;
    string key_;
    int temp;
    bson_timestamp_t ts;
    char oidhex[25];
    bson scope;
    bson_iterator_from_buffer(&i, data);

    while (bson_iterator_next(&i)) {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
            break;
        key = bson_iterator_key(&i);
        key_ = key;

//        for (temp = 0; temp <= depth; temp++)
//            bson_fprintf(f, "\t");
//        bson_fprintf(f, "%s : %d \t ", key, t);

        if (key_ == expsample)
        {
            cout << key << " is: "<< bson_iterator_string(&i) << endl;
        }

        if ((key_ == sbcomp) && (t == BSON_ARRAY))
        {
            cout << key << " is: "<< bson_iterator_string(&i) <<endl;

            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j)) {
                key = bson_iterator_key(&j);
                key_ = key;

                if ((key_ == comp))
                {
                    cout << key << " is: "<< bson_iterator_string(&j) <<endl;
                }
            }


//            bson_read_raw(f, bson_iterator_value(&i));
        }



//        bson_fprintf(f, "\n");
    }
}
