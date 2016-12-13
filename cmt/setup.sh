# echo "setup MLTree MLTree-00-00-00 in /share/home/jolsson/workarea/MLTreeAthenaAnalysis"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.7/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtMLTreetempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtMLTreetempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=MLTree -version=MLTree-00-00-00 -path=/share/home/jolsson/workarea/MLTreeAthenaAnalysis  -no_cleanup $* >${cmtMLTreetempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=MLTree -version=MLTree-00-00-00 -path=/share/home/jolsson/workarea/MLTreeAthenaAnalysis  -no_cleanup $* >${cmtMLTreetempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtMLTreetempfile}
  unset cmtMLTreetempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtMLTreetempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtMLTreetempfile}
unset cmtMLTreetempfile
return $cmtsetupstatus

