# Copyright (c) 2018 The University of Notre Dame
# Copyright (c) 2018 The Regents of the University of California
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or other
# materials provided with the distribution.

# 3. Neither the name of the copyright holder nor the names of its contributors may
# be used to endorse or promote products derived from this software without specific
# prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

# Contributors:
# Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

set -x

${AGAVE_JOB_CALLBACK_RUNNING}

export STARTINGDIR=`pwd`

export PYTHONDIR=$STARTINGDIR/python/CFDexec
export PYTHONPATH=$PYTHONPATH:$STARTINGDIR/python

unzip templates.zip
export TEMPLATER_FOLDER=$STARTINGDIR/templates

PARENTDIR="${directory}"
TASKSTAGE="${stage}"
export EXTRAFILE="${file_input}"

function dump_logs {
    echo "Starting cwe cleanup"

    cd $STARTINGDIR 
    mkdir logs

    mv .agave.log logs
    mv *.err logs/cwe-parallel.err
    mv *.out logs/cwe-parallel.out
    mv *.log logs

    if [ -f $EXTRAFILE ]
    then
        rm -rf $EXTRAFILE
    fi

    rm -rf $TEMPLATER_FOLDER
    echo "$1" > .exit
    exit $1
}

#Debug: output all inputs
echo "dest is $PARENTDIR"
echo "stage is $TASKSTAGE"

cp -f ${directory}/.caseParams .varStore

export DATA_FOLDER=`python $PYTHONDIR/Templater.py getdatafolder`
STOP=$?
if [ $STOP -ne 0 ]
then
    echo "Unable to retrive data folder path"
    dump_logs $STOP
fi

SCRIPT_FILE=`python $PYTHONDIR/Templater.py getscript $TASKSTAGE`

STOP=$?
if [ $STOP -ne 0 ]
then
    echo "Unable to retrive script name."
    dump_logs $STOP
fi

LAST_STAGE=`python $PYTHONDIR/Templater.py getlaststage $TASKSTAGE`

STOP=$?
if [ $STOP -ne 0 ]
then
    echo "Unable to retrive last stage name."
    dump_logs $STOP
fi

echo $LAST_STAGE

if [ $LAST_STAGE != "None" ]
then
    mv ${directory}/$LAST_STAGE/* .
    mv -f ${directory}/.caseParams .varStore
    rm -rf logs
    rm -f .exit
fi

rm -rf ${directory}
if [ -d "stats" ]
then
    python $PYTHONDIR/Templater.py paramset -s stats
else
    python $PYTHONDIR/Templater.py paramset
fi

chmod 700 $SCRIPT_FILE

$SCRIPT_FILE

STOP=$?
if [ $STOP -ne 0 ]
then
    echo "Error in job process"
    dump_logs $STOP
fi

if [ -d "stats" ]
then
    python $PYTHONDIR/Templater.py paramset -s stats
fi

STOP=$?
if [ $STOP -ne 0 ]
then
    echo "Error in job process"
    dump_logs $STOP
fi

echo "CWE task process complete."
dump_logs 0

