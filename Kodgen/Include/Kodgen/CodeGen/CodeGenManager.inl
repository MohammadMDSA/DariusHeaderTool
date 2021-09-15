/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

template <typename FileParserType, typename CodeGenUnitType>
void CodeGenManager::processFiles(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, std::set<fs::path> const& toProcessFiles, uint8 iterationCount, CodeGenResult& out_genResult) noexcept
{
	std::vector<std::shared_ptr<TaskBase>> generationTasks;

	//Reserve enough space for all tasks
	generationTasks.reserve(toProcessFiles.size() * iterationCount);

	//Lock the thread pool until all tasks have been pushed to avoid competing for the tasks mutex
	_threadPool.setIsRunning(false);

	//Launch all parsing -> generation processes
	std::shared_ptr<TaskBase> parsingTask;
	std::shared_ptr<TaskBase> generationTask;
	
	for (fs::path const& file : toProcessFiles)
	{
		auto parsingTaskLambda = [&fileParser, &file](TaskBase*) -> FileParsingResult
		{
			//Copy a parser for this task
			FileParserType		fileParserCopy = fileParser;
			FileParsingResult	parsingResult;

			fileParserCopy.parse(file, parsingResult);

			return parsingResult;
		};

		auto generationTaskLambda = [&codeGenUnit](TaskBase* parsingTask) -> CodeGenResult
		{
			CodeGenResult out_generationResult;

			//Copy the generation unit model to have a fresh one for this generation unit
			CodeGenUnitType	generationUnit = codeGenUnit;

			//Get the result of the parsing task
			FileParsingResult parsingResult = TaskHelper::getDependencyResult<FileParsingResult>(parsingTask, 0u);

			//Generate the file if no errors occured during parsing
			if (parsingResult.errors.empty())
			{
				out_generationResult.completed = generationUnit.generateCode(parsingResult);
			}

			return out_generationResult;

		};

		//Add file to the list of parsed files before starting the task to avoid having to synchronize threads
		out_genResult.parsedFiles.push_back(file);

		for (int i = 0; i < iterationCount; i++)
		{
			//Parse files
			//For multiple iterations on a same file, the parsing task depends on the previous generation task for the same file
			parsingTask = _threadPool.submitTask(parsingTaskLambda, ((i != 0) ? std::vector<std::shared_ptr<TaskBase>>{ generationTask } : std::vector<std::shared_ptr<TaskBase>>{}));

			//Generate code
			generationTask = generationTasks.emplace_back(_threadPool.submitTask(generationTaskLambda, { parsingTask }));
		}
	}

	_threadPool.setIsRunning(true);
	_threadPool.joinWorkers();

	//Merge all generation results together
	for (std::shared_ptr<TaskBase>& task : generationTasks)
	{
		out_genResult.mergeResult(TaskHelper::getResult<CodeGenResult>(task.get()));
	}
}

template <typename FileParserType, typename CodeGenUnitType>
CodeGenResult CodeGenManager::run(FileParserType& fileParser, CodeGenUnitType& codeGenUnit, bool forceRegenerateAll, uint8 iterationCount) noexcept
{
	//Check FileParser validity
	static_assert(std::is_base_of_v<FileParser, FileParserType>, "fileParser type must be a derived class of kodgen::FileParser.");
	static_assert(std::is_copy_constructible_v<FileParserType>, "The provided file parser must be copy-constructible.");

	//Check FileGenerationUnit validity
	static_assert(std::is_base_of_v<CodeGenUnit, CodeGenUnitType>, "codeGenUnit type must be a derived class of kodgen::CodeGenUnit.");
	static_assert(std::is_copy_constructible_v<CodeGenUnitType>, "The CodeGenUnit you provide must be copy-constructible.");

	CodeGenResult genResult;
	genResult.completed = true;

	if (!checkGenerationSetup(fileParser, codeGenUnit))
	{
		genResult.completed = false;
	}
	else
	{
		//Start timer here
		auto				start			= std::chrono::high_resolution_clock::now();
		std::set<fs::path>	filesToProcess	= identifyFilesToProcess(codeGenUnit, genResult, forceRegenerateAll);

		//Don't setup anything if there are no files to generate
		if (filesToProcess.size() > 0u)
		{
			//Initialize the parsing settings to setup parser compilation arguments.
			//parsingSettings can't be nullptr since it has been checked in the checkGenerationSetup call.
			fileParser.getSettings().init(logger);

			generateMacrosFile(fileParser.getSettings(), codeGenUnit.getSettings()->getOutputDirectory());

			//Start files processing
			processFiles(fileParser, codeGenUnit, filesToProcess, iterationCount, genResult);
		}

		genResult.duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001f;
	}
	
	return genResult;
}