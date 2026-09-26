
find_package(Git)

if(Git_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.git)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE git_commit
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} log -1 --format=%cs
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE git_date
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} branch --show-current
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE git_branch
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --is-shallow-repository
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE git_is_shallow
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(git_is_shallow)
		set(git_revision "git-${git_commit}")
	else()
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			OUTPUT_VARIABLE git_revision
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(git_branch STREQUAL "main" OR git_branch STREQUAL "")
			set(git_revision "${git_revision}-git-${git_commit}")
		else()
			set(git_revision "${git_branch}-${git_revision}-git-${git_commit}")
		endif()
	endif()
	message(STATUS "${PROJECT_NAME} ${git_branch} revision ${git_revision}, ${git_date}")
	list(APPEND RADIANT_COMMON_DEFINITIONS RADIANT_GIT_REVISION=\"${git_revision}\" RADIANT_GIT_DATE=\"${git_date}\" RADIANT_GIT_BRANCH=\"${git_branch}\")
endif()
