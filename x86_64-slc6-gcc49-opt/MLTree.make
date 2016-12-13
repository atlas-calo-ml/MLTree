#-- start of make_header -----------------

#====================================
#  Library MLTree
#
#   Generated Mon Dec 12 19:38:48 2016  by jolsson
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_MLTree_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_MLTree_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_MLTree

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTree = $(MLTree_tag)_MLTree.make
cmt_local_tagfile_MLTree = $(bin)$(MLTree_tag)_MLTree.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTree = $(MLTree_tag).make
cmt_local_tagfile_MLTree = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_MLTree)
#-include $(cmt_local_tagfile_MLTree)

ifdef cmt_MLTree_has_target_tag

cmt_final_setup_MLTree = $(bin)setup_MLTree.make
cmt_dependencies_in_MLTree = $(bin)dependencies_MLTree.in
#cmt_final_setup_MLTree = $(bin)MLTree_MLTreesetup.make
cmt_local_MLTree_makefile = $(bin)MLTree.make

else

cmt_final_setup_MLTree = $(bin)setup.make
cmt_dependencies_in_MLTree = $(bin)dependencies.in
#cmt_final_setup_MLTree = $(bin)MLTreesetup.make
cmt_local_MLTree_makefile = $(bin)MLTree.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#MLTree :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'MLTree'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = MLTree/
#MLTree::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of libary_header ---------------

MLTreelibname   = $(bin)$(library_prefix)MLTree$(library_suffix)
MLTreelib       = $(MLTreelibname).a
MLTreestamp     = $(bin)MLTree.stamp
MLTreeshstamp   = $(bin)MLTree.shstamp

MLTree :: dirs  MLTreeLIB
	$(echo) "MLTree ok"

#-- end of libary_header ----------------
#-- start of library_no_static ------

#MLTreeLIB :: $(MLTreelib) $(MLTreeshstamp)
MLTreeLIB :: $(MLTreeshstamp)
	$(echo) "MLTree : library ok"

$(MLTreelib) :: $(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o
	$(lib_echo) "static library $@"
	$(lib_silent) cd $(bin); \
	  $(ar) $(MLTreelib) $?
	$(lib_silent) $(ranlib) $(MLTreelib)
	$(lib_silent) cat /dev/null >$(MLTreestamp)

#------------------------------------------------------------------
#  Future improvement? to empty the object files after
#  storing in the library
#
##	  for f in $?; do \
##	    rm $${f}; touch $${f}; \
##	  done
#------------------------------------------------------------------

#
# We add one level of dependency upon the true shared library 
# (rather than simply upon the stamp file)
# this is for cases where the shared library has not been built
# while the stamp was created (error??) 
#

$(MLTreelibname).$(shlibsuffix) :: $(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o $(use_requirements) $(MLTreestamps)
	$(lib_echo) "shared library $@"
	$(lib_silent) $(shlibbuilder) $(shlibflags) -o $@ $(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o $(MLTree_shlibflags)
	$(lib_silent) cat /dev/null >$(MLTreestamp) && \
	  cat /dev/null >$(MLTreeshstamp)

$(MLTreeshstamp) :: $(MLTreelibname).$(shlibsuffix)
	$(lib_silent) if test -f $(MLTreelibname).$(shlibsuffix) ; then \
	  cat /dev/null >$(MLTreestamp) && \
	  cat /dev/null >$(MLTreeshstamp) ; fi

MLTreeclean ::
	$(cleanup_echo) objects MLTree
	$(cleanup_silent) /bin/rm -f $(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o
	$(cleanup_silent) /bin/rm -f $(patsubst %.o,%.d,$(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o) $(patsubst %.o,%.dep,$(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o) $(patsubst %.o,%.d.stamp,$(bin)MLTreeMaker.o $(bin)MLTree_entries.o $(bin)MLTree_load.o)
	$(cleanup_silent) cd $(bin); /bin/rm -rf MLTree_deps MLTree_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
MLTreeinstallname = $(library_prefix)MLTree$(library_suffix).$(shlibsuffix)

MLTree :: MLTreeinstall ;

install :: MLTreeinstall ;

MLTreeinstall :: $(install_dir)/$(MLTreeinstallname)
ifdef CMTINSTALLAREA
	$(echo) "installation done"
endif

$(install_dir)/$(MLTreeinstallname) :: $(bin)$(MLTreeinstallname)
ifdef CMTINSTALLAREA
	$(install_silent) $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(MLTreeinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

##MLTreeclean :: MLTreeuninstall

uninstall :: MLTreeuninstall ;

MLTreeuninstall ::
ifdef CMTINSTALLAREA
	$(cleanup_silent) $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(MLTreeinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

#-- end of library_no_static ------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),MLTreeclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)MLTreeMaker.d

$(bin)$(binobj)MLTreeMaker.d :

$(bin)$(binobj)MLTreeMaker.o : $(cmt_final_setup_MLTree)

$(bin)$(binobj)MLTreeMaker.o : $(src)MLTreeMaker.cxx
	$(cpp_echo) $(src)MLTreeMaker.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(MLTree_pp_cppflags) $(lib_MLTree_pp_cppflags) $(MLTreeMaker_pp_cppflags) $(use_cppflags) $(MLTree_cppflags) $(lib_MLTree_cppflags) $(MLTreeMaker_cppflags) $(MLTreeMaker_cxx_cppflags)  $(src)MLTreeMaker.cxx
endif
endif

else
$(bin)MLTree_dependencies.make : $(MLTreeMaker_cxx_dependencies)

$(bin)MLTree_dependencies.make : $(src)MLTreeMaker.cxx

$(bin)$(binobj)MLTreeMaker.o : $(MLTreeMaker_cxx_dependencies)
	$(cpp_echo) $(src)MLTreeMaker.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(MLTree_pp_cppflags) $(lib_MLTree_pp_cppflags) $(MLTreeMaker_pp_cppflags) $(use_cppflags) $(MLTree_cppflags) $(lib_MLTree_cppflags) $(MLTreeMaker_cppflags) $(MLTreeMaker_cxx_cppflags)  $(src)MLTreeMaker.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),MLTreeclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)MLTree_entries.d

$(bin)$(binobj)MLTree_entries.d :

$(bin)$(binobj)MLTree_entries.o : $(cmt_final_setup_MLTree)

$(bin)$(binobj)MLTree_entries.o : $(src)components/MLTree_entries.cxx
	$(cpp_echo) $(src)components/MLTree_entries.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(MLTree_pp_cppflags) $(lib_MLTree_pp_cppflags) $(MLTree_entries_pp_cppflags) $(use_cppflags) $(MLTree_cppflags) $(lib_MLTree_cppflags) $(MLTree_entries_cppflags) $(MLTree_entries_cxx_cppflags) -I../src/components $(src)components/MLTree_entries.cxx
endif
endif

else
$(bin)MLTree_dependencies.make : $(MLTree_entries_cxx_dependencies)

$(bin)MLTree_dependencies.make : $(src)components/MLTree_entries.cxx

$(bin)$(binobj)MLTree_entries.o : $(MLTree_entries_cxx_dependencies)
	$(cpp_echo) $(src)components/MLTree_entries.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(MLTree_pp_cppflags) $(lib_MLTree_pp_cppflags) $(MLTree_entries_pp_cppflags) $(use_cppflags) $(MLTree_cppflags) $(lib_MLTree_cppflags) $(MLTree_entries_cppflags) $(MLTree_entries_cxx_cppflags) -I../src/components $(src)components/MLTree_entries.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),MLTreeclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)MLTree_load.d

$(bin)$(binobj)MLTree_load.d :

$(bin)$(binobj)MLTree_load.o : $(cmt_final_setup_MLTree)

$(bin)$(binobj)MLTree_load.o : $(src)components/MLTree_load.cxx
	$(cpp_echo) $(src)components/MLTree_load.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(MLTree_pp_cppflags) $(lib_MLTree_pp_cppflags) $(MLTree_load_pp_cppflags) $(use_cppflags) $(MLTree_cppflags) $(lib_MLTree_cppflags) $(MLTree_load_cppflags) $(MLTree_load_cxx_cppflags) -I../src/components $(src)components/MLTree_load.cxx
endif
endif

else
$(bin)MLTree_dependencies.make : $(MLTree_load_cxx_dependencies)

$(bin)MLTree_dependencies.make : $(src)components/MLTree_load.cxx

$(bin)$(binobj)MLTree_load.o : $(MLTree_load_cxx_dependencies)
	$(cpp_echo) $(src)components/MLTree_load.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(MLTree_pp_cppflags) $(lib_MLTree_pp_cppflags) $(MLTree_load_pp_cppflags) $(use_cppflags) $(MLTree_cppflags) $(lib_MLTree_cppflags) $(MLTree_load_cppflags) $(MLTree_load_cxx_cppflags) -I../src/components $(src)components/MLTree_load.cxx

endif

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: MLTreeclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(MLTree.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

MLTreeclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library MLTree
	-$(cleanup_silent) cd $(bin) && \rm -f $(library_prefix)MLTree$(library_suffix).a $(library_prefix)MLTree$(library_suffix).$(shlibsuffix) MLTree.stamp MLTree.shstamp
#-- end of cleanup_library ---------------
