import glob
import sys

from serial import Serial, SerialException

class SerialConnection:
    def __init__(self, serial_port, baud_rate):
        self.serial_port = Serial(serial_port, baud_rate)

    def get_serial_ports(self):
        """
        Finds all the serial ports in the computer depending on the platform
        :return: A list of connected serial ports
        """
        if sys.platform.startswith("win"):
            ports = ["COM%s" % (i+1) for i in range(256)]
        elif sys.platform.startswith("linux") or sys.platform.startswith("cygwin"):
            ports = glob.glob("/dev/tty[A-Za-z]*")
        elif sys.platform.startswith("darwin"):
            ports = glob.glob("/dev/tty.*")
        else:
            raise EnvironmentError("Unsupported platform")

        valid_ports = []
        for port in ports:
            try:
                s = Serial(port)
                s.close()
                valid_ports.append(port)
            except (OSError, SerialException):
                pass

        return valid_ports

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



