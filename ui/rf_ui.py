from tkinter import *
from PIL import ImageTk, Image
import os

##########################################################################################
# Verificacion de entradas
def write_touch():
   config_file = open('../routine','a')
   config_file.write("touch\n")
   config_file.close()

def write_move():
  try:
    if (int(e1.get())>=0 and int(e2.get())>=0):
      config_file = open('../routine','a')
      config_file.write("move "+ e1.get() + "," + e2.get() +"\n")
      config_file.close()
      message_err.set("")
    else: 
      message_err.set("Insert positive coordinates...")
  except:
    message_err.set("Insert integer numbers...")

def write_press():
  try:
    if (int(e3.get())>0):
      config_file = open('../routine','a')
      config_file.write("press "+ e3.get() +"\n")
      config_file.close()
      message_err.set("")
    else: 
      message_err.set("Insert positive number...")
  except:
    message_err.set("Insert integer number...")

def write_mp():
  try:
    if (int(e4.get())>=0 and int(e5.get())>=0):
      config_file = open('../routine','a')
      config_file.write("map "+ e4.get() + "," + e5.get() +"\n")
      config_file.close()
      message_err.set("")
    else: 
      message_err.set("Insert positive coordinates..")
  except:
    message_err.set("Insert integer numbers...")

def write_pin():
  try:
    pin = int(e6.get())
    if(pin > 999999 or pin < 100000):
      message_err.set("Insert PIN with 6 digits...")
    else:
      config_file = open('../routine','a')
      config_file.write("pin "+ e6.get() +"\n")
      config_file.close()
      message_err.set("")
  except:
    message_err.set("Insert numerical PIN...")

def init_routine():
  size = 0
  if (opt.get() == "1x1"):
    size = 1
  elif (opt.get() == "2x2"):
    size = 2
  else:
    size = 4
  command = "./../interpreter/roboticFinger -c ../routine -p 0 -s " + str(size)

  # Run robotic finger 
  #os.system(command)
  #Remove file 
  os.remove("../routine")

##########################################################################################

OPTIONS = [
"1x1",
"2x2",
"4x4"
]

master = Tk()
master.title("Robotic Finger")

logo = PhotoImage(file="robot.png")
w1 = Label(master, image=logo).grid(row=4,column=8)

opt = StringVar(master)
opt.set(OPTIONS[0]) # default value
w = OptionMenu(master, opt, *OPTIONS)
w.grid(row=4,column=0)

message_err = StringVar()
displayLab = Label(master, textvariable=message_err)
displayLab.grid(row=4,column=1)

Label(master, text="Move").grid(row=0,column=0)
Label(master, text="Press").grid(row=1,column=0)
Label(master, text="Move & Press").grid(row=2,column=0)
Label(master, text="PIN").grid(row=3,column=0)

e1 = Entry(master) #MOVE
e2 = Entry(master) #MOVE
e3 = Entry(master) #PRESS
e4 = Entry(master) #M&P
e5 = Entry(master) #M&P
e6 = Entry(master) #PIN

e1.grid(row=0, column=1)
e2.grid(row=0, column=2)
e3.grid(row=1, column=1)
e4.grid(row=2, column=1)
e5.grid(row=2, column=2)
e6.grid(row=3, column=1)

##########################################################################################

Button(master, text='Quit', command=master.quit).grid(row=5, column=2, sticky=W, pady=4)
Button(master, text='Start Routine', command=init_routine).grid(row=5, column=0, sticky=W, pady=4)
Button(master, text='Touch', command=write_touch).grid(row=5, column=1, sticky=W, pady=4)


##########################################################################################

Button(master, text='OK', command=write_move).grid(row=0, column=4, sticky=W, pady=4) # MOVE
Button(master, text='OK', command=write_press).grid(row=1, column=4, sticky=W, pady=4) # PRESS
Button(master, text='OK', command=write_mp).grid(row=2, column=4, sticky=W, pady=4) # M&P
Button(master, text='OK', command=write_pin).grid(row=3, column=4, sticky=W, pady=4) # PIN

##########################################################################################

mainloop( )




