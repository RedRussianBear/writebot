from sys import argv
from os import listdir
from os.path import isfile, join
from time import sleep
from re import search
from serial import Serial

X = 1
Y = 2


def wait_for(ser):
    while ser.in_waiting == 0:
        sleep(.001)
    sleep(.01)
    return ser.read_all()


def tell(serial, message):
    return serial.write(message.encode('ascii'))


def numerify(path_command):
    parse = search('(?P<type>[A-Z])(?P<x>[0-9]+?.?[0-9]*?),(?P<y>[0-9]+?.?[0-9]*?)', path_command).groupdict()
    return [parse['type'], float(parse['x']), float(parse['y'])]


def svg_write(svg_directory, ser1, ser2):
    files = [join(svg_directory, file) for file in listdir(svg_directory) if isfile(join(svg_directory, file))]

    for file in files:
        with open(file) as r:
            paths = search('d="(.*?)"', r.read()).groups()
        paths = [[numerify(point) for point in path.strip().split(' ')] for path in paths]

        for path in paths:
            for point in path:
                print('m%0.2f,%0.2f' % (point[X], point[Y]))

                if point[0] == 'M':
                    print('lifting')
                    tell(ser1, 'p15')
                    wait_for(ser1)

                tell(ser2, ('M%0.3f,%0.3f' % (point[X], point[Y])))
                wait_for(ser2)

                if point[0] == 'M':
                    print('lowering')
                    tell(ser1, 'p-15')
                    wait_for(ser1)

        tell(ser1, 'p15')
        tell(ser2, 'M0,0')
        tell(ser1, 'p-15')


def calibrate(ser1, ser2):
    command = input()
    while command[0] != 'q':
        if command[0] in {'c', 'C', 'f', 'F'}:
            tell(ser1, command)
            tell(ser2, command)
        if command[0] in {'x', 'X', 'y', 'Y'}:
            tell(ser2, command)
        if command[0] in {'p', 'P', 'z', 'Z'}:
            tell(ser1, command)

        command = input()


if __name__ == '__main__':
    if len(argv) != 4:
        print('Incorrect number of arguments!')
        raise SystemExit

    board_0 = argv[2]
    board_1 = argv[3]
    serial1 = Serial(board_0, 9600, timeout=0)
    serial2 = Serial(board_1, 9600, timeout=0)
    wait_for(serial1)
    wait_for(serial2)

    calibrate(serial1, serial2)
    svg_write(argv[1], serial1, serial2)

    serial1.close()
    serial2.close()
