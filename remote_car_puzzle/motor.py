
import lgpio

# Define motor output pins
pin_out = [17, 22, 23, 24]
h = lgpio.gpiochip_open(0)

def init():
    pin_out = [17, 22, 23, 24]
    for pin in pin_out:
        lgpio.gpio_claim_output(h, pin)
    print("Motor initialized")

def terminate():
    print("Cleaning up GPIO")
    stop_motor()
    lgpio.gpiochip_close(h)

def forward():
    print("Moving forward")
    lgpio.gpio_write(h, pin_out[0], 0)
    lgpio.gpio_write(h, pin_out[1], 1)
    lgpio.gpio_write(h, pin_out[2], 1)
    lgpio.gpio_write(h, pin_out[3], 0)

def backward():
    print("Moving backward")
    lgpio.gpio_write(h, pin_out[0], 1)
    lgpio.gpio_write(h, pin_out[1], 0)
    lgpio.gpio_write(h, pin_out[2], 0)
    lgpio.gpio_write(h, pin_out[3], 1)

def right_turn():
    print("Right Turn")
    lgpio.gpio_write(h, pin_out[0], 1)
    lgpio.gpio_write(h, pin_out[1], 0)
    lgpio.gpio_write(h, pin_out[2], 1)
    lgpio.gpio_write(h, pin_out[3], 0)

def left_turn():
    print("left_turn")
    lgpio.gpio_write(h, pin_out[0], 0)
    lgpio.gpio_write(h, pin_out[1], 1)
    lgpio.gpio_write(h, pin_out[2], 0)
    lgpio.gpio_write(h, pin_out[3], 1)

def stop_motor():
    print("Stopping the motor")
    for pin in pin_out:
        lgpio.gpio_write(h, pin, 0)

