import tkinter as tk
from tkinter import ttk
 
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
        label.pack()

        control_buttons = []
        for letter in "qweasd":
            upper_letter = letter.upper()
            new_button = tk.Button(
                self, 
                text = upper_letter,
            command = lambda: print(upper_letter)
            )
            new_button.pack()
            control_buttons.append(new_button)

class ControlOptions(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Control Options")
        label.pack()

class DeviceOptions(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Device Options")
        label.pack()

if __name__ == "__main__":
    testObj = windows()
    testObj.mainloop()
