MoteEmu
=======
A *Chip8* emulator written in C++. Serves mostly educational purose (me learning how emulation works).  

Dependencies:
-------------
* SDL2 v2.0.8 - **included**

Information:
-----------
The emulator is somewhat functioning, but is not a 100% working product. My plan is to get it as close as possible to 100% and start improving from there.  
The improvments I plan include, but are not limited to:
* Improve the API and make the VM stuff reusable for other emulators
* Move the VM stuff to a different repository and leave only the Chip8 code here
* Get rid of the SDL2 dependency and roll out different solution for input/output handling
* Add logging features
* Include unit testing
* Implement CMake/Premake
* Start using Travis CI

License:
--------
Copyright © 2018 Ivan Angelov  
Licensed under the [MIT License](LICENSE) 