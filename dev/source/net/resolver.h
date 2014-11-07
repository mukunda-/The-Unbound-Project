//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef BOOST_ASIO_HPP
#  error Add boost/asio.hpp to PCH.
#endif


#include "util/threadstate.h"

namespace Net {


class Resolver : public std::enable_shared_from_this<Resolver> {

private:
	boost::asio::ip::tcp::resolver resolver; 
	//boost::asio::ip::tcp::endpoint endpoint;
	
	//Util::ThreadState finished;
	//int error; 
	
	typedef boost::function< void( const boost::system::error_code &error_code, boost::asio::ip::tcp::resolver::iterator &endpoints ) > callback_t;
	callback_t callback; 

	void OnResolve();

	void ResolveThreaded( const std::string &hostname, const std::string &service, callback_t callback );
	void Resolver::OnResolve( const boost::system::error_code &error_code, 
					boost::asio::ip::tcp::resolver::iterator r_endpoints,
					callback_t forward );
public:
	Resolver();

	boost::asio::ip::tcp::resolver::iterator 
		Resolve( const std::string &hostname, const std::string &service );

	typedef std::shared_ptr<Resolver> Pointer;

	static Pointer CreateThreaded( const std::string &hostname, const std::string &service, callback_t callback ) {
		Pointer resolver( new Resolver );
		resolver->ResolveThreaded( hostname, service, callback );
		return resolver;
	}
};

}
