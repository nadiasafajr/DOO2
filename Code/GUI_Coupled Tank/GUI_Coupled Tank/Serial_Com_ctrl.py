import serial.tools.list_ports  # pip install pyserial
# Secure the UART serial communication with MCU


class SerialCtrl():
    def __init__(self):
        '''
        Initializing the main varialbles for the serial data
        '''
        pass

    def getCOMList(self):
        '''
        Method that get the lost of available coms in the system
        '''
        ports = serial.tools.list_ports.comports()
        self.com_list = [com[0] for com in ports]
        self.com_list.insert(0, "-")

    def SerialOpen(self, ComGUI):
        '''
        Method to setup the serial connection and make sure to go for the next only 
        if the connection is done properly
        '''

        try:
            self.ser.is_open
        except:
            PORT = ComGUI.clicked_com.get()
            BAUD = "9600"                   # sesuai dengan baudrate di Arduino IDE
            self.ser = serial.Serial()
            self.ser.baudrate = BAUD
            self.ser.port = PORT
            self.ser.timeout = 0.1          # waktu tunggu maksimum selama 0.1 detik 

        try:
            if self.ser.is_open:
                print("Already Open")
                self.ser.status = True
            else:
                PORT = ComGUI.clicked_com.get()
                BAUD = "9600"
                self.ser = serial.Serial()
                self.ser.baudrate = BAUD
                self.ser.port = PORT
                self.ser.timeout = 0.01
                self.ser.open()
                self.ser.status = True
        except:
            self.ser.status = False

    def SerialClose(self, ComGUI):
        '''
        Method used to close the UART communication
        '''
        try:
            self.ser.is_open
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False
    
    def SerialWrite(self, data):
        '''
        Method used to write data to the Arduino
        '''
        try:
            self.ser.write(data)
        except Exception as e:
            print("Error writing data to Arduino:", e)

    def SerialWriteEncode(self, data):
        '''
        Method used to write data to the Arduino
        '''
        try:
            data = data.encode('ascii')
            self.ser.write(data)
            #data = self.ser.write().encode()
            self.ser.reset_output_buffer()
            return data
        
        except Exception as e:
            print("Error writing data to Arduino:", e)
    
    def SerialReadline(self):
        '''
        Method used to read data from the Arduino
        '''
        try:
            # read data from serial port
            data = self.ser.readline().decode().strip()
            # convert bytes to string 
            #data = data.decode('ascii').strip()
            self.ser.reset_input_buffer()
            return data
            
        except Exception as e:
            print("Error reading data from Arduino:", e)
            return None
        
    def flushInput(self):
        self.ser.flushInput()
