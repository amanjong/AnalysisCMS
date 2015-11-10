0. Analysis documentation
====

These twiki pages contain the CMS synchronization status of the WW and WZ cross section analyses at 13 TeV. There we can find the datasets used in each analysis, trigger bits, lepton selections and analysis cuts.

    https://twiki.cern.ch/twiki/bin/view/CMS/WZ13TeV
    https://twiki.cern.ch/twiki/bin/view/CMS/WW2015Variables


1. Everything starts here
====

Log in to gridui.

    ssh -Y gridui.ifca.es -o ServerAliveInterval=240
    source /cvmfs/cms.cern.ch/cmsset_default.sh


2. Set a CMSSW release
====

    export SCRAM_ARCH=slc6_amd64_gcc491
    cmsrel CMSSW_7_5_3
    cd CMSSW_7_5_3/src
    cmsenv


3. Get the material
====

    git clone https://github.com/piedraj/AnalysisCMS
    cd AnalysisCMS

<!---
The base class should be recreated anytime the latino trees have been updated.
Read a MC latino tree that contains the `GEN_weight_SM` variable,

    root -l latino_DYJetsToLL_M-50.root
    latino->MakeClass("AnalysisBase")
-->


4. Compile and run
====

Always compile first.

    ./make

It is always good to test the code. You can try the following example, that reads a latino tree and produces the corresponding histograms.

    ./runAnalysis /gpfs/csic_projects/tier3data/LatinosSkims/RunII/MC_Spring15/21Oct/25ns/latino_WZTo3LNu.root 25ns

Submit jobs to the gridui batch system.

    ./submit-jobs.sh samples/files_25ns.txt 25ns

Show the status of the submitted jobs.

    qstat -u piedra
    qstat -j <job-ID.ja-task-ID>

<!---
Notice that input files can be accessed directly from eos when working from lxplus.

    ./runAnalysis root://eoscms.cern.ch//eos/cms/store/user/kbutanov/HWWwidthRun2/7September/25ns/latino_WZTo3LNu.root 25ns
-->


5. Draw
====

First merge the files produced in the previous step.

    cd test
    ./merge.sh

Then compile and draw your favorite distributions.

    ./make
    ./runPlotter


6. Create your website
====

Follow the instructions at [Permissions for your AFS folder](https://espace.cern.ch/webservices-help/websitemanagement/ConfiguringAFSSites/Pages/PermissionsforyourAFSfolder.aspx).

    ssh -Y lxplus.cern.ch
    mkdir www
    fs setacl www webserver:afs read
    afind www -t d -e "fs setacl -dir {} -acl webserver:afs read"

Go to the CERN Web Services and click on [Create a new website](https://webservices.web.cern.ch/webservices/Services/CreateNewSite/Default.aspx).
Choose the "AFS folder" site type.


7. Share on the web
====

Create a soft link to your `figures` folder.

    cd figures
    export FIGURES_DIR=`pwd`
    cd
    ln -s $FIGURES_DIR

Copy the distributions to lxplus.

    ssh -Y lxplus.cern.ch
    scp -r piedra@gridui.ifca.es:figures www/.

And they should appear here,

    https://amanjong.web.cern.ch/amanjong/figures/
    https://cprieels.web.cern.ch/cprieels/figures/
    https://piedra.web.cern.ch/piedra/figures/

