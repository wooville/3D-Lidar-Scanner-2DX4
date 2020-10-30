#!C:\Miscellaneous\Projects\2DX4\Final\venv\Scripts\python.exe
# EASY-INSTALL-ENTRY-SCRIPT: 'numpy-stl==2.11.2','console_scripts','stl'
__requires__ = 'numpy-stl==2.11.2'
import re
import sys
from pkg_resources import load_entry_point

if __name__ == '__main__':
    sys.argv[0] = re.sub(r'(-script\.pyw?|\.exe)?$', '', sys.argv[0])
    sys.exit(
        load_entry_point('numpy-stl==2.11.2', 'console_scripts', 'stl')()
    )
