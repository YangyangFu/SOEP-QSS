// FMU-Based QSS Integer Variable
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

#ifndef QSS_fmu_Variable_I_hh_INCLUDED
#define QSS_fmu_Variable_I_hh_INCLUDED

// QSS Headers
#include <QSS/fmu/Variable.hh>

namespace QSS {
namespace fmu {

// FMU-Based QSS Integer Variable
class Variable_I final : public Variable
{

public: // Types

	using Super = Variable;
	using Integer = int;

public: // Creation

	// Name + Value Constructor
	Variable_I(
	 std::string const & name,
	 Integer const xIni,
	 FMU_Variable const var = FMU_Variable()
	) :
	 Super( name, xIni, var ),
	 x_( xIni )
	{}

	// Name Constructor
	explicit
	Variable_I(
	 std::string const & name,
	 FMU_Variable const var = FMU_Variable()
	) :
	 Super( name, var ),
	 x_( static_cast< Integer >( xIni ) )
	{}

public: // Predicate

	// Discrete Variable?
	bool
	is_Discrete() const
	{
		return true;
	}

public: // Properties

	// Order of Method
	int
	order() const
	{
		return 0;
	}

	// Integer Value
	Integer
	i() const
	{
		return x_;
	}

	// Continuous Integer Value at Time t
	Integer
	i( Time const ) const
	{
		return x_;
	}

	// Value
	Value
	x() const
	{
		return x_;
	}

	// Continuous Value at Time t
	Value
	x( Time const ) const
	{
		return x_;
	}

	// Continuous First Derivative at Time t
	Value
	x1( Time const ) const
	{
		return 0.0;
	}

	// Quantized Value
	Value
	q() const
	{
		return x_;
	}

	// Quantized Value at Time t
	Value
	q( Time const ) const
	{
		return x_;
	}

	// Simultaneous Value at Time t
	Value
	s( Time const ) const
	{
		return x_;
	}

	// Simultaneous Numeric Differentiation Value at Time t
	Value
	sn( Time const ) const
	{
		return x_;
	}

public: // Methods

	// Initialization
	void
	init()
	{
		init_0();
	}

	// Initialization to a Value
	void
	init( Value const x )
	{
		init_0( x );
	}

	// Initialization: Stage 0
	void
	init_0()
	{
		assert( observees_.empty() );
		shrink_observers(); // Optional
		sort_observers();
		x_ = static_cast< Integer >( xIni );
		add_handler();
		if ( options::output::d ) std::cout << "! " << name << '(' << tQ << ')' << " = " << x_ << '\n';
	}

	// Initialization to a Value: Stage 0
	void
	init_0( Value const x )
	{
		assert( observees_.empty() );
		shrink_observers(); // Optional
		sort_observers();
		x_ = static_cast< Integer >( x );
		add_handler();
		if ( options::output::d ) std::cout << "! " << name << '(' << tQ << ')' << " = " << x_ << '\n';
	}

	// Handler Advance
	void
	advance_handler( Time const t )
	{
		assert( tX <= t );
		tX = tQ = t;
		x_ = fmu_get_integer_value(); // Assume FMU ran zero-crossing handler
		advance_observers_1();
		if ( observers_max_order_ >= 2 ) {
			fmu::set_time( tN = tQ + options::dtNum );
			advance_observers_2();
		}
		shift_handler();
		if ( options::output::d ) {
			std::cout << "* " << name << '(' << tQ << ')' << " = " << x_ << '\n';
			advance_observers_d();
		}
	}

	// Handler Advance: Stage 0
	void
	advance_handler_0( Time const t )
	{
		assert( tX <= t );
		tX = tQ = t;
		x_ = fmu_get_integer_value(); // Assume FMU ran zero-crossing handler
		shift_handler();
		if ( options::output::d ) std::cout << "* " << name << '(' << tQ << ')' << " = " << x_ << '\n';
	}

private: // Data

	Integer x_; // Value

};

} // fmu
} // QSS

#endif
