# pbl-2nd-year-team-pankajbisht44254
OVERPARKING RESOLVER - PHASE 3
Smart Campus Parking Reservation, Redirection and Priority Waitlist System

============================================================
1. SIMPLE PROJECT FOLDER
============================================================

Overparking_Resolver_Phase3/
    HTML/
        index.html       (HTML + CSS + JavaScript in ONE file)
    CPP/
        backend.cpp      (C++ OOP + DSA + local HTTP server)
    build_windows.bat
    run_windows.bat
    README.txt

You do not need separate style.css or app.js files. The browser loads
one frontend page: HTML/index.html. Its CSS is inside a <style> section
and its JavaScript is inside a <script> section.

============================================================
2. HOW THE FRONTEND AND BACKEND WORK TOGETHER
============================================================

1. Start the C++ program. It runs a local web server.
2. Open http://127.0.0.1:8765 in Chrome or Edge.
3. The C++ server sends HTML/index.html to the browser.
4. The JavaScript inside index.html sends form data to /api/... routes.
5. C++ receives the request, runs the parking rules and DSA algorithms,
   and sends JSON back.
6. JavaScript updates the dashboard and tables using that response.

JavaScript only connects the interface to C++ and displays responses.
The parking decisions, reservation logic, graph traversal and emergency
allocation are performed by C++.

============================================================
3. PROJECT FEATURES
============================================================

- Dashboard with parking capacity and block occupancy.
- Vehicle registration for two-wheelers, four-wheelers and emergency
  vehicles, with Student, Faculty and Emergency priority categories.
- Standard reservation, automatic redirection, cancellation, check-in
  and undo.
- Priority waitlist: Emergency, then Faculty, then Student; FIFO within
  each priority.
- Emergency allocation using an adjacency matrix and BFS. C++ chooses
  the nearest reachable block with free space and returns the route.
- Graph page showing adjacency matrix and BFS/DFS traversal calculated
  in C++.
- No-show register using a Binary Search Tree.
- Fines, 80% capacity alerts, occupancy report and activity log.
- Activity log can be saved to a text file.

The emergency AI is an explainable, rule-based allocator. It is not a
trained machine-learning model.

============================================================
4. OOP AND DSA USED
============================================================

OOP: classes and objects, encapsulation, abstraction, inheritance,
runtime polymorphism, virtual functions, constructors, copy constructor,
destructors, operator overloading, function template and exception
handling.

Custom DSA without STL containers: arrays, pointers, dynamic arrays,
stack, linked lists, doubly linked list, linked priority queues, binary
search tree, hash table with chaining, adjacency matrix, BFS, DFS,
insertion sort, quick sort and binary search.

The project demonstrates selected topics from the semester syllabus;
it does not claim to implement every topic in the complete syllabus.

============================================================
5. RUN IN VS CODE ON WINDOWS
============================================================

STEP 1 - Install the compiler once
- Install MSYS2 from https://www.msys2.org/ if it is not already set up.
- Open MSYS2 UCRT64.
- Run:
    pacman -S --needed mingw-w64-ucrt-x86_64-gcc
- Add C:\msys64\ucrt64\bin to Windows PATH.
- Reopen VS Code and check in its terminal:
    g++ --version

STEP 2 - Open the project
- Extract the ZIP.
- Open VS Code -> File -> Open Folder.
- Select the Overparking_Resolver_Phase3 folder.

STEP 3 - Build the C++ backend
- In the VS Code terminal, run:
    .\build_windows.bat
  Or compile directly:
    g++ -std=c++11 -Wall -Wextra -pedantic CPP\backend.cpp -o parking_server.exe -lws2_32

STEP 4 - Start the server
- Run:
    .\run_windows.bat
  Keep this terminal open while using the website.

STEP 5 - Open the website
- Open this address in your browser:
    http://127.0.0.1:8765
- Do not double-click index.html and do not use Live Server. The page
  must be served by the C++ backend so its /api routes work.
- Stop the server with Ctrl+C.

============================================================
6. PHASE 2 MODULE CONTINUITY
============================================================

The combined backend retains the four Phase 2 modules:
1. Vehicle & Priority.
2. Parking Slot & Block.
3. Fine, Alert & Report.
4. Reservation & Resolver.

The Phase 2 proposal listed BFS/DFS graph search, a hash table and file
handling as planned improvements. These have been incorporated into the
Phase 3 code. The C++ file remains integrated as one backend source file
so the team can build and run the project without linking several
separate member source files.

============================================================
7. IMPORTANT TESTING NOTE
============================================================

The C++ backend must be compiled and run on the target Windows computer
to confirm the local MSYS2 setup. If the build fails, copy the complete
error from the VS Code terminal so the exact issue can be fixed.
