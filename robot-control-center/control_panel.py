from os import system
from sys import platform
import tkinter as tk
from serial import Serial

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

        Controls.buttons = dict()
        label.grid(row=0,column=1)
        for key, (i, j) in {'w': (1, 2), 'a': (2, 1), 's': (2, 2), 'd': (2, 3), 'shift': (3, 0)}.items():
            new_button = tk.Button(
                self, 
                text = key,
            )
            new_button.grid(row=i, column=j)
            Controls.buttons[key] = new_button

        tk.Label(self, text='Last message:').grid(row=4, column=0)
        Controls.last_message = tk.StringVar()
        tk.Label(self, textvariable=Controls.last_message).grid(row=4, column=1)


class ControlOptions(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent) 
        label = tk.Label(self, text="Control Options")
        label.grid(row=0, column=1)

        forward_label = tk.Label(self, text='Forward speed:')
        forward_label.grid(row=1, column=0)
        self.forward_entry = tk.Entry(self)
        self.forward_entry.insert(0, str(self.forward_speed))
        self.forward_entry.grid(row=1, column=1)

        backward_label = tk.Label(self, text='Backward speed:')
        backward_label.grid(row=2, column=0)
        self.backward_entry = tk.Entry(self)
        self.backward_entry.insert(0, str(self.backward_speed))
        self.backward_entry.grid(row=2, column=1)

        turnrate_label = tk.Label(self, text='Turning rate:')
        turnrate_label.grid(row=3, column=0)
        self.turnrate_entry = tk.Entry(self)
        self.turnrate_entry.insert(0, str(self.turn_fraction))
        self.turnrate_entry.grid(row=3, column=1)
        
        self.set_shift_values = tk.IntVar()
        self.set_shift_values.set(0)
        set_shift = tk.Checkbutton(
            self, 
            text = 'Set shift values',
            onvalue = 1, offvalue = 0,
            variable = self.set_shift_values
        )
        set_shift.grid(row=4, column=0)

        update_button = tk.Button(
            self,
            text='Update values',
            command=self.update_values
        )
        update_button.grid(row=4, column=1)

    def update_values(self):
        def limit_value(minimum, maximum, value):
            if value <= minimum:
                return minimum    
            elif value >= maximum:
                return maximum
            return value

        control_panel.focus()
        forward_str = self.forward_entry.get()
        if forward_str.isdigit():
            max_forward = 127; min_forward = 0
            limited_value = limit_value(min_forward, max_forward, int(forward_str))
            forward_var = limited_value
            print(f'Forward speed: {forward_var}')
        
        backward_str = self.backward_entry.get()
        if backward_str.isdigit():
            max_backward = 127; min_backward = 0
            limited_value = limit_value(min_backward, max_backward, int(backward_str))
            backward_var = limited_value
            print(f'Bacward speed: {backward_var}')

        turnrate_str = self.turnrate_entry.get()
        try:
            min_turnrate = 0.1; max_turnrate = 1
            limited_value = limit_value(min_turnrate, max_turnrate, float(turnrate_str))
            turn_var = limited_value
            print(f'Turn fraction: {turn_var}')
        except ValueError:
            return

        if self.set_shift_values.get() > 0: # setting alternate speed:
            print("Setting alternate speed")
            ControlOptions.forward_speed_alt = forward_var
            ControlOptions.backward_speed_alt = backward_var
            ControlOptions.turn_fraction_alt = turn_var
        else:
            print("Setting normal speed")
            ControlOptions.forward_speed = forward_var 
            ControlOptions.backward_speed = backward_var
            ControlOptions.turn_fraction = turn_var

class DeviceOptions(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Device Options")
        label.grid(row=0, column = 1)
        port_label = tk.Label(self, text='Port:')
        self.port_entry = tk.Entry(self)
        port_label.grid(row=1, column=0)
        self.port_entry.grid(row=1, column=1)

        baud_label = tk.Label(self, text='Baud rate:')
        self.baud_entry = tk.Entry(self)
        self.baud_entry.insert(0, '9600')
        baud_label.grid(row=2, column=0)
        self.baud_entry.grid(row=2, column=1)

        open_button = tk.Button(
            self,
            text = 'Open',
            command = self.open_serial
        )
        open_button.grid(row=3, column = 1)

        self.radio_choice = tk.IntVar()
        self.radio_choice.set(1)
        transmitter_checkbutton = tk.Checkbutton(
            self,
            text = 'Use backup transmitter',
            variable = self.radio_choice,
            onvalue = 0, offvalue = 1,
            command = self.choose_transmitter
        )
        transmitter_checkbutton.grid(row=4,column=1)
    
    def open_serial(self):
        control_panel.focus()
        ser.port = self.port_entry.get()
        baudrate = self.baud_entry.get()
        if baudrate.isdigit():
            ser.baudrate = int(baudrate)
        
        print(f'Opening serial at:\nport: {ser.port}, baud: {ser.baudrate}')
        ser.open()

    def choose_transmitter(self):
        radio_choice_command = 9
        value = self.radio_choice.get()
        message = f'{radio_choice_command},{value}\n'
        if ser.is_open:
            ser.write(bytes(message, 'utf-8'))
        else:
            print(message, end='')

def key_press(event):
    pressed_keys.add(event.keycode)
    process_keys()

def key_release(event):
    pressed_keys.remove(event.keycode)
    process_keys()

def process_keys():
    if platform == 'linux':
        key_links = { 25: 'w'
                    , 38: 'a'
                    , 39: 's'
                    , 40: 'd'
                    ,  9: 'esc'
                    , 50: 'shift' }
    else : 
        key_links = { 87: 'w'
                    , 65: 'a'
                    , 83: 's'
                    , 68: 'd'
                    , 27: 'esc'
                    , 16: 'shift' }
    
    if pressed_keys == process_keys.last_pressed_keys:
        return
    
    process_keys.last_pressed_keys = pressed_keys.copy()
    relevant_keys = set(filter(lambda x: x in key_links, pressed_keys))
    keys = list(map(lambda x: key_links[x], relevant_keys))
    ghostpress_buttons(keys)
    if 'esc' in keys:
        control_panel.focus()
    
    motor_a, motor_b = calculate_speed(keys)
    send_message(motor_a, motor_b)

process_keys.last_pressed_keys = set()

def ghostpress_buttons(keys):
    for key, button in Controls.buttons.items():
        if key in keys:
            button.config(relief='sunken')
        else:
            button.config(relief='raised')

def calculate_speed(keys):
    if 'shift' in keys:
        forward_speed  = ControlOptions.forward_speed_alt
        backward_speed = ControlOptions.backward_speed_alt
        turn_fraction  = ControlOptions.turn_fraction_alt
    else:
        forward_speed  = ControlOptions.forward_speed
        backward_speed = ControlOptions.backward_speed
        turn_fraction  = ControlOptions.turn_fraction

    motor_a = 0
    motor_b = 0
    if 'w' in keys:
        motor_a += forward_speed
        motor_b += forward_speed
    if 's' in keys:
        motor_a -= backward_speed
        motor_b -= backward_speed
    if 'd' in keys: #turn right
        if motor_a != 0:
            motor_b -= int(turn_fraction*motor_a)
        else:
            motor_a += int(turn_fraction*forward_speed)
            motor_b -= int(turn_fraction*forward_speed)
    if 'a' in keys: # turn left
        if motor_a != 0:
                motor_a -= int(turn_fraction*motor_a)
        else:
            motor_a -= int(turn_fraction*forward_speed)
            motor_b += int(turn_fraction*forward_speed)

    def limit_motor(value):
        min_motor = -127; max_motor = 127
        if value <= min_motor:
            return min_motor
        elif value >= max_motor:
            return max_motor
        return value

    return limit_motor(motor_a), limit_motor(motor_b)

def send_message(motor_a, motor_b):
    command_a = 10; command_b = 11
    message_a = f'{command_a},{motor_a}\n'
    message_b = f'{command_b},{motor_b}\n'
    transmit_message = ''

    duplicate_message_a = send_message.old_message_a == message_a
    duplicate_message_b = send_message.old_message_b == message_b
    if duplicate_message_a and duplicate_message_b:
        return
    
    if not duplicate_message_a:
        transmit_message += message_a
        send_message.old_message_a = message_a

    if not duplicate_message_b:
        transmit_message += message_b
        send_message.old_message_b = message_b

    if ser.is_open:
        ser.write(bytes(transmit_message, 'utf-8'))
    else:
        print(transmit_message, end='')

    Controls.last_message.set(transmit_message)

send_message.old_message_a = ''
send_message.old_message_b = ''

if __name__ == "__main__":
    # turn off key repeating, global os configuration (on linux)
    if platform == 'linux':
        system('xset r off')

    # global sets for keeping an eye in the keys (little ugly solution, i know ;( )
    pressed_keys      = set()
    # create serial object
    ser = Serial()

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
