# fetch glm
include(FetchContent)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

# link glm to the main library
target_link_libraries(glviskit_lib PUBLIC glm::glm)