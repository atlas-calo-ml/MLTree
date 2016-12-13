#-- start of make_header -----------------

#====================================
#  Document install_root_include_path
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

cmt_install_root_include_path_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_install_root_include_path_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_install_root_include_path

MLTree_tag = $(tag)

#cmt_local_tagfile_install_root_include_path = $(MLTree_tag)_install_root_include_path.make
cmt_local_tagfile_install_root_include_path = $(bin)$(MLTree_tag)_install_root_include_path.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_install_root_include_path = $(MLTree_tag).make
cmt_local_tagfile_install_root_include_path = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_install_root_include_path)
#-include $(cmt_local_tagfile_install_root_include_path)

ifdef cmt_install_root_include_path_has_target_tag

cmt_final_setup_install_root_include_path = $(bin)setup_install_root_include_path.make
cmt_dependencies_in_install_root_include_path = $(bin)dependencies_install_root_include_path.in
#cmt_final_setup_install_root_include_path = $(bin)MLTree_install_root_include_pathsetup.make
cmt_local_install_root_include_path_makefile = $(bin)install_root_include_path.make

else

cmt_final_setup_install_root_include_path = $(bin)setup.make
cmt_dependencies_in_install_root_include_path = $(bin)dependencies.in
#cmt_final_setup_install_root_include_path = $(bin)MLTreesetup.make
cmt_local_install_root_include_path_makefile = $(bin)install_root_include_path.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#install_root_include_path :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'install_root_include_path'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = install_root_include_path/
#install_root_include_path::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------

#
#  We want to install all header files that follow the standard convention
#
#    ../<package>
#
#  into
#
#    ${CMTINSTALLAREA}/root-include-path/<package>
#

ifeq ($(INSTALLAREA),)
installarea = $(CMTINSTALLAREA)
else
ifeq ($(findstring `,$(INSTALLAREA)),`)
installarea = $(shell $(subst `,, $(INSTALLAREA)))
else
installarea = $(INSTALLAREA)
endif
endif

install_include_dir = $(installarea)/root-include-path

install_root_include_path :: install_root_include_pathinstall

install :: install_root_include_pathinstall

install_root_include_pathinstall :: $(install_include_dir)

$(install_include_dir) ::
	@if test "$(install_include_dir)" = ""; then \
	  echo "Cannot install header files, no installation directory specified"; \
	else \
	  if test -d ../${package}; then \
	    here=`(cd ../${package}; pwd)`; \
	    if test -L $(install_include_dir) ; then rm -f $(install_include_dir); fi; \
	    if test ! -d $(install_include_dir) ; then mkdir -p $(install_include_dir); fi; \
	    echo "Installing files from standard ../${package} to $(install_include_dir)"; \
            eval ln -sf $${here} $(install_include_dir); \
            echo $${here} >|$(install_include_dir)/$(package).cmtref; \
	  else \
	    echo "No standard include file area"; \
	  fi; \
	fi

##	    (cd ../${package}; eval ln -s $(install_include_filter) $(install_include_dir));


##install_root_include_pathclean :: install_root_include_pathuninstall

uninstall :: install_root_include_pathuninstall

install_root_include_pathuninstall ::
	@if test "$(install_include_dir)" = ""; then \
	  echo "Cannot uninstall header files, no installation directory specified"; \
	else \
	  if test -d $(install_include_dir) ; then \
	    echo "Uninstalling files from $(install_include_dir)"; \
	    eval rm -rf $(install_include_dir) ; \
	  fi \
	fi


#-- start of cleanup_header --------------

clean :: install_root_include_pathclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(install_root_include_path.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

install_root_include_pathclean ::
#-- end of cleanup_header ---------------
