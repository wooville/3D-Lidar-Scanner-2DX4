import serial
import xlsxwriter

#Modify the following line with your own serial port details
#   Currently set COM3 as serial port at 115.2kbps 8N1
#   Refer to PySerial API for other options.  One option to consider is
#   the "timeout" - allowing the program to proceed if after a defined
#   timeout period.  The default = 0, which means wait forever.
s = serial.Serial("COM3", 115200)
print("Opening: " + s.name)

# create excel workbook, add worksheet
workbook = xlsxwriter.Workbook("dataset.xlsx")
workbook = workbook.add_worksheet()

# format excel sheet
workbook.write(0, 0, "Motor Position")
# headers
for col in range(1, 21):
    workbook.write(0, col, "x(m)=")
    workbook.write(1, col, -0.2*(col-1))
# rotation step
for row in range(2, 514):
    workbook.write(row, 0, row-2)

for col in range(1, 21):
    for row in range(2, 514):
        dist = ""
        while True: # will continue until a full measurement has been read (i.e. will hang in between push button presses)
            x = s.read()        # read one byte
            c = x.decode()      # convert byte type to str

            if c == 'E':    #E is the end bit sent at the end of every measurement
                break

            dist += c   #read each digit of the measurement one at a time

        workbook.write(row, col, dist)

print("Closing: " + s.name)
s.close()
