

#include "stdafx.h"

namespace User { namespace RXGServ {

	class Stream;

	//---------------------------------------------------------------------
	namespace {
		void EscapeLine( std::string &str ) {
			std::replace( str.begin(), str.end(), '\n', ' ' );
		}
	}

	//---------------------------------------------------------------------
	class Response {

		virtual void Write( Stream &stream ) = 0;
	};

	/// -------------------------------------------------------------------
	/// RT1
	class SimpleResponse : public Response {
		std::string m_text; 
	public:
		SimpleResponse( const std::string &text );
		void Write( Stream &stream ) override;
	};

	/// -------------------------------------------------------------------
	/// RT2
	///
	class ListResponse : public Response {
		std::vector<std::string> list;
	public:
		ListResponse &operator<<( const std::string &text );
		
		void Write( Stream &stream );
	};

	/// -------------------------------------------------------------------
	/// RT3
	///
	class KeyValues : public Response {
	};
	
}}
