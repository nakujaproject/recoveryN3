from tkinter import *
from tkinter import messagebox, ttk
from connection import SerialConnection
from serial import Serial, SerialException
import glob
import sys
from datetime import datetime, date
from PIL import ImageTk, Image
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from random import randint
import websocket
import csv

# HOST = "ws://192.168.172.93"

class App:
    def __init__(self,parent):
        super().__init__()
        self.parent = parent
        self.input_parameters = Frame(parent)
        self.logo = Frame(parent)
        self.graph_tab = Frame(parent, padx=10, bg="floralwhite")

        # test data
        style.use('ggplot')
        self.fig = plt.figure(figsize=(8, 6), dpi=100, edgecolor="green")
        self.ax = self.fig.add_subplot(1, 1, 1)

        self.xs = []
        self.ys = []

        self.logo.grid(row=0, column=0)
        self.input_parameters.grid(row=1, column=0)
        self.graph_tab.grid(row=0, column=1, columnspan=4, sticky="w")

        # set default appearance configurations

        self.fire_button_config = {
            "fg": "red",
            "bg": "gray10",
            "font": ("Arial", 15, "bold"),
            "relief": RAISED,
            "padx": 5,
            "pady": 4,
        }

        self.standby_button_config = {
            "fg": "green2",
            "bg": "gray60",
            "font": ("Arial", 15, "bold"),
            "relief": RAISED,
            "padx": 5,
            "pady": 4,
        }

        self.serial_button_config ={
            "fg": "black",
            "bg": "ghostwhite",
            "font": ("Arial", 12, "normal"),
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

        self.radio_button_config = {
            "font": ("Arial", 12, "normal"),
            "pady": 5
        }

        # useful data variables
        self.todays_date = date.today().strftime("%B %d, %Y")
        self.current_time = datetime.now().strftime(" %H:%M:%S")

        self.create_widgets()

        self.plot_canvas = FigureCanvasTkAgg(self.fig, self.parent)
        self.plot_canvas.get_tk_widget().grid(column=1, row=1)

        self.ani = animation.FuncAnimation(self.fig, self.animate, interval=1000)
        
    def create_widgets(self):
        """
        Create widgets for the main app window
        """

        # Nakuja project logo
        logo = ImageTk.PhotoImage(Image.open("app-images/logo.png"))

        logo_label = Label(self.logo, image=logo)
        logo_label.image = logo
        logo_label.grid(row=0, column=0)
        app_title = Label(self.logo, text="Piston Ejection Test System", font=("arial", 15, "bold"))
        app_title.grid(row=1, column=0)

        ports = self.get_serial_ports()
        # create and pack widgets for port
        self.port_select_label = Label(self.input_parameters, self.label_config, text="Select port:")
        # self.port_select_entry = Entry(self.input_parameters, self.entry_config, justify="right")
        self.port_select_combobox = ttk.Combobox(self.input_parameters, width=27, values=ports)

        # button to allow serial connection
        self.establish_serial_connection_button = Button(self.input_parameters, self.serial_button_config, text="Check connection", command=self.test_connection)
        self.establish_serial_connection_button.grid()

        # create and pack widgets for simulation time selection
        self.firing_time_label = Label(self.input_parameters, self.label_config, text="Pin HIGH time:")
        self.firing_time_entry = Entry(self.input_parameters, self.entry_config, justify="right")

        # widget to show type of powder being used
        self.powder_type_label = Label(self.input_parameters, self.label_config, text="Ejection powder:")
        self.powder_type_entry = Entry(self.input_parameters, self.entry_config, justify="right")
        self.powder_type_entry.insert(0, "Crimson Powder")
        self.powder_type_entry.config(state=DISABLED)

        # widget to display the mass of crimson powder used
        self.mass_of_crimson_used_label = Label(self.input_parameters, self.label_config, text="Mass of powder used:")
        self.mass_of_crimson_used_entry = Entry(self.input_parameters, self.entry_config, justify="right")

        # widget to show date and time of ejection test
        self.test_date_label = Label(self.input_parameters, self.label_config, text="Date:")
        self.test_date_entry = Entry(self.input_parameters, self.entry_config, justify="right")
        self.test_date_entry.insert(0, self.todays_date)
        self.test_date_entry.config(state=DISABLED)


        # widget to show test time
        self.test_time_label = Label(self.input_parameters, self.label_config, text="Time:")
        self.test_time_entry = Entry(self.input_parameters, self.entry_config, justify="right")
        self.test_time_entry.insert(0, self.current_time)
        self.test_time_entry.config(state=DISABLED)

        # option to save to internal file
        self.save_to_file = Radiobutton(self.input_parameters, self.radio_button_config, text="Save to CSV?")


        # create a FIRE button
        # play icon
        play_icon = ImageTk.PhotoImage(Image.open("app-images/play.jpg"))
        # play_icon_image = play_icon.subsample(1, 2) # resize to fit

        self.standby_button = Button(self.input_parameters, self.standby_button_config, text="STANDBY", command=self.connect_to_server)
        self.standby_button.grid(row=7, column=0)

        self.fire_button = Button(self.input_parameters, self.fire_button_config, text="FIRE")
        self.fire_button.grid(row=7, column=1)

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
        self.save_to_file.grid(row=6, column=1, sticky="e")

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

    def animate(self, i):
        '''
        This function is called periodically
        '''

        self.xs.append(datetime.now().strftime("%H:%M:%S.%f"))

        # data to plot
        self.ys.append(randint(10, 40))

        self.xs = self.xs[-20:]
        self.ys = self.ys[-20:]
        
        self.ax.clear()
        self.ax.plot(self.xs, self.ys)

        # format plot
        plt.xticks(rotation=45, ha="right")
        plt.subplots_adjust(bottom=0.30)
        plt.title("Force vs Time")
        plt.ylabel("Force(N)")

        # limit x axis to 20 items

    def connect_to_server(self):
        '''
        connect to server
        '''
        ws = websocket.WebSocket()
        
        ws.connect("ws://192.168.172.93")
        resp = ws.recv()

        print(resp)

    def save_to_file(self):
        ''' 
        save data received to internal memory
        this function runs if save to file checkbox is True
        '''
        # listen for url
        # get incoming value
        # save value to CSV (value, timestamp)

        with open("piston-force.csv", newline='') as csv_file:
            spam_writer = csv.writer(csv_file, delimiter=' ')
            spam_writer.writerow([randint(12, 20), datetime.now()])


def run_app():
    root = Tk()
    root.geometry('1250x700')
    root.title("N3 Ejection Test")
    root.resizable(False, False)
    main = App(root)

    root.mainloop()


if __name__ == "__main__":
    run_app()
