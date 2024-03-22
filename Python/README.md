# Acoustics Python Code
## Description
The Python code developed in this directory will provide reference
implementations of the algorithms used on the embedded-platform.
This helps with analyzing their behavior and detect faulty implementation
on the embedded device.

## Setup Python Environment
For versioning of the imported packages inside the code a virtual environment
shall be used.
So while being in the `./Resource/PYTHON/` directory, call your global
Python 3 interpreter with following arguments:
```
    python -m venv venv
```
or possibly
```
    python3 -m venv venv
```
depending on OS and/or OS distribution.
This will create a folder called venv including a project specific Python
interpreter and providing a folder to install all required packages.

You can double check the interpreter version with:
```
    python --version
```
It should say Python 3.6 or higher.

Then activate the environment with:
```
    source ./venv/bin/activate
```
in Linux CLI or
```
    .\venv\Scripts\activate.bat
```
for Windows cmd.

After having the virtual environment activated you can install all the
required packages with:
```
    pip install -r requirements.txt
```

## Add requirements
To add requirements use:
```
    pip install DESIRED_PACKAGE
```
and then overwrite the requirements.txt with:
```
    pip freeze > requirements.txt
```
After that you can commit the `requirements.txt` file with the updated required
packages.

## Code formatting
One of the requirements is the package `black`, which is a code formatter.
The documenation is [available online](https://black.readthedocs.io/en/stable/index.html).
So before commiting new code run the formatter on your changed files to keep
the formatting consistent:
```
    python -m black file0.py file1.py ... fileN.py
```

