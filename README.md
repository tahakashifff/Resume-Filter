# Resume Filter — Project Setup & Contributor Guide

This README helps your group members clone, build, run, and contribute to the Resume Filter (console-first) project on their own machines using GitHub and Visual Studio. It also includes notes about the later Qt6 GUI integration.

---

## Table of Contents
- Project overview
- Prerequisites
- Clone the repository
- Build with Visual Studio (console version)
- Run the program
- Sample data (quick copy-paste)
- Common issues & troubleshooting
- Contributing & GitHub workflow
- Adding the project to your GitHub account (if you fork/clone)
- Next steps: Qt6 GUI integration
- Contact / reporting issues

---

## Project overview
Resume Filter is a simple C++ console application that:
- Reads multiple resume `.txt` files from a folder.
- Reads one job description `.txt` file.
- Parses resume fields (name, skills, GPA, experience, certifications).
- Scores each candidate against the job criteria with static weights.
- Assigns a letter grade (A/B/C/D).
- Prints results to console and writes `selected_candidates_report.txt`.

The code is intentionally kept simple (structs + functions, minimal classes) and uses C++17 features (notably `std::filesystem`).

---

## Prerequisites
Make sure you have the following installed:

- Git (for cloning and pushing):
  - Windows: https://git-scm.com/download/win
- Visual Studio 2019 / 2022 (recommended) with:
  - Desktop development with C++
  - MSVC toolset
  - C++ CMake tools (optional)
- C++ language standard set to C++17 (project property)
- (Later, for GUI) Qt 6 and the "Qt Visual Studio Tools" plugin or use Qt Creator

---

## Clone the repository (from GitHub)
1. Open a terminal (Git Bash, PowerShell, Command Prompt).
2. Clone the repo:
   - If your team repo URL is `https://github.com/owner/repo.git`:
     - git clone https://github.com/owner/repo.git
   - Or fork the repo on GitHub and clone your fork:
     - git clone https://github.com/<your-username>/repo.git
3. Enter the project folder:
   - cd repo

If you prefer a GUI, you can use GitHub Desktop to clone the repository.

---

## Build with Visual Studio (console version)
1. Open Visual Studio.
2. Choose "Open a local folder" and select the repository folder, or create a new Visual Studio Console Project and add the existing files:
   - Add all .cpp and .h files: main.cpp, utils.cpp/h, parser.cpp/h, scorer.cpp/h, report.cpp/h.
3. Set C++ standard:
   - Right-click project -> Properties -> C/C++ -> Language -> C++ Language Standard -> set to `ISO C++17` (or later).
4. Set Debugging Working Directory (optional, for convenient runtime paths):
   - Right-click project -> Properties -> Debugging -> Working Directory -> set to the folder where you'll place resume files (e.g., `D:\CP PROJECT\resumes`) or `$(ProjectDir)`.
5. Build the project:
   - Build -> Build Solution (Ctrl+Shift+B)
6. If build fails on `std::filesystem`, confirm you are using C++17 and an up-to-date MSVC toolset.

---

## Run the program
1. Start the program (recommended: Start Without Debugging — Ctrl+F5) so the console remains open.
2. The program prompts:
   - Enter path to folder containing resume .txt files:
   - Enter path to job description file (.txt):
3. Provide full paths (e.g., `D:\CP PROJECT\resumes` and `D:\CP PROJECT\resumes\Job\job.txt`) or relative paths if working directory is set.
4. The program prints results to console and generates `selected_candidates_report.txt` in the working directory.

---

## Sample data (copy-paste)
Create a folder (for example `D:\CP PROJECT\resumes`) and add these files for quick testing.

resume1.txt:
```
Name: Alice Johnson
Education: B.Sc. Computer Science, University of Example
GPA: 3.8
Skills: C++, Python, SQL, Git, Linux
Experience: 2 years at ExampleCorp
Certifications: AWS Certified Solutions Architect
```

resume2.txt:
```
John Smith
Education: BS Information Technology
GPA: 3.1
Technical Skills: Java, JavaScript, HTML, CSS, SQL
Experience: 4 years at WebCo; 1 year internship at StartUp
Certifications: Oracle Certified Professional
```

resume3.txt:
```
Name: Maria Gomez
Education: B.Eng. Software Engineering
CGPA: 9.0/10
Skills: cpp, algorithms, data structures, machine learning, python
Experience: 1 year at DataInc
Certificates: None
```

job.txt (put in a subfolder like `Job` or same folder):
```
Job Title: Software Developer - Algorithms Focus
Required Skills: C++, algorithms, data structures
Preferred Skills: Python, SQL
Min GPA: 3.2
Min Experience: 2
Required Certifications: AWS Certified Solutions Architect
Description: Looking for strong fundamentals in algorithms and C++.
```

---

## Common issues & troubleshooting
- No resumes found:
  - Ensure the folder path is correct and the files have `.txt` extension.
  - Program checks for `.txt` extension exactly.
- Build error with `std::filesystem`:
  - Make sure your project uses C++17 and an up-to-date Visual Studio (2019/2022).
- Regex failures or strange build errors:
  - The parser has been made robust and avoids complex regex syntax; if you still see issues, ensure files are ASCII and there are no stray characters in source files.
- Wrong parsing of a resume:
  - Resume formats vary. If the parser misses fields, paste the resume text into a message and we can refine parsing rules.
- Program crashes or unexpected exit:
  - Run in Visual Studio with the debugger or start without debugging (Ctrl+F5) to see console messages. Share the console output if needed.

---

## Contributing & GitHub workflow (recommended for group)
Follow this simple Git workflow to contribute safely:

1. Create a branch for your work:
   - git checkout -b feature/your-name-description
2. Make changes and test locally.
3. Stage and commit:
   - git add .
   - git commit -m "Brief: what you changed"
4. Push your branch to GitHub:
   - git push -u origin feature/your-name-description
5. Open a Pull Request (PR) on GitHub to merge into `main` or `develop`.
6. Request team members to review the PR; once approved, merge.
7. Regularly pull from main to keep your branch up-to-date:
   - git checkout main
   - git pull
   - git checkout feature/...
   - git merge main

If your teammates forked the repo:
- They should fork on GitHub, clone their fork locally, create a branch, work, push to their fork, then open a PR to the original repo.

---

## Adding the project to GitHub (if starting locally)
If someone created the project locally and wants to add it to GitHub:
1. Create a new repository on GitHub (do **not** initialize with README/license).
2. In local project root:
   - git init
   - git add .
   - git commit -m "Initial commit"
   - git branch -M main
   - git remote add origin https://github.com/<username>/<repo>.git
   - git push -u origin main

Share the repo URL with teammates so they can clone it.

---


## Contact / reporting issues
- If you find a bug or want to request changes, open a GitHub Issue in the repo with:
  - Title: short summary
  - Body: steps to reproduce, console output, sample resume file (if applicable)

---

Thanks — follow the steps above to get the project running on your machine. If any teammate runs into errors, copy-paste the console output or Visual Studio build errors here and we'll fix them quickly. If you want, I can also prepare a short demo script and slides for your semester presentation.
