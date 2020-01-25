# Portcullis

### Using Arduino, C++, and a dash of Python

<p align="center">
<img src="https://i.imgur.com/4ngS6eD.png?1">
</p>

Portcullis is a little box that lives on the door and adds some unsophisticated security
to a room. Keep track of accesses and/or attach your own callback process using `portculler`
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

###### If you plan on extending the Python:
```
python -m virtualenv </path/to/virtualenv>
pip install pyserial
```

Take a peak at [pyserial](https://github.com/pyserial/pyserial) if that floats your boat.

---
#### Use:
- ##### With COM (recommended):
	- Connect via USB 
	- `portculler.exe`
	- **or if you're extending the python**
	- `python portculler.py`
- ##### Without COM (limited):
	- Power it via USB or a barrel connector and watch it count how many times the door opened.
