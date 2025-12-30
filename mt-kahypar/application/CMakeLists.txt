add_executable(MtKaHyParExecutable mt_kahypar.cc)
set_target_properties(MtKaHyParExecutable PROPERTIES OUTPUT_NAME MtKaHyPar)
target_link_libraries(MtKaHyParExecutable MtKaHyPar-BuildSources)


# additional installation target since installation-time renaming does not seem to be possible
add_executable(MtKaHyPar-CLI mt_kahypar.cc)
target_link_libraries(MtKaHyPar-CLI MtKaHyPar-BuildSources)
