HEADERS += $$FIT_H/gemsfit_iofiles.h \
           $$FIT_H/gemsfit_task.h \
           $$KEYS_H/keywords.h \
           $$FIT_H/data_manager.h \
    $$PWD/muparserfix.h \
    src-fit/opt_vector.h \
    src-fit/optimization.h \
    src-fit/gemsfit_global_variables.h \
    src-fit/gemsfit_global_functions.h \
    src-fit/gemsfit_target_functions.h \
    src-fit/statistics.h \
    src-fit/gemsfit_nested_functions.h \
    src-fit/s_formula.h \
    $$PWD/json_parse.h

SOURCES += $$FIT_CPP/main.cpp \
           $$FIT_CPP/gemsfit_iofiles.cpp \
           $$FIT_CPP/gemsfit_task.cpp \
           $$FIT_CPP/data_manager.cpp \
    $$PWD/muparserfix.cpp \
    src-fit/opt_vector.cpp \
    src-fit/optimization.cpp \
    src-fit/gemsfit_global_functions.cpp \
    src-fit/gemsfit_target_functions.cpp \
    src-fit/statistics.cpp \
    src-fit/gemsfit_nested_functions.cpp \
    src-fit/gemsfit_task2.cpp \
    src-fit/s_formula.cpp \
    $$PWD/json_parse.cpp



