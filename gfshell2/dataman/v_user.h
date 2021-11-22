#ifndef V_USER_H
#define V_USER_H

//-------------------------------------------------------------------
// $Id$
/// \file v_user.h
/// Declaration of platform-specific utility functions and classes
//
// Copyright (C) 1996,2001,2012 A.Rysin, S.Dmytriyeva
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMS3K code for thermodynamic modelling
// by Gibbs energy minimization <http://gems.web.psi.ch/GEMS3K/>
//
// GEMS3K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMS3K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMS3K code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------

#include <algorithm>
#include <iostream>
using namespace std;

#include "string.h"
#include "verror.h"

#ifdef __APPLE__

#ifndef __unix
#define __unix
#endif
#ifndef __FreeBSD
#define __FreeBSD
#endif

typedef unsigned int uint;
#endif

const int MAXKEYWD = 6+1;

#ifndef  __unix

typedef unsigned int uint;

#endif //  __noborl

// Returns string representation of current date in dd/mm/yyyy format
string curDate();

// Returns string representation of current date in dd/mm/yy format
string curDateSmol(char ch = '/');

// Returns string representation of current time in HH:MM  format
string curTime();

// Returns string representation of current date and time
inline
string curDateTime()
{
    return curDate() + curTime();
}
// Returns string representation of time in "dd/mm/yyyy, HH:MM"  format
string timeToStr( time_t time );

#endif // V_USER_H
