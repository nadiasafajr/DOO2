import tkinter as tk
from tkinter import messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import time
import serial
import threading

# Define the MainGUI class
class MainGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Coupled Tank App")
        self.geometry("800x400+100+100") # Set the size to 800x400 (width x height in pixel) & the position to (100,100) The first value 100 is the x-offset and the second value 100 is the y-offset.
        self.dataList = [] # Store sensor data points
        self.valve = 0 # Store valve value
        
        # Create a button for START & STOP pump
        self.on_button = tk.Button(self, text="START PUMP", command=self.on_pump, bg="#808080", fg="white", width=15)
        self.on_button.pack()
        self.off_button = tk.Button(self, text="STOP PUMP", command=self.off_pump, bg="#808080", fg="white", width=15)
        self.off_button.pack()
        
        self.on_button.pack(side=tk.BOTTOM)
        self.off_button.pack(side=tk.BOTTOM)
        self.on_button.place(x=7, y=5)
        self.off_button.place(x=7, y=35)
        
        # Create a figure and canvas for the plot
        self.fig =  plt.figure(figsize=(5,3))
        self.ax = self.fig.add_subplot(111)
        self.canvas = FigureCanvasTkAgg(self.fig, self)
        self.canvas.get_tk_widget().place(x=200, y=5, width=400, height=300)
        
        # Create a axis limit for the plot
        self.ax.set_title("Tank 1 - Level Tracking (cm)")
        self.ax.set_ylim(0,50) # y-axis limit 
        self.ax.set_xlim(0,100) # x-axis limit
        self.ax.set_xlabel("Time (sec)")
        self.ax.set_ylabel("Water Level (cm)")
        
        # Create a label for the valve value
        self.valve_label = tk.Label(self, text="Valve: 0")
        self.valve_label.pack(side=tk.BOTTOM)
        self.valve_label.place(x=7, y=80)
        
        # Connect to the Arduino Board (make sure your board match)
        self.ser = serial.Serial("COM7", 9600)
        time.sleep(2)
        
        # Create and Start a thread for receiving data from the Arduino
        self.data_thread = threading.Thread(target=self.get_data)
        self.data_thread.start()
    
    # This both function will toggle the state of the relay
    def on_pump(self):
        self.ser.write(b'1') # Send '1' to the Arduino to turn on the relay
        messagebox.showinfo("Info!", "Pompa telah diaktifkan")
    def off_pump(self):
        self.ser.write(b'0') # Send '0' to the Arduino to turn off the relay
        messagebox.showinfo("Info!", "Pompa telah dimatikan")
    
    # This function will start a thread for receiving sensor data
    def get_data(self):
        while True:
            self.ser.write(b'g') # Transmit the char to receive the Arduino data point
            arduinoData_string = self.ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string
            
            try:
                # Extract the sensor value and valve value from the data
                sensor_value, valve_value = arduinoData_string.split(',')
                #sensor_value = float(arduinoData_string)
                
                sensor_value = float(sensor_value)
                valve_value = int(valve_value)
                
                # Update the plot and valve label
                self.ax.clear()
                self.ax.plot(range(len(self.dataList)), self.dataList)
                self.canvas.draw()
                self.valve_label.config(text="Valve: {}".format(self.valve))
                
            except:
                # Pass if data point is invalid
                pass


gui = MainGUI()
gui.mainloop()
        
        
        
        

