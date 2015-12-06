all: compile flash

compile:
	particle compile photon ./photon --saveTo app.bin

usbflash:
	sudo particle flash --usb app.bin

flash:
	particle flash hcin720 app.bin

monitor:
	sudo particle serial monitor

run:
	node IA2/server.js
