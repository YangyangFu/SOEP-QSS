// QSS1 Zero-Crossing Variable
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

#ifndef QSS_dfn_Variable_ZC1_hh_INCLUDED
#define QSS_dfn_Variable_ZC1_hh_INCLUDED

// QSS Headers
#include <QSS/dfn/Variable_ZC.hh>

namespace QSS {
namespace dfn {

// QSS1 Zero-Crossing Variable
template< template< typename > class F, template< typename > class H >
class Variable_ZC1 final : public Variable_ZC< F, H >
{

public: // Types

	using Super = Variable_ZC< F, H >;
	using Time = Variable::Time;
	using Value = Variable::Value;
	using Crossing = Variable::Crossing;

	using Super::crossing;
	using Super::name;
	using Super::rTol;
	using Super::aTol;
	using Super::qTol;
	using Super::xIni;
	using Super::tQ;
	using Super::tX;
	using Super::tE;
	using Super::tZ;
	using Super::tZ_prev;
	using Super::dt_min;
	using Super::dt_max;
	using Super::dt_inf;
	using Super::self_observer;

	using Super::event;
	using Super::has;
	using Super::tE_infinity_tQ;

protected: // Types

	using Super::h_;

	using Super::crossing_type;

private: // Types

	using Super::event_;
	using Super::f_;
	using Super::observers_;

public: // Creation

	// Constructor
	explicit
	Variable_ZC1(
	 std::string const & name,
	 Value const rTol = 1.0e-4,
	 Value const aTol = 1.0e-6
	) :
	 Super( name, rTol, aTol )
	{}

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
		assert( ( tX <= t ) && ( t <= tE ) );
		return x_0_ + ( x_1_ * ( t - tX ) );
	}

	// Continuous First Derivative at Time t
	Value
	x1( Time const t ) const
	{
		assert( ( tX <= t ) && ( t <= tE ) );
		(void)t; // Suppress unused parameter warning
		return x_1_;
	}

	// Quantized Value at Time t
	Value
	q( Time const t ) const
	{
		assert( ( tQ <= t ) && ( t <= tE ) );
		(void)t; // Suppress unused parameter warning
		return q_0_;
	}

public: // Methods

	// Initialization
	void
	init()
	{
		self_observer = f_.finalize( this );
		if ( self_observer ) {
			std::cerr << "Error: Zero-crossing variable is self-observer: " << name << std::endl;
			std::exit( EXIT_FAILURE );
		}
		if ( ! observers_.empty() ) {
			std::cerr << "Error: Zero-crossing variable has observers: " << name << std::endl;
			std::exit( EXIT_FAILURE );
		}
		x_0_ = q_0_ = f_.q( tQ );
		set_qTol();
		x_1_ = f_.q1( tQ );
		set_tE();
		set_tZ();
		event( tE < tZ ? events.add_QSS( tE, this ) : events.add_ZC( tZ, this ) );
		if ( options::output::d ) std::cout << "! " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << "   tZ=" << tZ << '\n';
	}

	// Set Current Tolerance
	void
	set_qTol()
	{
		qTol = std::max( rTol * std::abs( q_0_ ), aTol );
		assert( qTol > 0.0 );
	}

	// QSS Advance
	void
	advance_QSS()
	{
		advance_QSS_core();
		if ( options::output::d ) std::cout << "! " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << "   tZ=" << tZ << '\n';
	}

	// QSS Advance: Simultaneous
	void
	advance_QSS_simultaneous()
	{
		advance_QSS_core();
		if ( options::output::d ) std::cout << "= " << name << '(' << tQ << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << "   tZ=" << tZ << '\n';
	}

	// Observer Advance
	void
	advance_observer( Time const t )
	{
		assert( ( tX <= t ) && ( t <= tE ) );
		int const sign_old( t == tZ_prev ? 0 : signum( x( t ) ) ); // Treat as if exactly zero if t is previous zero-crossing event time
		x_0_ = q_0_ = f_.q( tX = tQ = t );
		int const sign_new( signum( x_0_ ) );
		set_qTol();
		x_1_ = f_.q1( t );
		set_tE();
		crossing_detect( sign_old, signum( x_0_ ) );
		if ( options::output::d ) std::cout << "  " << name << '(' << t << ')' << " = " << q_0_ << " quantized, " << x_0_ << "+" << x_1_ << "*t internal   tE=" << tE << "   tZ=" << tZ <<  '\n';
	}

	// Zero-Crossing Advance
	void
	advance_ZC()
	{
		h_( tZ, crossing ); // Handler
		if ( options::output::d ) std::cout << "Z " << name << '(' << tZ << ')' << '\n';
		set_tZ( tZ_prev = tZ ); // Next zero-crossing: Might be in active segment
		event( tE < tZ ? events.shift_QSS( tE, event() ) : events.shift_ZC( tZ, event() ) );
	}

private: // Methods

	// QSS Advance: Core
	void
	advance_QSS_core()
	{
		x_0_ = q_0_ = f_.q( tX = tQ = tE );
		set_qTol();
		x_1_ = f_.q1( tE );
		set_tE();
		set_tZ();
		event( tE < tZ ? events.shift_QSS( tE, event() ) : events.shift_ZC( tZ, event() ) );
	}

	// Set End Time
	void
	set_tE()
	{
		assert( tX <= tQ );
		assert( dt_min <= dt_max );
		Time dt( x_1_ != 0.0 ? qTol / std::abs( x_1_ ) : infinity );
		dt = std::min( std::max( dt, dt_min ), dt_max );
		tE = ( dt != infinity ? tQ + dt : infinity );
		tE_infinity_tQ();
	}

	// Set Zero-Crossing Time and Type on Active Segment
	void
	set_tZ()
	{
		// Simple root search: Only robust for small active segments with continuous rep close to function //Do Make robust version
		if ( x_0_ == 0.0 ) { // Zero at segment start
			tZ = infinity;
		} else {
			int const sign_old( signum( x_0_ ) );
			int const sign_new( signum( x_1_ ) );
			Crossing const crossing_check( crossing_type( sign_old, sign_new ) );
			if ( has( crossing_check ) ) { // Crossing type is relevant
				if ( ( x_1_ != 0.0 ) && ( sign_old != sign_new ) ) { // Heading towards zero
					tZ = tX - ( x_0_ / x_1_ ); // Root of continuous rep
					assert( tX < tZ );
					crossing = crossing_check;
					Time t( tZ ), t_p( t );
					Value const vZ( f_.q( tZ ) );
					Value v( vZ ), v_p( vZ );
					Value m( 1.0 ); // Multiplier
					std::size_t i( 0 );
					std::size_t const n( 10u ); // Max iterations
					while ( ( ++i <= n ) && ( ( std::abs( v ) > aTol ) || ( std::abs( v ) < std::abs( v_p ) ) ) ) {
						Value const d( f_.q1( t ) );
						if ( d == 0.0 ) break;
						if ( ( signum( d ) != sign_old ) && ( tE < std::min( t_p, t ) ) ) break; // Zero-crossing seems to be >tE so don't refine further
						t -= m * ( v / d );
						v = f_.q( t );
						if ( std::abs( v ) >= std::abs( v_p ) ) m *= 0.5; // Non-converging step: Reduce step size
						t_p = t;
						v_p = v;
					}
					if ( ( t >= tX ) && ( std::abs( v ) < std::abs( vZ ) ) ) tZ = t;
					if ( ( i == n ) && ( options::output::d ) ) std::cout << "  " << name << '(' << t << ')' << " tZ may not have converged" <<  '\n';
				} else { // Heading away from zero
					tZ = infinity;
				}
			} else { // Crossing type not relevant
				tZ = infinity;
			}
		}
	}

	// Set Zero-Crossing Time and Type on (tB,tE]
	void
	set_tZ( Time const tB )
	{
		set_tZ(); // Look at whole active segment for now: To be refined
		tZ = ( tZ > tB ? tZ : infinity );
	}

	// Crossing Detection
	void
	crossing_detect( int const sign_old, int const sign_new )
	{
		if ( sign_old != sign_new ) { // Zero-crossing occurs at t
			Crossing const crossing_check( crossing_type( sign_old, sign_new ) );
			if ( has( crossing_check ) ) { // Crossing type is relevant
				crossing = crossing_check;
				tZ = tX;
				event( events.shift_ZC( tZ, event() ) );
			} else {
				set_tZ();
				event( tE < tZ ? events.shift_QSS( tE, event() ) : events.shift_ZC( tZ, event() ) );
			}
		} else {
			set_tZ();
			event( tE < tZ ? events.shift_QSS( tE, event() ) : events.shift_ZC( tZ, event() ) );
		}
	}

private: // Data

	Value x_0_{ 0.0 }, x_1_{ 0.0 }; // Continuous rep coefficients
	Value q_0_{ 0.0 }; // Quantized rep coefficients

};

} // dfn
} // QSS

#endif
