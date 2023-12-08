#include "Kodgen/InfoStructures/EntityInfo.h"

#include "Kodgen/Misc/Helpers.h"

using namespace kodgen;

EntityInfo::EntityInfo(CXCursor const& cursor, std::vector<Property>&& properties, EEntityType entityType) noexcept:
	entityType{entityType},
	name{Helpers::getString(clang_getCursorDisplayName(cursor))},
	id{Helpers::getString(clang_getCursorUSR(cursor))},
	properties{std::forward<std::vector<Property>>(properties)}
{
	auto location = clang_getCursorLocation(cursor);
	clang_getFileLocation(location, nullptr, &line, &column, &offset);
}

std::string EntityInfo::getFullName() const noexcept
{
	return (outerEntity != nullptr) ? outerEntity->getFullName() + "::" + name : name;
}

std::string EntityInfo::getFullName(CXCursor const& cursor) noexcept
{
	CXCursor parentCursor = clang_getCursorLexicalParent(cursor);

	return (!clang_equalCursors(parentCursor, clang_getNullCursor()) && parentCursor.kind != CXCursorKind::CXCursor_TranslationUnit) ?
		Helpers::getString(clang_getCursorDisplayName(parentCursor)) + "::" + Helpers::getString(clang_getCursorDisplayName(cursor)) :
		Helpers::getString(clang_getCursorDisplayName(cursor));
}

std::ostream& kodgen::operator<<(std::ostream& out_stream, EntityInfo const& entityInfo) noexcept
{
	out_stream << entityInfo.name;

	return out_stream;
}