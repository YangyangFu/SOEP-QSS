// QSS Zero-Crossing Variable Abstract Base Class
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

#ifndef QSS_dfn_Variable_ZC_hh_INCLUDED
#define QSS_dfn_Variable_ZC_hh_INCLUDED

// QSS Headers
#include <QSS/dfn/Variable.hh>
#include <QSS/EnumHash.hh> // GCC 5.x needs this

// C++ Headers
#include <unordered_set>

namespace QSS {
namespace dfn {

// QSS Zero-Crossing Variable Abstract Base Class
template< template< typename > class F, template< typename > class H >
class Variable_ZC : public Variable
{

public: // Types

	using Super = Variable;
	using Time = Variable::Time;
	using Value = Variable::Value;
	using Function = F< Variable >;
	using Handler = H< Variable >;
	using Crossing = Variable::Crossing;
	using Crossings = std::unordered_set< Crossing, EnumHash >;

protected: // Creation

	// Constructor
	explicit
	Variable_ZC(
	 std::string const & name,
	 Value const rTol = 1.0e-4,
	 Value const aTol = 1.0e-6
	) :
	 Super( name, rTol, aTol )
	{}

	// Copy Constructor
	Variable_ZC( Variable_ZC const & ) = default;

	// Move Constructor
	Variable_ZC( Variable_ZC && ) noexcept = default;

public: // Creation

	// Destructor
	virtual
	~Variable_ZC()
	{}

protected: // Assignment

	// Copy Assignment
	Variable_ZC &
	operator =( Variable_ZC const & ) = default;

	// Move Assignment
	Variable_ZC &
	operator =( Variable_ZC && ) noexcept = default;

public: // Predicate

	// Zero-Crossing Variable?
	bool
	is_ZC() const
	{
		return true;
	}

	// Has Crossing Type?
	bool
	has( Crossing const crossing )
	{
		return ( crossings.find( crossing ) != crossings.end() );
	}

public: // Properties

	// Zero-Crossing Time
	Time
	tZC() const
	{
		return tZ;
	}

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

	// Handler
	Handler const &
	h() const
	{
		return h_;
	}

	// Handler
	Handler &
	h()
	{
		return h_;
	}

public: // Methods

	// Add Crossing Type
	Variable_ZC &
	add( Crossing const crossing )
	{
		crossings.insert( crossing );
		return *this;
	}

	// Add All Crossing Types
	Variable_ZC &
	add_crossings_all()
	{
		add_crossings_Dn();
		crossings.insert( Crossing::Flat );
		add_crossings_Up();
		return *this;
	}

	// Add All non-Flat Crossing Types
	Variable_ZC &
	add_crossings_non_Flat()
	{
		add_crossings_Dn();
		add_crossings_Up();
		return *this;
	}

	// Add All Downward Crossing Types
	Variable_ZC &
	add_crossings_Dn()
	{
		crossings.insert( Crossing::DnPN );
		crossings.insert( Crossing::DnPZ );
		crossings.insert( Crossing::DnZN );
		crossings.insert( Crossing::Dn );
		return *this;
	}

	// Add All Downward and Flat Crossing Types
	Variable_ZC &
	add_crossings_Dn_Flat()
	{
		add_crossings_Dn();
		crossings.insert( Crossing::Flat );
		return *this;
	}

	// Add All Upward Crossing Types
	Variable_ZC &
	add_crossings_Up()
	{
		crossings.insert( Crossing::Up );
		crossings.insert( Crossing::UpZP );
		crossings.insert( Crossing::UpNZ );
		crossings.insert( Crossing::UpNP );
		return *this;
	}

	// Add All Upward and Flat Crossing Types
	Variable_ZC &
	add_crossings_Up_Flat()
	{
		crossings.insert( Crossing::Flat );
		add_crossings_Up();
		return *this;
	}

protected: // Methods

	// Crossing Type from Values
	template< typename T >
	static
	Crossing
	crossing_type( T const val1, T const val2 )
	{
		if ( val1 == T( 0 ) ) {
			return ( val2 > T( 0 ) ? Crossing::UpZP : ( val2 == T( 0 ) ? Crossing::Flat : Crossing::DnZN ) );
		} else if ( val2 == T( 0 ) ) {
			return ( val1 > T( 0 ) ? Crossing::DnPZ : Crossing::UpNZ );
		} else {
			return ( val1 > T( 0 ) ? Crossing::DnPN : Crossing::UpNP );
		}
	}

	// Crossing Type from Slope
	template< typename T >
	static
	Crossing
	crossing_type( T const slope )
	{
		if ( slope == T( 0 ) ) {
			return Crossing::Flat;
		} else if ( slope > T( 0 ) ) {
			return Crossing::UpNP;
		} else {
			return Crossing::DnPN;
		}
	}

public: // Data

	Time tZ{ infinity }; // Zero-crossing time: tQ <= tZ and tX <= tZ
	Time tZ_prev{ infinity }; // Previous zero-crossing time
	Crossing crossing{ Crossing::Flat }; // Zero-crossing type
	Crossings crossings; // Zero-crossing types handled

protected: // Data

	Function f_; // Zero-crossing function
	Handler h_; // Handler function

};

} // dfn
} // QSS

#endif
