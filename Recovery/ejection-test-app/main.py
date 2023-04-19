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
import csv
import paho.mqtt.client as mqtt
from time import sleep


class App:
    def __init__(self,parent):
        super().__init__()
        self.parent = parent

        self.parameters_frame = Frame(self.parent, bg="gray30")
        self.parameters_frame.grid(row=1, column=0, padx=5)
        label = Label(text="Control Parameters", fg="white", bg="gray30", borderwidth=4, font=("Arial", 14, "bold"))
        fixed_label = Label(text="Fixed Parameters", fg="white", bg="gray30", borderwidth=4, font=("Arial", 14, "bold"))
        self.input_parameters = LabelFrame(self.parameters_frame, labelwidget=label, bg="gray30")
        self.input_parameters.grid(row=1, column=0, sticky='ew', pady=15, ipady=10)

        self.fixed_parameters = LabelFrame(self.parameters_frame, labelwidget=fixed_label, bg="gray30")
        self.fixed_parameters.grid(row=2, column=0, sticky='ew', pady=15, ipady=10)

        message_label= Label(text="Status Messages", fg="white", bg="gray30", borderwidth=4, font=("Arial", 14, "bold"))
        self.message_frame = LabelFrame(self.parameters_frame, labelwidget=message_label, bg="gray30")
        self.message_frame.grid(row=3, column=0, sticky='ew')

        self.logo = Frame(parent, bg="gray30")
        self.graph_tab = Frame(parent, padx=10, bg="gray40")
        self.message_variable = StringVar()
        self.message_entry = Entry(self.message_frame, font=("Arial", 15, "bold"), fg="darkgoldenrod1", bg="gray30", borderwidth=3, width=44, textvariable=self.message_variable)
        self.message_entry.grid(row=0, column=0, ipadx=4, ipady=4, sticky='ew')

        # test data
        style.use('ggplot')
        self.fig = plt.figure(figsize=(8, 6), dpi=100, edgecolor="green", facecolor="dimgray")
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.ax.spines['bottom'].set_color('black')
        self.ax.spines['top'].set_color('black')
        self.ax.xaxis.label.set_color('white')
        self.ax.yaxis.label.set_color('white')
        self.ax.tick_params(axis='x', colors='white')
        self.ax.tick_params(axis='y', colors='white')


        self.xs = []
        self.ys = []
        self.piston_force = 0

        self.logo.grid(row=0, column=0)
        self.graph_tab.grid(row=0, column=1, columnspan=4, sticky="w")

        # mqtt client creation
        self.client = mqtt.Client()
        self.client.on_message = self.on_message
        self.subscribe_topic = "piston/force"
        self.publish_topic = "piston/activate"

        # set default appearance configurations

        self.fire_button_config = {
            "fg": "green2",
            "bg": "gray10",
            "font": ("Arial", 17, "bold"),
            "relief": RIDGE,
            "padx": 5,
            "pady": 4,
        }

        self.abort_button_config = {
            "fg": "black",
            "bg": "red",
            "font": ("Arial", 17, "bold"),
            "relief": RIDGE,
            "padx": 5,
            "pady": 4,
        }

        self.standby_button_config = {
            "fg": "orange",
            "bg": "gray10",
            "font": ("Arial", 13, "bold"),
            "relief": RAISED,
            "padx": 2,
            "pady": 2,
        }

        self.btn_config = {
            "font": ("Arial", 15, "bold"),
            "relief" : RAISED,
            "padx": 5,
            "pady": 4,
            "bg": "gray40",
            "fg": "white"
        }

        self.serial_button_config ={
            "fg": "white",
            "bg": "steelblue",
            "font": ("Arial", 13, "bold"),
            "relief": RAISED
        }

        self.entry_config ={
            "fg": "red",
            "bg": "black",
            "font": ("Arial", 14, "bold"),
            "relief": SUNKEN,
            "width": 20,
            "borderwidth": 4,
            "insertbackground": "#ffffff"
        }

        self.label_config = {
            "font": ("Arial", 12, "normal"),
            "pady": 5,
            "bg" : "gray30",
            "fg": "white"
        }

        self.radio_button_config = {
            "font": ("Arial", 12, "normal"),
            "pady": 5
        }

        self.progress_bar_config = {
            "font": ("Arial", 12, "normal"),
            "pady": 5
        }

        # useful data variables
        self.todays_date = date.today().strftime("%B %d, %Y")
        self.current_time = datetime.now().strftime("%H:%M:%S")

        self.create_widgets()

        self.plot_canvas = FigureCanvasTkAgg(self.fig, self.parent)
        self.plot_canvas.get_tk_widget().grid(column=1, row=1)

        self.ani = animation.FuncAnimation(self.fig, self.animate, interval=400)
        
    def create_widgets(self):
        """
        Create widgets for the main app window
        """

        # Nakuja project logo
        logo = ImageTk.PhotoImage(Image.open("app-images/logo.png"))

        logo_label = Label(self.logo, image=logo, bg="gray30", width=40, height=40)
        logo_label.image = logo
        logo_label.grid(row=0, column=0)
        app_title = Label(self.logo, text="Piston Ejection Test Control", font=("arial", 13, "bold"), bg="gray30", fg="white")
        app_title.grid(row=0, column=1)

        ports = self.get_serial_ports()
        # create and pack widgets for port
        style = ttk.Style()
        style.theme_use('clam')
        style.configure("TCombobox", fieldbackground="black", fieldforeground="red", background="black", font=("Arial", 14, "bold"), borderwidth=4, width=27)
        self.port_select_label = Label(self.input_parameters, self.label_config, text="SELECT PORT:")
        # self.port_select_entry = Entry(self.input_parameters, self.entry_config, justify="right")
        self.port_select_combobox = ttk.Combobox(self.input_parameters, width=19, values=ports, font=("Arial", 14, "bold"))

        # button to allow serial connection
        self.establish_serial_connection_button = Button(self.input_parameters, self.serial_button_config, text="CHECK", command=self.test_serial_connection)
        self.establish_serial_connection_button.grid(padx=4)

        # create and pack widgets for simulation time selection
        self.firing_time_label = Label(self.input_parameters, self.label_config, text="PIN HIGH TIME(S):")
        self.firing_time_entry = Entry(self.input_parameters, self.entry_config, justify="right")

        # widget to display the mass of crimson powder used
        self.mass_of_crimson_used_label = Label(self.input_parameters, self.label_config, text="POWDER MASS(G):")
        self.mass_of_crimson_used_entry = Entry(self.input_parameters, self.entry_config, justify="right")


        # widget to show type of powder being used
        self.powder_type_label = Label(self.fixed_parameters, self.label_config, text="POWDER TYPE:")
        self.powder_type_entry = Entry(self.fixed_parameters, self.entry_config, justify="right")
        self.powder_type_entry.insert(0, "CRIMSON")
        # self.powder_type_entry.config(state=DISABLED)

        # widget to show date and time of ejection test
        self.test_date_label = Label(self.fixed_parameters, self.label_config, text="DATE:")
        self.test_date_entry = Entry(self.fixed_parameters, self.entry_config, justify="right")
        self.test_date_entry.insert(0, self.todays_date)
        # self.test_date_entry.config(state=DISABLED)

        # widget to show test time
        self.test_time_label = Label(self.fixed_parameters, self.label_config, text="TIME:")
        self.test_time_entry = Entry(self.fixed_parameters, self.entry_config, justify="right")
        self.test_time_entry.insert(0, self.current_time)
        # self.test_time_entry.config(state=DISABLED)

        self.save_parameters = Button(self.fixed_parameters, self.serial_button_config, text="SAVE", command=self.save_parameters_to_file)
        self.save_parameters.grid(row=7, column=1, pady=6)

        # create a FIRE button

        self.control_buttons= Frame(self.parent, bg="gray30")
        self.control_buttons.grid(row=0, column=1, padx=5, pady=5, sticky='ew')

        mqttlabel = Label(text="Network", fg="white", bg="gray30", borderwidth=4, font=("Arial", 14, "bold"))
        self.mqtt_frame = LabelFrame(self.control_buttons, labelwidget=mqttlabel, bg="gray30")
        self.mqtt_frame.grid(row=0, column=0)

        # buttons_label = Label(text="Network", fg="white", bg="gray30", borderwidth=4, font=("Arial", 14, "bold"))
        self.btns_frame = Frame(self.control_buttons, bg="gray30")
        self.btns_frame.grid(row=1, column=0, pady=10, sticky='ew', ipadx=5)

        self.mqtt_label = Label(self.mqtt_frame, self.label_config, text="BROKER URL: ")
        self.mqtt_label.grid(row=0, column=1)
        self.mqtt_url = Entry(self.mqtt_frame, self.entry_config)
        self.mqtt_url.grid(row=0, column=2)

        self.status_variable = StringVar()
        self.status_variable.set("STANDBY")

        self.standby_button = Button(self.mqtt_frame, self.standby_button_config, text="CONNECT", command=self.connect_to_server)
        self.standby_button.grid(row=0, column=3, padx=40, pady=4)

        self.fire_button = Button(self.btns_frame, self.fire_button_config, text="FIRE", command=self.send_fire_command)
        self.fire_button.grid(row=0, column=4)

        self.abort_button = Button(self.btns_frame, self.abort_button_config, text="ABRT", command=self.send_abort_command)
        self.abort_button.grid(row=0, column=5, padx=20, pady=4)

        # heating time progress bar
        # self.progress_bar = Frame(self.parent)
        # self.progress_bar.grid(row=0, column=1)
        # self.progress_bar_label = Label(self.progress_bar, self.label_config, text="Nichrome Heating Progress")
        # self.progress_bar_label.grid(row=0, column=0)
        # self.heating_progress_bar = ttk.Progressbar(self.progress_bar, orient="horizontal", mode="determinate", length=500)
        # self.heating_progress_bar.grid(row=1, column=0, padx=10, pady=10)
        # self.heating_value = Label(self.progress_bar, font=("Arial", 12, "bold"), text="2"+" sec")
        # self.heating_value.grid(row=1, column=1)

        # arrange all the widgets on the screen
        self.port_select_label.grid(row=0, column=0, sticky='e')
        self.firing_time_label.grid(row=1, column=0, sticky='e')
        self.mass_of_crimson_used_label.grid(row=2, column=0, sticky='e')

        self.powder_type_label.grid(row=0, column=0, sticky='e')
        self.test_date_label.grid(row=1, column=0, sticky='e')
        self.test_time_label.grid(row=2, column=0, sticky='e')

        # self.port_select_entry.grid(row=0, column=1, sticky="e")
        self.port_select_combobox.grid(row=0, column=1, sticky="e")
        self.establish_serial_connection_button.grid(row=0, column=2)
        self.firing_time_entry.grid(row=1, column=1, sticky="e")
        self.mass_of_crimson_used_entry.grid(row=2, column=1, sticky="e")

        self.powder_type_entry.grid(row=0, column=1, sticky="e")
        self.test_date_entry.grid(row=1, column=1, sticky="e")
        self.test_time_entry.grid(row=2, column=1, sticky="e")

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

    def update_progress_bar(self):
        '''
        Update progress bar showing nichrome wire heating time
        :return:
        '''
        
        
        # get the value entered in the pin high time
        self.pin_high = self.firing_time_entry.get()

        for second in range(0, int(self.pin_high), 1):
            self.heating_progress_bar['value'] += second
            self.heating_value['text'] = self.update_test()

    def test_serial_connection(self):
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

    def save_parameters_to_file(self):
        '''this function saves the users input'''
        curr_time = datetime.now().strftime("%S")
        
        com = self.port_select_combobox.get()
        pin_high = self.firing_time_entry.get()
        powder_type = self.powder_type_entry.get()
        powder_mass = self.mass_of_crimson_used_entry.get()
        date_of_test = self.test_date_entry.get()
        time_of_test = self.test_time_entry.get()

        # open parameters file
        with open("parameters_"+curr_time+".txt", "a") as parameters_file:
            parameters_file.write("COM PORT: " + com + "\n")
            parameters_file.write("Pin high time: " + pin_high + "\n")
            parameters_file.write("Powder type: " + powder_type + "\n")
            parameters_file.write("Powder mass: " + powder_mass + "\n")
            parameters_file.write("Date of test: " + date_of_test + "\n")
            parameters_file.write("Time of test: " + time_of_test + "\n")

    def save_force_to_csv(self):
        '''
        save data received to internal memory
        this function runs if save to file checkbox is True
        '''
        # listen for url
        # get incoming value
        # save value to CSV (value, timestamp)

        with open("piston_force.csv", newline='') as csv_file:
            spam_writer = csv.writer(csv_file, delimiter=' ')
            spam_writer.writerow([randint(12, 20), datetime.now()])

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

        self.xs.append(datetime.now().strftime("%S"))

        # data to plot
        self.ys.append(self.piston_force)

        self.xs = self.xs[-20:]
        self.ys = self.ys[-20:]
        
        self.ax.clear()
        self.ax.plot(self.xs, self.ys, color="g")

        # format plot
        plt.xticks(rotation=45, ha="right")
        plt.subplots_adjust(bottom=0.30)
        plt.title("Force vs Time", color="white")
        plt.ylabel("Force(N)")
        plt.xlabel("Time(Sec)")


    def connect_to_server(self):
        '''
        connect to mqtt server
        '''

        # get broker URL
        broker = self.mqtt_url.get()

        self.status_variable.set("Connecting to broker...")

        self.client.connect(broker)  # connect to broker

        # update standby button to ready
        self.standby_button.config(fg="green1", text="CONNECTED")
        self.client.subscribe(self.subscribe_topic)

        self.status_variable.set("Connected to broker")
        self.client.loop_start()

    def on_message(self, client, userdata, message):
        '''

        process any incoming data from MQTT server
        '''

        self.piston_force = message.payload.decode("utf-8")

        
    def send_fire_command(self):
        '''
        send command to fire the piston
        :return:
        '''

        self.client.publish(self.publish_topic, "FIRE")
        self.client.loop_start()

    def send_abort_command(self):
        '''
        send command to cancel piston firing
        :return:
        '''

        self.client.publish(self.publish_topic, "ABORT")
        self.client.loop_start()


def run_app():
    root = Tk()
    root.geometry('1250x740')
    root.title("N3 Ejection Test")
    root.resizable(False, False)
    root.config(bg="gray30")
    main = App(root)
    # main.client.loop_start()

    root.mainloop()


if __name__ == "__main__":
    run_app()
