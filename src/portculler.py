from serial import Serial
from subprocess import run
# from sys import argv

###very beta###

def foo(line):
    print(line)
    run(["python", "test.py"])


def process_args():
    args = [arg for arg in argv[1:]]


def main():

    with Serial("COM5", timeout=1) as ser:
        while (True):
            line = ser.readline().decode().strip()
            if line:
                foo(line)


if __name__ == "__main__":
    main()
