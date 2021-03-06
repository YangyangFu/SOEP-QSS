// LIQSS1 Variable
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

#ifndef QSS_dfn_Variable_LIQSS1_hh_INCLUDED
#define QSS_dfn_Variable_LIQSS1_hh_INCLUDED

// QSS Headers
#include <QSS/dfn/Variable_QSS.hh>

namespace QSS {
namespace dfn {

// LIQSS1 Variable
template< template< typename > class F >
class Variable_LIQSS1 final : public Variable_QSS< F >
{

public: // Types

	using Super = Variable_QSS< F >;
	using Time = Variable::Time;
	using Value = Variable::Value;
	using AdvanceSpecs_LIQSS1 = typename Variable::AdvanceSpecs_LIQSS1;

	using Super::name;
	using Super::rTol;
	using Super::aTol;
	using Super::qTol;
	using Super::xIni;
	using Super::tQ;
	using Super::tX;
	using Super::tE;
	using Super::sT;
	using Super::dt_min;
	using Super::dt_max;
	using Super::dt_inf;
	using Super::self_observer;

	using Super::advance_observers;
	using Super::event;
	using Super::shrink_observers;
	using Super::tE_infinity_tQ;
	using Super::tE_infinity_tX;

private: // Types

	using Super::d_;
	using Super::event_;
	using Super::observers_;

public: // Creation

	// Constructor
	explicit
	Variable_LIQSS1(
	 std::string const & name,
	 Value const rTol = 1.0e-4,
	 Value const aTol = 1.0e-6,
	 Value const xIni = 0.0
	) :
	 Super( name, rTol, aTol, xIni ),
	 x_0_( xIni ),
	 q_c_( xIni ),
	 q_0_( xIni )
	{
		set_qTol();
	}

public: // Properties

	// Order of Method
	int
	order() const
	{
		return 1;
	}

	// Continuous Value at Time t
	Value
	x( Time const t ) const
	{
		return x_0_ + ( x_1_ * ( t - tX ) );
	}

	// Continuous First Derivative at Time t
	Value
	x1( Time const ) const
	{
		return x_1_;
	}

	// Quantized Value at Time t
	Value
	q( Time const ) const
	{
		return q_0_;
	}

	// Simultaneous Value at Time t
	Value
	s( Time const ) const
	{
		return ( sT == events.active_superdense_time() ? q_c_ : q_0_ );
	}

	// Simultaneous Numeric Differentiation Value at Time t
	Value
	sn( Time const t ) const
	{
		return ( sT == events.active_superdense_time() ? q_c_ : q_0_ );
	}

public: // Methods

	// Initialization
	void
	init()
	{
		init_0();
		init_1();
	}

	// Initialization to a Value
	void
	init( Value const x )
	{
		init_0( x );
		init_1();
	}

	// Initialization: Stage 0
	void
	init_0()
	{
		x_0_ = q_c_ = q_0_ = xIni;
		set_qTol();
	}

	// Initialization to a Value: Stage 0
	void
	init_0( Value const x )
	{
		x_0_ = q_c_ = q_0_ = x;
		set_qTol();
	}

	// Initialization: Stage 1
	void
	init_1()
	{
		self_observer = d_.finalize( this );
		shrink_observers(); // Optional
		if ( self_observer ) {
			advance_s( tQ ); // Simultaneous reps used to avoid cyclic dependency
		} else {
			x_1_ = d_.s( tQ ); // Simultaneous reps used to avoid cyclic dependency
			q_0_ += signum( x_1_ ) * qTol;
		}
		set_tE_aligned();
		event( events.add_QSS( tE, this ) );
		if ( options::output::d ) std::cout << "! " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << '\n';
	}

	// Set Current Tolerance
	void
	set_qTol()
	{
		qTol = std::max( rTol * std::abs( q_c_ ), aTol );
		assert( qTol > 0.0 );
	}

	// QSS Advance
	void
	advance_QSS()
	{
		x_0_ = q_c_ = q_0_ = x_0_ + ( x_1_ * ( ( tQ = tE ) - tX ) );
		set_qTol();
		if ( self_observer ) {
			advance_q( tX = tE );
		} else {
			x_1_ = d_.q( tX = tE );
			q_0_ += signum( x_1_ ) * qTol;
		}
		set_tE_aligned();
		event( events.shift_QSS( tE, event() ) );
		if ( options::output::d ) std::cout << "! " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << '\n';
		advance_observers();
	}

	// QSS Advance: Stage 0
	void
	advance_QSS_0()
	{
		x_0_ = q_c_ = q_0_ = x_0_ + ( x_1_ * ( ( tQ = tE ) - tX ) );
		tX = tE;
		set_qTol();
	}

	// QSS Advance: Stage 1
	void
	advance_QSS_1()
	{
		if ( self_observer ) {
			advance_s( tE ); // Simultaneous reps used to avoid cyclic dependency
		} else {
			x_1_ = d_.s( tE ); // Simultaneous reps used to avoid cyclic dependency
			q_0_ += signum( x_1_ ) * qTol;
		}
		set_tE_aligned();
		event( events.shift_QSS( tE, event() ) );
		if ( options::output::d ) std::cout << "= " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << '\n';
	}

	// Observer Advance
	void
	advance_observer( Time const t )
	{
		assert( ( tX <= t ) && ( t <= tE ) );
		x_0_ = x_0_ + ( x_1_ * ( t - tX ) );
		x_1_ = d_.q( tX = t );
		set_tE_unaligned();
		event( events.shift_QSS( tE, event() ) );
		if ( options::output::d ) std::cout << "  " << name << '(' << t << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << '\n';
	}

	// Handler Advance
	void
	advance_handler( Time const t, Value const x )
	{
		assert( ( tX <= t ) && ( tQ <= t ) && ( t <= tE ) );
		x_0_ = q_c_ = q_0_ = x;
		set_qTol();
		x_1_ = d_.q( tX = tQ = t );
		set_tE_aligned();
		event( events.shift_QSS( tE, event() ) );
		if ( options::output::d ) std::cout << "* " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << '\n';
		advance_observers();
	}

	// Handler Advance: Stage 0
	void
	advance_handler_0( Time const t, Value const x )
	{
		assert( ( tX <= t ) && ( tQ <= t ) && ( t <= tE ) );
		tX = tQ = t;
		x_0_ = q_c_ = q_0_ = x;
		set_qTol();
	}

	// Handler Advance: Stage 1
	void
	advance_handler_1()
	{
		x_1_ = d_.q( tQ );
		set_tE_aligned();
		event( events.shift_QSS( tE, event() ) );
		if ( options::output::d ) std::cout << "* " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << '\n';
	}

private: // Methods

	// Set End Time: Quantized and Continuous Aligned
	void
	set_tE_aligned()
	{
		assert( tX <= tQ );
		assert( dt_min <= dt_max );
		Time dt( x_1_ != 0.0 ? qTol / std::abs( x_1_ ) : infinity );
		dt = std::min( std::max( dt, dt_min ), dt_max );
		tE = ( dt != infinity ? tQ + dt : infinity );
		tE_infinity_tQ();
	}

	// Set End Time: Quantized and Continuous Unaligned
	void
	set_tE_unaligned()
	{
		assert( tQ <= tX );
		assert( dt_min <= dt_max );
		Time dt(
		 ( x_1_ > 0.0 ? ( q_c_ + qTol - x_0_ ) / x_1_ :
		 ( x_1_ < 0.0 ? ( q_c_ - qTol - x_0_ ) / x_1_ :
		 infinity ) ) );
		dt = std::min( std::max( dt, dt_min ), dt_max );
		tE = ( dt != infinity ? tX + dt : infinity );
		tE_infinity_tX();
	}

	// Advance Self-Observing Trigger
	void
	advance_LIQSS( AdvanceSpecs_LIQSS1 const & specs )
	{
		assert( qTol > 0.0 );
		assert( self_observer );

		// Set coefficients based on derivative signs
		int const dls( signum( specs.l ) );
		int const dus( signum( specs.u ) );
		if ( ( dls == -1 ) && ( dus == -1 ) ) { // Downward trajectory
			q_0_ -= qTol;
			x_1_ = specs.l;
		} else if ( ( dls == +1 ) && ( dus == +1 ) ) { // Upward trajectory
			q_0_ += qTol;
			x_1_ = specs.u;
		} else { // Flat trajectory
			q_0_ = std::min( std::max( specs.z, q_0_ - qTol ), q_0_ + qTol ); // Clipped in case of roundoff
			x_1_ = 0.0;
		}
	}

	// Advance Self-Observing Trigger using Continuous Derivative
	void
	advance_x( Time const t )
	{
		advance_LIQSS( d_.xlu1( t, qTol ) );
	}

	// Advance Self-Observing Trigger using Quantized Derivative
	void
	advance_q( Time const t )
	{
		advance_LIQSS( d_.qlu1( t, qTol ) );
	}

	// Advance Self-Observing Trigger using Simultaneous Derivative
	void
	advance_s( Time const t )
	{
		advance_LIQSS( d_.slu1( t, qTol ) );
	}

private: // Data

	Value x_0_{ 0.0 }, x_1_{ 0.0 }; // Continuous rep coefficients
	Value q_c_{ 0.0 }, q_0_{ 0.0 }; // Quantized rep coefficients

};

} // dfn
} // QSS

#endif
