#ifndef QSS_dnf_mdl_bball_hh_INCLUDED
#define QSS_dnf_mdl_bball_hh_INCLUDED

// Bouncing Ball Example Setup
//
// Project: QSS Solver
//
// Developed by Objexx Engineering, Inc. (http://objexx.com)
// under contract to the National Renewable Energy Laboratory
// of the U.S. Department of Energy

// QSS Headers
#include <QSS/dfn/Variable.fwd.hh>

// C++ Headers
#include <vector>

namespace QSS {
namespace dfn {
namespace mdl {

using Variables = std::vector< Variable * >;

// Bouncing Ball Example Setup
void
bball( Variables & vars );

} // mdl
} // dfn
} // QSS

#endif
