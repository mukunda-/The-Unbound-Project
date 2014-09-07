//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "network/resolver.h"
#include "nwcore.h"

namespace Network {

//-------------------------------------------------------------------------------------------------
Resolver::Resolver(): resolver( DefaultService()() ) {
	  
}

//-------------------------------------------------------------------------------------------------
boost::asio::ip::tcp::resolver::iterator Resolver::Resolve( 
					const std::string &hostname, 
					const std::string &service )  {
					

	boost::asio::ip::tcp::resolver::query q( hostname, service );
	return resolver.resolve( q );
}

//-------------------------------------------------------------------------------------------------
void Resolver::ResolveThreaded( const std::string &hostname, 
					const std::string &service, 
					callback_t callback )  {
	
	resolver.async_resolve( 
		boost::asio::ip::tcp::resolver::query( hostname, service ),
		boost::bind( &Resolver::OnResolve, shared_from_this(),
		  boost::asio::placeholders::error,
		  boost::asio::placeholders::iterator,
		  callback ) );
}

//-------------------------------------------------------------------------------------------------
void Resolver::OnResolve( const boost::system::error_code &error_code, 
					boost::asio::ip::tcp::resolver::iterator r_endpoints,
					callback_t forward ) {
	//endpoints = r_endpoints;
	//error = error_code;
	//finished.Set(1);
	forward( error_code, r_endpoints );
}

//-------------------------------------------------------------------------------------------------
//bool Resolver::Finished() {
//	return finished.Get() == 1;
//}

//-------------------------------------------------------------------------------------------------
//void Resolver::WaitResolved() {
//	finished.Wait(1);
//}

}
