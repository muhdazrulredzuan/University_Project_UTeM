#MUHAMMAD AZRUL BIN REDZUAN                 B122310626

import time
from tkinter import *
from tkinter import messagebox

# List of bus routes
bus_routes = {
    "SATRIA > KT": {
        "Monday-Thursday": ["7:15 AM - 8:00 AM", "9:30 AM", "1:00 PM", "2:00 PM"],
        "Friday": ["7:15 AM - 8:30 AM", "9:30 AM", "2:30 PM"]
    },
    "LESTARI > KT": {
        "Monday-Thursday": ["7:15 AM - 8:00 AM", "9:30 AM", "1:00 PM", "2:00 PM"],
        "Friday": ["7:15 AM - 8:30 AM", "9:30 AM", "2:30 PM"]
    },
    "AL-JAZARI > KT": {
        "Monday-Thursday": ["7:15 AM - 8:00 AM", "9:30 AM", "1:00 PM", "2:00 PM"],
        "Friday": ["7:15 AM - 8:30 AM", "9:30 AM", "2:30 PM"]
    },
    "KT > SATRIA": {
        "Monday-Thursday": ["11:30 AM", "12:30 PM", "1:30 PM", "4:30 PM", "5:30 PM", "6:30 PM"],
        "Friday": ["11:30 AM", "12:30 PM", "5:30 PM", "6:30 PM"]
    },
    "KT > LESTARI": {
        "Monday-Thursday": ["11:30 AM", "12:30 PM", "1:30 PM", "4:30 PM", "5:30 PM", "6:30 PM"],
        "Friday": ["11:30 AM", "12:30 PM", "5:30 PM", "6:30 PM"]
    },
    "KT > AL-JAZARI": {
        "Monday-Thursday": ["11:30 AM", "12:30 PM", "1:30 PM", "4:30 PM", "5:30 PM", "6:30 PM"],
        "Friday": ["11:30 AM", "12:30 PM", "5:30 PM", "6:30 PM"]
    }
}

# Function to check the timetable of a selected route
def check_timetable():
    route = route_var.get()
    day = day_var.get()
    timetable = bus_routes.get(route, {}).get(day)
    if timetable:
        result_label.config(text=f"Timetable for {route} ({day}):\n" + "\n".join(timetable), fg="white")
    else:
        result_label.config(text="Route or day not found.", fg="white")
        messagebox.showwarning("Not Found", "Route or day not found. Please select a valid route and day.")

# Function to list all routes and their timetables
def list_routes():
    all_routes = "\n".join(
        f"Route: {route}\n" + "\n".join(f"  {day}:\n    " + "\n    ".join(times) for day, times in timetable.items())
        for route, timetable in bus_routes.items()
    )
    display_info(all_routes)

# Function to display information in a new window
def display_info(info):
    info_window = Toplevel(root)
    info_window.title("Information")
    info_window.geometry("400x400")
    info_window.configure(bg="lightblue")
    text = Text(info_window, wrap=WORD, bg="lightyellow", font=("Arial", 12))
    text.pack(expand=1, fill=BOTH)
    text.insert(END, info)

# Function to update the clock
def update_clock():
    current_time = time.strftime("%H:%M:%S")
    clock_label.config(text=current_time)
    root.after(1000, update_clock)

# Function to find the next bus based on current time
def find_next_bus():
    route = route_var.get()
    day = day_var.get()
    current_time = time.strftime("%I:%M %p")
    timetable = bus_routes.get(route, {}).get(day)

    if timetable:
        next_buses = [t for t in timetable if time.strptime(t.split(' - ')[-1], "%I:%M %p") > time.strptime(current_time, "%I:%M %p")]
        if next_buses:
            result_label.config(text=f"Next bus for {route} ({day}): {next_buses[0]}", fg="white")
        else:
            result_label.config(text="No more buses today.", fg="white")
    else:
        result_label.config(text="Route or day not found.", fg="white")
        messagebox.showwarning("Not Found", "Route or day not found. Please select a valid route and day.")

# Create the main window
root = Tk()
root.title("UTeM Bus Route and Timetable Checker")
root.geometry("500x600")
root.configure(bg="cyan")

# Title label
Label(root, text="UTeM Bus Route and Timetable Checker", font=("Arial", 16, "bold"), bg="white").pack(pady=10)

# Real-time clock
clock_label = Label(root, font=("Arial", 12), bg="white")
clock_label.pack(pady=10)
update_clock()

# Frame for input fields
input_frame = Frame(root, bg="blue")
input_frame.pack(pady=20)

# Input for bus route and day
route_var = StringVar()
Label(input_frame, text="Select Bus Route:", font=("Arial", 12), bg="white").grid(row=0, column=0, padx=10, pady=5)
OptionMenu(input_frame, route_var, *bus_routes.keys()).grid(row=0, column=1, padx=10, pady=5)
day_var = StringVar()
Label(input_frame, text="Select Day:", font=("Arial", 12), bg="white").grid(row=1, column=0, padx=10, pady=5)
OptionMenu(input_frame, day_var, "Monday-Thursday", "Friday").grid(row=1, column=1, padx=10, pady=5)

# Buttons to check timetable, find next bus, and list all routes
Button(root, text="Check Timetable", command=check_timetable, bg="blue", fg="white", font=("Arial", 12)).pack(pady=10)
Button(root, text="Find Next Bus", command=find_next_bus, bg="blue", fg="white", font=("Arial", 12)).pack(pady=10)
Button(root, text="List All Routes", command=list_routes, bg="blue", fg="white", font=("Arial", 12)).pack(pady=10)

# Label to display the result of the search
result_label = Label(root, text="", font=("Arial", 12), bg="blue", wraplength=400, justify=LEFT)
result_label.pack(pady=20)

# Explanation text
explanation_text = (
    "Explanation:\n"
    "\nKT = Kampus Teknologi\t\t  Lestari = Kolej kediaman Lestari\n"
    "AJ = Kolej Kediaman Aj-jazari\t  Satria = Kolej kediaman Satria\n"
)
Label(root, text=explanation_text, bg="white", fg="black", justify="left", wraplength=380).pack(pady=5)

# Start the GUI event loop
root.mainloop()
