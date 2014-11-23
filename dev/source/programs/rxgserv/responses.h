

#include "stdafx.h"

namespace User { namespace RXGServ {

	class Stream;

	//---------------------------------------------------------------------
	namespace {
		void EscapeLine( std::string &str ) {
			std::replace( str.begin(), str.end(), '\n', ' ' );
		}
	}

	enum class RCodes {
		UNKNOWN_COMMAND = 400, 
	};

	//---------------------------------------------------------------------
	class RCodeText {

		static std::unordered_map<RCodes,std::string> m_values;
		static void Init();
	public:
		
		static const std::string &Get( RCodes code );
	};

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
		
		void Write( Stream &stream ) override;
	};

	/// -------------------------------------------------------------------
	/// RT3
	///
	class KVResponse : public Response {
	};

	class ErrorResponse : public Response {
		RCodes m_code;
	public:
		ErrorResponse( RCodes code );
		void Write( Stream &stream ) override;
	};
	
}}
