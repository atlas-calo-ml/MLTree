#-- start of make_header -----------------

#====================================
#  Document MLTreeConf
#
#   Generated Mon Dec 12 19:39:22 2016  by jolsson
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_MLTreeConf_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_MLTreeConf_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_MLTreeConf

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeConf = $(MLTree_tag)_MLTreeConf.make
cmt_local_tagfile_MLTreeConf = $(bin)$(MLTree_tag)_MLTreeConf.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeConf = $(MLTree_tag).make
cmt_local_tagfile_MLTreeConf = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_MLTreeConf)
#-include $(cmt_local_tagfile_MLTreeConf)

ifdef cmt_MLTreeConf_has_target_tag

cmt_final_setup_MLTreeConf = $(bin)setup_MLTreeConf.make
cmt_dependencies_in_MLTreeConf = $(bin)dependencies_MLTreeConf.in
#cmt_final_setup_MLTreeConf = $(bin)MLTree_MLTreeConfsetup.make
cmt_local_MLTreeConf_makefile = $(bin)MLTreeConf.make

else

cmt_final_setup_MLTreeConf = $(bin)setup.make
cmt_dependencies_in_MLTreeConf = $(bin)dependencies.in
#cmt_final_setup_MLTreeConf = $(bin)MLTreesetup.make
cmt_local_MLTreeConf_makefile = $(bin)MLTreeConf.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#MLTreeConf :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'MLTreeConf'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = MLTreeConf/
#MLTreeConf::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/genconfig_header
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

# Use genconf.exe to create configurables python modules, then have the
# normal python install procedure take over.

.PHONY: MLTreeConf MLTreeConfclean

confpy  := MLTreeConf.py
conflib := $(bin)$(library_prefix)MLTree.$(shlibsuffix)
confdb  := MLTree.confdb
instdir := $(CMTINSTALLAREA)$(shared_install_subdir)/python/$(package)
product := $(instdir)/$(confpy)
initpy  := $(instdir)/__init__.py

ifdef GENCONF_ECHO
genconf_silent =
else
genconf_silent = $(silent)
endif

MLTreeConf :: MLTreeConfinstall

install :: MLTreeConfinstall

MLTreeConfinstall : /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree/$(confpy)
	@echo "Installing /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree in /share/home/jolsson/workarea/MLTreeAthenaAnalysis/InstallArea/python" ; \
	 $(install_command) --exclude="*.py?" --exclude="__init__.py" --exclude="*.confdb" /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree /share/home/jolsson/workarea/MLTreeAthenaAnalysis/InstallArea/python ; \

/share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree/$(confpy) : $(conflib) /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree
	$(genconf_silent) $(genconfig_cmd)   -o /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree -p $(package) \
	  --configurable-module=GaudiKernel.Proxy \
	  --configurable-default-name=Configurable.DefaultName \
	  --configurable-algorithm=ConfigurableAlgorithm \
	  --configurable-algtool=ConfigurableAlgTool \
	  --configurable-auditor=ConfigurableAuditor \
          --configurable-service=ConfigurableService \
	  -i ../$(tag)/$(library_prefix)MLTree.$(shlibsuffix)

/share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree:
	@ if [ ! -d /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree ] ; then mkdir -p /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree ; fi ;

MLTreeConfclean :: MLTreeConfuninstall
	$(cleanup_silent) $(remove_command) /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree/$(confpy) /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree/$(confdb)

uninstall :: MLTreeConfuninstall

MLTreeConfuninstall ::
	@$(uninstall_command) /share/home/jolsson/workarea/MLTreeAthenaAnalysis/InstallArea/python
libMLTree_so_dependencies = ../x86_64-slc6-gcc49-opt/libMLTree.so
#-- start of cleanup_header --------------

clean :: MLTreeConfclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(MLTreeConf.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

MLTreeConfclean ::
#-- end of cleanup_header ---------------
