if(BUILD_DOXYGEN_DOCS)
   configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
   add_custom_command(OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/index.html
      COMMAND ${DOXYGEN_EXECUTABLE}
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      VERBATIM)
   # Runs only once
   add_custom_target(apidoc-all ALL
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/index.html)
   # For manual updates
   add_custom_target(apidoc
      COMMAND ${DOXYGEN_EXECUTABLE}
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      VERBATIM)
endif()

if(BUILD_MANUAL)
   set(MANUAL_SRCS
      manual/algorithms.tex
      manual/benchmarks.tex
      manual/changelogs.tex
      manual/contributors.tex
      manual/cover.tex
      manual/data.tex
      manual/design-decisions.tex
      manual/meshconfig.tex
      manual/meshsetup.tex
      manual/entities.tex
      manual/figures/conforming.eps
      manual/figures/entities.eps
      manual/figures/interface-tets.eps
      manual/figures/logo_asc.eps
      manual/figures/logo_px200.eps
      manual/figures/non-conforming.eps
      manual/figures/note.eps
      manual/figures/orientation-hypercube.eps
      manual/figures/orientation-simplex.eps
      manual/figures/recursive-inheritance.eps
      manual/figures/samplemesh.eps
      manual/figures/storage-vglob-cglob.eps
      manual/figures/storage-vglob-eglob-cglob.eps
      manual/figures/tip.eps
      manual/figures/TU_Signet_CMYK.eps
      manual/figures/voronoi.eps
      manual/IEEEtran_v1.13.bst
      manual/installation.tex
      manual/internals.tex
      manual/introduction.tex
      manual/io.tex
      manual/iterators.tex
      manual/keywords.tex
      manual/license.tex
      manual/orientations.tex
      manual/refinement.tex
      manual/setup.tex
      manual/versioning.tex
      manual/viennagrid.bib
      manual/viennagrid.tex)

   set(MANUAL_OBJS
      "${CMAKE_CURRENT_BINARY_DIR}/viennagrid.pdf"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.bbl"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.blg"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.dvi"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.idx"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.log"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.out"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.ps"
      "${CMAKE_CURRENT_BINARY_DIR}/manual/viennagrid.toc")
   foreach(f IN LISTS MANUAL_SRCS)
      configure_file(${f} ${CMAKE_CURRENT_BINARY_DIR}/${f} COPYONLY)
      if(f MATCHES "(.*)\\.tex$")
         list(APPEND MANUAL_OBJS
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_MATCH_1}.aux")
      endif()
   endforeach()

   add_custom_command(OUTPUT ${MANUAL_OBJS}
      COMMAND ${LATEX_COMPILER} viennagrid.tex
      COMMAND ${LATEX_COMPILER} viennagrid.tex
      COMMAND ${BIBTEX_COMPILER} viennagrid
      COMMAND ${LATEX_COMPILER} viennagrid.tex
      COMMAND ${BIBTEX_COMPILER} viennagrid
      COMMAND ${LATEX_COMPILER} viennagrid.tex
      COMMAND ${DVIPS_CONVERTER} -Ppdf -G0 -ta4 viennagrid.dvi
      COMMAND ${PS2PDF_CONVERTER} -dCompatibilityLevel=1.3
         -dMAxSubsetPct=100 -dSubsetFonts=true
         -dEmbedAllFonts=true -sPAPERSIZE=a4 -dAutoRotatePages=/None
         -dOptimize=true viennagrid.ps ../viennagrid.pdf
      DEPENDS ${MANUAL_SRCS}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/manual
      COMMENT "Generating manual viennagrid.pdf"
      VERBATIM)

   add_custom_target(manual ALL
      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/viennagrid.pdf")
endif()
