from flask import Flask, render_template, request, Response
from flask import jsonify
import base64, time, threading
import motor
import mqtt
from display import CameraStreamer

app = Flask(__name__)

camera = CameraStreamer()

@app.route('/', methods=['GET'])
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(camera.generate_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame', direct_passthrough=True)

@app.route('/move', methods=['POST'])
def motor_control():
    data = request.get_json()
    direction = data.get('direction')
    if direction == 'forward':
        motor.forward()
    elif direction == 'backward':
        motor.backward()
    elif direction == 'right':
        motor.right_turn()
    elif direction == 'left':
        motor.left_turn()
    elif direction == 'stop':
        motor.stop_motor()

    return jsonify({'status': 'ok', 'command': direction})

@app.route('/qr_count')
def get_qr_count():
    qr_count = camera.get_qr_count()
    if (qr_count == 3):
        mqtt.publish_finished()
    return jsonify(count=qr_count)

@app.route('/reset_qr_set', methods=['POST'])
def reset_qr_set():
    camera.reset_qr_set()
    return jsonify(status='success', message='QR set reset')

if __name__ == '__main__':
    try:
        motor.init()
        mqtt.start()
        app.run(host='0.0.0.0', port=5000, debug=True, use_reloader=False)
    finally:
        camera.stop()
        motor.terminate()
        mqtt.stop()