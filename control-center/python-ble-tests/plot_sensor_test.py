import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser

def read_sensor_data(test_name):
    with open(test_name, 'rb') as f:
        data = np.load(f)

    MAX_VAL = 2**15
    ACCEL_RANGE = MAX_VAL/16
    GYRO_RANGE = MAX_VAL/2000
    MAGN_RANGE = MAX_VAL/4900

    # Extract and convert values
    time = (data[:, 0] - data[0, 0])/1000
    accel = data[:, 1:4]/ACCEL_RANGE
    gyro = data[:, 4:7]/GYRO_RANGE
    mag = data[:, 7:10]/MAGN_RANGE
    temp = 21 + (data[:, 10] - 21)/334
    return time, accel, gyro, mag, temp

def plot_data(time, accel, gyro, mag):
    # Create plot
    _, axs = plt.subplots(3, 1)
    ax_acc, ax_gyr, ax_mag = axs
    # Plot accel, gyro and magnetometer data
    senses = (accel, gyro, mag)
    for i, sensor in enumerate(('accel', 'gyro', 'mag')):
        ax = axs[i]
        for j, dir in enumerate('XYZ'):
            ax.plot(time, senses[i][:, j], label=f'{dir} {sensor}')

    # Plot settings
    ax_acc.set_ylabel('Acceleration [g]')
    ax_gyr.set_ylabel('Rotation [dps]')
    ax_mag.set_ylabel(r'Magnetix flux density [$\mu T$]')
    for ax in axs:
        ax.set_xlabel('Time [s]')
        ax.legend()
        ax.grid()

    plt.show()

def main():
    parser = ArgumentParser(
    prog='Plot test',
    description='Plot data from sensor test',
    )
    parser.add_argument('--filename',
        type=str, default='test.npy',
        help='Filename of sensor data'
    )
    test_name = parser.parse_args().filename
    time, accel, gyro, mag, temp = read_sensor_data(test_name)

    # Print some statistics about measurements
    sample_f = int(round(len(time)/max(time), 0))
    print(f'{test_name} contains {len(time)} samples. They were sampled at a ' + \
        f'sample rate of ca {sample_f} Hz and were measured for ' + \
        f'{max(time)} seconds.')
    
    plot_data(time, accel, gyro, mag)

if __name__ == '__main__':
    main()