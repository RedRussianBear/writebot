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


def numerify(path_command):
    parse = search('(?P<type>[A-Z])(?P<x>[0-9]+?.?[0-9]*?),(?P<y>[0-9]+?.?[0-9]*?)', path_command).groupdict()
    return [parse['type'], float(parse['x']), float(parse['y'])]


def svg_write(svg_directory, ser1, ser2):
    files = [join(svg_directory, file) for file in listdir(svg_directory) if isfile(join(svg_directory, file))]

    for file in files:
        with open(file) as r:
            points = search('d="(.*?)"', r.read()).groups()
        points = [j for i in points for j in i.strip().split(' ')]
        points = [numerify(x) for x in points]

        for point in points:
            print('m%0.2f,%0.2f' % (point[X], point[Y]))

            if point[0] == 'M':
                print('lifting')
                ser1.write('p15'.encode('ascii'))
                wait_for(ser1)

            ser2.write(('m%0.2f,%0.2f' % (point[X], point[Y])).encode('ascii'))
            wait_for(ser2)

            if point[0] == 'M':
                print('lowering')
                ser1.write('p-15'.encode('ascii'))
                wait_for(ser1)

        ser1.write('p15'.encode('ascii'))
        ser2.write('M0,0'.encode('ascii'))
        ser1.write('p-15'.encode('ascii'))


def calibrate(ser1, ser2):
    command = input()
    while command[0] != 'q':
        if command[0] in {'c', 'C', 'f', 'F'}:
            ser1.write(command.encode('ascii'))
            ser2.write(command.encode('ascii'))
        if command[0] in {'x', 'X', 'y', 'Y'}:
            ser2.write(command.encode('ascii'))
        if command[0] in {'p', 'P', 'z', 'Z'}:
            ser1.write(command.encode('ascii'))

        command = input()


if __name__ == '__main__':
    board_0 = argv[2]
    board_1 = argv[3]
    serial1 = Serial(board_0, 9600, timeout=0)
    serial2 = Serial(board_1, 9600, timeout=0)
    wait_for(serial1)
    wait_for(serial2)

    if len(argv) == 4:
        calibrate(serial1, serial2)
        svg_write(argv[1], serial1, serial2)
    else:
        print('Incorrect number of arguments!')

    serial1.close()
    serial2.close()
