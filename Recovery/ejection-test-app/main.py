from tkinter import *
from tkinter import messagebox, ttk
from connection import SerialConnection
from serial import Serial, SerialException
import glob
import sys
from datetime import datetime, date
from PIL import ImageTk, Image


class App:
    def __init__(self, parent):
        self.parent = parent

        # set default appearance configurations

        self.button_config = {
            "fg": "green2",
            "bg": "gray10",
            "font": ("Arial", 15, "bold"),
            "relief": RAISED,
            "padx": 5,
            "pady": 4,
        }

        self.serial_button_config ={
            "fg": "gray10",
            "bg": "ghostwhite",
            "font": ("Arial", 10, "normal"),
            "relief": RAISED,
            "padx": 4,
            "pady": 2,
        }

        self.entry_config ={
            "fg": "black",
            "bg": "white",
            "font": ("Arial", 12, "normal"),
            "relief": RAISED,
            "width": 20
        }

        self.label_config = {
            "font": ("Arial", 12, "normal"),
            "pady": 5
        }

        # useful data variables
        self.todays_date = date.today().strftime("%B %d, %Y")
        self.current_time = datetime.now().strftime(" %H:%M:%S")

        self.create_widgets()

    def create_widgets(self):
        """
        Create widgets for the main app window
        """

        # Nakuja project logo
        # logo = ImageTk.PhotoImage( Image.open("./app-images/logo.png"))
        # logo_label = Label(self.parent, image=logo)
        # logo_label.grid(row=0, column=0)

        ports = self.get_serial_ports()
        # create and pack widgets for port
        self.port_select_label = Label(self.parent, self.label_config, text="Select port:")
        # self.port_select_entry = Entry(self.parent, self.entry_config, justify="right")
        self.port_select_combobox = ttk.Combobox(self.parent, width=27, values=ports)

        # button to allow serial connection
        self.establish_serial_connection_button = Button(self.parent, self.serial_button_config, text="Test connection", command=self.test_connection)
        self.establish_serial_connection_button.grid()

        # create and pack widgets for simulation time selection
        self.firing_time_label = Label(self.parent, self.label_config, text="Pin HIGH time:")
        self.firing_time_entry = Entry(self.parent, self.entry_config, justify="right")

        # widget to show type of powder being used
        self.powder_type_label = Label(self.parent, self.label_config, text="Ejection powder:")
        self.powder_type_entry = Entry(self.parent, self.entry_config, justify="right")
        self.powder_type_entry.insert(0, "Crimson Powder")
        self.powder_type_entry.config(state=DISABLED)

        # widget to display the mass of crimson powder used
        self.mass_of_crimson_used_label = Label(self.parent, self.label_config, text="Mass of powder used:")
        self.mass_of_crimson_used_entry = Entry(self.parent, self.entry_config, justify="right")

        # widget to show date and time of ejection test
        self.test_date_label = Label(self.parent, self.label_config, text="Date:")
        self.test_date_entry = Entry(self.parent, self.entry_config, justify="right")
        self.test_date_entry.insert(0, self.todays_date)
        self.test_date_entry.config(state=DISABLED)


        # widget to show test time
        self.test_time_label = Label(self.parent, self.label_config, text="Time:")
        self.test_time_entry = Entry(self.parent, self.entry_config, justify="right")
        self.test_time_entry.insert(0, self.current_time)
        self.test_time_entry.config(state=DISABLED)

        # create a FIRE button
        self.fire_button = Button(self.parent, self.button_config, text="FIRE")
        self.fire_button.grid(row=2, column=0, columnspan=4)

        # arrange all the widgets on the screen
        self.port_select_label.grid(row=0, column=0)
        self.firing_time_label.grid(row=1, column=0)
        self.powder_type_label.grid(row=2, column=0)
        self.mass_of_crimson_used_label.grid(row=3, column=0)
        self.test_date_label.grid(row=4, column=0)
        self.test_time_label.grid(row=5, column=0)

        # self.port_select_entry.grid(row=0, column=1, sticky="e")
        self.port_select_combobox.grid(row=0, column=1, sticky="e")
        self.establish_serial_connection_button.grid(row=0, column=2)
        self.firing_time_entry.grid(row=1, column=1, sticky="e")
        self.powder_type_entry.grid(row=2, column=1, sticky="e")
        self.mass_of_crimson_used_entry.grid(row=3, column=1, sticky="e")
        self.test_date_entry.grid(row=4, column=1, sticky="e")
        self.test_time_entry.grid(row=5, column=1, sticky="e")

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

        self.valid_ports = []
        for port in ports:
            try:
                s = Serial(port)
                s.close()
                self.valid_ports.append(port)
            except (OSError, SerialException):
                pass

        return self.valid_ports

    def test_connection(self):
        # for debug

        self.port_select_combobox.get()

        # this is not necessary

        if self.port_select_combobox.get() in self.valid_ports:
            flag = 0
            status = "Connection success!"
            title = "Success"
            self.show_message(flag, title, status)
        else:
            flag = 1
            status = "Port not found!"
            title = "Port not found"
            self.show_message(flag, title, status)

    def show_message(self, flag, title, message):
        """
        Display a message box
        :param flag: level of the message 0->INFO, 1->WARNING, 2->ERROR
        :param title:
        :param message:
        :return: message box widget
        """
        if flag == 0:
            messagebox.showinfo(title, message)
        elif flag == 1:
            messagebox.showwarning(title, message)
        elif flag == 2:
            messagebox.showerror(title, message)


def run_app():
    root = Tk()
    root.geometry('450x500')
    root.title("N3 Ejection Test")
    root.resizable(False, False)
    main = App(root)
    root.mainloop()


if __name__ == "__main__":
    run_app()
