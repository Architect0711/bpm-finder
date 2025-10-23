# Tools subdirectory - executables for testing DSP stages in isolation

# PipelineResultInitializationStage test tool
add_executable(PipelineResultInitialization
        dsp/time_domain_onset_detection/PipelineResultInitializationMain.cpp
)

# Link against the main library and nlohmann_json
target_link_libraries(PipelineResultInitialization
        bpm-finder-lib
        nlohmann_json::nlohmann_json
)

# Set output directory for tools
set_target_properties(PipelineResultInitialization
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

# PipelineStageTest test tool
add_executable(PipelineStageTest
        dsp/time_domain_onset_detection/PipelineStageMain.cpp
)

# Link against the main library and nlohmann_json
target_link_libraries(PipelineStageTest
        bpm-finder-lib
        nlohmann_json::nlohmann_json
)

# Set output directory for tools
set_target_properties(PipelineStageTest
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)