TrackMyFunds is a personal finance management system written in **C**, implemented in two versions:
- **Terminal (CLI) version**
- **GTK-based GUI version**

The project focuses on modular design, clean architecture, and file-based persistence.

---

## Project Structure

TrackMyFunds/
├── cli/ # Terminal version
├── gtk/ # GTK GUI version
├── shared/ # Shared business logic
├── README.md
└── .gitignore


---

## 🖥️ CLI Version (Terminal)

### Features
- User login & registration  
- Income and expense tracking  
- Category-based reports  
- Persistent file storage  

### Build & Run

gcc cli/main.c shared/*.c -I shared/include -o trackmyfunds
./trackmyfunds

🪟 GTK Version (GUI)
GTK-based desktop application built using GTK+ 3 and Cairo.

Features
-Graphical transaction management
-Balance summary
-Visual income vs expense charts

Build & Run (MSYS2 – MINGW64)

cd gtk
make
./trackmyfunds_gtk
Note: The GTK version must be run from MSYS2 (MINGW64) or Linux, not Windows CMD.
