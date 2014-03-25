HEADERS += $$DIALOGS_CPP/FITMainWindow.h \
#           $$DIALOGS_H/GraphDialog.h \
#           $$DIALOGS_H/LegendDialog.h \
           $$DIALOGS_H/HelpWindow.h \
    dialogs/ProjectSettingsDialog.h \
    dialogs/DBKeyDialog.h \
    dialogs/PreferencesDialog.h

SOURCES += $$DIALOGS_CPP/FITMainWindow.cpp \
#           $$DIALOGS_CPP/GraphDialog.cpp \
#           $$DIALOGS_CPP/LegendDialog.cpp \
           $$DIALOGS_CPP/HelpWindow.cpp \
    dialogs/FITMainMenu.cpp \
    dialogs/ProjectSettingsDialog.cpp \
    dialogs/DBKeyDialog.cpp \
    dialogs/PreferencesDialog.cpp


FORMS += $$DIALOGS_CPP/HelpWindow4.ui \
         $$DIALOGS_CPP/FITMainWindow.ui \
    dialogs/ProjectSettingsDialog.ui \
    dialogs/DBKeyDialog.ui \
    dialogs/PreferencesDialog.ui
#         $$DIALOGS_CPP/GraphDialog4.ui \
#         $$DIALOGS_CPP/LegendDialog4.ui \
