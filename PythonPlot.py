import serial
from tkinter import *
import time
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import numpy as np
import matplotlib.pyplot as plt

s= serial.Serial('COM6',9600)

dataArray = np.array([])
dataTime = np.array([])
x=0
cond = False

root = Tk()
root.title('Plot obstacle data')
root.configure(background = 'gray')
root.geometry("1000x520")

def auto():
    global cond
    if (cond == True):
        print("LED is Y")
        time.sleep(0.1) 
        s.write(b'Y')
        txt()
        plot()
    root.after(1,auto)

def forward():
    print("LED is F")
    time.sleep(0.1)
    s.write(b'F')
    
def left():
    print("LED is L")
    time.sleep(0.1)
    s.write(b'L')
    
def right():
    print("LED is R")
    time.sleep(0.1)
    s.write(b'R')

def backword():
    print("LED is B")
    time.sleep(0.1)
    s.write(b'B')
    
def stop():
    global cond
    print("LED is S")
    time.sleep(0.1)
    s.write(b'S')
    cond = False
    
def txt():
    global dataArray, dataTime, x
    x=x+1
    msg = s.read()
    print(msg)
    dec = msg.decode()
    print(dec)
    if dec == '1':
        print('done')
        file=open("car.txt", "a")
        file.write('1,')
        file.close()
    elif dec == '0':
        print('not done')
        file=open("car.txt", "a")
        file.write('0,')
        file.close()
    if dec == '1' or dec == '0':
        dataArray = np.append(dataArray, dec)
        print(dataArray)
        dataTime = np.append(dataTime, x)
        print(dataTime)

def plot():
    global dataArray, dataTime
    print(dataArray)
    figure = Figure(figsize=(4,4),dpi=70)

    a=figure.add_subplot(111)
    a.plot(dataTime,dataArray,marker='o')
    a.grid()

    a.set_title('Serial Data');
    a.set_xlabel('Time')
    a.set_ylabel('Detection')

    canvas = FigureCanvasTkAgg(figure, master=root)
    canvas.draw()

    graph_widget=canvas.get_tk_widget()
    graph_widget.place(x = 400,y=100, width = 500,height = 400)
def start():
    global cond
    cond = True

btn_forward = Button(root, text = "^",width = 15, command =forward)
btn_forward.place(x = 260,y=100, width = 50,height = 40)
btn_left = Button(root, text = "<" ,width = 15, command = left)
btn_left.place(x = 210,y=150, width = 50,height = 40)
btn_backward = Button(root, text = "_", width = 15, command = backword)
btn_backward.place(x = 260,y=200, width = 50,height = 40)
btn_right = Button(root, text = ">", width = 15, command = right)
btn_right.place(x = 310,y=150, width = 50,height = 40)
btn_stop = Button(root, text = "Stop", width = 15, command = stop)
btn_stop.place(x = 125,y=200, width = 50,height = 40)

btn_start = Button(root, text = "auto", width = 15, command = start)
btn_start.place(x = 125,y=100, width = 50,height = 40)


root.after(1,auto)
root.mainloop()

