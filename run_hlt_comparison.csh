#!/bin/csh

root -b -q head_noBEAN.C 'run_hlt_comparison_using_class.C+' >&! hlt_comparison_log_pre1_pre2_prescaled_test.txt &
sleep 3 
echo "Running HLT event-by-event comparisons."
echo "Job ID:"
ps -o pid,args | grep "run_hlt_comparison_using_class.C+" | head -n 1
