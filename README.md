# gcadapter-evdev


A user-space linux driver for the Wii U GameCube Adapter.


# Usage


`./gcadapter_evdev`


gcadapter_evdev will create up to four evdev devices dynamically, allowing usage in applications that do not explicitly support the Wii U GameCube Adapter.


You will need to have set the permission of the adapter before gcadapter_evdev will detect it.


Refer to https://wiki.dolphin-emu.org/index.php?title=How_to_use_the_Official_GameCube_Controller_Adapter_for_Wii_U_in_Dolphin#Linux for more details.


# Building


`make`


You will need libusb-1.0-0-dev and libgcadapter to build gcadapter-evdev.


Refer to the readme in libgcadapter/ before building.
