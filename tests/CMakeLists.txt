function(qtlibarchive_add_unit_test Name)
    add_executable("${Name}" "${Name}/${Name}.cpp")
    target_link_libraries("${Name}" PUBLIC Qt5::Test Qt::LibArchive)
    add_test(NAME ${Name} COMMAND ${Name})
endfunction()

find_package(Qt5 COMPONENTS Test REQUIRED)

qtlibarchive_add_unit_test(BasicFileIoTest)
