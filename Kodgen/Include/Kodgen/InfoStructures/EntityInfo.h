/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <clang-c/Index.h>

#include "Kodgen/Misc/FundamentalTypes.h"
#include "Kodgen/InfoStructures/EEntityType.h"
#include "Kodgen/Properties/Property.h"

namespace kodgen
{
	class EntityInfo
	{
		public:
			/** Type of entity. */
			EEntityType				entityType	= EEntityType::Undefined;
			
			/** Name of the entity. */
			std::string				name;
			
			/** Unique id of the entity. */
			std::string				id;

			/** Line number of the entity in the file */
			unsigned int			line;

			/** Column location the entity in the line */
			unsigned int			column;

			/** Offset of the entity in the file */
			unsigned int			offset;

			/** Entity this entity is contained into, nullptr if none (file level). */
			EntityInfo const*		outerEntity	= nullptr;
			
			/** All properties bound to this entity. */
			std::vector<Property>	properties;

			EntityInfo()										= default;
			EntityInfo(CXCursor const&			cursor,
					   std::vector<Property>&&	propertyGroup,
					   EEntityType				entityType)		noexcept;

			/**
			*	@brief Generates the full name of this entity (with outer entities) from a clang cursor.
			* 
			*	@param cursor The target cursor.
			*	
			*	@return The full name of the cursor entity.
			*/
			static std::string getFullName(CXCursor const& cursor)	noexcept;

			/**
			*	@brief Generates the full name of this entity (with outer entities).
			*	
			*	@return The full name of the entity.
			*/
			std::string getFullName()						const	noexcept;
	};

	std::ostream& operator<<(std::ostream& out_stream, EntityInfo const&) noexcept;
}