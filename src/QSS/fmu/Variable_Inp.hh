// FMU-Based QSS Input Variable Abstract Base Class
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

#ifndef QSS_fmu_Variable_Inp_hh_INCLUDED
#define QSS_fmu_Variable_Inp_hh_INCLUDED

// QSS Headers
#include <QSS/fmu/Variable.hh>
#include <QSS/fmu/SmoothToken.hh>

// C++ Headers
#include <functional>

namespace QSS {
namespace fmu {

// FMU-Based QSS Input Variable Abstract Base Class
class Variable_Inp : public Variable
{

public: // Types

	using Super = Variable;
	using Function = std::function< SmoothToken const &( Time const ) >;

protected: // Creation

	// Name + Tolerance Constructor
	explicit
	Variable_Inp(
	 std::string const & name,
	 Value const rTol,
	 Value const aTol,
	 FMU_Variable const var = FMU_Variable(),
	 Function f = Function()
	) :
	 Super( name, rTol, aTol, var ),
	 f_( f )
	{
		xIni = f_( tQ ).x_0;
		tD = f_( tQ ).tD;
	}

	// Name Constructor
	explicit
	Variable_Inp(
	 std::string const & name,
	 FMU_Variable const var = FMU_Variable(),
	 Function f = Function()
	) :
	 Super( name, var ),
	 f_( f )
	{
		xIni = f_( tQ ).x_0;
		tD = f_( tQ ).tD;
	}

	// Copy Constructor
	Variable_Inp( Variable_Inp const & ) = default;

	// Move Constructor
	Variable_Inp( Variable_Inp && ) noexcept = default;

public: // Creation

	// Destructor
	virtual
	~Variable_Inp()
	{}

protected: // Assignment

	// Copy Assignment
	Variable_Inp &
	operator =( Variable_Inp const & ) = default;

	// Move Assignment
	Variable_Inp &
	operator =( Variable_Inp && ) noexcept = default;

public: // Predicate

	// Input Variable?
	bool
	is_Input() const
	{
		return true;
	}

public: // Properties

	// Function
	Function const &
	f() const
	{
		return f_;
	}

	// Function
	Function &
	f()
	{
		return f_;
	}

protected: // Data

	Function f_; // Input function

};

} // fmu
} // QSS

#endif
