// FMU-Based Variable Abstract Base Class
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

#ifndef QSS_fmu_Variable_hh_INCLUDED
#define QSS_fmu_Variable_hh_INCLUDED

// QSS Headers
#include <QSS/fmu/Variable.fwd.hh>
#include <QSS/fmu/FMI.hh>
#include <QSS/fmu/FMU_Variable.hh>
#include <QSS/fmu/globals_fmu.hh>
#include <QSS/EventQueue.hh>
#include <QSS/math.hh>
#include <QSS/options.hh>

// C++ Headers
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

namespace QSS {
namespace fmu {

// FMU-Based Variable Abstract Base Class
class Variable
{

public: // Types

	using Time = double;
	using Value = double;
	using Variables = std::vector< Variable * >;
	using EventQ = EventQueue< Variable >;
	using size_type = Variables::size_type;

	// Zero Crossing Type
	enum class Crossing {
	 DnPN = -4, // Downward: Positive to negative
	 DnZN = -3, // Downward: Zero to negative
	 DnPZ = -2, // Downward: Positive to zero
	 Dn = -1,   // Downward
	 Flat =  0, // Flat zero value
	 Up = +1,   // Upward
	 UpNZ = +2, // Upward: Negative to zero
	 UpZP = +3, // Upward: Zero to positive
	 UpNP = +4  // Upward: Negative to positive
	};

protected: // Creation

	// Name + Tolerance + Value Constructor
	Variable(
	 std::string const & name,
	 Value const rTol,
	 Value const aTol,
	 Value const xIni,
	 FMU_Variable const var = FMU_Variable(),
	 FMU_Variable const der = FMU_Variable()
	) :
	 name( name ),
	 rTol( std::max( rTol, 0.0 ) ),
	 aTol( std::max( aTol, std::numeric_limits< Value >::min() ) ),
	 xIni( xIni ),
	 dt_min( options::dtMin ),
	 dt_max( options::dtMax ),
	 dt_inf( options::dtInf ),
	 dt_inf_rlx( options::dtInf == infinity ? infinity : 0.5 * options::dtInf ),
	 var( var ),
	 der( der )
	{}

	// Name + Tolerance Constructor
	Variable(
	 std::string const & name,
	 Value const rTol,
	 Value const aTol,
	 FMU_Variable const var = FMU_Variable(),
	 FMU_Variable const der = FMU_Variable()
	) :
	 name( name ),
	 rTol( std::max( rTol, 0.0 ) ),
	 aTol( std::max( aTol, std::numeric_limits< Value >::min() ) ),
	 xIni( 0.0 ),
	 dt_min( options::dtMin ),
	 dt_max( options::dtMax ),
	 dt_inf( options::dtInf ),
	 dt_inf_rlx( options::dtInf == infinity ? infinity : 0.5 * options::dtInf ),
	 var( var ),
	 der( der )
	{}

	// Name + Value Constructor
	explicit
	Variable(
	 std::string const & name,
	 Value const xIni,
	 FMU_Variable const var = FMU_Variable(),
	 FMU_Variable const der = FMU_Variable()
	) :
	 name( name ),
	 xIni( xIni ),
	 dt_min( options::dtMin ),
	 dt_max( options::dtMax ),
	 dt_inf( options::dtInf ),
	 dt_inf_rlx( options::dtInf == infinity ? infinity : 0.5 * options::dtInf ),
	 var( var ),
	 der( der )
	{}

	// Name Constructor
	explicit
	Variable(
	 std::string const & name,
	 FMU_Variable const var = FMU_Variable(),
	 FMU_Variable const der = FMU_Variable()
	) :
	 name( name ),
	 xIni( 0.0 ),
	 dt_min( options::dtMin ),
	 dt_max( options::dtMax ),
	 dt_inf( options::dtInf ),
	 dt_inf_rlx( options::dtInf == infinity ? infinity : 0.5 * options::dtInf ),
	 var( var ),
	 der( der )
	{}

	// Copy Constructor
	Variable( Variable const & ) = default;

	// Move Constructor
	Variable( Variable && ) noexcept = default;

public: // Creation

	// Destructor
	virtual
	~Variable()
	{}

protected: // Assignment

	// Copy Assignment
	Variable &
	operator =( Variable const & ) = default;

	// Move Assignment
	Variable &
	operator =( Variable && ) noexcept = default;

public: // Predicate

	// Discrete Variable?
	virtual
	bool
	is_Discrete() const
	{ // Default implementation
		return false;
	}

	// Input Variable?
	virtual
	bool
	is_Input() const
	{ // Default implementation
		return false;
	}

	// QSS Variable?
	virtual
	bool
	is_QSS() const
	{ // Default implementation
		return false;
	}

	// Zero-Crossing Variable?
	virtual
	bool
	is_ZC() const
	{ // Default implementation
		return false;
	}

public: // Properties

	// Order of Method
	virtual
	int
	order() const = 0;

	// Max Observer Order
	int
	observers_max_order() const
	{
		return observers_max_order_;
	}

	// Continuous Value at Time t
	virtual
	Value
	x( Time const t ) const = 0;

	// Continuous First Derivative at Time t
	virtual
	Value
	x1( Time const t ) const = 0;

	// Continuous Second Derivative at Time t
	virtual
	Value
	x2( Time const ) const
	{
		return 0.0;
	}

	// Continuous Third Derivative at Time t
	virtual
	Value
	x3( Time const ) const
	{
		return 0.0;
	}

	// Quantized Value at Time t
	virtual
	Value
	q( Time const t ) const = 0;

	// Quantized First Derivative at Time t
	virtual
	Value
	q1( Time const ) const
	{
		return 0.0;
	}

	// Quantized Second Derivative at Time t
	virtual
	Value
	q2( Time const ) const
	{
		return 0.0;
	}

	// Simultaneous Value at Time t
	virtual
	Value
	s( Time const ) const
	{
		assert( false ); // Missing override
		return 0.0;
	}

	// Simultaneous Numeric Differentiation Value at Time t
	virtual
	Value
	sn( Time const ) const
	{
		assert( false ); // Missing override
		return 0.0;
	}

	// Simultaneous First Derivative at Time t
	virtual
	Value
	s1( Time const ) const
	{
		return 0.0;
	}

	// Simultaneous Second Derivative at Time t
	virtual
	Value
	s2( Time const ) const
	{
		return 0.0;
	}

	// Observers
	Variables const &
	observers() const
	{
		return observers_;
	}

	// Observers
	Variables &
	observers()
	{
		return observers_;
	}

	// Observees
	Variables const &
	observees() const
	{
		return observees_;
	}

	// Observees
	Variables &
	observees()
	{
		return observees_;
	}

	// Event Queue Iterator
	EventQ::iterator &
	event()
	{
		return event_;
	}

	// Event Queue Iterator Assignment
	void
	event( EventQ::iterator const i )
	{
		event_ = i;
		assert( event_->second.var() == this );
	}

	// Zero-Crossing Time
	virtual
	Time
	tZC() const
	{
		assert( false ); // Not a ZC variable
		return Time( 0.0 );
	}

public: // Methods

	// Set Max Time Step
	void
	set_dt_min( Time const dt )
	{
		assert( dt >= 0.0 );
		dt_min = dt;
	}

	// Set Max Time Step
	void
	set_dt_max( Time const dt )
	{
		assert( dt > 0.0 );
		dt_max = dt;
	}

	// Add Observer
	void
	add_observer( Variable & v )
	{
		if ( &v != this ) observers_.push_back( &v ); // Don't need to self-observe: Observers called at the end of self requantization
	}

	// Add Observer
	void
	add_observer( Variable * v )
	{
		if ( v != this ) observers_.push_back( v ); // Don't need to self-observe: Observers called at the end of self requantization
	}

	// Shrink Observers Collection
	void
	shrink_observers()
	{
		observers_.shrink_to_fit();
	}

	// Sort Observers Collection by Order
	void
	sort_observers()
	{
		std::sort( observers_.begin(), observers_.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } );
		iBeg_observers_2_ = static_cast< size_type >( std::distance( observers_.begin(), std::find_if( observers_.begin(), observers_.end(), []( Variable * v ){ return v->order() >= 2; } ) ) );
		observers_max_order_ = ( observers_.empty() ? 0 : observers_.back()->order() );
	}

	// Add Observee
	void
	add_observee( Variable & v )
	{
		if ( &v != this ) observees_.push_back( &v ); // Don't need to self-observe
	}

	// Add Observee
	void
	add_observee( Variable * v )
	{
		if ( v != this ) observees_.push_back( v ); // Don't need to self-observe
	}

	// Shrink Observees Collection
	void
	shrink_observees() // May be worth calling after all observees added to improve memory and cache use
	{
		observees_.shrink_to_fit();
	}

	// Add Handler Event
	void
	add_handler()
	{
		event_ = events.add_handler( this );
	}

	// Initialization
	virtual
	void
	init()
	{}

	// Initialization to a Value
	virtual
	void
	init( Value const )
	{}

	// Initialization: Observers
	void
	init_observers()
	{
		shrink_observers(); // Optional
		shrink_observees(); // Optional
		sort_observers();

		// Observers observees setup
		std::unordered_set< Variable * > oo2s; // Observees of observers of order 2+
		for ( auto observer : observers_ ) {
			if ( observer->order() >= 2 ) {
				if ( observer->self_observer ) oo2s.insert( observer );
				for ( auto observee : observer->observees_ ) {
					oo2s.insert( observee );
				}
			}
		}
		std::unordered_set< Variable * > oo1s; // Observees of observers of order <=1 not in oo2s
		for ( auto observer : observers_ ) {
			if ( observer->order() <= 1 ) {
				if ( ( observer->self_observer ) && ( oo2s.find( observer ) == oo2s.end() ) ) oo1s.insert( observer );
				for ( auto observee : observer->observees_ ) {
					if ( oo2s.find( observee ) == oo2s.end() ) oo1s.insert( observee );
				}
			}
		}
		observers_observees_.clear();
		observers_observees_.reserve( oo1s.size() + oo2s.size() );
		for ( auto observee : oo1s ) {
			observers_observees_.push_back( observee );
		}
		for ( auto observee : oo2s ) {
			observers_observees_.push_back( observee );
		}
		iBeg_observers_2_observees_ = oo1s.size();
	}

	// Initialization: Stage 0
	virtual
	void
	init_0()
	{}

	// Initialization to a Value: Stage 0
	virtual
	void
	init_0( Value const )
	{}

	// Initialization: Stage 1
	virtual
	void
	init_1()
	{}

	// Initialization: LIQSS Stage 2
	virtual
	void
	init_LIQSS_2()
	{}

	// Initialization: Stage 2
	virtual
	void
	init_2()
	{}

	// Discrete Advance
	virtual
	void
	advance_discrete()
	{
		assert( false );
	}

	// Discrete Advance: Stages 0 and 1
	virtual
	void
	advance_discrete_0_1()
	{
		assert( false );
	}

	// Discrete Advance: Stage 2
	virtual
	void
	advance_discrete_2()
	{}

	// QSS Advance
	virtual
	void
	advance_QSS()
	{
		assert( false );
	}

	// QSS Advance: Stage 0
	virtual
	void
	advance_QSS_0()
	{
		assert( false );
	}

	// QSS Advance: Stage 1
	virtual
	void
	advance_QSS_1()
	{
		assert( false );
	}

	// QSS Advance: Stage 2
	virtual
	void
	advance_QSS_2()
	{}

	// Advance Observers: Stage 1
	void
	advance_observers_1()
	{
		fmu_set_observers_observees_q( tQ );
		for ( Variable * observer : observers_ ) {
			observer->advance_observer_1( tQ );
		}
	}

	// Advance Observers: Stage 2
	void
	advance_observers_2()
	{
		assert( tN == tQ + options::dtNum );
		fmu_set_observers_2_observees_q( tN );
		for ( size_type i = iBeg_observers_2_, n = observers_.size(); i < n; ++i ) { // Order 2+ observers
			observers_[ i ]->advance_observer_2();
		}
	}

	// Advance Observers: Stage d
	void
	advance_observers_d()
	{
		assert( options::output::d );
		for ( Variable * observer : observers_ ) {
			observer->advance_observer_d();
		}
	}

	// Observer Advance
	virtual
	void
	advance_observer( Time const )
	{
		assert( false ); // Not a QSS or ZC variable
	}

	// Observer Advance: Stage 1
	virtual
	void
	advance_observer_1( Time const )
	{
		assert( false );
	}

	// Observer Advance: Stage 2
	virtual
	void
	advance_observer_2()
	{
		assert( false );
	}

	// Observer Advance: Simultaneous Stage 1
	void
	advance_observer_simultaneous_1( Time const t )
	{
		fmu_set_observees_q( t );
		if ( self_observer ) fmu_set_q( t );
		advance_observer_1( t );
	}

	// Observer Advance: Simultaneous Stage 2
	void
	advance_observer_simultaneous_2( Time const t )
	{
		assert( order() >= 2 );
		fmu_set_observees_q( t );
		if ( self_observer ) fmu_set_q( t );
		advance_observer_2();
	}

	// Observer Advance: Stage d
	virtual
	void
	advance_observer_d()
	{
		assert( false );
	}

	// Zero-Crossing Advance
	virtual
	void
	advance_ZC()
	{
		assert( false ); // Not a ZC variable
	}

	// Handler Advance
	virtual
	void
	advance_handler( Time const )
	{
		assert( false ); // Not a QSS or Discrete variable
	}

	// Handler Advance: Stage 0
	virtual
	void
	advance_handler_0( Time const )
	{
		assert( false ); // Not a QSS or Discrete variable
	}

	// Handler Advance: Stage 1
	virtual
	void
	advance_handler_1()
	{
		assert( false ); // Not a QSS variable
	}

	// Handler Advance: Stage 2
	virtual
	void
	advance_handler_2()
	{
		assert( false ); // Not a QSS variable
	}

	// Shift Handler to Time t
	void
	shift_handler( Time const t )
	{
		event_ = events.shift_handler( t, event_ );
	}

	// Shift Handler to Time Infinity
	void
	shift_handler()
	{
		event_ = events.shift_handler( event_ );
	}

public: // Methods: FMU

	// Get FMU Variable Value
	Value
	fmu_get_value() const
	{
		return fmu::get_real( var.ref );
	}

	// Get FMU Variable Derivative
	Value
	fmu_get_deriv() const
	{
		return fmu::get_real( der.ref );
	}

	// Set FMU Variable to a Value
	void
	fmu_set_value( Value const v ) const
	{
		fmu::set_real( var.ref, v );
	}

	// Set FMU Variable to Continuous Value at Time t
	void
	fmu_set_x( Time const t ) const
	{
		fmu::set_real( var.ref, x( t ) );
	}

	// Set FMU Variable to Quantized Value at Time t
	void
	fmu_set_q( Time const t ) const
	{
		fmu::set_real( var.ref, q( t ) );
	}

	// Set FMU Variable to Simultaneous Value at Time t
	void
	fmu_set_s( Time const t ) const
	{
		fmu::set_real( var.ref, s( t ) );
	}

	// Set FMU Variable to Simultaneous Numeric Differentiation Value at Time t
	void
	fmu_set_sn( Time const t ) const
	{
		fmu::set_real( var.ref, sn( t ) );
	}

	// Get FMU Integer Variable Value
	Integer
	fmu_get_integer_value() const
	{
		return fmu::get_integer( var.ref );
	}

	// Set FMU Integer Variable to a Value
	void
	fmu_set_integer_value( Integer const v ) const
	{
		fmu::set_integer( var.ref, v );
	}

	// Get FMU Boolean Variable Value
	bool
	fmu_get_boolean_value() const
	{
		return fmu::get_boolean( var.ref );
	}

	// Set FMU Boolean Variable to a Value
	void
	fmu_set_boolean_value( bool const v ) const
	{
		fmu::set_boolean( var.ref, v );
	}

	// Set All Observee FMU Variables to Continuous Value at Time t
	void
	fmu_set_observees_x( Time const t ) const
	{
		for ( auto observee : observees_ ) {
			if ( ! observee->is_Discrete() ) observee->fmu_set_x( t );
		}
	}

	// Set All Observee FMU Variables to Quantized Value at Time t
	void
	fmu_set_observees_q( Time const t ) const
	{
		for ( auto observee : observees_ ) {
			if ( ! observee->is_Discrete() ) observee->fmu_set_q( t );
		}
	}

	// Set All Observee FMU Variables to Simultaneous Value at Time t
	void
	fmu_set_observees_s( Time const t ) const
	{
		for ( auto observee : observees_ ) {
			if ( ! observee->is_Discrete() ) observee->fmu_set_s( t );
		}
	}

	// Set All Observee FMU Variables to Simultaneous Numeric Differentiation Value at Time t
	void
	fmu_set_observees_sn( Time const t ) const
	{
		for ( auto observee : observees_ ) {
			if ( ! observee->is_Discrete() ) observee->fmu_set_sn( t );
		}
	}

	// Set All Observers Observee FMU Variables to Quantized Value at Time t
	void
	fmu_set_observers_observees_q( Time const t ) const
	{
		for ( auto observee : observers_observees_ ) {
			if ( ! observee->is_Discrete() ) observee->fmu_set_q( t );
		}
	}

	// Set All Order 2+ Observers Observee FMU Variables to Quantized Value at Time t
	void
	fmu_set_observers_2_observees_q( Time const t ) const
	{
		for ( size_type i = iBeg_observers_2_observees_, n = observers_observees_.size(); i < n; ++i ) {
			auto & observee( observers_observees_[ i ] );
			if ( ! observee->is_Discrete() ) observee->fmu_set_q( t );
		}
	}

protected: // Methods

	// Infinite Aligned Time Step Processing
	void
	tE_infinity_tQ()
	{
		if ( dt_inf != infinity ) { // Deactivation control is enabled
			if ( tE == infinity ) { // Deactivation has occurred
				if ( dt_inf_rlx < half_infinity ) { // Relax and use deactivation time step
					dt_inf_rlx *= 2.0;
					tE = tQ + dt_inf_rlx;
				}
			} else { // Reset deactivation time step
				dt_inf_rlx = dt_inf;
			}
		}
	}

	// Infinite Unaligned Time Step Processing
	void
	tE_infinity_tX()
	{
		if ( dt_inf != infinity ) { // Deactivation control is enabled
			if ( tE == infinity ) { // Deactivation has occurred
				if ( dt_inf_rlx < half_infinity ) { // Relax and use deactivation time step
					dt_inf_rlx *= 2.0;
					tE = tX + dt_inf_rlx;
				}
			} else { // Reset deactivation time step
				dt_inf_rlx = dt_inf;
			}
		}
	}

public: // Static Data

	static int const max_order = 3; // Max QSS order supported

public: // Data

	std::string name;
	Value rTol{ 1.0e-4 }; // Relative tolerance
	Value aTol{ 1.0e-6 }; // Absolute tolerance
	Value qTol{ 1.0e-6 }; // Quantization tolerance
	Value xIni{ 0.0 }; // Initial value
	Time tQ{ 0.0 }; // Quantized time range begin
	Time tX{ 0.0 }; // Continuous time range begin
	Time tE{ 0.0 }; // Time range end: tQ <= tE and tX <= tE
	Time tN{ 0.0 }; // Numeric differentiation time
	Time tD{ infinity }; // Discrete event time: tQ <= tD and tX <= tD
	Time dt_min{ 0.0 }; // Time step min
	Time dt_max{ infinity }; // Time step max
	Time dt_inf{ infinity }; // Time step inf
	Time dt_inf_rlx{ infinity }; // Relaxed time step inf
	SuperdenseTime sT; // Trigger superdense time
	bool self_observer{ false }; // Variable appears in its function/derivative?
	FMU_Variable var; // FMU variables specs
	FMU_Variable der; // FMU derivative specs

protected: // Data

	int observers_max_order_{ 0 }; // Max QSS order of observers
	Variables observers_; // Variables dependent on this one
	size_type iBeg_observers_2_{ 0 }; // Index of first observer of order 2+
	Variables observees_; // Variables this one depends on
	Variables observers_observees_; // Observers observees (including self-observing observers)
	size_type iBeg_observers_2_observees_{ 0 }; // Index of first observee of observer of order 2+
	EventQ::iterator event_; // Iterator to event queue entry

};

} // fmu
} // QSS

#endif
