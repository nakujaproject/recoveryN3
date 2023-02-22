

from serial import Serial, SerialException

class SerialConnection:
    def __init__(self, serial_port, baud_rate):
        self.serial_port = Serial(serial_port, baud_rate)

    def establish_serial_connection(self):
        """
        Establish connection with the selected serial port
        :return:
        """
        if self.serial_port.open():
            return "Connection success!"
        else:
            return "Connection failed. Please try again"

    def close_serial_connection(self):
        pass



