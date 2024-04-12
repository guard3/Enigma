# Enigma
A simple and fast C++ implementation of an [Enigma machine](https://en.wikipedia.org/wiki/Enigma_machine), with support for 94 [ASCII](https://www.asciitable.com) characters and 2 rotors. Created just for fun! :stuck_out_tongue_winking_eye:\
![Usage example](https://github.com/guard3/Enigma/blob/master/images/enigma.png)
## Getting Started
To simply get **Enigma** up and running, download an executable from the **bin/** folder, depending on your platform. Though not necessary, it's recommended that you add the executable path to **$PATH**([Windows](https://helpdeskgeek.com/windows-10/add-windows-path-environment-variable/)/[Linux](https://opensource.com/article/17/6/set-path-linux)).
### Launch
To run **Enigma**, double click on the executable or launch from the commandline.\
On Windows:
```
cd path_to_enigma_executable
enigma
```
On macOS or Linux:
```
cd path_to_enigma_executable
./enigma
```
If you can't execute due to a `Permission denied` error, you're probably missing execution permission. To fix, simply type:
```
cd path_to_enigma_executable
chmod +x enigma
```
and try again.
### Cypher messages
After **Enigma** is up and running, simply type a normal or cyphered message and let **Enigma** (de)cypher it. Supported characters are [ASCII](https://www.asciitable.com) ' ' to '}'.\
![(De)cyphering example](https://github.com/guard3/Enigma/blob/master/images/screenshot1.png)
### Settings
The fun of the **Enigma** relies on its many different settings. Cyphering messages requires an **enigma.set** file to be present in the current working directory, which stores the settings for the 2 rotors, plugboard and reflector (more on how an enigma machine works on [Wikipedia](https://en.wikipedia.org/wiki/Enigma_machine)). If a settings file isn't present, Enigma automatically generates one on startup. When you want to exchange cyphered messages with others, make sure everyone is using the same **enigma.set** file, otherwise decyphering messages will produce rubbish.
### Special Commands
`~h ~H`: Show **h**elp menu\
`~r ~R`: **R**eload settings file\
`~n ~N`: Create a **n**ew settings file\
`~q ~Q`: **Q**uit

![Example](https://github.com/guard3/Enigma/blob/master/images/screenshot2.png)

## How to compile
This repository provides a [premake](https://premake.github.io) configuration for Visual Studio for Windows. To generate a solution, execute the appropriate .bat file. For example, to generate a solution for Visual Studio 2019, run:
```
vs2019.bat
```
The resulting solution will be created in a folder called **project/**.\
Alternatively, source can be compiled with any other compiler, such as [g++](https://gcc.gnu.org).
```
g++ -std=c++17 src/main.cpp src/Enigma.cpp -O3 -o enigma
```
## License
**Enigma** is licensed under the **GNU General Public License** - see [LICENSE](https://github.com/guard3/Enigma/blob/master/LICENSE) file for details.
