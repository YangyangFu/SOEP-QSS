// QSS Defined Model Simulation Runner
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

// QSS Headers
#include <QSS/dfn/simulate_dfn.hh>
#include <QSS/dfn/globals_dfn.hh>
#include <QSS/dfn/Variable.hh>
#include <QSS/dfn/Variable_ZC.hh>
#include <QSS/dfn/mdl/achilles.hh>
#include <QSS/dfn/mdl/achilles2.hh>
#include <QSS/dfn/mdl/achillesc.hh>
#include <QSS/dfn/mdl/achilles_ND.hh>
#include <QSS/dfn/mdl/bball.hh>
#include <QSS/dfn/mdl/exponential_decay.hh>
#include <QSS/dfn/mdl/exponential_decay_sine.hh>
#include <QSS/dfn/mdl/exponential_decay_sine_ND.hh>
#include <QSS/dfn/mdl/exponential_decay_step.hh>
#include <QSS/dfn/mdl/nonlinear.hh>
#include <QSS/dfn/mdl/nonlinear_ND.hh>
#include <QSS/dfn/mdl/StateEvent6.hh>
#include <QSS/dfn/mdl/stiff.hh>
#include <QSS/dfn/mdl/xy.hh>
#include <QSS/dfn/mdl/xyz.hh>
#include <QSS/options.hh>

// C++ Headers
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace QSS {
namespace dfn {

// Simulate a Defined Model
void
simulate_dfn()
{
	// Types
	using Variables = Variable::Variables;
	using size_type = Variables::size_type;
	using Events = EventQueue< Variable >::Events;
	using Time = Variable::Time;
	using Var_Idx = std::unordered_map< Variable const *, size_type >; // Map from Variables to their indexes
	using VariableLookup = std::unordered_set< Variable * >; // Fast Variable lookup container
	using ObserversSet = std::unordered_set< Variable * >; // Simultaneous trigger observers collection

	// I/o setup
	std::cout << std::setprecision( 16 );
	std::cerr << std::setprecision( 16 );
	std::vector< std::ofstream > x_streams; // Continuous output streams
	std::vector< std::ofstream > q_streams; // Quantized output streams

	// Variables collection
	Variables vars;

	// Model setup
	if ( options::model == "achilles" ) {
		mdl::achilles( vars );
	} else if ( options::model== "achilles2" ) {
		mdl::achilles2( vars );
	} else if ( options::model== "achillesc" ) {
		mdl::achillesc( vars );
	} else if ( options::model== "achilles_ND" ) {
		mdl::achilles_ND( vars );
	} else if ( options::model== "bball" ) {
		mdl::bball( vars );
	} else if ( options::model== "exponential_decay" ) {
		mdl::exponential_decay( vars );
	} else if ( options::model== "exponential_decay_sine" ) {
		mdl::exponential_decay_sine( vars );
	} else if ( options::model== "exponential_decay_sine_ND" ) {
		mdl::exponential_decay_sine_ND( vars );
	} else if ( options::model== "exponential_decay_step" ) {
		mdl::exponential_decay_step( vars );
	} else if ( options::model== "nonlinear" ) {
		mdl::nonlinear( vars );
	} else if ( options::model== "nonlinear_ND" ) {
		mdl::nonlinear_ND( vars );
	} else if ( options::model== "stiff" ) {
		mdl::stiff( vars );
	} else if ( ( options::model== "StateEvent6" ) || ( options::model== "stateevent6" ) ) {
		mdl::StateEvent6( vars );
	} else if ( options::model== "xy" ) {
		mdl::xy( vars );
	} else if ( options::model== "xyz" ) {
		mdl::xyz( vars );
	} else {
		std::cerr << "Error: Unknown model: " << options::model << std::endl;
		std::exit( EXIT_FAILURE );
	}

	// Size setup
	size_type const n_vars( vars.size() );

	// Variable-index map setup
	Var_Idx var_idx;
	for ( size_type i = 0; i < n_vars; ++i ) {
		var_idx[ vars[ i ] ] = i;
	}

	// Containers of ZC and non-ZC variables
	Variables vars_ZC;
	Variables vars_nonZC;
	int max_QSS_order( 0 );
	for ( auto var : vars ) {
		if ( var->is_ZC() ) { // ZC variable
			vars_ZC.push_back( var );
		} else { // Non-ZC variable
			vars_nonZC.push_back( var );
			max_QSS_order = std::max( max_QSS_order, var->order() ); // Max QSS order of non-ZC variables to avoid unnec loop stages
		}
	}
	int const QSS_order_max( max_QSS_order ); // Highest QSS order in use
	assert( QSS_order_max <= 3 );

	// Timing setup
	Time const t0( 0.0 ); // Simulation start time
	Time const tE( options::tEnd ); // Simulation end time
	Time t( t0 ); // Simulation current time
	Time tOut( t0 + options::dtOut ); // Sampling time
	size_type iOut( 1u ); // Output step index

	// Variable initialization
	std::cout << "\nInitialization =====" << std::endl;
	for ( auto var : vars_nonZC ) {
		var->init_0();
	}
	for ( auto var : vars_nonZC ) {
		var->init_1();
	}
	if ( QSS_order_max >= 2 ) {
		for ( auto var : vars_nonZC ) {
			var->init_2();
		}
		if ( QSS_order_max >= 3 ) {
			for ( auto var : vars_nonZC ) {
				var->init_3();
			}
		}
	}
	for ( auto var : vars_ZC ) { // ZC variables after to get actual LIQSS2+ quantized reps
		var->init();
	}

	// Output stream initialization
	bool const doSOut( options::output::s && ( options::output::x || options::output::q ) );
	bool const doTOut( options::output::t && ( options::output::x || options::output::q ) );
	bool const doROut( options::output::r && ( options::output::x || options::output::q ) );
	if ( ( options::output::t || options::output::r || options::output::s ) && ( options::output::x || options::output::q ) ) { // t0 QSS outputs
		for ( auto var : vars ) { // QSS outputs
			if ( options::output::x ) {
				x_streams.push_back( std::ofstream( var->name + ".x.out", std::ios_base::binary | std::ios_base::out ) );
				x_streams.back() << std::setprecision( 16 ) << t << '\t' << var->x( t ) << '\n';
			}
			if ( options::output::q ) {
				q_streams.push_back( std::ofstream( var->name + ".q.out", std::ios_base::binary | std::ios_base::out ) );
				q_streams.back() << std::setprecision( 16 ) << t << '\t' << var->q( t ) << '\n';
			}
		}
	}

	// Simulation loop
	std::cout << "\nSimulation Loop =====" << std::endl;
	size_type n_discrete_events( 0 );
	size_type n_QSS_events( 0 );
	size_type n_QSS_simultaneous_events( 0 );
	size_type n_ZC_events( 0 );
	while ( t <= tE ) {
		t = events.top_time();
		if ( doSOut ) { // Sampled outputs
			Time const tStop( std::min( t, tE ) );
			while ( tOut < tStop ) {
				for ( size_type i = 0; i < n_vars; ++i ) {
					if ( options::output::x ) x_streams[ i ] << tOut << '\t' << vars[ i ]->x( tOut ) << '\n';
					if ( options::output::q ) q_streams[ i ] << tOut << '\t' << vars[ i ]->q( tOut ) << '\n';
				}
				assert( iOut < std::numeric_limits< size_type >::max() );
				tOut = t0 + ( ++iOut ) * options::dtOut;
			}
		}
		if ( t <= tE ) { // Perform event(s)
			Event< Variable > & event( events.top() );
			SuperdenseTime const & s( events.top_superdense_time() );
			events.set_active_time();
			if ( event.is_discrete() ) { // Discrete event
				++n_discrete_events;
				if ( events.single() ) { // Single trigger
					Variable * trigger( events.top_var() );
					assert( trigger->tD == t );
					if ( doTOut ) { // Time event variable output: Before discontinuous discrete changes
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Time event and/or observer variable output
							if ( options::output::t ) { // Time event variable output
								size_type const i( var_idx[ trigger ] );
								if ( options::output::x ) x_streams[ i ] << t << '\t' << trigger->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << trigger->q( t ) << '\n';
								for ( Variable const * observer : trigger->observers() ) {
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : trigger->observers() ) { // Zero-crossing observer output
									if ( ( ! options::output::t ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::t
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
					trigger->advance_discrete();
					if ( doTOut ) { // Time event variable output
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Time event and/or observer variable output
							if ( options::output::t ) { // Time event variable output
								size_type const i( var_idx[ trigger ] );
								if ( options::output::x ) x_streams[ i ] << t << '\t' << trigger->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << trigger->q( t ) << '\n';
								for ( Variable const * observer : trigger->observers() ) {
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : trigger->observers() ) { // Zero-crossing observer output
									if ( ( ! options::output::t ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::t
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
				} else { // Simultaneous triggers
					Variables triggers( events.top_vars() );
					std::sort( triggers.begin(), triggers.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } ); // Sort triggers by order
					for ( Variable * trigger : triggers ) {
						trigger->sT = s; // Set trigger superdense time
					}
					size_type const iBeg_triggers_2( static_cast< size_type >( std::distance( triggers.begin(), std::find_if( triggers.begin(), triggers.end(), []( Variable * v ){ return v->order() >= 2; } ) ) ) );
					size_type const iBeg_triggers_3( static_cast< size_type >( std::distance( triggers.begin(), std::find_if( triggers.begin(), triggers.end(), []( Variable * v ){ return v->order() >= 3; } ) ) ) );
					int const triggers_order_max( triggers.empty() ? 0 : triggers.back()->order() );
					VariableLookup const var_lookup( triggers.begin(), triggers.end() );
					ObserversSet observers_set;
					for ( Variable * trigger : triggers ) { // Collect observers to avoid duplicate advance calls
						for ( Variable * observer : trigger->observers() ) {
							if ( var_lookup.find( observer ) == var_lookup.end() ) observers_set.insert( observer ); // Skip triggers
						}
					}
					Variables observers( observers_set.begin(), observers_set.end() );
					std::sort( observers.begin(), observers.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } ); // Sort observers by order
					size_type const iBeg_observers_2( static_cast< size_type >( std::distance( observers.begin(), std::find_if( observers.begin(), observers.end(), []( Variable * v ){ return v->order() >= 2; } ) ) ) );
					int const order_max( observers.empty() ? triggers_order_max : std::max( triggers_order_max, observers.back()->order() ) );
					if ( doTOut ) { // Time event output: Before discontinuous discrete changes
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Time event and/or observer variable output
							if ( options::output::t ) { // Time event variable output
								for ( Variable const * trigger : triggers ) { // Triggers
									size_type const i( var_idx[ trigger ] );
									if ( options::output::x ) x_streams[ i ] << t << '\t' << trigger->x( t ) << '\n';
									if ( options::output::q ) q_streams[ i ] << t << '\t' << trigger->q( t ) << '\n';
								}
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( ( ! options::output::t ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::t
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
					for ( Variable * trigger : triggers ) {
						assert( trigger->tD == t );
						trigger->advance_discrete_0_1();
					}
					if ( order_max >= 2 ) { // 2nd order pass
						for ( size_type i = iBeg_triggers_2, n = triggers.size(); i < n; ++i ) {
							triggers[ i ]->advance_discrete_2();
						}
						if ( order_max >= 3 ) { // 3rd order pass
							for ( size_type i = iBeg_triggers_3, n = triggers.size(); i < n; ++i ) {
								triggers[ i ]->advance_discrete_3();
							}
						}
					}
					for ( Variable * observer : observers ) {
						observer->advance_observer( t );
					}
					if ( doTOut ) { // Time event output
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Time event and/or observer variable output
							if ( options::output::t ) { // Time event variable output
								for ( Variable const * trigger : triggers ) { // Triggers
									size_type const i( var_idx[ trigger ] );
									if ( options::output::x ) x_streams[ i ] << t << '\t' << trigger->x( t ) << '\n';
									if ( options::output::q ) q_streams[ i ] << t << '\t' << trigger->q( t ) << '\n';
								}
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( ( ! options::output::t ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::t
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
				}
			} else if ( event.is_QSS() ) { // QSS requantization event
				++n_QSS_events;
				if ( events.single() ) { // Single trigger
					Variable * trigger( events.top_var() );
					assert( trigger->tE == t );
					trigger->advance_QSS();
					if ( doROut ) { // Requantization output
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Requantization and/or observer variable output
							if ( options::output::r ) { // Requantization variable output
								size_type const i( var_idx[ trigger ] );
								if ( options::output::x ) x_streams[ i ] << t << '\t' << trigger->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << trigger->q( t ) << '\n';
								for ( Variable const * observer : trigger->observers() ) {
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : trigger->observers() ) { // Zero-crossing observer output
									if ( ( ! options::output::r ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::r
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
				} else { // Simultaneous triggers
					++n_QSS_simultaneous_events;
					Variables triggers( events.top_vars() );
					std::sort( triggers.begin(), triggers.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } ); // Sort triggers by order
					Variables triggers_ZC;
					Variables triggers_nonZC;
					for ( Variable * trigger : triggers ) {
						if ( trigger->is_ZC() ) { // ZC variable
							triggers_ZC.push_back( trigger );
						} else { // Non-ZC variable
							triggers_nonZC.push_back( trigger );
						}
						trigger->sT = s; // Set trigger superdense time
					}
					size_type const iBeg_triggers_nonZC_2( static_cast< size_type >( std::distance( triggers_nonZC.begin(), std::find_if( triggers_nonZC.begin(), triggers_nonZC.end(), []( Variable * v ){ return v->order() >= 2; } ) ) ) );
					size_type const iBeg_triggers_nonZC_3( static_cast< size_type >( std::distance( triggers_nonZC.begin(), std::find_if( triggers_nonZC.begin(), triggers_nonZC.end(), []( Variable * v ){ return v->order() >= 3; } ) ) ) );
					int const triggers_nonZC_order_max( triggers_nonZC.empty() ? 0 : triggers_nonZC.back()->order() );
					VariableLookup const var_lookup( triggers_nonZC.begin(), triggers_nonZC.end() );
					ObserversSet observers_set;
					for ( Variable * trigger : triggers_nonZC ) { // Collect observers to avoid duplicate advance calls
						for ( Variable * observer : trigger->observers() ) {
							if ( var_lookup.find( observer ) == var_lookup.end() ) observers_set.insert( observer ); // Skip triggers
						}
					}
					Variables observers( observers_set.begin(), observers_set.end() );
					std::sort( observers.begin(), observers.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } ); // Sort observers by order
					size_type const iBeg_observers_2( static_cast< size_type >( std::distance( observers.begin(), std::find_if( observers.begin(), observers.end(), []( Variable * v ){ return v->order() >= 2; } ) ) ) );
					int const nonZC_order_max( observers.empty() ? triggers_nonZC_order_max : std::max( triggers_nonZC_order_max, observers.back()->order() ) );
					for ( Variable * trigger : triggers_nonZC ) {
						assert( trigger->tE == t );
						trigger->advance_QSS_0();
					}
					for ( Variable * trigger : triggers_nonZC ) {
						trigger->advance_QSS_1();
					}
					if ( nonZC_order_max >= 2 ) { // 2nd order pass
						for ( size_type i = iBeg_triggers_nonZC_2, n = triggers_nonZC.size(); i < n; ++i ) {
							triggers_nonZC[ i ]->advance_QSS_2();
						}
						if ( nonZC_order_max >= 3 ) { // 3rd order pass
							for ( size_type i = iBeg_triggers_nonZC_3, n = triggers_nonZC.size(); i < n; ++i ) {
								triggers_nonZC[ i ]->advance_QSS_3();
							}
						}
					}
					for ( Variable * trigger : triggers_ZC ) {
						assert( trigger->tE == t );
						trigger->advance_QSS_simultaneous();
					}
					for ( Variable * observer : observers ) {
						observer->advance_observer( t );
					}
					if ( doROut ) { // Requantization output
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Requantization and/or observer variable output
							if ( options::output::r ) { // Requantization variable output
								for ( Variable const * trigger : triggers ) { // Triggers
									size_type const i( var_idx[ trigger ] );
									if ( options::output::x ) x_streams[ i ] << t << '\t' << trigger->x( t ) << '\n';
									if ( options::output::q ) q_streams[ i ] << t << '\t' << trigger->q( t ) << '\n';
								}
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( ( ! options::output::r ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::r
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
				}
			} else if ( event.is_ZC() ) { // Zero-crossing event
				++n_ZC_events;
				while ( events.top_superdense_time() == s ) {
					Variable * trigger( events.top_var() );
					assert( trigger->tZC() == t );
					trigger->advance_ZC();
				}
			} else if ( event.is_handler() ) { // Zero-crossing handler event
				if ( events.single() ) { // Single handler
					if ( doROut ) { // Requantization output: Before discontinuous handler changes
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Requantization and/or observer variable output
							Variable const * handler( event.var() );
							if ( options::output::r ) { // Requantization variable output
								size_type const i( var_idx[ handler ] );
								if ( options::output::x ) x_streams[ i ] << t << '\t' << handler->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << handler->q( t ) << '\n';
								for ( Variable const * observer : handler->observers() ) {
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : handler->observers() ) { // Zero-crossing observer output
									if ( ( ! options::output::r ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::r
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
					event.var()->advance_handler( t, event.val() );
					if ( doROut ) { // Requantization output
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Requantization and/or observer variable output
							Variable const * handler( event.var() );
							if ( options::output::r ) { // Requantization variable output
								size_type const i( var_idx[ handler ] );
								if ( options::output::x ) x_streams[ i ] << t << '\t' << handler->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << handler->q( t ) << '\n';
								for ( Variable const * observer : handler->observers() ) {
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : handler->observers() ) { // Zero-crossing observer output
									if ( ( ! options::output::r ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::r
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
				} else { // Simultaneous handlers
					Events tops( events.top_events() );
					Variables handlers;
					handlers.reserve( tops.size() );
					for ( auto & e : tops ) handlers.push_back( e.var() );
					std::sort( handlers.begin(), handlers.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } ); // Sort handlers by order
					size_type const iBeg_handlers_1( static_cast< size_type >( std::distance( handlers.begin(), std::find_if( handlers.begin(), handlers.end(), []( Variable * v ){ return v->order() >= 1; } ) ) ) );
					size_type const iBeg_handlers_2( static_cast< size_type >( std::distance( handlers.begin(), std::find_if( handlers.begin(), handlers.end(), []( Variable * v ){ return v->order() >= 2; } ) ) ) );
					size_type const iBeg_handlers_3( static_cast< size_type >( std::distance( handlers.begin(), std::find_if( handlers.begin(), handlers.end(), []( Variable * v ){ return v->order() >= 3; } ) ) ) );
					int const handlers_order_max( handlers.empty() ? 0 : handlers.back()->order() );
					VariableLookup const var_lookup( handlers.begin(), handlers.end() );
					ObserversSet observers_set;
					for ( Variable * handler : handlers ) { // Collect observers to avoid duplicate advance calls
						for ( Variable * observer : handler->observers() ) {
							if ( var_lookup.find( observer ) == var_lookup.end() ) observers_set.insert( observer ); // Skip handlers
						}
					}
					Variables observers( observers_set.begin(), observers_set.end() );
					std::sort( observers.begin(), observers.end(), []( Variable * v1, Variable * v2 ){ return v1->order() < v2->order(); } ); // Sort observers by order
					if ( doROut ) { // Requantization output: Before discontinuous handler changes
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Requantization and/or observer variable output
							if ( options::output::r ) { // Requantization variable output
								for ( Variable const * handler : handlers ) {
									size_type const i( var_idx[ handler ] );
									if ( options::output::x ) x_streams[ i ] << t << '\t' << handler->x( t ) << '\n';
									if ( options::output::q ) q_streams[ i ] << t << '\t' << handler->q( t ) << '\n';
								}
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( ( ! options::output::r ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::r
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
					for ( auto & e : tops ) {
						e.var()->advance_handler_0( t, e.val() );
					}
					for ( size_type i = iBeg_handlers_1, n = handlers.size(); i < n; ++i ) {
						handlers[ i ]->advance_handler_1();
					}
					if ( handlers_order_max >= 2 ) { // 2nd order pass
						for ( size_type i = iBeg_handlers_2, n = handlers.size(); i < n; ++i ) {
							handlers[ i ]->advance_handler_2();
						}
						if ( handlers_order_max >= 3 ) { // 3rd order pass
							for ( size_type i = iBeg_handlers_3, n = handlers.size(); i < n; ++i ) {
								handlers[ i ]->advance_handler_3();
							}
						}
					}
					for ( Variable * observer : observers ) {
						observer->advance_observer( t );
					}
					if ( doROut ) { // Requantization output
						if ( options::output::a ) { // All variables output
							for ( size_type i = 0; i < n_vars; ++i ) {
								if ( options::output::x ) x_streams[ i ] << t << '\t' << vars[ i ]->x( t ) << '\n';
								if ( options::output::q ) q_streams[ i ] << t << '\t' << vars[ i ]->q( t ) << '\n';
							}
						} else { // Requantization and/or observer variable output
							if ( options::output::r ) { // Requantization variable output
								for ( Variable const * handler : handlers ) {
									size_type const i( var_idx[ handler ] );
									if ( options::output::x ) x_streams[ i ] << t << '\t' << handler->x( t ) << '\n';
									if ( options::output::q ) q_streams[ i ] << t << '\t' << handler->q( t ) << '\n';
								}
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( observer->is_ZC() ) { // Zero-crossing variables requantize in observer advance
										size_type const i( var_idx[ observer ] );
										if ( options::output::x ) x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
										if ( options::output::q ) q_streams[ i ] << t << '\t' << observer->q( t ) << '\n';
									}
								}
							}
							if ( ( options::output::o ) && ( options::output::x ) ) { // Observer variable output
								for ( Variable const * observer : observers ) { // Zero-crossing observer output
									if ( ( ! options::output::r ) || ( ! observer->is_ZC() ) ) { // ZC observers output above if options::output::r
										size_type const i( var_idx[ observer ] );
										x_streams[ i ] << t << '\t' << observer->x( t ) << '\n';
									}
								}
							}
						}
					}
				}
			} else { // Unsupported event
				assert( false );
			}
		}
	}

	// End time outputs and streams close
	if ( ( options::output::r || options::output::s ) && ( options::output::x || options::output::q ) ) {
		for ( size_type i = 0; i < n_vars; ++i ) {
			Variable const * var( vars[ i ] );
			if ( var->tQ < tE ) {
				if ( options::output::x ) {
					x_streams[ i ] << tE << '\t' << var->x( tE ) << '\n';
					x_streams[ i ].close();
				}
				if ( options::output::q ) {
					q_streams[ i ] << tE << '\t' << var->q( tE ) << '\n';
					q_streams[ i ].close();
				}
			}
		}
	}

	// Reporting
	std::cout << "\nSimulation Complete =====" << std::endl;
	if ( n_discrete_events > 0 ) std::cout << n_discrete_events << " discrete event passes" << std::endl;
	if ( n_QSS_events > 0 ) std::cout << n_QSS_events << " requantization event passes" << std::endl;
	if ( n_QSS_simultaneous_events > 0 ) std::cout << n_QSS_simultaneous_events << " simultaneous requantization event passes" << std::endl;
	if ( n_ZC_events > 0 ) std::cout << n_ZC_events << " zero-crossing event passes" << std::endl;

	// QSS cleanup
	for ( auto & var : vars ) delete var;
}

} // dfn
} // QSS
