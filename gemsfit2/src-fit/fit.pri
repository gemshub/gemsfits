HEADERS += $$FIT_H/gemsfit_iofiles.h \
           $$FIT_H/gemsfit_task.h \
           $$KEYS_H/keywords.h \
           $$FIT_H/data_manager.h \
    src-fit/opt_vector.h \
    src-fit/optimization.h \
    src-fit/gemsfit_global_variables.h \
    src-fit/gemsfit_global_functions.h \
    src-fit/gemsfit_target_functions.h

SOURCES += $$FIT_CPP/main.cpp \
           $$FIT_CPP/gemsfit_iofiles.cpp \
           $$FIT_CPP/gemsfit_task.cpp \
           $$FIT_CPP/data_manager.cpp \
    src-fit/opt_vector.cpp \
    src-fit/optimization.cpp \
    src-fit/gemsfit_global_functions.cpp \
    src-fit/gemsfit_target_functions.cpp



