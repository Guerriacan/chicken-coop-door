# chicken-coop-door
An automated chicken coop door powered by Arduino Nano

## How it works
The main goal of this project is to automatically open the hatch in the morning and to close it at night.

When turning on, the hatch will open fully to it's origin set by the endswitch and will set the `motorTurnCount` variable to `0`.\
The closing heigh is set by the `motorTurnCountTarget` variable, which is incremented when closing.\

When in the `AUTO` mode, the ambient light sensor will measures the light brightness every 3 minutes and makes an average if the last five measures. The larger the value, the dimmer the light.

The `lightSensorAverage` is then compared to the thresholds `lightOpenLevel` and `lightCloseLevel` :
- If `lightSensorAverage` is smaller than `lightOpenLevel`, the hatch opens.
- If `lightSensorAverage` is greater than `lightCloseLevel`, the hatch closes.

Using the buttons it can also be operated manually :
- Pressing any button will switch the hatch to `MANUAL` mode.
- Pressing either `UP` or `DOWN` button will open or close the hatch.
- Pressing the `MIDDLE` button will stop the hatch.
- Long pressing the `MIDDLE` button will switch the hatch to `AUTO` mode.

LEDs signals the mode and state of the hatch :
- `UP` or `DOWN` LEDs will breath when opening or closing.
- `MIDDLE GREEN` LED will blink every 3 seconds when in `AUTO` mode.
- `MIDDLE BLUE` LED will blink every 3 seconds when in `MANUAL` mode.
- `MIDDLE RED` LED will blink every 3 seconds when an error occurs.
- `MIDDLE YELLOW` LED will breath while initializing.

When closing, the motor will stop once `motorTurnCount` is greater or equal to `motorTurnCountTarget`. When opening, the motor will stop once it reaches the endswitch.

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