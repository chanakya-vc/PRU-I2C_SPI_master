# PRU-I2C_SPI_master
Work in progress for firmware for SPI Master

These are the rough guide lines to compile the code and observe the waveforms.A more elaborate readme would be availbale in the master branch once the firmaware is complete.

To test the firmware code ,perform the following steps:

1) Before compiling the code,the PRU_CGT variable must point to the TI PRU compiler directory.As an example:
    export PRU_CGT=/home/vc/Desktop/beagle-gsoc/compile/ti-cgt-pru_2.1.0

2)Then simply run make to compile the code.

3)An obj file should appear in the gen folder .

4)Then scp the file to /lib/firmware/am335x-pru0-fw on the BeagleBone

5)Use congig-pin to configure P8_11 and P8_12 as output pins.Use config-pin -h for help with syntax.

6)Then rmmod pru_rproc and modprobe pru_rproc to load the firmware.Observe with a logic analyzer.
