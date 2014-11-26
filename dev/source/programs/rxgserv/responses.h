#pragma once

namespace User { namespace RXGServ {

	class Stream;
	namespace Procs {
		class Context;
	}

	enum class RCodes {
		UNKNOWN_COMMAND = 400, 
	};

	//-------------------------------------------------------------------------
	class RCodeText {

		static std::unordered_map<RCodes,std::string> m_values;
		static void Init();
	public:
		
		static const std::string &Get( RCodes code );
	};

	//-------------------------------------------------------------------------
	class Response {

	protected:

		/// -------------------------------------------------------------------
		/// Write implementation.
		///
		virtual void DoWrite( Stream &stream ) = 0;

	public:

		/// -------------------------------------------------------------------
		/// Write this response to a context's stream.
		///
		/// Throws an exception if the context already had a response written.
		///
		void Write( Procs::Context &ct );
		void Write( std::shared_ptr<Procs::Context> &ct );
	};

	/// -----------------------------------------------------------------------
	/// RT1
	class SimpleResponse : public Response {
		std::string m_text; 
	public:
		SimpleResponse( const std::string &text );
		void DoWrite( Stream &stream ) override;
	};

	/// -----------------------------------------------------------------------
	/// RT2
	///
	class ListResponse : public Response {
		std::vector<std::string> m_list;
	public:
		ListResponse &operator<<( const std::string &text );
		void DoWrite( Stream &stream ) override;
	};

	/// -----------------------------------------------------------------------
	/// RT3
	///
	class KVResponse : public Response {
		std::string current;
		bool m_getting_value = false;

		std::unordered_map< std::string, std::string > m_values;
	public:
		KVResponse &Put( const std::string &key, const std::string &value );
		KVResponse &Put( const std::string &key, int value );
		KVResponse &Erase( const std::string &key );

		void DoWrite( Stream &stream ) override;
	};

	//-------------------------------------------------------------------------
	class ErrorResponse : public Response {
		std::string m_status;
		std::string m_desc;
	public:
		ErrorResponse( const std::string &status, 
					   const std::string &desc );
		void DoWrite( Stream &stream ) override;
	};
	
}}
