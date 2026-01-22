import customtkinter as ctk
import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import time
import serial
import threading
import csv
from datetime import datetime
import matplotlib.ticker as ticker

ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

class App(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
        self.thread = threading.Thread(target=self.read_from_uart, daemon=True)
        self.thread.start()

        self.title("Environmental measurement system")
        self.geometry("1000x700")
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self.x_data = []
        self.y_temp = []
        self.y_press = []
        self.y_probetemp = []
        self.start_time = time.time()

        self.view_mode = tk.IntVar(value=0) 

        self.sidebar_frame = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar_frame.grid(row=0, column=0, sticky="nsew")

        self.logo_label = ctk.CTkLabel(self.sidebar_frame, text="Pick view", font=ctk.CTkFont(size=20, weight="bold"))
        self.logo_label.grid(row=0, column=0, padx=20, pady=(20, 10))

        self.radio_temp = ctk.CTkRadioButton(self.sidebar_frame, text="Temperature", variable=self.view_mode, value=0)
        self.radio_temp.grid(row=1, column=0, padx=20, pady=10, sticky="w")

        self.radio_press = ctk.CTkRadioButton(self.sidebar_frame, text="Pressure", variable=self.view_mode, value=1)
        self.radio_press.grid(row=2, column=0, padx=20, pady=10, sticky="w")

        self.radio_press = ctk.CTkRadioButton(self.sidebar_frame, text="Temperature(probe)", variable=self.view_mode, value=2)
        self.radio_press.grid(row=3, column=0, padx=20, pady=10, sticky="w")

        self.btn_save = ctk.CTkButton(self.sidebar_frame, text="Save to CSV", command=self.save_data, fg_color="green")
        self.btn_save.grid(row=5, column=0, padx=20, pady=(40, 10))

        self.btn_clear = ctk.CTkButton(self.sidebar_frame, text="Clear data", command=self.clear_data, fg_color="firebrick")
        self.btn_clear.grid(row=6, column=0, padx=20, pady=10)

        self.btn_exit = ctk.CTkButton(self.sidebar_frame, text="Exit", command=self.destroy, fg_color="transparent", border_width=2)
        self.btn_exit.grid(row=10, column=0, padx=20, pady=20)
        self.sidebar_frame.grid_rowconfigure(9, weight=1) 

        self.main_frame = ctk.CTkFrame(self)
        self.main_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.fig = Figure(figsize=(5, 4), dpi=100, facecolor='#2B2B2B')
        self.ax = self.fig.add_subplot(111)
        self.ax.set_facecolor('#2B2B2B')
        
        self.ax.tick_params(colors='gray')
        for spine in self.ax.spines.values():
            spine.set_color('gray')
        self.ax.grid(True, color='#404040')
        self.ax.set_xlabel("t [s]", color="gray")
        
        y_formatter = ticker.ScalarFormatter(useOffset=False)
        y_formatter.set_scientific(False)
        self.ax.yaxis.set_major_formatter(y_formatter)
        
        self.line, = self.ax.plot([], [], linewidth=2)

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.main_frame)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        self.update_plot()
        self.elapsed = 0;
        self.start_time = datetime.now()

    def clear_data(self):
        self.elapsed = 0
        self.x_data = []
        self.y_temp = []
        self.y_press = []
        self.y_probetemp = []
        self.start_time = time.time()
        self.start_time = datetime.now()

    def save_data(self):
        filename = f"measurement_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

        start_date_str = self.start_time.strftime("%Y-%m-%d %H:%M:%S")

        with open(filename, mode='w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file, delimiter=';')
            writer.writerow(["Start of the measurement:", start_date_str])
            writer.writerow([])
            writer.writerow(["Time [s]", "Temperature [C]", "Pressure [hPa]", "Temperature(probe) [C]"])

            for i in range(len(self.x_data)):
                writer.writerow([
                    self.x_data[i],       
                    self.y_temp[i],
                    self.y_press[i],
                    self.y_probetemp[i]
                ])

    def read_from_uart(self):
        while True:
            if self.ser.in_waiting:
                line = self.ser.readline().decode('utf-8').strip()
                self.x_data.append(self.elapsed)
                self.elapsed+=1
                try:
                    t, p, t2 = line.split(',')
                    
                    self.y_temp.append(float(t))
                    self.y_press.append(float(p))
                    self.y_probetemp.append(float(t2))
                except:
                    pass

    def update_plot(self):
        
        if len(self.x_data) > 200:
            self.x_data.pop(0)
            self.y_temp.pop(0)
            self.y_press.pop(0)
            self.y_probetemp.pop(0)

        mode = self.view_mode.get()

        if mode == 0: 
            self.line.set_data(self.x_data, self.y_temp)
            self.line.set_color('#FF5555') 
            self.ax.set_title("Temperature [°C]", color="white")
            
        elif mode == 1: 
            self.line.set_data(self.x_data, self.y_press)
            self.line.set_color('#5555FF') 
            self.ax.set_title("Pressure [hPa]", color="white")

        elif mode == 2:
            self.line.set_data(self.x_data, self.y_probetemp)
            self.line.set_color('#FF5555') 
            self.ax.set_title("Temperature(probe) [°C]", color="white")

        self.ax.relim()
        self.ax.autoscale_view()
        
        self.canvas.draw()
        self.after(100, self.update_plot)

if __name__ == "__main__":
    app = App()
    app.mainloop()