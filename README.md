# Robotic Finger

Physical device (a robotic finger) that interacts with a smartphone and a tablet along with the corresponding software layers: device driver, device library, creation of a small language to interact with the robotic finger, an interpretation program for Set up the robotic finger and a test program to evaluate the capabilities of the robotic finger. The main objective is to understand how to interact operating systems with physical devices.

### Compilation
Compile the executable files using the configure script `rf_conf` in the root directory:

    $ ./rf_conf

### Execution

To execute the code, run the executable file that has been generated in root directory, with the following command: 


    $ ./roboticFinger −c <config_file> −p <hardware_port> [−s <keyboard_size>]

