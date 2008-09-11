ADD_EXECUTABLE(AimsRelax
anneal.cc
cliques.cc
iograph.cc
main.cc
meshdistance.cc
sites.cc
)

TARGET_LINK_LIBRARIES(AimsRelax
aimsalgo
)

INSTALL(TARGETS AimsRelax DESTINATION bin/)
