from tkinter import *
import bluetooth
import serial
import schedule
import time
import socket

P = ["",""]
decoData = ""

clientMACAddress = "98:D3:31:FB:53:F2"
port = 1
portH = 8
size = 1024
s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((clientMACAddress,port))

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

	try:
		while True:
			data = s.recv(size)
			if len(data) != 0: break
		#print("received [%s]" % data)
		if data != (b'\n') and data != (b'\n0'):
			global decoData 
			decoData = data.decode("utf-8")
	except:
		print("error")

def sendData():
	print(decoData)

schedule.every(0.05).seconds.do(valoresLectura)
schedule.every(5).seconds.do(sendData)

while True:
	schedule.run_pending()
	ventana.update()

s.close()
