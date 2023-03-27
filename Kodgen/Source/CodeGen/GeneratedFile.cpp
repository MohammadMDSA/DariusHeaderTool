#include "Kodgen/CodeGen/GeneratedFile.h"

using namespace kodgen;

GeneratedFile::GeneratedFile(fs::path&& generatedFilePath, fs::path const& sourceFilePath) noexcept :
	_path{ std::forward<fs::path>(generatedFilePath) },
	_sourceFilePath{ sourceFilePath }
{
	_path_temp = _path.string() + ".tmp";
	_streamToFile = std::ofstream(_path_temp.string(), std::ios::out | std::ios::trunc);
}

GeneratedFile::~GeneratedFile() noexcept
{
	_streamToFile.close();

	if (hasNewContent())
	{
		fs::copy_file(_path_temp, _path, fs::copy_options::overwrite_existing);
	}
	fs::remove(_path_temp);
}

bool GeneratedFile::hasNewContent() const
{
	if (!fs::exists(_path))
		return true;

	std::ifstream lFile(_path.c_str(), std::ifstream::in | std::ifstream::binary);
	std::ifstream rFile(_path_temp.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!lFile.is_open() || !rFile.is_open())
	{
		return true;
	}

#define BUFFER_SIZE 2048

	char* lBuffer = new char[BUFFER_SIZE]();
	char* rBuffer = new char[BUFFER_SIZE]();

	do {
		lFile.read(lBuffer, BUFFER_SIZE);
		rFile.read(rBuffer, BUFFER_SIZE);

		if (std::memcmp(lBuffer, rBuffer, BUFFER_SIZE) != 0)
		{
			delete[] lBuffer;
			delete[] rBuffer;
			return true;
		}
	} while (lFile.good() || rFile.good());

	delete[] lBuffer;
	delete[] rBuffer;
	return false;
}

void GeneratedFile::writeLine(std::string const& line) noexcept
{
	_streamToFile << line << "\n";
}

void GeneratedFile::writeLine(std::string&& line) noexcept
{
	_streamToFile << std::forward<std::string>(line) << "\n";
}

void GeneratedFile::writeLines(std::string const& line) noexcept
{
	writeLine(line);
}

void GeneratedFile::writeLines(std::string&& line) noexcept
{
	writeLine(std::forward<std::string>(line));
}

void GeneratedFile::expandWriteMacroLines(std::string const& line) noexcept
{
	writeLine(line + "\n");
}

void GeneratedFile::expandWriteMacroLines(std::string&& line) noexcept
{
	writeLine(std::forward<std::string>(line) + "\n");
}

void GeneratedFile::writeMacro(std::string&& macroName) noexcept
{
	writeLine("#define " + std::forward<std::string>(macroName));
}

void GeneratedFile::undefMacro(std::string const& macroName) noexcept
{
	writeLine("#ifdef " + macroName + "\n\t#undef " + macroName + "\n#endif");
}

fs::path const& GeneratedFile::getPath() const noexcept
{
	return _path;
}

fs::path const& GeneratedFile::getSourceFilePath() const noexcept
{
	return _sourceFilePath;
}