Instruction for connecting to private network

 ssh -XY abdollah@cmslpc-sl6.fnal.gov

ssh -XY otsdaq@ftbf-daq-08

ssh -XY daq@192.168.133.220



ssh -L  1234:localhost:11111 abdollah@cmslpc26.fnal.gov ssh -Y -D 11111 otsdaq@ftbf-daq-08.fnal.gov
to add proxy
network
advanced
Proxies
SOCKS proxy
SOCKS Proxy Server
localhost    1234`
