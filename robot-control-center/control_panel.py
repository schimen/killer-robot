from os import system
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
        for key, (i, j) in {'w': (1, 1), 'a': (2, 0), 's': (2, 1), 'd': (2, 2)}.items():
            new_button = tk.Button(
                self, 
                text = key,
            )
            new_button.grid(row=i, column=j)
            Controls.buttons[key] = new_button

        tk.Label(self, text='Last message:').grid(row=3, column=0)
        Controls.last_message = tk.StringVar()
        tk.Label(self, textvariable=Controls.last_message).grid(row=3, column=1)


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
            ControlOptions.forward_speed = limited_value
            print(f'Set forward speed to {ControlOptions.forward_speed}')
        
        backward_str = self.backward_entry.get()
        if backward_str.isdigit():
            max_backward = 127; min_backward = 0
            limited_value = limit_value(min_backward, max_backward, int(backward_str))
            ControlOptions.backward_speed = limited_value
            print(f'Set bacward speed to {ControlOptions.backward_speed}')

        turnrate_str = self.turnrate_entry.get()
        try:
            min_turnrate = 0.1; max_turnrate = 1;
            limited_value = limit_value(min_turnrate, max_turnrate, float(turnrate_str))
            ControlOptions.turn_fraction = limited_value
            print(f'Set turn fraction to {ControlOptions.turn_fraction}')
        except ValueError:
            return

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
        baud_label.grid(row=2, column=0)
        self.baud_entry.grid(row=2, column=1)

        open_button = tk.Button(
            self,
            text = 'Open',
            command = self.open_serial
        )
        open_button.grid(row=3, column = 1)
    
    def open_serial(self):
        control_panel.focus()
        ser.port = self.port_entry.get()
        baudrate = self.baud_entry.get()
        if baudrate.isdigit():
            ser.baudrate = int(baudrate)
        
        print(f'Opening serial at:\nport: {ser.port}, baud: {ser.baudrate}')
        ser.open()

def key_press(event):
    pressed_keys.add(event.keycode)
    process_keys()

def key_release(event):
    pressed_keys.remove(event.keycode)
    process_keys()

def process_keys():
    key_links = { 25: 'w'
                , 38: 'a'
                , 39: 's'
                , 40: 'd'
                ,  9: 'esc'
                , 36: 'enter'
                , 37: 'ctrl'
                , 50: 'shift'
                , 64: 'alt'
                , 65: 'space' }
    relevant_keys = set(filter(lambda x: x in key_links, pressed_keys))
    keys = list(map(lambda x: key_links[x], relevant_keys))
    ghostpress_buttons(keys)
    if 'esc' in keys:
        control_panel.focus()
    
    motor_a, motor_b = calculate_speed(keys)
    send_message(motor_a, motor_b)

def ghostpress_buttons(keys):
    for key, button in Controls.buttons.items():
        if key in keys:
            button.config(relief='sunken')
        else:
            button.config(relief='raised')

def calculate_speed(keys):
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
    # turn off key repeating, global os configuration
    system('xset r off')
    # global sets for keeping an eye in the keys (little ugly solution, i know ;( )
    pressed_keys = set()
    # create serial object
    ser = Serial()

    ControlOptions.forward_speed = 127
    ControlOptions.backward_speed = ControlOptions.forward_speed//2
    ControlOptions.turn_fraction = 0.4

    # tkinter setup
    control_panel = windows()
    control_panel.bind("<KeyPress>",   key_press)
    control_panel.bind("<KeyRelease>", key_release)
    control_panel.mainloop()
    # turn on key repeating again
    system('xset r on')
    # close serial
    ser.close()
