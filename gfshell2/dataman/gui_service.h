//-------------------------------------------------------------------
// $Id: gui_service.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of GUI functions
//
// Copyright (C) 2014-2023  S.V.Dmytriyeva
// Uses EJDB (https://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

class QWidget;
class QDir;
class QString;

namespace common {
class TAbstractFile;
}

//----------------------------------------------------------------------
// service functions

const int VF3_1 = 6;
const int VF3_2 = 7;
const int VF3_3 = 2;

void messageCritical(QWidget* par, const std::string& title, const std::string& mess);

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess,
                const std::string& s1, const std::string& s2,  const std::string& s3="&Cancel",
                bool i_mov = false);

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess);

std::vector<std::string> vfMultiKeys(QWidget* par, const char* title, int record_type, const char* key);

bool removeDirectoryEntry(QDir dir);

void removeComments(QString& valCsv);

bool ChooseFileOpen(common::TAbstractFile* file, QWidget* par, std::string& template_path, const char* title, const char *filter);
bool ChooseFileSave(common::TAbstractFile* file, QWidget* par, std::string& template_path, const char* title, const char *filter);


