osld-lib
========

3GPP LTE eNodeB SDR Library with no external dependencies. The DSP modules are based on OSLD (https://github.com/flexnets/aloe). 

Currently, the PSS/SSS and modulation/demodulation modules have been ported to the new library. 

To run the PSS/SSS example:

'
mkdir build
cd build
cmake ../
make
examples/synch_test -i lte_signal.txt -N 0 
'

Searches for an N_id_2 PSS/SSS signal in the provided file. 
