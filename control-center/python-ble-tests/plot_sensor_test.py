import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser

parser = ArgumentParser(
    prog='Plot test',
    description='Plot data from sensor test',
)
parser.add_argument('--filename',
    type=str, default='test.npy',
    help='Filename of sensor data'
)
test_name = parser.parse_args().filename
with open(test_name, 'rb') as f:
    data = np.load(f)

MAX_VAL = 2**15
ACCEL_RANGE = MAX_VAL/16
GYRO_RANGE = MAX_VAL/2000
MAGN_RANGE = MAX_VAL/4900

# Extract and convert values
timestamp = (data[:, 0] - data[0, 0])/1000
accel_x = data[:, 1]/ACCEL_RANGE
accel_y = data[:, 2]/ACCEL_RANGE
accel_z = data[:, 3]/ACCEL_RANGE
gyro_x = data[:, 4]/GYRO_RANGE
gyro_y = data[:, 5]/GYRO_RANGE
gyro_z = data[:, 6]/GYRO_RANGE
mag_x = data[:, 7]/MAGN_RANGE
mag_y = data[:, 8]/MAGN_RANGE
mag_z = data[:, 9]/MAGN_RANGE
temp = 21 + (data[:, 10] - 21)/334

# Print some statistics about measurements
sample_f = int(round(len(data)/max(timestamp), 0))
print(f'{test_name} contains {len(data)} samples. They were sampled at a ' + \
      f'sample rate of ca {sample_f} Hz and were measured for ' + \
      f'{max(timestamp)} seconds.')

# Plot values
_, (ax_acc, ax_gyr, ax_mag, ax_temp) = plt.subplots(4, 1)
ax_acc.plot(timestamp, accel_x, label='X accel')
ax_acc.plot(timestamp, accel_y, label='Y accel')
ax_acc.plot(timestamp, accel_z, label='Z accel')
ax_gyr.plot(timestamp, gyro_x, label='X gyro')
ax_gyr.plot(timestamp, gyro_y, label='Y gyro')
ax_gyr.plot(timestamp, gyro_z, label='Z gyro')
ax_mag.plot(timestamp, mag_x, label='X mag')
ax_mag.plot(timestamp, mag_y, label='Y mag')
ax_mag.plot(timestamp, mag_z, label='Z mag')
ax_temp.plot(timestamp, temp, label='Temperature')
ax_acc.set_xlabel('Time [s]')
ax_acc.set_ylabel('Acceleration [g]')
ax_gyr.set_xlabel('Time [s]')
ax_gyr.set_ylabel('Rotation [dps]')
ax_mag.set_xlabel('Time [s]')
ax_mag.set_ylabel(r'Magnetix flux density [$\mu T$]')
ax_temp.set_xlabel('Time [s]')
ax_temp.set_ylabel(r'Temperature [$\degree C$]')
ax_acc.legend()
ax_gyr.legend()
ax_mag.legend()
ax_temp.legend()
ax_acc.grid()
ax_gyr.grid()
ax_mag.grid()
ax_temp.grid()
plt.show()
