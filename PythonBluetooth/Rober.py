from tkinter import *
import bluetooth
import serial
import schedule
import time

serEnable = False #habilitar la comunicacion serial
P = ["",""] #array para guardar los valores
puerto = serial.Serial()
#Baudrate igual que el del PIC
puerto.baudrate=9600
puerto.timeout=3#tiempo de espera para recibir un dato cualquiera
#Puerto usado para el serial
puerto.port='COM9'
puerto.open()
print('Puerto activado con exito')

hostMACAddress = "98:D3:31:FB:53:F2"
port = 1
s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((hostMACAddress,port))

def sendAvanzar():
	s.send(bytes('a','UTF-8'))

def sendRetroceder():
	s.send(bytes('b','UTF-8'))

def sendIzquierda():
	s.send(bytes('c','UTF-8'))

def sendDerecha():
	s.send(bytes('d','UTF-8'))

def sendDetener():
	s.send(bytes('e','UTF-8'))

ventana = Tk()
ventana.title("Comunicacion Bluetooth-ROBER")

avanzar = Button(ventana,text = "up",command = sendAvanzar)
retroceder = Button(ventana,text = "down",command = sendRetroceder)
izquierda = Button(ventana,text = "Left", command = sendIzquierda)
derecha = Button(ventana,text = "right", command = sendDerecha)

avanzar.place(x=100,y=10)
izquierda.place(x=50,y=60)
derecha.place(x=150,y=60)
retroceder.place(x=100,y=110)

def valoresLectura():
	with puerto:
		for i in range(2):
			P[i]=puerto.readline().strip()
	print(P[0])
	print(P[1])

schedule.every(5).seconds.do(valoresLectura)

while True:
	schedule.run_pending()
	ventana.update()

s.close()
