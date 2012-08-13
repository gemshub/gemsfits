HEADERS +=  $$FIT_H/gemsfit_global_variables.h \
            $$FIT_H/system_properties.h \
            $$FIT_H/data_manager.h \
            $$FIT_H/opt_vector.h \
            $$FIT_H/plot_class.h \
            $$FIT_H/optimization.h \
            $$FIT_H/gemsfit_global_functions.h \
            $$FIT_H/fit_statistics.h

SOURCES += $$FIT_CPP/system_properties.cpp \
           $$FIT_CPP/data_manager.cpp \
           $$FIT_CPP/opt_vector.cpp \
           $$FIT_CPP/plot_class.cpp \
           $$FIT_CPP/measdata_modified.cpp \
           $$FIT_CPP/optimization.cpp \
           $$FIT_CPP/gemsfit_global_functions.cpp \
           $$FIT_CPP/fit_statistics.cpp \
           $$FIT_CPP/main.cpp
