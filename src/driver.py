from serial import Serial
# from subprocess import run


def foo():
    print("Foo!")
    run(["python", "test.py"])


def main():
    with Serial("COM5", timeout=1) as ser:
        while (True):
            line = ser.readline().decode().strip()
            if line:
                print(line)
                foo()


if __name__ == "__main__":
    main()
