#!/usr/bin/env python

# Display Si7021 temperature value on PIC-drive 7-segment LED
# 2017/10/17 Roger Cheng

class PIC_LTC4627JR:
  """ A class to display information on the Lite-On LTC-4627JR 7-segment LED """

  def __init__(self, pi, bus=1, addr=0x42):
    """
    Instantiate with the Pi.

    Optionally the I2C bus may be specified, default 1.

    Optionally the I2C address may be specified, default 0x42.
    """
    self.pi = pi
    self._h = pi.i2c_open(bus, addr)

  def cancel(self):
    """
    Release I2C resources
    """
    self.pi.i2c_close(self._h)

  def displayString(self, value):
    """
    Display the given string.
    """

    # Send the string to the PIC for display.
    self.pi.i2c_write_i2c_block_data(self._h, 0, value)

if __name__ == "__main__":
  import time
  import pigpio
  import Si7021

  pi = pigpio.pi()

  if not pi.connected:
    exit(0)

  # Sensor initialization
  s = Si7021.sensor(pi)

  s.set_resolution(0)
  print("Sensor resolution set to {:x}".format(s.get_resolution()))

  print("Sensor hardware revision={:x} with ID1={:08x} and ID2={:08x}".format(
    s.firmware_revision(),
    s.electronic_id_1(), 
    s.electronic_id_2()))

  # LCD initialization
  led = PIC_LTC4627JR(pi)

  try:
    print("Press Control-C to exit program.")
    while True:
      celsius = s.temperature()
      farenheit = celsius * 1.8 + 32
      farenheitString = "{:4.2f}".format(farenheit)
      try:
        led.displayString(farenheitString)
      except Exception as err:
        if err.value == "I2C write failed":
          print("PIC has reset, waiting to retry.")
        else:
          raise
      time.sleep(0.1)
  except KeyboardInterrupt:
    pass
    
  print ()
  print ("Contro-C pressed, cleaning up and quitting.")

  s.cancel()
  led.cancel()
  pi.stop()

