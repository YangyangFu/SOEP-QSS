// Achilles and the Tortoise Numeric Differentiation Example Setup
//
// Project: QSS Solver
//
// Developed by Objexx Engineering, Inc. (http://objexx.com) under contract to
// the National Renewable Energy Laboratory of the U.S. Department of Energy
//
// Copyright (c) 2017 Objexx Engineerinc, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
// (3) Neither the name of the copyright holder nor the names of its
//     contributors may be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// QSS Headers
#include <QSS/dfn/mdl/achilles_ND.hh>
#include <QSS/dfn/mdl/Function_LTI_ND.hh>
//#include <QSS/dfn/Variable_LIQSS1.hh>
//#include <QSS/dfn/Variable_LIQSS2.hh>
#include <QSS/dfn/Variable_QSS1.hh>
#include <QSS/dfn/Variable_QSS2.hh>
#include <QSS/dfn/Variable_QSS3.hh>
#include <QSS/options.hh>

namespace QSS {
namespace dfn {
namespace mdl {

using Variables = std::vector< Variable * >;

// Achilles and the Tortoise Numeric Differentiation Example Setup
void
achilles_ND( Variables & vars )
{
	using namespace options;

	// Timing
	if ( ! options::tEnd_set ) options::tEnd = 10.0;

	// Variables
	using V = Variable_QSS< Function_LTI_ND >;
	V * x1( nullptr );
	V * x2( nullptr );
	vars.clear();
	vars.reserve( 2 );
	if ( qss == QSS::QSS1 ) {
		vars.push_back( x1 = new Variable_QSS1< Function_LTI_ND >( "x1", rTol, aTol, 0.0 ) );
		vars.push_back( x2 = new Variable_QSS1< Function_LTI_ND >( "x2", rTol, aTol, 2.0 ) );
	} else if ( qss == QSS::QSS2 ) {
		vars.push_back( x1 = new Variable_QSS2< Function_LTI_ND >( "x1", rTol, aTol, 0.0 ) );
		vars.push_back( x2 = new Variable_QSS2< Function_LTI_ND >( "x2", rTol, aTol, 2.0 ) );
	} else if ( qss == QSS::QSS3 ) {
		vars.push_back( x1 = new Variable_QSS3< Function_LTI_ND >( "x1", rTol, aTol, 0.0 ) );
		vars.push_back( x2 = new Variable_QSS3< Function_LTI_ND >( "x2", rTol, aTol, 2.0 ) );
//	} else if ( qss == QSS::LIQSS1 ) {
//		vars.push_back( x1 = new Variable_LIQSS1< Function_LTI_ND >( "x1", rTol, aTol, 0.0 ) );
//		vars.push_back( x2 = new Variable_LIQSS1< Function_LTI_ND >( "x2", rTol, aTol, 2.0 ) );
//	} else if ( qss == QSS::LIQSS2 ) {
//		vars.push_back( x1 = new Variable_LIQSS2< Function_LTI_ND >( "x1", rTol, aTol, 0.0 ) );
//		vars.push_back( x2 = new Variable_LIQSS2< Function_LTI_ND >( "x2", rTol, aTol, 2.0 ) );
	} else {
		std::cerr << "Unsupported QSS method" << std::endl;
		std::exit( EXIT_FAILURE );
	}

	// Derivatives
	x1->d().add( -0.5, x1 ).add( 1.5, x2 );
	x2->d().add( -1.0, x1 );
}

} // mdl
} // dfn
} // QSS
