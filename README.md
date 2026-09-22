# UHBadge
![Hardware License](https://img.shields.io/github/license/gianluca-rainis/UHBadge)
![Software License](https://img.shields.io/badge/license-MIT_License-grey?color=%23939393)
![GitHub Release](https://img.shields.io/github/v/release/gianluca-rainis/UHBadge?logo=github&color=orange)
![GitHub Repo stars](https://img.shields.io/github/stars/gianluca-rainis/UHBadge)
![GitHub watchers](https://img.shields.io/github/watchers/gianluca-rainis/UHBadge)
[![View PCB](images/pcbViewBadge.svg)](https://kicanvas.org/?repo=https://github.com/gianluca-rainis/UHBadge)

![UHBadge Logo](/images/UHBadge.png)

Universal Hacker Badge - The ultimate badge for hackers.

![UHBadge PCB Image](/images/UHBadgePCB.png)

## Author
The author of the whole project is: 
**Gianluca Rainis** - [gianluca-rainis](https://github.com/gianluca-rainis) on GitHub.

## License
### Hardware design files (schematics, PCB, BOM):
Licensed under [CERN-OHL-S v2.0](LICENSE-CERN-OHL-S.md)

### Software, firmware and documentation:
Licensed under the [MIT License](LICENSE-MIT.md)

## AI Disclaimer
**No AI was used in the conception, design, or engineering of the hardware.**
Every circuit and every design decision are the original work of the author.

*AI tools* were used as **writing assistants** for parts of the firmware, to help write technical drivers and low-level functions.
**All content has been reviewed, verified, and edited by the author.**

## Development Tools
- **PCB Design:** KiCad
- **Documentation and code:** Visual Studio Code

## PCB Manufacturing
The Gerber files for PCB manufacturing are available in the [`gerber/`](gerber/) directory.

## Project Structure
```
UHBadge/
├── CustomFootprints.pretty/  # Custom footprints
├── firmware/                 # The RP2350 firmware
├── gerber/                   # PCB manufacturing files (Gerber format)
├── images/                   # Documentation images
├── .gitattributes            # Gitattributes file
├── .gitignore                # Gitignore file
├── gerber.zip                # Zip folder with all the gerber files
├── UHBadge.kicad_pcb         # PCB layout file
├── UHBadge.kicad_pro         # KiCad project file
├── UHBadge.kicad_sch         # Schematic file
├── UHBadge.step              # 3D model
├── UHBadge.csv                   # Bill Of Materials
├── LICENSE-CERN-OHL-S.md     # CERN-OHL-S License
├── LICENSE-MIT.md            # MIT License
└── README.md                 # This file
```

## Technical Details
- **Size**: 70mm x 117.6mm
- **Layers**: 2
- **Supply Voltage**: 3v3

## Build the firmware
From the directory of the repo:
```bash
cd firmware/build

cmake ..
make -j$(sysctl -n hw.ncpu)
```

You'll find the `UHBadge.uf2` file in the `firmware/build` directory.

## Hardware
### Schematic
![UHBadge Schematics](/images/UHBadgeSchematics.png)

### PCB
![UHBadge PCB Image](/images/UHBadgePCB.png)

### 3D Render
![UHBadge Render Front](/images/UHBadge3DFront.png)
![UHBadge Render Back](/images/UHBadge3DBack.png)
