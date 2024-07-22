# chicken-coop-door
An Arduino Nano powered chicken coop door

## How it works
The hatch automatically opens at dawn and closes at sunset when the light sensor reaches a certain threshold.\
Using the buttons it can also be operated manually.
The LEDs signals the state of the hatch, closing, opening, or if an error has occured.

## Images
[<img src="images\enclosure.jpg" alt="drawing" height="500"/>](images\enclosure.jpg)
[<img src="images\hatch.jpg" alt="drawing" height="500"/>](images\hatch.jpg)

## Technical details
### Schematic
![Schematic](images\schematic.png?raw=true)

### Librairies used
- [OneButton](https://github.com/mathertel/OneButton)
- [TimerOne](https://github.com/PaulStoffregen/TimerOne)
- [arduino-timer](https://github.com/contrem/arduino-timer)
- [singleLEDLibrary](https://github.com/SethSenpai/singleLEDLibrary)