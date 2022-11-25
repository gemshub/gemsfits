#cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/home/svd/Qt/5.9/gcc_64
#./install-thirdparty.sh /home/svd/Qt/5.9/gcc_64

SOURCES += \
    $$CHARTS_CPP/graph_data.cpp \
    $$CHARTS_CPP/SymbolDialog.cpp \
    $$CHARTS_CPP/LegendDialog.cpp \
    $$CHARTS_CPP/GraphDialog.cpp \
    $$CHARTS_CPP/markershapes.cpp \
    $$CHARTS_CPP/chart_model.cpp \
    $$CHARTS_CPP/chart_view.cpp \

HEADERS += \
    $$CHARTS_H/graph_data.h \
    $$CHARTS_H/GraphDialog.h \
    $$CHARTS_H/LegendDialog.h \
    $$CHARTS_H/SymbolDialog.h \
    $$CHARTS_H/markershapes.h \
    $$CHARTS_H/chart_model.h \
    $$CHARTS_H/chart_view.h \
    $$PWD/from_jsonio.h

FORMS += \
    $$CHARTS_CPP/GraphDialog4.ui \
    $$CHARTS_CPP/LegendDialog4.ui \
    $$CHARTS_CPP/SymbolDialog4.ui
    
