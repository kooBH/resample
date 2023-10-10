# CMAKE version of Ubuntu 16.04 LTS is 3.5.
cmake_minimum_required(VERSION 3.5)
project(libresample)

set(LINKLIBS)
set(COMPILE_OPTION)
set(SRC)
set(INCL)

list(APPEND INCL
	${CMAKE_CURRENT_LIST_DIR}
	
)

list(APPEND SRC
	${CMAKE_CURRENT_LIST_DIR}/configtemplate.h
	${CMAKE_CURRENT_LIST_DIR}/filterkit.h
	${CMAKE_CURRENT_LIST_DIR}/filterkit.c
	${CMAKE_CURRENT_LIST_DIR}/libresample.h
	${CMAKE_CURRENT_LIST_DIR}/resample.c
	${CMAKE_CURRENT_LIST_DIR}/resample_defs.h
	${CMAKE_CURRENT_LIST_DIR}/resamplesubs.c
)


if(MSVC)
list(APPEND LINKLIBS
	
  )
else()
list(APPEND LINKLIBS
  m
)
endif()

if(MSVC)
list(APPEND COMPILE_OPTION
	  #/openmp
	)
else()
list(APPEND COMPILE_OPTION
	 # -fopenmp
	  -O3
	)
list(APPEND LINKLIBS
	#-fopenmp
	m
)
endif()

add_executable(test testresample.c ${SRC})
target_include_directories(test	PRIVATE	${INCL}  )
target_link_libraries(test ${LINKLIBS})
target_compile_options(test PRIVATE ${COMPILE_OPTION} )
set_target_properties(test PROPERTIES  CXX_STANDARD 17 )


