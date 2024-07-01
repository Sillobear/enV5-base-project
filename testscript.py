import serial


def read_config(device: serial.Serial) -> int:
    device.write('c'.encode())
    input_raw = device.readline()
    input_raw = input_raw.decode(encoding="utf-8")
    [_, pagesize] = input_raw.split(sep=":")
    return int(pagesize)


def power_on_fpga(device: serial.Serial) -> None:
    device.write('F'.encode())

def power_off_fpga(device: serial.Serial) -> None:
    device.write('f'.encode())

def send_bin_file(device: serial.Serial) -> None:
    device.write('b'.encode())

    with open('led_test.bin', 'rb') as binfile:
        binary = binfile.read()
        size_of_binary = len(binary)
        print(size_of_binary)

    size_of_binary = size_of_binary.to_bytes(8, 'little')
    print(size_of_binary)
    device.write(size_of_binary + '\n'.encode())




if __name__ == '__main__':
    ElasticNode = serial.Serial(port="/dev/ttyACM0", baudrate=115200)

    config = read_config(ElasticNode)
    print(f"Pagesize: {config}")

    send_bin_file(ElasticNode)

    ElasticNode.close()
