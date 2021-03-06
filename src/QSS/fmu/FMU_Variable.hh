// QSS FMU Variable Specifications
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
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER, THE UNITED STATES
// GOVERNMENT, OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef QSS_fmu_FMU_Variable_hh_INCLUDED
#define QSS_fmu_FMU_Variable_hh_INCLUDED

// FMI Library Headers
#include <FMI2/fmi2_import.h>

// C++ Headers
#include <cstddef>

namespace QSS {
namespace fmu {

// QSS FMU Variable Specifications
class FMU_Variable
{

public: // Types

	using size_type = std::size_t;

public: // Creation

	// Default Constructor
	FMU_Variable()
	{}

	// Real Variable Constructor
	FMU_Variable(
	 fmi2_import_variable_t * const var,
	 fmi2_import_real_variable_t * const rvr,
	 fmi2_value_reference_t const ref,
	 size_type const idx,
	 size_type const ics = 0u
	) :
	 var( var ),
	 rvr( rvr ),
	 ref( ref ),
	 idx( idx ),
	 ics( ics )
	{}

	// Integer Variable Constructor
	FMU_Variable(
	 fmi2_import_variable_t * const var,
	 fmi2_import_integer_variable_t * const ivr,
	 fmi2_value_reference_t const ref,
	 size_type const idx,
	 size_type const ics = 0u
	) :
	 var( var ),
	 ivr( ivr ),
	 ref( ref ),
	 idx( idx ),
	 ics( ics )
	{}

	// Boolean Variable Constructor
	FMU_Variable(
	 fmi2_import_variable_t * const var,
	 fmi2_import_bool_variable_t * const bvr,
	 fmi2_value_reference_t const ref,
	 size_type const idx,
	 size_type const ics = 0u
	) :
	 var( var ),
	 bvr( bvr ),
	 ref( ref ),
	 idx( idx ),
	 ics( ics )
	{}

public: // Data

	fmi2_import_variable_t * var{ nullptr }; // FMU variable pointer (non-owning)
	union { // Support FMU real and integer variables
		fmi2_import_real_variable_t * rvr{ nullptr }; // FMU real variable pointer (non-owning)
		fmi2_import_integer_variable_t * ivr; // FMU integer variable pointer (non-owning)
		fmi2_import_bool_variable_t * bvr; // FMU boolean variable pointer (non-owning)
	};
	fmi2_value_reference_t ref{ 0 }; // FMU variable value reference
	size_type idx{ 0u }; // FMU variable index
	size_type ics{ 0u }; // FMU continuous state index

};

} // fmu
} // QSS

#endif
