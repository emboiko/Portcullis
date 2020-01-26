from serial import Serial, SerialException
from subprocess import run
from sys import argv, path


class Portculler:
    def __init__(self):
        self.port = argv[1]
        self.callbacks = {
            "-1": self.reset,
            "-2": self.wrap,
            "-3": self.arm,
            "-4": self.disarm,
            "-5": self.open,
            "-6": self.close,
            "-7": self.start,
        }
        self.wraps = 0
        self.WRAP_LIMIT = 9999
        self.full_count = 0

        try:
            self._listen()
        except SerialException as err:
            print(err)


    def _listen(self):
        with Serial(self.port, timeout=1) as ser:
            while (True):
                line = ser.readline().decode().strip()
                if line:
                    self._callback(line)


    def _callback(self, line):
        if int(line) < 0:
            self.callbacks[line]()
        else:
            self.count(int(line))


    def count(self, num):
        self.full_count = num + (self.WRAP_LIMIT * self.wraps)
        print(self.full_count)


    def reset(self):
        print("Counter Reset.")


    def wrap(self):
        self.wraps += 1


    def arm(self):
        # run(["/path/to/your/script.exe"])
        print("Armed")


    def disarm(self):
        # run(["/path/to/your/script.exe"])
        print("Disarmed")


    def open(self):
        # run([f"{path[0]}\\down.vbs"], shell=True)
        print("Open")


    def close(self):
        # run([f"{path[0]}\\up.vbs"], shell=True)
        print("Closed")


    def start(self):
        # run(["/path/to/your/script.exe"])
        print("Porctullis Online...")


def main():
    portculler = Portculler()


if __name__ == "__main__":
    main()
