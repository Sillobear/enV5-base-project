import math
import time

import serial
import pathlib

def read_config(device: serial.Serial) -> tuple[int, int]:
    #device.write('l'.encode())
    time.sleep(5)
    device.write('c'.encode())

    input_raw = device.readline()
    input_raw = input_raw.decode(encoding="utf-8")
    [_, pagesize] = input_raw.split(sep=":")

    input_raw = device.readline()
    input_raw = input_raw.decode(encoding="utf-8")
    [_, sector_size] = input_raw.split(sep=":")

    return int(pagesize), int(sector_size)


def power_on_fpga(device: serial.Serial) -> None:
    device.write('F'.encode())


def power_off_fpga(device: serial.Serial) -> None:
    device.write('f'.encode())


def send_bin_file(device: serial.Serial, pagesize:int, sector_size: int, fpga: str, bin_file: str) -> None:
    device.write('b'.encode())

    # Read file
    path_to_bin_file = pathlib.Path(f"./bin_files/{fpga}/{bin_file}")
    with open(path_to_bin_file, 'rb') as binfile:
        binary = binfile.read()
    size_of_binary = len(binary)

    # Write size of binary
    size_of_binary = (str(size_of_binary)+"\n").encode()
    print(f"{len(binary)=} in bytes: {size_of_binary}")
    device.write(size_of_binary)

    # Ack size of binary
    ack_size_of_binary = device.readline()
    print(f"{ack_size_of_binary}")

    num_sectors = math.ceil(len(binary)/sector_size) # Calculate sector_sizes_to_erase sizes of flash

    # Ack erase sectors
    for i in range(num_sectors):
        acknowledge = device.readline()
        print(f"{acknowledge}")
    print(f"sending binfile")



    # Write pages
    print("Send Binfile: ")
    len_binary = len(binary)
    percent = round(len_binary/100)
    for i, data in enumerate(binary):
        device.write(data)
        if i % percent == 0:
            print(f"\r{(i/len_binary):3.0%}", end='', flush=True)
    print("\n")
    print(f"bin_file_completed")
    acknowledge = device.readline()
    print(f"{acknowledge}")


if __name__ == '__main__':
    fpga = "S50" #"S15"

    elastic_node = serial.Serial(port="/dev/tty.usbmodem1101", baudrate=115200)
    print(f"opened serial")
    pagesize, sector_size = read_config(elastic_node)
    print(f"{pagesize=}")
    print(f"{sector_size=}")

    send_bin_file(elastic_node, pagesize, sector_size, fpga, "blink_slow/led_test.bin")
    print(f"Power On FPGA")
    power_on_fpga(elastic_node)
    elastic_node.close()
