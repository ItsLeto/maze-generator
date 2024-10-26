set(CXX_FLAGS "-Wall -Wextra -Werror -Wconversion -Wno-format -Wno-psabi")

# need to add -pthread to resolve the build error "undefined reference to symbol 'pthread_create@@GLIBC'"
# https://stackoverflow.com/questions/34143265/undefined-reference-to-symbol-pthread-createglibc-2-2-5
# TODO(lhe5wi): [2022-11-24] check if this flag is needed by all target or if it can be moved to a specific target
set(EXTRA_CXX_FLAGS "-pthread")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS} ${EXTRA_CXX_FLAGS}")
