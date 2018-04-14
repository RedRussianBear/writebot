from sys import argv
from os import listdir
from os.path import isfile, join
from time import sleep
from re import search
import serial

X = 1
Y = 2

def wait_for(ser):
    while ser.in_waiting == 0:
        sleep(.001)
    sleep(.01)
    return ser.read_all()


def numerify(path_command):
    parse = search('(?P<type>[A-Z])\s*?(?P<x>[0-9]+?.?[0-9]*?)[ ,]+(?P<y>[0-9]+?.?[0-9]*?)', path_command).groupdict()
    return [parse['type'], float(parse['x'])/3, float(parse['y'])/3]


def svg_write(svg_directory, board_0, board_1):
    ser1 = serial.Serial(board_0, 9600, timeout=0)
    ser2 = serial.Serial(board_1, 9600, timeout=0)

    wait_for(ser1)
    wait_for(ser2)

    files = [file for file in listdir(svg_directory) if isfile(join(svg_directory, file))]

    for file in files:
        with open(file) as r:
            points = search('d="(.*?)"', r.read()).groups()
        points = [j for i in points for j in i]
        points = [numerify(x) for x in points]

        for point in points:

            if point[0] == 'M':
                ser1.write('Z10,30')
                wait_for(ser1)

            ser2.write('M%f,%f' % (point[X], point[Y]))
            wait_for(ser2)

            if point[0] == 'M':
                ser1.write('Z-10,-30')
                wait_for(ser1)

    ser1.close()
    ser2.close()


if __name__ == '__main__':
    if len(argv) == 4:
        svg_write(argv[1], argv[2], argv[3])
    else:
        print('Incorrect number of arguments!')
