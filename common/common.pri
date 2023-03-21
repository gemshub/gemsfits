         HEADERS += $$COMMON_H/f_database.h \
                    $$COMMON_H/f_file.h \
                    $$COMMON_H/v_bson_ejdb.h \
                    $$COMMON_H/v_yaml.h \
                    $$COMMON_H/v_json.h \
                    $$COMMON_H/type_test.h \
                    $$COMMON_H/jsonparser.h \
    $$PWD/db_ejdb.h \
    $$PWD/dbdriver.h


         SOURCES += $$COMMON_CPP/f_database.cpp \
                    $$COMMON_CPP/f_file.cpp \
                    $$COMMON_CPP/v_bson_ejdb.cpp \
                    $$COMMON_CPP/v_yaml.cpp \
                    $$COMMON_CPP/v_json.cpp \
                    $$COMMON_CPP/jsonparser.cpp \
    $$PWD/db_ejdb.cpp


