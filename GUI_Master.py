from tkinter import *
from PIL import Image, ImageTk
from tkinter import messagebox
import time
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from datetime import datetime
import numpy as np
import csv
import queue

class RootGUI():
    def __init__(self):
        '''Initializing the root GUI'''
        self.root = Tk()
        self.root.title("Coupled Tank App")
        self.root.geometry("295x175")
        self.root.config(bg="white")

        self.logo()

    def logo(self):
        self.image = Image.open("C:\\Users\\nasaf\\Downloads\\logooohh.png")
        self.photo_telu = ImageTk.PhotoImage(self.image)
        self.label_logo1 = Label(self.root, image=self.photo_telu, bg="white")
        self.label_logo1.grid(row=0, column=0, padx=10, pady=7)

        #self.image_doo2 = Image.open("C:\\Users\\nasaf\\Downloads\\DOO2_GUI_LABEL.png")
        #self.photo_doo2 = ImageTk.PhotoImage(self.image_doo2)
        #self.label_logo2 = Label(self.root, image=self.photo_doo2, bg="white")
        #self.label_logo2.grid(row=0, column=1, pady=10)


class ComGUI():
    def __init__(self, root, serial):
        '''
        Initialize the connection GUI and initialize the main widgets 
        '''
        # Initializing the Widgets
        self.root = root
        self.serial = serial
        self.frame = LabelFrame(root, text="Com Manager",
                                padx=5, pady=5, bg="white")
        self.label_com = Label(
            self.frame, text="Available Port(s): ", bg="white", width=15, anchor="w")

        # Setup the Drop option menu
        self.ComOptionMenu()

        self.btn_connect = Button(self.frame, text="Connect",
                                  width=13, state="disabled",  command=self.serial_connect)

        # Optional Graphic parameters
        self.padx = 25
        self.pady = 5

        # Put on the grid all the elements
        self.publish()

    def publish(self):
        '''
         Method to display all the Widget of the main frame
        '''
        self.root.geometry("297x177")
        self.frame.grid(row=1, column=0, rowspan=3,
                        columnspan=3, padx=5, pady=5)
        
        self.label_com.grid(column=1, row=2)

        self.drop_com.grid(column=2, row=2, padx=self.padx)

        self.btn_connect.grid(column=2, row=3, padx=self.padx, pady=self.pady)

    def ComOptionMenu(self):
        '''
         Method to Get the available COMs connected to the PC
         and list them into the drop menu
        '''
        # Generate the list of available coms

        self.serial.getCOMList()

        self.clicked_com = StringVar()
        self.clicked_com.set(self.serial.com_list[0])
        self.drop_com = OptionMenu(
            self.frame, self.clicked_com, *self.serial.com_list, command=self.connect_ctrl)

        self.drop_com.config(width=10)

    def connect_ctrl(self, widget):
        '''
        Mehtod to keep the connect button disabled if all the 
        conditions are not cleared
        '''
        print("Connect ctrl")
        # Checking the logic consistency to keep the connection button
        if "-" in self.clicked_com.get():
            self.btn_connect["state"] = "disabled"
        else:
            self.btn_connect["state"] = "active"

    def serial_connect(self):
        '''
        Method that Updates the GUI during connect / disconnect status
        Manage serials and data flows during connect / disconnect status
        '''
        if self.btn_connect["text"] in "Connect":
            # Start the serial communication
            self.serial.SerialOpen(self)

            # If connection established move on
            if self.serial.ser.status:
                # Update the COM manager
                self.btn_connect["text"] = "Disconnect"
                InfoMsg = f"Successful UART connection using {self.clicked_com.get()}"
                messagebox.showinfo("showinfo", InfoMsg)

                # Display the channel manager
                self.ctrl = CtrlMode(self.root, self.serial)

            else:
                ErrorMsg = f"Failure to estabish UART connection using {self.clicked_com.get()} "
                messagebox.showerror("showerror", ErrorMsg)
        else:

            # Closing the Serial COM
            # Close the serial communication
            self.serial.SerialClose(self)

            # Closing the Conn Manager
            # Destroy the channel manager
            #self.conn.ConnGUIClose()

            InfoMsg = f"UART connection using {self.clicked_com.get()} is now closed"
            messagebox.showwarning("showinfo", InfoMsg)
            self.btn_connect["text"] = "Connect"
            #self.btn_refresh["state"] = "active"
            #self.drop_baud["state"] = "active"
            self.drop_com["state"] = "active"


class CtrlMode():
    def __init__(self, root, serial):
        '''
        Initialize main widgets for Control Mode : Auto/Manual
        '''
        self.root = root
        self.serial = serial

        self.frame = LabelFrame(root, text="Control Mode",
                                padx=5, pady=5, bg="white", width=60)
        
        self.ctrl_label = Label(self.frame, text="Status: ", 
                                bg="white", width=15, anchor="w")
        self.ctrl_status = Label(self.frame, text="..Null..", 
                                 bg="white", fg="orange", width=5)
        
        self.mode_label = Label(self.frame, text="Mode: ",
                                bg="white", width=15, anchor="w")
        self.mode_btn_auto = Button(self.frame, text="Auto", state="active",
                                    width=7, command= self.open_auto)
        self.mode_btn_manual = Button(self.frame, text="Manual", state="active",
                                      width=7, command= self.open_manual)
        
        self.padx = 15
        self.pady = 13

        self.CtrlModeOpen()

    def CtrlModeOpen(self):
        '''
        Display all the widgets
        '''
        self.root.geometry("612x177")
        self.frame.grid(row=1, column=4, rowspan=2,
                        columnspan=3, padx=5, pady=5)
        
        self.ctrl_label.grid(column=1, row=1)
        self.ctrl_status.grid(column=2, row=1)

        self.mode_label.grid(column=1, row=2, pady=self.pady)
        self.mode_btn_auto.grid(column=2, row=2, padx=self.padx)
        self.mode_btn_manual.grid(column=3, row=2, padx=self.padx)

    def open_auto(self):
        self.mode_btn_auto["state"] = "active" 
        self.mode_btn_manual["state"] = "disabled"
        self.ctrl_status.config(text="Auto", fg="Red")

        self.window_auto = Window_Auto(self.root, self.serial)
        self.window_auto.Master_Auto()

        self.window_auto.window1.protocol("WM_DELETE_WINDOW", self.window_close_auto)

    def open_manual(self):
        self.mode_btn_manual["state"] = "active" 
        self.mode_btn_auto["state"] = "disabled"
        self.ctrl_status.config(text="Manual", fg="blue")

        # Send a signal byte '4' to Arduino for manual open loop control
        self.serial.SerialWrite(b'4')

        self.window_manual = Window_Manual(self.root, self.serial)
        self.window_manual.Master_Manual()

        self.window_manual.window2.protocol("WM_DELETE_WINDOW", self.window_close_manual)

    def window_close_auto(self):
        self.mode_btn_auto["state"] = "active" 
        self.mode_btn_manual["state"] = "active"

        self.ctrl_status.config(text="..Null..", fg="orange")
        self.window_auto.window1.destroy()
    
    def window_close_manual(self):
        self.mode_btn_auto["state"] = "active" 
        self.mode_btn_manual["state"] = "active"

        self.ctrl_status.config(text="..Null..", fg="orange")
        self.window_manual.window2.destroy()

    
class Window_Auto():
    def __init__(self, root, serial):
        '''
        Initialize variables
        '''
        self.root = root
        self.serial = serial

        self.inletData = DoubleVar()                 
        self.Tank1Data = DoubleVar()                 
        self.Tank2Data = DoubleVar()                 

        self.filename = ''
        self.frames = []

        self.dataList_Tank1 = []
        self.dataList_Tank2 = []
        self.dataList_Inflow = []
        self.FloatListData = []
        
        self.XData = []
        self.DisplayTimeRange = 5.0
        self.RefTime = 0.0

        self.save = False
        self.data_queue = queue.Queue()  # Initialization (queue)
        self.data_to_csv_thread = None
        
    def Master_Auto(self):
        self.Masterframe_Auto()
        self.Connframe()
        self.Connframe_publish()

    def Masterframe_Auto(self):
        self.window1 = Toplevel(self.root, bg="white")
        self.window1.title("Auto - Closed Loop")
        self.Parentframe()
    
    def Parentframe(self):
        self.parent_frame = Frame(self.window1, padx=11, pady=11, bg="white", borderwidth=2, relief="groove")
        self.parent_frame.grid(row=0, column=0, padx=10, pady=12)
        self.Super_Parentframe()

    def Super_Parentframe(self):
        self.super_frame1 = Frame(self.parent_frame, bg="white", padx=5)
        self.super_frame1.grid(row=0, column=0, rowspan=6 , columnspan=2)

        self.super_frame2 = Frame(self.parent_frame, bg="white", padx=5, pady=5)
        self.super_frame2.grid(row=0, column=3, rowspan=6 , columnspan=3)

    def Connframe(self):
        self.frame_conn = LabelFrame(self.super_frame1, text="Main Manager",
                                padx=5, pady=5, bg="white", width=60)
        
        self.tankctrl_label = Label(self.frame_conn, text="Tank Control: ",
                                bg="white", width=15, anchor="w")
        
        self.clicked_ctrl = StringVar()
        self.tankctrl_list = ["-", "Tank 1", "Tank 2"]
        self.clicked_ctrl.set(self.tankctrl_list[0])
        self.tankctrl_dropdown = OptionMenu(self.frame_conn, self.clicked_ctrl,
                                            *self.tankctrl_list, command=lambda selected_value: self.ctrltank_selected(selected_value))

        self.pump_label = Label(self.frame_conn, text="Pump Toggle: ",
                                bg="white", width=15, anchor="w")
        
        self.btn_pumpctrl = Button(self.frame_conn, text="Start", state="disabled",
                                width=9, command=self.button_clicked)

        self.save = False
        self.SaveVar = IntVar()
        self.save_check = Checkbutton(self.frame_conn, text="Save to CSV", anchor="w", variable=self.SaveVar,
                                      onvalue=1, offvalue=0, bg="white", state="active",
                                      command=self.save_data)
        
    def Connframe_publish(self):
        '''
        Method to display all the widgets 
        '''
        self.padx = 15
        self.pady = 5

        self.window1.geometry("295x188")
        self.frame_conn.grid(row=0, column=0, rowspan=3,
                        columnspan=2, padx=5, pady=5)
        
        self.tankctrl_label.grid(column=0, row=0)

        self.tankctrl_dropdown.grid(column=1, row=0, padx=self.padx, pady=self.pady)

        self.pump_label.grid(column=0, row=1)

        self.btn_pumpctrl.grid(column=1, row=1, padx=self.padx)

        self.save_check.grid(column=0, row=2, pady=self.pady)

    def ctrltank_selected(self, selected_value):
        '''
        Method to keep the start pump button disabled if user not clicked the ctrltank
        and to set selected tank to control by the PID
        '''
        print("Tank Ctrl")
        # Checking the logic consistency to keep the connection button
        if selected_value == "-":
            self.btn_pumpctrl["state"] = "disabled"
        else:
            self.btn_pumpctrl["state"] = "active"

        # send message to arduino by the selected tank to control
        if selected_value == "Tank 1":
            send = "2"
        elif selected_value == "Tank 2":
            send = "3"
        else:
            send = ""

        print(send)
        self.serial.SerialWriteEncode(send)

    def button_clicked(self):
        if self.btn_pumpctrl["text"] in 'Start':
            # start the pump, go to start_pump() function
            self.start_pump()
            self.btn_pumpctrl['text'] = 'Stop'

        elif self.btn_pumpctrl["text"] in  'Stop':
            # stop the pump, go to stop_pump() function
            self.stop_pump()
            self.btn_pumpctrl['text'] = 'Start'

    def start_pump(self):
        self.serial.SerialWrite(b'1') # Send '1' to the Arduino to turn on the relay
        InfoMsg = "Pump activated (ON)"
        messagebox.showinfo("showinfo", InfoMsg)

        self.Displayframe()

    def stop_pump(self):
        self.serial.SerialWrite(b'0') # Send '0' to the Arduino to turn off the relay
        WarningMsg = "Pump deactivated (OFF)"
        messagebox.showwarning("showwarning", WarningMsg)

    def save_data(self):
        self.save = not self.save  # Membalik nilai self.save

        if self.save:
            self.set_filename()
            self.StreamDataCheck()
            self.start_data_to_csv_thread()
        else:
            # Menghentikan thread data_to_csv jika sedang berjalan
            if self.data_to_csv_thread is not None and self.data_to_csv_thread.is_alive():
                self.data_queue.put(None)  # Mengirim sinyal None ke antrian untuk menghentikan thread
                self.data_to_csv_thread.join()  # Menunggu thread selesai

    def start_data_to_csv_thread(self):
        if self.data_to_csv_thread is None or not self.data_to_csv_thread.is_alive():
            self.data_to_csv_thread = threading.Thread(target=self.data_to_csv, daemon=True)
            self.data_to_csv_thread.start()

    def StreamDataCheck(self):
        if self.update_thread:
            self.FloatListData = list(zip(
                self.XData,
                self.dataList_Inflow,
                self.dataList_Tank1,
                self.dataList_Tank2
            ))

    def set_filename(self):
        now = datetime.now()
        self.filename = now.strftime("%Y_%m_%d_%H-%M-%S") + ".csv"
    
    def data_to_csv(self):
        with open(self.filename, 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(['Time (s)', 'Inflow', 'Tank1', 'Tank2'])

        while self.save:
            try:
                data = self.data_queue.get(timeout=1)  # Mengambil data dari antrian dengan waktu tunggu maksimum 1 detik
                with open(self.filename, 'a', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow(data)
                self.data_queue.task_done()  # Menandai bahwa data telah selesai diproses
            except queue.Empty:
                pass

    def Displayframe(self):
        self.frame_dis = LabelFrame(self.super_frame1, text="Display Manager", 
                                padx=5, bg="white", width=60)
        
        self.inlet_label = Label(
            self.frame_dis, text="Inlet Flow (L/min):", bg="white", width=20, anchor="w")
        self.inletData_label = Label(
            self.frame_dis, text="0.00", bg="white", fg="green", width=5, textvariable=self.inletData)
        
        self.Tank1_label = Label(
            self.frame_dis, text="Water Level Tank 1 (cm):", bg="white", width=20, anchor="w")
        self.Tank1Data_label = Label(
            self.frame_dis, text="0.00", bg="white", fg="green", width=5, textvariable=self.Tank1Data)
        
        self.Tank2_label = Label(
            self.frame_dis, text="Water Level Tank 2 (cm):", bg="white", width=20, anchor="w")
        self.Tank2Data_label = Label(
            self.frame_dis, text="0.00", bg="white", fg="green", width=5, textvariable=self.Tank2Data)
        
        self.Displayframe_publish()

    def Displayframe_publish(self):
        '''
        Method to display all the widgets 
        '''
        self.frame_dis.grid(row=4, column=0, rowspan=3,
                        columnspan=2, padx=5, pady=5)
        
        self.window1.geometry("300x300")
        self.inlet_label.grid(column=0, row=0)
        self.inletData_label.grid(column=1, row=0, pady=7, padx = 18, sticky="w")
        
        self.Tank1_label.grid(column=0, row=1)
        self.Tank1Data_label.grid(column=1, row=1, pady=8, padx = 18, sticky="w")

        self.Tank2_label.grid(column=0, row=2)
        self.Tank2Data_label.grid(column=1, row=2, pady=8, padx = 18, sticky="w")

        self.Controllerframe()

    def update_data(self):
        while True:

            inletData_float = 0.0
            Tank1Data_float = 0.0
            Tank2Data_float = 0.0

            Data_string = None
            Data_string = self.serial.SerialReadline()

            if Data_string is not None:
                time.sleep(0.5)
                print(" Data_string:", Data_string)

                # Split data into FOUR variables
                data_list = Data_string.split(',', maxsplit=3)
                print("data_list:", data_list)
                if len(data_list) == 4:
                    Tank1Data_split, Tank2Data_split, inletData_split, _ = data_list
                    try:
                        inletData_float = float(inletData_split)
                        Tank1Data_float = float(Tank1Data_split)
                        Tank2Data_float = float(Tank2Data_split)

                        self.inletData.set(inletData_float)
                        self.Tank1Data.set(Tank1Data_float)
                        self.Tank2Data.set(Tank2Data_float)

                        self.dataList_Inflow.append(inletData_float)
                        self.dataList_Tank1.append(Tank1Data_float)
                        self.dataList_Tank2.append(Tank2Data_float)

                        # Update the value text on motor control frame
                        self.inletData_label.config(text="{:.2f}".format(self.inletData.get()))
                        self.Tank1Data_label.config(text="{:.2f}".format(self.Tank1Data.get()))
                        self.Tank2Data_label.config(text="{:.2f}".format(self.Tank2Data.get()))

                        self.UpdataXdata()  
                        self.SetRefTime()
                        self.AdjustData()

                        # Mengirim data ke antrian
                        self.data_queue.put([round(self.XData[-1], 1), inletData_float, Tank1Data_float, Tank2Data_float])

                    except ValueError:
                        pass
            else:
                print("Error: Data_string is None or empty")

    def Controllerframe(self):
        self.frame_ctrl = LabelFrame(self.super_frame2, text="Controller Manager", 
                                padx=5, pady=5, bg="white", width=60)
        
        self.kp_entry = Entry(self.frame_ctrl, bg="white", width=8)
        self.kp_label = Label(self.frame_ctrl, text="Kp: ", bg="white", width=16, anchor="w")

        self.ki_entry = Entry(self.frame_ctrl, bg="white", width=8)
        self.ki_label = Label(self.frame_ctrl, text="Ki: ", bg="white", width=16, anchor="w")

        self.kd_entry = Entry(self.frame_ctrl, bg="white", width=8)
        self.kd_label = Label(self.frame_ctrl, text="Kd: ", bg="white", width=16, anchor="w")
        
        self.setpoint_entry = Entry(self.frame_ctrl, bg="white", width=8)
        self.setpoint_label = Label(self.frame_ctrl, text="Setpoint (cm): ", bg="white", width=16, anchor="w")

        self.send_btn = Button(self.frame_ctrl, text="Enter", state="active",
                               width=7, command=self.send_data_to_arduino)
        
        self.Controllerframe_publish()

    def Controllerframe_publish(self):
        '''
        Method to display all the widgets 
        '''
        self.frame_ctrl.grid(row=0, column=0, rowspan=5,
                        columnspan=2, padx=5, pady=5)
        
        self.window1.geometry("600x300")
        self.kp_label.grid(column=0, row=0, padx = 8, pady=9)
        self.kp_entry.grid(column=1, row=0, pady=8, padx= 15)

        self.ki_label.grid(column=0, row=1, padx = 8, pady=9)
        self.ki_entry.grid(column=1, row=1, pady=8, padx= 15)

        self.kd_label.grid(column=0, row=2, padx = 8, pady=9)
        self.kd_entry.grid(column=1, row=2, pady=8, padx= 15)

        self.setpoint_label.grid(column=0, row=3, padx =8, pady=9)
        self.setpoint_entry.grid(column=1, row=3, pady=8, padx=15)

        self.send_btn.grid(column=1, row=4, padx=10, pady=8)

        self.TankPlot()
    
    def send_data_to_arduino(self):
        self.num_kp = self.kp_entry.get()
        self.num_ki = self.ki_entry.get()
        self.num_kd = self.kd_entry.get()
        self.num_setpoint = self.setpoint_entry.get()

        dataTosend = "P,{},{},{},{},0".format(self.num_kp, self.num_ki, self.num_kd, self.num_setpoint)
        self.serial.SerialWriteEncode(dataTosend)

        print("Data", dataTosend, "terkirim")

    def TankPlot(self):
        self.AddMasterFrame1()
        self.AddMasterFrame2()
        self.AddMasterFrame3()
        self.AdjustRootFrame()

        self.AddGraph1()
        self.AddGraph2()
        self.AddGraph3()
        
        self.update_thread = threading.Thread(target=self.update_data, daemon=True)
        self.update_thread.start()

    def AddMasterFrame1(self):
        self.frames.append(LabelFrame(self.window1, text="Tank 1", padx=5, bg="white"))

        self.frames[0].grid(
            padx=10, column=0, row=9, columnspan=9, sticky=NW)
        
    def AddMasterFrame2(self):
        self.frames.append(LabelFrame(self.window1, text="Tank 2", padx=5, bg="white"))
        
        self.frames[1].grid(
            padx=5, column=7, row=9, columnspan=9, sticky=NW)
        
    def AddMasterFrame3(self):
        self.frames.append(LabelFrame(self.window1, text="Input Process", padx=5, bg="white"))
        
        self.frames[2].grid(
            padx=5, column=8, row=0, columnspan=5, sticky=NW, pady=4)

    def AdjustRootFrame(self):
        self.window1.geometry("1100x653")

    def AddGraph1(self):
        self.fig1 = plt.Figure(figsize=(6.5, 3.5), dpi=80)
        self.ax1 = self.fig1.add_subplot(111)
        self.canvas1 = FigureCanvasTkAgg(self.fig1, self.frames[0])
        self.canvas1.get_tk_widget().grid(column=7, row=0, rowspan=17, columnspan=4, sticky=N, pady=7)

        self.ani1 = animation.FuncAnimation(self.fig1, self.animate1, interval=500, cache_frame_data=False)
        
    def AddGraph2(self):
        self.fig2 = plt.Figure(figsize=(6.5, 3.5), dpi=80)
        self.ax2 = self.fig2.add_subplot(111)
        self.canvas2 = FigureCanvasTkAgg(self.fig2, self.frames[1])
        self.canvas2.get_tk_widget().grid(column=11, row=0, rowspan=17, columnspan=4, sticky=N, pady=7)

        self.ani2 = animation.FuncAnimation(self.fig2, self.animate2, interval=500, cache_frame_data=False)

    def AddGraph3(self):
        self.fig3 = plt.Figure(figsize=(6.5, 3.5), dpi=80)
        self.ax3 = self.fig3.add_subplot(111)
        self.canvas3 = FigureCanvasTkAgg(self.fig3, self.frames[2])
        self.canvas3.get_tk_widget().grid(column=1, row=0, rowspan=10, columnspan=4, sticky=N, pady=7)

        self.ani3 = animation.FuncAnimation(self.fig3, self.animate3, interval=500, cache_frame_data=False)

    def animate1(self, i):
        self.ax1.clear()
        self.ax1.set_title("Tank 1 - Water Level", fontsize=10)
        self.ax1.set_ylabel("cm")
        self.ax1.set_xlabel("Time (s)")
        self.ax1.xaxis.label.set_size(8)
        self.ax1.yaxis.label.set_size(8)

        self.UpdataXdata()  
        self.SetRefTime()
        self.AdjustData()

        x = self.XData[:len(self.dataList_Tank1)]
        y = self.dataList_Tank1

        if len(x) == 0 or len(y) == 0:
            return

        x_time = [round(val, 2) for val in x]
        self.ax1.plot(x_time, y, color="green",dash_capstyle='projecting', linewidth=1)

        # Set rentang pada sumbu y
        self.ax1.set_ylim(0, 40)
        self.ax1.set_yticks(range(0, 41, 5))
        self.ax1.set_yticklabels([str(i) for i in range(0, 41, 5)])

        self.ax1.relim()
        self.ax1.autoscale_view()

    def animate2(self, i):
        self.ax2.clear()
        self.ax2.set_title("Tank 2 - Water Level", fontsize=10)
        self.ax2.set_ylabel("cm")
        self.ax2.set_xlabel("Time (s)") 
        self.ax2.xaxis.label.set_size(8)
        self.ax2.yaxis.label.set_size(8)

        self.UpdataXdata()  
        self.SetRefTime()
        self.AdjustData()

        x = self.XData[:len(self.dataList_Tank2)]
        y = self.dataList_Tank2

        if len(x) == 0 or len(y) == 0:
            return

        x_time = [round(val, 2) for val in x]
        self.ax2.plot(x_time, y, color="green",dash_capstyle='projecting', linewidth=1)

        # Set rentang pada sumbu y
        self.ax2.set_ylim(0, 30)
        self.ax2.set_yticks(range(0, 31, 5))
        self.ax2.set_yticklabels([str(i) for i in range(0, 31, 5)])

        self.ax2.relim()
        self.ax2.autoscale_view()
    
    def animate3(self, i):
        self.ax3.clear()
        self.ax3.set_title("Inflow Rate", fontsize=10)
        self.ax3.set_ylabel("L/minutes")
        self.ax3.set_xlabel("Time (s)")
        self.ax3.xaxis.label.set_size(8)
        self.ax3.yaxis.label.set_size(8)

        self.UpdataXdata()  
        self.SetRefTime()
        self.AdjustData()

        x = self.XData[:len(self.dataList_Inflow)]
        y = self.dataList_Inflow

        if len(x) == 0 or len(y) == 0:
            return

        x_time = [round(val, 2) for val in x]
        self.ax3.plot(x_time, y, color="blue",dash_capstyle='projecting', linewidth=1)

        # Set rentang pada sumbu y
        self.ax3.set_ylim(0, 20)
        self.ax3.set_yticks(range(0, 21, 5))
        self.ax3.set_yticklabels([str(i) for i in range(0, 21, 5)])

        self.ax3.relim()
        self.ax3.autoscale_view()
    
    def SetRefTime(self):
        if len(self.XData) == 0:
            self.RefTime = time.time()
        else:
            self.RefTime = time.time() - self.XData[-1]

    def UpdataXdata(self):
        if len(self.XData) == 0:
            self.XData.append(0)
        else:
            self.XData.append(time.time() - self.RefTime)

    def AdjustData(self):
        lenXdata = len(self.XData)
        lenInflowData = len(self.dataList_Inflow)
        lenTank1Data = len(self.dataList_Tank1)
        lenTank2Data = len(self.dataList_Tank2)
        if lenXdata > 50 and lenInflowData > 50 and lenTank1Data > 50 and lenTank2Data > 50:
            del self.XData[0]
            del self.dataList_Inflow[0]
            del self.dataList_Tank1[0]
            del self.dataList_Tank2[0]


class Window_Manual():
    def __init__(self, root, serial):
        '''
        Initialize variables
        '''
        self.root = root
        self.serial = serial

        self.inletData = DoubleVar()
        self.Tank1Data = DoubleVar()                 
        self.Tank2Data = DoubleVar()                      
        self.valveData = DoubleVar()                 

        self.filename = ''
        self.frames = []
        self.dataList_Inflow = []
        self.dataList_Tank1 = []
        self.dataList_Tank2 = []

        self.XData = []
        self.DisplayTimeRange = 5.0
        self.RefTime = 0.0

        self.save = False
        self.data_queue = queue.Queue()  # Inisialisasi antrian (queue)
        self.data_to_csv_thread = None

    def Master_Manual(self):
        self.Masterframe_Manual()
        self.Connframe()
        self.Connframe_publish()

    def Masterframe_Manual(self):
        self.window2 = Toplevel(self.root, bg="white")
        self.window2.title("Manual - Open Loop")
        self.Parentframe()
    
    def Parentframe(self):
        self.parent_frame = Frame(self.window2, padx=15, pady=15, bg="white", borderwidth=2, relief="groove")
        self.parent_frame.grid(row=0, column=0, pady=10, padx=10)
        self.Super_Parentframe()

    def Super_Parentframe(self):
        self.super_frame1 = Frame(self.parent_frame, bg="white")
        self.super_frame1.grid(row=0, column=0, rowspan=3, columnspan=2)

    def Connframe(self):
        self.frame_conn = LabelFrame(self.super_frame1, text="Main Manager",
                                padx=5, pady=5, bg="white", width=60)
        
        self.pump_label = Label(self.frame_conn, text="Pump Toggle: ",
                                bg="white", width=15, anchor="w")
        
        self.btn_pumpctrl = Button(self.frame_conn, text="Start", state="active",
                                width=9, command=self.button_clicked)

        self.save = False
        self.SaveVar = IntVar()
        self.save_check = Checkbutton(self.frame_conn, text="Save to CSV", anchor="w", variable=self.SaveVar,
                                      onvalue=1, offvalue=0, bg="white", state="active",
                                      command=self.save_data)
    
    def Connframe_publish(self):
        '''
        Method to display all the widgets 
        '''
        self.padx = 6
        self.pady = 12

        self.window2.geometry("285x190")
        self.frame_conn.grid(row=0, column=0, rowspan=3,
                        columnspan=2, padx=5, pady=5)

        self.pump_label.grid(column=0, row=0, pady=15, padx=self.padx)

        self.btn_pumpctrl.grid(column=1, row=0, padx=self.padx)

        self.save_check.grid(column=0, row=1, pady=self.pady)

    def button_clicked(self):
        if self.btn_pumpctrl["text"] in 'Start':
            # start the pump, go to start_pump() function
            self.start_pump()
            self.btn_pumpctrl['text'] = 'Stop'

        elif self.btn_pumpctrl["text"] in  'Stop':
            # stop the pump, go to stop_pump() function
            self.stop_pump()
            self.btn_pumpctrl['text'] = 'Start'

    def start_pump(self):
        self.serial.SerialWrite(b'1') # Send '1' to the Arduino to turn on the relay
        InfoMsg = "Pump activated (ON)"
        messagebox.showinfo("showinfo", InfoMsg)

        self.Displayframe()

    def stop_pump(self):
        self.serial.SerialWrite(b'0') # Send '0' to the Arduino to turn off the relay
        WarningMsg = "Pump deactivated (OFF)"
        messagebox.showwarning("showwarning", WarningMsg)

    def save_data(self):
        self.save = not self.save  # Membalik nilai self.save

        if self.save:
            self.set_filename()
            self.StreamDataCheck()
            self.start_data_to_csv_thread()
        else:
            # Menghentikan thread data_to_csv jika sedang berjalan
            if self.data_to_csv_thread is not None and self.data_to_csv_thread.is_alive():
                self.data_queue.put(None)  # Mengirim sinyal None ke antrian untuk menghentikan thread
                self.data_to_csv_thread.join()  # Menunggu thread selesai

    def start_data_to_csv_thread(self):
        if self.data_to_csv_thread is None or not self.data_to_csv_thread.is_alive():
            self.data_to_csv_thread = threading.Thread(target=self.data_to_csv, daemon=True)
            self.data_to_csv_thread.start()

    def StreamDataCheck(self):
        if self.update_thread:
            self.FloatListData = list(zip(
                self.XData,
                self.dataList_Inflow,
                self.dataList_Tank1,
                self.dataList_Tank2
            ))

    def set_filename(self):
        now = datetime.now()
        self.filename = now.strftime("%Y_%m_%d_%H-%M-%S") + ".csv"
    
    def data_to_csv(self):
        with open(self.filename, 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(['Time (s)', 'Inflow', 'Tank1', 'Tank2', 'Valve Position'])

        while self.save:
            try:
                data = self.data_queue.get(timeout=1)  # Mengambil data dari antrian dengan waktu tunggu maksimum 1 detik
                with open(self.filename, 'a', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow(data)
                self.data_queue.task_done()  # Menandai bahwa data telah selesai diproses
            except queue.Empty:
                pass

    def Displayframe(self):
        self.frame_dis = LabelFrame(self.super_frame1, text="Display Manager", 
                                padx=5, bg="white", width=60)
        
        self.inlet_label = Label(
            self.frame_dis, text="Inlet Flow (L/min):", bg="white", width=20, anchor="w")
        self.inletData_label = Label(
            self.frame_dis, text="0.00", bg="white", fg="green", width=5, textvariable=self.inletData)
        
        self.Tank1_label = Label(
            self.frame_dis, text="Water Level Tank 1 (cm):", bg="white", width=20, anchor="w")
        self.Tank1Data_label = Label(
            self.frame_dis, text="0.00", bg="white", fg="green", width=5, textvariable=self.Tank1Data)
        
        self.Tank2_label = Label(
            self.frame_dis, text="Water Level Tank 2 (cm):", bg="white", width=20, anchor="w")
        self.Tank2Data_label = Label(
            self.frame_dis, text="0.00", bg="white", fg="green", width=5, textvariable=self.Tank2Data)
        
        self.Displayframe_publish()

    def Displayframe_publish(self):
        '''
        Method to display all the widgets 
        '''
        self.frame_dis.grid(row=0, column=3, rowspan=3,
                        columnspan=2, padx=5, pady=5)
        
        #self.window1.geometry("300x300")
        self.inlet_label.grid(column=0, row=0, padx = 5)
        self.inletData_label.grid(column=1, row=0, pady=7, padx =5, sticky="w")
        
        self.Tank1_label.grid(column=0, row=1, padx = 5)
        self.Tank1Data_label.grid(column=1, row=1, pady=8, padx =5, sticky="w")

        self.Tank2_label.grid(column=0, row=2, padx = 5)
        self.Tank2Data_label.grid(column=1, row=2, pady=8, padx =5, sticky="w")

        self.Motorframe()

    def Motorframe(self):
        self.frame_motor = LabelFrame(self.parent_frame, text="Motor Control",
                                padx=5, pady=5, bg="white", width=60)
        
        self.frame_m1 = Frame(self.frame_motor, bg="white")
        
        self.label_valve_sensor = Label(self.frame_m1, text="Actual Position: ",
                                bg="white", width=20, anchor="w")
        self.data_valve_sensor = Label(self.frame_m1, text="0", 
                                bg="white", width=5, textvariable=self.valveData)
        
        self.status_value = Label(self.frame_m1, text="Move to: ",
                                bg="white", width=10, anchor="w")
        self.label_status_value = Label(self.frame_m1, text="0 step", width=10)


        self.slider_valve = Scale(self.frame_motor, from_=0, to=5200, orient="horizontal", 
                                length=425, sliderlength=20, showvalue=0, tickinterval=800, resolution=800, command=self.update_label)
        self.slider_valve.bind("<ButtonRelease>", self.slider_released)

        
        self.send_data_thread = None
        self.Motorframe_publish()
    
    def Motorframe_publish(self):
        '''
        Method to display all the widgets 
        '''
        self.window2.geometry("308x600")
        self.frame_motor.grid(row=4, column=0, rowspan=6, columnspan=3, padx=5, pady=5)

        self.frame_m1.grid(row=0, column=0, rowspan=2 , columnspan=2, pady=5, padx=5, sticky="w")

        self.label_valve_sensor.grid(row=0, column=0, sticky="w")
        self.data_valve_sensor.grid(row=0, column=1, padx=10)

        self.status_value.grid(row=1, column=0, pady=5, sticky="w")
        self.label_status_value.grid(row=1, column=1, padx=5, pady=5)

        self.slider_valve.grid(row=3, column=0, padx=5, pady=10, sticky="w")

        self.TankPlot()

    def update_data(self):
        while True:

            inletData_float = 0.0
            Tank1Data_float = 0.0
            Tank2Data_float = 0.0
            valveData_float = 0.0

            Data_string = None
            Data_string = self.serial.SerialReadline()

            if Data_string is not None:
                time.sleep(0.5)
                print("Data_string:", Data_string)

                # Split data into FOUR variables
                data_list = Data_string.split(',', maxsplit=3)
                print("data_list:", data_list)
                if len(data_list) == 4:
                    Tank1Data_split, Tank2Data_split, inletData_split, valveData_split = data_list
                    try:

                        inletData_float = float(inletData_split)
                        Tank1Data_float = float(Tank1Data_split)
                        Tank2Data_float = float(Tank2Data_split)
                        valveData_float = float(valveData_split)

                        self.inletData.set(inletData_float)
                        self.Tank1Data.set(Tank1Data_float)
                        self.Tank2Data.set(Tank2Data_float)
                        self.valveData.set(valveData_float)

                        self.dataList_Inflow.append(inletData_float)
                        #self.dataList_Tank1.append(Tank1Data_float)
                        #self.dataList_Tank2.append(Tank2Data_float)

                        # Update the valve actual value text on motor control frame
                        self.data_valve_sensor.config(text="{:.2f}".format(self.valveData.get()))

                        self.UpdataXdata()  
                        self.SetRefTime()
                        self.AdjustData()

                        # Mengirim data ke antrian
                        self.data_queue.put([round(self.XData[-1], 1), inletData_float, Tank1Data_float, Tank2Data_float, valveData_float])

                    except ValueError:
                        pass

            else:
                print("Error: Data_string is None or empty")

    def update_label(self, value):
        self.value = value
        self.label_status_value.config(text="{} Step".format(self.value))

    def slider_released(self, event):
        self.send_data_to_arduino()

    def send_data_to_arduino(self):
        self.num_valve = self.slider_valve.get()

        dataTosend = "P,0,0,0,0,{}".format(self.num_valve)
        self.serial.SerialWriteEncode(dataTosend)

        print("Data", dataTosend, "terkirim")

    def TankPlot(self):
        self.AddMasterFrame1()
        self.AdjustRootFrame()

        self.update_thread = threading.Thread(target=self.update_data, daemon=True)
        self.update_thread.start()

        self.AddGraph1()
    
    def AddMasterFrame1(self):
        self.frames.append(LabelFrame(self.window2, text="Input Process", bg="white"))

        self.frames[0].grid(
            padx=5, column=4, row=0, columnspan=9, pady=2, sticky=NW)

    def AdjustRootFrame(self):
        self.window2.geometry("1055x355")

    def AddGraph1(self):
        self.fig3 = plt.Figure(figsize=(6.5, 3.9), dpi=80)
        self.ax3 = self.fig3.add_subplot(111)
        self.canvas3 = FigureCanvasTkAgg(self.fig3, self.frames[0])
        self.canvas3.get_tk_widget().grid(column=0, row=0, rowspan=10, columnspan=4, sticky=N, pady=7)

        self.ani3 = animation.FuncAnimation(self.fig3, self.animate3, interval=500, cache_frame_data=False)

    def animate3(self, i):
        self.ax3.clear()
        self.ax3.set_title("Inlflow Rate", fontsize=10)
        self.ax3.set_ylabel("L/minutes")
        self.ax3.set_xlabel("Time (s)")
        self.ax3.xaxis.label.set_size(8)
        self.ax3.yaxis.label.set_size(8)

        self.UpdataXdata()  
        self.SetRefTime()
        self.AdjustData()

        x = self.XData[:len(self.dataList_Inflow)]
        y = self.dataList_Inflow

        if len(x) == 0 or len(y) == 0:
            return

        x_time = [round(val, 2) for val in x]
        self.ax3.plot(x_time, y, color="blue",dash_capstyle='projecting', linewidth=1)

        # Set rentang pada sumbu y
        self.ax3.set_ylim(0, 20)
        self.ax3.set_yticks(range(0, 21, 5))
        self.ax3.set_yticklabels([str(i) for i in range(0, 21, 5)])

        self.ax3.relim()
        self.ax3.autoscale_view()

    def SetRefTime(self):
        if len(self.XData) == 0:
            self.RefTime = time.time()
        else:
            self.RefTime = time.time() - self.XData[-1]

    def UpdataXdata(self):
        if len(self.XData) == 0:
            self.XData.append(0)
        else:
            self.XData.append(time.time() - self.RefTime)

    def AdjustData(self):
        lenXdata = len(self.XData)
        lenInflowData = len(self.dataList_Inflow)
        if lenXdata > 50 and lenInflowData > 50:
            del self.XData[0]
            del self.dataList_Inflow[0]


if __name__ == "__main__":
    RootGUI()
    ComGUI()
    CtrlMode()
    Window_Auto()
    Window_Manual()


        