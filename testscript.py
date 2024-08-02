import math
import time
import threading
import serial
import pathlib
import atexit


def exit_handler(device: serial.Serial):
    device.close()
    print(f"closed {device.port=}")




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

def read_acknowledge_per_page(device: serial.Serial):
    for i in range(9):
        ack = device.readline()
        print(ack)


def send_bin_file(device: serial.Serial, pagesize:int, sector_size: int, fpga: str, bin_file: str) -> None:
    device.write('b'.encode())
    ack_start_erase_and_load_binfile = device.readline()
    print(ack_start_erase_and_load_binfile)
    # Read file
    path_to_bin_file = pathlib.Path(f"./bin_files/{fpga}/{bin_file}")
    with open(path_to_bin_file, 'rb') as binfile:
        binary = binfile.read()
    size_of_binary = len(binary)

    # Write size of binary
    print(f"Send: Write size of binary")
    size_of_binary = (str(size_of_binary)+"\n").encode()
    device.write(size_of_binary)



    # Ack size of binary
    ack_size_of_binary = device.readline()
    print(ack_size_of_binary)
    ack_size_of_binary = device.readline()
    print(f"{ack_size_of_binary} == {size_of_binary}")

    num_sectors = math.ceil(len(binary)/sector_size) # Calculate sector_sizes_to_erase sizes of flash

    # Ack erase sectors
    for i in range(num_sectors):
        acknowledge = device.readline()
        print(f"{acknowledge}")


    # Write pages
    print("Send: Binfile")
    acknowledge = device.readline()
    print(f"{acknowledge}")
    len_binary = len(binary)
    percent = round(len_binary/100)
    for i, data in enumerate(binary):
        device.write(data)
        if i % percent == 0:
            print(f"\r{(i/len_binary):3.0%}", end='', flush=True)
        if i % pagesize == pagesize-1:
            print(f"{i=}")
            read_acknowledge_per_page(device)
        elif i == len(binary)-1:
            read_acknowledge_per_page(device)
    print("\n")
    print(f"bin_file_completed")
    acknowledge = device.readline()
    print(f"{acknowledge}")


if __name__ == '__main__':
    fpga = "S15"
    #fpga = "S50"


    elastic_node = serial.Serial(port="/dev/tty.usbmodem101", baudrate=115200)

    atexit.register(exit_handler, elastic_node)
    print(f"opened serial")
    pagesize, sector_size = read_config(elastic_node)
    print(f"Send: {pagesize=}")
    print(f"Send: {sector_size=}")

    send_bin_file(elastic_node, pagesize, sector_size, fpga, "blink_slow/led_test.bin")
    print(f"Power On FPGA")
    power_on_fpga(elastic_node)

