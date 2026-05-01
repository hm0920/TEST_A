add_executable(Tetris
    main.cpp
    Tetromino.cpp
    GameGrid.cpp
    Game.cpp
    InputHandler.cpp
    Renderer.cpp
    ParticleSystem.cpp
)

target_include_directories(Tetris PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Tetris PRIVATE raylib)

set_target_properties(Tetris PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
