#!/bin/bash
#
# Filename: mostfragged.sh
#
# What this does:
# Runs an analysis on files by looking at thier sizes and number of extents (using filefrag tool), then it organizes the data to help find which files are most fragmented (extents per mb)
#
# Instructions:
# 1. Copy paste contents into a script called mostfragged.sh (or whatever name you wnat)
# 2. Read through the "Pick a Find" section below (and follow its steps if you want to skip reading certain folders, like snapshot folders)
# 3. Save the script
# 4. Run the script: mostfragged.sh <directory to analyze>
# 5. Wait for results that will get dumped to current directory
# 6. Read the MF-FINAL results for the best info, and the MF-0000# for other information
#
# Extra Info:
# /tmp files are generated and final files are generated into current directory (where mostfragged.sh is ran from - not where mostfragged.sh is located)
#
# How to read data:
# MB is size in megabytes (base 2, mebibytes MiB), e is filefrag extents, and e/MB is extents per megabytes (base 2, mebibytes MiB)
#
# Usage: $0 <directory to analyze> [optional directory to dump files to]
# Example 1: $0 /data
# Example 2: $0 /data results1
# Example 2 would dump data to `pwd`/results1
# Example 3: $0 /data /tmp/results
# Example 3 would dump data to /tmp/results
#
# Example output of a couple of the analysis files:
# cat MF-EXTRA--_data_Main_Downloads--average-no01.txt
# Average Number of Extents: 121.417
# Average Filesize in Bytes: 5.03206e+07
# Average Extents Per Byte: 2.41287e-06
# Average Bytes per Extent: 414444
# Average KiB per Extent: 404.731
# Average MiB per Extent: 0.395245
# Average GiB per Extent: 0.000385981
# Total Number of Files: 187
# Total Size of files: 9.40996e+09 Bytes, 9.18941e+06 KiB, 8974.04 MiB, 8.76371 GiB, 0.00855831 TiB
# Total Number of Extents: 22705
#
# Also added percent fragmentation based on the number of files (number of fragmentated files / total number of files). 
# Using same simple formula calculated percent fragmentation based on the number of extents (number of fragmented extents / total number of extents)
# Using same formula for percent fragmentation based on the number of bytes. This will let us know how many bytes (storage bytes) are acctually fragmented.
# the 3 numbers of fragmentation percentage seem reasonable
# so I also made an average fragmentation percentage which is the average of the 3 numbers.
# Here is the output in the STATS files (both of the STATS files have the same #s for fragmentation percentage)
# Example output in STATS file (also this output is shown at then end of the mostfragged run on the screen)
# * % File Fragmentation: 42.7665
# * % Extent Fragmentation: 49.3532
# * % Size Fragmentation: 49.8358
# * % Average Fragmentation: 47.3185
#
# Release Notes:
# UPDATE 8-21-2014: added support to skip snapshot folders for READYNAS OS6 scanning (just need to comment out default find, and uncomment the correct find). Read section below called pick a find.
# UPDATE 8-20-2014: fixed files with spaces (see #### comments below, ## for style change). dumping all results to same folder & better useability (check for argument, etc...)
# UPDATE 5-27-2014: filenames output are better now in script (make more sense)
# UPDATE 10-9-2014: PART1: changed normal find to verbose (With tee and nl), kept "SILENT" find but in comments below each find, also added "error to trash" on first readlink to minimize error output
# UPDATE 10-9-2014: PART2: added EXTRA info output (alot more files), added ability to dump to another directory (made showusage function), better output on screen (increases time to completion with all this)
# UPDATE 10-10-2014: clearing up outputs
# UPDATE 10-13-2014: minor adjustments in outputs - added FOOTER to stats files
# UPDATE 5-14-2015: added new STATS files with % fragmentation calculations
#
####################
# HELPER FUNCTIONS #
####################
#
function showusage {
    echo "Usage: $0 <directory to analyze> [optional directory to dump files to]";
    echo "Example 1: $0 /data";
    echo "Example 2: $0 /data results1";
    echo "Example 2 would dump data to `pwd`/results1"
    echo "Example 3: $0 /data /tmp/results";
    echo "Example 3 would dump data to /tmp/results"
}
#
###################
# THE MAIN SCRIPT #
###################
#
# --- set location of scan (To absolute path of given folder) ---
LOCATION=`readlink -f "$1" 2> /dev/null`;
DDIR="$2";
if [ -z ${LOCATION} ]; then
    echo "ERROR: Missing argument";
    showusage;
    exit 1;
fi;
if [ ! -d ${LOCATION} ]; then
    echo "ERROR: directory doesn't exist"
    showusage;
    exit 1;
fi;
if [ -z $DDIR ]; then
    DPREFIX=`pwd`;
    echo "Dumping output files here: $DPREFIX";
else
    mkdir -p $DDIR 2> /dev/null;
    DPREFIX=`readlink -f $DDIR`;
    echo "Dumping Output files to: $DPREFIX";
fi;
D81S=`date +s%s-d%D-t%T | tr /: -`
D81=`date +d%Y-%m-%d-t%T | tr /: -`
D81Ss=$(date +"%s")

# --- set file prefix (to a string that includes the full file path that can make do as a filename) ---
PATH2STRING=$(echo ${LOCATION} | sed -e 's/[^A-Za-z0-9._-]/_/g')
pref1x="MF-00001--${D81}--${PATH2STRING}"
pref11x="MF-00002--${D81}--${PATH2STRING}"
pref2x="MF-00003--${D81}--${PATH2STRING}"
pref3x="MF-00004--${D81}--${PATH2STRING}"
pref4x="MF-00005--${D81}--${PATH2STRING}"
pref5x="MF-FINAL--${D81}--${PATH2STRING}"
pref6x="MF-FINAL--${D81}--${PATH2STRING}"
# filenames
file1="${DPREFIX}/${pref1x}--extents-and-sizes.txt"    # old filename: /tmp/extents-${D81}.txt
file11="${DPREFIX}/${pref11x}--extents-and-sizes-organized.txt" # this is like step 1.5 (to fix files with spaces)
file2="${DPREFIX}/${pref2x}--SORTED-by-extents.txt"    # old filename: /tmp/sortbyextents-${D81}.txt
file3="${DPREFIX}/${pref3x}--bytes-per-extent.txt"    # old filename: /tmp/bytes-per-extent-${D81}.txt
file4="${DPREFIX}/${pref4x}--extents-per-mb.txt"    # old filename: /tmp/extents-per-mb-${D81}.txt
file5="${DPREFIX}/${pref5x}--extents-per-mb-SORTED.txt"    # old filename: `pwd`/mostfragged-extents-per-mb-${D81}.txt
file6="${DPREFIX}/${pref6x}--bytes-per-extent-SORTED.txt"    # old filename: `pwd`/mostfragged-bytes-per-extent-${D81}.txt
# extra files
new0="${DPREFIX}/MF-EXTRA--${PATH2STRING}--0-extent-files.txt"
new1="${DPREFIX}/MF-EXTRA--${PATH2STRING}--1-extent-files.txt"
new2="${DPREFIX}/MF-EXTRA--${PATH2STRING}--all-fragmented-files-more-than-1-extent.txt"
new3="${DPREFIX}/MF-EXTRA--${PATH2STRING}--100-plus-extent-files.txt"
new3s="${DPREFIX}/MF-EXTRA--${PATH2STRING}--100-plus-SORTED-extent-files.txt"
new4="${DPREFIX}/MF-EXTRA--${PATH2STRING}--1000-plus-extent-files.txt"
new4s="${DPREFIX}/MF-EXTRA--${PATH2STRING}--1000-plus-SORTED-extent-files.txt"
new5="${DPREFIX}/MF-EXTRA--${PATH2STRING}--STATS-of-all-fragmented-files-more-than-1-extent.txt"
new5a="${DPREFIX}/MF-EXTRA--${PATH2STRING}--STATS-of-all-files.txt"

# --------- work-begins ---
echo "-----------START INFO:---------------------"
echo "START INFO: Starting fragmentation analysis on: ${LOCATION}"
echo "START INFO: This might take a while (seconds to hours - depending on amount of data & fragmentation)"
echo "START INFO: Dumping data to current directory: `pwd`"
echo "-----------START:--------------------------"
# ttt is the total number of steps (its a constant)
ttt=17
echo "* [`date +%D-%T`][`date +%s`] STARTING 1/${ttt}: Analyzing $LOCATION for sizes and number of extents"

################################################
################################################
# ==============Pick a find:===================#
# Scanning with find will go through the entire
# directory tree, it will even scan the millions
# of copies that could exist due to snapshots
# The default find will scan everything.
# If you want to skip snapshot folders then pick
# one of the below finds (other then the default
# one). If on a Readynas I recommend using the
# ABSOLUTE FIND instead of the RELATIVE FIND.
# The RELATIVE FIND will skip all folders with
# the words snapshot/.snapshot/.snapshots (Even
# if the are customer made). The ABSOLUTE FIND
# will make sure that they are the NETGEAR made
# snapshot folder by looking into how deep into
# the filesystem the folders are as well. If
# Not on a READYNAS and your snapshots dont
# adhere to the folder notation: snapshot
# or .snapshot or .snapshots then you can make
# your own CUSTOM FIND
################################################
################################################
# **************** Default Find ************** # - comment this out if not on a readynas (or else this script will go on for forever through all of the snapshots)
find $LOCATION -type f -printf "%s - " -exec filefrag {} \; > ${file1} | tee "${file1}" | nl
################################################
################################################
# **************** RELATIVE FIND to skip snapshots for Readynas OS6 ************** #
# - since this is relative this will skip all directories names snapshot/.snapshots/.snapshot (even if its customers own dirs)
# find $LOCATION -type f -not -iwholename "*/.snapshot/*" -not -iwholename "*/.snapshots/*" -not -iwholename "*/snapshot/*" -not -iwholename "*/._snap-*/*" -printf "%s - " -exec filefrag {} \; > ${file1} | tee "${file1}" | nl
################################################
################################################
# **************** ABSOLUTE FIND to skip snapshots for Readynas OS6 ************** # <----------------------------- pick this one for readynas os6 best results
# - since this is absolute this will skip only netgear named snapshot/.snapshots/.snapshot folders
# find $LOCATION -type f -not -iwholename "/*/._share/*/.snapshot/*" -not -iwholename "/*/*/.snapshots/*/snapshot/*" -not -iwholename "/*/*/snapshot/*" -not -iwholename "/*/._snap-*/*" -printf "%s - " -exec filefrag {} \; | tee "${file1}" | nl
################################################
################################################
# **************** CUSTOM FIND for Snapshots ************** #
# - change the below snaps to whatever you call your snapshot folder (so if you call them snapshotties, then put in "*/snapshotties/*"
# find $LOCATION -type f -not -iwholename "*/snaps/*" -printf "%s - " -exec filefrag {} \; > ${file1} | tee "${file1}" | nl
################################################
################################################
#### added below to fix files with spaces

echo "* [`date +%D-%T`][`date +%s`] 2/${ttt}: Organizing each line for readability"
cat ${file1} | sed -n 's/^\([0-9]*\) - \(.*\): \([0-9]*\) extent[s]* found$/\1 bytes \3 extents - \2/p' > "${file11}"

echo "* [`date +%D-%T`][`date +%s`] 3/${ttt}: Finding none 0 and none 1 extent files (for main analysis)"
cat ${file11} | grep -v " 1 extents " | grep -v " 0 extents " > "${new2}"

echo "* [`date +%D-%T`][`date +%s`] 4/${ttt}: Putting all 0 extent files to a file (for record keeping, and curiousity)"
cat ${file11} | grep " 0 extents " > "${new0}"

echo "* [`date +%D-%T`][`date +%s`] 5/${ttt}: Putting all 1 extent files to a file (for record keeping, and curiousity)"
cat ${file11} | grep " 1 extents " > "${new1}"

############ ----------- old awk (squished output for stats) - start ------------ #################

###### AWKSTRING='BEGIN{i=0;x=0;s=0;}{i=i+1;x=x+$3;s=s+$1;}END{isum=i;xsum=x;xav=xsum/isum;ssum=s;sav=ssum/isum;xps=xsum/ssum;spx=ssum/xsum; print "Stats of fragmentation (Everything is Average & Totaled per file)"; print "Average Number of Extents: " xav " (less is best)"; print "Average Filesize: " sav " Bytes, " sav/1024 " KiB, " sav/1024/1024 " MiB, " sav/1024/1024/1024 " GiB, " sav/1024/1024/1024/1024 " TiB"; print "Average Extents Per Byte: " xps " (this value does not mean much, bigger is better)"; print "Average Size of an Extent: " spx " Bytes, " spx/1024 " KiB, " spx/1024/1024 " MiB, " spx/1024/1024/1024 " GiB, " spx/1024/1024/1024/1024 " TiB (bigger is better)"; print "Total Number of Files: " isum; print "Total Size of files: " ssum " Bytes, " ssum/1024 " KiB, " ssum/1024/1024 " MiB, " ssum/1024/1024/1024 " GiB, " ssum/1024/1024/1024/1024 " TiB"; print "Total Number of Extents: " xsum " (less is best)"}'

###### echo "* [`date +%D-%T`][`date +%s`] 6/${ttt}: Calculating Average Extents in files more than 1 extent"
cat ${new2} | awk "$AWKSTRING"  > "${new5}"

###### echo "* [`date +%D-%T`][`date +%s`] 7/${ttt}: Calculating Average Extents from all files (0 and 1 extents includes)"
cat ${file11} | awk "$AWKSTRING" > "${new5a}"

############ ----------- old awk (squished output for stats) - end ------------ #################

############ ----------- testing zone start ------------ #################

# Uncomment above ###### and comment below
# I like below output better :-) update 2014-10-13

TESTAWKSTRING='BEGIN{i=0;x=0;s=0;}{i=i+1;x=x+$3;s=s+$1;}END{isum=i;xsum=x;xav=xsum/isum;ssum=s;sav=ssum/isum;xps=xsum/ssum;spx=ssum/xsum; print "Stats of fragmentation (Everything is Average & Totaled per file)"; print "Average Number of Extents: " xav " (less is best)"; print "Average Filesize: " sav " Bytes"; print "Average Filesize: " sav/1024 " KiB"; print "Average Filesize: " sav/1024/1024 " MiB"; print "Average Filesize: " sav/1024/1024/1024 " GiB"; print "Average Filesize: " sav/1024/1024/1024/1024 " TiB"; print "Average Extents Per Byte: " xps " (this value does not mean much, bigger is better)"; print "Average Size of an Extent: " spx " Bytes (bigger is better)"; print "Average Size of an Extent: " spx/1024 " KiB (bigger is better)"; print "Average Size of an Extent: " spx/1024/1024 " MiB (bigger is better)"; print "Average Size of an Extent: " spx/1024/1024/1024 " GiB (bigger is better)"; print "Average Size of an Extent: " spx/1024/1024/1024/1024 " TiB (bigger is better)"; print "Total Number of Files: " isum; print "Total Size of files: " ssum " Bytes"; print "Total Size of files: " ssum/1024 " KiB"; print "Total Size of files: " ssum/1024/1024 " MiB"; print "Total Size of files: " ssum/1024/1024/1024 " GiB"; print "Total Size of files: " ssum/1024/1024/1024/1024 " TiB"; print "Total Number of Extents: " xsum " (less is best)"}'

echo "* [`date +%D-%T`][`date +%s`] 6/${ttt}: Calculating Average Extents in files more than 1 extent"
cat ${new2} | awk "$TESTAWKSTRING"  > "${new5}"

echo "* [`date +%D-%T`][`date +%s`] 7/${ttt}: Calculating Average Extents from all files (0 and 1 extents includes)"
cat ${file11} | awk "$TESTAWKSTRING" > "${new5a}"

############ ----------- testing zone end ------------ #################

FOOTER="FOOTER `date +%s` sec | `date` | $0 $1 $2 | ${LOCATION} | Saving to | ${DPREFIX}"
echo $FOOTER >> "${new5}"
echo $FOOTER >> "${new5a}"

echo "* [`date +%D-%T`][`date +%s`] 8/${ttt}: Finding all files with more than 100 extents"
cat ${new2} | awk '{EXTENTS=$3;THRESHOLD=100;if(EXTENTS>=THRESHOLD){print $0;}}' > "${new3}"

echo "* [`date +%D-%T`][`date +%s`] 9/${ttt}: Finding all files with more than 1000 extents"
cat ${new2} | awk '{EXTENTS=$3;THRESHOLD=1000;if(EXTENTS>=THRESHOLD){print $0;}}' > "${new4}"

echo "* [`date +%D-%T`][`date +%s`] 10/${ttt}: Sorting main output to make ${file2}"
#### changed below from file1 to file11 and -nk4 to -nk3 to fix spaces issue
# COMMENTED WITH "new" CHANGES: to not count 0 and 1 extents file in main calculation
# COMMENTED WITH "new" CHANGES: cat ${file11} | sort -nk3 > "${file2}"
cat ${new2} | sort -nk3 > "${file2}"
# COMMENTED WITH "new" CHANGES: to not do sorting again just using file2 output which is already sorted

echo "* [`date +%D-%T`][`date +%s`] 11/${ttt}: Sorting 100 extent output (not actually running sort alg again for CPU and time saving)"
cat ${file2} | awk '{EXTENTS=$3;THRESHOLD=100;if(EXTENTS>=THRESHOLD){print $0;}}' > "${new3s}"

echo "* [`date +%D-%T`][`date +%s`] 12/${ttt}: Sorting 1000 extent output (not actually running sort alg again for CPU and time saving)"
cat ${file2} | awk '{EXTENTS=$3;THRESHOLD=1000;if(EXTENTS>=THRESHOLD){print $0;}}' > "${new4s}"

echo "* [`date +%D-%T`][`date +%s`] 13/${ttt}: Starting Calculating BYTES PER EXTENT"
#### for files with space: $1 stays, $4 becomes $3, $3 to $6 (need to make it $6+)
#### ---- interesting awk trick (to print nth column to end) start comment ---- ####
# How printed file name (because of spaces it can span more than just the 6th column, but it could be 6th column +): Note that $1=$2=$3=$4=$5="", so that $0 is the 6th column plus the rest, but that leaves left trailing spaces which we remove with gsub(match this,change to this,on this variable)
#### ---- interesting awk trick (to print nth column to end) end comment ---- ####
cat ${file2} | awk '{SIZE=$1; EXTENTS=$3; $1=$2=$3=$4=$5=""; gsub(/^[ \t]+/,"",$0); FILENAME=$0; print SIZE " Bytes, " EXTENTS " extents, " SIZE/EXTENTS " B/e - " FILENAME}' > "${file3}"

echo "* [`date +%D-%T`][`date +%s`] 14/${ttt}: Starting Calculating EXTENTS PER MB"
#### for files with space: $1 stays, $4 becomes $3, $3 to $6 (need to make it $6+)
cat ${file2} | awk '{SIZE=$1; EXTENTS=$3; $1=$2=$3=$4=$5=""; gsub(/^[ \t]+/,"",$0); FILENAME=$0; print SIZE/1024/1024 " MiB, " EXTENTS " extents, " (EXTENTS/(SIZE/1024/1024)) " Extents/MiB - " FILENAME}' > "${file4}"

echo "* [`date +%D-%T`][`date +%s`] 15/${ttt}: Sorting EXTENTS PER MB output"
# THE 5TH COLUMN IS E/M WHICH IS THE EXTENTS PER MEGABYTE, LETS SORT BY IT
cat ${file4} | sort -nk5 > "${file5}"

echo "* [`date +%D-%T`][`date +%s`] 16/${ttt}: Sorting BYTES PER EXTENT output"
# THE 5TH COLUMN BELOW IS B/E LETS SORT BY IT
cat ${file3} | sort -nk5 > "${file6}"

echo "* [`date +%D-%T`][`date +%s`] 17/${ttt}: DONE"

# ----------------------- calculating new fragmentation numbers - start -----------------#
# number of extents/files/bytes all files or fragmented files (have _F for frag)
# fragmented files have more than 1 extent. what new and new5a contain
# new5 is frag files, new5a is all files
# Total Number of Files: 246409
# Total Size of files: 9.03173e+12 Bytes
# Total Number of Extents: 353389 (less is best)
N_FILES=`cat "${new5a}" | grep 'Total Number of Files' | cut -f2 -d: | awk '{print $1;}'`
N_FILES_F=`cat "${new5}" | grep 'Total Number of Files' | cut -f2 -d: | awk '{print $1;}'`
N_EXT=`cat "${new5a}" | grep 'Total Number of Extents' | cut -f2 -d: | awk '{print $1;}'`
N_EXT_F=`cat "${new5}" | grep 'Total Number of Extents' | cut -f2 -d: | awk '{print $1;}'`
N_BYTE=`cat "${new5a}" | grep 'Total Size of files' | grep 'Bytes' | cut -f2 -d: | awk '{print $1;}'`
N_BYTE_F=`cat "${new5}" | grep 'Total Size of files' | grep 'Bytes' | cut -f2 -d: | awk '{print $1;}'`
echo "$N_FILES $N_FILES_F $N_EXT $N_EXT_F $N_BYTE $N_BYTE_F"
# calculation
N_FILE_FRAG_PERCENT=`echo "${N_FILES_F} ${N_FILES}" | awk '{TOTAL=($1+$2);print (($1/TOTAL)*100);}'`
N_EXT_FRAG_PERCENT=`echo "${N_EXT_F} ${N_EXT}" | awk '{TOTAL=($1+$2);print (($1/TOTAL)*100);}'`
N_BYTE_FRAG_PERCENT=`echo "${N_BYTE_F} ${N_BYTE}" | awk '{TOTAL=($1+$2);print (($1/TOTAL)*100);}'`
N_AVERAGE_PERCENT=`echo "${N_EXT_FRAG_PERCENT} ${N_FILE_FRAG_PERCENT} ${N_BYTE_FRAG_PERCENT}" | awk '{print ($1+$2+$3)/3;}'`
# add same info to both files (at same time with tee -a and >>)
echo "-----------PERCENT FRAGMENTATION:--------------"
echo "* % File Fragmentation: ${N_FILE_FRAG_PERCENT}"
echo "* % File Fragmentation: ${N_FILE_FRAG_PERCENT}"  | tee -a "${new5a}" >> "${new5}"
echo "* % Extent Fragmentation: ${N_EXT_FRAG_PERCENT}"
echo "* % Extent Fragmentation: ${N_EXT_FRAG_PERCENT}" | tee -a "${new5a}" >> "${new5}"
echo "* % Size Fragmentation: ${N_BYTE_FRAG_PERCENT}"
echo "* % Size Fragmentation: ${N_BYTE_FRAG_PERCENT}"  | tee -a "${new5a}" >> "${new5}"
echo "* % Average Fragmentation: ${N_AVERAGE_PERCENT}" 
echo "* % Average Fragmentation: ${N_AVERAGE_PERCENT}" | tee -a "${new5a}" >> "${new5}"
# ----------------------- calculating new fragmentation numbers - end -----------------#

echo "-----------ANALYSIS COMPLETE:--------------"
D81F=`date +s%s-d%D-t%T | tr /: -`
D81Fs=$(date +"%s")
Tdiff=$(($D81Fs-$D81Ss))
echo "* DURATION: $(($Tdiff / 60)) minutes and $(($Tdiff % 60)) seconds = $Tdiff seconds total"
echo "* START TIME: ${D81S}"
echo "* END TIME: ${D81F}"
# ----------------------- log these times to stats - start------------------------#
# --- to 5 --- #
echo "FOOTER | RUN DURATION | $(($Tdiff / 60)) | minutes | $(($Tdiff % 60)) seconds | $Tdiff | seconds total" >> "${new5}"
echo "FOOTER | RUN START TIME | ${D81S}" >> "${new5}"
echo "FOOTER | RUN END TIME | ${D81F}" >> "${new5}"
# --- to 5a --- #
echo "FOOTER | RUN DURATION | $(($Tdiff / 60)) | minutes | $(($Tdiff % 60)) seconds | $Tdiff | seconds total" >> "${new5a}"
echo "FOOTER | RUN START TIME | ${D81S}" >> "${new5a}"
echo "FOOTER | RUN END TIME | ${D81F}" >> "${new5a}"
# ----------------------- log these times to stats - end ------------------------#
echo "-----------NONE IMPORTANT RESULTS:---------"
echo "* The following files are optional and inbetween information (you can choose to delete them with these command):"
echo "rm ${file1}"
echo "rm ${file11}"
echo "rm ${file2}"
echo "rm ${file3}"
echo "rm ${file4}"
echo "-----------EXTRA CURIOUS RESULTS:----------"
echo "* Interersting extra curious results:"
echo "- ${new0}"
echo "- ${new1}"
echo "- ${new2}"
echo "- ${new3}"
echo "- ${new3s}"
echo "- ${new4}"
echo "- ${new4s}"
echo "- ${new5}"
echo "- ${new5a}"
echo "-----------IMPORTANT RESULTS:--------------"
echo "* The MF-FINAL files should be used for analysis:"
echo "(1) ${file5}"
echo "(2) ${file6}"
echo "-----------THE END-------------------------"
