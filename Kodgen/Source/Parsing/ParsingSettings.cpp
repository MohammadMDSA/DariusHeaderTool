#include "Kodgen/Parsing/ParsingSettings.h"

#include "Kodgen/Misc/CompilerHelpers.h"
#include "Kodgen/Misc/TomlUtility.h"
#include "Kodgen/Misc/ILogger.h"
#include "Kodgen/Misc/Helpers.h"

using namespace kodgen;

opt::optional<ECppVersion> ParsingSettings::getMatchingCppVersion(uint8 cppVersionAsInt) noexcept
{
	switch (cppVersionAsInt)
	{
		case 17u:
			return ECppVersion::Cpp17;

		case 20u:
			return ECppVersion::Cpp20;

		default:
			return opt::nullopt;
	}
}

void ParsingSettings::init(ILogger* logger) noexcept
{
	refreshCompilationArguments(logger);
}

void ParsingSettings::refreshBuildCommandStrings(ILogger* logger) noexcept
{
	_cppVersionCommandLine = "-std=c++" + std::to_string(static_cast<uint8>(cppVersion));

	_namespacePropertyMacro	= "-D" + propertyParsingSettings.namespaceMacroName	+ "(...)=__attribute__((annotate(\"KGN:\"#__VA_ARGS__)))";
	_classPropertyMacro		= "-D" + propertyParsingSettings.classMacroName		+ "(...)=__attribute__((annotate(\"KGC:\"#__VA_ARGS__)))";
	_structPropertyMacro	= "-D" + propertyParsingSettings.structMacroName	+ "(...)=__attribute__((annotate(\"KGS:\"#__VA_ARGS__)))";
	_variablePropertyMacro	= "-D" + propertyParsingSettings.variableMacroName	+ "(...)=__attribute__((annotate(\"KGV:\"#__VA_ARGS__)))";
	_fieldPropertyMacro		= "-D" + propertyParsingSettings.fieldMacroName		+ "(...)=__attribute__((annotate(\"KGF:\"#__VA_ARGS__)))";
	_functionPropertyMacro	= "-D" + propertyParsingSettings.functionMacroName	+ "(...)=__attribute__((annotate(\"KGFu:\"#__VA_ARGS__)))";
	_methodPropertyMacro	= "-D" + propertyParsingSettings.methodMacroName	+ "(...)=__attribute__((annotate(\"KGM:\"#__VA_ARGS__)))";
	_enumPropertyMacro		= "-D" + propertyParsingSettings.enumMacroName		+ "(...)=__attribute__((annotate(\"KGE:\"#__VA_ARGS__)))";
	_enumValuePropertyMacro	= "-D" + propertyParsingSettings.enumValueMacroName	+ "(...)=__attribute__((annotate(\"KGEV:\"#__VA_ARGS__)))";
	_pchPath				= "-include-pch=" + pchPath.lexically_normal().string();

	//Setup project include directories
	std::vector<fs::path> nativeIncludeDirectories;

	try
	{
		if (!getCompilerExeName().empty())
		{
			nativeIncludeDirectories = CompilerHelpers::getCompilerNativeIncludeDirectories(getCompilerExeName());

			if (nativeIncludeDirectories.empty())
			{
				logger->log("Could not find any include directory from the specified compiler. Make sure the compiler is installed on your computer.", kodgen::ILogger::ELogSeverity::Warning);
			}
			else
			{
				logger->log("Using native include directory:");
				for (auto const& dir : nativeIncludeDirectories)
				{
					logger->log("- " + dir.string());
				}

			}
		}
		else
		{
			logger->log("ParsingSettings::compilerExeName must be set to parse files.", kodgen::ILogger::ELogSeverity::Error);
		}
	}
	catch (std::exception const& e)
	{
		logger->log(e.what(), kodgen::ILogger::ELogSeverity::Error);
	}

	_projectIncludeDirs.clear();
	_projectIncludeDirs.reserve(getProjectIncludeDirectories().size() + nativeIncludeDirectories.size());

	//Add user manually specified include directories
	for (fs::path const& includeDir : getProjectIncludeDirectories())
	{
		_projectIncludeDirs.emplace_back("-I" + includeDir.string());
	}

	//Add compiler native include directories
	for (fs::path& includeDir : nativeIncludeDirectories)
	{
		_projectIncludeDirs.emplace_back("-I" + includeDir.string());
	}
}

void ParsingSettings::refreshCompilationArguments(ILogger* logger) noexcept
{
	_compilationArguments.clear();

	refreshBuildCommandStrings(logger);

#if KODGEN_DEV
	constexpr size_t baseCompilationArgCount = 4u;	//-xc++, -v, -std=c++1z & _kodgenParsingMacro
#else
	constexpr size_t baseCompilationArgCount = 3u;	//-xc++, -std=c++1z & _kodgenParsingMacro
#endif

													/**
													*	3 to include -xc++, -std=c++1z & _kodgenParsingMacro
													*
													*	9 because we make an additional parameter per possible entity
													*	Namespace, Class, Struct, Variable, Field, Function, Method, Enum, EnumValue
													*/
	_compilationArguments.reserve(baseCompilationArgCount + 9u + _projectIncludeDirs.size());

	//Parsing C++
	_compilationArguments.emplace_back("-xc++");

#if KODGEN_DEV
	_compilationArguments.emplace_back("-v");
#endif

	//Use the user-specified C++ version
	_compilationArguments.emplace_back(_cppVersionCommandLine.data());

	//Macro set when we are parsing with Kodgen
	_compilationArguments.emplace_back(_kodgenParsingMacro.data());

	_compilationArguments.emplace_back(_namespacePropertyMacro.data());
	_compilationArguments.emplace_back(_classPropertyMacro.data());
	_compilationArguments.emplace_back(_structPropertyMacro.data());
	_compilationArguments.emplace_back(_variablePropertyMacro.data());
	_compilationArguments.emplace_back(_fieldPropertyMacro.data());
	_compilationArguments.emplace_back(_methodPropertyMacro.data());
	_compilationArguments.emplace_back(_functionPropertyMacro.data());
	_compilationArguments.emplace_back(_enumPropertyMacro.data());
	_compilationArguments.emplace_back(_enumValuePropertyMacro.data());
	_compilationArguments.emplace_back("-DNOMINMAX");
	_compilationArguments.emplace_back("-DCODEGEN_BUILD");

	for (std::string const& includeDir : _projectIncludeDirs)
	{
		_compilationArguments.emplace_back(includeDir.data());
	}

	// PCH
	if (shouldUsePch)
	{
		logger->log("Using pch: " + pchPath.string());
		_compilationArguments.emplace_back("-Xclang");
		//_compilationArguments.emplace_back("-include-pch");
		//_compilationArguments.emplace_back("-Xclang");
		//_compilationArguments.emplace_back(pchPath.string().c_str());
		_compilationArguments.emplace_back(_pchPath.data());
	}

}

bool ParsingSettings::loadSettingsValues(toml::value const& tomlData, ILogger* logger) noexcept
{
	if (tomlData.contains(_tomlSectionName))
	{
		toml::value const& tomlParsingSettings = toml::find(tomlData, _tomlSectionName);

		loadCppVersion(tomlParsingSettings, logger);
		loadShouldParseAllEntities(tomlParsingSettings, logger);
		loadShouldAbortParsingOnFirstError(tomlParsingSettings, logger);
		loadShouldLogDiagnostic(tomlParsingSettings, logger);
		loadCompilerExeName(tomlParsingSettings, logger);
		loadProjectIncludeDirectories(tomlParsingSettings, logger);

		return propertyParsingSettings.loadSettingsValues(tomlParsingSettings, logger);
	}
	else if (logger != nullptr)
	{
		logger->log("Could not find the [" + std::string(_tomlSectionName) + "] section in the TOML file.", ILogger::ELogSeverity::Warning);
	}

	return false;
}

void ParsingSettings::loadCppVersion(toml::value const& tomlFileParsingSettings, ILogger* logger) noexcept
{
	uint8 loadedCppVersion;

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "cppVersion", loadedCppVersion, logger))
	{
		opt::optional<ECppVersion> cppVersionEnumValue = ParsingSettings::getMatchingCppVersion(loadedCppVersion);

		//Check that the loaded c++ version is valid
		if (cppVersionEnumValue.has_value())
		{
			cppVersion = cppVersionEnumValue.value();

			if (logger != nullptr)
			{
				logger->log("[TOML] Load cppVersion: c++" + std::to_string(static_cast<uint8>(cppVersion)));
			}
		}
		else if (logger != nullptr)
		{
			logger->log("[TOML] Failed to load cppVersion: c++" + std::to_string(loadedCppVersion) + " is not supported. Supported versions are 17 and 20.", ILogger::ELogSeverity::Warning);
		}
	}
}

void ParsingSettings::loadShouldParseAllEntities(toml::value const& tomlFileParsingSettings, ILogger* logger) noexcept
{
	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllNamespaces", shouldParseAllNamespaces, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllNamespaces: " + Helpers::toString(shouldParseAllNamespaces));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllClasses", shouldParseAllClasses, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllClasses: " + Helpers::toString(shouldParseAllClasses));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllStructs", shouldParseAllStructs, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllStructs: " + Helpers::toString(shouldParseAllStructs));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllVariables", shouldParseAllVariables, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllVariables: " + Helpers::toString(shouldParseAllVariables));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllFields", shouldParseAllFields, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllFields: " + Helpers::toString(shouldParseAllFields));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllFunctions", shouldParseAllFunctions, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllFunctions: " + Helpers::toString(shouldParseAllFunctions));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllMethods", shouldParseAllMethods, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllMethods: " + Helpers::toString(shouldParseAllMethods));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllEnums", shouldParseAllEnums, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllEnums: " + Helpers::toString(shouldParseAllEnums));
	}

	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldParseAllEnumValues", shouldParseAllEnumValues, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldParseAllEnumValues: " + Helpers::toString(shouldParseAllEnumValues));
	}
}

void ParsingSettings::loadShouldAbortParsingOnFirstError(toml::value const& tomlFileParsingSettings, ILogger* logger) noexcept
{
	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldAbortParsingOnFirstError", shouldAbortParsingOnFirstError, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldAbortParsingOnFirstError: " + Helpers::toString(shouldAbortParsingOnFirstError));
	}
}

void ParsingSettings::loadShouldLogDiagnostic(toml::value const& tomlFileParsingSettings, ILogger* logger) noexcept
{
	if (TomlUtility::updateSetting(tomlFileParsingSettings, "shouldLogDiagnostic", shouldLogDiagnostic, logger) && logger != nullptr)
	{
		logger->log("[TOML] Load shouldLogDiagnostic: " + Helpers::toString(shouldLogDiagnostic));
	}
}

void ParsingSettings::loadCompilerExeName(toml::value const& parsingSettings, ILogger* logger) noexcept
{
	std::string compilerExeName;

	if (TomlUtility::updateSetting(parsingSettings, "compilerExeName", compilerExeName, logger))
	{
		bool success = setCompilerExeName(compilerExeName);

		if (logger != nullptr)
		{
			if (success)
			{
				logger->log("[TOML] Load compiler: " + compilerExeName);
			}
			else
			{
				logger->log("[TOML] " + compilerExeName + " doesn't exist, is not supported or could not be run on the current computer.", ILogger::ELogSeverity::Error);

#if _WIN32
				//Failed while loading MSVC, VSWhere might not be located next to the executable
				if (CompilerHelpers::isMSVC(compilerExeName))
				{
					logger->log("[TOML] Make sure that vswhere.exe is located next to your executable.", ILogger::ELogSeverity::Error);
				}
#endif
			}
		}
	}
}

void ParsingSettings::loadProjectIncludeDirectories(toml::value const& parsingSettings, ILogger* logger) noexcept
{
	std::unordered_set<fs::path, PathHash> projectIncludeDirectories;

	if (TomlUtility::updateSetting(parsingSettings, "projectIncludeDirectories", projectIncludeDirectories, logger))
	{
		for (fs::path const& includeDirectoryPath : projectIncludeDirectories)
		{
			bool success = addProjectIncludeDirectory(includeDirectoryPath);

			//Log load result
			if (logger != nullptr)
			{
				if (success)
				{
					logger->log("[TOML] Load new project include directory: " + FilesystemHelpers::sanitizePath(includeDirectoryPath).string());
				}
				else
				{
					logger->log("[TOML] Discard project include directory as it doesn't exist or is not a directory: " + includeDirectoryPath.string(), ILogger::ELogSeverity::Warning);
				}
			}
		}
	}
}

bool ParsingSettings::addProjectIncludeDirectory(fs::path const& directoryPath) noexcept
{
	fs::path sanitizedPath = FilesystemHelpers::sanitizePath(directoryPath);

	if (!sanitizedPath.empty() && fs::is_directory(sanitizedPath))
	{
		return _projectIncludeDirectories.emplace(std::move(sanitizedPath)).second;
	}

	return false;
}

void ParsingSettings::removeProjectIncludeDirectory(fs::path const& directoryPath) noexcept
{
	_projectIncludeDirectories.erase(FilesystemHelpers::sanitizePath(directoryPath));
}

void ParsingSettings::clearProjectIncludeDirectories() noexcept
{
	_projectIncludeDirectories.clear();
}

std::unordered_set<fs::path, PathHash> const& ParsingSettings::getProjectIncludeDirectories() const noexcept
{
	return _projectIncludeDirectories;
}

std::string const& ParsingSettings::getCompilerExeName() const noexcept
{
	return _compilerExeName;
}

std::vector<char const*> const& ParsingSettings::getCompilationArguments() const noexcept
{
	return _compilationArguments;
}

bool ParsingSettings::setCompilerExeName(std::string const& compilerExeName) noexcept
{
	if (CompilerHelpers::isSupportedCompiler(compilerExeName))
	{
		_compilerExeName = compilerExeName;

		return true;
	}

	return false;
}