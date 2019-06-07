Instruction for connecting to private network

 ssh -XY abdollah@cmslpc-sl6.fnal.gov

ssh -XY otsdaq@ftbf-daq-08

ssh -XY daq@192.168.133.220


#To connect to raspberry pi:

ssh -XY pi@192.168.133.221


ssh -t -Y -L 1234:localhost:11111 abdollah@cmslpc26.fnal.gov ssh -Y -D 11111 otsdaq@ftbf-daq-08.fnal.gov


#to add proxy

network

advanced

Proxies

SOCKS proxy

SOCKS Proxy Server

localhost    1234`

# edit time

sudo date -s '2017-02-05 15:30:00'

source ~cmstiming/cmstiming_setup.sh

Location of the VME root files:
/home/otsdaq/CMSTimingJarvis/2019_04_April_CMSTiming/VME/RecoData/RecoWithTracks


Location of the raw data:
daq@timingdaq02
2019_04_April_CMSTiming/TOFHIR/RawData/


Location of the TRACK data:
/home/daq/2019_04_April_CMSTiming/Tracks/




