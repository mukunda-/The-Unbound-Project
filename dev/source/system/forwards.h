//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

	namespace Commands {
		class Instance;
		using InstancePtr = std::shared_ptr<Instance>;
	}

	class Command;
	class Variable;
	class Callback;
	class Program;
	class Main;

	using CommandPtr = std::unique_ptr< Command >;
	using VariablePtr = std::unique_ptr< Variable >;
}