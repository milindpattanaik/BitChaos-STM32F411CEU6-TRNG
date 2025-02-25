from flask import Flask, render_template, jsonify,request
import serial
import time

app = Flask(__name__)

# Flask route to serve the main page
@app.route('/')
def home():
    return render_template('index.html')  # returns the index.html file whenever the app is loaded or refreshed

# flask route to get the random number
@app.route('/generate', methods=['POST'])
def get_random_number():
    
    if request.method == 'POST':
        try:
            # 'COM9' is the serial port through which the stm32 is sending serial data to the laptop
            ser = serial.Serial('COM9', 115200, timeout=1)
            time.sleep(3)
            ser.write(b'G') # send G to stm32 to generate the random bit
            time.sleep(3)
            random_number = ser.readline().decode().strip()  # reading and parsing of the data being recieved by the microcontroller
            ser.close()
            return jsonify({"random_number": random_number})
        except Exception as e:
            return jsonify({"error": str(e)})
    else :
        return jsonify({"error": "Invalid request method"})

if __name__ == '__main__':
    app.run(debug=True)
