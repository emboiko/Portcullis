# Portcullis

### Using Arduino, C++, and a dash of Python

<p align="center">
<img style="width:25%;"src="https://i.imgur.com/4ngS6eD.png?1">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/vBMjLzB.jpg">
</p>

Portcullis is a little box that lives on the door and adds some unsophisticated security
to a room via a laser transmitter + receiver. Keep track of accesses and/or attach your own callback process with `portculler.py`

###### Coming soon: drawings and .STL are in development for a 3D printable housing

---
#### Hardware:

- ##### Arduino Uno
- ##### 3x LED
- ##### 2x Momentary switch
- ##### 4-bit 7-segment LED display
- ##### Laser emitter
- ##### Laser receiver
---
#### Installation:

`git clone https://github.com/emboiko/Portcullis.git Portcullis`

- Import to platformio w/  VSCode or Arduino IDE
- Build & upload to your board.

##### If you plan on extending the Python:
```
python -m virtualenv </path/to/virtualenv>
pip install pyserial
```

Take a peak at [pyserial](https://github.com/pyserial/pyserial) if that floats your boat.

---
#### Use:
- ##### With COM (recommended):
	- Connect via USB 
	- `portculler.exe COMx` (where `x` is an appropriate integer COM port #)
	- **or if you're extending the python**
	- `python portculler.py COMx`
- ##### Without COM (limited):
	- Power it via USB or a barrel connector and watch it count how many times the door opened.

#### Overview:
<p align="center">
Upon powering the device, we'll find the display reading 0 with the yellow LED indicating a disarmed state.
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/DDfDWrw.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/LQDIJfv.jpg?1">
</p>

<p align="center">
Under the current configuration, the red button will arm or disarm the device, and the white button is used for a reset.
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/sK0QspW.jpg?1">
</p>

<p align="center">
Let's arm the device by pressing the red button, and observe the results. If our laser beam is unbroken, we'll find the counter is not incremented, and the green LED is now lit up.
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/OhumYMr.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/EukgzGO.jpg">
</p>

<p align="center">
In the case that the laser beam is broken, we'll observe the red LED being lit up.
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/LRdT5YP.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/D0pViLF.jpg">
</p>

<p align="center">
For this example, let's begin with the device armed and the laser unbroken. Our 3D printed cube will act as the laser break and simulate a swinging door. The counter will increment for each time the system is "opened", or in this case "blocked".
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/Vlm6eoS.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/9CR41MW.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/GugcVqK.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/g0BGSa0.jpg">
</p>
<p align="center">
<img style="width:50%;"src="https://i.imgur.com/wsO2DSk.jpg">
</p>

---

#### Using `Portculler.py`

<p align="center">
Portculler is a "driver" for Portcullis, made with Python and compiled with PyInstaller. 
Let's start by loading up the program, and arming the device. In this case, we've armed with a "closed" system:
</p>
<p align="center">
<img src="https://i.imgur.com/vTr5iaa.png?1">
</p>

<p align="center">
Subsequent breaks in the laser beam will result in some additional output in the terminal. Here, our system opened and closed twice:
</p>
<p align="center">
<img src="https://i.imgur.com/am3lX9c.png">
</p>

<p align="center">
Arm, disarm, and reset (in this case, with a closed system):
</p>
<p align="center">
<img src="https://i.imgur.com/0vPjFsi.png">
</p>

<p align="center">
Portculler will continue to keep track of the count when the 7-segment display no longer can fit the data:
</p>

<p align="center">
<img style="width:50%;"src="https://i.imgur.com/I1j35b8.jpg">
</p><p align="center">
<img style="width:50%;"src="https://i.imgur.com/OpguGUt.jpg">
</p>
<p align="center">
At the terminal:
</p>
<p align="center">
<img src="https://i.imgur.com/wJRRBEh.png">
</p>

#### Attaching more functionality:

<p align="center">
The following methods (and the entire class, for that matter) in Portculler.py are intended specifically for extension towards different and unique scenarios. By default, only terminal output is produced. Furthermore, if you wish to extend Portculler with your own callbacks, it's as simple as modifying/extending these methods in the python. In this naive example, we'll minimize all windows if the system gets opened, and bring them back up when it's closed.
</p>

```
def arm(self):
	# run(["/path/to/your/script.exe"])
	print("Armed")


def disarm(self):
	# run(["/path/to/your/script.exe"])
	print("Disarmed")


def open(self):
	run([f"{path[0]}\\down.vbs"], shell=True)
	print("Open")


def close(self):
	run([f"{path[0]}\\up.vbs"], shell=True)
	print("Closed")


def start(self):
	# run(["/path/to/your/script.exe"])
	print("Porctullis Online...")
```

```
' down.vbs

set oShellApp = CreateObject("Shell.Application")

oShellApp.MinimizeAll
```

```
' up.vbs

set oShellApp = CreateObject("Shell.Application")

oShellApp.UndoMinimizeAll
```
