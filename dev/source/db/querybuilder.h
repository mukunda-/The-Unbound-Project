//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


namespace DB {

	class Line;

	/// -----------------------------------------------------------------------
	/// When RawString is fed to a query builder, the exact string contents
	/// are output into the query, as opposed to being wrapped in quotes
	/// and escaped like an normal string.
	///
	/// It may be more convenient to pass RawString directly into an execute 
	/// command rather than using the FeedUnescapedString function yourself.
	///
	class RawString {
		const std::string &m_value;
	public: 
		RawString( const std::string &value ) : m_value(value) {}
		const std::string &Get() const { return m_value; }
	};

	using RS = DB::RawString;
	
	class QueryBuilder final {
		boost::format m_formatter;
		sql::Connection &m_conn;

	public:
		/// -------------------------------------------------------------------
		/// Create a query builder, requires an active connection for
		/// string escaping.
		///
		/// @param line/connection The connection used to escape strings.
		/// @param format          Query template.
		///
		QueryBuilder( Line &line, const std::string &format );
		QueryBuilder( sql::Connection &connection, 
					  const std::string &format );
		
		/// -------------------------------------------------------------------
		/// Feed operator.
		///
		/// @param arg Argument to feed into the builder. An exception will
		///            be thrown if too many arguments are given.
		/// @returns QueryBuilder for chaining.
		///
		/// Strings will be SQL escaped *and* quotes are added!
		///
		/// To feed a raw string, use FeedUnescapedString.
		///
		template< typename T >
		QueryBuilder &operator%( T arg ) {
			m_formatter % arg;
			return *this;
		}
		 
		// string specializations
		template<> QueryBuilder &operator%( const char *arg )         { return FeedEscapedString( arg ); }
		template<> QueryBuilder &operator%( const std::string &arg )  { return FeedEscapedString( arg ); }
		template<> QueryBuilder &operator%( const std::string &&arg ) {	return FeedEscapedString( arg ); }
		template<> QueryBuilder &operator%( const std::string arg )   { return FeedEscapedString( arg ); }

		// rawstring specializations
		template<> QueryBuilder &operator%( const RawString &arg )  { return FeedUnescapedString( arg.Get() ); }
		template<> QueryBuilder &operator%( const RawString &&arg ) { return FeedUnescapedString( arg.Get() ); }
		template<> QueryBuilder &operator%( const RawString arg )   { return FeedUnescapedString( arg.Get() ); }

		// see feed operator.
		QueryBuilder &FeedEscapedString( const std::string &arg );

		/// -------------------------------------------------------------------
		/// Feed raw string to the formatter.
		///
		/// @param arg Arg to substitute for the next format marker.
		/// @returns QueryBuilder for chaining.
		///
		QueryBuilder &FeedUnescapedString( const std::string &arg );
		
		/// -------------------------------------------------------------------
		/// Finalize and return the formatted string.
		/// 
		/// @returns Formatted string.
		///
		/// @throws ? if the template is not satisfied.
		///
		std::string String() {
			return m_formatter.str();
		}
	};
}
