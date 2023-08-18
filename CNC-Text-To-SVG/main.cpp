// Corrade Includes.
#include "Corrade/Containers/Optional.h"
#include "Corrade/Containers/Pair.h"
#include "Corrade/Containers/String.h"
#include "Corrade/Containers/StringView.h"
#include "Corrade/Utility/Arguments.h"
#include "Corrade/Utility/Path.h"
#include "Corrade/Utility/String.h"

// std Includes.
#include <iostream>

using namespace Corrade;
using namespace Corrade::Containers;

namespace CNCTextToSVG
{
	Optional< Utility::Arguments > ParseCommandLineArguments( int argc, char** argv )
	{
		Utility::Arguments args;
		args
			.addOption( 'd', "directory", "." ).setHelp( "directory", "the directory containing the generated nc files" )
			.addOption( 'e', "extension", "uk" ).setHelp( "extension", "the extension (or a sub-string of it) of the generated .cnc files" )
			.addBooleanOption( 's', "silent" ).setHelp( "silent", "if set, program does not report to console & terminates without waiting for the user to hit enter." )
			.setGlobalHelp( "Generates an .SVG file containing the vector image of the engraved text for all NC files in the specified directory recursively." );

		if( !args.tryParse( argc, argv ) )
		{
			std::cerr << "Could not parse command line arguments.\n";

			return NullOpt;
		}

		return args;
	}

	std::vector< Corrade::Containers::String > FilterOutFilesToBeProcessed( const StringView directory, const StringView extension_expected )
	{
		std::vector< String > file_info_list;

		if( const auto& entries = Utility::Path::list( directory ); entries )
		{
			for( const auto& entry : *entries )
			{
				const auto entry_lowercase( Utility::String::lowercase( entry ) );

				const auto extension = Utility::Path::splitExtension( entry_lowercase ).second();
				if( extension.contains( extension_expected ) )
					file_info_list.push_back( entry );
			}
		}

		return file_info_list;
	}
}

int main( int argc, char** argv )
{
	using namespace CNCTextToSVG;

	int error_code = 0;

	bool silent_mode = false;

	if( const auto&& args = ParseCommandLineArguments( argc, argv ); args )
	{
		silent_mode = args->isSet( "silent" );

		const String directory( args->value( "directory" ) );
		const auto extension_expected( args->value( "extension" ) );

		const auto& file_info_list = FilterOutFilesToBeProcessed( args->value( "directory" ), args->value( "extension" ) );

		for( auto& file_info : file_info_list )
			std::cout << file_info.data() << "\n";
	}
	else
		error_code = -1;

	if( !silent_mode )
	{
		std::cout << "Hit enter to exit the program.\n";
		std::cin.get(); // Wait for user to hit enter.
	}

	return error_code;
}