#
#  $Id: regres.pl,v 1.1 2007-11-09 21:24:23 edwards Exp $
#
#  This is the portion of a script this is included recursively
#

#
# Each test has a name, input file name, output file name,
# and the good output that is tested against.
#
@regres_list = 
    (
     {
	 exec_path   => "$top_builddir/mainprogs/main" , 
	 execute     => "chroma" , 
	 input       => "$test_dir/chroma/gauge_transf/gauge_transf/gauge_transf.ini.xml" , 
	 output      => "gauge_transf.candidate.xml",
	 metric      => "$test_dir/chroma/gauge_transf/gauge_transf/gauge_transf.metric.xml" ,
	 controlfile => "$test_dir/chroma/gauge_transf/gauge_transf/gauge_transf.out.xml" ,
     }
     );