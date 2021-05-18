/**
*	Copyright (c) 2021 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include <vector>
#include <string>

namespace kodgen
{
	//Forward declarations
	class EntityInfo;
	class CodeGenModule;
	struct CodeGenData;

	class CodeGenModuleGroup
	{
		private:
			/** Collection of generation modules. */
			std::vector<CodeGenModule*>	_generationModules;

		public:
			/**
			*	@brief Add a module to the internal list of generation modules.
			* 
			*	@param generationModule The generation module to add.
			*/
			void addModule(CodeGenModule& generationModule)				noexcept;

			/**
			*	@brief Remove a module from the internal list of generation modules.
			* 
			*	@param generationModule The generation module to remove.
			* 
			*	@return true if a module has been successfully removed, else false.
			*/
			bool removeModule(CodeGenModule& generationModule)			noexcept;

			/**
			*	@brief Generate code by executing the generateCode method of each registered generation module.
			* 
			*	@param entity		Entity the code is generated for. Might be nullptr, in which case the code is not generated for a specific entity.
			*	@param data			Code generation data (yes).
			*	@param out_result	String filled with the generated code.
			* 
			*	@return true if all registered generation modules completed their code generation successfully, else false.
			*/
			bool generateCode(EntityInfo const*		entity,
							  CodeGenData&			data,
							  std::string&			out_result)	const	noexcept;
	};
}