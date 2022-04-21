"""
Control panel for killer robot
Made by Simen
"""

from os                      import system
from sys                     import platform
from serial                  import Serial
from serial.serialutil       import SerialException
from serial.tools.list_ports import comports
from queue import Queue
import threading
import tkinter as tk

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
 
        for i, F in enumerate((ControlOptions, Controls, DeviceOptions)):
            frame = F(container, self)
            frame.grid(row=i, column=0)

class Controls(tk.Frame):
    def __init__(self, parent, controller):
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
                          , 'x': (3, 3)
                          , 'shift': (3, 0)
                          , 'space': (3, 4)
                          }
        for key, (i, j) in control_buttons.items():
            new_button = tk.Button(
                self, 
                text = key,
            )
            new_button.grid(row=i, column=j)
            Controls.buttons[key] = new_button

        # show last sent message
        tk.Label(self, text='Last message:').grid(row=4, column=0)
        Controls.last_message = tk.StringVar()
        tk.Label(self, textvariable=Controls.last_message).grid(row=4, column=1)


class ControlOptions(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent) 
        label = tk.Label(self, text="Control Options")
        label.grid(row=0, column=1)

        # create entries for the options
        self.forward_entry = create_option_entry( self
                                                , 'Forward speed'
                                                , (1, 0)
                                                , self.forward_speed
                                                )
        self.backward_entry = create_option_entry( self
                                                , 'Backward speed'
                                                , (2, 0)
                                                , self.backward_speed
                                                )
        self.turnrate_entry = create_option_entry( self
                                                , 'Turning rate'
                                                , (3, 0)
                                                , self.turn_fraction
                                                )
        # setting alternate variables
        self.set_shift_values = create_option_check( self
                                                   , (4, 0)
                                                   , 0
                                                   , text = 'Set shift values'
                                                   , onvalue = 1
                                                   , offvalue = 0
                                                   )
        # update the variables
        update_button = tk.Button( self
                                 , text = 'Update values'
                                 , command = self.update_values
                                 )
        update_button.grid(row=4, column=1)

    def update_values(self):

        # limit the values so they do not exceed max-values
        def limit_value(minimum, maximum, value):
            if value <= minimum:
                return minimum   

            elif value >= maximum:
                return maximum

            return value

        # unfocus entries
        control_panel.focus()

        # set forward speed
        forward_str = self.forward_entry.get()
        if forward_str.isdigit():
            max_forward = 127; min_forward = 0
            limited_value = limit_value(min_forward, max_forward, int(forward_str))
            forward_var = limited_value
            print(f'Forward speed: {forward_var}')
        
        # set bacwkard speed
        backward_str = self.backward_entry.get()
        if backward_str.isdigit():
            max_backward = 127; min_backward = 0
            limited_value = limit_value(min_backward, max_backward, int(backward_str))
            backward_var = limited_value
            print(f'Bacward speed: {backward_var}')

        # set turning rate
        turnrate_str = self.turnrate_entry.get()
        # this will cause ValueError if `turnrate_str` can not be translated to a float
        try:
            min_turnrate = 0.1; max_turnrate = 2
            limited_value = limit_value(min_turnrate, max_turnrate, float(turnrate_str))
            turn_var = limited_value
            print(f'Turn fraction: {turn_var}')
        except ValueError:
            return

        if self.set_shift_values.get() > 0: # setting alternate speed
            print("Setting alternate speed")
            ControlOptions.forward_speed_alt = forward_var
            ControlOptions.backward_speed_alt = backward_var
            ControlOptions.turn_fraction_alt = turn_var
        else:                               # setting normal speed
            print("Setting normal speed")
            ControlOptions.forward_speed = forward_var 
            ControlOptions.backward_speed = backward_var
            ControlOptions.turn_fraction = turn_var

class DeviceOptions(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Device Options")
        label.grid(row=0, column = 1)
        # Settings for transmitter device
        
        self.port_entry = create_option_entry( self
                                             , 'Port'
                                             , (1, 0)
                                             , '/dev/ttyACM0'#list(port.device for port in comports())[0]
                                             )
        self.baud_entry = create_option_entry( self
                                             , 'Baud rate'
                                             , (2, 0)
                                             , 9600
                                             )
        self.open_button = tk.Button( self
                                    , text = 'Open serial'
                                    , command = self.open_serial
                                    )
        self.open_button.grid(row=3, column = 1)

        self.radio_choice = create_option_check( self
                                               , (4, 1)
                                               , 1
                                               , text = 'Use backup transmitter'
                                               , onvalue = 0
                                               , offvalue = 1
                                               , command = self.choose_transmitter
                                               )
    
    def open_serial(self):
        if ser.is_open: # close port if it is already open
            ser.close()
            print('Closed serial')
            self.open_button.config(text='Open serial')
            return
        

        # unfocus entries
        control_panel.focus()

        # set up port
        ser.port = self.port_entry.get()
        baudrate = self.baud_entry.get()
        if baudrate.isdigit():
            ser.baudrate = int(baudrate)
        
        try: # try to open the port
            ser.open()
            print(f'Opening serial at:\nport: {ser.port}, baud: {ser.baudrate}')
            # if the serial opened succesfully, the button will now close the serial
            self.open_button.config(text=f'Close serial')

            # print all serial communication to terminal
            threading.Thread(target=read_serial_thread, args=(ser,), daemon=True).start()

        except SerialException:
            print(f'Could not open serial port at {ser.port} with {ser.baudrate}.')
            print(f'Maybe try one of these: {list(port.device for port in comports())}')

    def choose_transmitter(self):
        # send command to choose between main and backup transmitter
        radio_choice_command = 9
        value = self.radio_choice.get()
        message = f'{radio_choice_command},{value}\n'
        if ser.is_open:
            ser.write(bytes(message, 'utf-8'))
        else:
            print(message, end='')

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
                    }
    else : 
        key_links = { 87: 'w'
                    , 65: 'a'
                    , 83: 's'
                    , 68: 'd'
                    , 27: 'esc'
                    , 16: 'shift' 
                    , 32: 'space'
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

    if 'space' in keys: # go reverse!
        motor_b, motor_a = calculate_speed(keys)
        motor_b = -1*motor_b; motor_a = -1*motor_a

    else: # calculate speed based on keys pressed
        motor_a, motor_b = calculate_speed(keys)

    # send the message :)
    send_message(motor_a, motor_b)

process_keys.last_pressed_keys = set() # static variable for keeping tab on prevous pressed keys

def ghostpress_buttons(keys):
    # make the keys look like they are pressed
    for key, button in Controls.buttons.items():
        if key in keys:
            button.config(relief='sunken') # pressed key
        else:
            button.config(relief='raised') # non pressed key

def calculate_speed(keys):
    if 'shift' in keys: # alternate speed
        forward_speed  = ControlOptions.forward_speed_alt
        backward_speed = ControlOptions.backward_speed_alt
        turn_fraction  = ControlOptions.turn_fraction_alt
    else:               # normal speed
        forward_speed  = ControlOptions.forward_speed
        backward_speed = ControlOptions.backward_speed
        turn_fraction  = ControlOptions.turn_fraction

    motor_a = 0
    motor_b = 0

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

    def limit_motor(value):
        # limit the motor value
        min_motor = -127; max_motor = 127
        if value <= min_motor:
            return min_motor
        elif value >= max_motor:
            return max_motor
        return value

    return limit_motor(motor_a), limit_motor(motor_b)

def get_message_id():
    get_message_id.id += 1
    if get_message_id.id > 31:
        get_message_id.id = 0
    return get_message_id.id
get_message_id.id = 0

def send_message(motor_a, motor_b):
    """ send message to transmitter """
    message_id = get_message_id()
    command = 2
    value = 2

    # motor a
    command_a = 10
    message_a = f'{command_a},{motor_a}\n'
    # motor b
    command_b = 11
    message_b = f'{command_b},{motor_b}\n'
    # string to be transmitted
    transmit_message = ''
    # check if message_a or message_b  are duplicates from earlier
    duplicate_message_a = send_message.old_message_a == message_a
    duplicate_message_b = send_message.old_message_b == message_b
    if duplicate_message_a and duplicate_message_b:
        return
    # add message a
    if not duplicate_message_a:
        transmit_message += message_a
        send_message.old_message_a = message_a
    # add message b
    if not duplicate_message_b:
        transmit_message += message_b
        send_message.old_message_b = message_b
    # send message
    if ser.is_open:
        
        head = command << 5 | message_id
        ser.write(bytearray([ord(':'), head, value, ord(';')]))
    else:
        print(f'command: {command}, id: {message_id}, value: {value}')

    # set the `last_message` label in controls to the new message
    Controls.last_message.set(transmit_message)
    print_new_commands()

# static variables for `send_message`, for keeping track on earlier messages
send_message.old_message_a = ''
send_message.old_message_b = ''

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
    checkbutton_variable.set(1)
    transmitter_checkbutton = tk.Checkbutton(root, variable=checkbutton_variable, **kwargs)
    row, column = pos
    transmitter_checkbutton.grid(row=row,column=column)
    return checkbutton_variable

def read_serial_thread(ser):
    """
    Function that continually reads serial information and prints to terminal
    """
    serial_buffer = b''
    while ser.is_open:
        try:
            # add new characters to buffer
            serial_buffer += ser.read()

        except TypeError:
            # serial was apparently closed
            print('Stopped listening to serial')
            return None

        if b':' not in serial_buffer:
            # reset serial_buffer if there is no command start
            serial_buffer = b''

        # command beginning and end is in buffer
        if b':' in serial_buffer and b';' in serial_buffer:
            while len(serial_buffer) > 0:
                start = serial_buffer.find(b':')
                end = serial_buffer.find(b';') + 1
                if start >= end:
                    print('received partial command, throw it away')
                    serial_buffer = serial_buffer[start:]
                elif end - start > 4:
                    serial_buffer = serial_buffer[start+1:]
                else:
                    command_string = serial_buffer[start:end]
                    if len(command_string) == 4:
                        head = command_string[1]
                        value = command_string[2]

                    command = (head & 0xE0) >> 5
                    message_id = head & 0x1F
                    incoming_commands.put((command, message_id, value))

                    # command finished, reset
                    serial_buffer = serial_buffer[end:]

def print_new_commands():
    if not incoming_commands.empty():
        for i in range(incoming_commands.qsize()):
            print(f'new command: {incoming_commands.get()}')

if __name__ == "__main__":
    # turn off key repeating, global os configuration (on linux)
    if platform == 'linux':
        system('xset r off')

    # global sets for keeping an eye in the keys (little ugly solution, i know ;( )
    pressed_keys = set()

    # create serial object
    ser = Serial()

    # queue for incoming commands
    incoming_commands = Queue()

    # Variables for controlling the different speed values
    ControlOptions.forward_speed  = 127
    ControlOptions.backward_speed = 127
    ControlOptions.turn_fraction  = 1
    ControlOptions.forward_speed_alt  = 127
    ControlOptions.backward_speed_alt = 127
    ControlOptions.turn_fraction_alt  = 0.5

    # tkinter setup
    control_panel = windows()
    control_panel.bind("<KeyPress>",   key_press)
    control_panel.bind("<KeyRelease>", key_release)
    control_panel.mainloop()
    
    # turn on key repeating again (linux)
    if platform == 'linux':
        system('xset r on')
    
    # close serial
    ser.close()
