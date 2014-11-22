#include "stdafx.h"
#include "rxgserv.h"

namespace User { namespace RXGServ {

	Stream::Stream() : m_prochandler( *this ), m_procq( m_prochandler ) {}

}}
