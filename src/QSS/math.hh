// QSS Solver Math Functions
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

#ifndef QSS_math_hh_INCLUDED
#define QSS_math_hh_INCLUDED

// C++ Headers
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace QSS {

// Globals
extern double const zero;
extern double const two;
extern double const three;
extern double const four;
extern double const six;
extern double const one_half;
extern double const one_third;
extern double const one_fourth;
extern double const one_sixth;
extern double const one_ninth;
extern double const two_thirds;
extern double const pi;
extern double const infinity;
extern double const half_infinity;

// Sign: Returns Passed Type
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
sign( T const x )
{
	return ( std::signbit( x ) ? T( -1 ) : T( +1 ) );
}

// Signum: Returns Passed Type
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
sgn( T const x )
{
	return ( x < T( 0 ) ? T( -1 ) : ( x > T( 0 ) ? T( +1 ) : T( 0 ) ) );
}

// Signum: Returns Passed Type
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
T
sgn( T const & x )
{
	return ( x < T( 0 ) ? T( -1 ) : ( x > T( 0 ) ? T( +1 ) : T( 0 ) ) );
}

// Signum: Returns int
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
int
signum( T const x )
{
	return ( x < T( 0 ) ? -1 : ( x > T( 0 ) ? +1 : 0 ) );
}

// Signum: Returns int
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
int
signum( T const & x )
{
	return ( x < T( 0 ) ? -1 : ( x > T( 0 ) ? +1 : 0 ) );
}

// Square
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
square( T const x )
{
	return x * x;
}

// Square
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
T
square( T const & x )
{
	return x * x;
}

// Cube
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
cube( T const x )
{
	return x * x * x;
}

// Cube
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
T
cube( T const & x )
{
	return x * x * x;
}

// Min of 3 Values
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min( T const x, T const y, T const z )
{
	return ( x < y ? ( x < z ? x : z ) : ( y < z ? y : z ) );
}

// Min of 3 Values
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
T const &
min( T const & x, T const & y, T const & z )
{
	return ( x < y ? ( x < z ? x : z ) : ( y < z ? y : z ) );
}

// Min Positive of Two Nonnegative Values
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_positive( T const x, T const y )
{
	assert( x >= 0.0 );
	assert( y >= 0.0 );
	if ( x > 0.0 ) {
		if ( y > 0.0 ) {
			return std::min( x, y );
		} else {
			return x;
		}
	} else {
		return y;
	}
}

// Min Positive of Two Nonnegative Values
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
T const &
min_positive( T const & x, T const & y )
{
	assert( x >= 0.0 );
	assert( y >= 0.0 );
	if ( x > 0.0 ) {
		if ( y > 0.0 ) {
			return std::min( x, y );
		} else {
			return x;
		}
	} else {
		return y;
	}
}

// Min Positive of Three Nonnegative Values
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_positive( T const x, T const y, T const z )
{
	assert( x >= 0.0 );
	assert( y >= 0.0 );
	assert( z >= 0.0 );
	if ( x > 0.0 ) {
		if ( y > 0.0 ) {
			if ( z > 0.0 ) {
				return min( x, y, z );
			} else {
				return std::min( x, y );
			}
		} else if ( z > 0.0 ) {
			return std::min( x, z );
		} else {
			return x;
		}
	} else if ( y > 0.0 ) {
		if ( z > 0.0 ) {
			return std::min( y, z );
		} else {
			return y;
		}
	} else {
		return z;
	}
}

// Min Positive of Three Nonnegative Values
template< typename T, class = typename std::enable_if< ! std::is_arithmetic< T >::value >::type >
inline
T const &
min_positive( T const & x, T const & y, T const & z )
{
	assert( x >= 0.0 );
	assert( y >= 0.0 );
	assert( z >= 0.0 );
	if ( x > 0.0 ) {
		if ( y > 0.0 ) {
			if ( z > 0.0 ) {
				return min( x, y, z );
			} else {
				return std::min( x, y );
			}
		} else if ( z > 0.0 ) {
			return std::min( x, z );
		} else {
			return x;
		}
	} else if ( y > 0.0 ) {
		if ( z > 0.0 ) {
			return std::min( y, z );
		} else {
			return y;
		}
	} else {
		return z;
	}
}

// Value if Positive or Infinity
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
positive_or_infinity( T const r )
{
	return ( r > 0.0 ? r : infinity );
}

// Min Nonnegative Root of Quadratic Equation a x^2 + b x + c = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_quadratic( T const a, T const b, T const c )
{
	if ( a == 0.0 ) { // Linear
		if ( b == 0.0 ) { // Constant
			return ( c == 0.0 ? 0.0 : infinity );
		} else if ( c == 0.0 ) {
			return 0.0;
		} else {
			return ( sign( b ) != sign( c ) ? -( c / b ) : infinity );
		}
	} else { // Quadratic
		if ( c == 0.0 ) {
			return 0.0;
		} else if ( b == 0.0 ) {
			return ( sign( a ) != sign( c ) ? std::sqrt( -( c / a ) ) : infinity );
		} else {
			T const disc( ( b * b ) - ( 4.0 * a * c ) );
			if ( disc <= 0.0 ) { // Zero or one real root(s)
				if ( disc == 0.0 ) {
					return ( sign( a ) != sign( b ) ? -( b / ( 2.0 * a ) ) : infinity );
				} else {
					return infinity;
				}
			} else { // Two real roots: From http://mathworld.wolfram.com/QuadraticEquation.html for precision
				T const q( -0.5 * ( b + ( sign( b ) * std::sqrt( disc ) ) ) );
				if ( c > 0.0 ) {
					if ( b + ( 2.0 * q ) <= 0.0 ) { // Crossing direction test
						return std::max( q / a, 0.0 );
					} else {
						return std::max( c / q, 0.0 );
					}
				} else {
					assert( c < 0.0 );
					if ( b + ( 2.0 * q ) >= 0.0 ) { // Crossing direction test
						return std::max( q / a, 0.0 );
					} else {
						return std::max( c / q, 0.0 );
					}
				}
			}
		}
	}
}

// Min Positive Root of Quadratic Equation a x^2 + b x + c = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_positive_root_quadratic( T const a, T const b, T const c )
{
	if ( a == 0.0 ) { // Linear
		if ( b == 0.0 ) { // Constant
			return infinity; // Not a distinct root even if c == 0
		} else if ( c == 0.0 ) {
			return infinity;
		} else {
			return ( sign( b ) != sign( c ) ? -( c / b ) : infinity );
		}
	} else { // Quadratic
		if ( c == 0.0 ) { // Roots at 0 and -b/a
			if ( a == 0.0 ) {
				return infinity;
			} else if ( b == 0.0 ) { // No positive root
				return infinity;
			} else {
				return ( sign( a ) != sign( b ) ? -( b / a ) : infinity );
			}
		} else if ( b == 0.0 ) {
			return ( sign( a ) != sign( c ) ? std::sqrt( -( c / a ) ) : infinity );
		} else {
			T const disc( ( b * b ) - ( 4.0 * a * c ) );
			if ( disc <= 0.0 ) { // Zero or one real root(s)
				if ( disc == 0.0 ) {
					return ( sign( a ) != sign( b ) ? -( b / ( 2.0 * a ) ) : infinity );
				} else {
					return infinity;
				}
			} else { // Two real roots: From http://mathworld.wolfram.com/QuadraticEquation.html for precision
				T const q( -0.5 * ( b + ( sign( b ) * std::sqrt( disc ) ) ) );
				T r; // Tentative root
				if ( c > 0.0 ) {
					if ( b + ( 2.0 * q ) <= 0.0 ) { // Crossing direction test
						r = q / a;
					} else {
						r = c / q;
					}
				} else {
					assert( c < 0.0 );
					if ( b + ( 2.0 * q ) >= 0.0 ) { // Crossing direction test
						r = q / a;
					} else {
						r = c / q;
					}
				}
				return ( r > 0.0 ? r : infinity );
			}
		}
	}
}

// Min Nonnegative Root of Lower Boundary Quadratic Equation a x^2 + b x + c = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_quadratic_lower( T const a, T const b, T const c )
{
	assert( a <= 0.0 );
	assert( b <= 0.0 );
	// c > 0 with exact precision
	if ( c <= 0.0 ) { // Precision loss: x(tX) < q(tX) - qTol
		return 0.0;
	} else if ( a == 0.0 ) { // Linear
		if ( b == 0.0 ) { // Constant
			return infinity;
		} else {
			return -( c / b );
		}
	} else { // Quadratic
		T const disc( ( b * b ) - ( 4.0 * a * c ) );
		if ( disc <= 0.0 ) { // Zero or one real root(s) => Precision loss
			return 0.0;
		} else { // Two real roots: From http://mathworld.wolfram.com/QuadraticEquation.html for precision
			T const q( -0.5 * ( b + ( sign( b ) * std::sqrt( disc ) ) ) );
			if ( b + ( 2.0 * q ) <= 0.0 ) { // Crossing direction test
				return std::max( q / a, 0.0 );
			} else {
				return std::max( c / q, 0.0 );
			}
		}
	}
}

// Min Nonnegative Root of Upper Boundary Quadratic Equation a x^2 + b x + c = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_quadratic_upper( T const a, T const b, T const c )
{
	assert( a >= 0.0 );
	assert( b >= 0.0 );
	// c < 0 with exact precision
	if ( c >= 0.0 ) { // Precision loss: x(tX) > q(tX) + qTol
		return 0.0;
	} else if ( a == 0.0 ) { // Linear
		if ( b == 0.0 ) { // Constant
			return infinity;
		} else {
			return -( c / b );
		}
	} else { // Quadratic
		T const disc( ( b * b ) - ( 4.0 * a * c ) );
		if ( disc <= 0.0 ) { // Zero or one real root(s) => Precision loss
			return 0.0;
		} else { // Two real roots: From http://mathworld.wolfram.com/QuadraticEquation.html for precision
			T const q( -0.5 * ( b + ( sign( b ) * std::sqrt( disc ) ) ) );
			if ( b + ( 2.0 * q ) >= 0.0 ) { // Crossing direction test
				return std::max( q / a, 0.0 );
			} else {
				return std::max( c / q, 0.0 );
			}
		}
	}
}

// Min Nonnegative Root of Both Boundary Quadratic Equations a x^2 + b x + c = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_quadratic_both( T const a, T const b, T const cl, T const cu )
{
	// cl > 0 with exact precision
	// cu < 0 with exact precision
	if ( ( cl <= 0.0 ) || ( cu >= 0.0 ) ) { // Precision loss: x(tX) < q(tX) - qTol or x(tX) > q(tX) + qTol
		return 0.0;
	} else if ( a == 0.0 ) { // Linear
		if ( b == 0.0 ) { // Constant
			return infinity;
		} else if ( b <= 0.0 ) {
			return -( cl / b );
		} else {
			return -( cu / b );
		}
	} else { // Quadratic
		T const bb( b * b );
		T const a4( 4.0 * a );

		// Lower boundary
		T const discl( bb - ( a4 * cl ) );
		T rootl;
		if ( discl < 0.0 ) { // No real roots
			rootl = infinity;
		} else if ( discl == 0.0 ) { // One real root
			rootl = -b / ( 2.0 * a );
			if ( rootl < 0.0 ) rootl = infinity;
		} else { // Two real roots: From http://mathworld.wolfram.com/QuadraticEquation.html for precision
			T const q( -0.5 * ( b + ( sign( b ) * std::sqrt( discl ) ) ) );
			if ( b + ( 2.0 * q ) <= 0.0 ) { // Crossing direction test
				rootl = q / a;
			} else {
				rootl = cl / q;
			}
		}

		// Upper boundary
		T const discu( bb - ( a4 * cu ) );
		T rootu;
		if ( discu < 0.0 ) { // No real roots
			rootu = infinity;
		} else if ( discu == 0.0 ) { // One real root
			rootu = -b / ( 2.0 * a );
			if ( rootu < 0.0 ) rootu = infinity;
		} else { // Two real roots: From http://mathworld.wolfram.com/QuadraticEquation.html for precision
			T const q( -0.5 * ( b + ( sign( b ) * std::sqrt( discu ) ) ) );
			if ( b + ( 2.0 * q ) >= 0.0 ) { // Crossing direction test
				rootu = q / a;
			} else {
				rootu = cu / q;
			}
		}

		if ( ( rootl == infinity ) && ( rootu == infinity ) ) { // Precision loss
			return 0.0;
		} else {
			return std::max( std::min( rootl, rootu ), 0.0 );
		}
	}
}

// Min Positive Root of Quadratic Equation a x^2 + b x + c = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
peak_mag_quadratic( T const a, T const b, T const c )
{
	if ( a == 0.0 ) { // Linear
		return ( b == 0.0 ? c : infinity );
	} else {
		return std::abs( c - ( b * b ) / ( T( 4 ) * a ) );
	}
}

// Root of a Cubic if it Crosses Outward or Zero
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
cubic_cull( T const a, T const b, T const r )
{
	return ( r > 0.0 ? ( ( 3.0 * r * r ) + ( 2.0 * a * r ) + b >= 0.0 ? r : 0.0 ) : 0.0 );
}

// Root of a Cubic if it Crosses Upward or Zero
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
cubic_cull_upper( T const a, T const b, T const r, T const s )
{
	return ( r > 0.0 ? ( ( ( 3.0 * r * r ) + ( 2.0 * a * r ) + b ) * s >= 0.0 ? r : 0.0 ) : 0.0 );
}

// Root of a Cubic if it Crosses Downward or Zero
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
cubic_cull_lower( T const a, T const b, T const r, T const s )
{
	return ( r > 0.0 ? ( ( ( 3.0 * r * r ) + ( 2.0 * a * r ) + b ) * s <= 0.0 ? r : 0.0 ) : 0.0 );
}

// Min Positive Root of Cubic Equation a x^3 + b x^2 + c x + d = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_positive_root_cubic( T a, T b, T c, T const d )
{
	static T const one_54( 1.0 / 54.0 );
	static T const one_1458( 1.0 / 1458.0 );
	static T const two_thirds_pi( ( 2.0 / 3.0 ) * pi );
	if ( a == 0.0 ) { // Quadratic
		return min_positive_root_quadratic( b, c, d );
	} else { // Cubic
		T const inv_a( 1.0 / a ); // Normalize to x^3 + a x^2 + b x + c
		a = b * inv_a;
		b = c * inv_a;
		c = d * inv_a;
		T const a_3( one_third * a );
		T const a2( a * a );
		T const q( a2 - ( 3.0 * b ) );
		T const r( ( ( ( 2.0 * a2 ) - ( 9.0 * b ) ) * a ) + ( 27.0 * c ) );
		if ( ( q == 0.0 ) && ( r == 0.0 ) ) {
			return ( a_3 < 0.0 ? -a_3 : infinity );
		} else {
			T const q3( q * q * q );
			T const CR2( 729.0 * r * r );
			T const CQ3( 2916.0 * q3 );
			T const Q( one_ninth * q );
			if ( CR2 > CQ3 ) { // One real root
				T const A( -sign( r ) * std::cbrt( ( one_54 * std::abs( r ) ) + ( one_1458 * std::sqrt( CR2 - CQ3 ) ) ) );
				T const B( Q / A );
				return positive_or_infinity( A + B - a_3 );
			} else if ( CR2 < CQ3 ) { // Three real roots
				T const sqrt_q( std::sqrt( q ) );
				T const scl( -two_thirds * sqrt_q );
				T const theta_3( one_third * std::acos( 0.5 * r / ( sqrt_q * sqrt_q * sqrt_q ) ) );
				T const root1( ( scl * std::cos( theta_3 ) ) - a_3 );
				T const root2( ( scl * std::cos( theta_3 + two_thirds_pi ) ) - a_3 );
				T const root3( ( scl * std::cos( theta_3 - two_thirds_pi ) ) - a_3 );
				return min_positive( root1, root2, root3 );
			} else { // Two real roots
				assert( CR2 == CQ3 );
				T const sqrt_Q( std::sqrt( Q ) );
				if ( r > 0.0 ) {
					T const root1( -( 2.0 * sqrt_Q ) - a_3 );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						return root1;
					} else {
						return positive_or_infinity( sqrt_Q - a_3 );
					}
				} else {
					T const root1( positive_or_infinity( -sqrt_Q - a_3 ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						return root1;
					} else {
						return positive_or_infinity( ( 2.0 * sqrt_Q ) - a_3 );
					}
				}
			}
		}
	}
}

// Min Nonnegative Root of Upper Boundary Cubic Equation a x^3 + b x^2 + c x + d = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_cubic_upper( T a, T b, T c, T const d )
{
	assert( a >= 0.0 );
	assert( b >= 0.0 );
	assert( c >= 0.0 );
	// d < 0 with exact precision
	static T const one_54( 1.0 / 54.0 );
	static T const one_1458( 1.0 / 1458.0 );
	static T const two_thirds_pi( ( 2.0 / 3.0 ) * pi );
	if ( a == 0.0 ) { // Quadratic
		return min_root_quadratic_upper( b, c, d );
	} else { // Cubic
		T const inv_a( 1.0 / a ); // Normalize to x^3 + a x^2 + b x + c
		a = b * inv_a;
		b = c * inv_a;
		c = d * inv_a;
		T const a_3( one_third * a );
		T const a2( a * a );
		T const q( a2 - ( 3.0 * b ) );
		T const r( ( ( ( 2.0 * a2 ) - ( 9.0 * b ) ) * a ) + ( 27.0 * c ) );
		if ( ( q == 0.0 ) && ( r == 0.0 ) ) {
			return std::max( -a_3, 0.0 );
		} else {
			T const q3( q * q * q );
			T const CR2( 729.0 * r * r );
			T const CQ3( 2916.0 * q3 );
			T const Q( one_ninth * q );
			if ( CR2 > CQ3 ) { // One real root
				T const A( -sign( r ) * std::cbrt( ( one_54 * std::abs( r ) ) + ( one_1458 * std::sqrt( CR2 - CQ3 ) ) ) );
				T const B( Q / A );
				return cubic_cull( a, b, A + B - a_3 );
			} else if ( CR2 < CQ3 ) { // Three real roots
				T const sqrt_q( std::sqrt( q ) );
				T const scl( -two_thirds * sqrt_q );
				T const theta_3( one_third * std::acos( 0.5 * r / ( sqrt_q * sqrt_q * sqrt_q ) ) );
				T const root1( cubic_cull( a, b, ( scl * std::cos( theta_3 ) ) - a_3 ) );
				T const root2( cubic_cull( a, b, ( scl * std::cos( theta_3 + two_thirds_pi ) ) - a_3 ) );
				T const root3( cubic_cull( a, b, ( scl * std::cos( theta_3 - two_thirds_pi ) ) - a_3 ) );
				return min_positive( root1, root2, root3 );
			} else { // Two real roots
				assert( CR2 == CQ3 );
				T const sqrt_Q( std::sqrt( Q ) );
				if ( r > 0.0 ) {
					T const root1( cubic_cull( a, b, -( 2.0 * sqrt_Q ) - a_3 ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						return root1;
					} else {
						return cubic_cull( a, b, sqrt_Q - a_3 );
					}
				} else {
					T const root1( cubic_cull( a, b, -sqrt_Q - a_3 ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						return root1;
					} else {
						return cubic_cull( a, b, ( 2.0 * sqrt_Q ) - a_3 );
					}
				}
			}
		}
	}
}

// Min Nonnegative Root of Lower Boundary Cubic Equation a x^3 + b x^2 + c x + d = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_cubic_lower( T a, T b, T c, T const d )
{
	assert( a <= 0.0 );
	assert( b <= 0.0 );
	assert( c <= 0.0 );
	// d > 0 with exact precision
	static T const one_54( 1.0 / 54.0 );
	static T const one_1458( 1.0 / 1458.0 );
	static T const two_thirds_pi( ( 2.0 / 3.0 ) * pi );
	if ( a == 0.0 ) { // Quadratic
		return min_root_quadratic_lower( b, c, d );
	} else { // Cubic
		T const inv_a( 1.0 / a ); // Normalize to x^3 + a x^2 + b x + c
		a = b * inv_a;
		b = c * inv_a;
		c = d * inv_a;
		T const a_3( one_third * a );
		T const a2( a * a );
		T const q( a2 - ( 3.0 * b ) );
		T const r( ( ( ( 2.0 * a2 ) - ( 9.0 * b ) ) * a ) + ( 27.0 * c ) );
		if ( ( q == 0.0 ) && ( r == 0.0 ) ) {
			return std::max( -a_3, 0.0 );
		} else {
			T const q3( q * q * q );
			T const CR2( 729.0 * r * r );
			T const CQ3( 2916.0 * q3 );
			T const Q( one_ninth * q );
			if ( CR2 > CQ3 ) { // One real root
				T const A( -sign( r ) * std::cbrt( ( one_54 * std::abs( r ) ) + ( one_1458 * std::sqrt( CR2 - CQ3 ) ) ) );
				T const B( Q / A );
				return cubic_cull( a, b, A + B - a_3 );
			} else if ( CR2 < CQ3 ) { // Three real roots
				T const sqrt_q( std::sqrt( q ) );
				T const scl( -two_thirds * sqrt_q );
				T const theta_3( one_third * std::acos( 0.5 * r / ( sqrt_q * sqrt_q * sqrt_q ) ) );
				T const root1( cubic_cull( a, b, ( scl * std::cos( theta_3 ) ) - a_3 ) );
				T const root2( cubic_cull( a, b, ( scl * std::cos( theta_3 + two_thirds_pi ) ) - a_3 ) );
				T const root3( cubic_cull( a, b, ( scl * std::cos( theta_3 - two_thirds_pi ) ) - a_3 ) );
				return min_positive( root1, root2, root3 );
			} else { // Two real roots
				assert( CR2 == CQ3 );
				T const sqrt_Q( std::sqrt( Q ) );
				if ( r > 0.0 ) {
					T const root1( cubic_cull( a, b, -( 2.0 * sqrt_Q ) - a_3 ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						return root1;
					} else {
						return cubic_cull( a, b, sqrt_Q - a_3 );
					}
				} else {
					T const root1( cubic_cull( a, b, -sqrt_Q - a_3 ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						return root1;
					} else {
						return cubic_cull( a, b, ( 2.0 * sqrt_Q ) - a_3 );
					}
				}
			}
		}
	}
}

// Min Nonnegative Root of Both Boundary Cubic Equations a x^3 + b x^2 + c x + d = 0
template< typename T, class = typename std::enable_if< std::is_arithmetic< T >::value >::type >
inline
T
min_root_cubic_both( T a, T b, T const c, T const dl, T const du )
{
	// dl > 0 with exact precision
	// du < 0 with exact precision
	static T const one_54( 1.0 / 54.0 );
	static T const one_1458( 1.0 / 1458.0 );
	static T const two_thirds_pi( ( 2.0 / 3.0 ) * pi );
	if ( a == 0.0 ) { // Quadratic
		return min_root_quadratic_both( b, c, dl, du );
	} else { // Cubic
		T const s( sign( a ) );
		T const inv_a( 1.0 / a ); // Normalize to x^3 + a x^2 + b x + c
		a = b * inv_a;
		b = c * inv_a;
		T const cl( dl * inv_a );
		T const cu( du * inv_a );
		T const a_3( one_third * a );
		T const a2( a * a );
		T const q( a2 - ( 3.0 * b ) );
		T const q3( q * q * q );
		T const CQ3( 2916.0 * q3 );
		T const Q( one_ninth * q );
		T const rm( ( ( 2.0 * a2 ) - ( 9.0 * b ) ) * a );
		T r;

		// Lower boundary
		T rootl;
		r = rm + ( 27.0 * cl );
		if ( ( q == 0.0 ) && ( r == 0.0 ) ) {
			rootl = std::max( -a_3, 0.0 );
		} else {
			T const CR2( 729.0 * r * r );
			if ( CR2 > CQ3 ) { // One real root
				T const A( -sign( r ) * std::cbrt( ( one_54 * std::abs( r ) ) + ( one_1458 * std::sqrt( CR2 - CQ3 ) ) ) );
				T const B( Q / A );
				rootl = cubic_cull_lower( a, b, A + B - a_3, s );
			} else if ( CR2 < CQ3 ) { // Three real roots
				T const sqrt_q( std::sqrt( q ) );
				T const scl( -two_thirds * sqrt_q );
				T const theta_3( one_third * std::acos( 0.5 * r / ( sqrt_q * sqrt_q * sqrt_q ) ) );
				T const root1( cubic_cull_lower( a, b, ( scl * std::cos( theta_3 ) ) - a_3, s ) );
				T const root2( cubic_cull_lower( a, b, ( scl * std::cos( theta_3 + two_thirds_pi ) ) - a_3, s ) );
				T const root3( cubic_cull_lower( a, b, ( scl * std::cos( theta_3 - two_thirds_pi ) ) - a_3, s ) );
				rootl = min_positive( root1, root2, root3 );
			} else { // Two real roots
				assert( CR2 == CQ3 );
				T const sqrt_Q( std::sqrt( Q ) );
				if ( r > 0.0 ) {
					T const root1( cubic_cull_lower( a, b, -( 2.0 * sqrt_Q ) - a_3, s ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						rootl = root1;
					} else {
						rootl = cubic_cull_lower( a, b, sqrt_Q - a_3, s );
					}
				} else {
					T const root1( cubic_cull_lower( a, b, -sqrt_Q - a_3, s ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						rootl = root1;
					} else {
						rootl = cubic_cull_lower( a, b, ( 2.0 * sqrt_Q ) - a_3, s );
					}
				}
			}
		}

		// Upper boundary
		T rootu;
		r = rm + ( 27.0 * cu );
		if ( ( q == 0.0 ) && ( r == 0.0 ) ) {
			rootu = std::max( -a_3, 0.0 );
		} else {
			T const CR2( 729.0 * r * r );
			if ( CR2 > CQ3 ) { // One real root
				T const A( -sign( r ) * std::cbrt( ( one_54 * std::abs( r ) ) + ( one_1458 * std::sqrt( CR2 - CQ3 ) ) ) );
				T const B( Q / A );
				rootu = cubic_cull_upper( a, b, A + B - a_3, s );
			} else if ( CR2 < CQ3 ) { // Three real roots
				T const sqrt_q( std::sqrt( q ) );
				T const scl( -two_thirds * sqrt_q );
				T const theta_3( one_third * std::acos( 0.5 * r / ( sqrt_q * sqrt_q * sqrt_q ) ) );
				T const root1( cubic_cull_upper( a, b, ( scl * std::cos( theta_3 ) ) - a_3, s ) );
				T const root2( cubic_cull_upper( a, b, ( scl * std::cos( theta_3 + two_thirds_pi ) ) - a_3, s ) );
				T const root3( cubic_cull_upper( a, b, ( scl * std::cos( theta_3 - two_thirds_pi ) ) - a_3, s ) );
				rootu = min_positive( root1, root2, root3 );
			} else { // Two real roots
				assert( CR2 == CQ3 );
				T const sqrt_Q( std::sqrt( Q ) );
				if ( r > 0.0 ) {
					T const root1( cubic_cull_upper( a, b, -( 2.0 * sqrt_Q ) - a_3, s ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						rootu = root1;
					} else {
						rootu = cubic_cull_upper( a, b, sqrt_Q - a_3, s );
					}
				} else {
					T const root1( cubic_cull_upper( a, b, -sqrt_Q - a_3, s ) );
					if ( root1 > 0.0 ) { // Must be smallest positive root
						rootu = root1;
					} else {
						rootu = cubic_cull_upper( a, b, ( 2.0 * sqrt_Q ) - a_3, s );
					}
				}
			}
		}

		return min_positive( rootl, rootu );
	}
}

} // QSS

#endif
