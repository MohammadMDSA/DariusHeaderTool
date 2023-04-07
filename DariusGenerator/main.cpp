#include <Kodgen/Parsing/FileParser.h>
#include <Kodgen/CodeGen/CodeGenManager.h>
#include <Kodgen/CodeGen/Macro/MacroCodeGenUnit.h>
#include <Kodgen/CodeGen/Macro/MacroCodeGenUnitSettings.h>
#include <Kodgen/Misc/Filesystem.h>
#include <Kodgen/Misc/DefaultLogger.h>

#include "GetSetCGM.h"

void initCodeGenUnitSettings(fs::path const& outputDirectory, kodgen::MacroCodeGenUnitSettings& out_cguSettings)
{
	//All generated files will be located in WorkingDir/Include/Generated
	out_cguSettings.setOutputDirectory(outputDirectory);
	
	//Setup generated files name pattern
	out_cguSettings.setGeneratedHeaderFileNamePattern("##FILENAME##.generated.hpp");
	out_cguSettings.setGeneratedSourceFileNamePattern("##FILENAME##.sgenerated.hpp");
	out_cguSettings.setClassFooterMacroPattern("##CLASSFULLNAME##_GENERATED");
	out_cguSettings.setHeaderFileFooterMacroPattern("File_##FILENAME##_GENERATED");
}

void initCodeGenManagerSettings(fs::path const& workingDirectory, kodgen::CodeGenManagerSettings& out_generatorSettings)
{
	//Parse WorkingDir/...
	out_generatorSettings.addToProcessDirectory(workingDirectory);

	//Ignore generated files...
	out_generatorSettings.addIgnoredDirectory(workingDirectory / "Libs");
	out_generatorSettings.addIgnoredDirectory(workingDirectory / "Generated");

	//Only parse .hpp files
	out_generatorSettings.addSupportedFileExtension(".hpp");
}

void addIncludeDirectories(int argc, char** argv, kodgen::ParsingSettings& parsingSettings, kodgen::DefaultLogger logger)
{
	for (int i = 4; i < argc; i++)
	{
		auto path = fs::path(argv[i]);

		if (parsingSettings.addProjectIncludeDirectory(path))
			logger.log("Adding include path: " + path.string());
		/*else
			logger.log("Failed adding include path: " + path.string(), kodgen::ILogger::ELogSeverity::Error);*/
	}

	fs::path pchP(argv[3]);
	if (fs::exists(pchP) && !fs::is_directory(pchP))
	{
		parsingSettings.pchPath = pchP;
		parsingSettings.shouldUsePch = true;
	}
}

bool initParsingSettings(kodgen::ParsingSettings& parsingSettings)
{
	
	//We abort parsing if we encounter a single error while parsing
	parsingSettings.shouldAbortParsingOnFirstError = true;

	//Each property will be separed by a ,
	parsingSettings.propertyParsingSettings.propertySeparator = ',';

	//Subproperties are surrounded by []
	parsingSettings.propertyParsingSettings.argumentEnclosers[0] = '[';
	parsingSettings.propertyParsingSettings.argumentEnclosers[1] = ']';

	//Each subproperty will be separed by a ,
	parsingSettings.propertyParsingSettings.argumentSeparator = ',';

	//Define the macros to use for each entity type
	parsingSettings.propertyParsingSettings.namespaceMacroName	= "DNamespace";
	parsingSettings.propertyParsingSettings.classMacroName		= "DClass";
	parsingSettings.propertyParsingSettings.structMacroName		= "DStruct";
	parsingSettings.propertyParsingSettings.fieldMacroName		= "DVariable";
	parsingSettings.propertyParsingSettings.fieldMacroName		= "DField";
	parsingSettings.propertyParsingSettings.functionMacroName	= "DFunction";
	parsingSettings.propertyParsingSettings.methodMacroName		= "DMethod";
	parsingSettings.propertyParsingSettings.enumMacroName		= "DEnum";
	parsingSettings.propertyParsingSettings.enumValueMacroName	= "DEnumVal";

	parsingSettings.shouldParseAllNamespaces = true;
	
	parsingSettings.shouldLogDiagnostic = false;

	parsingSettings.cppVersion = kodgen::ECppVersion::Cpp20;

	//This is setup that way for CI tools only
	//In reality, the compiler used by the user machine running the generator should be set.
	//It has nothing to see with the compiler used to compile the generator.
#if defined(__GNUC__)
	return parsingSettings.setCompilerExeName("g++");
#elif defined(__clang__)
	return parsingSettings.setCompilerExeName("clang++");
#elif defined(_MSC_VER)
	return parsingSettings.setCompilerExeName("msvc");
#else
	return false;	//Unsupported compiler
#endif
}

int main(int argc, char** argv)
{
	kodgen::DefaultLogger logger;

	if (argc <= 1)
	{
		logger.log("No working directory provided as first program argument", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	if (argc <= 2)
	{
		logger.log("No output directory provided as second program argument",
			kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	fs::path workingDirectory = argv[1];
	fs::path outputDirectory = argv[2];

	if (!fs::is_directory(workingDirectory))
	{
		logger.log("Provided working directory is not a directory or doesn't exist", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	if (fs::exists(outputDirectory) && !fs::is_directory(outputDirectory))
	{
		logger.log("Provided output directory is not a directory", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	logger.log("Working Directory: " + workingDirectory.string());
	logger.log("Output Directory: " + outputDirectory.string());

	//Setup FileParser
	kodgen::FileParser fileParser;
	fileParser.logger = &logger;

	auto& settings = fileParser.getSettings();

	addIncludeDirectories(argc, argv, settings, logger);
	if (!initParsingSettings(settings))
	{
		logger.log("Compiler could not be set because it is not supported on the current machine or vswhere could not be found (Windows|MSVC only).", kodgen::ILogger::ELogSeverity::Error);
		return EXIT_FAILURE;
	}

	//Setup code generation unit
	kodgen::MacroCodeGenUnit codeGenUnit;
	codeGenUnit.logger = &logger;

	kodgen::MacroCodeGenUnitSettings cguSettings;
	initCodeGenUnitSettings(outputDirectory, cguSettings);
	codeGenUnit.setSettings(cguSettings);

	//Add code generation modules
	GetSetCGM getSetCodeGenModule;
	codeGenUnit.addModule(getSetCodeGenModule);

	//Setup CodeGenManager
	kodgen::CodeGenManager codeGenMgr;
	codeGenMgr.logger = &logger;

	initCodeGenManagerSettings(workingDirectory, codeGenMgr.settings);

	//Kick-off code generation
	kodgen::CodeGenResult genResult = codeGenMgr.run(fileParser, codeGenUnit, false);

	if (genResult.completed)
	{
		logger.log("Generation completed successfully in " + std::to_string(genResult.duration) + " seconds.");
	}
	else
	{
		logger.log("An error happened during code generation.", kodgen::ILogger::ELogSeverity::Error);
	}

	return EXIT_SUCCESS;
}