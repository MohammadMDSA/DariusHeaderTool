/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <vector>
#include <cassert>

#include "Kodgen/Parsing/ParsingError.h"
#include "Kodgen/Parsing/ParsingSettings.h"
#include "Kodgen/Parsing/ParsingResults/ParsingResultBase.h"
#include "Kodgen/InfoStructures/NamespaceInfo.h"
#include "Kodgen/InfoStructures/StructClassInfo.h"
#include "Kodgen/InfoStructures/NestedStructClassInfo.h"
#include "Kodgen/InfoStructures/EnumInfo.h"
#include "Kodgen/InfoStructures/FunctionInfo.h"
#include "Kodgen/InfoStructures/VariableInfo.h"
#include "Kodgen/InfoStructures/StructClassTree.h"
#include "Kodgen/Misc/Filesystem.h"

namespace kodgen
{
	class FileParsingResult : public ParsingResultBase
	{
		public:
			/** Path to the parsed file. */
			fs::path						parsedFile;

			/** Unique file id */
			std::string						fileId;

			/** All namespaces contained directly under file level. */
			std::vector<NamespaceInfo>		namespaces;

			/** All classes contained directly under file level. */
			std::vector<StructClassInfo>	classes;

			/** All structs contained directly under file level. */
			std::vector<StructClassInfo>	structs;

			/** All enums contained directly under file level. */
			std::vector<EnumInfo>			enums;

			/** All functions contained directly under file level. */
			std::vector<FunctionInfo>		functions;

			/** All variables contained directory under file level. */
			std::vector<VariableInfo>		variables;

			/** Structure containing the whole struct/class hierarchy linked to parsed structs/classes. */
			StructClassTree					structClassTree;

			/**
			*	@brief Call a visitor function on each entity of the provided type(s) contained in a file.
			* 
			*	@param entityMask	All types of entities the visitor function should be called on.
			*	@param visitor		Function to call on entities.
			*/
			template <typename Functor, typename = std::enable_if_t<std::is_invocable_v<Functor, EntityInfo const&>>>
			void foreachEntityOfType(EEntityType entityMask, Functor visitor)	const	noexcept;
	};

	#include "Kodgen/Parsing/ParsingResults/FileParsingResult.inl"
}