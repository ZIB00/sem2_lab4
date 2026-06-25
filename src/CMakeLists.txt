add_executable(Lazy_Sequence 
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    TabBase.cpp
    TabBase.hpp
    TabAlgebra.cpp
    TabAlgebra.hpp
    TabOrdinals.cpp
    TabOrdinals.hpp
    SequenceWrapper.hpp
)

target_include_directories(Lazy_Sequence PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Lazy_Sequence PRIVATE Qt6::Widgets)