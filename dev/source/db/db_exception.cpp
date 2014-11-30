//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "exception.h"

using namespace std;

namespace DB {

	namespace {

		//---------------------------------------------------------------------
		std::unordered_map<int, int> g_error_map;
		bool g_map_built = false;

		//---------------------------------------------------------------------
		template< size_t size >
		void SetErrorMap( const int (&codes)[size], int value ) {
			for( int i = 0; i < size; i++ ) {
				g_error_map[ codes[i] ] = value;
			}
		}

		//---------------------------------------------------------------------
		void BuildErrorMap() {
			if( g_map_built ) return;

			// see mysql_error_categories.txt
			static const int recoverable[] = {
				1081, 1152, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 
				1180, 1181, 1183, 1184, 1189, 1190, 1196, 1218, 1219, 1220, 
				1317, 1376, 1412, 1429, 1430,

				2000, 2001, 2002, 2003, 2004, 2006, 2008, 2012, 2013, 2014,
				2024, 2025, 2027, 2055 
			};

			static const int retry[] = {
				1205, 1213
			};

			static const int softfail[] = {
				1040, 1053, 1203, 2016, 2017
			};

			static const int hardfail[] = {
				1045, 1129, 1130, 2005, 2007, 2009, 2018
			};

			SetErrorMap( recoverable, Exception::RECOVERABLE );
			SetErrorMap( retry,       Exception::RETRY       );
			SetErrorMap( softfail,    Exception::SOFTFAIL    );
			SetErrorMap( hardfail,    Exception::HARDFAIL    );
		}

		//---------------------------------------------------------------------
		int GetErrorCategory( int mysql_error ) {
			if( g_error_map.count(mysql_error) == 0 ) {
				return Exception::UNKNOWN;
			}
			return g_error_map.at( mysql_error );
		}
	}

	//-------------------------------------------------------------------------
	Exception::Exception( const sql::SQLException &e ) : 
			m_mysql_error( e.getErrorCode() ), 
			runtime_error( e.what() ) {

		if( m_mysql_error ) {
			// this was an sql error.
			m_sqlstate = e.getSQLState();
		} else {
			// this was not an sql error. (?)
		}

		m_type = GetErrorCategory( m_mysql_error );
	}

	//-------------------------------------------------------------------------
	Exception::Exception( int type, const string &desc ) : 
			m_type(type), runtime_error( desc ) 
	{}
}
