"""
Control panel for killer robot
Made by Simen
"""
from os import system
from sys import platform
import threading
import tkinter as tk
from tkinter import ttk
import atexit
from time import sleep

from communication import \
    list_comports, open_serial, init_serial, Communication

# global sets for keeping an eye in the keys (little ugly solution, i know ;( )
pressed_keys = set()

# Global object for sending messages
comm = Communication()

class windows(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        # Adding a title to the window
        self.wm_title("Control panel")
 
        # creating a frame and assigning it to container
        container = tk.Frame(self)
        # specifying the region where the frame is packed in root
        container.pack(side="top", fill="both", expand=True)
 
        # configuring the location of the container using grid
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}
        for i, F in enumerate((ControlOptions, Controls, DeviceOptions, BluetoothOptions)):
            frame = F(container, self)
            frame.grid(row=i, column=0)
            self.frames[frame.name] = frame

class Controls(tk.Frame):
    def __init__(self, parent, _):
        self.name = "Controls"
        tk.Frame.__init__(self, parent) 
        label = tk.Label(self, text="Controls")

        # Buttons that can be used controlling the robot.
        # These buttons does not do anything, but indicates which keys are pressed.
        Controls.buttons = dict()
        label.grid(row=0,column=1)
        control_buttons = { 'w': (1, 2)
                          , 'a': (2, 1)
                          , 's': (2, 2)
                          , 'd': (2, 3)
                          , 'shift': (3, 0)
                          , 'alt':   (3, 1)
                          , 'space': (3, 2)
                          }
        for key, (i, j) in control_buttons.items():
            new_button = tk.Button(
                self, 
                text = key,
            )
            new_button.grid(row=i, column=j)
            Controls.buttons[key] = new_button


class ControlOptions(tk.Frame):
    def __init__(self, parent, _):
        self.name = "ControlOptions"
        tk.Frame.__init__(self, parent) 
        label = tk.Label(self, text="Control Options")
        label.grid(row=0, column=1)

        # create entries for the options
        self.weapon_entry = create_option_entry(
            self, 'Weapon speed', (1, 0), self.weapon_speed
        )
        self.forward_entry = create_option_entry(
            self, 'Forward speed', (2, 0), self.forward_speed
        )
        self.backward_entry = create_option_entry(
            self, 'Backward speed', (3, 0), self.backward_speed
        )
        self.turnrate_entry = create_option_entry(
            self, 'Turning rate', (4, 0), self.turn_fraction
        )
        # setting alternate variables
        self.set_shift_values = create_option_check(
            self, (4, 0), 0, 
            text = 'Set shift values', onvalue = 1, offvalue = 0,
            command = self.set_shift_entries
        )
        # update the variables
        update_button = tk.Button(
            self, 
            text = 'Update values', command = self.update_values
        )
        update_button.grid(row=5, column=1)

    def update_values(self):
        # unfocus entries
        control_panel.focus()

        # set weapon speed
        weapon_var = get_speed(self.weapon_entry)
        if weapon_var is None:
            print('Weapon entry needs number')
            return

        # set forward speed
        forward_var = get_speed(self.forward_entry)
        if forward_var is None:
            print('Forward entry needs number')
            return
        
        # set bacwkard speed
        backward_var = get_speed(self.backward_entry)
        if backward_var is None:
            print('Backward entry needs number')
            return

        # set turning rate
        turnrate_str = self.turnrate_entry.get()
        # this will cause ValueError if `turnrate_str` can not be translated to a float
        try:
            min_turnrate = 0.1; max_turnrate = 2
            limited_value = limit_value(min_turnrate, max_turnrate, float(turnrate_str))
            turn_var = limited_value
        except ValueError:
            print('Turn fraction needs float')
            return

        if self.set_shift_values.get() > 0: # setting alternate speed
            print("Setting alternate speed")
            ControlOptions.weapon_speed_alt = weapon_var
            ControlOptions.forward_speed_alt = forward_var
            ControlOptions.backward_speed_alt = backward_var
            ControlOptions.turn_fraction_alt = turn_var
        else:                               # setting normal speed
            print("Setting normal speed")
            ControlOptions.weapon_speed = weapon_var
            ControlOptions.forward_speed = forward_var 
            ControlOptions.backward_speed = backward_var
            ControlOptions.turn_fraction = turn_var

    def set_shift_entries(self):
        # Clear all entries
        self.weapon_entry.delete(0, 4)
        self.forward_entry.delete(0, 4)
        self.backward_entry.delete(0, 4)
        self.turnrate_entry.delete(0, 4)

        if self.set_shift_values.get() > 0:
            self.weapon_entry.insert(0, str(ControlOptions.weapon_speed_alt))
            self.forward_entry.insert(0, str(ControlOptions.forward_speed_alt))
            self.backward_entry.insert(0, str(ControlOptions.backward_speed_alt))
            self.turnrate_entry.insert(0, str(ControlOptions.turn_fraction_alt))

        else:
            self.weapon_entry.insert(0, str(ControlOptions.weapon_speed))
            self.forward_entry.insert(0, str(ControlOptions.forward_speed))
            self.backward_entry.insert(0, str(ControlOptions.backward_speed))
            self.turnrate_entry.insert(0, str(ControlOptions.turn_fraction))

class DeviceOptions(tk.Frame):
    def __init__(self, parent, _):
        self.name = "DeviceOptions"
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Device Options")
        label.grid(row=0, column = 1)
        # Update the serial device list every 5 seconds
        threading.Thread(target=self.update_port_list, args=(5,), daemon=True).start()
        self.ser = init_serial()

        # Settings for transmitter device
        self.port_combo = create_option_combo(
            self, 'Port', (1, 0), []
        )
        self.baud_entry = create_option_entry(
            self, 'Baud rate', (2, 0), 115200)
        self.open_button = tk.Button(
            self, text = 'Open serial', command = self.open_serial
        )
        self.open_button.grid(row=3, column = 1)

    def open_serial(self):
        # Close port if it is already open
        if self.ser.is_open:
            comm.remove_interface(self.ser)
            self.ser.close()
            print('Closed serial')
            self.open_button.config(text='Open serial')
            return
        
        # unfocus entries
        control_panel.focus()

        # Open serial
        baudrate = self.baud_entry.get()
        if baudrate.isdigit():
            if open_serial(self.ser, self.port_combo.get(), int(baudrate)):
                comm.add_interface(self.ser, 1, None)
                # if the serial opened succesfully, the button will now close the serial
                self.open_button.config(text=f'Close serial')
                # print all serial communication to terminal
                threading.Thread(target=comm.read_serial_thread, args=(self.ser,), daemon=True).start()

    def update_port_list(self, interval):
        while True:
            self.device_list = list_comports()
            self.port_combo['values'] = self.device_list
            sleep(interval)

class BluetoothOptions(tk.Frame):
    def __init__(self, parent, _):
        self.name = "BluetoothOptions"
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Bluetooth options")
        label.grid(row=0, column = 1)
        self.pings = []

        # Settings for ble device
        self.device_entry = create_option_entry(
            self, 'BLE device name', (1, 0), 'rupert'
        )
        self.open_button = tk.Button(
            self, text = 'Connect', command = self.bt_connect
        )
        self.open_button.grid(row=3, column = 1)
        self.ping_var = tk.StringVar()
        tk.Label(
            self, textvariable=self.ping_var, 
        ).grid(row=3, column=0)

    def bt_disconnect_cb(self, client):
        comm.remove_interface(client)
        self.ping_var.set('')
        print('Disconnected')
        try:
            self.open_button.config(text='Connect')
        except tk.TclError:
            print('GUI already closed')
            

    def bt_connect_cb(self):
        comm.add_interface(comm.ble_client, 0, self.send_cb)
        self.open_button.config(text='Disconnect')
        print('Connected')

    def bt_connect(self):
        # Unfocus entries after pressing connect button
        control_panel.focus()

        # Disconnect if already connected
        if comm.ble_client:
            if comm.ble_client.is_connected:
                print('Try to disconnect')
                threading.Thread(
                    target=comm.ble_disconnect,
                    daemon=True
                ).start()
                return

        name = self.device_entry.get()
        print(name)
        threading.Thread(
            target=comm.ble_connect,
            args=(name, self.bt_connect_cb, self.bt_disconnect_cb),
            daemon=True
        ).start()
    
    def send_cb(self, time):
        # Calculate ping based on last average 10 times
        self.pings.append(time)
        if len(self.pings) > 10:
            self.pings.pop(0)
        ping = sum(self.pings)/len(self.pings)
        # Set ping at control panel
        self.ping_var.set(f'Ping: {str(int(ping*1000)).rjust(3)} ms')


# limit the values so they do not exceed max-values
def limit_value(minimum, maximum, value):
    if value <= minimum:
        return minimum   

    elif value >= maximum:
        return maximum

    return value

def get_speed(entry, min_val=-100, max_val=100):
    # set weapon speed
    entry_str = entry.get()
    if entry_str.isdigit():
        return limit_value(min_val, max_val, int(entry_str))
    else:
        return None

def key_press(event):
    """ called when key is pressed """
    pressed_keys.add(event.keycode)
    process_keys()

def key_release(event):
    """ called when key is released """
    pressed_keys.remove(event.keycode)
    process_keys()

def process_keys():
    """ called every time a key  is pressed or released """

    # different key numbers for different platforms (here is only linux and windows)
    if platform == 'linux':
        key_links = { 25: 'w'
                    , 38: 'a'
                    , 39: 's'
                    , 40: 'd'
                    ,  9: 'esc'
                    , 50: 'shift' 
                    , 65: 'space'
                    , 36: 'enter'
                    , 64: 'alt'
                    }
    else : 
        key_links = { 87: 'w'
                    , 65: 'a'
                    , 83: 's'
                    , 68: 'd'
                    , 27: 'esc'
                    , 16: 'shift' 
                    , 32: 'space'
                    , 36: 'enter' # windows value?
                    , 64: 'alt' # windows value?
                    }
    # if there is no change since last time keys were pressed
    if pressed_keys == process_keys.last_pressed_keys:
        return
    process_keys.last_pressed_keys = pressed_keys.copy()
    # filter out uninteresting keys
    relevant_keys = set(filter(lambda x: x in key_links, pressed_keys))
    # get key names from key numbers
    keys = list(map(lambda x: key_links[x], relevant_keys))
    # make buttons on control panel look like they are pressed
    ghostpress_buttons(keys)

    if 'esc' in keys: # unfocus stuff
        control_panel.focus()

    # Calculate motor speeds
    motor_a, motor_b, motor_w = calculate_speed(keys)
    
    # send the message :)
    comm.send_message(motor_a, motor_b, motor_w)

# Static variable for keeping tab on prevous pressed keys
process_keys.last_pressed_keys = set()

def ghostpress_buttons(keys):
    # make the keys look like they are pressed
    for key, button in Controls.buttons.items():
        if key in keys:
            button.config(relief='sunken') # pressed key
        else:
            button.config(relief='raised') # non pressed key

def calculate_speed(keys):
    if 'alt' in keys: # alternate speed
        weapon_speed   = ControlOptions.weapon_speed_alt
        forward_speed  = ControlOptions.forward_speed_alt
        backward_speed = ControlOptions.backward_speed_alt
        turn_fraction  = ControlOptions.turn_fraction_alt
    else:               # normal speed
        weapon_speed   = ControlOptions.weapon_speed
        forward_speed  = ControlOptions.forward_speed
        backward_speed = ControlOptions.backward_speed
        turn_fraction  = ControlOptions.turn_fraction

    motor_a = 0
    motor_b = 0
    motor_w = 0

    if 'w' in keys: # forward
        motor_a += forward_speed
        motor_b += forward_speed
    if 's' in keys: # backward
        motor_a -= backward_speed
        motor_b -= backward_speed
    if 'd' in keys: # right
        if motor_a != 0:
            motor_b -= int(turn_fraction*motor_a)
        else:
            motor_a += int(turn_fraction*forward_speed)
            motor_b -= int(turn_fraction*forward_speed)
    if 'a' in keys: # left
        if motor_a != 0:
                motor_a -= int(turn_fraction*motor_a)
        else: 
            motor_a -= int(turn_fraction*forward_speed)
            motor_b += int(turn_fraction*forward_speed)

    if 'space' in keys: # weapon on
        motor_w = weapon_speed

    if 'shift' in keys: # go reverse!
        # invert speed
        motor_w = -1*motor_w
        motor_a = -1*motor_a
        motor_b = -1*motor_b

    
    return limit_motor(motor_a+127), limit_motor(motor_b+127), limit_motor(motor_w+127)

def limit_motor(value):
    # limit the motor value
    if     value <=   0: return   0
    elif   value >= 255: return 255
    return value

def create_option_combo(root, label, pos, start_value):
    """
    Template for comboboxes
    """
    row, column = pos
    label = tk.Label(root, text=f'{label}:')
    label.grid(row=row, column=column)
    combo = ttk.Combobox(root, textvariable='Test')
    combo['values'] = start_value
    combo.grid(row=row, column=column+1)
    return combo

def create_option_entry(root, label, pos, start_value):
    """
    Template for option entries
    """
    row, column = pos
    label = tk.Label(root, text=f'{label}:')
    label.grid(row=row, column=column)
    entry = tk.Entry(root)
    entry.insert(0, str(start_value))
    entry.grid(row=row, column=column+1)
    return entry

def create_option_check(root, pos, start_value, **kwargs):
    """
    Template for checkbutton
    Add Checkbutton settings in keyword arguments (**kwargs)
    """
    checkbutton_variable = tk.IntVar()
    checkbutton_variable.set(start_value)
    transmitter_checkbutton = tk.Checkbutton(root, variable=checkbutton_variable, **kwargs)
    row, column = pos
    transmitter_checkbutton.grid(row=row,column=column)
    return checkbutton_variable

def xset_on():
    system('xset r on')

if __name__ == "__main__":
    # turn off key repeating, global os configuration (on linux)
    if platform == 'linux':
        # Turn xset off for linux machines, and turn it on again on exit
        system('xset r off')
        atexit.register(xset_on)


    # Variables for controlling the different speed values
    ControlOptions.weapon_speed = 20
    ControlOptions.forward_speed  = 100
    ControlOptions.backward_speed = 100
    ControlOptions.turn_fraction  = 1
    ControlOptions.weapon_speed_alt = 10
    ControlOptions.forward_speed_alt  = 75
    ControlOptions.backward_speed_alt = 75
    ControlOptions.turn_fraction_alt  = 0.5

    # tkinter setup
    control_panel = windows()
    control_panel.bind("<KeyPress>",   key_press)
    control_panel.bind("<KeyRelease>", key_release)
    # Run tkinter in a thread
    control_panel.mainloop()
